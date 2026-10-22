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

#समावेश <string_view>

#समावेश "unicode/utypes.h"
#समावेश "unicode/appendable.h"
#समावेश "unicode/putil.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/utf.h"
#समावेश "unicode/utf16.h"
#समावेश "uelement.h"
#समावेश "ustr_imp.h"
#समावेश "umutex.h"
#समावेश "uassert.h"

#अगर 0

#समावेश <iostream>
using namespace std;

//DEBUGGING
व्योम
print(स्थिर UnicodeString& s,
      स्थिर अक्षर *name)
{
  char16_t c;
  अ_बाहर << name << ":|";
  क्रम(पूर्णांक i = 0; i < s.length(); ++i) {
    c = s[i];
    अगर(c>= 0x007E || c < 0x0020)
      अ_बाहर << "[0x" << षष्ठ << s[i] << "]";
    अन्यथा
      अ_बाहर << (अक्षर) s[i];
  }
  अ_बाहर << '|' << प_पूर्ण;
}

व्योम
print(स्थिर char16_t *s,
      int32_t len,
      स्थिर अक्षर *name)
{
  char16_t c;
  अ_बाहर << name << ":|";
  क्रम(पूर्णांक i = 0; i < len; ++i) {
    c = s[i];
    अगर(c>= 0x007E || c < 0x0020)
      अ_बाहर << "[0x" << षष्ठ << s[i] << "]";
    अन्यथा
      अ_बाहर << (अक्षर) s[i];
  }
  अ_बाहर << '|' << प_पूर्ण;
}
// END DEBUGGING
#पूर्ण_अगर

// Local function definitions for now

