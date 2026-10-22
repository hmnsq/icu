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

#અન્તર્ભાવ <string_view>

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/appendable.h"
#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/utf.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "uelement.h"
#અન્તર્ભાવ "ustr_imp.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "uassert.h"

#જો 0

#અન્તર્ભાવ <iostream>
using નામાકાશ std;

//DEBUGGING
શૂન્ય
print(અચલ UnicodeString& s,
      અચલ અક્ષર *name)
{
  char16_t c;
  cout << name << ":|";
  માટે(પૂર્ણાંક i = 0; i < s.length(); ++i) {
    c = s[i];
    જો(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    નહીંતર
      cout << (અક્ષર) s[i];
  }
  cout << '|' << endl;
}

શૂન્ય
print(અચલ char16_t *s,
      int32_t len,
      અચલ અક્ષર *name)
{
  char16_t c;
  cout << name << ":|";
  માટે(પૂર્ણાંક i = 0; i < len; ++i) {
    c = s[i];
    જો(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    નહીંતર
      cout << (અક્ષર) s[i];
  }
  cout << '|' << endl;
}
// END DEBUGGING
#endif

// Local function definitions for now

// need to copy areas that may overlap
સ્થિર
અંતઃરેખ શૂન્ય
us_arrayCopy(અચલ char16_t *src, int32_t srcStart,
         char16_t *dst, int32_t dstStart, int32_t count)
{
  જો(count>0) {
    uprv_memmove(dst+dstStart, src+srcStart, (size_t)count*કદ(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
સ્થિર char16_t U_CALLCONV
UnicodeString_charAt(int32_t offset, શૂન્ય *context) {
    પાછા ((icu::UnicodeString*) context)->charAt(offset);
}
U_CDECL_END

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
operator+ (અચલ UnicodeString &s1, અચલ UnicodeString &s2) {
  int32_t sumLengths;
  જો (uprv_add32_overflow(s1.length(), s2.length(), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    પાછા bogus;
  }
  જો (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  પાછા UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_internalConcat(અચલ UnicodeString &s1, std::u16string_view s2) {
  int32_t sumLengths;
  જો (s2.length() > INT32_MAX ||
      uprv_add32_overflow(s1.length(), static_cast<int32_t>(s2.length()), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    પાછા bogus;
  }
  જો (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  પાછા UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

શૂન્ય
UnicodeString::addRef() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  પાછા umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_ગણતરી() અચલ {
  પાછા umtx_loadAcquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

શૂન્ય
UnicodeString::releaseArray() {
  જો((fUnion.fFields.fLengthAndFlags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.fLengthAndFlags = 0;
  જો (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } નહીંતર જો(c <= 0xffff) {
    int32_t length = count;
    જો(capacity < length) {
      capacity = length;
    }
    જો(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t unit = static_cast<char16_t>(c);
      માટે(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      setLength(length);
    }
  } નહીંતર {  // supplementary code point, write surrogate pairs
    જો(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      પાછા;
    }
    int32_t length = count * 2;
    જો(capacity < length) {
      capacity = length;
    }
    જો(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      માટે(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      setLength(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.fLengthAndFlags = kLength1 | kShortString;
  fUnion.f_ઢગલો_Fields.fBuffer[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  int32_t i = 0;
  UBool isError = ખોટું;
  U16_APPEND(fUnion.f_ઢગલો_Fields.fBuffer, i, US_STACKBUF_SIZE, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  જો(!isError) {
    setShortLength(i);
  }
}

UnicodeString::UnicodeString(અચલ char16_t *text,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  doAppend(text, 0, textLength);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  અચલ char16_t *text = textPtr;
  જો(text == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } નહીંતર જો(textLength < -1 ||
            (textLength == -1 && !isTerminated) ||
            (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
  } નહીંતર {
    જો(textLength == -1) {
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
  જો(buff == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } નહીંતર જો(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
  } નહીંતર {
    જો(buffLength == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      અચલ char16_t *p = buff, *limit = buff + buffCapacity;
      જ્યારે(p != limit && *p != 0) {
        ++p;
      }
      buffLength = static_cast<int32_t>(p - buff);
    }
    setArray(buff, buffLength, buffCapacity);
  }
}

UnicodeString::UnicodeString(અચલ અક્ષર *src, int32_t length, EInvariant) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  જો(src==nullptr) {
    // treat as an empty string
  } નહીંતર {
    જો(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    જો(cloneArrayIfNeeded(length, length, ખોટું)) {
      u_charsToUChars(src, getArrayStart(), length);
      setLength(length);
    } નહીંતર {
      setToBogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view text U_LIFETIME_BOUND) {
  UnicodeString result;
  જો (text.length() <= INT32_MAX) {
    result.setTo(ખોટું, text.data(), static_cast<int32_t>(text.length()));
  } નહીંતર {
    result.setToBogus();
  }
  પાછા result;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(અચલ UnicodeString& text U_LIFETIME_BOUND) {
  UnicodeString result;
  જો (text.isBogus()) {
    result.setToBogus();
  } નહીંતર {
    result.setTo(ખોટું, text.getBuffer(), text.length());
  }
  પાછા result;
}

#જો U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(અચલ અક્ષર *codepageData) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  જો (codepageData != nullptr) {
    setToUTF8(codepageData);
  }
}

UnicodeString::UnicodeString(અચલ અક્ષર *codepageData, int32_t dataLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  // if there's nothing to convert, do nothing
  જો (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
    પાછા;
  }
  જો(dataLength == -1) {
    dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
  }
  setToUTF8(StringPiece(codepageData, dataLength));
}

// else see unistr_cnv.cpp
#endif

UnicodeString::UnicodeString(અચલ UnicodeString& that) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, સાચું);
}

UnicodeString::UnicodeString(અચલ UnicodeString& that,
                             int32_t srcStart) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart);
}

UnicodeString::UnicodeString(અચલ UnicodeString& that,
                             int32_t srcStart,
                             int32_t srcLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart, srcLength);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() અચલ {
  પાછા nullptr;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() અચલ {
  LocalPointer<UnicodeString> clonedString(નવું UnicodeString(*આ));
  પાછા clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : nullptr;
}

//========================================
// array allocation
//========================================

નામાકાશ {

અચલ int32_t kGrowSize = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
અચલ int32_t kMaxCapacity = 0x7ffffff5;

int32_t getGrowCapacity(int32_t newLength) {
  int32_t growSize = (newLength >> 2) + kGrowSize;
  જો(growSize <= (kMaxCapacity - newLength)) {
    પાછા newLength + growSize;
  } નહીંતર {
    પાછા kMaxCapacity;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  જો(capacity <= US_STACKBUF_SIZE) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    પાછા સાચું;
  }
  જો(capacity <= kMaxCapacity) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t અંક_Bytes = કદ(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    અંક_Bytes = (અંક_Bytes + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(અંક_Bytes));
    જો(array != nullptr) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      અંક_Bytes -= કદ(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.fCapacity = static_cast<int32_t>(અંક_Bytes / U_SIZEOF_UCHAR);
      fUnion.fFields.fLengthAndFlags = kLongString;
      પાછા સાચું;
    }
  }
  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
  પાછા ખોટું;
}

//========================================
// Destructor
//========================================

#ifdef UNISTR_ગણતરી_FINAL_STRING_LENGTHS
સ્થિર u_atomic_int32_t finalLengthCounts[0x400];  // UnicodeString::kMaxShortLength+1
સ્થિર u_atomic_int32_t beyond_ગણતરી(0);

U_CAPI શૂન્ય unistr_printLengths() {
  int32_t i;
  માટે(i = 0; i <= 59; ++i) {
    printf("%2d,  %9d\n", i, (int32_t)finalLengthCounts[i]);
  }
  int32_t beyond = beyond_ગણતરી;
  માટે(; i < UPRV_LENGTHOF(finalLengthCounts); ++i) {
    beyond += finalLengthCounts[i];
  }
  printf(">59, %9d\n", beyond);
}
#endif

UnicodeString::~UnicodeString()
{
#ifdef UNISTR_ગણતરી_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  જો((fUnion.fFields.fLengthAndFlags&(kOpenGetBuffer|kReadonlyAlias|kWritableAlias)) == 0) {
    જો(hasShortLength()) {
      umtx_atomic_inc(finalLengthCounts + getShortLength());
    } નહીંતર {
      umtx_atomic_inc(&beyond_ગણતરી);
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
  પાછા result;
}

UnicodeString UnicodeString::fromUTF32(અચલ UChar32 *utf32, int32_t length) {
  UnicodeString result;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  જો(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } નહીંતર {
    capacity = length + (length >> 4) + 4;
  }
  કરો {
    char16_t *utf16 = result.getBuffer(capacity);
    int32_t length16;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, result.getCapacity(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
    result.releaseBuffer(length16);
    જો(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      ચાલુ;
    } નહીંતર જો(U_FAILURE(errorCode)) {
      result.setToBogus();
    }
    તોડો;
  } જ્યારે(સાચું);
  પાછા result;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::operator=(અચલ UnicodeString& src) U_LIFETIME_BOUND {
  પાછા copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(અચલ UnicodeString& src) U_LIFETIME_BOUND {
  પાછા copyFrom(src, સાચું);
}

UnicodeString&
UnicodeString::copyFrom(અચલ UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  જો(આ == &src) {
    પાછા *આ;
  }

  // is the right side bogus?
  જો(src.isBogus()) {
    setToBogus();
    પાછા *આ;
  }

  // delete the current contents
  releaseArray();

  જો(src.isEmpty()) {
    // empty string - use the stack buffer
    setToEmpty();
    પાછા *આ;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  પસંદગી(src.fUnion.fFields.fLengthAndFlags & kAllStorageFlags) {
  સ્થિતિ kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_ઢગલો_Fields.fBuffer, src.fUnion.f_ઢગલો_Fields.fBuffer,
                getShortLength() * U_SIZEOF_UCHAR);
    તોડો;
  સ્થિતિ kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).addRef();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    જો(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    તોડો;
  સ્થિતિ kReadonlyAlias:
    જો(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
      જો(!hasShortLength()) {
        fUnion.fFields.fLength = src.fUnion.fFields.fLength;
      }
      તોડો;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  સ્થિતિ kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t srcLength = src.length();
    જો(allocate(srcLength)) {
      u_memcpy(getArrayStart(), src.getArrayStart(), srcLength);
      setLength(srcLength);
      તોડો;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  મૂળભૂત:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.fLengthAndFlags = kIsBogus;
    fUnion.fFields.fArray = nullptr;
    fUnion.fFields.fCapacity = 0;
    તોડો;
  }

  પાછા *આ;
}

UnicodeString& UnicodeString::operator=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, સાચું);
  પાછા *આ;
}

// Same as move assignment except without memory management.
શૂન્ય UnicodeString::copyFieldsFrom(UnicodeString &src, UBool setSrcToBogus) noexcept {
  int16_t lengthAndFlags = fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  જો(lengthAndFlags & k_Using_ઢગલો_Buffer) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    જો(આ != &src) {
      uprv_memcpy(fUnion.f_ઢગલો_Fields.fBuffer, src.fUnion.f_ઢગલો_Fields.fBuffer,
                  getShortLength() * U_SIZEOF_UCHAR);
    }
  } નહીંતર {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    જો(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    જો(setSrcToBogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.fLengthAndFlags = kIsBogus;
      src.fUnion.fFields.fArray = nullptr;
      src.fUnion.fFields.fCapacity = 0;
    }
  }
}

શૂન્ય UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString temp;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  temp.copyFieldsFrom(*આ, ખોટું);
  આ->copyFieldsFrom(other, ખોટું);
  other.copyFieldsFrom(temp, ખોટું);
  // Set temp to an empty string so that other's memory is not released twice.
  temp.fUnion.fFields.fLengthAndFlags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() અચલ {
    UnicodeString result(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    જો (result.isBogus()) {
        પાછા result;
    }
    અચલ char16_t *array = getBuffer();
    int32_t len = length();
    int32_t prev = 0;
    માટે (int32_t i=0;;) {
        જો (i == len) {
            result.append(array, prev, len - prev);
            તોડો;
        }
        જો (array[i++] == 0x5C /*'\\'*/) {
            result.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            જો (c < 0) {
                result.remove(); // return empty string
                તોડો; // invalid escape sequence
            }
            result.append(c);
            prev = i;
        }
    }
    પાછા result;
}

UChar32 UnicodeString::unescapeAt(int32_t &offset) અચલ {
    પાછા u_unescapeAt(UnicodeString_charAt, &offset, length(), (શૂન્ય*)આ);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(અચલ char16_t *text, int32_t len) અચલ {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  પાછા uprv_memcmp(getArrayStart(), text, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t start,
              int32_t length,
              અચલ char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) અચલ
{
  // compare illegal string values
  જો(isBogus()) {
    પાછા ખોટું;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  જો(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    પાછા length == 0 ? સાચું : ખોટું;
  }

  // get the correct pointer
  અચલ char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  // get the srcLength if necessary
  જો(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  જો (length != srcLength) {
    પાછા ખોટું;
  }

  જો(length == 0 || chars == srcChars) {
    પાછા સાચું;
  }

  પાછા u_memcmp(chars, srcChars, srcLength) == 0;
}

int8_t
UnicodeString::doCompare( int32_t start,
              int32_t length,
              અચલ char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) અચલ
{
  // compare illegal string values
  જો(isBogus()) {
    પાછા -1;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  જો(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    પાછા length == 0 ? 0 : 1;
  }

  // get the correct pointer
  અચલ char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  int32_t minLength;
  int8_t lengthResult;

  // get the srcLength if necessary
  જો(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  // are we comparing different lengths?
  જો(length != srcLength) {
    જો(length < srcLength) {
      minLength = length;
      lengthResult = -1;
    } નહીંતર {
      minLength = srcLength;
      lengthResult = 1;
    }
  } નહીંતર {
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

  જો(minLength > 0 && chars != srcChars) {
    int32_t result;

#   જો U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      result = uprv_memcmp(chars, srcChars, minLength * કદ(char16_t));
      જો(result != 0) {
        પાછા (int8_t)(result >> 15 | 1);
      }
#   નહીંતર
      // little-endian: compare char16_t units
      કરો {
        result = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        જો(result != 0) {
          પાછા static_cast<int8_t>(result >> 15 | 1);
        }
      } જ્યારે(--minLength > 0);
#   endif
  }
  પાછા lengthResult;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::doCompareCodePointOrder(int32_t start,
                                       int32_t length,
                                       અચલ char16_t *srcChars,
                                       int32_t srcStart,
                                       int32_t srcLength) અચલ
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  જો(isBogus()) {
    પાછા -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  જો(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=nullptr)?(srcChars + srcStart):nullptr, srcLength, ખોટું, સાચું);
  /* translate the 32-bit result into an 8-bit one */
  જો(diff!=0) {
    પાછા static_cast<int8_t>(diff >> 15 | 1);
  } નહીંતર {
    પાછા 0;
  }
}

int32_t
UnicodeString::getLength() અચલ {
    પાછા length();
}

char16_t
UnicodeString::getCharAt(int32_t offset) અચલ {
  પાછા charAt(offset);
}

UChar32
UnicodeString::getChar32At(int32_t offset) અચલ {
  પાછા char32At(offset);
}

UChar32
UnicodeString::char32At(int32_t offset) અચલ
{
  int32_t len = length();
  જો (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    અચલ char16_t *array = getArrayStart();
    UChar32 c;
    U16_GET(array, 0, offset, len, c);
    પાછા c;
  } નહીંતર {
    પાછા kInvalidUChar;
  }
}

int32_t
UnicodeString::getChar32Start(int32_t offset) અચલ {
  જો (static_cast<uint32_t>(offset) < static_cast<uint32_t>(length())) {
    અચલ char16_t *array = getArrayStart();
    U16_SET_CP_START(array, 0, offset);
    પાછા offset;
  } નહીંતર {
    પાછા 0;
  }
}

int32_t
UnicodeString::getChar32Limit(int32_t offset) અચલ {
  int32_t len = length();
  જો (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    અચલ char16_t *array = getArrayStart();
    U16_SET_CP_LIMIT(array, 0, offset, len);
    પાછા offset;
  } નહીંતર {
    પાછા len;
  }
}

int32_t
UnicodeString::ગણતરી_Char32(int32_t start, int32_t length) અચલ {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  પાછા u_ગણતરી_Char32(getArrayStart()+start, length);
}

UBool
UnicodeString::hasMoreChar32Than(int32_t start, int32_t length, int32_t number) અચલ {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  પાછા u_strHasMoreChar32Than(getArrayStart()+start, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t index, int32_t delta) અચલ {
  // pin index
  int32_t len = length();
  જો(index<0) {
    index=0;
  } નહીંતર જો(index>len) {
    index=len;
  }

  અચલ char16_t *array = getArrayStart();
  જો(delta>0) {
    U16_FWD_N(array, index, len, delta);
  } નહીંતર {
    U16_BACK_N(array, 0, index, -delta);
  }

  પાછા index;
}

શૂન્ય
UnicodeString::doExtract(int32_t start,
             int32_t length,
             char16_t *dst,
             int32_t dstStart) અચલ
{
  // pin indices to legal values
  pinIndices(start, length);

  // do not copy anything if we alias dst itself
  અચલ char16_t *array = getArrayStart();
  જો(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t destCapacity,
                       UErrorCode &errorCode) અચલ {
  int32_t len = length();
  જો(U_SUCCESS(errorCode)) {
    જો (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
      errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } નહીંતર {
      અચલ char16_t *array = getArrayStart();
      જો(len>0 && len<=destCapacity && array!=dest) {
        u_memcpy(dest, array, len);
      }
      પાછા u_terminateUChars(dest, destCapacity, len, &errorCode);
    }
  }

  પાછા len;
}

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       અક્ષર *target,
                       int32_t targetCapacity,
                       ગણના EInvariant) અચલ
{
  // if the arguments are illegal, then do nothing
  જો(targetCapacity < 0 || (targetCapacity > 0 && target == nullptr)) {
    પાછા 0;
  }

  // pin the indices to legal values
  pinIndices(start, length);

  જો(length <= targetCapacity) {
    u_UCharsToChars(getArrayStart() + start, target, length);
  }
  UErrorCode status = U_ZERO_ERROR;
  પાછા u_terminateChars(target, targetCapacity, length, &status);
}

UnicodeString
UnicodeString::tempSubString(int32_t start, int32_t len) અચલ U_LIFETIME_BOUND {
  pinIndices(start, len);
  અચલ char16_t *array = getBuffer();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  જો(array==nullptr) {
    array=fUnion.f_ઢગલો_Fields.fBuffer;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  પાછા UnicodeString(ખોટું, array + start, len);
}

int32_t
UnicodeString::toUTF8(int32_t start, int32_t len,
                      અક્ષર *target, int32_t capacity) અચલ {
  pinIndices(start, len);
  int32_t length8;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strToUTF8WithSub(target, capacity, &length8,
                     getBuffer() + start, len,
                     0xFFFD,  // Standard substitution character.
                     nullptr,    // Don't care about number of substitutions.
                     &errorCode);
  પાછા length8;
}

#જો U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start, int32_t len,
                       અક્ષર *target, uint32_t dstSize) અચલ {
  // if the arguments are illegal, then do nothing
  જો (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
    પાછા 0;
  }
  પાછા toUTF8(start, len, target, dstSize <= 0x7fffffff ? static_cast<int32_t>(dstSize) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#endif

શૂન્ય 
UnicodeString::extractBetween(int32_t start,
                  int32_t limit,
                  UnicodeString& target) અચલ {
  pinIndex(start);
  pinIndex(limit);
  doExtract(start, limit - start, target);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
શૂન્ય
UnicodeString::toUTF8(ByteSink &sink) અચલ {
  int32_t length16 = length();
  જો(length16 != 0) {
    અક્ષર ઢગલો_Buffer[1024];
    int32_t capacity = static_cast<int32_t>(કદ(ઢગલો_Buffer));
    UBool utf8IsOwned = ખોટું;
    અક્ષર *utf8 = sink.GetAppendBuffer(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      ઢગલો_Buffer, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8WithSub(utf8, capacity, &length8,
                       getBuffer(), length16,
                       0xFFFD,  // Standard substitution character.
                       nullptr,    // Don't care about number of substitutions.
                       &errorCode);
    જો(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      utf8 = static_cast<અક્ષર*>(uprv_malloc(length8));
      જો(utf8 != nullptr) {
        utf8IsOwned = સાચું;
        errorCode = U_ZERO_ERROR;
        u_strToUTF8WithSub(utf8, length8, &length8,
                           getBuffer(), length16,
                           0xFFFD,  // Standard substitution character.
                           nullptr,    // Don't care about number of substitutions.
                           &errorCode);
      } નહીંતર {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
      }
    }
    જો(U_SUCCESS(errorCode)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    જો(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::toUTF32(UChar32 *utf32, int32_t capacity, UErrorCode &errorCode) અચલ {
  int32_t length32=0;
  જો(U_SUCCESS(errorCode)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_strToUTF32WithSub(utf32, capacity, &length32,
        getBuffer(), length(),
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
  }
  પાછા length32;
}

int32_t 
UnicodeString::indexOf(અચલ char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) અચલ
{
  જો (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    પાછા -1;
  }

  // UnicodeString does not find empty substrings
  જો(srcLength < 0 && srcChars[srcStart] == 0) {
    પાછા -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  અચલ char16_t *array = getArrayStart();
  અચલ char16_t *match = u_strFindFirst(array + start, length, srcChars + srcStart, srcLength);
  જો(match == nullptr) {
    પાછા -1;
  } નહીંતર {
    પાછા static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(char16_t c,
             int32_t start,
             int32_t length) અચલ
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  અચલ char16_t *array = getArrayStart();
  અચલ char16_t *match = u_memchr(array + start, c, length);
  જો(match == nullptr) {
    પાછા -1;
  } નહીંતર {
    પાછા static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(UChar32 c,
                         int32_t start,
                         int32_t length) અચલ {
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  અચલ char16_t *array = getArrayStart();
  અચલ char16_t *match = u_memchr32(array + start, c, length);
  જો(match == nullptr) {
    પાછા -1;
  } નહીંતર {
    પાછા static_cast<int32_t>(match - array);
  }
}

int32_t 
UnicodeString::lastIndexOf(અચલ char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) અચલ
{
  જો (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    પાછા -1;
  }

  // UnicodeString does not find empty substrings
  જો(srcLength < 0 && srcChars[srcStart] == 0) {
    પાછા -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the last occurrence of the substring
  અચલ char16_t *array = getArrayStart();
  અચલ char16_t *match = u_strFindLast(array + start, length, srcChars + srcStart, srcLength);
  જો(match == nullptr) {
    પાછા -1;
  } નહીંતર {
    પાછા static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(char16_t c,
                 int32_t start,
                 int32_t length) અચલ
{
  જો(isBogus()) {
    પાછા -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  અચલ char16_t *array = getArrayStart();
  અચલ char16_t *match = u_memrchr(array + start, c, length);
  જો(match == nullptr) {
    પાછા -1;
  } નહીંતર {
    પાછા static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(UChar32 c,
                             int32_t start,
                             int32_t length) અચલ {
  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  અચલ char16_t *array = getArrayStart();
  અચલ char16_t *match = u_memrchr32(array + start, c, length);
  જો(match == nullptr) {
    પાછા -1;
  } નહીંતર {
    પાછા static_cast<int32_t>(match - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::findAndReplace(int32_t start,
                              int32_t length,
                              અચલ UnicodeString& oldText,
                              int32_t oldStart,
                              int32_t oldLength,
                              અચલ UnicodeString& newText,
                              int32_t newStart,
                              int32_t newLength) U_LIFETIME_BOUND
{
  જો(isBogus() || oldText.isBogus() || newText.isBogus()) {
    પાછા *આ;
  }

  pinIndices(start, length);
  oldText.pinIndices(oldStart, oldLength);
  newText.pinIndices(newStart, newLength);

  જો(oldLength == 0) {
    પાછા *આ;
  }

  જ્યારે(length > 0 && length >= oldLength) {
    int32_t pos = indexOf(oldText, oldStart, oldLength, start, length);
    જો(pos < 0) {
      // no more oldText's here: done
      તોડો;
    } નહીંતર {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, oldLength, newText, newStart, newLength);
      length -= pos + oldLength - start;
      start = pos + newLength;
    }
  }

  પાછા *આ;
}


શૂન્ય
UnicodeString::setToBogus()
{
  releaseArray();

  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
}

// turn a bogus string into an empty one
શૂન્ય
UnicodeString::unBogus() {
  જો(fUnion.fFields.fLengthAndFlags & kIsBogus) {
    setToEmpty();
  }
}

અચલ char16_t*
UnicodeString::getTerminatedBuffer() U_LIFETIME_BOUND {
  જો(!isWritable()) {
    પાછા nullptr;
  }
  char16_t *array = getArrayStart();
  int32_t len = length();
  જો(len < getCapacity()) {
    જો(fUnion.fFields.fLengthAndFlags & kBufferIsReadonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      જો(array[len] == 0) {
        પાછા array;
      }
    } નહીંતર જો(((fUnion.fFields.fLengthAndFlags & kRefCounted) == 0 || ref_ગણતરી() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      પાછા array;
    }
  }
  જો(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = getArrayStart();
    array[len] = 0;
    પાછા array;
  } નહીંતર {
    પાછા nullptr;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(UBool isTerminated,
                     ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                     int32_t textLength) U_LIFETIME_BOUND
{
  જો(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    પાછા *આ;
  }

  અચલ char16_t *text = textPtr;
  જો(text == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    પાછા *આ;
  }

  જો( textLength < -1 ||
      (textLength == -1 && !isTerminated) ||
      (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
    પાછા *આ;
  }

  releaseArray();

  જો(textLength == -1) {
    // text is terminated, or else it would have failed the above test
    textLength = u_strlen(text);
  }
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  setArray(const_cast<char16_t*>(text), textLength, isTerminated ? textLength + 1 : textLength);
  પાછા *આ;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(char16_t* buffer,
                     int32_t buffLength U_LIFETIME_BOUND,
                     int32_t buffCapacity) U_LIFETIME_BOUND {
  જો(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    પાછા *આ;
  }

  જો(buffer == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    પાછા *આ;
  }

  જો(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
    પાછા *આ;
  } નહીંતર જો(buffLength == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    અચલ char16_t *p = buffer, *limit = buffer + buffCapacity;
    જ્યારે(p != limit && *p != 0) {
      ++p;
    }
    buffLength = static_cast<int32_t>(p - buffer);
  }

  releaseArray();

  fUnion.fFields.fLengthAndFlags = kWritableAlias;
  setArray(buffer, buffLength, buffCapacity);
  પાછા *આ;
}

UnicodeString& UnicodeString::setToUTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  જો(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } નહીંતર {
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
  જો(U_FAILURE(errorCode)) {
    setToBogus();
  }
  પાછા *આ;
}

UnicodeString&
UnicodeString::setCharAt(int32_t offset, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  જો(cloneArrayIfNeeded() && len > 0) {
    જો(offset < 0) {
      offset = 0;
    } નહીંતર જો(offset >= len) {
      offset = len - 1;
    }

    getArrayStart()[offset] = c;
  }
  પાછા *આ;
}

UnicodeString&
UnicodeString::replace(int32_t start,
                       int32_t _length,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t count = 0;
  UBool isError = ખોટું;
  U16_APPEND(buffer, count, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  પાછા doReplace(start, _length, buffer, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t _length = 0;
  UBool isError = ખોટું;
  U16_APPEND(buffer, _length, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  પાછા isError ? *આ : doAppend(buffer, 0, _length);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         અચલ UnicodeString& src,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);

  // get the characters from src
  // and replace the range in ourselves with them
  પાછા doReplace(start, length, src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         અચલ char16_t* srcChars,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  જો(!isWritable()) {
    પાછા *આ;
  }

  int32_t oldLength = આ->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  જો((fUnion.fFields.fLengthAndFlags&kBufferIsReadonly) && srcLength == 0) {
    જો(start == 0) {
      // remove prefix by adjusting the array pointer
      pinIndex(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.fCapacity -= length;
      setLength(oldLength - length);
      પાછા *આ;
    } નહીંતર {
      pinIndex(start);
      જો(length >= (oldLength - start)) {
        // remove suffix by reducing the length (like truncate())
        setLength(start);
        fUnion.fFields.fCapacity = start;  // not NUL-terminated any more
        પાછા *આ;
      }
    }
  }

  જો(start == oldLength) {
    પાછા doAppend(srcChars, srcStart, srcLength);
  }

  જો (srcChars == nullptr) {
    srcLength = 0;
  } નહીંતર {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    જો (srcLength < 0) {
      // get the srcLength if necessary
      srcLength = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t newLength = oldLength - length;
  જો(srcLength > (INT32_MAX - newLength)) {
    setToBogus();
    પાછા *આ;
  }
  newLength += srcLength;

  // Check for insertion into ourself
  અચલ char16_t *oldArray = getArrayStart();
  જો (isBufferWritable() &&
      oldArray < srcChars + srcLength &&
      srcChars < oldArray + oldLength) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, srcLength);
    જો (copy.isBogus()) {
      setToBogus();
      પાછા *આ;
    }
    પાછા doReplace(start, length, copy.getArrayStart(), 0, srcLength);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_ઢગલો_Buffer[US_STACKBUF_SIZE];
  જો((fUnion.fFields.fLengthAndFlags&k_Using_ઢગલો_Buffer) && (newLength > US_STACKBUF_SIZE)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_ઢગલો_Buffer, oldArray, oldLength);
    oldArray = old_ઢગલો_Buffer;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *bufferToDelete = nullptr;
  જો(!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength),
                         ખોટું, &bufferToDelete)
  ) {
    પાછા *આ;
  }

  // now do the replace

  char16_t *newArray = getArrayStart();
  જો(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + srcLength,
                 oldLength - (start + length));
  } નહીંતર જો(length != srcLength) {
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
  જો (bufferToDelete) {
    uprv_free(bufferToDelete);
  }

  પાછા *આ;
}

UnicodeString&
UnicodeString::doReplace(int32_t start, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  જો (!isWritable()) {
    પાછા *આ;
  }
  જો (src.length() > INT32_MAX) {
    setToBogus();
    પાછા *આ;
  }
  પાછા doReplace(start, length, src.data(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(અચલ UnicodeString& src,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  જો(srcLength == 0) {
    પાછા *આ;
  }

  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);
  પાછા doAppend(src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doAppend(અચલ char16_t* srcChars,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  જો(!isWritable() || srcLength == 0 || srcChars == nullptr) {
    પાછા *આ;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  જો(srcLength < 0) {
    // get the srcLength if necessary
    જો((srcLength = u_strlen(srcChars)) == 0) {
      પાછા *આ;
    }
  }

  int32_t oldLength = length();
  int32_t newLength;

  જો (srcLength <= getCapacity() - oldLength && isBufferWritable()) {
    newLength = oldLength + srcLength;
    // Faster than a memmove
    જો (srcLength <= 4) {
      char16_t *arr = getArrayStart();
      arr[oldLength] = srcChars[0];
      જો (srcLength > 1) arr[oldLength+1] = srcChars[1];
      જો (srcLength > 2) arr[oldLength+2] = srcChars[2];
      જો (srcLength > 3) arr[oldLength+3] = srcChars[3];
      setLength(newLength);
      પાછા *આ;
    }
  } નહીંતર {
    જો (uprv_add32_overflow(oldLength, srcLength, &newLength)) {
      setToBogus();
      પાછા *આ;
    }

    // Check for append onto ourself
    અચલ char16_t* oldArray = getArrayStart();
    જો (isBufferWritable() &&
        oldArray < srcChars + srcLength &&
        srcChars < oldArray + oldLength) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, srcLength);
      જો (copy.isBogus()) {
        setToBogus();
        પાછા *આ;
      }
      પાછા doAppend(copy.getArrayStart(), 0, srcLength);
    }

    // optimize append() onto a large-enough, owned string
    જો (!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength))) {
      પાછા *આ;
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
  જો(srcChars != newArray + oldLength) {
    us_arrayCopy(srcChars, 0, newArray, oldLength, srcLength);
  }
  setLength(newLength);

  પાછા *આ;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  જો (!isWritable() || src.empty()) {
    પાછા *આ;
  }
  જો (src.length() > INT32_MAX) {
    setToBogus();
    પાછા *આ;
  }
  પાછા doAppend(src.data(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
શૂન્ય
UnicodeString::handleReplaceBetween(int32_t start,
                                    int32_t limit,
                                    અચલ UnicodeString& text) {
    replaceBetween(start, limit, text);
}

/**
 * Replaceable API
 */
શૂન્ય 
UnicodeString::copy(int32_t start, int32_t limit, int32_t dest) {
    જો (limit <= start) {
        પાછા; // Nothing to do; avoid bogus malloc call
    }
    char16_t* text = static_cast<char16_t*>(uprv_malloc(કદ(char16_t) * (limit - start)));
    // Check to make sure text is not null.
    જો (text != nullptr) {
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
UBool Replaceable::hasMetaData() અચલ {
    પાછા સાચું;
}

/**
 * Replaceable API
 */
UBool UnicodeString::hasMetaData() અચલ {
    પાછા ખોટું;
}

UnicodeString&
UnicodeString::doReverse(int32_t start, int32_t length) U_LIFETIME_BOUND {
  જો(length <= 1 || !cloneArrayIfNeeded()) {
    પાછા *આ;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  જો(length <= 1) {  // pinIndices() might have shrunk the length
    પાછા *આ;
  }

  char16_t *left = getArrayStart() + start;
  char16_t *જમણું = left + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = ખોટું;

  // Before the loop we know left<right because length>=2.
  કરો {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *left));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left++ = *જમણું));
    *જમણું-- = swap;
  } જ્યારે(left < જમણું);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  જો(hasSupplementary) {
    char16_t swap2;

    left = getArrayStart() + start;
    જમણું = left + length - 1; // -1 so that we can look at *(left+1) if left<right
    જ્યારે(left < જમણું) {
      જો(U16_IS_TRAIL(swap = *left) && U16_IS_LEAD(swap2 = *(left + 1))) {
        *left++ = swap2;
        *left++ = swap;
      } નહીંતર {
        ++left;
      }
    }
  }

  પાછા *આ;
}

UBool 
UnicodeString::padLeading(int32_t targetLength,
                          char16_t padChar)
{
  int32_t oldLength = length();
  જો(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    પાછા ખોટું;
  } નહીંતર {
    // move contents up by padding width
    char16_t *array = getArrayStart();
    int32_t start = targetLength - oldLength;
    us_arrayCopy(array, 0, array, start, oldLength);

    // fill in padding character
    જ્યારે(--start >= 0) {
      array[start] = padChar;
    }
    setLength(targetLength);
    પાછા સાચું;
  }
}

UBool 
UnicodeString::padTrailing(int32_t targetLength,
                           char16_t padChar)
{
  int32_t oldLength = length();
  જો(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    પાછા ખોટું;
  } નહીંતર {
    // fill in padding character
    char16_t *array = getArrayStart();
    int32_t length = targetLength;
    જ્યારે(--length >= oldLength) {
      array[length] = padChar;
    }
    setLength(targetLength);
    પાછા સાચું;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::doHashCode() અચલ
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hashCode = ustr_hashUCharsN(getArrayStart(), length());
    જો (hashCode == kInvalidHashCode) {
        hashCode = kEmptyHashCode;
    }
    પાછા hashCode;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::getBuffer(int32_t minCapacity) U_LIFETIME_BOUND {
  જો(minCapacity>=-1 && cloneArrayIfNeeded(minCapacity)) {
    fUnion.fFields.fLengthAndFlags|=kOpenGetBuffer;
    setZeroLength();
    પાછા getArrayStart();
  } નહીંતર {
    પાછા nullptr;
  }
}

શૂન્ય
UnicodeString::releaseBuffer(int32_t newLength) {
  જો(fUnion.fFields.fLengthAndFlags&kOpenGetBuffer && newLength>=-1) {
    // set the new fLength
    int32_t capacity=getCapacity();
    જો(newLength==-1) {
      // the new length is the string length, capped by fCapacity
      અચલ char16_t *array=getArrayStart(), *p=array, *limit=array+capacity;
      જ્યારે(p<limit && *p!=0) {
        ++p;
      }
      newLength = static_cast<int32_t>(p - array);
    } નહીંતર જો(newLength>capacity) {
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
  જો(newCapacity == -1) {
    newCapacity = getCapacity();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  જો(!isWritable()) {
    પાછા ખોટું;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  જો(forceClone ||
     fUnion.fFields.fLengthAndFlags & kBufferIsReadonly ||
     (fUnion.fFields.fLengthAndFlags & kRefCounted && ref_ગણતરી() > 1) ||
     newCapacity > getCapacity()
  ) {
    // check growCapacity for default value and use of the stack buffer
    જો(growCapacity < 0) {
      growCapacity = newCapacity;
    } નહીંતર જો(newCapacity <= US_STACKBUF_SIZE && growCapacity > US_STACKBUF_SIZE) {
      growCapacity = US_STACKBUF_SIZE;
    } નહીંતર જો(newCapacity > growCapacity) {
      setToBogus();
      પાછા ખોટું;  // bad inputs
    }
    જો(growCapacity > kMaxCapacity) {
      setToBogus();
      પાછા ખોટું;
    }

    // save old values
    char16_t old_ઢગલો_Buffer[US_STACKBUF_SIZE];
    char16_t *oldArray;
    int32_t oldLength = length();
    int16_t flags = fUnion.fFields.fLengthAndFlags;

    જો(flags&k_Using_ઢગલો_Buffer) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      જો(doCopyArray && growCapacity > US_STACKBUF_SIZE) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_ઢગલો_Fields.fBuffer, 0, old_ઢગલો_Buffer, 0, oldLength);
        oldArray = old_ઢગલો_Buffer;
      } નહીંતર {
        oldArray = nullptr; // no need to copy from the stack buffer to itself
      }
    } નહીંતર {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=nullptr); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    જો(allocate(growCapacity) ||
       (newCapacity < growCapacity && allocate(newCapacity))
    ) {
      જો(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t minLength = oldLength;
        newCapacity = getCapacity();
        જો(newCapacity < minLength) {
          minLength = newCapacity;
        }
        જો(oldArray != nullptr) {
          us_arrayCopy(oldArray, 0, getArrayStart(), 0, minLength);
        }
        setLength(minLength);
      } નહીંતર {
        setZeroLength();
      }

      // release the old array
      જો(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_ગણતરી = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        જો(umtx_atomic_dec(p_Ref_ગણતરી) == 0) {
          જો (pBufferToDelete == nullptr) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((શૂન્ય *)p_Ref_ગણતરી);
          } નહીંતર {
            // the caller requested to delete it himself
            *pBufferToDelete = reinterpret_cast<int32_t*>(p_Ref_ગણતરી);
          }
        }
      }
    } નહીંતર {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      જો(!(flags&k_Using_ઢગલો_Buffer)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.fLengthAndFlags = flags;
      setToBogus();
      પાછા ખોટું;
    }
  }
  પાછા સાચું;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::appendCodeUnit(char16_t c) {
  પાછા str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::appendCodePoint(UChar32 c) {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t cLength = 0;
  UBool isError = ખોટું;
  U16_APPEND(buffer, cLength, U16_MAX_LENGTH, c, isError);
  પાછા !isError && str.doAppend(buffer, 0, cLength).isWritable();
}

UBool
UnicodeStringAppendable::appendString(અચલ char16_t *s, int32_t length) {
  પાછા str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserveAppendCapacity(int32_t appendCapacity) {
  પાછા str.cloneArrayIfNeeded(str.length() + appendCapacity);
}

char16_t* UnicodeStringAppendable::getAppendBuffer(int32_t minCapacity,
                                                   int32_t desiredCapacityHint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratchCapacity,
                                                   int32_t* resultCapacity) U_LIFETIME_BOUND {
  જો(minCapacity < 1 || scratchCapacity < minCapacity) {
    *resultCapacity = 0;
    પાછા nullptr;
  }
  int32_t oldLength = str.length();
  જો(minCapacity <= (kMaxCapacity - oldLength) &&
      desiredCapacityHint <= (kMaxCapacity - oldLength) &&
      str.cloneArrayIfNeeded(oldLength + minCapacity, oldLength + desiredCapacityHint)) {
    *resultCapacity = str.getCapacity() - oldLength;
    પાછા str.getArrayStart() + oldLength;
  }
  *resultCapacity = scratchCapacity;
  પાછા scratch;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(અચલ UElement ચાવી) {
    અચલ UnicodeString *str = (અચલ UnicodeString*) ચાવી.pointer;
    પાછા (str == nullptr) ? 0 : str->hashCode();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(અચલ UElement key1, અચલ UElement key2) {
    અચલ UnicodeString *str1 = (અચલ UnicodeString*) key1.pointer;
    અચલ UnicodeString *str2 = (અચલ UnicodeString*) key2.pointer;
    જો (str1 == str2) {
        પાછા સાચું;
    }
    જો (str1 == nullptr || str2 == nullptr) {
        પાછા ખોટું;
    }
    પાછા *str1 == *str2;
}

#ifdef U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#જો defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
સ્થિર શૂન્ય uprv_UnicodeStringDummy() {
    delete [] (નવું UnicodeString[2]);
}
#pragma GCC diagnostic pop
#endif
#endif
