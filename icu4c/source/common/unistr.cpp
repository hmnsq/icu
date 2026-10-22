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

#شامل <string_view>

#شامل "unicode/utypes.h"
#شامل "unicode/appendable.h"
#شامل "unicode/putil.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "unicode/ustring.h"
#شامل "unicode/unistr.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf16.h"
#شامل "uelement.h"
#شامل "ustr_imp.h"
#شامل "umutex.h"
#شامل "uassert.h"

#اگر 0

#شامل <iostream>
using فضاینام std;

//DEBUGGING
تهی
print(ثابت UnicodeString& s,
      ثابت نویسه *name)
{
  char16_t c;
  cout << name << ":|";
  برای(صحیح i = 0; i < s.length(); ++i) {
    c = s[i];
    اگر(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    وگرنه
      cout << (نویسه) s[i];
  }
  cout << '|' << endl;
}

تهی
print(ثابت char16_t *s,
      int32_t len,
      ثابت نویسه *name)
{
  char16_t c;
  cout << name << ":|";
  برای(صحیح i = 0; i < len; ++i) {
    c = s[i];
    اگر(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    وگرنه
      cout << (نویسه) s[i];
  }
  cout << '|' << endl;
}
// END DEBUGGING
#endif

// Local function definitions for now

// need to copy areas that may overlap
ایستا
درونخطی تهی
us_arrayCopy(ثابت char16_t *src, int32_t srcStart,
         char16_t *dst, int32_t dstStart, int32_t count)
{
  اگر(count>0) {
    uprv_memmove(dst+dstStart, src+srcStart, (size_t)count*اندازه(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
ایستا char16_t U_CALLCONV
UnicodeString_charAt(int32_t offset, تهی *context) {
    بازگشت ((icu::UnicodeString*) context)->charAt(offset);
}
U_CDECL_END

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
operator+ (ثابت UnicodeString &s1, ثابت UnicodeString &s2) {
  int32_t sumLengths;
  اگر (uprv_add32_overflow(s1.length(), s2.length(), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    بازگشت bogus;
  }
  اگر (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  بازگشت UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_internalConcat(ثابت UnicodeString &s1, std::u16string_view s2) {
  int32_t sumLengths;
  اگر (s2.length() > INT32_MAX ||
      uprv_add32_overflow(s1.length(), static_cast<int32_t>(s2.length()), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    بازگشت bogus;
  }
  اگر (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  بازگشت UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

تهی
UnicodeString::addRef() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  بازگشت umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_شمار() ثابت {
  بازگشت umtx_loadAcquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

تهی
UnicodeString::releaseArray() {
  اگر((fUnion.fFields.fLengthAndFlags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.fLengthAndFlags = 0;
  اگر (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } وگرنه اگر(c <= 0xffff) {
    int32_t length = count;
    اگر(capacity < length) {
      capacity = length;
    }
    اگر(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t unit = static_cast<char16_t>(c);
      برای(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      setLength(length);
    }
  } وگرنه {  // supplementary code point, write surrogate pairs
    اگر(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      بازگشت;
    }
    int32_t length = count * 2;
    اگر(capacity < length) {
      capacity = length;
    }
    اگر(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      برای(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      setLength(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.fLengthAndFlags = kLength1 | kShortString;
  fUnion.f_پشته_Fields.fBuffer[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  int32_t i = 0;
  UBool isError = نادرست;
  U16_APPEND(fUnion.f_پشته_Fields.fBuffer, i, US_STACKBUF_SIZE, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  اگر(!isError) {
    setShortLength(i);
  }
}

UnicodeString::UnicodeString(ثابت char16_t *text,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  doAppend(text, 0, textLength);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  ثابت char16_t *text = textPtr;
  اگر(text == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } وگرنه اگر(textLength < -1 ||
            (textLength == -1 && !isTerminated) ||
            (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
  } وگرنه {
    اگر(textLength == -1) {
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
  اگر(buff == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } وگرنه اگر(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
  } وگرنه {
    اگر(buffLength == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ثابت char16_t *p = buff, *limit = buff + buffCapacity;
      وقتی(p != limit && *p != 0) {
        ++p;
      }
      buffLength = static_cast<int32_t>(p - buff);
    }
    setArray(buff, buffLength, buffCapacity);
  }
}

UnicodeString::UnicodeString(ثابت نویسه *src, int32_t length, EInvariant) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  اگر(src==nullptr) {
    // treat as an empty string
  } وگرنه {
    اگر(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    اگر(cloneArrayIfNeeded(length, length, نادرست)) {
      u_charsToUChars(src, getArrayStart(), length);
      setLength(length);
    } وگرنه {
      setToBogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view text U_LIFETIME_BOUND) {
  UnicodeString result;
  اگر (text.length() <= INT32_MAX) {
    result.setTo(نادرست, text.data(), static_cast<int32_t>(text.length()));
  } وگرنه {
    result.setToBogus();
  }
  بازگشت result;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ثابت UnicodeString& text U_LIFETIME_BOUND) {
  UnicodeString result;
  اگر (text.isBogus()) {
    result.setToBogus();
  } وگرنه {
    result.setTo(نادرست, text.getBuffer(), text.length());
  }
  بازگشت result;
}

#اگر U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ثابت نویسه *codepageData) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  اگر (codepageData != nullptr) {
    setToUTF8(codepageData);
  }
}

UnicodeString::UnicodeString(ثابت نویسه *codepageData, int32_t dataLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  // if there's nothing to convert, do nothing
  اگر (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
    بازگشت;
  }
  اگر(dataLength == -1) {
    dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
  }
  setToUTF8(StringPiece(codepageData, dataLength));
}

// else see unistr_cnv.cpp
#endif

UnicodeString::UnicodeString(ثابت UnicodeString& that) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, درست);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t srcStart) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t srcStart,
                             int32_t srcLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart, srcLength);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ثابت {
  بازگشت nullptr;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ثابت {
  LocalPointer<UnicodeString> clonedString(جدید UnicodeString(*این));
  بازگشت clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : nullptr;
}

//========================================
// array allocation
//========================================

فضاینام {

ثابت int32_t kGrowSize = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ثابت int32_t kMaxCapacity = 0x7ffffff5;

int32_t getGrowCapacity(int32_t newLength) {
  int32_t growSize = (newLength >> 2) + kGrowSize;
  اگر(growSize <= (kMaxCapacity - newLength)) {
    بازگشت newLength + growSize;
  } وگرنه {
    بازگشت kMaxCapacity;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  اگر(capacity <= US_STACKBUF_SIZE) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    بازگشت درست;
  }
  اگر(capacity <= kMaxCapacity) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t رقم_Bytes = اندازه(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    رقم_Bytes = (رقم_Bytes + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(رقم_Bytes));
    اگر(array != nullptr) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      رقم_Bytes -= اندازه(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.fCapacity = static_cast<int32_t>(رقم_Bytes / U_SIZEOF_UCHAR);
      fUnion.fFields.fLengthAndFlags = kLongString;
      بازگشت درست;
    }
  }
  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
  بازگشت نادرست;
}

//========================================
// Destructor
//========================================

#ifdef UNISTR_شمار_FINAL_STRING_LENGTHS
ایستا u_atomic_int32_t finalLengthCounts[0x400];  // UnicodeString::kMaxShortLength+1
ایستا u_atomic_int32_t beyond_شمار(0);

U_CAPI تهی unistr_printLengths() {
  int32_t i;
  برای(i = 0; i <= 59; ++i) {
    printf("%2d,  %9d\n", i, (int32_t)finalLengthCounts[i]);
  }
  int32_t beyond = beyond_شمار;
  برای(; i < UPRV_LENGTHOF(finalLengthCounts); ++i) {
    beyond += finalLengthCounts[i];
  }
  printf(">59, %9d\n", beyond);
}
#endif

UnicodeString::~UnicodeString()
{
#ifdef UNISTR_شمار_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  اگر((fUnion.fFields.fLengthAndFlags&(kOpenGetBuffer|kReadonlyAlias|kWritableAlias)) == 0) {
    اگر(hasShortLength()) {
      umtx_atomic_inc(finalLengthCounts + getShortLength());
    } وگرنه {
      umtx_atomic_inc(&beyond_شمار);
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
  بازگشت result;
}

UnicodeString UnicodeString::fromUTF32(ثابت UChar32 *utf32, int32_t length) {
  UnicodeString result;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  اگر(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } وگرنه {
    capacity = length + (length >> 4) + 4;
  }
  انجامبده {
    char16_t *utf16 = result.getBuffer(capacity);
    int32_t length16;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, result.getCapacity(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
    result.releaseBuffer(length16);
    اگر(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      ادامه;
    } وگرنه اگر(U_FAILURE(errorCode)) {
      result.setToBogus();
    }
    بشکن;
  } وقتی(درست);
  بازگشت result;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::operator=(ثابت UnicodeString& src) U_LIFETIME_BOUND {
  بازگشت copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ثابت UnicodeString& src) U_LIFETIME_BOUND {
  بازگشت copyFrom(src, درست);
}

UnicodeString&
UnicodeString::copyFrom(ثابت UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  اگر(این == &src) {
    بازگشت *این;
  }

  // is the right side bogus?
  اگر(src.isBogus()) {
    setToBogus();
    بازگشت *این;
  }

  // delete the current contents
  releaseArray();

  اگر(src.isEmpty()) {
    // empty string - use the stack buffer
    setToEmpty();
    بازگشت *این;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  انتخاب(src.fUnion.fFields.fLengthAndFlags & kAllStorageFlags) {
  حالت kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_پشته_Fields.fBuffer, src.fUnion.f_پشته_Fields.fBuffer,
                getShortLength() * U_SIZEOF_UCHAR);
    بشکن;
  حالت kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).addRef();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    اگر(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    بشکن;
  حالت kReadonlyAlias:
    اگر(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
      اگر(!hasShortLength()) {
        fUnion.fFields.fLength = src.fUnion.fFields.fLength;
      }
      بشکن;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  حالت kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t srcLength = src.length();
    اگر(allocate(srcLength)) {
      u_memcpy(getArrayStart(), src.getArrayStart(), srcLength);
      setLength(srcLength);
      بشکن;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  پیشفرض:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.fLengthAndFlags = kIsBogus;
    fUnion.fFields.fArray = nullptr;
    fUnion.fFields.fCapacity = 0;
    بشکن;
  }

  بازگشت *این;
}

UnicodeString& UnicodeString::operator=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, درست);
  بازگشت *این;
}

// Same as move assignment except without memory management.
تهی UnicodeString::copyFieldsFrom(UnicodeString &src, UBool setSrcToBogus) noexcept {
  int16_t lengthAndFlags = fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  اگر(lengthAndFlags & k_Using_پشته_Buffer) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    اگر(این != &src) {
      uprv_memcpy(fUnion.f_پشته_Fields.fBuffer, src.fUnion.f_پشته_Fields.fBuffer,
                  getShortLength() * U_SIZEOF_UCHAR);
    }
  } وگرنه {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    اگر(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    اگر(setSrcToBogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.fLengthAndFlags = kIsBogus;
      src.fUnion.fFields.fArray = nullptr;
      src.fUnion.fFields.fCapacity = 0;
    }
  }
}

تهی UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString temp;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  temp.copyFieldsFrom(*این, نادرست);
  این->copyFieldsFrom(other, نادرست);
  other.copyFieldsFrom(temp, نادرست);
  // Set temp to an empty string so that other's memory is not released twice.
  temp.fUnion.fFields.fLengthAndFlags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ثابت {
    UnicodeString result(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    اگر (result.isBogus()) {
        بازگشت result;
    }
    ثابت char16_t *array = getBuffer();
    int32_t len = length();
    int32_t prev = 0;
    برای (int32_t i=0;;) {
        اگر (i == len) {
            result.append(array, prev, len - prev);
            بشکن;
        }
        اگر (array[i++] == 0x5C /*'\\'*/) {
            result.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            اگر (c < 0) {
                result.remove(); // return empty string
                بشکن; // invalid escape sequence
            }
            result.append(c);
            prev = i;
        }
    }
    بازگشت result;
}

UChar32 UnicodeString::unescapeAt(int32_t &offset) ثابت {
    بازگشت u_unescapeAt(UnicodeString_charAt, &offset, length(), (تهی*)این);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(ثابت char16_t *text, int32_t len) ثابت {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  بازگشت uprv_memcmp(getArrayStart(), text, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t start,
              int32_t length,
              ثابت char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) ثابت
{
  // compare illegal string values
  اگر(isBogus()) {
    بازگشت نادرست;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  اگر(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    بازگشت length == 0 ? درست : نادرست;
  }

  // get the correct pointer
  ثابت char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  // get the srcLength if necessary
  اگر(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  اگر (length != srcLength) {
    بازگشت نادرست;
  }

  اگر(length == 0 || chars == srcChars) {
    بازگشت درست;
  }

  بازگشت u_memcmp(chars, srcChars, srcLength) == 0;
}

int8_t
UnicodeString::doCompare( int32_t start,
              int32_t length,
              ثابت char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) ثابت
{
  // compare illegal string values
  اگر(isBogus()) {
    بازگشت -1;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  اگر(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    بازگشت length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ثابت char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  int32_t minLength;
  int8_t lengthResult;

  // get the srcLength if necessary
  اگر(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  // are we comparing different lengths?
  اگر(length != srcLength) {
    اگر(length < srcLength) {
      minLength = length;
      lengthResult = -1;
    } وگرنه {
      minLength = srcLength;
      lengthResult = 1;
    }
  } وگرنه {
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

  اگر(minLength > 0 && chars != srcChars) {
    int32_t result;

#   اگر U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      result = uprv_memcmp(chars, srcChars, minLength * اندازه(char16_t));
      اگر(result != 0) {
        بازگشت (int8_t)(result >> 15 | 1);
      }
#   وگرنه
      // little-endian: compare char16_t units
      انجامبده {
        result = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        اگر(result != 0) {
          بازگشت static_cast<int8_t>(result >> 15 | 1);
        }
      } وقتی(--minLength > 0);
#   endif
  }
  بازگشت lengthResult;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::doCompareCodePointOrder(int32_t start,
                                       int32_t length,
                                       ثابت char16_t *srcChars,
                                       int32_t srcStart,
                                       int32_t srcLength) ثابت
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  اگر(isBogus()) {
    بازگشت -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  اگر(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=nullptr)?(srcChars + srcStart):nullptr, srcLength, نادرست, درست);
  /* translate the 32-bit result into an 8-bit one */
  اگر(diff!=0) {
    بازگشت static_cast<int8_t>(diff >> 15 | 1);
  } وگرنه {
    بازگشت 0;
  }
}

int32_t
UnicodeString::getLength() ثابت {
    بازگشت length();
}

char16_t
UnicodeString::getCharAt(int32_t offset) ثابت {
  بازگشت charAt(offset);
}

UChar32
UnicodeString::getChar32At(int32_t offset) ثابت {
  بازگشت char32At(offset);
}

UChar32
UnicodeString::char32At(int32_t offset) ثابت
{
  int32_t len = length();
  اگر (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = getArrayStart();
    UChar32 c;
    U16_GET(array, 0, offset, len, c);
    بازگشت c;
  } وگرنه {
    بازگشت kInvalidUChar;
  }
}

int32_t
UnicodeString::getChar32Start(int32_t offset) ثابت {
  اگر (static_cast<uint32_t>(offset) < static_cast<uint32_t>(length())) {
    ثابت char16_t *array = getArrayStart();
    U16_SET_CP_START(array, 0, offset);
    بازگشت offset;
  } وگرنه {
    بازگشت 0;
  }
}

int32_t
UnicodeString::getChar32Limit(int32_t offset) ثابت {
  int32_t len = length();
  اگر (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = getArrayStart();
    U16_SET_CP_LIMIT(array, 0, offset, len);
    بازگشت offset;
  } وگرنه {
    بازگشت len;
  }
}

int32_t
UnicodeString::شمار_Char32(int32_t start, int32_t length) ثابت {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  بازگشت u_شمار_Char32(getArrayStart()+start, length);
}

UBool
UnicodeString::hasMoreChar32Than(int32_t start, int32_t length, int32_t number) ثابت {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  بازگشت u_strHasMoreChar32Than(getArrayStart()+start, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t index, int32_t delta) ثابت {
  // pin index
  int32_t len = length();
  اگر(index<0) {
    index=0;
  } وگرنه اگر(index>len) {
    index=len;
  }

  ثابت char16_t *array = getArrayStart();
  اگر(delta>0) {
    U16_FWD_N(array, index, len, delta);
  } وگرنه {
    U16_BACK_N(array, 0, index, -delta);
  }

  بازگشت index;
}

تهی
UnicodeString::doExtract(int32_t start,
             int32_t length,
             char16_t *dst,
             int32_t dstStart) ثابت
{
  // pin indices to legal values
  pinIndices(start, length);

  // do not copy anything if we alias dst itself
  ثابت char16_t *array = getArrayStart();
  اگر(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t destCapacity,
                       UErrorCode &errorCode) ثابت {
  int32_t len = length();
  اگر(U_SUCCESS(errorCode)) {
    اگر (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
      errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } وگرنه {
      ثابت char16_t *array = getArrayStart();
      اگر(len>0 && len<=destCapacity && array!=dest) {
        u_memcpy(dest, array, len);
      }
      بازگشت u_terminateUChars(dest, destCapacity, len, &errorCode);
    }
  }

  بازگشت len;
}

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       نویسه *target,
                       int32_t targetCapacity,
                       شمارش EInvariant) ثابت
{
  // if the arguments are illegal, then do nothing
  اگر(targetCapacity < 0 || (targetCapacity > 0 && target == nullptr)) {
    بازگشت 0;
  }

  // pin the indices to legal values
  pinIndices(start, length);

  اگر(length <= targetCapacity) {
    u_UCharsToChars(getArrayStart() + start, target, length);
  }
  UErrorCode status = U_ZERO_ERROR;
  بازگشت u_terminateChars(target, targetCapacity, length, &status);
}

UnicodeString
UnicodeString::tempSubString(int32_t start, int32_t len) ثابت U_LIFETIME_BOUND {
  pinIndices(start, len);
  ثابت char16_t *array = getBuffer();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  اگر(array==nullptr) {
    array=fUnion.f_پشته_Fields.fBuffer;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  بازگشت UnicodeString(نادرست, array + start, len);
}

int32_t
UnicodeString::toUTF8(int32_t start, int32_t len,
                      نویسه *target, int32_t capacity) ثابت {
  pinIndices(start, len);
  int32_t length8;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strToUTF8WithSub(target, capacity, &length8,
                     getBuffer() + start, len,
                     0xFFFD,  // Standard substitution character.
                     nullptr,    // Don't care about number of substitutions.
                     &errorCode);
  بازگشت length8;
}

#اگر U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start, int32_t len,
                       نویسه *target, uint32_t dstSize) ثابت {
  // if the arguments are illegal, then do nothing
  اگر (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
    بازگشت 0;
  }
  بازگشت toUTF8(start, len, target, dstSize <= 0x7fffffff ? static_cast<int32_t>(dstSize) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#endif

تهی 
UnicodeString::extractBetween(int32_t start,
                  int32_t limit,
                  UnicodeString& target) ثابت {
  pinIndex(start);
  pinIndex(limit);
  doExtract(start, limit - start, target);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
تهی
UnicodeString::toUTF8(ByteSink &sink) ثابت {
  int32_t length16 = length();
  اگر(length16 != 0) {
    نویسه پشته_Buffer[1024];
    int32_t capacity = static_cast<int32_t>(اندازه(پشته_Buffer));
    UBool utf8IsOwned = نادرست;
    نویسه *utf8 = sink.GetAppendBuffer(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      پشته_Buffer, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8WithSub(utf8, capacity, &length8,
                       getBuffer(), length16,
                       0xFFFD,  // Standard substitution character.
                       nullptr,    // Don't care about number of substitutions.
                       &errorCode);
    اگر(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      utf8 = static_cast<نویسه*>(uprv_malloc(length8));
      اگر(utf8 != nullptr) {
        utf8IsOwned = درست;
        errorCode = U_ZERO_ERROR;
        u_strToUTF8WithSub(utf8, length8, &length8,
                           getBuffer(), length16,
                           0xFFFD,  // Standard substitution character.
                           nullptr,    // Don't care about number of substitutions.
                           &errorCode);
      } وگرنه {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
      }
    }
    اگر(U_SUCCESS(errorCode)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    اگر(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::toUTF32(UChar32 *utf32, int32_t capacity, UErrorCode &errorCode) ثابت {
  int32_t length32=0;
  اگر(U_SUCCESS(errorCode)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_strToUTF32WithSub(utf32, capacity, &length32,
        getBuffer(), length(),
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
  }
  بازگشت length32;
}

int32_t 
UnicodeString::indexOf(ثابت char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) ثابت
{
  اگر (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    بازگشت -1;
  }

  // UnicodeString does not find empty substrings
  اگر(srcLength < 0 && srcChars[srcStart] == 0) {
    بازگشت -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  ثابت char16_t *array = getArrayStart();
  ثابت char16_t *match = u_strFindFirst(array + start, length, srcChars + srcStart, srcLength);
  اگر(match == nullptr) {
    بازگشت -1;
  } وگرنه {
    بازگشت static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(char16_t c,
             int32_t start,
             int32_t length) ثابت
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ثابت char16_t *array = getArrayStart();
  ثابت char16_t *match = u_memchr(array + start, c, length);
  اگر(match == nullptr) {
    بازگشت -1;
  } وگرنه {
    بازگشت static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(UChar32 c,
                         int32_t start,
                         int32_t length) ثابت {
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ثابت char16_t *array = getArrayStart();
  ثابت char16_t *match = u_memchr32(array + start, c, length);
  اگر(match == nullptr) {
    بازگشت -1;
  } وگرنه {
    بازگشت static_cast<int32_t>(match - array);
  }
}

int32_t 
UnicodeString::lastIndexOf(ثابت char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) ثابت
{
  اگر (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    بازگشت -1;
  }

  // UnicodeString does not find empty substrings
  اگر(srcLength < 0 && srcChars[srcStart] == 0) {
    بازگشت -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the last occurrence of the substring
  ثابت char16_t *array = getArrayStart();
  ثابت char16_t *match = u_strFindLast(array + start, length, srcChars + srcStart, srcLength);
  اگر(match == nullptr) {
    بازگشت -1;
  } وگرنه {
    بازگشت static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(char16_t c,
                 int32_t start,
                 int32_t length) ثابت
{
  اگر(isBogus()) {
    بازگشت -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ثابت char16_t *array = getArrayStart();
  ثابت char16_t *match = u_memrchr(array + start, c, length);
  اگر(match == nullptr) {
    بازگشت -1;
  } وگرنه {
    بازگشت static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(UChar32 c,
                             int32_t start,
                             int32_t length) ثابت {
  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ثابت char16_t *array = getArrayStart();
  ثابت char16_t *match = u_memrchr32(array + start, c, length);
  اگر(match == nullptr) {
    بازگشت -1;
  } وگرنه {
    بازگشت static_cast<int32_t>(match - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::findAndReplace(int32_t start,
                              int32_t length,
                              ثابت UnicodeString& oldText,
                              int32_t oldStart,
                              int32_t oldLength,
                              ثابت UnicodeString& newText,
                              int32_t newStart,
                              int32_t newLength) U_LIFETIME_BOUND
{
  اگر(isBogus() || oldText.isBogus() || newText.isBogus()) {
    بازگشت *این;
  }

  pinIndices(start, length);
  oldText.pinIndices(oldStart, oldLength);
  newText.pinIndices(newStart, newLength);

  اگر(oldLength == 0) {
    بازگشت *این;
  }

  وقتی(length > 0 && length >= oldLength) {
    int32_t pos = indexOf(oldText, oldStart, oldLength, start, length);
    اگر(pos < 0) {
      // no more oldText's here: done
      بشکن;
    } وگرنه {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, oldLength, newText, newStart, newLength);
      length -= pos + oldLength - start;
      start = pos + newLength;
    }
  }

  بازگشت *این;
}


تهی
UnicodeString::setToBogus()
{
  releaseArray();

  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
}

// turn a bogus string into an empty one
تهی
UnicodeString::unBogus() {
  اگر(fUnion.fFields.fLengthAndFlags & kIsBogus) {
    setToEmpty();
  }
}

ثابت char16_t*
UnicodeString::getTerminatedBuffer() U_LIFETIME_BOUND {
  اگر(!isWritable()) {
    بازگشت nullptr;
  }
  char16_t *array = getArrayStart();
  int32_t len = length();
  اگر(len < getCapacity()) {
    اگر(fUnion.fFields.fLengthAndFlags & kBufferIsReadonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      اگر(array[len] == 0) {
        بازگشت array;
      }
    } وگرنه اگر(((fUnion.fFields.fLengthAndFlags & kRefCounted) == 0 || ref_شمار() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      بازگشت array;
    }
  }
  اگر(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = getArrayStart();
    array[len] = 0;
    بازگشت array;
  } وگرنه {
    بازگشت nullptr;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(UBool isTerminated,
                     ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                     int32_t textLength) U_LIFETIME_BOUND
{
  اگر(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    بازگشت *این;
  }

  ثابت char16_t *text = textPtr;
  اگر(text == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    بازگشت *این;
  }

  اگر( textLength < -1 ||
      (textLength == -1 && !isTerminated) ||
      (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
    بازگشت *این;
  }

  releaseArray();

  اگر(textLength == -1) {
    // text is terminated, or else it would have failed the above test
    textLength = u_strlen(text);
  }
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  setArray(const_cast<char16_t*>(text), textLength, isTerminated ? textLength + 1 : textLength);
  بازگشت *این;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(char16_t* buffer,
                     int32_t buffLength U_LIFETIME_BOUND,
                     int32_t buffCapacity) U_LIFETIME_BOUND {
  اگر(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    بازگشت *این;
  }

  اگر(buffer == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    بازگشت *این;
  }

  اگر(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
    بازگشت *این;
  } وگرنه اگر(buffLength == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ثابت char16_t *p = buffer, *limit = buffer + buffCapacity;
    وقتی(p != limit && *p != 0) {
      ++p;
    }
    buffLength = static_cast<int32_t>(p - buffer);
  }

  releaseArray();

  fUnion.fFields.fLengthAndFlags = kWritableAlias;
  setArray(buffer, buffLength, buffCapacity);
  بازگشت *این;
}

UnicodeString& UnicodeString::setToUTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  اگر(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } وگرنه {
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
  اگر(U_FAILURE(errorCode)) {
    setToBogus();
  }
  بازگشت *این;
}

UnicodeString&
UnicodeString::setCharAt(int32_t offset, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  اگر(cloneArrayIfNeeded() && len > 0) {
    اگر(offset < 0) {
      offset = 0;
    } وگرنه اگر(offset >= len) {
      offset = len - 1;
    }

    getArrayStart()[offset] = c;
  }
  بازگشت *این;
}

UnicodeString&
UnicodeString::replace(int32_t start,
                       int32_t _length,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t count = 0;
  UBool isError = نادرست;
  U16_APPEND(buffer, count, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  بازگشت doReplace(start, _length, buffer, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t _length = 0;
  UBool isError = نادرست;
  U16_APPEND(buffer, _length, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  بازگشت isError ? *این : doAppend(buffer, 0, _length);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ثابت UnicodeString& src,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);

  // get the characters from src
  // and replace the range in ourselves with them
  بازگشت doReplace(start, length, src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ثابت char16_t* srcChars,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  اگر(!isWritable()) {
    بازگشت *این;
  }

  int32_t oldLength = این->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  اگر((fUnion.fFields.fLengthAndFlags&kBufferIsReadonly) && srcLength == 0) {
    اگر(start == 0) {
      // remove prefix by adjusting the array pointer
      pinIndex(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.fCapacity -= length;
      setLength(oldLength - length);
      بازگشت *این;
    } وگرنه {
      pinIndex(start);
      اگر(length >= (oldLength - start)) {
        // remove suffix by reducing the length (like truncate())
        setLength(start);
        fUnion.fFields.fCapacity = start;  // not NUL-terminated any more
        بازگشت *این;
      }
    }
  }

  اگر(start == oldLength) {
    بازگشت doAppend(srcChars, srcStart, srcLength);
  }

  اگر (srcChars == nullptr) {
    srcLength = 0;
  } وگرنه {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    اگر (srcLength < 0) {
      // get the srcLength if necessary
      srcLength = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t newLength = oldLength - length;
  اگر(srcLength > (INT32_MAX - newLength)) {
    setToBogus();
    بازگشت *این;
  }
  newLength += srcLength;

  // Check for insertion into ourself
  ثابت char16_t *oldArray = getArrayStart();
  اگر (isBufferWritable() &&
      oldArray < srcChars + srcLength &&
      srcChars < oldArray + oldLength) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, srcLength);
    اگر (copy.isBogus()) {
      setToBogus();
      بازگشت *این;
    }
    بازگشت doReplace(start, length, copy.getArrayStart(), 0, srcLength);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_پشته_Buffer[US_STACKBUF_SIZE];
  اگر((fUnion.fFields.fLengthAndFlags&k_Using_پشته_Buffer) && (newLength > US_STACKBUF_SIZE)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_پشته_Buffer, oldArray, oldLength);
    oldArray = old_پشته_Buffer;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *bufferToDelete = nullptr;
  اگر(!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength),
                         نادرست, &bufferToDelete)
  ) {
    بازگشت *این;
  }

  // now do the replace

  char16_t *newArray = getArrayStart();
  اگر(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + srcLength,
                 oldLength - (start + length));
  } وگرنه اگر(length != srcLength) {
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
  اگر (bufferToDelete) {
    uprv_free(bufferToDelete);
  }

  بازگشت *این;
}

UnicodeString&
UnicodeString::doReplace(int32_t start, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  اگر (!isWritable()) {
    بازگشت *این;
  }
  اگر (src.length() > INT32_MAX) {
    setToBogus();
    بازگشت *این;
  }
  بازگشت doReplace(start, length, src.data(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ثابت UnicodeString& src,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  اگر(srcLength == 0) {
    بازگشت *این;
  }

  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);
  بازگشت doAppend(src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doAppend(ثابت char16_t* srcChars,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  اگر(!isWritable() || srcLength == 0 || srcChars == nullptr) {
    بازگشت *این;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  اگر(srcLength < 0) {
    // get the srcLength if necessary
    اگر((srcLength = u_strlen(srcChars)) == 0) {
      بازگشت *این;
    }
  }

  int32_t oldLength = length();
  int32_t newLength;

  اگر (srcLength <= getCapacity() - oldLength && isBufferWritable()) {
    newLength = oldLength + srcLength;
    // Faster than a memmove
    اگر (srcLength <= 4) {
      char16_t *arr = getArrayStart();
      arr[oldLength] = srcChars[0];
      اگر (srcLength > 1) arr[oldLength+1] = srcChars[1];
      اگر (srcLength > 2) arr[oldLength+2] = srcChars[2];
      اگر (srcLength > 3) arr[oldLength+3] = srcChars[3];
      setLength(newLength);
      بازگشت *این;
    }
  } وگرنه {
    اگر (uprv_add32_overflow(oldLength, srcLength, &newLength)) {
      setToBogus();
      بازگشت *این;
    }

    // Check for append onto ourself
    ثابت char16_t* oldArray = getArrayStart();
    اگر (isBufferWritable() &&
        oldArray < srcChars + srcLength &&
        srcChars < oldArray + oldLength) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, srcLength);
      اگر (copy.isBogus()) {
        setToBogus();
        بازگشت *این;
      }
      بازگشت doAppend(copy.getArrayStart(), 0, srcLength);
    }

    // optimize append() onto a large-enough, owned string
    اگر (!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength))) {
      بازگشت *این;
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
  اگر(srcChars != newArray + oldLength) {
    us_arrayCopy(srcChars, 0, newArray, oldLength, srcLength);
  }
  setLength(newLength);

  بازگشت *این;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  اگر (!isWritable() || src.empty()) {
    بازگشت *این;
  }
  اگر (src.length() > INT32_MAX) {
    setToBogus();
    بازگشت *این;
  }
  بازگشت doAppend(src.data(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
تهی
UnicodeString::handleReplaceBetween(int32_t start,
                                    int32_t limit,
                                    ثابت UnicodeString& text) {
    replaceBetween(start, limit, text);
}

/**
 * Replaceable API
 */
تهی 
UnicodeString::copy(int32_t start, int32_t limit, int32_t dest) {
    اگر (limit <= start) {
        بازگشت; // Nothing to do; avoid bogus malloc call
    }
    char16_t* text = static_cast<char16_t*>(uprv_malloc(اندازه(char16_t) * (limit - start)));
    // Check to make sure text is not null.
    اگر (text != nullptr) {
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
UBool Replaceable::hasMetaData() ثابت {
    بازگشت درست;
}

/**
 * Replaceable API
 */
UBool UnicodeString::hasMetaData() ثابت {
    بازگشت نادرست;
}

UnicodeString&
UnicodeString::doReverse(int32_t start, int32_t length) U_LIFETIME_BOUND {
  اگر(length <= 1 || !cloneArrayIfNeeded()) {
    بازگشت *این;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  اگر(length <= 1) {  // pinIndices() might have shrunk the length
    بازگشت *این;
  }

  char16_t *left = getArrayStart() + start;
  char16_t *راست = left + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = نادرست;

  // Before the loop we know left<right because length>=2.
  انجامبده {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *left));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left++ = *راست));
    *راست-- = swap;
  } وقتی(left < راست);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  اگر(hasSupplementary) {
    char16_t swap2;

    left = getArrayStart() + start;
    راست = left + length - 1; // -1 so that we can look at *(left+1) if left<right
    وقتی(left < راست) {
      اگر(U16_IS_TRAIL(swap = *left) && U16_IS_LEAD(swap2 = *(left + 1))) {
        *left++ = swap2;
        *left++ = swap;
      } وگرنه {
        ++left;
      }
    }
  }

  بازگشت *این;
}

UBool 
UnicodeString::padLeading(int32_t targetLength,
                          char16_t padChar)
{
  int32_t oldLength = length();
  اگر(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    بازگشت نادرست;
  } وگرنه {
    // move contents up by padding width
    char16_t *array = getArrayStart();
    int32_t start = targetLength - oldLength;
    us_arrayCopy(array, 0, array, start, oldLength);

    // fill in padding character
    وقتی(--start >= 0) {
      array[start] = padChar;
    }
    setLength(targetLength);
    بازگشت درست;
  }
}

UBool 
UnicodeString::padTrailing(int32_t targetLength,
                           char16_t padChar)
{
  int32_t oldLength = length();
  اگر(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    بازگشت نادرست;
  } وگرنه {
    // fill in padding character
    char16_t *array = getArrayStart();
    int32_t length = targetLength;
    وقتی(--length >= oldLength) {
      array[length] = padChar;
    }
    setLength(targetLength);
    بازگشت درست;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::doHashCode() ثابت
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hashCode = ustr_hashUCharsN(getArrayStart(), length());
    اگر (hashCode == kInvalidHashCode) {
        hashCode = kEmptyHashCode;
    }
    بازگشت hashCode;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::getBuffer(int32_t minCapacity) U_LIFETIME_BOUND {
  اگر(minCapacity>=-1 && cloneArrayIfNeeded(minCapacity)) {
    fUnion.fFields.fLengthAndFlags|=kOpenGetBuffer;
    setZeroLength();
    بازگشت getArrayStart();
  } وگرنه {
    بازگشت nullptr;
  }
}

تهی
UnicodeString::releaseBuffer(int32_t newLength) {
  اگر(fUnion.fFields.fLengthAndFlags&kOpenGetBuffer && newLength>=-1) {
    // set the new fLength
    int32_t capacity=getCapacity();
    اگر(newLength==-1) {
      // the new length is the string length, capped by fCapacity
      ثابت char16_t *array=getArrayStart(), *p=array, *limit=array+capacity;
      وقتی(p<limit && *p!=0) {
        ++p;
      }
      newLength = static_cast<int32_t>(p - array);
    } وگرنه اگر(newLength>capacity) {
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
  اگر(newCapacity == -1) {
    newCapacity = getCapacity();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  اگر(!isWritable()) {
    بازگشت نادرست;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  اگر(forceClone ||
     fUnion.fFields.fLengthAndFlags & kBufferIsReadonly ||
     (fUnion.fFields.fLengthAndFlags & kRefCounted && ref_شمار() > 1) ||
     newCapacity > getCapacity()
  ) {
    // check growCapacity for default value and use of the stack buffer
    اگر(growCapacity < 0) {
      growCapacity = newCapacity;
    } وگرنه اگر(newCapacity <= US_STACKBUF_SIZE && growCapacity > US_STACKBUF_SIZE) {
      growCapacity = US_STACKBUF_SIZE;
    } وگرنه اگر(newCapacity > growCapacity) {
      setToBogus();
      بازگشت نادرست;  // bad inputs
    }
    اگر(growCapacity > kMaxCapacity) {
      setToBogus();
      بازگشت نادرست;
    }

    // save old values
    char16_t old_پشته_Buffer[US_STACKBUF_SIZE];
    char16_t *oldArray;
    int32_t oldLength = length();
    int16_t flags = fUnion.fFields.fLengthAndFlags;

    اگر(flags&k_Using_پشته_Buffer) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      اگر(doCopyArray && growCapacity > US_STACKBUF_SIZE) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_پشته_Fields.fBuffer, 0, old_پشته_Buffer, 0, oldLength);
        oldArray = old_پشته_Buffer;
      } وگرنه {
        oldArray = nullptr; // no need to copy from the stack buffer to itself
      }
    } وگرنه {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=nullptr); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    اگر(allocate(growCapacity) ||
       (newCapacity < growCapacity && allocate(newCapacity))
    ) {
      اگر(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t minLength = oldLength;
        newCapacity = getCapacity();
        اگر(newCapacity < minLength) {
          minLength = newCapacity;
        }
        اگر(oldArray != nullptr) {
          us_arrayCopy(oldArray, 0, getArrayStart(), 0, minLength);
        }
        setLength(minLength);
      } وگرنه {
        setZeroLength();
      }

      // release the old array
      اگر(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_شمار = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        اگر(umtx_atomic_dec(p_Ref_شمار) == 0) {
          اگر (pBufferToDelete == nullptr) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((تهی *)p_Ref_شمار);
          } وگرنه {
            // the caller requested to delete it himself
            *pBufferToDelete = reinterpret_cast<int32_t*>(p_Ref_شمار);
          }
        }
      }
    } وگرنه {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      اگر(!(flags&k_Using_پشته_Buffer)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.fLengthAndFlags = flags;
      setToBogus();
      بازگشت نادرست;
    }
  }
  بازگشت درست;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::appendCodeUnit(char16_t c) {
  بازگشت str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::appendCodePoint(UChar32 c) {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t cLength = 0;
  UBool isError = نادرست;
  U16_APPEND(buffer, cLength, U16_MAX_LENGTH, c, isError);
  بازگشت !isError && str.doAppend(buffer, 0, cLength).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ثابت char16_t *s, int32_t length) {
  بازگشت str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserveAppendCapacity(int32_t appendCapacity) {
  بازگشت str.cloneArrayIfNeeded(str.length() + appendCapacity);
}

char16_t* UnicodeStringAppendable::getAppendBuffer(int32_t minCapacity,
                                                   int32_t desiredCapacityHint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratchCapacity,
                                                   int32_t* resultCapacity) U_LIFETIME_BOUND {
  اگر(minCapacity < 1 || scratchCapacity < minCapacity) {
    *resultCapacity = 0;
    بازگشت nullptr;
  }
  int32_t oldLength = str.length();
  اگر(minCapacity <= (kMaxCapacity - oldLength) &&
      desiredCapacityHint <= (kMaxCapacity - oldLength) &&
      str.cloneArrayIfNeeded(oldLength + minCapacity, oldLength + desiredCapacityHint)) {
    *resultCapacity = str.getCapacity() - oldLength;
    بازگشت str.getArrayStart() + oldLength;
  }
  *resultCapacity = scratchCapacity;
  بازگشت scratch;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ثابت UElement کلید) {
    ثابت UnicodeString *str = (ثابت UnicodeString*) کلید.pointer;
    بازگشت (str == nullptr) ? 0 : str->hashCode();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.pointer;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.pointer;
    اگر (str1 == str2) {
        بازگشت درست;
    }
    اگر (str1 == nullptr || str2 == nullptr) {
        بازگشت نادرست;
    }
    بازگشت *str1 == *str2;
}

#ifdef U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#اگر defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
ایستا تهی uprv_UnicodeStringDummy() {
    delete [] (جدید UnicodeString[2]);
}
#pragma GCC diagnostic pop
#endif
#endif