// need to copy areas that may overlap
अटल
अंतरभूत व्योम
us_arrayCopy(स्थिर char16_t *src, int32_t srcStart,
         char16_t *dst, int32_t dstStart, int32_t count)
{
  अगर(count>0) {
    uprv_memmove(dst+dstStart, src+srcStart, (माप_प्रकार)count*माप(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
अटल char16_t U_CALLCONV
UnicodeString_charAt(int32_t offset, व्योम *context) {
    वापस ((icu::UnicodeString*) context)->charAt(offset);
}
U_CDECL_END

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
चालक+ (स्थिर UnicodeString &s1, स्थिर UnicodeString &s2) {
  int32_t योग_Lengths;
  अगर (uprv_add32_overflow(s1.length(), s2.length(), &योग_Lengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    वापस bogus;
  }
  अगर (योग_Lengths != INT32_MAX) {
    ++योग_Lengths;  // space for a terminating NUL if we need one
  }
  वापस UnicodeString(योग_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_internalConcat(स्थिर UnicodeString &s1, std::u16string_view s2) {
  int32_t योग_Lengths;
  अगर (s2.length() > INT32_MAX ||
      uprv_add32_overflow(s1.length(), static_cast<int32_t>(s2.length()), &योग_Lengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    वापस bogus;
  }
  अगर (योग_Lengths != INT32_MAX) {
    ++योग_Lengths;  // space for a terminating NUL if we need one
  }
  वापस UnicodeString(योग_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

व्योम
UnicodeString::addRef() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  वापस umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_गिनती() स्थिर {
  वापस umtx_loadAcquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

व्योम
UnicodeString::releaseArray() {
  अगर((fUnion.fFields.f_लंबाई_And_Flags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.f_लंबाई_And_Flags = 0;
  अगर (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } अन्यथा अगर(c <= 0xffff) {
    int32_t length = count;
    अगर(capacity < length) {
      capacity = length;
    }
    अगर(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t unit = static_cast<char16_t>(c);
      क्रम(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      set_लंबाई(length);
    }
  } अन्यथा {  // supplementary code point, write surrogate pairs
    अगर(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      वापस;
    }
    int32_t length = count * 2;
    अगर(capacity < length) {
      capacity = length;
    }
    अगर(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      क्रम(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      set_लंबाई(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.f_लंबाई_And_Flags = kLength1 | kShortString;
  fUnion.f_ढेर_Fields.f_बफर[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.f_लंबाई_And_Flags = kShortString;
  int32_t i = 0;
  UBool isError = false;
  U16_APPEND(fUnion.f_ढेर_Fields.f_बफर, i, US_STACKBUF_आकार, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  अगर(!isError) {
    set_Short_लंबाई(i);
  }
}

UnicodeString::UnicodeString(स्थिर char16_t *text,
                             int32_t text_लंबाई) {
  fUnion.fFields.f_लंबाई_And_Flags = kShortString;
  doAppend(text, 0, text_लंबाई);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                             int32_t text_लंबाई) {
  fUnion.fFields.f_लंबाई_And_Flags = kReadonlyAlias;
  स्थिर char16_t *text = textPtr;
  अगर(text == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } अन्यथा अगर(text_लंबाई < -1 ||
            (text_लंबाई == -1 && !isTerminated) ||
            (text_लंबाई >= 0 && isTerminated && text[text_लंबाई] != 0)
  ) {
    setToBogus();
  } अन्यथा {
    अगर(text_लंबाई == -1) {
      // text is terminated, or else it would have failed the above test
      text_लंबाई = u_strlen(text);
    }
    setArray(const_cast<char16_t *>(text), text_लंबाई,
             isTerminated ? text_लंबाई + 1 : text_लंबाई);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buff_लंबाई,
                             int32_t buffCapacity) {
  fUnion.fFields.f_लंबाई_And_Flags = kWritableAlias;
  अगर(buff == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } अन्यथा अगर(buff_लंबाई < -1 || buffCapacity < 0 || buff_लंबाई > buffCapacity) {
    setToBogus();
  } अन्यथा {
    अगर(buff_लंबाई == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      स्थिर char16_t *p = buff, *limit = buff + buffCapacity;
      जबतक(p != limit && *p != 0) {
        ++p;
      }
      buff_लंबाई = static_cast<int32_t>(p - buff);
    }
    setArray(buff, buff_लंबाई, buffCapacity);
  }
}

UnicodeString::UnicodeString(स्थिर अक्षर *src, int32_t length, EInvariant) {
  fUnion.fFields.f_लंबाई_And_Flags = kShortString;
  अगर(src==nullptr) {
    // treat as an empty string
  } अन्यथा {
    अगर(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    अगर(cloneArrayIfNeeded(length, length, false)) {
      u_charsToUChars(src, getArrayStart(), length);
      set_लंबाई(length);
    } अन्यथा {
      setToBogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view text U_LIFETIME_BOUND) {
  UnicodeString परिणाम;
  अगर (text.length() <= INT32_MAX) {
    परिणाम.setTo(false, text.तथ्य(), static_cast<int32_t>(text.length()));
  } अन्यथा {
    परिणाम.setToBogus();
  }
  वापस परिणाम;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(स्थिर UnicodeString& text U_LIFETIME_BOUND) {
  UnicodeString परिणाम;
  अगर (text.isBogus()) {
    परिणाम.setToBogus();
  } अन्यथा {
    परिणाम.setTo(false, text.get_बफर(), text.length());
  }
  वापस परिणाम;
}

#अगर U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(स्थिर अक्षर *codepage_तथ्य) {
  fUnion.fFields.f_लंबाई_And_Flags = kShortString;
  अगर (codepage_तथ्य != nullptr) {
    setToUTF8(codepage_तथ्य);
  }
}

UnicodeString::UnicodeString(स्थिर अक्षर *codepage_तथ्य, int32_t तथ्य_लंबाई) {
  fUnion.fFields.f_लंबाई_And_Flags = kShortString;
  // if there's nothing to convert, do nothing
  अगर (codepage_तथ्य == nullptr || तथ्य_लंबाई == 0 || तथ्य_लंबाई < -1) {
    वापस;
  }
  अगर(तथ्य_लंबाई == -1) {
    तथ्य_लंबाई = static_cast<int32_t>(uprv_strlen(codepage_तथ्य));
  }
  setToUTF8(StringPiece(codepage_तथ्य, तथ्य_लंबाई));
}

// else see unistr_cnv.cpp
#पूर्ण_अगर

UnicodeString::UnicodeString(स्थिर UnicodeString& that) {
  fUnion.fFields.f_लंबाई_And_Flags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, true);
}

UnicodeString::UnicodeString(स्थिर UnicodeString& that,
                             int32_t srcStart) {
  fUnion.fFields.f_लंबाई_And_Flags = kShortString;
  setTo(that, srcStart);
}

UnicodeString::UnicodeString(स्थिर UnicodeString& that,
                             int32_t srcStart,
                             int32_t src_लंबाई) {
  fUnion.fFields.f_लंबाई_And_Flags = kShortString;
  setTo(that, srcStart, src_लंबाई);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() स्थिर {
  वापस nullptr;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() स्थिर {
  LocalPointer<UnicodeString> clonedString(नया UnicodeString(*यह));
  वापस clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : nullptr;
}

//========================================
// array allocation
//========================================

namespace {

स्थिर int32_t k_Grow_आकार = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
स्थिर int32_t kMaxCapacity = 0x7ffffff5;

int32_t getGrowCapacity(int32_t new_लंबाई) {
  int32_t grow_आकार = (new_लंबाई >> 2) + k_Grow_आकार;
  अगर(grow_आकार <= (kMaxCapacity - new_लंबाई)) {
    वापस new_लंबाई + grow_आकार;
  } अन्यथा {
    वापस kMaxCapacity;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  अगर(capacity <= US_STACKBUF_आकार) {
    fUnion.fFields.f_लंबाई_And_Flags = kShortString;
    वापस true;
  }
  अगर(capacity <= kMaxCapacity) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    माप_प्रकार अंक_Bytes = माप(int32_t) + static_cast<माप_प्रकार>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    अंक_Bytes = (अंक_Bytes + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(अंक_Bytes));
    अगर(array != nullptr) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      अंक_Bytes -= माप(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.fCapacity = static_cast<int32_t>(अंक_Bytes / U_SIZEOF_UCHAR);
      fUnion.fFields.f_लंबाई_And_Flags = kLongString;
      वापस true;
    }
  }
  fUnion.fFields.f_लंबाई_And_Flags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
  वापस false;
}

//========================================
// Destructor
//========================================

#अगर_घोषित UNISTR_गिनती_FINAL_STRING_LENGTHS
अटल u_atomic_int32_t final_लंबाई_Counts[0x400];  // UnicodeString::kMaxShortLength+1
अटल u_atomic_int32_t beyond_गिनती(0);

U_CAPI व्योम unistr_printLengths() {
  int32_t i;
  क्रम(i = 0; i <= 59; ++i) {
    म_लिखो("%2d,  %9d\n", i, (int32_t)final_लंबाई_Counts[i]);
  }
  int32_t beyond = beyond_गिनती;
  क्रम(; i < UPRV_LENGTHOF(final_लंबाई_Counts); ++i) {
    beyond += final_लंबाई_Counts[i];
  }
  म_लिखो(">59, %9d\n", beyond);
}
#पूर्ण_अगर

UnicodeString::~UnicodeString()
{
#अगर_घोषित UNISTR_गिनती_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  अगर((fUnion.fFields.f_लंबाई_And_Flags&(k_Open_Get_बफर|kReadonlyAlias|kWritableAlias)) == 0) {
    अगर(has_Short_लंबाई()) {
      umtx_atomic_inc(final_लंबाई_Counts + get_Short_लंबाई());
    } अन्यथा {
      umtx_atomic_inc(&beyond_गिनती);
    }
  }
#पूर्ण_अगर

  releaseArray();
}

//========================================
// Factory methods
//========================================

UnicodeString UnicodeString::fromUTF8(StringPiece utf8) {
  UnicodeString परिणाम;
  परिणाम.setToUTF8(utf8);
  वापस परिणाम;
}

UnicodeString UnicodeString::fromUTF32(स्थिर UChar32 *utf32, int32_t length) {
  UnicodeString परिणाम;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  अगर(length <= US_STACKBUF_आकार) {
    capacity = US_STACKBUF_आकार;
  } अन्यथा {
    capacity = length + (length >> 4) + 4;
  }
  करो {
    char16_t *utf16 = परिणाम.get_बफर(capacity);
    int32_t length16;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, परिणाम.getCapacity(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
    परिणाम.release_बफर(length16);
    अगर(errorCode == U_बफर_OVERFLOW_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      जारी;
    } अन्यथा अगर(U_FAILURE(errorCode)) {
      परिणाम.setToBogus();
    }
    अवरोध;
  } जबतक(true);
  वापस परिणाम;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::चालक=(स्थिर UnicodeString& src) U_LIFETIME_BOUND {
  वापस copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(स्थिर UnicodeString& src) U_LIFETIME_BOUND {
  वापस copyFrom(src, true);
}

UnicodeString&
UnicodeString::copyFrom(स्थिर UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  अगर(यह == &src) {
    वापस *यह;
  }

  // is the right side bogus?
  अगर(src.isBogus()) {
    setToBogus();
    वापस *यह;
  }

  // delete the current contents
  releaseArray();

  अगर(src.isEmpty()) {
    // empty string - use the stack buffer
    setToEmpty();
    वापस *यह;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.f_लंबाई_And_Flags = src.fUnion.fFields.f_लंबाई_And_Flags;
  चयन(src.fUnion.fFields.f_लंबाई_And_Flags & kAllStorageFlags) {
  हाल kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_ढेर_Fields.f_बफर, src.fUnion.f_ढेर_Fields.f_बफर,
                get_Short_लंबाई() * U_SIZEOF_UCHAR);
    अवरोध;
  हाल kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).addRef();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    अगर(!has_Short_लंबाई()) {
      fUnion.fFields.f_लंबाई = src.fUnion.fFields.f_लंबाई;
    }
    अवरोध;
  हाल kReadonlyAlias:
    अगर(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
      अगर(!has_Short_लंबाई()) {
        fUnion.fFields.f_लंबाई = src.fUnion.fFields.f_लंबाई;
      }
      अवरोध;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  हाल kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t src_लंबाई = src.length();
    अगर(allocate(src_लंबाई)) {
      u_memcpy(getArrayStart(), src.getArrayStart(), src_लंबाई);
      set_लंबाई(src_लंबाई);
      अवरोध;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  शेष:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.f_लंबाई_And_Flags = kIsBogus;
    fUnion.fFields.fArray = nullptr;
    fUnion.fFields.fCapacity = 0;
    अवरोध;
  }

  वापस *यह;
}

UnicodeString& UnicodeString::चालक=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, true);
  वापस *यह;
}

// Same as move assignment except without memory management.
व्योम UnicodeString::copyFieldsFrom(UnicodeString &src, UBool setSrcToBogus) noexcept {
  int16_t लंबाई_And_Flags = fUnion.fFields.f_लंबाई_And_Flags = src.fUnion.fFields.f_लंबाई_And_Flags;
  अगर(लंबाई_And_Flags & k_Using_ढेर_बफर) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    अगर(यह != &src) {
      uprv_memcpy(fUnion.f_ढेर_Fields.f_बफर, src.fUnion.f_ढेर_Fields.f_बफर,
                  get_Short_लंबाई() * U_SIZEOF_UCHAR);
    }
  } अन्यथा {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    अगर(!has_Short_लंबाई()) {
      fUnion.fFields.f_लंबाई = src.fUnion.fFields.f_लंबाई;
    }
    अगर(setSrcToBogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.f_लंबाई_And_Flags = kIsBogus;
      src.fUnion.fFields.fArray = nullptr;
      src.fUnion.fFields.fCapacity = 0;
    }
  }
}

व्योम UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString अस्थायी;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  अस्थायी.copyFieldsFrom(*यह, false);
  यह->copyFieldsFrom(other, false);
  other.copyFieldsFrom(अस्थायी, false);
  // Set temp to an empty string so that other's memory is not released twice.
  अस्थायी.fUnion.fFields.f_लंबाई_And_Flags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() स्थिर {
    UnicodeString परिणाम(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    अगर (परिणाम.isBogus()) {
        वापस परिणाम;
    }
    स्थिर char16_t *array = get_बफर();
    int32_t len = length();
    int32_t पूर्व = 0;
    क्रम (int32_t i=0;;) {
        अगर (i == len) {
            परिणाम.append(array, पूर्व, len - पूर्व);
            अवरोध;
        }
        अगर (array[i++] == 0x5C /*'\\'*/) {
            परिणाम.append(array, पूर्व, (i - 1) - पूर्व);
            UChar32 c = unescapeAt(i); // advances i
            अगर (c < 0) {
                परिणाम.हटाओ(); // return empty string
                अवरोध; // invalid escape sequence
            }
            परिणाम.append(c);
            पूर्व = i;
        }
    }
    वापस परिणाम;
}

UChar32 UnicodeString::unescapeAt(int32_t &offset) स्थिर {
    वापस u_unescapeAt(UnicodeString_charAt, &offset, length(), (व्योम*)यह);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(स्थिर char16_t *text, int32_t len) स्थिर {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  वापस uprv_memcmp(getArrayStart(), text, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t start,
              int32_t length,
              स्थिर char16_t *srcChars,
              int32_t srcStart,
              int32_t src_लंबाई) स्थिर
{
  // compare illegal string values
  अगर(isBogus()) {
    वापस false;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  अगर(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    वापस length == 0 ? true : false;
  }

  // get the correct pointer
  स्थिर char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  // get the srcLength if necessary
  अगर(src_लंबाई < 0) {
    src_लंबाई = u_strlen(srcChars + srcStart);
  }

  अगर (length != src_लंबाई) {
    वापस false;
  }

  अगर(length == 0 || chars == srcChars) {
    वापस true;
  }

  वापस u_memcmp(chars, srcChars, src_लंबाई) == 0;
}

int8_t
UnicodeString::doCompare( int32_t start,
              int32_t length,
              स्थिर char16_t *srcChars,
              int32_t srcStart,
              int32_t src_लंबाई) स्थिर
{
  // compare illegal string values
  अगर(isBogus()) {
    वापस -1;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  अगर(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    वापस length == 0 ? 0 : 1;
  }

  // get the correct pointer
  स्थिर char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  int32_t min_लंबाई;
  int8_t लंबाई_परिणाम;

  // get the srcLength if necessary
  अगर(src_लंबाई < 0) {
    src_लंबाई = u_strlen(srcChars + srcStart);
  }

  // are we comparing different lengths?
  अगर(length != src_लंबाई) {
    अगर(length < src_लंबाई) {
      min_लंबाई = length;
      लंबाई_परिणाम = -1;
    } अन्यथा {
      min_लंबाई = src_लंबाई;
      लंबाई_परिणाम = 1;
    }
  } अन्यथा {
    min_लंबाई = length;
    लंबाई_परिणाम = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  अगर(min_लंबाई > 0 && chars != srcChars) {
    int32_t परिणाम;

#   अगर U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      परिणाम = uprv_memcmp(chars, srcChars, min_लंबाई * माप(char16_t));
      अगर(परिणाम != 0) {
        वापस (int8_t)(परिणाम >> 15 | 1);
      }
#   अन्यथा
      // little-endian: compare char16_t units
      करो {
        परिणाम = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        अगर(परिणाम != 0) {
          वापस static_cast<int8_t>(परिणाम >> 15 | 1);
        }
      } जबतक(--min_लंबाई > 0);
#   endif
  }
  वापस लंबाई_परिणाम;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::doCompareCodePointOrder(int32_t start,
                                       int32_t length,
                                       स्थिर char16_t *srcChars,
                                       int32_t srcStart,
                                       int32_t src_लंबाई) स्थिर
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  अगर(isBogus()) {
    वापस -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  अगर(srcChars == nullptr) {
    srcStart = src_लंबाई = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=nullptr)?(srcChars + srcStart):nullptr, src_लंबाई, false, true);
  /* translate the 32-bit result into an 8-bit one */
  अगर(diff!=0) {
    वापस static_cast<int8_t>(diff >> 15 | 1);
  } अन्यथा {
    वापस 0;
  }
}

int32_t
UnicodeString::get_लंबाई() स्थिर {
    वापस length();
}

char16_t
UnicodeString::getCharAt(int32_t offset) स्थिर {
  वापस charAt(offset);
}

UChar32
UnicodeString::getChar32At(int32_t offset) स्थिर {
  वापस char32At(offset);
}

UChar32
UnicodeString::char32At(int32_t offset) स्थिर
{
  int32_t len = length();
  अगर (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    स्थिर char16_t *array = getArrayStart();
    UChar32 c;
    U16_GET(array, 0, offset, len, c);
    वापस c;
  } अन्यथा {
    वापस kInvalidUChar;
  }
}

int32_t
UnicodeString::getChar32Start(int32_t offset) स्थिर {
  अगर (static_cast<uint32_t>(offset) < static_cast<uint32_t>(length())) {
    स्थिर char16_t *array = getArrayStart();
    U16_SET_CP_START(array, 0, offset);
    वापस offset;
  } अन्यथा {
    वापस 0;
  }
}

int32_t
UnicodeString::getChar32Limit(int32_t offset) स्थिर {
  int32_t len = length();
  अगर (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    स्थिर char16_t *array = getArrayStart();
    U16_SET_CP_LIMIT(array, 0, offset, len);
    वापस offset;
  } अन्यथा {
    वापस len;
  }
}

int32_t
UnicodeString::गिनती_Char32(int32_t start, int32_t length) स्थिर {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  वापस u_गिनती_Char32(getArrayStart()+start, length);
}

UBool
UnicodeString::hasMoreChar32Than(int32_t start, int32_t length, int32_t number) स्थिर {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  वापस u_strHasMoreChar32Than(getArrayStart()+start, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t सूचकांक, int32_t delta) स्थिर {
  // pin index
  int32_t len = length();
  अगर(सूचकांक<0) {
    सूचकांक=0;
  } अन्यथा अगर(सूचकांक>len) {
    सूचकांक=len;
  }

  स्थिर char16_t *array = getArrayStart();
  अगर(delta>0) {
    U16_FWD_N(array, सूचकांक, len, delta);
  } अन्यथा {
    U16_BACK_N(array, 0, सूचकांक, -delta);
  }

  वापस सूचकांक;
}

व्योम
UnicodeString::doExtract(int32_t start,
             int32_t length,
             char16_t *dst,
             int32_t dstStart) स्थिर
{
  // pin indices to legal values
  pinIndices(start, length);

  // do not copy anything if we alias dst itself
  स्थिर char16_t *array = getArrayStart();
  अगर(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t destCapacity,
                       UErrorCode &errorCode) स्थिर {
  int32_t len = length();
  अगर(U_SUCCESS(errorCode)) {
    अगर (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
      errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } अन्यथा {
      स्थिर char16_t *array = getArrayStart();
      अगर(len>0 && len<=destCapacity && array!=dest) {
        u_memcpy(dest, array, len);
      }
      वापस u_terminateUChars(dest, destCapacity, len, &errorCode);
    }
  }

  वापस len;
}

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       अक्षर *target,
                       int32_t targetCapacity,
                       क्रमागत EInvariant) स्थिर
{
  // if the arguments are illegal, then do nothing
  अगर(targetCapacity < 0 || (targetCapacity > 0 && target == nullptr)) {
    वापस 0;
  }

  // pin the indices to legal values
  pinIndices(start, length);

  अगर(length <= targetCapacity) {
    u_UCharsToChars(getArrayStart() + start, target, length);
  }
  UErrorCode status = U_ZERO_ERROR;
  वापस u_terminateChars(target, targetCapacity, length, &status);
}

UnicodeString
UnicodeString::अस्थायी_Sub_String(int32_t start, int32_t len) स्थिर U_LIFETIME_BOUND {
  pinIndices(start, len);
  स्थिर char16_t *array = get_बफर();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  अगर(array==nullptr) {
    array=fUnion.f_ढेर_Fields.f_बफर;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  वापस UnicodeString(false, array + start, len);
}

int32_t
UnicodeString::toUTF8(int32_t start, int32_t len,
                      अक्षर *target, int32_t capacity) स्थिर {
  pinIndices(start, len);
  int32_t length8;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strToUTF8WithSub(target, capacity, &length8,
                     get_बफर() + start, len,
                     0xFFFD,  // Standard substitution character.
                     nullptr,    // Don't care about number of substitutions.
                     &errorCode);
  वापस length8;
}

#अगर U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start, int32_t len,
                       अक्षर *target, uint32_t dst_आकार) स्थिर {
  // if the arguments are illegal, then do nothing
  अगर (/*dstSize < 0 || */(dst_आकार > 0 && target == nullptr)) {
    वापस 0;
  }
  वापस toUTF8(start, len, target, dst_आकार <= 0x7fffffff ? static_cast<int32_t>(dst_आकार) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#पूर्ण_अगर

व्योम 
UnicodeString::extractBetween(int32_t start,
                  int32_t limit,
                  UnicodeString& target) स्थिर {
  pin_सूचकांक(start);
  pin_सूचकांक(limit);
  doExtract(start, limit - start, target);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
व्योम
UnicodeString::toUTF8(ByteSink &sink) स्थिर {
  int32_t length16 = length();
  अगर(length16 != 0) {
    अक्षर ढेर_बफर[1024];
    int32_t capacity = static_cast<int32_t>(माप(ढेर_बफर));
    UBool utf8IsOwned = false;
    अक्षर *utf8 = sink.Get_Append_बफर(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      ढेर_बफर, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8WithSub(utf8, capacity, &length8,
                       get_बफर(), length16,
                       0xFFFD,  // Standard substitution character.
                       nullptr,    // Don't care about number of substitutions.
                       &errorCode);
    अगर(errorCode == U_बफर_OVERFLOW_ERROR) {
      utf8 = static_cast<अक्षर*>(uprv_malloc(length8));
      अगर(utf8 != nullptr) {
        utf8IsOwned = true;
        errorCode = U_ZERO_ERROR;
        u_strToUTF8WithSub(utf8, length8, &length8,
                           get_बफर(), length16,
                           0xFFFD,  // Standard substitution character.
                           nullptr,    // Don't care about number of substitutions.
                           &errorCode);
      } अन्यथा {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
      }
    }
    अगर(U_SUCCESS(errorCode)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    अगर(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::toUTF32(UChar32 *utf32, int32_t capacity, UErrorCode &errorCode) स्थिर {
  int32_t length32=0;
  अगर(U_SUCCESS(errorCode)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_strToUTF32WithSub(utf32, capacity, &length32,
        get_बफर(), length(),
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
  }
  वापस length32;
}

int32_t 
UnicodeString::सूचकांक_Of(स्थिर char16_t *srcChars,
               int32_t srcStart,
               int32_t src_लंबाई,
               int32_t start,
               int32_t length) स्थिर
{
  अगर (isBogus() || srcChars == nullptr || srcStart < 0 || src_लंबाई == 0) {
    वापस -1;
  }

  // UnicodeString does not find empty substrings
  अगर(src_लंबाई < 0 && srcChars[srcStart] == 0) {
    वापस -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  स्थिर char16_t *array = getArrayStart();
  स्थिर char16_t *match = u_strFindFirst(array + start, length, srcChars + srcStart, src_लंबाई);
  अगर(match == nullptr) {
    वापस -1;
  } अन्यथा {
    वापस static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::do_सूचकांक_Of(char16_t c,
             int32_t start,
             int32_t length) स्थिर
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  स्थिर char16_t *array = getArrayStart();
  स्थिर char16_t *match = u_memchr(array + start, c, length);
  अगर(match == nullptr) {
    वापस -1;
  } अन्यथा {
    वापस static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::do_सूचकांक_Of(UChar32 c,
                         int32_t start,
                         int32_t length) स्थिर {
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  स्थिर char16_t *array = getArrayStart();
  स्थिर char16_t *match = u_memchr32(array + start, c, length);
  अगर(match == nullptr) {
    वापस -1;
  } अन्यथा {
    वापस static_cast<int32_t>(match - array);
  }
}

int32_t 
UnicodeString::last_सूचकांक_Of(स्थिर char16_t *srcChars,
               int32_t srcStart,
               int32_t src_लंबाई,
               int32_t start,
               int32_t length) स्थिर
{
  अगर (isBogus() || srcChars == nullptr || srcStart < 0 || src_लंबाई == 0) {
    वापस -1;
  }

  // UnicodeString does not find empty substrings
  अगर(src_लंबाई < 0 && srcChars[srcStart] == 0) {
    वापस -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the last occurrence of the substring
  स्थिर char16_t *array = getArrayStart();
  स्थिर char16_t *match = u_strFindLast(array + start, length, srcChars + srcStart, src_लंबाई);
  अगर(match == nullptr) {
    वापस -1;
  } अन्यथा {
    वापस static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::do_Last_सूचकांक_Of(char16_t c,
                 int32_t start,
                 int32_t length) स्थिर
{
  अगर(isBogus()) {
    वापस -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  स्थिर char16_t *array = getArrayStart();
  स्थिर char16_t *match = u_memrchr(array + start, c, length);
  अगर(match == nullptr) {
    वापस -1;
  } अन्यथा {
    वापस static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::do_Last_सूचकांक_Of(UChar32 c,
                             int32_t start,
                             int32_t length) स्थिर {
  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  स्थिर char16_t *array = getArrayStart();
  स्थिर char16_t *match = u_memrchr32(array + start, c, length);
  अगर(match == nullptr) {
    वापस -1;
  } अन्यथा {
    वापस static_cast<int32_t>(match - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::findAndReplace(int32_t start,
                              int32_t length,
                              स्थिर UnicodeString& oldText,
                              int32_t oldStart,
                              int32_t old_लंबाई,
                              स्थिर UnicodeString& newText,
                              int32_t newStart,
                              int32_t new_लंबाई) U_LIFETIME_BOUND
{
  अगर(isBogus() || oldText.isBogus() || newText.isBogus()) {
    वापस *यह;
  }

  pinIndices(start, length);
  oldText.pinIndices(oldStart, old_लंबाई);
  newText.pinIndices(newStart, new_लंबाई);

  अगर(old_लंबाई == 0) {
    वापस *यह;
  }

  जबतक(length > 0 && length >= old_लंबाई) {
    int32_t pos = सूचकांक_Of(oldText, oldStart, old_लंबाई, start, length);
    अगर(pos < 0) {
      // no more oldText's here: done
      अवरोध;
    } अन्यथा {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, old_लंबाई, newText, newStart, new_लंबाई);
      length -= pos + old_लंबाई - start;
      start = pos + new_लंबाई;
    }
  }

  वापस *यह;
}


व्योम
UnicodeString::setToBogus()
{
  releaseArray();

  fUnion.fFields.f_लंबाई_And_Flags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
}

// turn a bogus string into an empty one
व्योम
UnicodeString::unBogus() {
  अगर(fUnion.fFields.f_लंबाई_And_Flags & kIsBogus) {
    setToEmpty();
  }
}

स्थिर char16_t*
UnicodeString::get_Terminated_बफर() U_LIFETIME_BOUND {
  अगर(!isWritable()) {
    वापस nullptr;
  }
  char16_t *array = getArrayStart();
  int32_t len = length();
  अगर(len < getCapacity()) {
    अगर(fUnion.fFields.f_लंबाई_And_Flags & k_बफर_Is_Readonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      अगर(array[len] == 0) {
        वापस array;
      }
    } अन्यथा अगर(((fUnion.fFields.f_लंबाई_And_Flags & kRefCounted) == 0 || ref_गिनती() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      वापस array;
    }
  }
  अगर(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = getArrayStart();
    array[len] = 0;
    वापस array;
  } अन्यथा {
    वापस nullptr;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(UBool isTerminated,
                     ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                     int32_t text_लंबाई) U_LIFETIME_BOUND
{
  अगर(fUnion.fFields.f_लंबाई_And_Flags & k_Open_Get_बफर) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    वापस *यह;
  }

  स्थिर char16_t *text = textPtr;
  अगर(text == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    वापस *यह;
  }

  अगर( text_लंबाई < -1 ||
      (text_लंबाई == -1 && !isTerminated) ||
      (text_लंबाई >= 0 && isTerminated && text[text_लंबाई] != 0)
  ) {
    setToBogus();
    वापस *यह;
  }

  releaseArray();

  अगर(text_लंबाई == -1) {
    // text is terminated, or else it would have failed the above test
    text_लंबाई = u_strlen(text);
  }
  fUnion.fFields.f_लंबाई_And_Flags = kReadonlyAlias;
  setArray(const_cast<char16_t*>(text), text_लंबाई, isTerminated ? text_लंबाई + 1 : text_लंबाई);
  वापस *यह;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(char16_t* बफर,
                     int32_t buff_लंबाई U_LIFETIME_BOUND,
                     int32_t buffCapacity) U_LIFETIME_BOUND {
  अगर(fUnion.fFields.f_लंबाई_And_Flags & k_Open_Get_बफर) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    वापस *यह;
  }

  अगर(बफर == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    वापस *यह;
  }

  अगर(buff_लंबाई < -1 || buffCapacity < 0 || buff_लंबाई > buffCapacity) {
    setToBogus();
    वापस *यह;
  } अन्यथा अगर(buff_लंबाई == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    स्थिर char16_t *p = बफर, *limit = बफर + buffCapacity;
    जबतक(p != limit && *p != 0) {
      ++p;
    }
    buff_लंबाई = static_cast<int32_t>(p - बफर);
  }

  releaseArray();

  fUnion.fFields.f_लंबाई_And_Flags = kWritableAlias;
  setArray(बफर, buff_लंबाई, buffCapacity);
  वापस *यह;
}

UnicodeString& UnicodeString::setToUTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  अगर(length <= US_STACKBUF_आकार) {
    capacity = US_STACKBUF_आकार;
  } अन्यथा {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = get_बफर(capacity);
  int32_t length16;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, getCapacity(), &length16,
      utf8.तथ्य(), length,
      0xfffd,  // Substitution character.
      nullptr,    // Don't care about number of substitutions.
      &errorCode);
  release_बफर(length16);
  अगर(U_FAILURE(errorCode)) {
    setToBogus();
  }
  वापस *यह;
}

UnicodeString&
UnicodeString::setCharAt(int32_t offset, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  अगर(cloneArrayIfNeeded() && len > 0) {
    अगर(offset < 0) {
      offset = 0;
    } अन्यथा अगर(offset >= len) {
      offset = len - 1;
    }

    getArrayStart()[offset] = c;
  }
  वापस *यह;
}

UnicodeString&
UnicodeString::replace(int32_t start,
                       int32_t लंबाई_4,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t बफर[U16_MAX_लंबाई];
  int32_t count = 0;
  UBool isError = false;
  U16_APPEND(बफर, count, U16_MAX_लंबाई, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  वापस doReplace(start, लंबाई_4, बफर, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t बफर[U16_MAX_लंबाई];
  int32_t लंबाई_4 = 0;
  UBool isError = false;
  U16_APPEND(बफर, लंबाई_4, U16_MAX_लंबाई, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  वापस isError ? *यह : doAppend(बफर, 0, लंबाई_4);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         स्थिर UnicodeString& src,
                         int32_t srcStart,
                         int32_t src_लंबाई) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(srcStart, src_लंबाई);

  // get the characters from src
  // and replace the range in ourselves with them
  वापस doReplace(start, length, src.getArrayStart(), srcStart, src_लंबाई);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         स्थिर char16_t* srcChars,
                         int32_t srcStart,
                         int32_t src_लंबाई) U_LIFETIME_BOUND
{
  अगर(!isWritable()) {
    वापस *यह;
  }

  int32_t old_लंबाई = यह->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  अगर((fUnion.fFields.f_लंबाई_And_Flags&k_बफर_Is_Readonly) && src_लंबाई == 0) {
    अगर(start == 0) {
      // remove prefix by adjusting the array pointer
      pin_सूचकांक(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.fCapacity -= length;
      set_लंबाई(old_लंबाई - length);
      वापस *यह;
    } अन्यथा {
      pin_सूचकांक(start);
      अगर(length >= (old_लंबाई - start)) {
        // remove suffix by reducing the length (like truncate())
        set_लंबाई(start);
        fUnion.fFields.fCapacity = start;  // not NUL-terminated any more
        वापस *यह;
      }
    }
  }

  अगर(start == old_लंबाई) {
    वापस doAppend(srcChars, srcStart, src_लंबाई);
  }

  अगर (srcChars == nullptr) {
    src_लंबाई = 0;
  } अन्यथा {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    अगर (src_लंबाई < 0) {
      // get the srcLength if necessary
      src_लंबाई = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t new_लंबाई = old_लंबाई - length;
  अगर(src_लंबाई > (INT32_MAX - new_लंबाई)) {
    setToBogus();
    वापस *यह;
  }
  new_लंबाई += src_लंबाई;

  // Check for insertion into ourself
  स्थिर char16_t *oldArray = getArrayStart();
  अगर (is_बफर_Writable() &&
      oldArray < srcChars + src_लंबाई &&
      srcChars < oldArray + old_लंबाई) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, src_लंबाई);
    अगर (copy.isBogus()) {
      setToBogus();
      वापस *यह;
    }
    वापस doReplace(start, length, copy.getArrayStart(), 0, src_लंबाई);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_ढेर_बफर[US_STACKBUF_आकार];
  अगर((fUnion.fFields.f_लंबाई_And_Flags&k_Using_ढेर_बफर) && (new_लंबाई > US_STACKBUF_आकार)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_ढेर_बफर, oldArray, old_लंबाई);
    oldArray = old_ढेर_बफर;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *बफर_To_Delete = nullptr;
  अगर(!cloneArrayIfNeeded(new_लंबाई, getGrowCapacity(new_लंबाई),
                         false, &बफर_To_Delete)
  ) {
    वापस *यह;
  }

  // now do the replace

  char16_t *newArray = getArrayStart();
  अगर(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + src_लंबाई,
                 old_लंबाई - (start + length));
  } अन्यथा अगर(length != src_लंबाई) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, start + length,
                 newArray, start + src_लंबाई,
                 old_लंबाई - (start + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, start, src_लंबाई);

  set_लंबाई(new_लंबाई);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  अगर (बफर_To_Delete) {
    uprv_free(बफर_To_Delete);
  }

  वापस *यह;
}

UnicodeString&
UnicodeString::doReplace(int32_t start, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  अगर (!isWritable()) {
    वापस *यह;
  }
  अगर (src.length() > INT32_MAX) {
    setToBogus();
    वापस *यह;
  }
  वापस doReplace(start, length, src.तथ्य(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(स्थिर UnicodeString& src,
                        int32_t srcStart,
                        int32_t src_लंबाई) U_LIFETIME_BOUND {
  अगर(src_लंबाई == 0) {
    वापस *यह;
  }

  // pin the indices to legal values
  src.pinIndices(srcStart, src_लंबाई);
  वापस doAppend(src.getArrayStart(), srcStart, src_लंबाई);
}

UnicodeString&
UnicodeString::doAppend(स्थिर char16_t* srcChars,
                        int32_t srcStart,
                        int32_t src_लंबाई) U_LIFETIME_BOUND {
  अगर(!isWritable() || src_लंबाई == 0 || srcChars == nullptr) {
    वापस *यह;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  अगर(src_लंबाई < 0) {
    // get the srcLength if necessary
    अगर((src_लंबाई = u_strlen(srcChars)) == 0) {
      वापस *यह;
    }
  }

  int32_t old_लंबाई = length();
  int32_t new_लंबाई;

  अगर (src_लंबाई <= getCapacity() - old_लंबाई && is_बफर_Writable()) {
    new_लंबाई = old_लंबाई + src_लंबाई;
    // Faster than a memmove
    अगर (src_लंबाई <= 4) {
      char16_t *arr = getArrayStart();
      arr[old_लंबाई] = srcChars[0];
      अगर (src_लंबाई > 1) arr[old_लंबाई+1] = srcChars[1];
      अगर (src_लंबाई > 2) arr[old_लंबाई+2] = srcChars[2];
      अगर (src_लंबाई > 3) arr[old_लंबाई+3] = srcChars[3];
      set_लंबाई(new_लंबाई);
      वापस *यह;
    }
  } अन्यथा {
    अगर (uprv_add32_overflow(old_लंबाई, src_लंबाई, &new_लंबाई)) {
      setToBogus();
      वापस *यह;
    }

    // Check for append onto ourself
    स्थिर char16_t* oldArray = getArrayStart();
    अगर (is_बफर_Writable() &&
        oldArray < srcChars + src_लंबाई &&
        srcChars < oldArray + old_लंबाई) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, src_लंबाई);
      अगर (copy.isBogus()) {
        setToBogus();
        वापस *यह;
      }
      वापस doAppend(copy.getArrayStart(), 0, src_लंबाई);
    }

    // optimize append() onto a large-enough, owned string
    अगर (!cloneArrayIfNeeded(new_लंबाई, getGrowCapacity(new_लंबाई))) {
      वापस *यह;
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
  अगर(srcChars != newArray + old_लंबाई) {
    us_arrayCopy(srcChars, 0, newArray, old_लंबाई, src_लंबाई);
  }
  set_लंबाई(new_लंबाई);

  वापस *यह;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  अगर (!isWritable() || src.empty()) {
    वापस *यह;
  }
  अगर (src.length() > INT32_MAX) {
    setToBogus();
    वापस *यह;
  }
  वापस doAppend(src.तथ्य(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
व्योम
UnicodeString::handleReplaceBetween(int32_t start,
                                    int32_t limit,
                                    स्थिर UnicodeString& text) {
    replaceBetween(start, limit, text);
}

/**
 * Replaceable API
 */
व्योम 
UnicodeString::copy(int32_t start, int32_t limit, int32_t dest) {
    अगर (limit <= start) {
        वापस; // Nothing to do; avoid bogus malloc call
    }
    char16_t* text = static_cast<char16_t*>(uprv_malloc(माप(char16_t) * (limit - start)));
    // Check to make sure text is not null.
    अगर (text != nullptr) {
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
UBool Replaceable::has_Meta_तथ्य() स्थिर {
    वापस true;
}

/**
 * Replaceable API
 */
UBool UnicodeString::has_Meta_तथ्य() स्थिर {
    वापस false;
}

UnicodeString&
UnicodeString::doReverse(int32_t start, int32_t length) U_LIFETIME_BOUND {
  अगर(length <= 1 || !cloneArrayIfNeeded()) {
    वापस *यह;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  अगर(length <= 1) {  // pinIndices() might have shrunk the length
    वापस *यह;
  }

  char16_t *बाँए = getArrayStart() + start;
  char16_t *दाँए = बाँए + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = false;

  // Before the loop we know left<right because length>=2.
  करो {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *बाँए));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*बाँए++ = *दाँए));
    *दाँए-- = swap;
  } जबतक(बाँए < दाँए);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*बाँए));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  अगर(hasSupplementary) {
    char16_t swap2;

    बाँए = getArrayStart() + start;
    दाँए = बाँए + length - 1; // -1 so that we can look at *(left+1) if left<right
    जबतक(बाँए < दाँए) {
      अगर(U16_IS_TRAIL(swap = *बाँए) && U16_IS_LEAD(swap2 = *(बाँए + 1))) {
        *बाँए++ = swap2;
        *बाँए++ = swap;
      } अन्यथा {
        ++बाँए;
      }
    }
  }

  वापस *यह;
}

UBool 
UnicodeString::padLeading(int32_t target_लंबाई,
                          char16_t padChar)
{
  int32_t old_लंबाई = length();
  अगर(old_लंबाई >= target_लंबाई || !cloneArrayIfNeeded(target_लंबाई)) {
    वापस false;
  } अन्यथा {
    // move contents up by padding width
    char16_t *array = getArrayStart();
    int32_t start = target_लंबाई - old_लंबाई;
    us_arrayCopy(array, 0, array, start, old_लंबाई);

    // fill in padding character
    जबतक(--start >= 0) {
      array[start] = padChar;
    }
    set_लंबाई(target_लंबाई);
    वापस true;
  }
}

UBool 
UnicodeString::padTrailing(int32_t target_लंबाई,
                           char16_t padChar)
{
  int32_t old_लंबाई = length();
  अगर(old_लंबाई >= target_लंबाई || !cloneArrayIfNeeded(target_लंबाई)) {
    वापस false;
  } अन्यथा {
    // fill in padding character
    char16_t *array = getArrayStart();
    int32_t length = target_लंबाई;
    जबतक(--length >= old_लंबाई) {
      array[length] = padChar;
    }
    set_लंबाई(target_लंबाई);
    वापस true;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::doHashCode() स्थिर
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hashCode = ustr_hashUCharsN(getArrayStart(), length());
    अगर (hashCode == kInvalidHashCode) {
        hashCode = kEmptyHashCode;
    }
    वापस hashCode;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::get_बफर(int32_t minCapacity) U_LIFETIME_BOUND {
  अगर(minCapacity>=-1 && cloneArrayIfNeeded(minCapacity)) {
    fUnion.fFields.f_लंबाई_And_Flags|=k_Open_Get_बफर;
    set_Zero_लंबाई();
    वापस getArrayStart();
  } अन्यथा {
    वापस nullptr;
  }
}

व्योम
UnicodeString::release_बफर(int32_t new_लंबाई) {
  अगर(fUnion.fFields.f_लंबाई_And_Flags&k_Open_Get_बफर && new_लंबाई>=-1) {
    // set the new fLength
    int32_t capacity=getCapacity();
    अगर(new_लंबाई==-1) {
      // the new length is the string length, capped by fCapacity
      स्थिर char16_t *array=getArrayStart(), *p=array, *limit=array+capacity;
      जबतक(p<limit && *p!=0) {
        ++p;
      }
      new_लंबाई = static_cast<int32_t>(p - array);
    } अन्यथा अगर(new_लंबाई>capacity) {
      new_लंबाई=capacity;
    }
    set_लंबाई(new_लंबाई);
    fUnion.fFields.f_लंबाई_And_Flags&=~k_Open_Get_बफर;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t newCapacity,
                                  int32_t growCapacity,
                                  UBool doCopyArray,
                                  int32_t **p_बफर_To_Delete,
                                  UBool forceClone) {
  // default parameters need to be static, therefore
  // the defaults are -1 to have convenience defaults
  अगर(newCapacity == -1) {
    newCapacity = getCapacity();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  अगर(!isWritable()) {
    वापस false;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  अगर(forceClone ||
     fUnion.fFields.f_लंबाई_And_Flags & k_बफर_Is_Readonly ||
     (fUnion.fFields.f_लंबाई_And_Flags & kRefCounted && ref_गिनती() > 1) ||
     newCapacity > getCapacity()
  ) {
    // check growCapacity for default value and use of the stack buffer
    अगर(growCapacity < 0) {
      growCapacity = newCapacity;
    } अन्यथा अगर(newCapacity <= US_STACKBUF_आकार && growCapacity > US_STACKBUF_आकार) {
      growCapacity = US_STACKBUF_आकार;
    } अन्यथा अगर(newCapacity > growCapacity) {
      setToBogus();
      वापस false;  // bad inputs
    }
    अगर(growCapacity > kMaxCapacity) {
      setToBogus();
      वापस false;
    }

    // save old values
    char16_t old_ढेर_बफर[US_STACKBUF_आकार];
    char16_t *oldArray;
    int32_t old_लंबाई = length();
    int16_t flags = fUnion.fFields.f_लंबाई_And_Flags;

    अगर(flags&k_Using_ढेर_बफर) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      अगर(doCopyArray && growCapacity > US_STACKBUF_आकार) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_ढेर_Fields.f_बफर, 0, old_ढेर_बफर, 0, old_लंबाई);
        oldArray = old_ढेर_बफर;
      } अन्यथा {
        oldArray = nullptr; // no need to copy from the stack buffer to itself
      }
    } अन्यथा {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=nullptr); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    अगर(allocate(growCapacity) ||
       (newCapacity < growCapacity && allocate(newCapacity))
    ) {
      अगर(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t min_लंबाई = old_लंबाई;
        newCapacity = getCapacity();
        अगर(newCapacity < min_लंबाई) {
          min_लंबाई = newCapacity;
        }
        अगर(oldArray != nullptr) {
          us_arrayCopy(oldArray, 0, getArrayStart(), 0, min_लंबाई);
        }
        set_लंबाई(min_लंबाई);
      } अन्यथा {
        set_Zero_लंबाई();
      }

      // release the old array
      अगर(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_गिनती = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        अगर(umtx_atomic_dec(p_Ref_गिनती) == 0) {
          अगर (p_बफर_To_Delete == nullptr) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((व्योम *)p_Ref_गिनती);
          } अन्यथा {
            // the caller requested to delete it himself
            *p_बफर_To_Delete = reinterpret_cast<int32_t*>(p_Ref_गिनती);
          }
        }
      }
    } अन्यथा {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      अगर(!(flags&k_Using_ढेर_बफर)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.f_लंबाई_And_Flags = flags;
      setToBogus();
      वापस false;
    }
  }
  वापस true;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::appendCodeUnit(char16_t c) {
  वापस str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::appendCodePoint(UChar32 c) {
  char16_t बफर[U16_MAX_लंबाई];
  int32_t c_लंबाई = 0;
  UBool isError = false;
  U16_APPEND(बफर, c_लंबाई, U16_MAX_लंबाई, c, isError);
  वापस !isError && str.doAppend(बफर, 0, c_लंबाई).isWritable();
}

UBool
UnicodeStringAppendable::appendString(स्थिर char16_t *s, int32_t length) {
  वापस str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserveAppendCapacity(int32_t appendCapacity) {
  वापस str.cloneArrayIfNeeded(str.length() + appendCapacity);
}

char16_t* UnicodeStringAppendable::get_Append_बफर(int32_t minCapacity,
                                                   int32_t desiredCapacityHint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratchCapacity,
                                                   int32_t* परिणाम_Capacity) U_LIFETIME_BOUND {
  अगर(minCapacity < 1 || scratchCapacity < minCapacity) {
    *परिणाम_Capacity = 0;
    वापस nullptr;
  }
  int32_t old_लंबाई = str.length();
  अगर(minCapacity <= (kMaxCapacity - old_लंबाई) &&
      desiredCapacityHint <= (kMaxCapacity - old_लंबाई) &&
      str.cloneArrayIfNeeded(old_लंबाई + minCapacity, old_लंबाई + desiredCapacityHint)) {
    *परिणाम_Capacity = str.getCapacity() - old_लंबाई;
    वापस str.getArrayStart() + old_लंबाई;
  }
  *परिणाम_Capacity = scratchCapacity;
  वापस scratch;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(स्थिर UElement कुंजी) {
    स्थिर UnicodeString *str = (स्थिर UnicodeString*) कुंजी.pointer;
    वापस (str == nullptr) ? 0 : str->hashCode();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(स्थिर UElement key1, स्थिर UElement key2) {
    स्थिर UnicodeString *str1 = (स्थिर UnicodeString*) key1.pointer;
    स्थिर UnicodeString *str2 = (स्थिर UnicodeString*) key2.pointer;
    अगर (str1 == str2) {
        वापस true;
    }
    अगर (str1 == nullptr || str2 == nullptr) {
        वापस false;
    }
    वापस *str1 == *str2;
}

#अगर_घोषित U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#अगर defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#आशय GCC diagnostic push
#आशय GCC diagnostic ignored "-Wunused-function"
अटल व्योम uprv_UnicodeStringDummy() {
    मिटाओ [] (नया UnicodeString[2]);
}
#आशय GCC diagnostic pop
#पूर्ण_अगर
#पूर्ण_अगर
