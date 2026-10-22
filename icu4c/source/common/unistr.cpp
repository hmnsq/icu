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

#यदि 0

#अन्तर्भाव <iostream>
उपयुज्य नामाकाश std;

//DEBUGGING
शून्य
print(ध्रुव UnicodeString& s,
      ध्रुव वर्ण *name)
{
  char16_t c;
  निर्गमः << name << ":|";
  प्रति(पूर्णाङ्क i = 0; i < s.length(); ++i) {
    c = s[i];
    यदि(c>= 0x007E || c < 0x0020)
      निर्गमः << "[0x" << hex << s[i] << "]";
    अन्यथा
      निर्गमः << (वर्ण) s[i];
  }
  निर्गमः << '|' << पङ्क्त्यन्तः;
}

शून्य
print(ध्रुव char16_t *s,
      int32_t len,
      ध्रुव वर्ण *name)
{
  char16_t c;
  निर्गमः << name << ":|";
  प्रति(पूर्णाङ्क i = 0; i < len; ++i) {
    c = s[i];
    यदि(c>= 0x007E || c < 0x0020)
      निर्गमः << "[0x" << hex << s[i] << "]";
    अन्यथा
      निर्गमः << (वर्ण) s[i];
  }
  निर्गमः << '|' << पङ्क्त्यन्तः;
}
// END DEBUGGING
#यदि_अन्तः

// Local function definitions for now

