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

#include <string_view>

#include "unicode/utypes.h"
#include "unicode/appendable.h"
#include "unicode/putil.h"
#include "cstring.h"
#include "cmemory.h"
#include "unicode/ustring.h"
#include "unicode/unistr.h"
#include "unicode/utf.h"
#include "unicode/utf16.h"
#include "uelement.h"
#include "ustr_imp.h"
#include "umutex.h"
#include "uassert.h"

#གལ་སྲིད 0

#include <iostream>
using namespace std;

//DEBUGGING
void
print(const UnicodeString& s,
      const var_na *name)
{
  char16_t c;
  cout << name << ":|";
  དོན་ལུ(ཧྲིལ i = 0; i < s.length(); ++i) {
    c = s[i];
    གལ་སྲིད(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    དེ་མིན
      cout << (var_na) s[i];
  }
  cout << '|' << endl;
}

void
print(const char16_t *s,
      int32_t len,
      const var_na *name)
{
  char16_t c;
  cout << name << ":|";
  དོན་ལུ(ཧྲིལ i = 0; i < len; ++i) {
    c = s[i];
    གལ་སྲིད(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    དེ་མིན
      cout << (var_na) s[i];
  }
  cout << '|' << endl;
}
// END DEBUGGING
#endif

// Local function definitions for now

// need to copy areas that may overlap
static
anta_hstha void
us_arrayCopy(const char16_t *src, int32_t srcStart,
         char16_t *dst, int32_t dstStart, int32_t count)
{
  གལ་སྲིད(count>0) {
    uprv_memmove(dst+dstStart, src+srcStart, (size_t)count*parimaa_na(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
static char16_t U_CALLCONV
UnicodeString_charAt(int32_t offset, void *context) {
    ལོག ((icu::UnicodeString*) context)->charAt(offset);
}
U_CDECL_END

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
operator+ (const UnicodeString &s1, const UnicodeString &s2) {
  int32_t sumLengths;
  གལ་སྲིད (uprv_add32_overflow(s1.length(), s2.length(), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    ལོག bogus;
  }
  གལ་སྲིད (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  ལོག UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_internalConcat(const UnicodeString &s1, std::u16string_view s2) {
  int32_t sumLengths;
  གལ་སྲིད (s2.length() > INT32_MAX ||
      uprv_add32_overflow(s1.length(), static_cast<int32_t>(s2.length()), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    ལོག bogus;
  }
  གལ་སྲིད (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  ལོག UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

void
UnicodeString::addRef() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  ལོག umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::refCount() const {
  ལོག umtx_loadAcquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

void
UnicodeString::releaseArray() {
  གལ་སྲིད((fUnion.fFields.fLengthAndFlags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.fLengthAndFlags = 0;
  གལ་སྲིད (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } དེ་མིན གལ་སྲིད(c <= 0xffff) {
    int32_t length = count;
    གལ་སྲིད(capacity < length) {
      capacity = length;
    }
    གལ་སྲིད(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t unit = static_cast<char16_t>(c);
      དོན་ལུ(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      setLength(length);
    }
  } དེ་མིན {  // supplementary code point, write surrogate pairs
    གལ་སྲིད(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      ལོག;
    }
    int32_t length = count * 2;
    གལ་སྲིད(capacity < length) {
      capacity = length;
    }
    གལ་སྲིད(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      དོན་ལུ(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      setLength(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.fLengthAndFlags = kLength1 | kShortString;
  fUnion.fStackFields.fBuffer[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  int32_t i = 0;
  UBool isError = false;
  U16_APPEND(fUnion.fStackFields.fBuffer, i, US_STACKBUF_SIZE, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  གལ་སྲིད(!isError) {
    setShortLength(i);
  }
}

UnicodeString::UnicodeString(const char16_t *text,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  doAppend(text, 0, textLength);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  const char16_t *text = textPtr;
  གལ་སྲིད(text == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } དེ་མིན གལ་སྲིད(textLength < -1 ||
            (textLength == -1 && !isTerminated) ||
            (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
  } དེ་མིན {
    གལ་སྲིད(textLength == -1) {
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
  གལ་སྲིད(buff == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } དེ་མིན གལ་སྲིད(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
  } དེ་མིན {
    གལ་སྲིད(buffLength == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      const char16_t *p = buff, *limit = buff + buffCapacity;
      སྐབས(p != limit && *p != 0) {
        ++p;
      }
      buffLength = static_cast<int32_t>(p - buff);
    }
    setArray(buff, buffLength, buffCapacity);
  }
}

UnicodeString::UnicodeString(const var_na *src, int32_t length, EInvariant) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  གལ་སྲིད(src==nullptr) {
    // treat as an empty string
  } དེ་མིན {
    གལ་སྲིད(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    གལ་སྲིད(cloneArrayIfNeeded(length, length, false)) {
      u_charsToUChars(src, getArrayStart(), length);
      setLength(length);
    } དེ་མིན {
      setToBogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view text U_LIFETIME_BOUND) {
  UnicodeString result;
  གལ་སྲིད (text.length() <= INT32_MAX) {
    result.setTo(false, text.data(), static_cast<int32_t>(text.length()));
  } དེ་མིན {
    result.setToBogus();
  }
  ལོག result;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(const UnicodeString& text U_LIFETIME_BOUND) {
  UnicodeString result;
  གལ་སྲིད (text.isBogus()) {
    result.setToBogus();
  } དེ་མིན {
    result.setTo(false, text.getBuffer(), text.length());
  }
  ལོག result;
}

#གལ་སྲིད U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(const var_na *codepageData) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  གལ་སྲིད (codepageData != nullptr) {
    setToUTF8(codepageData);
  }
}

UnicodeString::UnicodeString(const var_na *codepageData, int32_t dataLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  // if there's nothing to convert, do nothing
  གལ་སྲིད (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
    ལོག;
  }
  གལ་སྲིད(dataLength == -1) {
    dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
  }
  setToUTF8(StringPiece(codepageData, dataLength));
}

// else see unistr_cnv.cpp
#endif

UnicodeString::UnicodeString(const UnicodeString& that) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, true);
}

UnicodeString::UnicodeString(const UnicodeString& that,
                             int32_t srcStart) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart);
}

UnicodeString::UnicodeString(const UnicodeString& that,
                             int32_t srcStart,
                             int32_t srcLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart, srcLength);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() const {
  ལོག nullptr;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() const {
  LocalPointer<UnicodeString> clonedString(new UnicodeString(*this));
  ལོག clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : nullptr;
}

//========================================
// array allocation
//========================================

namespace {

const int32_t kGrowSize = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
const int32_t kMaxCapacity = 0x7ffffff5;

int32_t getGrowCapacity(int32_t newLength) {
  int32_t growSize = (newLength >> 2) + kGrowSize;
  གལ་སྲིད(growSize <= (kMaxCapacity - newLength)) {
    ལོག newLength + growSize;
  } དེ་མིན {
    ལོག kMaxCapacity;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  གལ་སྲིད(capacity <= US_STACKBUF_SIZE) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ལོག true;
  }
  གལ་སྲིད(capacity <= kMaxCapacity) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t numBytes = parimaa_na(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    numBytes = (numBytes + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(numBytes));
    གལ་སྲིད(array != nullptr) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      numBytes -= parimaa_na(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.fCapacity = static_cast<int32_t>(numBytes / U_SIZEOF_UCHAR);
      fUnion.fFields.fLengthAndFlags = kLongString;
      ལོག true;
    }
  }
  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
  ལོག false;
}

//========================================
// Destructor
//========================================

#ifdef UNISTR_COUNT_FINAL_STRING_LENGTHS
static u_atomic_int32_t finalLengthCounts[0x400];  // UnicodeString::kMaxShortLength+1
static u_atomic_int32_t beyondCount(0);

U_CAPI void unistr_printLengths() {
  int32_t i;
  དོན་ལུ(i = 0; i <= 59; ++i) {
    printf("%2d,  %9d\n", i, (int32_t)finalLengthCounts[i]);
  }
  int32_t beyond = beyondCount;
  དོན་ལུ(; i < UPRV_LENGTHOF(finalLengthCounts); ++i) {
    beyond += finalLengthCounts[i];
  }
  printf(">59, %9d\n", beyond);
}
#endif

UnicodeString::~UnicodeString()
{
#ifdef UNISTR_COUNT_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  གལ་སྲིད((fUnion.fFields.fLengthAndFlags&(kOpenGetBuffer|kReadonlyAlias|kWritableAlias)) == 0) {
    གལ་སྲིད(hasShortLength()) {
      umtx_atomic_inc(finalLengthCounts + getShortLength());
    } དེ་མིན {
      umtx_atomic_inc(&beyondCount);
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
  ལོག result;
}

UnicodeString UnicodeString::fromUTF32(const UChar32 *utf32, int32_t length) {
  UnicodeString result;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  གལ་སྲིད(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } དེ་མིན {
    capacity = length + (length >> 4) + 4;
  }
  do {
    char16_t *utf16 = result.getBuffer(capacity);
    int32_t length16;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, result.getCapacity(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
    result.releaseBuffer(length16);
    གལ་སྲིད(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      continue;
    } དེ་མིན གལ་སྲིད(U_FAILURE(errorCode)) {
      result.setToBogus();
    }
    bha_nga;
  } སྐབས(true);
  ལོག result;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::operator=(const UnicodeString& src) U_LIFETIME_BOUND {
  ལོག copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(const UnicodeString& src) U_LIFETIME_BOUND {
  ལོག copyFrom(src, true);
}

UnicodeString&
UnicodeString::copyFrom(const UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  གལ་སྲིད(this == &src) {
    ལོག *this;
  }

  // is the right side bogus?
  གལ་སྲིད(src.isBogus()) {
    setToBogus();
    ལོག *this;
  }

  // delete the current contents
  releaseArray();

  གལ་སྲིད(src.isEmpty()) {
    // empty string - use the stack buffer
    setToEmpty();
    ལོག *this;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  switch(src.fUnion.fFields.fLengthAndFlags & kAllStorageFlags) {
  pak_sa kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.fStackFields.fBuffer, src.fUnion.fStackFields.fBuffer,
                getShortLength() * U_SIZEOF_UCHAR);
    bha_nga;
  pak_sa kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).addRef();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    གལ་སྲིད(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    bha_nga;
  pak_sa kReadonlyAlias:
    གལ་སྲིད(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
      གལ་སྲིད(!hasShortLength()) {
        fUnion.fFields.fLength = src.fUnion.fFields.fLength;
      }
      bha_nga;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  pak_sa kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t srcLength = src.length();
    གལ་སྲིད(allocate(srcLength)) {
      u_memcpy(getArrayStart(), src.getArrayStart(), srcLength);
      setLength(srcLength);
      bha_nga;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  default:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.fLengthAndFlags = kIsBogus;
    fUnion.fFields.fArray = nullptr;
    fUnion.fFields.fCapacity = 0;
    bha_nga;
  }

  ལོག *this;
}

UnicodeString& UnicodeString::operator=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, true);
  ལོག *this;
}

// Same as move assignment except without memory management.
void UnicodeString::copyFieldsFrom(UnicodeString &src, UBool setSrcToBogus) noexcept {
  int16_t lengthAndFlags = fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  གལ་སྲིད(lengthAndFlags & kUsingStackBuffer) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    གལ་སྲིད(this != &src) {
      uprv_memcpy(fUnion.fStackFields.fBuffer, src.fUnion.fStackFields.fBuffer,
                  getShortLength() * U_SIZEOF_UCHAR);
    }
  } དེ་མིན {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    གལ་སྲིད(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    གལ་སྲིད(setSrcToBogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.fLengthAndFlags = kIsBogus;
      src.fUnion.fFields.fArray = nullptr;
      src.fUnion.fFields.fCapacity = 0;
    }
  }
}

void UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString temp;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  temp.copyFieldsFrom(*this, false);
  this->copyFieldsFrom(other, false);
  other.copyFieldsFrom(temp, false);
  // Set temp to an empty string so that other's memory is not released twice.
  temp.fUnion.fFields.fLengthAndFlags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() const {
    UnicodeString result(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    གལ་སྲིད (result.isBogus()) {
        ལོག result;
    }
    const char16_t *array = getBuffer();
    int32_t len = length();
    int32_t prev = 0;
    དོན་ལུ (int32_t i=0;;) {
        གལ་སྲིད (i == len) {
            result.append(array, prev, len - prev);
            bha_nga;
        }
        གལ་སྲིད (array[i++] == 0x5C /*'\\'*/) {
            result.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            གལ་སྲིད (c < 0) {
                result.remove(); // return empty string
                bha_nga; // invalid escape sequence
            }
            result.append(c);
            prev = i;
        }
    }
    ལོག result;
}

UChar32 UnicodeString::unescapeAt(int32_t &offset) const {
    ལོག u_unescapeAt(UnicodeString_charAt, &offset, length(), (void*)this);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(const char16_t *text, int32_t len) const {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  ལོག uprv_memcmp(getArrayStart(), text, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t start,
              int32_t length,
              const char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) const
{
  // compare illegal string values
  གལ་སྲིད(isBogus()) {
    ལོག false;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  གལ་སྲིད(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ལོག length == 0 ? true : false;
  }

  // get the correct pointer
  const char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  // get the srcLength if necessary
  གལ་སྲིད(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  གལ་སྲིད (length != srcLength) {
    ལོག false;
  }

  གལ་སྲིད(length == 0 || chars == srcChars) {
    ལོག true;
  }

  ལོག u_memcmp(chars, srcChars, srcLength) == 0;
}

int8_t
UnicodeString::doCompare( int32_t start,
              int32_t length,
              const char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) const
{
  // compare illegal string values
  གལ་སྲིད(isBogus()) {
    ལོག -1;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  གལ་སྲིད(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ལོག length == 0 ? 0 : 1;
  }

  // get the correct pointer
  const char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  int32_t minLength;
  int8_t lengthResult;

  // get the srcLength if necessary
  གལ་སྲིད(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  // are we comparing different lengths?
  གལ་སྲིད(length != srcLength) {
    གལ་སྲིད(length < srcLength) {
      minLength = length;
      lengthResult = -1;
    } དེ་མིན {
      minLength = srcLength;
      lengthResult = 1;
    }
  } དེ་མིན {
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

  གལ་སྲིད(minLength > 0 && chars != srcChars) {
    int32_t result;

#   གལ་སྲིད U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      result = uprv_memcmp(chars, srcChars, minLength * parimaa_na(char16_t));
      གལ་སྲིད(result != 0) {
        ལོག (int8_t)(result >> 15 | 1);
      }
#   དེ་མིན
      // little-endian: compare char16_t units
      do {
        result = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        གལ་སྲིད(result != 0) {
          ལོག static_cast<int8_t>(result >> 15 | 1);
        }
      } སྐབས(--minLength > 0);
#   endif
  }
  ལོག lengthResult;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::doCompareCodePointOrder(int32_t start,
                                       int32_t length,
                                       const char16_t *srcChars,
                                       int32_t srcStart,
                                       int32_t srcLength) const
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  གལ་སྲིད(isBogus()) {
    ལོག -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  གལ་སྲིད(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=nullptr)?(srcChars + srcStart):nullptr, srcLength, false, true);
  /* translate the 32-bit result into an 8-bit one */
  གལ་སྲིད(diff!=0) {
    ལོག static_cast<int8_t>(diff >> 15 | 1);
  } དེ་མིན {
    ལོག 0;
  }
}

int32_t
UnicodeString::getLength() const {
    ལོག length();
}

char16_t
UnicodeString::getCharAt(int32_t offset) const {
  ལོག charAt(offset);
}

UChar32
UnicodeString::getChar32At(int32_t offset) const {
  ལོག char32At(offset);
}

UChar32
UnicodeString::char32At(int32_t offset) const
{
  int32_t len = length();
  གལ་སྲིད (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    const char16_t *array = getArrayStart();
    UChar32 c;
    U16_GET(array, 0, offset, len, c);
    ལོག c;
  } དེ་མིན {
    ལོག kInvalidUChar;
  }
}

int32_t
UnicodeString::getChar32Start(int32_t offset) const {
  གལ་སྲིད (static_cast<uint32_t>(offset) < static_cast<uint32_t>(length())) {
    const char16_t *array = getArrayStart();
    U16_SET_CP_START(array, 0, offset);
    ལོག offset;
  } དེ་མིན {
    ལོག 0;
  }
}

int32_t
UnicodeString::getChar32Limit(int32_t offset) const {
  int32_t len = length();
  གལ་སྲིད (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    const char16_t *array = getArrayStart();
    U16_SET_CP_LIMIT(array, 0, offset, len);
    ལོག offset;
  } དེ་མིན {
    ལོག len;
  }
}

int32_t
UnicodeString::countChar32(int32_t start, int32_t length) const {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  ལོག u_countChar32(getArrayStart()+start, length);
}

UBool
UnicodeString::hasMoreChar32Than(int32_t start, int32_t length, int32_t number) const {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  ལོག u_strHasMoreChar32Than(getArrayStart()+start, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t index, int32_t delta) const {
  // pin index
  int32_t len = length();
  གལ་སྲིད(index<0) {
    index=0;
  } དེ་མིན གལ་སྲིད(index>len) {
    index=len;
  }

  const char16_t *array = getArrayStart();
  གལ་སྲིད(delta>0) {
    U16_FWD_N(array, index, len, delta);
  } དེ་མིན {
    U16_BACK_N(array, 0, index, -delta);
  }

  ལོག index;
}

void
UnicodeString::doExtract(int32_t start,
             int32_t length,
             char16_t *dst,
             int32_t dstStart) const
{
  // pin indices to legal values
  pinIndices(start, length);

  // do not copy anything if we alias dst itself
  const char16_t *array = getArrayStart();
  གལ་སྲིད(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t destCapacity,
                       UErrorCode &errorCode) const {
  int32_t len = length();
  གལ་སྲིད(U_SUCCESS(errorCode)) {
    གལ་སྲིད (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
      errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } དེ་མིན {
      const char16_t *array = getArrayStart();
      གལ་སྲིད(len>0 && len<=destCapacity && array!=dest) {
        u_memcpy(dest, array, len);
      }
      ལོག u_terminateUChars(dest, destCapacity, len, &errorCode);
    }
  }

  ལོག len;
}

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       var_na *target,
                       int32_t targetCapacity,
                       ga_nanaa EInvariant) const
{
  // if the arguments are illegal, then do nothing
  གལ་སྲིད(targetCapacity < 0 || (targetCapacity > 0 && target == nullptr)) {
    ལོག 0;
  }

  // pin the indices to legal values
  pinIndices(start, length);

  གལ་སྲིད(length <= targetCapacity) {
    u_UCharsToChars(getArrayStart() + start, target, length);
  }
  UErrorCode status = U_ZERO_ERROR;
  ལོག u_terminateChars(target, targetCapacity, length, &status);
}

UnicodeString
UnicodeString::tempSubString(int32_t start, int32_t len) const U_LIFETIME_BOUND {
  pinIndices(start, len);
  const char16_t *array = getBuffer();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  གལ་སྲིད(array==nullptr) {
    array=fUnion.fStackFields.fBuffer;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  ལོག UnicodeString(false, array + start, len);
}

int32_t
UnicodeString::toUTF8(int32_t start, int32_t len,
                      var_na *target, int32_t capacity) const {
  pinIndices(start, len);
  int32_t length8;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strToUTF8WithSub(target, capacity, &length8,
                     getBuffer() + start, len,
                     0xFFFD,  // Standard substitution character.
                     nullptr,    // Don't care about number of substitutions.
                     &errorCode);
  ལོག length8;
}

#གལ་སྲིད U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start, int32_t len,
                       var_na *target, uint32_t dstSize) const {
  // if the arguments are illegal, then do nothing
  གལ་སྲིད (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
    ལོག 0;
  }
  ལོག toUTF8(start, len, target, dstSize <= 0x7fffffff ? static_cast<int32_t>(dstSize) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#endif

void 
UnicodeString::extractBetween(int32_t start,
                  int32_t limit,
                  UnicodeString& target) const {
  pinIndex(start);
  pinIndex(limit);
  doExtract(start, limit - start, target);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
void
UnicodeString::toUTF8(ByteSink &sink) const {
  int32_t length16 = length();
  གལ་སྲིད(length16 != 0) {
    var_na stackBuffer[1024];
    int32_t capacity = static_cast<int32_t>(parimaa_na(stackBuffer));
    UBool utf8IsOwned = false;
    var_na *utf8 = sink.GetAppendBuffer(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      stackBuffer, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8WithSub(utf8, capacity, &length8,
                       getBuffer(), length16,
                       0xFFFD,  // Standard substitution character.
                       nullptr,    // Don't care about number of substitutions.
                       &errorCode);
    གལ་སྲིད(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      utf8 = static_cast<var_na*>(uprv_malloc(length8));
      གལ་སྲིད(utf8 != nullptr) {
        utf8IsOwned = true;
        errorCode = U_ZERO_ERROR;
        u_strToUTF8WithSub(utf8, length8, &length8,
                           getBuffer(), length16,
                           0xFFFD,  // Standard substitution character.
                           nullptr,    // Don't care about number of substitutions.
                           &errorCode);
      } དེ་མིན {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
      }
    }
    གལ་སྲིད(U_SUCCESS(errorCode)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    གལ་སྲིད(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::toUTF32(UChar32 *utf32, int32_t capacity, UErrorCode &errorCode) const {
  int32_t length32=0;
  གལ་སྲིད(U_SUCCESS(errorCode)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_strToUTF32WithSub(utf32, capacity, &length32,
        getBuffer(), length(),
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
  }
  ལོག length32;
}

int32_t 
UnicodeString::indexOf(const char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) const
{
  གལ་སྲིད (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    ལོག -1;
  }

  // UnicodeString does not find empty substrings
  གལ་སྲིད(srcLength < 0 && srcChars[srcStart] == 0) {
    ལོག -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  const char16_t *array = getArrayStart();
  const char16_t *match = u_strFindFirst(array + start, length, srcChars + srcStart, srcLength);
  གལ་སྲིད(match == nullptr) {
    ལོག -1;
  } དེ་མིན {
    ལོག static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(char16_t c,
             int32_t start,
             int32_t length) const
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  const char16_t *array = getArrayStart();
  const char16_t *match = u_memchr(array + start, c, length);
  གལ་སྲིད(match == nullptr) {
    ལོག -1;
  } དེ་མིན {
    ལོག static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(UChar32 c,
                         int32_t start,
                         int32_t length) const {
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  const char16_t *array = getArrayStart();
  const char16_t *match = u_memchr32(array + start, c, length);
  གལ་སྲིད(match == nullptr) {
    ལོག -1;
  } དེ་མིན {
    ལོག static_cast<int32_t>(match - array);
  }
}

int32_t 
UnicodeString::lastIndexOf(const char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) const
{
  གལ་སྲིད (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    ལོག -1;
  }

  // UnicodeString does not find empty substrings
  གལ་སྲིད(srcLength < 0 && srcChars[srcStart] == 0) {
    ལོག -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the last occurrence of the substring
  const char16_t *array = getArrayStart();
  const char16_t *match = u_strFindLast(array + start, length, srcChars + srcStart, srcLength);
  གལ་སྲིད(match == nullptr) {
    ལོག -1;
  } དེ་མིན {
    ལོག static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(char16_t c,
                 int32_t start,
                 int32_t length) const
{
  གལ་སྲིད(isBogus()) {
    ལོག -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  const char16_t *array = getArrayStart();
  const char16_t *match = u_memrchr(array + start, c, length);
  གལ་སྲིད(match == nullptr) {
    ལོག -1;
  } དེ་མིན {
    ལོག static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(UChar32 c,
                             int32_t start,
                             int32_t length) const {
  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  const char16_t *array = getArrayStart();
  const char16_t *match = u_memrchr32(array + start, c, length);
  གལ་སྲིད(match == nullptr) {
    ལོག -1;
  } དེ་མིན {
    ལོག static_cast<int32_t>(match - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::findAndReplace(int32_t start,
                              int32_t length,
                              const UnicodeString& oldText,
                              int32_t oldStart,
                              int32_t oldLength,
                              const UnicodeString& newText,
                              int32_t newStart,
                              int32_t newLength) U_LIFETIME_BOUND
{
  གལ་སྲིད(isBogus() || oldText.isBogus() || newText.isBogus()) {
    ལོག *this;
  }

  pinIndices(start, length);
  oldText.pinIndices(oldStart, oldLength);
  newText.pinIndices(newStart, newLength);

  གལ་སྲིད(oldLength == 0) {
    ལོག *this;
  }

  སྐབས(length > 0 && length >= oldLength) {
    int32_t pos = indexOf(oldText, oldStart, oldLength, start, length);
    གལ་སྲིད(pos < 0) {
      // no more oldText's here: done
      bha_nga;
    } དེ་མིན {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, oldLength, newText, newStart, newLength);
      length -= pos + oldLength - start;
      start = pos + newLength;
    }
  }

  ལོག *this;
}


void
UnicodeString::setToBogus()
{
  releaseArray();

  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
}

// turn a bogus string into an empty one
void
UnicodeString::unBogus() {
  གལ་སྲིད(fUnion.fFields.fLengthAndFlags & kIsBogus) {
    setToEmpty();
  }
}

const char16_t*
UnicodeString::getTerminatedBuffer() U_LIFETIME_BOUND {
  གལ་སྲིད(!isWritable()) {
    ལོག nullptr;
  }
  char16_t *array = getArrayStart();
  int32_t len = length();
  གལ་སྲིད(len < getCapacity()) {
    གལ་སྲིད(fUnion.fFields.fLengthAndFlags & kBufferIsReadonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      གལ་སྲིད(array[len] == 0) {
        ལོག array;
      }
    } དེ་མིན གལ་སྲིད(((fUnion.fFields.fLengthAndFlags & kRefCounted) == 0 || refCount() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      ལོག array;
    }
  }
  གལ་སྲིད(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = getArrayStart();
    array[len] = 0;
    ལོག array;
  } དེ་མིན {
    ལོག nullptr;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(UBool isTerminated,
                     ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                     int32_t textLength) U_LIFETIME_BOUND
{
  གལ་སྲིད(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ལོག *this;
  }

  const char16_t *text = textPtr;
  གལ་སྲིད(text == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    ལོག *this;
  }

  གལ་སྲིད( textLength < -1 ||
      (textLength == -1 && !isTerminated) ||
      (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
    ལོག *this;
  }

  releaseArray();

  གལ་སྲིད(textLength == -1) {
    // text is terminated, or else it would have failed the above test
    textLength = u_strlen(text);
  }
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  setArray(const_cast<char16_t*>(text), textLength, isTerminated ? textLength + 1 : textLength);
  ལོག *this;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(char16_t* buffer,
                     int32_t buffLength U_LIFETIME_BOUND,
                     int32_t buffCapacity) U_LIFETIME_BOUND {
  གལ་སྲིད(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ལོག *this;
  }

  གལ་སྲིད(buffer == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    ལོག *this;
  }

  གལ་སྲིད(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
    ལོག *this;
  } དེ་མིན གལ་སྲིད(buffLength == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    const char16_t *p = buffer, *limit = buffer + buffCapacity;
    སྐབས(p != limit && *p != 0) {
      ++p;
    }
    buffLength = static_cast<int32_t>(p - buffer);
  }

  releaseArray();

  fUnion.fFields.fLengthAndFlags = kWritableAlias;
  setArray(buffer, buffLength, buffCapacity);
  ལོག *this;
}

UnicodeString& UnicodeString::setToUTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  གལ་སྲིད(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } དེ་མིན {
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
  གལ་སྲིད(U_FAILURE(errorCode)) {
    setToBogus();
  }
  ལོག *this;
}

UnicodeString&
UnicodeString::setCharAt(int32_t offset, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  གལ་སྲིད(cloneArrayIfNeeded() && len > 0) {
    གལ་སྲིད(offset < 0) {
      offset = 0;
    } དེ་མིན གལ་སྲིད(offset >= len) {
      offset = len - 1;
    }

    getArrayStart()[offset] = c;
  }
  ལོག *this;
}

UnicodeString&
UnicodeString::replace(int32_t start,
                       int32_t _length,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t count = 0;
  UBool isError = false;
  U16_APPEND(buffer, count, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  ལོག doReplace(start, _length, buffer, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t _length = 0;
  UBool isError = false;
  U16_APPEND(buffer, _length, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  ལོག isError ? *this : doAppend(buffer, 0, _length);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         const UnicodeString& src,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);

  // get the characters from src
  // and replace the range in ourselves with them
  ལོག doReplace(start, length, src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         const char16_t* srcChars,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  གལ་སྲིད(!isWritable()) {
    ལོག *this;
  }

  int32_t oldLength = this->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  གལ་སྲིད((fUnion.fFields.fLengthAndFlags&kBufferIsReadonly) && srcLength == 0) {
    གལ་སྲིད(start == 0) {
      // remove prefix by adjusting the array pointer
      pinIndex(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.fCapacity -= length;
      setLength(oldLength - length);
      ལོག *this;
    } དེ་མིན {
      pinIndex(start);
      གལ་སྲིད(length >= (oldLength - start)) {
        // remove suffix by reducing the length (like truncate())
        setLength(start);
        fUnion.fFields.fCapacity = start;  // not NUL-terminated any more
        ལོག *this;
      }
    }
  }

  གལ་སྲིད(start == oldLength) {
    ལོག doAppend(srcChars, srcStart, srcLength);
  }

  གལ་སྲིད (srcChars == nullptr) {
    srcLength = 0;
  } དེ་མིན {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    གལ་སྲིད (srcLength < 0) {
      // get the srcLength if necessary
      srcLength = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t newLength = oldLength - length;
  གལ་སྲིད(srcLength > (INT32_MAX - newLength)) {
    setToBogus();
    ལོག *this;
  }
  newLength += srcLength;

  // Check for insertion into ourself
  const char16_t *oldArray = getArrayStart();
  གལ་སྲིད (isBufferWritable() &&
      oldArray < srcChars + srcLength &&
      srcChars < oldArray + oldLength) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, srcLength);
    གལ་སྲིད (copy.isBogus()) {
      setToBogus();
      ལོག *this;
    }
    ལོག doReplace(start, length, copy.getArrayStart(), 0, srcLength);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t oldStackBuffer[US_STACKBUF_SIZE];
  གལ་སྲིད((fUnion.fFields.fLengthAndFlags&kUsingStackBuffer) && (newLength > US_STACKBUF_SIZE)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(oldStackBuffer, oldArray, oldLength);
    oldArray = oldStackBuffer;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *bufferToDelete = nullptr;
  གལ་སྲིད(!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength),
                         false, &bufferToDelete)
  ) {
    ལོག *this;
  }

  // now do the replace

  char16_t *newArray = getArrayStart();
  གལ་སྲིད(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + srcLength,
                 oldLength - (start + length));
  } དེ་མིན གལ་སྲིད(length != srcLength) {
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
  གལ་སྲིད (bufferToDelete) {
    uprv_free(bufferToDelete);
  }

  ལོག *this;
}

UnicodeString&
UnicodeString::doReplace(int32_t start, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  གལ་སྲིད (!isWritable()) {
    ལོག *this;
  }
  གལ་སྲིད (src.length() > INT32_MAX) {
    setToBogus();
    ལོག *this;
  }
  ལོག doReplace(start, length, src.data(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(const UnicodeString& src,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  གལ་སྲིད(srcLength == 0) {
    ལོག *this;
  }

  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);
  ལོག doAppend(src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doAppend(const char16_t* srcChars,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  གལ་སྲིད(!isWritable() || srcLength == 0 || srcChars == nullptr) {
    ལོག *this;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  གལ་སྲིད(srcLength < 0) {
    // get the srcLength if necessary
    གལ་སྲིད((srcLength = u_strlen(srcChars)) == 0) {
      ལོག *this;
    }
  }

  int32_t oldLength = length();
  int32_t newLength;

  གལ་སྲིད (srcLength <= getCapacity() - oldLength && isBufferWritable()) {
    newLength = oldLength + srcLength;
    // Faster than a memmove
    གལ་སྲིད (srcLength <= 4) {
      char16_t *arr = getArrayStart();
      arr[oldLength] = srcChars[0];
      གལ་སྲིད (srcLength > 1) arr[oldLength+1] = srcChars[1];
      གལ་སྲིད (srcLength > 2) arr[oldLength+2] = srcChars[2];
      གལ་སྲིད (srcLength > 3) arr[oldLength+3] = srcChars[3];
      setLength(newLength);
      ལོག *this;
    }
  } དེ་མིན {
    གལ་སྲིད (uprv_add32_overflow(oldLength, srcLength, &newLength)) {
      setToBogus();
      ལོག *this;
    }

    // Check for append onto ourself
    const char16_t* oldArray = getArrayStart();
    གལ་སྲིད (isBufferWritable() &&
        oldArray < srcChars + srcLength &&
        srcChars < oldArray + oldLength) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, srcLength);
      གལ་སྲིད (copy.isBogus()) {
        setToBogus();
        ལོག *this;
      }
      ལོག doAppend(copy.getArrayStart(), 0, srcLength);
    }

    // optimize append() onto a large-enough, owned string
    གལ་སྲིད (!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength))) {
      ལོག *this;
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
  གལ་སྲིད(srcChars != newArray + oldLength) {
    us_arrayCopy(srcChars, 0, newArray, oldLength, srcLength);
  }
  setLength(newLength);

  ལོག *this;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  གལ་སྲིད (!isWritable() || src.empty()) {
    ལོག *this;
  }
  གལ་སྲིད (src.length() > INT32_MAX) {
    setToBogus();
    ལོག *this;
  }
  ལོག doAppend(src.data(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
void
UnicodeString::handleReplaceBetween(int32_t start,
                                    int32_t limit,
                                    const UnicodeString& text) {
    replaceBetween(start, limit, text);
}

/**
 * Replaceable API
 */
void 
UnicodeString::copy(int32_t start, int32_t limit, int32_t dest) {
    གལ་སྲིད (limit <= start) {
        ལོག; // Nothing to do; avoid bogus malloc call
    }
    char16_t* text = static_cast<char16_t*>(uprv_malloc(parimaa_na(char16_t) * (limit - start)));
    // Check to make sure text is not null.
    གལ་སྲིད (text != nullptr) {
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
UBool Replaceable::hasMetaData() const {
    ལོག true;
}

/**
 * Replaceable API
 */
UBool UnicodeString::hasMetaData() const {
    ལོག false;
}

UnicodeString&
UnicodeString::doReverse(int32_t start, int32_t length) U_LIFETIME_BOUND {
  གལ་སྲིད(length <= 1 || !cloneArrayIfNeeded()) {
    ལོག *this;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  གལ་སྲིད(length <= 1) {  // pinIndices() might have shrunk the length
    ལོག *this;
  }

  char16_t *left = getArrayStart() + start;
  char16_t *right = left + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = false;

  // Before the loop we know left<right because length>=2.
  do {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *left));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left++ = *right));
    *right-- = swap;
  } སྐབས(left < right);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  གལ་སྲིད(hasSupplementary) {
    char16_t swap2;

    left = getArrayStart() + start;
    right = left + length - 1; // -1 so that we can look at *(left+1) if left<right
    སྐབས(left < right) {
      གལ་སྲིད(U16_IS_TRAIL(swap = *left) && U16_IS_LEAD(swap2 = *(left + 1))) {
        *left++ = swap2;
        *left++ = swap;
      } དེ་མིན {
        ++left;
      }
    }
  }

  ལོག *this;
}

UBool 
UnicodeString::padLeading(int32_t targetLength,
                          char16_t padChar)
{
  int32_t oldLength = length();
  གལ་སྲིད(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    ལོག false;
  } དེ་མིན {
    // move contents up by padding width
    char16_t *array = getArrayStart();
    int32_t start = targetLength - oldLength;
    us_arrayCopy(array, 0, array, start, oldLength);

    // fill in padding character
    སྐབས(--start >= 0) {
      array[start] = padChar;
    }
    setLength(targetLength);
    ལོག true;
  }
}

UBool 
UnicodeString::padTrailing(int32_t targetLength,
                           char16_t padChar)
{
  int32_t oldLength = length();
  གལ་སྲིད(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    ལོག false;
  } དེ་མིན {
    // fill in padding character
    char16_t *array = getArrayStart();
    int32_t length = targetLength;
    སྐབས(--length >= oldLength) {
      array[length] = padChar;
    }
    setLength(targetLength);
    ལོག true;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::doHashCode() const
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hashCode = ustr_hashUCharsN(getArrayStart(), length());
    གལ་སྲིད (hashCode == kInvalidHashCode) {
        hashCode = kEmptyHashCode;
    }
    ལོག hashCode;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::getBuffer(int32_t minCapacity) U_LIFETIME_BOUND {
  གལ་སྲིད(minCapacity>=-1 && cloneArrayIfNeeded(minCapacity)) {
    fUnion.fFields.fLengthAndFlags|=kOpenGetBuffer;
    setZeroLength();
    ལོག getArrayStart();
  } དེ་མིན {
    ལོག nullptr;
  }
}

void
UnicodeString::releaseBuffer(int32_t newLength) {
  གལ་སྲིད(fUnion.fFields.fLengthAndFlags&kOpenGetBuffer && newLength>=-1) {
    // set the new fLength
    int32_t capacity=getCapacity();
    གལ་སྲིད(newLength==-1) {
      // the new length is the string length, capped by fCapacity
      const char16_t *array=getArrayStart(), *p=array, *limit=array+capacity;
      སྐབས(p<limit && *p!=0) {
        ++p;
      }
      newLength = static_cast<int32_t>(p - array);
    } དེ་མིན གལ་སྲིད(newLength>capacity) {
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
  གལ་སྲིད(newCapacity == -1) {
    newCapacity = getCapacity();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  གལ་སྲིད(!isWritable()) {
    ལོག false;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  གལ་སྲིད(forceClone ||
     fUnion.fFields.fLengthAndFlags & kBufferIsReadonly ||
     (fUnion.fFields.fLengthAndFlags & kRefCounted && refCount() > 1) ||
     newCapacity > getCapacity()
  ) {
    // check growCapacity for default value and use of the stack buffer
    གལ་སྲིད(growCapacity < 0) {
      growCapacity = newCapacity;
    } དེ་མིན གལ་སྲིད(newCapacity <= US_STACKBUF_SIZE && growCapacity > US_STACKBUF_SIZE) {
      growCapacity = US_STACKBUF_SIZE;
    } དེ་མིན གལ་སྲིད(newCapacity > growCapacity) {
      setToBogus();
      ལོག false;  // bad inputs
    }
    གལ་སྲིད(growCapacity > kMaxCapacity) {
      setToBogus();
      ལོག false;
    }

    // save old values
    char16_t oldStackBuffer[US_STACKBUF_SIZE];
    char16_t *oldArray;
    int32_t oldLength = length();
    int16_t flags = fUnion.fFields.fLengthAndFlags;

    གལ་སྲིད(flags&kUsingStackBuffer) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      གལ་སྲིད(doCopyArray && growCapacity > US_STACKBUF_SIZE) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.fStackFields.fBuffer, 0, oldStackBuffer, 0, oldLength);
        oldArray = oldStackBuffer;
      } དེ་མིན {
        oldArray = nullptr; // no need to copy from the stack buffer to itself
      }
    } དེ་མིན {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=nullptr); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    གལ་སྲིད(allocate(growCapacity) ||
       (newCapacity < growCapacity && allocate(newCapacity))
    ) {
      གལ་སྲིད(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t minLength = oldLength;
        newCapacity = getCapacity();
        གལ་སྲིད(newCapacity < minLength) {
          minLength = newCapacity;
        }
        གལ་སྲིད(oldArray != nullptr) {
          us_arrayCopy(oldArray, 0, getArrayStart(), 0, minLength);
        }
        setLength(minLength);
      } དེ་མིན {
        setZeroLength();
      }

      // release the old array
      གལ་སྲིད(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* pRefCount = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        གལ་སྲིད(umtx_atomic_dec(pRefCount) == 0) {
          གལ་སྲིད (pBufferToDelete == nullptr) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((void *)pRefCount);
          } དེ་མིན {
            // the caller requested to delete it himself
            *pBufferToDelete = reinterpret_cast<int32_t*>(pRefCount);
          }
        }
      }
    } དེ་མིན {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      གལ་སྲིད(!(flags&kUsingStackBuffer)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.fLengthAndFlags = flags;
      setToBogus();
      ལོག false;
    }
  }
  ལོག true;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::appendCodeUnit(char16_t c) {
  ལོག str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::appendCodePoint(UChar32 c) {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t cLength = 0;
  UBool isError = false;
  U16_APPEND(buffer, cLength, U16_MAX_LENGTH, c, isError);
  ལོག !isError && str.doAppend(buffer, 0, cLength).isWritable();
}

UBool
UnicodeStringAppendable::appendString(const char16_t *s, int32_t length) {
  ལོག str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserveAppendCapacity(int32_t appendCapacity) {
  ལོག str.cloneArrayIfNeeded(str.length() + appendCapacity);
}

char16_t* UnicodeStringAppendable::getAppendBuffer(int32_t minCapacity,
                                                   int32_t desiredCapacityHint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratchCapacity,
                                                   int32_t* resultCapacity) U_LIFETIME_BOUND {
  གལ་སྲིད(minCapacity < 1 || scratchCapacity < minCapacity) {
    *resultCapacity = 0;
    ལོག nullptr;
  }
  int32_t oldLength = str.length();
  གལ་སྲིད(minCapacity <= (kMaxCapacity - oldLength) &&
      desiredCapacityHint <= (kMaxCapacity - oldLength) &&
      str.cloneArrayIfNeeded(oldLength + minCapacity, oldLength + desiredCapacityHint)) {
    *resultCapacity = str.getCapacity() - oldLength;
    ལོག str.getArrayStart() + oldLength;
  }
  *resultCapacity = scratchCapacity;
  ལོག scratch;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(const UElement key) {
    const UnicodeString *str = (const UnicodeString*) key.pointer;
    ལོག (str == nullptr) ? 0 : str->hashCode();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(const UElement key1, const UElement key2) {
    const UnicodeString *str1 = (const UnicodeString*) key1.pointer;
    const UnicodeString *str2 = (const UnicodeString*) key2.pointer;
    གལ་སྲིད (str1 == str2) {
        ལོག true;
    }
    གལ་སྲིད (str1 == nullptr || str2 == nullptr) {
        ལོག false;
    }
    ལོག *str1 == *str2;
}

#ifdef U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#གལ་སྲིད defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static void uprv_UnicodeStringDummy() {
    delete [] (new UnicodeString[2]);
}
#pragma GCC diagnostic pop
#endif
#endif