// need to copy areas that may overlap
स्थिर
अन्तःस्थ शून्य
us_arrayCopy(ध्रुव char16_t *src, int32_t srcStart,
         char16_t *dst, int32_t dstStart, int32_t count)
{
  यदि(count>0) {
    uprv_memmove(dst+dstStart, src+srcStart, (size_t)count*परिमाण(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
स्थिर char16_t U_CALLCONV
UnicodeString_charAt(int32_t विस्थापनम्, शून्य *context) {
    प्रत्यागम ((icu::UnicodeString*) context)->charAt(विस्थापनम्);
}
U_CDECL_अन्तः

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
संक्रियकः+ (ध्रुव UnicodeString &s1, ध्रुव UnicodeString &s2) {
  int32_t योग_Lengths;
  यदि (uprv_add32_अतिप्रवाहः(s1.length(), s2.length(), &योग_Lengths)) {
    UnicodeString bogus;
    bogus.समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम bogus;
  }
  यदि (योग_Lengths != INT32_MAX) {
    ++योग_Lengths;  // space for a terminating NUL if we need one
  }
  प्रत्यागम UnicodeString(योग_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_आन्तरिकम्_Concat(ध्रुव UnicodeString &s1, std::u16string_view s2) {
  int32_t योग_Lengths;
  यदि (s2.length() > INT32_MAX ||
      uprv_add32_अतिप्रवाहः(s1.length(), static_cast<int32_t>(s2.length()), &योग_Lengths)) {
    UnicodeString bogus;
    bogus.समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम bogus;
  }
  यदि (योग_Lengths != INT32_MAX) {
    ++योग_Lengths;  // space for a terminating NUL if we need one
  }
  प्रत्यागम UnicodeString(योग_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

शून्य
UnicodeString::सङ्कलय_Ref() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  प्रत्यागम umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_गणनम्() ध्रुव {
  प्रत्यागम umtx_आरोपय_Acquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

शून्य
UnicodeString::releaseArray() {
  यदि((fUnion.fFields.f_दीर्घता_And_Flags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.f_दीर्घता_And_Flags = 0;
  यदि (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } अन्यथा यदि(c <= 0xffff) {
    int32_t length = count;
    यदि(capacity < length) {
      capacity = length;
    }
    यदि(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t unit = static_cast<char16_t>(c);
      प्रति(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      समुच्चयः_दीर्घता(length);
    }
  } अन्यथा {  // supplementary code point, write surrogate pairs
    यदि(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      प्रत्यागम;
    }
    int32_t length = count * 2;
    यदि(capacity < length) {
      capacity = length;
    }
    यदि(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      प्रति(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      समुच्चयः_दीर्घता(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.f_दीर्घता_And_Flags = kLength1 | kShortString;
  fUnion.f_राशिः_Fields.f_बफर[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
  int32_t i = 0;
  UBool isError = असत्य;
  U16_APPEND(fUnion.f_राशिः_Fields.f_बफर, i, US_STACKBUF_आकार, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  यदि(!isError) {
    समुच्चयः_Short_दीर्घता(i);
  }
}

UnicodeString::UnicodeString(ध्रुव char16_t *text,
                             int32_t text_दीर्घता) {
  fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
  doAppend(text, 0, text_दीर्घता);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                             int32_t text_दीर्घता) {
  fUnion.fFields.f_दीर्घता_And_Flags = kReadonlyAlias;
  ध्रुव char16_t *text = textPtr;
  यदि(text == शून्यसूचकः) {
    // treat as an empty string, do not alias
    समुच्चयः_गन्तव्यम्_रिक्तानि();
  } अन्यथा यदि(text_दीर्घता < -1 ||
            (text_दीर्घता == -1 && !isTerminated) ||
            (text_दीर्घता >= 0 && isTerminated && text[text_दीर्घता] != 0)
  ) {
    समुच्चयः_गन्तव्यम्_Bogus();
  } अन्यथा {
    यदि(text_दीर्घता == -1) {
      // text is terminated, or else it would have failed the above test
      text_दीर्घता = u_strlen(text);
    }
    समुच्चयः_Array(const_cast<char16_t *>(text), text_दीर्घता,
             isTerminated ? text_दीर्घता + 1 : text_दीर्घता);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buff_दीर्घता,
                             int32_t buff_क्षमता) {
  fUnion.fFields.f_दीर्घता_And_Flags = kWritableAlias;
  यदि(buff == शून्यसूचकः) {
    // treat as an empty string, do not alias
    समुच्चयः_गन्तव्यम्_रिक्तानि();
  } अन्यथा यदि(buff_दीर्घता < -1 || buff_क्षमता < 0 || buff_दीर्घता > buff_क्षमता) {
    समुच्चयः_गन्तव्यम्_Bogus();
  } अन्यथा {
    यदि(buff_दीर्घता == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ध्रुव char16_t *p = buff, *सीमा = buff + buff_क्षमता;
      यावत्(p != सीमा && *p != 0) {
        ++p;
      }
      buff_दीर्घता = static_cast<int32_t>(p - buff);
    }
    समुच्चयः_Array(buff, buff_दीर्घता, buff_क्षमता);
  }
}

UnicodeString::UnicodeString(ध्रुव वर्ण *src, int32_t length, EInvariant) {
  fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
  यदि(src==शून्यसूचकः) {
    // treat as an empty string
  } अन्यथा {
    यदि(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    यदि(cloneArrayIfNeeded(length, length, असत्य)) {
      u_chars_गन्तव्यम्_UChars(src, getArrayStart(), length);
      समुच्चयः_दीर्घता(length);
    } अन्यथा {
      समुच्चयः_गन्तव्यम्_Bogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view text U_LIFETIME_BOUND) {
  UnicodeString परिणाम;
  यदि (text.length() <= INT32_MAX) {
    परिणाम.समुच्चयः_गन्तव्यम्(असत्य, text.दत्त(), static_cast<int32_t>(text.length()));
  } अन्यथा {
    परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
  }
  प्रत्यागम परिणाम;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ध्रुव UnicodeString& text U_LIFETIME_BOUND) {
  UnicodeString परिणाम;
  यदि (text.isBogus()) {
    परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
  } अन्यथा {
    परिणाम.समुच्चयः_गन्तव्यम्(असत्य, text.get_बफर(), text.length());
  }
  प्रत्यागम परिणाम;
}

#यदि U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ध्रुव वर्ण *codepage_दत्त) {
  fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
  यदि (codepage_दत्त != शून्यसूचकः) {
    समुच्चयः_गन्तव्यम्_UTF8(codepage_दत्त);
  }
}

UnicodeString::UnicodeString(ध्रुव वर्ण *codepage_दत्त, int32_t दत्त_दीर्घता) {
  fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
  // if there's nothing to convert, do nothing
  यदि (codepage_दत्त == शून्यसूचकः || दत्त_दीर्घता == 0 || दत्त_दीर्घता < -1) {
    प्रत्यागम;
  }
  यदि(दत्त_दीर्घता == -1) {
    दत्त_दीर्घता = static_cast<int32_t>(uprv_strlen(codepage_दत्त));
  }
  समुच्चयः_गन्तव्यम्_UTF8(StringPiece(codepage_दत्त, दत्त_दीर्घता));
}

// else see unistr_cnv.cpp
#यदि_अन्तः

UnicodeString::UnicodeString(ध्रुव UnicodeString& that) {
  fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, सत्य);
}

UnicodeString::UnicodeString(ध्रुव UnicodeString& that,
                             int32_t srcStart) {
  fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
  समुच्चयः_गन्तव्यम्(that, srcStart);
}

UnicodeString::UnicodeString(ध्रुव UnicodeString& that,
                             int32_t srcStart,
                             int32_t src_दीर्घता) {
  fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
  समुच्चयः_गन्तव्यम्(that, srcStart, src_दीर्घता);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ध्रुव {
  प्रत्यागम शून्यसूचकः;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ध्रुव {
  LocalPointer<UnicodeString> clonedString(नूतन UnicodeString(*एतत्));
  प्रत्यागम clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : शून्यसूचकः;
}

//========================================
// array allocation
//========================================

नामाकाश {

ध्रुव int32_t k_Grow_आकार = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ध्रुव int32_t k_Max_क्षमता = 0x7ffffff5;

int32_t get_Grow_क्षमता(int32_t new_दीर्घता) {
  int32_t grow_आकार = (new_दीर्घता >> 2) + k_Grow_आकार;
  यदि(grow_आकार <= (k_Max_क्षमता - new_दीर्घता)) {
    प्रत्यागम new_दीर्घता + grow_आकार;
  } अन्यथा {
    प्रत्यागम k_Max_क्षमता;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  यदि(capacity <= US_STACKBUF_आकार) {
    fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
    प्रत्यागम सत्य;
  }
  यदि(capacity <= k_Max_क्षमता) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t अङ्कः_अष्टकानि = परिमाण(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    अङ्कः_अष्टकानि = (अङ्कः_अष्टकानि + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(अङ्कः_अष्टकानि));
    यदि(array != शून्यसूचकः) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      अङ्कः_अष्टकानि -= परिमाण(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.f_क्षमता = static_cast<int32_t>(अङ्कः_अष्टकानि / U_SIZEOF_UCHAR);
      fUnion.fFields.f_दीर्घता_And_Flags = kLongString;
      प्रत्यागम सत्य;
    }
  }
  fUnion.fFields.f_दीर्घता_And_Flags = kIsBogus;
  fUnion.fFields.fArray = शून्यसूचकः;
  fUnion.fFields.f_क्षमता = 0;
  प्रत्यागम असत्य;
}

//========================================
// Destructor
//========================================

#यदि_परिभाषितम् UNISTR_गणनम्_FINAL_STRING_LENGTHS
स्थिर u_atomic_int32_t final_दीर्घता_Counts[0x400];  // UnicodeString::kMaxShortLength+1
स्थिर u_atomic_int32_t beyond_गणनम्(0);

U_CAPI शून्य unistr_printLengths() {
  int32_t i;
  प्रति(i = 0; i <= 59; ++i) {
    प्रारूप_लिख("%2d,  %9d\n", i, (int32_t)final_दीर्घता_Counts[i]);
  }
  int32_t beyond = beyond_गणनम्;
  प्रति(; i < UPRV_LENGTHOF(final_दीर्घता_Counts); ++i) {
    beyond += final_दीर्घता_Counts[i];
  }
  प्रारूप_लिख(">59, %9d\n", beyond);
}
#यदि_अन्तः

UnicodeString::~UnicodeString()
{
#यदि_परिभाषितम् UNISTR_गणनम्_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  यदि((fUnion.fFields.f_दीर्घता_And_Flags&(k_Open_Get_बफर|kReadonlyAlias|kWritableAlias)) == 0) {
    यदि(has_Short_दीर्घता()) {
      umtx_atomic_inc(final_दीर्घता_Counts + get_Short_दीर्घता());
    } अन्यथा {
      umtx_atomic_inc(&beyond_गणनम्);
    }
  }
#यदि_अन्तः

  releaseArray();
}

//========================================
// Factory methods
//========================================

UnicodeString UnicodeString::fromUTF8(StringPiece utf8) {
  UnicodeString परिणाम;
  परिणाम.समुच्चयः_गन्तव्यम्_UTF8(utf8);
  प्रत्यागम परिणाम;
}

UnicodeString UnicodeString::fromUTF32(ध्रुव UChar32 *utf32, int32_t length) {
  UnicodeString परिणाम;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  यदि(length <= US_STACKBUF_आकार) {
    capacity = US_STACKBUF_आकार;
  } अन्यथा {
    capacity = length + (length >> 4) + 4;
  }
  कुरु {
    char16_t *utf16 = परिणाम.get_बफर(capacity);
    int32_t length16;
    UError_संहिता error_संहिता = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, परिणाम.get_क्षमता(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        शून्यसूचकः,    // Don't care about number of substitutions.
        &error_संहिता);
    परिणाम.release_बफर(length16);
    यदि(error_संहिता == U_बफर_अतिप्रवाहः_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      अनुवर्त;
    } अन्यथा यदि(U_FAILURE(error_संहिता)) {
      परिणाम.समुच्चयः_गन्तव्यम्_Bogus();
    }
    भङ्ग;
  } यावत्(सत्य);
  प्रत्यागम परिणाम;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::संक्रियकः=(ध्रुव UnicodeString& src) U_LIFETIME_BOUND {
  प्रत्यागम copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ध्रुव UnicodeString& src) U_LIFETIME_BOUND {
  प्रत्यागम copyFrom(src, सत्य);
}

UnicodeString&
UnicodeString::copyFrom(ध्रुव UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  यदि(एतत् == &src) {
    प्रत्यागम *एतत्;
  }

  // is the right side bogus?
  यदि(src.isBogus()) {
    समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम *एतत्;
  }

  // delete the current contents
  releaseArray();

  यदि(src.is_रिक्तानि()) {
    // empty string - use the stack buffer
    समुच्चयः_गन्तव्यम्_रिक्तानि();
    प्रत्यागम *एतत्;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.f_दीर्घता_And_Flags = src.fUnion.fFields.f_दीर्घता_And_Flags;
  विकल्प(src.fUnion.fFields.f_दीर्घता_And_Flags & kAllStorageFlags) {
  पक्ष kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_राशिः_Fields.f_बफर, src.fUnion.f_राशिः_Fields.f_बफर,
                get_Short_दीर्घता() * U_SIZEOF_UCHAR);
    भङ्ग;
  पक्ष kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).सङ्कलय_Ref();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_क्षमता = src.fUnion.fFields.f_क्षमता;
    यदि(!has_Short_दीर्घता()) {
      fUnion.fFields.f_दीर्घता = src.fUnion.fFields.f_दीर्घता;
    }
    भङ्ग;
  पक्ष kReadonlyAlias:
    यदि(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.f_क्षमता = src.fUnion.fFields.f_क्षमता;
      यदि(!has_Short_दीर्घता()) {
        fUnion.fFields.f_दीर्घता = src.fUnion.fFields.f_दीर्घता;
      }
      भङ्ग;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  पक्ष kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t src_दीर्घता = src.length();
    यदि(allocate(src_दीर्घता)) {
      u_memcpy(getArrayStart(), src.getArrayStart(), src_दीर्घता);
      समुच्चयः_दीर्घता(src_दीर्घता);
      भङ्ग;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  स्वभाव:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.f_दीर्घता_And_Flags = kIsBogus;
    fUnion.fFields.fArray = शून्यसूचकः;
    fUnion.fFields.f_क्षमता = 0;
    भङ्ग;
  }

  प्रत्यागम *एतत्;
}

UnicodeString& UnicodeString::संक्रियकः=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, सत्य);
  प्रत्यागम *एतत्;
}

// Same as move assignment except without memory management.
शून्य UnicodeString::copyFieldsFrom(UnicodeString &src, UBool समुच्चयः_Src_गन्तव्यम्_Bogus) noexcept {
  int16_t दीर्घता_And_Flags = fUnion.fFields.f_दीर्घता_And_Flags = src.fUnion.fFields.f_दीर्घता_And_Flags;
  यदि(दीर्घता_And_Flags & k_Using_राशिः_बफर) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    यदि(एतत् != &src) {
      uprv_memcpy(fUnion.f_राशिः_Fields.f_बफर, src.fUnion.f_राशिः_Fields.f_बफर,
                  get_Short_दीर्घता() * U_SIZEOF_UCHAR);
    }
  } अन्यथा {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_क्षमता = src.fUnion.fFields.f_क्षमता;
    यदि(!has_Short_दीर्घता()) {
      fUnion.fFields.f_दीर्घता = src.fUnion.fFields.f_दीर्घता;
    }
    यदि(समुच्चयः_Src_गन्तव्यम्_Bogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.f_दीर्घता_And_Flags = kIsBogus;
      src.fUnion.fFields.fArray = शून्यसूचकः;
      src.fUnion.fFields.f_क्षमता = 0;
    }
  }
}

शून्य UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString अस्थायी;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  अस्थायी.copyFieldsFrom(*एतत्, असत्य);
  एतत्->copyFieldsFrom(other, असत्य);
  other.copyFieldsFrom(अस्थायी, असत्य);
  // Set temp to an empty string so that other's memory is not released twice.
  अस्थायी.fUnion.fFields.f_दीर्घता_And_Flags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ध्रुव {
    UnicodeString परिणाम(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    यदि (परिणाम.isBogus()) {
        प्रत्यागम परिणाम;
    }
    ध्रुव char16_t *array = get_बफर();
    int32_t len = length();
    int32_t पूर्व = 0;
    प्रति (int32_t i=0;;) {
        यदि (i == len) {
            परिणाम.append(array, पूर्व, len - पूर्व);
            भङ्ग;
        }
        यदि (array[i++] == 0x5C /*'\\'*/) {
            परिणाम.append(array, पूर्व, (i - 1) - पूर्व);
            UChar32 c = unescapeAt(i); // advances i
            यदि (c < 0) {
                परिणाम.remove(); // return empty string
                भङ्ग; // invalid escape sequence
            }
            परिणाम.append(c);
            पूर्व = i;
        }
    }
    प्रत्यागम परिणाम;
}

UChar32 UnicodeString::unescapeAt(int32_t &विस्थापनम्) ध्रुव {
    प्रत्यागम u_unescapeAt(UnicodeString_charAt, &विस्थापनम्, length(), (शून्य*)एतत्);
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
              int32_t src_दीर्घता) ध्रुव
{
  // compare illegal string values
  यदि(isBogus()) {
    प्रत्यागम असत्य;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  यदि(srcChars == शून्यसूचकः) {
    // treat const char16_t *srcChars==nullptr as an empty string
    प्रत्यागम length == 0 ? सत्य : असत्य;
  }

  // get the correct pointer
  ध्रुव char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  // get the srcLength if necessary
  यदि(src_दीर्घता < 0) {
    src_दीर्घता = u_strlen(srcChars + srcStart);
  }

  यदि (length != src_दीर्घता) {
    प्रत्यागम असत्य;
  }

  यदि(length == 0 || chars == srcChars) {
    प्रत्यागम सत्य;
  }

  प्रत्यागम u_memcmp(chars, srcChars, src_दीर्घता) == 0;
}

int8_t
UnicodeString::doCompare( int32_t start,
              int32_t length,
              ध्रुव char16_t *srcChars,
              int32_t srcStart,
              int32_t src_दीर्घता) ध्रुव
{
  // compare illegal string values
  यदि(isBogus()) {
    प्रत्यागम -1;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  यदि(srcChars == शून्यसूचकः) {
    // treat const char16_t *srcChars==nullptr as an empty string
    प्रत्यागम length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ध्रुव char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  int32_t min_दीर्घता;
  int8_t दीर्घता_परिणाम;

  // get the srcLength if necessary
  यदि(src_दीर्घता < 0) {
    src_दीर्घता = u_strlen(srcChars + srcStart);
  }

  // are we comparing different lengths?
  यदि(length != src_दीर्घता) {
    यदि(length < src_दीर्घता) {
      min_दीर्घता = length;
      दीर्घता_परिणाम = -1;
    } अन्यथा {
      min_दीर्घता = src_दीर्घता;
      दीर्घता_परिणाम = 1;
    }
  } अन्यथा {
    min_दीर्घता = length;
    दीर्घता_परिणाम = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  यदि(min_दीर्घता > 0 && chars != srcChars) {
    int32_t परिणाम;

#   यदि U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      परिणाम = uprv_memcmp(chars, srcChars, min_दीर्घता * परिमाण(char16_t));
      यदि(परिणाम != 0) {
        प्रत्यागम (int8_t)(परिणाम >> 15 | 1);
      }
#   अन्यथा
      // little-endian: compare char16_t units
      कुरु {
        परिणाम = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        यदि(परिणाम != 0) {
          प्रत्यागम static_cast<int8_t>(परिणाम >> 15 | 1);
        }
      } यावत्(--min_दीर्घता > 0);
#   endif
  }
  प्रत्यागम दीर्घता_परिणाम;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::do_Compare_संहिता_Point_क्रमः(int32_t start,
                                       int32_t length,
                                       ध्रुव char16_t *srcChars,
                                       int32_t srcStart,
                                       int32_t src_दीर्घता) ध्रुव
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  यदि(isBogus()) {
    प्रत्यागम -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  यदि(srcChars == शून्यसूचकः) {
    srcStart = src_दीर्घता = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=शून्यसूचकः)?(srcChars + srcStart):शून्यसूचकः, src_दीर्घता, असत्य, सत्य);
  /* translate the 32-bit result into an 8-bit one */
  यदि(diff!=0) {
    प्रत्यागम static_cast<int8_t>(diff >> 15 | 1);
  } अन्यथा {
    प्रत्यागम 0;
  }
}

int32_t
UnicodeString::get_दीर्घता() ध्रुव {
    प्रत्यागम length();
}

char16_t
UnicodeString::getCharAt(int32_t विस्थापनम्) ध्रुव {
  प्रत्यागम charAt(विस्थापनम्);
}

UChar32
UnicodeString::getChar32At(int32_t विस्थापनम्) ध्रुव {
  प्रत्यागम char32At(विस्थापनम्);
}

UChar32
UnicodeString::char32At(int32_t विस्थापनम्) ध्रुव
{
  int32_t len = length();
  यदि (static_cast<uint32_t>(विस्थापनम्) < static_cast<uint32_t>(len)) {
    ध्रुव char16_t *array = getArrayStart();
    UChar32 c;
    U16_GET(array, 0, विस्थापनम्, len, c);
    प्रत्यागम c;
  } अन्यथा {
    प्रत्यागम kInvalidUChar;
  }
}

int32_t
UnicodeString::getChar32Start(int32_t विस्थापनम्) ध्रुव {
  यदि (static_cast<uint32_t>(विस्थापनम्) < static_cast<uint32_t>(length())) {
    ध्रुव char16_t *array = getArrayStart();
    U16_समुच्चयः_CP_START(array, 0, विस्थापनम्);
    प्रत्यागम विस्थापनम्;
  } अन्यथा {
    प्रत्यागम 0;
  }
}

int32_t
UnicodeString::get_Char32_सीमा(int32_t विस्थापनम्) ध्रुव {
  int32_t len = length();
  यदि (static_cast<uint32_t>(विस्थापनम्) < static_cast<uint32_t>(len)) {
    ध्रुव char16_t *array = getArrayStart();
    U16_समुच्चयः_CP_सीमा(array, 0, विस्थापनम्, len);
    प्रत्यागम विस्थापनम्;
  } अन्यथा {
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
UnicodeString::has_अधिकम्_Char32_Than(int32_t start, int32_t length, int32_t number) ध्रुव {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  प्रत्यागम u_str_Has_अधिकम्_Char32_Than(getArrayStart()+start, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t सूचकांक, int32_t delta) ध्रुव {
  // pin index
  int32_t len = length();
  यदि(सूचकांक<0) {
    सूचकांक=0;
  } अन्यथा यदि(सूचकांक>len) {
    सूचकांक=len;
  }

  ध्रुव char16_t *array = getArrayStart();
  यदि(delta>0) {
    U16_FWD_N(array, सूचकांक, len, delta);
  } अन्यथा {
    U16_पृष्ठभागः_N(array, 0, सूचकांक, -delta);
  }

  प्रत्यागम सूचकांक;
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
  यदि(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t dest_क्षमता,
                       UError_संहिता &error_संहिता) ध्रुव {
  int32_t len = length();
  यदि(U_SUCCESS(error_संहिता)) {
    यदि (isBogus() || dest_क्षमता < 0 || (dest_क्षमता > 0 && dest == शून्यसूचकः)) {
      error_संहिता=U_ILLEGAL_तर्कः_ERROR;
    } अन्यथा {
      ध्रुव char16_t *array = getArrayStart();
      यदि(len>0 && len<=dest_क्षमता && array!=dest) {
        u_memcpy(dest, array, len);
      }
      प्रत्यागम u_terminateUChars(dest, dest_क्षमता, len, &error_संहिता);
    }
  }

  प्रत्यागम len;
}

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       वर्ण *target,
                       int32_t target_क्षमता,
                       गणना EInvariant) ध्रुव
{
  // if the arguments are illegal, then do nothing
  यदि(target_क्षमता < 0 || (target_क्षमता > 0 && target == शून्यसूचकः)) {
    प्रत्यागम 0;
  }

  // pin the indices to legal values
  pinIndices(start, length);

  यदि(length <= target_क्षमता) {
    u_UChars_गन्तव्यम्_Chars(getArrayStart() + start, target, length);
  }
  UError_संहिता स्थितिः = U_ZERO_ERROR;
  प्रत्यागम u_terminateChars(target, target_क्षमता, length, &स्थितिः);
}

UnicodeString
UnicodeString::अस्थायी_Sub_String(int32_t start, int32_t len) ध्रुव U_LIFETIME_BOUND {
  pinIndices(start, len);
  ध्रुव char16_t *array = get_बफर();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  यदि(array==शून्यसूचकः) {
    array=fUnion.f_राशिः_Fields.f_बफर;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  प्रत्यागम UnicodeString(असत्य, array + start, len);
}

int32_t
UnicodeString::गन्तव्यम्_UTF8(int32_t start, int32_t len,
                      वर्ण *target, int32_t capacity) ध्रुव {
  pinIndices(start, len);
  int32_t length8;
  UError_संहिता error_संहिता = U_ZERO_ERROR;
  u_str_गन्तव्यम्_UTF8_With_Sub(target, capacity, &length8,
                     get_बफर() + start, len,
                     0xFFFD,  // Standard substitution character.
                     शून्यसूचकः,    // Don't care about number of substitutions.
                     &error_संहिता);
  प्रत्यागम length8;
}

#यदि U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start, int32_t len,
                       वर्ण *target, uint32_t dst_आकार) ध्रुव {
  // if the arguments are illegal, then do nothing
  यदि (/*dstSize < 0 || */(dst_आकार > 0 && target == शून्यसूचकः)) {
    प्रत्यागम 0;
  }
  प्रत्यागम गन्तव्यम्_UTF8(start, len, target, dst_आकार <= 0x7fffffff ? static_cast<int32_t>(dst_आकार) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#यदि_अन्तः

शून्य 
UnicodeString::extractBetween(int32_t start,
                  int32_t सीमा,
                  UnicodeString& target) ध्रुव {
  pin_सूचकांक(start);
  pin_सूचकांक(सीमा);
  doExtract(start, सीमा - start, target);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
शून्य
UnicodeString::गन्तव्यम्_UTF8(ByteSink &sink) ध्रुव {
  int32_t length16 = length();
  यदि(length16 != 0) {
    वर्ण राशिः_बफर[1024];
    int32_t capacity = static_cast<int32_t>(परिमाण(राशिः_बफर));
    UBool utf8IsOwned = असत्य;
    वर्ण *utf8 = sink.Get_Append_बफर(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      राशिः_बफर, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UError_संहिता error_संहिता = U_ZERO_ERROR;
    u_str_गन्तव्यम्_UTF8_With_Sub(utf8, capacity, &length8,
                       get_बफर(), length16,
                       0xFFFD,  // Standard substitution character.
                       शून्यसूचकः,    // Don't care about number of substitutions.
                       &error_संहिता);
    यदि(error_संहिता == U_बफर_अतिप्रवाहः_ERROR) {
      utf8 = static_cast<वर्ण*>(uprv_malloc(length8));
      यदि(utf8 != शून्यसूचकः) {
        utf8IsOwned = सत्य;
        error_संहिता = U_ZERO_ERROR;
        u_str_गन्तव्यम्_UTF8_With_Sub(utf8, length8, &length8,
                           get_बफर(), length16,
                           0xFFFD,  // Standard substitution character.
                           शून्यसूचकः,    // Don't care about number of substitutions.
                           &error_संहिता);
      } अन्यथा {
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
      }
    }
    यदि(U_SUCCESS(error_संहिता)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    यदि(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::गन्तव्यम्_UTF32(UChar32 *utf32, int32_t capacity, UError_संहिता &error_संहिता) ध्रुव {
  int32_t length32=0;
  यदि(U_SUCCESS(error_संहिता)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_str_गन्तव्यम्_UTF32_With_Sub(utf32, capacity, &length32,
        get_बफर(), length(),
        0xfffd,  // Substitution character.
        शून्यसूचकः,    // Don't care about number of substitutions.
        &error_संहिता);
  }
  प्रत्यागम length32;
}

int32_t 
UnicodeString::सूचकांक_सम्बन्धि(ध्रुव char16_t *srcChars,
               int32_t srcStart,
               int32_t src_दीर्घता,
               int32_t start,
               int32_t length) ध्रुव
{
  यदि (isBogus() || srcChars == शून्यसूचकः || srcStart < 0 || src_दीर्घता == 0) {
    प्रत्यागम -1;
  }

  // UnicodeString does not find empty substrings
  यदि(src_दीर्घता < 0 && srcChars[srcStart] == 0) {
    प्रत्यागम -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *मेलनम् = u_strFindFirst(array + start, length, srcChars + srcStart, src_दीर्घता);
  यदि(मेलनम् == शून्यसूचकः) {
    प्रत्यागम -1;
  } अन्यथा {
    प्रत्यागम static_cast<int32_t>(मेलनम् - array);
  }
}

int32_t
UnicodeString::do_सूचकांक_सम्बन्धि(char16_t c,
             int32_t start,
             int32_t length) ध्रुव
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *मेलनम् = u_memchr(array + start, c, length);
  यदि(मेलनम् == शून्यसूचकः) {
    प्रत्यागम -1;
  } अन्यथा {
    प्रत्यागम static_cast<int32_t>(मेलनम् - array);
  }
}

int32_t
UnicodeString::do_सूचकांक_सम्बन्धि(UChar32 c,
                         int32_t start,
                         int32_t length) ध्रुव {
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *मेलनम् = u_memchr32(array + start, c, length);
  यदि(मेलनम् == शून्यसूचकः) {
    प्रत्यागम -1;
  } अन्यथा {
    प्रत्यागम static_cast<int32_t>(मेलनम् - array);
  }
}

int32_t 
UnicodeString::चरमः_सूचकांक_सम्बन्धि(ध्रुव char16_t *srcChars,
               int32_t srcStart,
               int32_t src_दीर्घता,
               int32_t start,
               int32_t length) ध्रुव
{
  यदि (isBogus() || srcChars == शून्यसूचकः || srcStart < 0 || src_दीर्घता == 0) {
    प्रत्यागम -1;
  }

  // UnicodeString does not find empty substrings
  यदि(src_दीर्घता < 0 && srcChars[srcStart] == 0) {
    प्रत्यागम -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the last occurrence of the substring
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *मेलनम् = u_str_Find_चरमः(array + start, length, srcChars + srcStart, src_दीर्घता);
  यदि(मेलनम् == शून्यसूचकः) {
    प्रत्यागम -1;
  } अन्यथा {
    प्रत्यागम static_cast<int32_t>(मेलनम् - array);
  }
}

int32_t
UnicodeString::do_चरमः_सूचकांक_सम्बन्धि(char16_t c,
                 int32_t start,
                 int32_t length) ध्रुव
{
  यदि(isBogus()) {
    प्रत्यागम -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *मेलनम् = u_memrchr(array + start, c, length);
  यदि(मेलनम् == शून्यसूचकः) {
    प्रत्यागम -1;
  } अन्यथा {
    प्रत्यागम static_cast<int32_t>(मेलनम् - array);
  }
}

int32_t
UnicodeString::do_चरमः_सूचकांक_सम्बन्धि(UChar32 c,
                             int32_t start,
                             int32_t length) ध्रुव {
  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *मेलनम् = u_memrchr32(array + start, c, length);
  यदि(मेलनम् == शून्यसूचकः) {
    प्रत्यागम -1;
  } अन्यथा {
    प्रत्यागम static_cast<int32_t>(मेलनम् - array);
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
                              int32_t old_दीर्घता,
                              ध्रुव UnicodeString& newText,
                              int32_t newStart,
                              int32_t new_दीर्घता) U_LIFETIME_BOUND
{
  यदि(isBogus() || oldText.isBogus() || newText.isBogus()) {
    प्रत्यागम *एतत्;
  }

  pinIndices(start, length);
  oldText.pinIndices(oldStart, old_दीर्घता);
  newText.pinIndices(newStart, new_दीर्घता);

  यदि(old_दीर्घता == 0) {
    प्रत्यागम *एतत्;
  }

  यावत्(length > 0 && length >= old_दीर्घता) {
    int32_t pos = सूचकांक_सम्बन्धि(oldText, oldStart, old_दीर्घता, start, length);
    यदि(pos < 0) {
      // no more oldText's here: done
      भङ्ग;
    } अन्यथा {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, old_दीर्घता, newText, newStart, new_दीर्घता);
      length -= pos + old_दीर्घता - start;
      start = pos + new_दीर्घता;
    }
  }

  प्रत्यागम *एतत्;
}


शून्य
UnicodeString::समुच्चयः_गन्तव्यम्_Bogus()
{
  releaseArray();

  fUnion.fFields.f_दीर्घता_And_Flags = kIsBogus;
  fUnion.fFields.fArray = शून्यसूचकः;
  fUnion.fFields.f_क्षमता = 0;
}

// turn a bogus string into an empty one
शून्य
UnicodeString::unBogus() {
  यदि(fUnion.fFields.f_दीर्घता_And_Flags & kIsBogus) {
    समुच्चयः_गन्तव्यम्_रिक्तानि();
  }
}

ध्रुव char16_t*
UnicodeString::get_Terminated_बफर() U_LIFETIME_BOUND {
  यदि(!isWritable()) {
    प्रत्यागम शून्यसूचकः;
  }
  char16_t *array = getArrayStart();
  int32_t len = length();
  यदि(len < get_क्षमता()) {
    यदि(fUnion.fFields.f_दीर्घता_And_Flags & k_बफर_Is_Readonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      यदि(array[len] == 0) {
        प्रत्यागम array;
      }
    } अन्यथा यदि(((fUnion.fFields.f_दीर्घता_And_Flags & kRefCounted) == 0 || ref_गणनम्() == 1)) {
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
  यदि(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = getArrayStart();
    array[len] = 0;
    प्रत्यागम array;
  } अन्यथा {
    प्रत्यागम शून्यसूचकः;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::समुच्चयः_गन्तव्यम्(UBool isTerminated,
                     ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                     int32_t text_दीर्घता) U_LIFETIME_BOUND
{
  यदि(fUnion.fFields.f_दीर्घता_And_Flags & k_Open_Get_बफर) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    प्रत्यागम *एतत्;
  }

  ध्रुव char16_t *text = textPtr;
  यदि(text == शून्यसूचकः) {
    // treat as an empty string, do not alias
    releaseArray();
    समुच्चयः_गन्तव्यम्_रिक्तानि();
    प्रत्यागम *एतत्;
  }

  यदि( text_दीर्घता < -1 ||
      (text_दीर्घता == -1 && !isTerminated) ||
      (text_दीर्घता >= 0 && isTerminated && text[text_दीर्घता] != 0)
  ) {
    समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम *एतत्;
  }

  releaseArray();

  यदि(text_दीर्घता == -1) {
    // text is terminated, or else it would have failed the above test
    text_दीर्घता = u_strlen(text);
  }
  fUnion.fFields.f_दीर्घता_And_Flags = kReadonlyAlias;
  समुच्चयः_Array(const_cast<char16_t*>(text), text_दीर्घता, isTerminated ? text_दीर्घता + 1 : text_दीर्घता);
  प्रत्यागम *एतत्;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::समुच्चयः_गन्तव्यम्(char16_t* बफर,
                     int32_t buff_दीर्घता U_LIFETIME_BOUND,
                     int32_t buff_क्षमता) U_LIFETIME_BOUND {
  यदि(fUnion.fFields.f_दीर्घता_And_Flags & k_Open_Get_बफर) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    प्रत्यागम *एतत्;
  }

  यदि(बफर == शून्यसूचकः) {
    // treat as an empty string, do not alias
    releaseArray();
    समुच्चयः_गन्तव्यम्_रिक्तानि();
    प्रत्यागम *एतत्;
  }

  यदि(buff_दीर्घता < -1 || buff_क्षमता < 0 || buff_दीर्घता > buff_क्षमता) {
    समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम *एतत्;
  } अन्यथा यदि(buff_दीर्घता == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ध्रुव char16_t *p = बफर, *सीमा = बफर + buff_क्षमता;
    यावत्(p != सीमा && *p != 0) {
      ++p;
    }
    buff_दीर्घता = static_cast<int32_t>(p - बफर);
  }

  releaseArray();

  fUnion.fFields.f_दीर्घता_And_Flags = kWritableAlias;
  समुच्चयः_Array(बफर, buff_दीर्घता, buff_क्षमता);
  प्रत्यागम *एतत्;
}

UnicodeString& UnicodeString::समुच्चयः_गन्तव्यम्_UTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  यदि(length <= US_STACKBUF_आकार) {
    capacity = US_STACKBUF_आकार;
  } अन्यथा {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = get_बफर(capacity);
  int32_t length16;
  UError_संहिता error_संहिता = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, get_क्षमता(), &length16,
      utf8.दत्त(), length,
      0xfffd,  // Substitution character.
      शून्यसूचकः,    // Don't care about number of substitutions.
      &error_संहिता);
  release_बफर(length16);
  यदि(U_FAILURE(error_संहिता)) {
    समुच्चयः_गन्तव्यम्_Bogus();
  }
  प्रत्यागम *एतत्;
}

UnicodeString&
UnicodeString::समुच्चयः_Char_At(int32_t विस्थापनम्, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  यदि(cloneArrayIfNeeded() && len > 0) {
    यदि(विस्थापनम् < 0) {
      विस्थापनम् = 0;
    } अन्यथा यदि(विस्थापनम् >= len) {
      विस्थापनम् = len - 1;
    }

    getArrayStart()[विस्थापनम्] = c;
  }
  प्रत्यागम *एतत्;
}

UnicodeString&
UnicodeString::replace(int32_t start,
                       int32_t दीर्घता_4,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t बफर[U16_MAX_दीर्घता];
  int32_t count = 0;
  UBool isError = असत्य;
  U16_APPEND(बफर, count, U16_MAX_दीर्घता, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  प्रत्यागम doReplace(start, दीर्घता_4, बफर, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t बफर[U16_MAX_दीर्घता];
  int32_t दीर्घता_4 = 0;
  UBool isError = असत्य;
  U16_APPEND(बफर, दीर्घता_4, U16_MAX_दीर्घता, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  प्रत्यागम isError ? *एतत् : doAppend(बफर, 0, दीर्घता_4);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ध्रुव UnicodeString& src,
                         int32_t srcStart,
                         int32_t src_दीर्घता) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(srcStart, src_दीर्घता);

  // get the characters from src
  // and replace the range in ourselves with them
  प्रत्यागम doReplace(start, length, src.getArrayStart(), srcStart, src_दीर्घता);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ध्रुव char16_t* srcChars,
                         int32_t srcStart,
                         int32_t src_दीर्घता) U_LIFETIME_BOUND
{
  यदि(!isWritable()) {
    प्रत्यागम *एतत्;
  }

  int32_t old_दीर्घता = एतत्->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  यदि((fUnion.fFields.f_दीर्घता_And_Flags&k_बफर_Is_Readonly) && src_दीर्घता == 0) {
    यदि(start == 0) {
      // remove prefix by adjusting the array pointer
      pin_सूचकांक(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.f_क्षमता -= length;
      समुच्चयः_दीर्घता(old_दीर्घता - length);
      प्रत्यागम *एतत्;
    } अन्यथा {
      pin_सूचकांक(start);
      यदि(length >= (old_दीर्घता - start)) {
        // remove suffix by reducing the length (like truncate())
        समुच्चयः_दीर्घता(start);
        fUnion.fFields.f_क्षमता = start;  // not NUL-terminated any more
        प्रत्यागम *एतत्;
      }
    }
  }

  यदि(start == old_दीर्घता) {
    प्रत्यागम doAppend(srcChars, srcStart, src_दीर्घता);
  }

  यदि (srcChars == शून्यसूचकः) {
    src_दीर्घता = 0;
  } अन्यथा {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    यदि (src_दीर्घता < 0) {
      // get the srcLength if necessary
      src_दीर्घता = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t new_दीर्घता = old_दीर्घता - length;
  यदि(src_दीर्घता > (INT32_MAX - new_दीर्घता)) {
    समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम *एतत्;
  }
  new_दीर्घता += src_दीर्घता;

  // Check for insertion into ourself
  ध्रुव char16_t *oldArray = getArrayStart();
  यदि (is_बफर_Writable() &&
      oldArray < srcChars + src_दीर्घता &&
      srcChars < oldArray + old_दीर्घता) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, src_दीर्घता);
    यदि (copy.isBogus()) {
      समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम *एतत्;
    }
    प्रत्यागम doReplace(start, length, copy.getArrayStart(), 0, src_दीर्घता);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_राशिः_बफर[US_STACKBUF_आकार];
  यदि((fUnion.fFields.f_दीर्घता_And_Flags&k_Using_राशिः_बफर) && (new_दीर्घता > US_STACKBUF_आकार)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_राशिः_बफर, oldArray, old_दीर्घता);
    oldArray = old_राशिः_बफर;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *बफर_गन्तव्यम्_Delete = शून्यसूचकः;
  यदि(!cloneArrayIfNeeded(new_दीर्घता, get_Grow_क्षमता(new_दीर्घता),
                         असत्य, &बफर_गन्तव्यम्_Delete)
  ) {
    प्रत्यागम *एतत्;
  }

  // now do the replace

  char16_t *newArray = getArrayStart();
  यदि(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + src_दीर्घता,
                 old_दीर्घता - (start + length));
  } अन्यथा यदि(length != src_दीर्घता) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, start + length,
                 newArray, start + src_दीर्घता,
                 old_दीर्घता - (start + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, start, src_दीर्घता);

  समुच्चयः_दीर्घता(new_दीर्घता);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  यदि (बफर_गन्तव्यम्_Delete) {
    uprv_free(बफर_गन्तव्यम्_Delete);
  }

  प्रत्यागम *एतत्;
}

UnicodeString&
UnicodeString::doReplace(int32_t start, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  यदि (!isWritable()) {
    प्रत्यागम *एतत्;
  }
  यदि (src.length() > INT32_MAX) {
    समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम *एतत्;
  }
  प्रत्यागम doReplace(start, length, src.दत्त(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ध्रुव UnicodeString& src,
                        int32_t srcStart,
                        int32_t src_दीर्घता) U_LIFETIME_BOUND {
  यदि(src_दीर्घता == 0) {
    प्रत्यागम *एतत्;
  }

  // pin the indices to legal values
  src.pinIndices(srcStart, src_दीर्घता);
  प्रत्यागम doAppend(src.getArrayStart(), srcStart, src_दीर्घता);
}

UnicodeString&
UnicodeString::doAppend(ध्रुव char16_t* srcChars,
                        int32_t srcStart,
                        int32_t src_दीर्घता) U_LIFETIME_BOUND {
  यदि(!isWritable() || src_दीर्घता == 0 || srcChars == शून्यसूचकः) {
    प्रत्यागम *एतत्;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  यदि(src_दीर्घता < 0) {
    // get the srcLength if necessary
    यदि((src_दीर्घता = u_strlen(srcChars)) == 0) {
      प्रत्यागम *एतत्;
    }
  }

  int32_t old_दीर्घता = length();
  int32_t new_दीर्घता;

  यदि (src_दीर्घता <= get_क्षमता() - old_दीर्घता && is_बफर_Writable()) {
    new_दीर्घता = old_दीर्घता + src_दीर्घता;
    // Faster than a memmove
    यदि (src_दीर्घता <= 4) {
      char16_t *arr = getArrayStart();
      arr[old_दीर्घता] = srcChars[0];
      यदि (src_दीर्घता > 1) arr[old_दीर्घता+1] = srcChars[1];
      यदि (src_दीर्घता > 2) arr[old_दीर्घता+2] = srcChars[2];
      यदि (src_दीर्घता > 3) arr[old_दीर्घता+3] = srcChars[3];
      समुच्चयः_दीर्घता(new_दीर्घता);
      प्रत्यागम *एतत्;
    }
  } अन्यथा {
    यदि (uprv_add32_अतिप्रवाहः(old_दीर्घता, src_दीर्घता, &new_दीर्घता)) {
      समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम *एतत्;
    }

    // Check for append onto ourself
    ध्रुव char16_t* oldArray = getArrayStart();
    यदि (is_बफर_Writable() &&
        oldArray < srcChars + src_दीर्घता &&
        srcChars < oldArray + old_दीर्घता) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, src_दीर्घता);
      यदि (copy.isBogus()) {
        समुच्चयः_गन्तव्यम्_Bogus();
        प्रत्यागम *एतत्;
      }
      प्रत्यागम doAppend(copy.getArrayStart(), 0, src_दीर्घता);
    }

    // optimize append() onto a large-enough, owned string
    यदि (!cloneArrayIfNeeded(new_दीर्घता, get_Grow_क्षमता(new_दीर्घता))) {
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
  यदि(srcChars != newArray + old_दीर्घता) {
    us_arrayCopy(srcChars, 0, newArray, old_दीर्घता, src_दीर्घता);
  }
  समुच्चयः_दीर्घता(new_दीर्घता);

  प्रत्यागम *एतत्;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  यदि (!isWritable() || src.empty()) {
    प्रत्यागम *एतत्;
  }
  यदि (src.length() > INT32_MAX) {
    समुच्चयः_गन्तव्यम्_Bogus();
    प्रत्यागम *एतत्;
  }
  प्रत्यागम doAppend(src.दत्त(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
शून्य
UnicodeString::handleReplaceBetween(int32_t start,
                                    int32_t सीमा,
                                    ध्रुव UnicodeString& text) {
    replaceBetween(start, सीमा, text);
}

/**
 * Replaceable API
 */
शून्य 
UnicodeString::copy(int32_t start, int32_t सीमा, int32_t dest) {
    यदि (सीमा <= start) {
        प्रत्यागम; // Nothing to do; avoid bogus malloc call
    }
    char16_t* text = static_cast<char16_t*>(uprv_malloc(परिमाण(char16_t) * (सीमा - start)));
    // Check to make sure text is not null.
    यदि (text != शून्यसूचकः) {
	    extractBetween(start, सीमा, text, 0);
	    insert(dest, text, 0, सीमा - start);    
	    uprv_free(text);
    }
}

/**
 * Replaceable API
 *
 * NOTE: This is for the Replaceable class.  There is no rep.cpp,
 * so we implement this function here.
 */
UBool Replaceable::has_Meta_दत्त() ध्रुव {
    प्रत्यागम सत्य;
}

/**
 * Replaceable API
 */
UBool UnicodeString::has_Meta_दत्त() ध्रुव {
    प्रत्यागम असत्य;
}

UnicodeString&
UnicodeString::doReverse(int32_t start, int32_t length) U_LIFETIME_BOUND {
  यदि(length <= 1 || !cloneArrayIfNeeded()) {
    प्रत्यागम *एतत्;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  यदि(length <= 1) {  // pinIndices() might have shrunk the length
    प्रत्यागम *एतत्;
  }

  char16_t *वामतः = getArrayStart() + start;
  char16_t *दक्षिणतः = वामतः + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = असत्य;

  // Before the loop we know left<right because length>=2.
  कुरु {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *वामतः));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*वामतः++ = *दक्षिणतः));
    *दक्षिणतः-- = swap;
  } यावत्(वामतः < दक्षिणतः);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*वामतः));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  यदि(hasSupplementary) {
    char16_t swap2;

    वामतः = getArrayStart() + start;
    दक्षिणतः = वामतः + length - 1; // -1 so that we can look at *(left+1) if left<right
    यावत्(वामतः < दक्षिणतः) {
      यदि(U16_IS_TRAIL(swap = *वामतः) && U16_IS_LEAD(swap2 = *(वामतः + 1))) {
        *वामतः++ = swap2;
        *वामतः++ = swap;
      } अन्यथा {
        ++वामतः;
      }
    }
  }

  प्रत्यागम *एतत्;
}

UBool 
UnicodeString::padLeading(int32_t target_दीर्घता,
                          char16_t padChar)
{
  int32_t old_दीर्घता = length();
  यदि(old_दीर्घता >= target_दीर्घता || !cloneArrayIfNeeded(target_दीर्घता)) {
    प्रत्यागम असत्य;
  } अन्यथा {
    // move contents up by padding width
    char16_t *array = getArrayStart();
    int32_t start = target_दीर्घता - old_दीर्घता;
    us_arrayCopy(array, 0, array, start, old_दीर्घता);

    // fill in padding character
    यावत्(--start >= 0) {
      array[start] = padChar;
    }
    समुच्चयः_दीर्घता(target_दीर्घता);
    प्रत्यागम सत्य;
  }
}

UBool 
UnicodeString::padTrailing(int32_t target_दीर्घता,
                           char16_t padChar)
{
  int32_t old_दीर्घता = length();
  यदि(old_दीर्घता >= target_दीर्घता || !cloneArrayIfNeeded(target_दीर्घता)) {
    प्रत्यागम असत्य;
  } अन्यथा {
    // fill in padding character
    char16_t *array = getArrayStart();
    int32_t length = target_दीर्घता;
    यावत्(--length >= old_दीर्घता) {
      array[length] = padChar;
    }
    समुच्चयः_दीर्घता(target_दीर्घता);
    प्रत्यागम सत्य;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::do_Hash_संहिता() ध्रुव
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hash_संहिता = ustr_hashUCharsN(getArrayStart(), length());
    यदि (hash_संहिता == k_Invalid_Hash_संहिता) {
        hash_संहिता = k_रिक्तानि_Hash_संहिता;
    }
    प्रत्यागम hash_संहिता;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::get_बफर(int32_t min_क्षमता) U_LIFETIME_BOUND {
  यदि(min_क्षमता>=-1 && cloneArrayIfNeeded(min_क्षमता)) {
    fUnion.fFields.f_दीर्घता_And_Flags|=k_Open_Get_बफर;
    समुच्चयः_Zero_दीर्घता();
    प्रत्यागम getArrayStart();
  } अन्यथा {
    प्रत्यागम शून्यसूचकः;
  }
}

शून्य
UnicodeString::release_बफर(int32_t new_दीर्घता) {
  यदि(fUnion.fFields.f_दीर्घता_And_Flags&k_Open_Get_बफर && new_दीर्घता>=-1) {
    // set the new fLength
    int32_t capacity=get_क्षमता();
    यदि(new_दीर्घता==-1) {
      // the new length is the string length, capped by fCapacity
      ध्रुव char16_t *array=getArrayStart(), *p=array, *सीमा=array+capacity;
      यावत्(p<सीमा && *p!=0) {
        ++p;
      }
      new_दीर्घता = static_cast<int32_t>(p - array);
    } अन्यथा यदि(new_दीर्घता>capacity) {
      new_दीर्घता=capacity;
    }
    समुच्चयः_दीर्घता(new_दीर्घता);
    fUnion.fFields.f_दीर्घता_And_Flags&=~k_Open_Get_बफर;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t new_क्षमता,
                                  int32_t grow_क्षमता,
                                  UBool doCopyArray,
                                  int32_t **p_बफर_गन्तव्यम्_Delete,
                                  UBool forceClone) {
  // default parameters need to be static, therefore
  // the defaults are -1 to have convenience defaults
  यदि(new_क्षमता == -1) {
    new_क्षमता = get_क्षमता();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  यदि(!isWritable()) {
    प्रत्यागम असत्य;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  यदि(forceClone ||
     fUnion.fFields.f_दीर्घता_And_Flags & k_बफर_Is_Readonly ||
     (fUnion.fFields.f_दीर्घता_And_Flags & kRefCounted && ref_गणनम्() > 1) ||
     new_क्षमता > get_क्षमता()
  ) {
    // check growCapacity for default value and use of the stack buffer
    यदि(grow_क्षमता < 0) {
      grow_क्षमता = new_क्षमता;
    } अन्यथा यदि(new_क्षमता <= US_STACKBUF_आकार && grow_क्षमता > US_STACKBUF_आकार) {
      grow_क्षमता = US_STACKBUF_आकार;
    } अन्यथा यदि(new_क्षमता > grow_क्षमता) {
      समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम असत्य;  // bad inputs
    }
    यदि(grow_क्षमता > k_Max_क्षमता) {
      समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम असत्य;
    }

    // save old values
    char16_t old_राशिः_बफर[US_STACKBUF_आकार];
    char16_t *oldArray;
    int32_t old_दीर्घता = length();
    int16_t flags = fUnion.fFields.f_दीर्घता_And_Flags;

    यदि(flags&k_Using_राशिः_बफर) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      यदि(doCopyArray && grow_क्षमता > US_STACKBUF_आकार) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_राशिः_Fields.f_बफर, 0, old_राशिः_बफर, 0, old_दीर्घता);
        oldArray = old_राशिः_बफर;
      } अन्यथा {
        oldArray = शून्यसूचकः; // no need to copy from the stack buffer to itself
      }
    } अन्यथा {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=शून्यसूचकः); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    यदि(allocate(grow_क्षमता) ||
       (new_क्षमता < grow_क्षमता && allocate(new_क्षमता))
    ) {
      यदि(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t min_दीर्घता = old_दीर्घता;
        new_क्षमता = get_क्षमता();
        यदि(new_क्षमता < min_दीर्घता) {
          min_दीर्घता = new_क्षमता;
        }
        यदि(oldArray != शून्यसूचकः) {
          us_arrayCopy(oldArray, 0, getArrayStart(), 0, min_दीर्घता);
        }
        समुच्चयः_दीर्घता(min_दीर्घता);
      } अन्यथा {
        समुच्चयः_Zero_दीर्घता();
      }

      // release the old array
      यदि(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_गणनम् = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        यदि(umtx_atomic_dec(p_Ref_गणनम्) == 0) {
          यदि (p_बफर_गन्तव्यम्_Delete == शून्यसूचकः) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((शून्य *)p_Ref_गणनम्);
          } अन्यथा {
            // the caller requested to delete it himself
            *p_बफर_गन्तव्यम्_Delete = reinterpret_cast<int32_t*>(p_Ref_गणनम्);
          }
        }
      }
    } अन्यथा {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      यदि(!(flags&k_Using_राशिः_बफर)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.f_दीर्घता_And_Flags = flags;
      समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम असत्य;
    }
  }
  प्रत्यागम सत्य;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::append_संहिता_Unit(char16_t c) {
  प्रत्यागम str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::append_संहिता_Point(UChar32 c) {
  char16_t बफर[U16_MAX_दीर्घता];
  int32_t c_दीर्घता = 0;
  UBool isError = असत्य;
  U16_APPEND(बफर, c_दीर्घता, U16_MAX_दीर्घता, c, isError);
  प्रत्यागम !isError && str.doAppend(बफर, 0, c_दीर्घता).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ध्रुव char16_t *s, int32_t length) {
  प्रत्यागम str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserve_Append_क्षमता(int32_t append_क्षमता) {
  प्रत्यागम str.cloneArrayIfNeeded(str.length() + append_क्षमता);
}

char16_t* UnicodeStringAppendable::get_Append_बफर(int32_t min_क्षमता,
                                                   int32_t desired_क्षमता_Hint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratch_क्षमता,
                                                   int32_t* परिणाम_क्षमता) U_LIFETIME_BOUND {
  यदि(min_क्षमता < 1 || scratch_क्षमता < min_क्षमता) {
    *परिणाम_क्षमता = 0;
    प्रत्यागम शून्यसूचकः;
  }
  int32_t old_दीर्घता = str.length();
  यदि(min_क्षमता <= (k_Max_क्षमता - old_दीर्घता) &&
      desired_क्षमता_Hint <= (k_Max_क्षमता - old_दीर्घता) &&
      str.cloneArrayIfNeeded(old_दीर्घता + min_क्षमता, old_दीर्घता + desired_क्षमता_Hint)) {
    *परिणाम_क्षमता = str.get_क्षमता() - old_दीर्घता;
    प्रत्यागम str.getArrayStart() + old_दीर्घता;
  }
  *परिणाम_क्षमता = scratch_क्षमता;
  प्रत्यागम scratch;
}

U_NAMESPACE_अन्तः

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ध्रुव UElement कुञ्चिका) {
    ध्रुव UnicodeString *str = (ध्रुव UnicodeString*) कुञ्चिका.pointer;
    प्रत्यागम (str == शून्यसूचकः) ? 0 : str->hash_संहिता();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ध्रुव UElement key1, ध्रुव UElement key2) {
    ध्रुव UnicodeString *str1 = (ध्रुव UnicodeString*) key1.pointer;
    ध्रुव UnicodeString *str2 = (ध्रुव UnicodeString*) key2.pointer;
    यदि (str1 == str2) {
        प्रत्यागम सत्य;
    }
    यदि (str1 == शून्यसूचकः || str2 == शून्यसूचकः) {
        प्रत्यागम असत्य;
    }
    प्रत्यागम *str1 == *str2;
}

#यदि_परिभाषितम् U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#यदि defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#निर्देशः GCC diagnostic push
#निर्देशः GCC diagnostic ignored "-Wunused-function"
स्थिर शून्य uprv_UnicodeStringDummy() {
    विलोपय [] (नूतन UnicodeString[2]);
}
#निर्देशः GCC diagnostic pop
#यदि_अन्तः
#यदि_अन्तः
