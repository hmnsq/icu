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

#అన్తర్భావ <string_view>

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/appendable.h"
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "uelement.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "uassert.h"

#ఒకవేళ 0

#అన్తర్భావ <iostream>
ఉపయోగించి నామాకాశ std;

//DEBUGGING
శూన్య
print(స్థిరరాశి UnicodeString& s,
      స్థిరరాశి అక్షరం *name)
{
  char16_t c;
  బయటకు << name << ":|";
  కొరకు(పూర్ణాంకం i = 0; i < s.length(); ++i) {
    c = s[i];
    ఒకవేళ(c>= 0x007E || c < 0x0020)
      బయటకు << "[0x" << hex << s[i] << "]";
    లేకపోతే
      బయటకు << (అక్షరం) s[i];
  }
  బయటకు << '|' << పంక్తి_ముగింపు;
}

శూన్య
print(స్థిరరాశి char16_t *s,
      int32_t len,
      స్థిరరాశి అక్షరం *name)
{
  char16_t c;
  బయటకు << name << ":|";
  కొరకు(పూర్ణాంకం i = 0; i < len; ++i) {
    c = s[i];
    ఒకవేళ(c>= 0x007E || c < 0x0020)
      బయటకు << "[0x" << hex << s[i] << "]";
    లేకపోతే
      బయటకు << (అక్షరం) s[i];
  }
  బయటకు << '|' << పంక్తి_ముగింపు;
}
// END DEBUGGING
#అయితే_ముగింపు

// Local function definitions for now

// need to copy areas that may overlap
స్థిర
అంతఃవరుస శూన్య
us_arrayCopy(స్థిరరాశి char16_t *src, int32_t src_ప్రారంభం,
         char16_t *dst, int32_t dst_ప్రారంభం, int32_t count)
{
  ఒకవేళ(count>0) {
    uprv_memmove(dst+dst_ప్రారంభం, src+src_ప్రారంభం, (size_t)count*పరిమాణం(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
స్థిర char16_t U_CALLCONV
UnicodeString_charAt(int32_t స్థానభ్రంశం, శూన్య *context) {
    తిరిగి ((icu::UnicodeString*) context)->charAt(స్థానభ్రంశం);
}
U_CDECL_చివర

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
క్రియాకారకం+ (స్థిరరాశి UnicodeString &s1, స్థిరరాశి UnicodeString &s2) {
  int32_t కూడిక_Lengths;
  ఒకవేళ (uprv_add32_అధికప్రవాహం(s1.length(), s2.length(), &కూడిక_Lengths)) {
    UnicodeString bogus;
    bogus.సమితి_గమ్యం_Bogus();
    తిరిగి bogus;
  }
  ఒకవేళ (కూడిక_Lengths != INT32_MAX) {
    ++కూడిక_Lengths;  // space for a terminating NUL if we need one
  }
  తిరిగి UnicodeString(కూడిక_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_అంతర్గత_Concat(స్థిరరాశి UnicodeString &s1, std::u16string_view s2) {
  int32_t కూడిక_Lengths;
  ఒకవేళ (s2.length() > INT32_MAX ||
      uprv_add32_అధికప్రవాహం(s1.length(), static_cast<int32_t>(s2.length()), &కూడిక_Lengths)) {
    UnicodeString bogus;
    bogus.సమితి_గమ్యం_Bogus();
    తిరిగి bogus;
  }
  ఒకవేళ (కూడిక_Lengths != INT32_MAX) {
    ++కూడిక_Lengths;  // space for a terminating NUL if we need one
  }
  తిరిగి UnicodeString(కూడిక_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

శూన్య
UnicodeString::కూడు_Ref() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  తిరిగి umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_లెక్క() స్థిరరాశి {
  తిరిగి umtx_ఎక్కించు_Acquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

శూన్య
UnicodeString::releaseArray() {
  ఒకవేళ((fUnion.fFields.f_నిడివి_And_Flags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.f_నిడివి_And_Flags = 0;
  ఒకవేళ (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } లేకపోతే ఒకవేళ(c <= 0xffff) {
    int32_t length = count;
    ఒకవేళ(capacity < length) {
      capacity = length;
    }
    ఒకవేళ(allocate(capacity)) {
      char16_t *array = get_Array_ప్రారంభం();
      char16_t unit = static_cast<char16_t>(c);
      కొరకు(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      సమితి_నిడివి(length);
    }
  } లేకపోతే {  // supplementary code point, write surrogate pairs
    ఒకవేళ(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      తిరిగి;
    }
    int32_t length = count * 2;
    ఒకవేళ(capacity < length) {
      capacity = length;
    }
    ఒకవేళ(allocate(capacity)) {
      char16_t *array = get_Array_ప్రారంభం();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      కొరకు(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      సమితి_నిడివి(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.f_నిడివి_And_Flags = kLength1 | kShortString;
  fUnion.f_దొంతర_Fields.f_బఫరు[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.f_నిడివి_And_Flags = kShortString;
  int32_t i = 0;
  UBool isError = అబద్ధం;
  U16_APPEND(fUnion.f_దొంతర_Fields.f_బఫరు, i, US_STACKBUF_కొలత, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  ఒకవేళ(!isError) {
    సమితి_Short_నిడివి(i);
  }
}

UnicodeString::UnicodeString(స్థిరరాశి char16_t *పాఠ్యం,
                             int32_t పాఠ్యం_నిడివి) {
  fUnion.fFields.f_నిడివి_And_Flags = kShortString;
  doAppend(పాఠ్యం, 0, పాఠ్యం_నిడివి);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr పాఠ్యం_Ptr U_LIFETIME_BOUND,
                             int32_t పాఠ్యం_నిడివి) {
  fUnion.fFields.f_నిడివి_And_Flags = kReadonlyAlias;
  స్థిరరాశి char16_t *పాఠ్యం = పాఠ్యం_Ptr;
  ఒకవేళ(పాఠ్యం == శూన్యసూచిక) {
    // treat as an empty string, do not alias
    సమితి_గమ్యం_ఖాళీలు();
  } లేకపోతే ఒకవేళ(పాఠ్యం_నిడివి < -1 ||
            (పాఠ్యం_నిడివి == -1 && !isTerminated) ||
            (పాఠ్యం_నిడివి >= 0 && isTerminated && పాఠ్యం[పాఠ్యం_నిడివి] != 0)
  ) {
    సమితి_గమ్యం_Bogus();
  } లేకపోతే {
    ఒకవేళ(పాఠ్యం_నిడివి == -1) {
      // text is terminated, or else it would have failed the above test
      పాఠ్యం_నిడివి = u_strlen(పాఠ్యం);
    }
    సమితి_Array(const_cast<char16_t *>(పాఠ్యం), పాఠ్యం_నిడివి,
             isTerminated ? పాఠ్యం_నిడివి + 1 : పాఠ్యం_నిడివి);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buff_నిడివి,
                             int32_t buff_సామర్థ్యం) {
  fUnion.fFields.f_నిడివి_And_Flags = kWritableAlias;
  ఒకవేళ(buff == శూన్యసూచిక) {
    // treat as an empty string, do not alias
    సమితి_గమ్యం_ఖాళీలు();
  } లేకపోతే ఒకవేళ(buff_నిడివి < -1 || buff_సామర్థ్యం < 0 || buff_నిడివి > buff_సామర్థ్యం) {
    సమితి_గమ్యం_Bogus();
  } లేకపోతే {
    ఒకవేళ(buff_నిడివి == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      స్థిరరాశి char16_t *p = buff, *హద్దు = buff + buff_సామర్థ్యం;
      అయితే(p != హద్దు && *p != 0) {
        ++p;
      }
      buff_నిడివి = static_cast<int32_t>(p - buff);
    }
    సమితి_Array(buff, buff_నిడివి, buff_సామర్థ్యం);
  }
}

UnicodeString::UnicodeString(స్థిరరాశి అక్షరం *src, int32_t length, EInvariant) {
  fUnion.fFields.f_నిడివి_And_Flags = kShortString;
  ఒకవేళ(src==శూన్యసూచిక) {
    // treat as an empty string
  } లేకపోతే {
    ఒకవేళ(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    ఒకవేళ(cloneArrayIfNeeded(length, length, అబద్ధం)) {
      u_chars_గమ్యం_UChars(src, get_Array_ప్రారంభం(), length);
      సమితి_నిడివి(length);
    } లేకపోతే {
      సమితి_గమ్యం_Bogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view పాఠ్యం U_LIFETIME_BOUND) {
  UnicodeString ఫలితం;
  ఒకవేళ (పాఠ్యం.length() <= INT32_MAX) {
    ఫలితం.సమితి_గమ్యం(అబద్ధం, పాఠ్యం.దత్తాంశం(), static_cast<int32_t>(పాఠ్యం.length()));
  } లేకపోతే {
    ఫలితం.సమితి_గమ్యం_Bogus();
  }
  తిరిగి ఫలితం;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(స్థిరరాశి UnicodeString& పాఠ్యం U_LIFETIME_BOUND) {
  UnicodeString ఫలితం;
  ఒకవేళ (పాఠ్యం.isBogus()) {
    ఫలితం.సమితి_గమ్యం_Bogus();
  } లేకపోతే {
    ఫలితం.సమితి_గమ్యం(అబద్ధం, పాఠ్యం.get_బఫరు(), పాఠ్యం.length());
  }
  తిరిగి ఫలితం;
}

#ఒకవేళ U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(స్థిరరాశి అక్షరం *codepage_దత్తాంశం) {
  fUnion.fFields.f_నిడివి_And_Flags = kShortString;
  ఒకవేళ (codepage_దత్తాంశం != శూన్యసూచిక) {
    సమితి_గమ్యం_UTF8(codepage_దత్తాంశం);
  }
}

UnicodeString::UnicodeString(స్థిరరాశి అక్షరం *codepage_దత్తాంశం, int32_t దత్తాంశం_నిడివి) {
  fUnion.fFields.f_నిడివి_And_Flags = kShortString;
  // if there's nothing to convert, do nothing
  ఒకవేళ (codepage_దత్తాంశం == శూన్యసూచిక || దత్తాంశం_నిడివి == 0 || దత్తాంశం_నిడివి < -1) {
    తిరిగి;
  }
  ఒకవేళ(దత్తాంశం_నిడివి == -1) {
    దత్తాంశం_నిడివి = static_cast<int32_t>(uprv_strlen(codepage_దత్తాంశం));
  }
  సమితి_గమ్యం_UTF8(StringPiece(codepage_దత్తాంశం, దత్తాంశం_నిడివి));
}

// else see unistr_cnv.cpp
#అయితే_ముగింపు

UnicodeString::UnicodeString(స్థిరరాశి UnicodeString& that) {
  fUnion.fFields.f_నిడివి_And_Flags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, నిజం);
}

UnicodeString::UnicodeString(స్థిరరాశి UnicodeString& that,
                             int32_t src_ప్రారంభం) {
  fUnion.fFields.f_నిడివి_And_Flags = kShortString;
  సమితి_గమ్యం(that, src_ప్రారంభం);
}

UnicodeString::UnicodeString(స్థిరరాశి UnicodeString& that,
                             int32_t src_ప్రారంభం,
                             int32_t src_నిడివి) {
  fUnion.fFields.f_నిడివి_And_Flags = kShortString;
  సమితి_గమ్యం(that, src_ప్రారంభం, src_నిడివి);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() స్థిరరాశి {
  తిరిగి శూన్యసూచిక;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() స్థిరరాశి {
  Local_సూచిక<UnicodeString> clonedString(కొత్త UnicodeString(*ఇది));
  తిరిగి clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : శూన్యసూచిక;
}

//========================================
// array allocation
//========================================

నామాకాశ {

స్థిరరాశి int32_t k_Grow_కొలత = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
స్థిరరాశి int32_t k_Max_సామర్థ్యం = 0x7ffffff5;

int32_t get_Grow_సామర్థ్యం(int32_t new_నిడివి) {
  int32_t grow_కొలత = (new_నిడివి >> 2) + k_Grow_కొలత;
  ఒకవేళ(grow_కొలత <= (k_Max_సామర్థ్యం - new_నిడివి)) {
    తిరిగి new_నిడివి + grow_కొలత;
  } లేకపోతే {
    తిరిగి k_Max_సామర్థ్యం;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  ఒకవేళ(capacity <= US_STACKBUF_కొలత) {
    fUnion.fFields.f_నిడివి_And_Flags = kShortString;
    తిరిగి నిజం;
  }
  ఒకవేళ(capacity <= k_Max_సామర్థ్యం) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t అంకె_బైట్లు = పరిమాణం(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    అంకె_బైట్లు = (అంకె_బైట్లు + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(అంకె_బైట్లు));
    ఒకవేళ(array != శూన్యసూచిక) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      అంకె_బైట్లు -= పరిమాణం(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.f_సామర్థ్యం = static_cast<int32_t>(అంకె_బైట్లు / U_SIZEOF_UCHAR);
      fUnion.fFields.f_నిడివి_And_Flags = kLongString;
      తిరిగి నిజం;
    }
  }
  fUnion.fFields.f_నిడివి_And_Flags = kIsBogus;
  fUnion.fFields.fArray = శూన్యసూచిక;
  fUnion.fFields.f_సామర్థ్యం = 0;
  తిరిగి అబద్ధం;
}

//========================================
// Destructor
//========================================

#నిర్వచితమైతే UNISTR_లెక్క_FINAL_STRING_LENGTHS
స్థిర u_atomic_int32_t final_నిడివి_Counts[0x400];  // UnicodeString::kMaxShortLength+1
స్థిర u_atomic_int32_t beyond_లెక్క(0);

U_CAPI శూన్య unistr_printLengths() {
  int32_t i;
  కొరకు(i = 0; i <= 59; ++i) {
    రాయి("%2d,  %9d\n", i, (int32_t)final_నిడివి_Counts[i]);
  }
  int32_t beyond = beyond_లెక్క;
  కొరకు(; i < UPRV_LENGTHOF(final_నిడివి_Counts); ++i) {
    beyond += final_నిడివి_Counts[i];
  }
  రాయి(">59, %9d\n", beyond);
}
#అయితే_ముగింపు

UnicodeString::~UnicodeString()
{
#నిర్వచితమైతే UNISTR_లెక్క_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  ఒకవేళ((fUnion.fFields.f_నిడివి_And_Flags&(k_Open_Get_బఫరు|kReadonlyAlias|kWritableAlias)) == 0) {
    ఒకవేళ(has_Short_నిడివి()) {
      umtx_atomic_inc(final_నిడివి_Counts + get_Short_నిడివి());
    } లేకపోతే {
      umtx_atomic_inc(&beyond_లెక్క);
    }
  }
#అయితే_ముగింపు

  releaseArray();
}

//========================================
// Factory methods
//========================================

UnicodeString UnicodeString::fromUTF8(StringPiece utf8) {
  UnicodeString ఫలితం;
  ఫలితం.సమితి_గమ్యం_UTF8(utf8);
  తిరిగి ఫలితం;
}

UnicodeString UnicodeString::fromUTF32(స్థిరరాశి UChar32 *utf32, int32_t length) {
  UnicodeString ఫలితం;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  ఒకవేళ(length <= US_STACKBUF_కొలత) {
    capacity = US_STACKBUF_కొలత;
  } లేకపోతే {
    capacity = length + (length >> 4) + 4;
  }
  చేయి {
    char16_t *utf16 = ఫలితం.get_బఫరు(capacity);
    int32_t length16;
    UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, ఫలితం.get_సామర్థ్యం(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        శూన్యసూచిక,    // Don't care about number of substitutions.
        &error_సంకేతం);
    ఫలితం.release_బఫరు(length16);
    ఒకవేళ(error_సంకేతం == U_బఫరు_అధికప్రవాహం_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      కొనసాగు;
    } లేకపోతే ఒకవేళ(U_FAILURE(error_సంకేతం)) {
      ఫలితం.సమితి_గమ్యం_Bogus();
    }
    ఆపు;
  } అయితే(నిజం);
  తిరిగి ఫలితం;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::క్రియాకారకం=(స్థిరరాశి UnicodeString& src) U_LIFETIME_BOUND {
  తిరిగి copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(స్థిరరాశి UnicodeString& src) U_LIFETIME_BOUND {
  తిరిగి copyFrom(src, నిజం);
}

UnicodeString&
UnicodeString::copyFrom(స్థిరరాశి UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  ఒకవేళ(ఇది == &src) {
    తిరిగి *ఇది;
  }

  // is the right side bogus?
  ఒకవేళ(src.isBogus()) {
    సమితి_గమ్యం_Bogus();
    తిరిగి *ఇది;
  }

  // delete the current contents
  releaseArray();

  ఒకవేళ(src.is_ఖాళీలు()) {
    // empty string - use the stack buffer
    సమితి_గమ్యం_ఖాళీలు();
    తిరిగి *ఇది;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.f_నిడివి_And_Flags = src.fUnion.fFields.f_నిడివి_And_Flags;
  ఎంపిక(src.fUnion.fFields.f_నిడివి_And_Flags & kAllStorageFlags) {
  స్థితి kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_దొంతర_Fields.f_బఫరు, src.fUnion.f_దొంతర_Fields.f_బఫరు,
                get_Short_నిడివి() * U_SIZEOF_UCHAR);
    ఆపు;
  స్థితి kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).కూడు_Ref();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_సామర్థ్యం = src.fUnion.fFields.f_సామర్థ్యం;
    ఒకవేళ(!has_Short_నిడివి()) {
      fUnion.fFields.f_నిడివి = src.fUnion.fFields.f_నిడివి;
    }
    ఆపు;
  స్థితి kReadonlyAlias:
    ఒకవేళ(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.f_సామర్థ్యం = src.fUnion.fFields.f_సామర్థ్యం;
      ఒకవేళ(!has_Short_నిడివి()) {
        fUnion.fFields.f_నిడివి = src.fUnion.fFields.f_నిడివి;
      }
      ఆపు;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  స్థితి kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t src_నిడివి = src.length();
    ఒకవేళ(allocate(src_నిడివి)) {
      u_memcpy(get_Array_ప్రారంభం(), src.get_Array_ప్రారంభం(), src_నిడివి);
      సమితి_నిడివి(src_నిడివి);
      ఆపు;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  అప్రమేయ:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.f_నిడివి_And_Flags = kIsBogus;
    fUnion.fFields.fArray = శూన్యసూచిక;
    fUnion.fFields.f_సామర్థ్యం = 0;
    ఆపు;
  }

  తిరిగి *ఇది;
}

UnicodeString& UnicodeString::క్రియాకారకం=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, నిజం);
  తిరిగి *ఇది;
}

// Same as move assignment except without memory management.
శూన్య UnicodeString::copyFieldsFrom(UnicodeString &src, UBool సమితి_Src_గమ్యం_Bogus) noexcept {
  int16_t నిడివి_And_Flags = fUnion.fFields.f_నిడివి_And_Flags = src.fUnion.fFields.f_నిడివి_And_Flags;
  ఒకవేళ(నిడివి_And_Flags & k_Using_దొంతర_బఫరు) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    ఒకవేళ(ఇది != &src) {
      uprv_memcpy(fUnion.f_దొంతర_Fields.f_బఫరు, src.fUnion.f_దొంతర_Fields.f_బఫరు,
                  get_Short_నిడివి() * U_SIZEOF_UCHAR);
    }
  } లేకపోతే {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_సామర్థ్యం = src.fUnion.fFields.f_సామర్థ్యం;
    ఒకవేళ(!has_Short_నిడివి()) {
      fUnion.fFields.f_నిడివి = src.fUnion.fFields.f_నిడివి;
    }
    ఒకవేళ(సమితి_Src_గమ్యం_Bogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.f_నిడివి_And_Flags = kIsBogus;
      src.fUnion.fFields.fArray = శూన్యసూచిక;
      src.fUnion.fFields.f_సామర్థ్యం = 0;
    }
  }
}

శూన్య UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString తాత్కాలిక;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  తాత్కాలిక.copyFieldsFrom(*ఇది, అబద్ధం);
  ఇది->copyFieldsFrom(other, అబద్ధం);
  other.copyFieldsFrom(తాత్కాలిక, అబద్ధం);
  // Set temp to an empty string so that other's memory is not released twice.
  తాత్కాలిక.fUnion.fFields.f_నిడివి_And_Flags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() స్థిరరాశి {
    UnicodeString ఫలితం(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    ఒకవేళ (ఫలితం.isBogus()) {
        తిరిగి ఫలితం;
    }
    స్థిరరాశి char16_t *array = get_బఫరు();
    int32_t len = length();
    int32_t prev = 0;
    కొరకు (int32_t i=0;;) {
        ఒకవేళ (i == len) {
            ఫలితం.append(array, prev, len - prev);
            ఆపు;
        }
        ఒకవేళ (array[i++] == 0x5C /*'\\'*/) {
            ఫలితం.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            ఒకవేళ (c < 0) {
                ఫలితం.remove(); // return empty string
                ఆపు; // invalid escape sequence
            }
            ఫలితం.append(c);
            prev = i;
        }
    }
    తిరిగి ఫలితం;
}

UChar32 UnicodeString::unescapeAt(int32_t &స్థానభ్రంశం) స్థిరరాశి {
    తిరిగి u_unescapeAt(UnicodeString_charAt, &స్థానభ్రంశం, length(), (శూన్య*)ఇది);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(స్థిరరాశి char16_t *పాఠ్యం, int32_t len) స్థిరరాశి {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  తిరిగి uprv_memcmp(get_Array_ప్రారంభం(), పాఠ్యం, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t ప్రారంభం,
              int32_t length,
              స్థిరరాశి char16_t *srcChars,
              int32_t src_ప్రారంభం,
              int32_t src_నిడివి) స్థిరరాశి
{
  // compare illegal string values
  ఒకవేళ(isBogus()) {
    తిరిగి అబద్ధం;
  }
  
  // pin indices to legal values
  pinIndices(ప్రారంభం, length);

  ఒకవేళ(srcChars == శూన్యసూచిక) {
    // treat const char16_t *srcChars==nullptr as an empty string
    తిరిగి length == 0 ? నిజం : అబద్ధం;
  }

  // get the correct pointer
  స్థిరరాశి char16_t *chars = get_Array_ప్రారంభం();

  chars += ప్రారంభం;
  srcChars += src_ప్రారంభం;

  // get the srcLength if necessary
  ఒకవేళ(src_నిడివి < 0) {
    src_నిడివి = u_strlen(srcChars + src_ప్రారంభం);
  }

  ఒకవేళ (length != src_నిడివి) {
    తిరిగి అబద్ధం;
  }

  ఒకవేళ(length == 0 || chars == srcChars) {
    తిరిగి నిజం;
  }

  తిరిగి u_memcmp(chars, srcChars, src_నిడివి) == 0;
}

int8_t
UnicodeString::doCompare( int32_t ప్రారంభం,
              int32_t length,
              స్థిరరాశి char16_t *srcChars,
              int32_t src_ప్రారంభం,
              int32_t src_నిడివి) స్థిరరాశి
{
  // compare illegal string values
  ఒకవేళ(isBogus()) {
    తిరిగి -1;
  }
  
  // pin indices to legal values
  pinIndices(ప్రారంభం, length);

  ఒకవేళ(srcChars == శూన్యసూచిక) {
    // treat const char16_t *srcChars==nullptr as an empty string
    తిరిగి length == 0 ? 0 : 1;
  }

  // get the correct pointer
  స్థిరరాశి char16_t *chars = get_Array_ప్రారంభం();

  chars += ప్రారంభం;
  srcChars += src_ప్రారంభం;

  int32_t min_నిడివి;
  int8_t నిడివి_ఫలితం;

  // get the srcLength if necessary
  ఒకవేళ(src_నిడివి < 0) {
    src_నిడివి = u_strlen(srcChars + src_ప్రారంభం);
  }

  // are we comparing different lengths?
  ఒకవేళ(length != src_నిడివి) {
    ఒకవేళ(length < src_నిడివి) {
      min_నిడివి = length;
      నిడివి_ఫలితం = -1;
    } లేకపోతే {
      min_నిడివి = src_నిడివి;
      నిడివి_ఫలితం = 1;
    }
  } లేకపోతే {
    min_నిడివి = length;
    నిడివి_ఫలితం = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  ఒకవేళ(min_నిడివి > 0 && chars != srcChars) {
    int32_t ఫలితం;

#   ఒకవేళ U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      ఫలితం = uprv_memcmp(chars, srcChars, min_నిడివి * పరిమాణం(char16_t));
      ఒకవేళ(ఫలితం != 0) {
        తిరిగి (int8_t)(ఫలితం >> 15 | 1);
      }
#   లేకపోతే
      // little-endian: compare char16_t units
      చేయి {
        ఫలితం = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        ఒకవేళ(ఫలితం != 0) {
          తిరిగి static_cast<int8_t>(ఫలితం >> 15 | 1);
        }
      } అయితే(--min_నిడివి > 0);
#   endif
  }
  తిరిగి నిడివి_ఫలితం;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::do_Compare_సంకేతం_బిందువు_క్రమం(int32_t ప్రారంభం,
                                       int32_t length,
                                       స్థిరరాశి char16_t *srcChars,
                                       int32_t src_ప్రారంభం,
                                       int32_t src_నిడివి) స్థిరరాశి
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  ఒకవేళ(isBogus()) {
    తిరిగి -1;
  }

  // pin indices to legal values
  pinIndices(ప్రారంభం, length);

  ఒకవేళ(srcChars == శూన్యసూచిక) {
    src_ప్రారంభం = src_నిడివి = 0;
  }

  int32_t diff = uprv_strCompare(get_Array_ప్రారంభం() + ప్రారంభం, length, (srcChars!=శూన్యసూచిక)?(srcChars + src_ప్రారంభం):శూన్యసూచిక, src_నిడివి, అబద్ధం, నిజం);
  /* translate the 32-bit result into an 8-bit one */
  ఒకవేళ(diff!=0) {
    తిరిగి static_cast<int8_t>(diff >> 15 | 1);
  } లేకపోతే {
    తిరిగి 0;
  }
}

int32_t
UnicodeString::get_నిడివి() స్థిరరాశి {
    తిరిగి length();
}

char16_t
UnicodeString::getCharAt(int32_t స్థానభ్రంశం) స్థిరరాశి {
  తిరిగి charAt(స్థానభ్రంశం);
}

UChar32
UnicodeString::getChar32At(int32_t స్థానభ్రంశం) స్థిరరాశి {
  తిరిగి char32At(స్థానభ్రంశం);
}

UChar32
UnicodeString::char32At(int32_t స్థానభ్రంశం) స్థిరరాశి
{
  int32_t len = length();
  ఒకవేళ (static_cast<uint32_t>(స్థానభ్రంశం) < static_cast<uint32_t>(len)) {
    స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
    UChar32 c;
    U16_GET(array, 0, స్థానభ్రంశం, len, c);
    తిరిగి c;
  } లేకపోతే {
    తిరిగి kInvalidUChar;
  }
}

int32_t
UnicodeString::get_Char32_ప్రారంభం(int32_t స్థానభ్రంశం) స్థిరరాశి {
  ఒకవేళ (static_cast<uint32_t>(స్థానభ్రంశం) < static_cast<uint32_t>(length())) {
    స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
    U16_సమితి_CP_ప్రారంభం(array, 0, స్థానభ్రంశం);
    తిరిగి స్థానభ్రంశం;
  } లేకపోతే {
    తిరిగి 0;
  }
}

int32_t
UnicodeString::get_Char32_హద్దు(int32_t స్థానభ్రంశం) స్థిరరాశి {
  int32_t len = length();
  ఒకవేళ (static_cast<uint32_t>(స్థానభ్రంశం) < static_cast<uint32_t>(len)) {
    స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
    U16_సమితి_CP_హద్దు(array, 0, స్థానభ్రంశం, len);
    తిరిగి స్థానభ్రంశం;
  } లేకపోతే {
    తిరిగి len;
  }
}

int32_t
UnicodeString::లెక్క_Char32(int32_t ప్రారంభం, int32_t length) స్థిరరాశి {
  pinIndices(ప్రారంభం, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  తిరిగి u_లెక్క_Char32(get_Array_ప్రారంభం()+ప్రారంభం, length);
}

UBool
UnicodeString::has_మరిన్ని_Char32_Than(int32_t ప్రారంభం, int32_t length, int32_t number) స్థిరరాశి {
  pinIndices(ప్రారంభం, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  తిరిగి u_str_Has_మరిన్ని_Char32_Than(get_Array_ప్రారంభం()+ప్రారంభం, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t సూచీ, int32_t delta) స్థిరరాశి {
  // pin index
  int32_t len = length();
  ఒకవేళ(సూచీ<0) {
    సూచీ=0;
  } లేకపోతే ఒకవేళ(సూచీ>len) {
    సూచీ=len;
  }

  స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
  ఒకవేళ(delta>0) {
    U16_FWD_N(array, సూచీ, len, delta);
  } లేకపోతే {
    U16_వెనుక_N(array, 0, సూచీ, -delta);
  }

  తిరిగి సూచీ;
}

శూన్య
UnicodeString::do_వెలికితీయి(int32_t ప్రారంభం,
             int32_t length,
             char16_t *dst,
             int32_t dst_ప్రారంభం) స్థిరరాశి
{
  // pin indices to legal values
  pinIndices(ప్రారంభం, length);

  // do not copy anything if we alias dst itself
  స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
  ఒకవేళ(array + ప్రారంభం != dst + dst_ప్రారంభం) {
    us_arrayCopy(array, ప్రారంభం, dst, dst_ప్రారంభం, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t dest_సామర్థ్యం,
                       UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
  int32_t len = length();
  ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
    ఒకవేళ (isBogus() || dest_సామర్థ్యం < 0 || (dest_సామర్థ్యం > 0 && dest == శూన్యసూచిక)) {
      error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
    } లేకపోతే {
      స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
      ఒకవేళ(len>0 && len<=dest_సామర్థ్యం && array!=dest) {
        u_memcpy(dest, array, len);
      }
      తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, len, &error_సంకేతం);
    }
  }

  తిరిగి len;
}

int32_t
UnicodeString::extract(int32_t ప్రారంభం,
                       int32_t length,
                       అక్షరం *లక్ష్యం,
                       int32_t లక్ష్యం_సామర్థ్యం,
                       గణన EInvariant) స్థిరరాశి
{
  // if the arguments are illegal, then do nothing
  ఒకవేళ(లక్ష్యం_సామర్థ్యం < 0 || (లక్ష్యం_సామర్థ్యం > 0 && లక్ష్యం == శూన్యసూచిక)) {
    తిరిగి 0;
  }

  // pin the indices to legal values
  pinIndices(ప్రారంభం, length);

  ఒకవేళ(length <= లక్ష్యం_సామర్థ్యం) {
    u_UChars_గమ్యం_Chars(get_Array_ప్రారంభం() + ప్రారంభం, లక్ష్యం, length);
  }
  UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
  తిరిగి u_terminateChars(లక్ష్యం, లక్ష్యం_సామర్థ్యం, length, &స్థితి_2);
}

UnicodeString
UnicodeString::తాత్కాలిక_Sub_String(int32_t ప్రారంభం, int32_t len) స్థిరరాశి U_LIFETIME_BOUND {
  pinIndices(ప్రారంభం, len);
  స్థిరరాశి char16_t *array = get_బఫరు();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  ఒకవేళ(array==శూన్యసూచిక) {
    array=fUnion.f_దొంతర_Fields.f_బఫరు;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  తిరిగి UnicodeString(అబద్ధం, array + ప్రారంభం, len);
}

int32_t
UnicodeString::గమ్యం_UTF8(int32_t ప్రారంభం, int32_t len,
                      అక్షరం *లక్ష్యం, int32_t capacity) స్థిరరాశి {
  pinIndices(ప్రారంభం, len);
  int32_t length8;
  UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
  u_str_గమ్యం_UTF8_With_Sub(లక్ష్యం, capacity, &length8,
                     get_బఫరు() + ప్రారంభం, len,
                     0xFFFD,  // Standard substitution character.
                     శూన్యసూచిక,    // Don't care about number of substitutions.
                     &error_సంకేతం);
  తిరిగి length8;
}

#ఒకవేళ U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t ప్రారంభం, int32_t len,
                       అక్షరం *లక్ష్యం, uint32_t dst_కొలత) స్థిరరాశి {
  // if the arguments are illegal, then do nothing
  ఒకవేళ (/*dstSize < 0 || */(dst_కొలత > 0 && లక్ష్యం == శూన్యసూచిక)) {
    తిరిగి 0;
  }
  తిరిగి గమ్యం_UTF8(ప్రారంభం, len, లక్ష్యం, dst_కొలత <= 0x7fffffff ? static_cast<int32_t>(dst_కొలత) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#అయితే_ముగింపు

శూన్య 
UnicodeString::వెలికితీయి_Between(int32_t ప్రారంభం,
                  int32_t హద్దు,
                  UnicodeString& లక్ష్యం) స్థిరరాశి {
  pin_సూచీ(ప్రారంభం);
  pin_సూచీ(హద్దు);
  do_వెలికితీయి(ప్రారంభం, హద్దు - ప్రారంభం, లక్ష్యం);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
శూన్య
UnicodeString::గమ్యం_UTF8(ByteSink &sink) స్థిరరాశి {
  int32_t length16 = length();
  ఒకవేళ(length16 != 0) {
    అక్షరం దొంతర_బఫరు[1024];
    int32_t capacity = static_cast<int32_t>(పరిమాణం(దొంతర_బఫరు));
    UBool utf8IsOwned = అబద్ధం;
    అక్షరం *utf8 = sink.Get_Append_బఫరు(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      దొంతర_బఫరు, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
    u_str_గమ్యం_UTF8_With_Sub(utf8, capacity, &length8,
                       get_బఫరు(), length16,
                       0xFFFD,  // Standard substitution character.
                       శూన్యసూచిక,    // Don't care about number of substitutions.
                       &error_సంకేతం);
    ఒకవేళ(error_సంకేతం == U_బఫరు_అధికప్రవాహం_ERROR) {
      utf8 = static_cast<అక్షరం*>(uprv_malloc(length8));
      ఒకవేళ(utf8 != శూన్యసూచిక) {
        utf8IsOwned = నిజం;
        error_సంకేతం = U_ZERO_ERROR;
        u_str_గమ్యం_UTF8_With_Sub(utf8, length8, &length8,
                           get_బఫరు(), length16,
                           0xFFFD,  // Standard substitution character.
                           శూన్యసూచిక,    // Don't care about number of substitutions.
                           &error_సంకేతం);
      } లేకపోతే {
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
      }
    }
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    ఒకవేళ(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::గమ్యం_UTF32(UChar32 *utf32, int32_t capacity, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
  int32_t length32=0;
  ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_str_గమ్యం_UTF32_With_Sub(utf32, capacity, &length32,
        get_బఫరు(), length(),
        0xfffd,  // Substitution character.
        శూన్యసూచిక,    // Don't care about number of substitutions.
        &error_సంకేతం);
  }
  తిరిగి length32;
}

int32_t 
UnicodeString::సూచీ_యొక్క(స్థిరరాశి char16_t *srcChars,
               int32_t src_ప్రారంభం,
               int32_t src_నిడివి,
               int32_t ప్రారంభం,
               int32_t length) స్థిరరాశి
{
  ఒకవేళ (isBogus() || srcChars == శూన్యసూచిక || src_ప్రారంభం < 0 || src_నిడివి == 0) {
    తిరిగి -1;
  }

  // UnicodeString does not find empty substrings
  ఒకవేళ(src_నిడివి < 0 && srcChars[src_ప్రారంభం] == 0) {
    తిరిగి -1;
  }

  // get the indices within bounds
  pinIndices(ప్రారంభం, length);

  // find the first occurrence of the substring
  స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
  స్థిరరాశి char16_t *సరిపోలిక = u_str_కనుగొను_మొదటి(array + ప్రారంభం, length, srcChars + src_ప్రారంభం, src_నిడివి);
  ఒకవేళ(సరిపోలిక == శూన్యసూచిక) {
    తిరిగి -1;
  } లేకపోతే {
    తిరిగి static_cast<int32_t>(సరిపోలిక - array);
  }
}

int32_t
UnicodeString::do_సూచీ_యొక్క(char16_t c,
             int32_t ప్రారంభం,
             int32_t length) స్థిరరాశి
{
  // pin indices
  pinIndices(ప్రారంభం, length);

  // find the first occurrence of c
  స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
  స్థిరరాశి char16_t *సరిపోలిక = u_memchr(array + ప్రారంభం, c, length);
  ఒకవేళ(సరిపోలిక == శూన్యసూచిక) {
    తిరిగి -1;
  } లేకపోతే {
    తిరిగి static_cast<int32_t>(సరిపోలిక - array);
  }
}

int32_t
UnicodeString::do_సూచీ_యొక్క(UChar32 c,
                         int32_t ప్రారంభం,
                         int32_t length) స్థిరరాశి {
  // pin indices
  pinIndices(ప్రారంభం, length);

  // find the first occurrence of c
  స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
  స్థిరరాశి char16_t *సరిపోలిక = u_memchr32(array + ప్రారంభం, c, length);
  ఒకవేళ(సరిపోలిక == శూన్యసూచిక) {
    తిరిగి -1;
  } లేకపోతే {
    తిరిగి static_cast<int32_t>(సరిపోలిక - array);
  }
}

int32_t 
UnicodeString::చివరి_సూచీ_యొక్క(స్థిరరాశి char16_t *srcChars,
               int32_t src_ప్రారంభం,
               int32_t src_నిడివి,
               int32_t ప్రారంభం,
               int32_t length) స్థిరరాశి
{
  ఒకవేళ (isBogus() || srcChars == శూన్యసూచిక || src_ప్రారంభం < 0 || src_నిడివి == 0) {
    తిరిగి -1;
  }

  // UnicodeString does not find empty substrings
  ఒకవేళ(src_నిడివి < 0 && srcChars[src_ప్రారంభం] == 0) {
    తిరిగి -1;
  }

  // get the indices within bounds
  pinIndices(ప్రారంభం, length);

  // find the last occurrence of the substring
  స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
  స్థిరరాశి char16_t *సరిపోలిక = u_str_కనుగొను_చివరి(array + ప్రారంభం, length, srcChars + src_ప్రారంభం, src_నిడివి);
  ఒకవేళ(సరిపోలిక == శూన్యసూచిక) {
    తిరిగి -1;
  } లేకపోతే {
    తిరిగి static_cast<int32_t>(సరిపోలిక - array);
  }
}

int32_t
UnicodeString::do_చివరి_సూచీ_యొక్క(char16_t c,
                 int32_t ప్రారంభం,
                 int32_t length) స్థిరరాశి
{
  ఒకవేళ(isBogus()) {
    తిరిగి -1;
  }

  // pin indices
  pinIndices(ప్రారంభం, length);

  // find the last occurrence of c
  స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
  స్థిరరాశి char16_t *సరిపోలిక = u_memrchr(array + ప్రారంభం, c, length);
  ఒకవేళ(సరిపోలిక == శూన్యసూచిక) {
    తిరిగి -1;
  } లేకపోతే {
    తిరిగి static_cast<int32_t>(సరిపోలిక - array);
  }
}

int32_t
UnicodeString::do_చివరి_సూచీ_యొక్క(UChar32 c,
                             int32_t ప్రారంభం,
                             int32_t length) స్థిరరాశి {
  // pin indices
  pinIndices(ప్రారంభం, length);

  // find the last occurrence of c
  స్థిరరాశి char16_t *array = get_Array_ప్రారంభం();
  స్థిరరాశి char16_t *సరిపోలిక = u_memrchr32(array + ప్రారంభం, c, length);
  ఒకవేళ(సరిపోలిక == శూన్యసూచిక) {
    తిరిగి -1;
  } లేకపోతే {
    తిరిగి static_cast<int32_t>(సరిపోలిక - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::కనుగొను_And_Replace(int32_t ప్రారంభం,
                              int32_t length,
                              స్థిరరాశి UnicodeString& old_పాఠ్యం,
                              int32_t old_ప్రారంభం,
                              int32_t old_నిడివి,
                              స్థిరరాశి UnicodeString& new_పాఠ్యం,
                              int32_t new_ప్రారంభం,
                              int32_t new_నిడివి) U_LIFETIME_BOUND
{
  ఒకవేళ(isBogus() || old_పాఠ్యం.isBogus() || new_పాఠ్యం.isBogus()) {
    తిరిగి *ఇది;
  }

  pinIndices(ప్రారంభం, length);
  old_పాఠ్యం.pinIndices(old_ప్రారంభం, old_నిడివి);
  new_పాఠ్యం.pinIndices(new_ప్రారంభం, new_నిడివి);

  ఒకవేళ(old_నిడివి == 0) {
    తిరిగి *ఇది;
  }

  అయితే(length > 0 && length >= old_నిడివి) {
    int32_t pos = సూచీ_యొక్క(old_పాఠ్యం, old_ప్రారంభం, old_నిడివి, ప్రారంభం, length);
    ఒకవేళ(pos < 0) {
      // no more oldText's here: done
      ఆపు;
    } లేకపోతే {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, old_నిడివి, new_పాఠ్యం, new_ప్రారంభం, new_నిడివి);
      length -= pos + old_నిడివి - ప్రారంభం;
      ప్రారంభం = pos + new_నిడివి;
    }
  }

  తిరిగి *ఇది;
}


శూన్య
UnicodeString::సమితి_గమ్యం_Bogus()
{
  releaseArray();

  fUnion.fFields.f_నిడివి_And_Flags = kIsBogus;
  fUnion.fFields.fArray = శూన్యసూచిక;
  fUnion.fFields.f_సామర్థ్యం = 0;
}

// turn a bogus string into an empty one
శూన్య
UnicodeString::unBogus() {
  ఒకవేళ(fUnion.fFields.f_నిడివి_And_Flags & kIsBogus) {
    సమితి_గమ్యం_ఖాళీలు();
  }
}

స్థిరరాశి char16_t*
UnicodeString::get_Terminated_బఫరు() U_LIFETIME_BOUND {
  ఒకవేళ(!isWritable()) {
    తిరిగి శూన్యసూచిక;
  }
  char16_t *array = get_Array_ప్రారంభం();
  int32_t len = length();
  ఒకవేళ(len < get_సామర్థ్యం()) {
    ఒకవేళ(fUnion.fFields.f_నిడివి_And_Flags & k_బఫరు_Is_Readonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      ఒకవేళ(array[len] == 0) {
        తిరిగి array;
      }
    } లేకపోతే ఒకవేళ(((fUnion.fFields.f_నిడివి_And_Flags & kRefCounted) == 0 || ref_లెక్క() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      తిరిగి array;
    }
  }
  ఒకవేళ(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = get_Array_ప్రారంభం();
    array[len] = 0;
    తిరిగి array;
  } లేకపోతే {
    తిరిగి శూన్యసూచిక;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::సమితి_గమ్యం(UBool isTerminated,
                     ConstChar16Ptr పాఠ్యం_Ptr U_LIFETIME_BOUND,
                     int32_t పాఠ్యం_నిడివి) U_LIFETIME_BOUND
{
  ఒకవేళ(fUnion.fFields.f_నిడివి_And_Flags & k_Open_Get_బఫరు) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    తిరిగి *ఇది;
  }

  స్థిరరాశి char16_t *పాఠ్యం = పాఠ్యం_Ptr;
  ఒకవేళ(పాఠ్యం == శూన్యసూచిక) {
    // treat as an empty string, do not alias
    releaseArray();
    సమితి_గమ్యం_ఖాళీలు();
    తిరిగి *ఇది;
  }

  ఒకవేళ( పాఠ్యం_నిడివి < -1 ||
      (పాఠ్యం_నిడివి == -1 && !isTerminated) ||
      (పాఠ్యం_నిడివి >= 0 && isTerminated && పాఠ్యం[పాఠ్యం_నిడివి] != 0)
  ) {
    సమితి_గమ్యం_Bogus();
    తిరిగి *ఇది;
  }

  releaseArray();

  ఒకవేళ(పాఠ్యం_నిడివి == -1) {
    // text is terminated, or else it would have failed the above test
    పాఠ్యం_నిడివి = u_strlen(పాఠ్యం);
  }
  fUnion.fFields.f_నిడివి_And_Flags = kReadonlyAlias;
  సమితి_Array(const_cast<char16_t*>(పాఠ్యం), పాఠ్యం_నిడివి, isTerminated ? పాఠ్యం_నిడివి + 1 : పాఠ్యం_నిడివి);
  తిరిగి *ఇది;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::సమితి_గమ్యం(char16_t* బఫరు,
                     int32_t buff_నిడివి U_LIFETIME_BOUND,
                     int32_t buff_సామర్థ్యం) U_LIFETIME_BOUND {
  ఒకవేళ(fUnion.fFields.f_నిడివి_And_Flags & k_Open_Get_బఫరు) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    తిరిగి *ఇది;
  }

  ఒకవేళ(బఫరు == శూన్యసూచిక) {
    // treat as an empty string, do not alias
    releaseArray();
    సమితి_గమ్యం_ఖాళీలు();
    తిరిగి *ఇది;
  }

  ఒకవేళ(buff_నిడివి < -1 || buff_సామర్థ్యం < 0 || buff_నిడివి > buff_సామర్థ్యం) {
    సమితి_గమ్యం_Bogus();
    తిరిగి *ఇది;
  } లేకపోతే ఒకవేళ(buff_నిడివి == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    స్థిరరాశి char16_t *p = బఫరు, *హద్దు = బఫరు + buff_సామర్థ్యం;
    అయితే(p != హద్దు && *p != 0) {
      ++p;
    }
    buff_నిడివి = static_cast<int32_t>(p - బఫరు);
  }

  releaseArray();

  fUnion.fFields.f_నిడివి_And_Flags = kWritableAlias;
  సమితి_Array(బఫరు, buff_నిడివి, buff_సామర్థ్యం);
  తిరిగి *ఇది;
}

UnicodeString& UnicodeString::సమితి_గమ్యం_UTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  ఒకవేళ(length <= US_STACKBUF_కొలత) {
    capacity = US_STACKBUF_కొలత;
  } లేకపోతే {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = get_బఫరు(capacity);
  int32_t length16;
  UError_సంకేతం error_సంకేతం = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, get_సామర్థ్యం(), &length16,
      utf8.దత్తాంశం(), length,
      0xfffd,  // Substitution character.
      శూన్యసూచిక,    // Don't care about number of substitutions.
      &error_సంకేతం);
  release_బఫరు(length16);
  ఒకవేళ(U_FAILURE(error_సంకేతం)) {
    సమితి_గమ్యం_Bogus();
  }
  తిరిగి *ఇది;
}

UnicodeString&
UnicodeString::సమితి_Char_At(int32_t స్థానభ్రంశం, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  ఒకవేళ(cloneArrayIfNeeded() && len > 0) {
    ఒకవేళ(స్థానభ్రంశం < 0) {
      స్థానభ్రంశం = 0;
    } లేకపోతే ఒకవేళ(స్థానభ్రంశం >= len) {
      స్థానభ్రంశం = len - 1;
    }

    get_Array_ప్రారంభం()[స్థానభ్రంశం] = c;
  }
  తిరిగి *ఇది;
}

UnicodeString&
UnicodeString::replace(int32_t ప్రారంభం,
                       int32_t నిడివి_4,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t బఫరు[U16_MAX_నిడివి];
  int32_t count = 0;
  UBool isError = అబద్ధం;
  U16_APPEND(బఫరు, count, U16_MAX_నిడివి, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  తిరిగి doReplace(ప్రారంభం, నిడివి_4, బఫరు, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t బఫరు[U16_MAX_నిడివి];
  int32_t నిడివి_4 = 0;
  UBool isError = అబద్ధం;
  U16_APPEND(బఫరు, నిడివి_4, U16_MAX_నిడివి, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  తిరిగి isError ? *ఇది : doAppend(బఫరు, 0, నిడివి_4);
}

UnicodeString&
UnicodeString::doReplace(int32_t ప్రారంభం,
                         int32_t length,
                         స్థిరరాశి UnicodeString& src,
                         int32_t src_ప్రారంభం,
                         int32_t src_నిడివి) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(src_ప్రారంభం, src_నిడివి);

  // get the characters from src
  // and replace the range in ourselves with them
  తిరిగి doReplace(ప్రారంభం, length, src.get_Array_ప్రారంభం(), src_ప్రారంభం, src_నిడివి);
}

UnicodeString&
UnicodeString::doReplace(int32_t ప్రారంభం,
                         int32_t length,
                         స్థిరరాశి char16_t* srcChars,
                         int32_t src_ప్రారంభం,
                         int32_t src_నిడివి) U_LIFETIME_BOUND
{
  ఒకవేళ(!isWritable()) {
    తిరిగి *ఇది;
  }

  int32_t old_నిడివి = ఇది->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  ఒకవేళ((fUnion.fFields.f_నిడివి_And_Flags&k_బఫరు_Is_Readonly) && src_నిడివి == 0) {
    ఒకవేళ(ప్రారంభం == 0) {
      // remove prefix by adjusting the array pointer
      pin_సూచీ(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.f_సామర్థ్యం -= length;
      సమితి_నిడివి(old_నిడివి - length);
      తిరిగి *ఇది;
    } లేకపోతే {
      pin_సూచీ(ప్రారంభం);
      ఒకవేళ(length >= (old_నిడివి - ప్రారంభం)) {
        // remove suffix by reducing the length (like truncate())
        సమితి_నిడివి(ప్రారంభం);
        fUnion.fFields.f_సామర్థ్యం = ప్రారంభం;  // not NUL-terminated any more
        తిరిగి *ఇది;
      }
    }
  }

  ఒకవేళ(ప్రారంభం == old_నిడివి) {
    తిరిగి doAppend(srcChars, src_ప్రారంభం, src_నిడివి);
  }

  ఒకవేళ (srcChars == శూన్యసూచిక) {
    src_నిడివి = 0;
  } లేకపోతే {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += src_ప్రారంభం;
    ఒకవేళ (src_నిడివి < 0) {
      // get the srcLength if necessary
      src_నిడివి = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(ప్రారంభం, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t new_నిడివి = old_నిడివి - length;
  ఒకవేళ(src_నిడివి > (INT32_MAX - new_నిడివి)) {
    సమితి_గమ్యం_Bogus();
    తిరిగి *ఇది;
  }
  new_నిడివి += src_నిడివి;

  // Check for insertion into ourself
  స్థిరరాశి char16_t *oldArray = get_Array_ప్రారంభం();
  ఒకవేళ (is_బఫరు_Writable() &&
      oldArray < srcChars + src_నిడివి &&
      srcChars < oldArray + old_నిడివి) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, src_నిడివి);
    ఒకవేళ (copy.isBogus()) {
      సమితి_గమ్యం_Bogus();
      తిరిగి *ఇది;
    }
    తిరిగి doReplace(ప్రారంభం, length, copy.get_Array_ప్రారంభం(), 0, src_నిడివి);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_దొంతర_బఫరు[US_STACKBUF_కొలత];
  ఒకవేళ((fUnion.fFields.f_నిడివి_And_Flags&k_Using_దొంతర_బఫరు) && (new_నిడివి > US_STACKBUF_కొలత)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_దొంతర_బఫరు, oldArray, old_నిడివి);
    oldArray = old_దొంతర_బఫరు;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *బఫరు_గమ్యం_Delete = శూన్యసూచిక;
  ఒకవేళ(!cloneArrayIfNeeded(new_నిడివి, get_Grow_సామర్థ్యం(new_నిడివి),
                         అబద్ధం, &బఫరు_గమ్యం_Delete)
  ) {
    తిరిగి *ఇది;
  }

  // now do the replace

  char16_t *newArray = get_Array_ప్రారంభం();
  ఒకవేళ(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, ప్రారంభం);
    us_arrayCopy(oldArray, ప్రారంభం + length,
                 newArray, ప్రారంభం + src_నిడివి,
                 old_నిడివి - (ప్రారంభం + length));
  } లేకపోతే ఒకవేళ(length != src_నిడివి) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, ప్రారంభం + length,
                 newArray, ప్రారంభం + src_నిడివి,
                 old_నిడివి - (ప్రారంభం + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, ప్రారంభం, src_నిడివి);

  సమితి_నిడివి(new_నిడివి);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  ఒకవేళ (బఫరు_గమ్యం_Delete) {
    uprv_free(బఫరు_గమ్యం_Delete);
  }

  తిరిగి *ఇది;
}

UnicodeString&
UnicodeString::doReplace(int32_t ప్రారంభం, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  ఒకవేళ (!isWritable()) {
    తిరిగి *ఇది;
  }
  ఒకవేళ (src.length() > INT32_MAX) {
    సమితి_గమ్యం_Bogus();
    తిరిగి *ఇది;
  }
  తిరిగి doReplace(ప్రారంభం, length, src.దత్తాంశం(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(స్థిరరాశి UnicodeString& src,
                        int32_t src_ప్రారంభం,
                        int32_t src_నిడివి) U_LIFETIME_BOUND {
  ఒకవేళ(src_నిడివి == 0) {
    తిరిగి *ఇది;
  }

  // pin the indices to legal values
  src.pinIndices(src_ప్రారంభం, src_నిడివి);
  తిరిగి doAppend(src.get_Array_ప్రారంభం(), src_ప్రారంభం, src_నిడివి);
}

UnicodeString&
UnicodeString::doAppend(స్థిరరాశి char16_t* srcChars,
                        int32_t src_ప్రారంభం,
                        int32_t src_నిడివి) U_LIFETIME_BOUND {
  ఒకవేళ(!isWritable() || src_నిడివి == 0 || srcChars == శూన్యసూచిక) {
    తిరిగి *ఇది;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += src_ప్రారంభం;

  ఒకవేళ(src_నిడివి < 0) {
    // get the srcLength if necessary
    ఒకవేళ((src_నిడివి = u_strlen(srcChars)) == 0) {
      తిరిగి *ఇది;
    }
  }

  int32_t old_నిడివి = length();
  int32_t new_నిడివి;

  ఒకవేళ (src_నిడివి <= get_సామర్థ్యం() - old_నిడివి && is_బఫరు_Writable()) {
    new_నిడివి = old_నిడివి + src_నిడివి;
    // Faster than a memmove
    ఒకవేళ (src_నిడివి <= 4) {
      char16_t *arr = get_Array_ప్రారంభం();
      arr[old_నిడివి] = srcChars[0];
      ఒకవేళ (src_నిడివి > 1) arr[old_నిడివి+1] = srcChars[1];
      ఒకవేళ (src_నిడివి > 2) arr[old_నిడివి+2] = srcChars[2];
      ఒకవేళ (src_నిడివి > 3) arr[old_నిడివి+3] = srcChars[3];
      సమితి_నిడివి(new_నిడివి);
      తిరిగి *ఇది;
    }
  } లేకపోతే {
    ఒకవేళ (uprv_add32_అధికప్రవాహం(old_నిడివి, src_నిడివి, &new_నిడివి)) {
      సమితి_గమ్యం_Bogus();
      తిరిగి *ఇది;
    }

    // Check for append onto ourself
    స్థిరరాశి char16_t* oldArray = get_Array_ప్రారంభం();
    ఒకవేళ (is_బఫరు_Writable() &&
        oldArray < srcChars + src_నిడివి &&
        srcChars < oldArray + old_నిడివి) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, src_నిడివి);
      ఒకవేళ (copy.isBogus()) {
        సమితి_గమ్యం_Bogus();
        తిరిగి *ఇది;
      }
      తిరిగి doAppend(copy.get_Array_ప్రారంభం(), 0, src_నిడివి);
    }

    // optimize append() onto a large-enough, owned string
    ఒకవేళ (!cloneArrayIfNeeded(new_నిడివి, get_Grow_సామర్థ్యం(new_నిడివి))) {
      తిరిగి *ఇది;
    }
  }

  char16_t *newArray = get_Array_ప్రారంభం();
  // Do not copy characters when
  //   char16_t *buffer=str.getAppendBuffer(...);
  // is followed by
  //   str.append(buffer, length);
  // or
  //   str.appendString(buffer, length)
  // or similar.
  ఒకవేళ(srcChars != newArray + old_నిడివి) {
    us_arrayCopy(srcChars, 0, newArray, old_నిడివి, src_నిడివి);
  }
  సమితి_నిడివి(new_నిడివి);

  తిరిగి *ఇది;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  ఒకవేళ (!isWritable() || src.empty()) {
    తిరిగి *ఇది;
  }
  ఒకవేళ (src.length() > INT32_MAX) {
    సమితి_గమ్యం_Bogus();
    తిరిగి *ఇది;
  }
  తిరిగి doAppend(src.దత్తాంశం(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
శూన్య
UnicodeString::handleReplaceBetween(int32_t ప్రారంభం,
                                    int32_t హద్దు,
                                    స్థిరరాశి UnicodeString& పాఠ్యం) {
    replaceBetween(ప్రారంభం, హద్దు, పాఠ్యం);
}

/**
 * Replaceable API
 */
శూన్య 
UnicodeString::copy(int32_t ప్రారంభం, int32_t హద్దు, int32_t dest) {
    ఒకవేళ (హద్దు <= ప్రారంభం) {
        తిరిగి; // Nothing to do; avoid bogus malloc call
    }
    char16_t* పాఠ్యం = static_cast<char16_t*>(uprv_malloc(పరిమాణం(char16_t) * (హద్దు - ప్రారంభం)));
    // Check to make sure text is not null.
    ఒకవేళ (పాఠ్యం != శూన్యసూచిక) {
	    వెలికితీయి_Between(ప్రారంభం, హద్దు, పాఠ్యం, 0);
	    insert(dest, పాఠ్యం, 0, హద్దు - ప్రారంభం);    
	    uprv_free(పాఠ్యం);
    }
}

/**
 * Replaceable API
 *
 * NOTE: This is for the Replaceable class.  There is no rep.cpp,
 * so we implement this function here.
 */
UBool Replaceable::has_Meta_దత్తాంశం() స్థిరరాశి {
    తిరిగి నిజం;
}

/**
 * Replaceable API
 */
UBool UnicodeString::has_Meta_దత్తాంశం() స్థిరరాశి {
    తిరిగి అబద్ధం;
}

UnicodeString&
UnicodeString::do_తిరగేయి(int32_t ప్రారంభం, int32_t length) U_LIFETIME_BOUND {
  ఒకవేళ(length <= 1 || !cloneArrayIfNeeded()) {
    తిరిగి *ఇది;
  }

  // pin the indices to legal values
  pinIndices(ప్రారంభం, length);
  ఒకవేళ(length <= 1) {  // pinIndices() might have shrunk the length
    తిరిగి *ఇది;
  }

  char16_t *ఎడమ = get_Array_ప్రారంభం() + ప్రారంభం;
  char16_t *కుడి = ఎడమ + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = అబద్ధం;

  // Before the loop we know left<right because length>=2.
  చేయి {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *ఎడమ));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*ఎడమ++ = *కుడి));
    *కుడి-- = swap;
  } అయితే(ఎడమ < కుడి);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*ఎడమ));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  ఒకవేళ(hasSupplementary) {
    char16_t swap2;

    ఎడమ = get_Array_ప్రారంభం() + ప్రారంభం;
    కుడి = ఎడమ + length - 1; // -1 so that we can look at *(left+1) if left<right
    అయితే(ఎడమ < కుడి) {
      ఒకవేళ(U16_IS_TRAIL(swap = *ఎడమ) && U16_IS_LEAD(swap2 = *(ఎడమ + 1))) {
        *ఎడమ++ = swap2;
        *ఎడమ++ = swap;
      } లేకపోతే {
        ++ఎడమ;
      }
    }
  }

  తిరిగి *ఇది;
}

UBool 
UnicodeString::padLeading(int32_t లక్ష్యం_నిడివి,
                          char16_t padChar)
{
  int32_t old_నిడివి = length();
  ఒకవేళ(old_నిడివి >= లక్ష్యం_నిడివి || !cloneArrayIfNeeded(లక్ష్యం_నిడివి)) {
    తిరిగి అబద్ధం;
  } లేకపోతే {
    // move contents up by padding width
    char16_t *array = get_Array_ప్రారంభం();
    int32_t ప్రారంభం = లక్ష్యం_నిడివి - old_నిడివి;
    us_arrayCopy(array, 0, array, ప్రారంభం, old_నిడివి);

    // fill in padding character
    అయితే(--ప్రారంభం >= 0) {
      array[ప్రారంభం] = padChar;
    }
    సమితి_నిడివి(లక్ష్యం_నిడివి);
    తిరిగి నిజం;
  }
}

UBool 
UnicodeString::padTrailing(int32_t లక్ష్యం_నిడివి,
                           char16_t padChar)
{
  int32_t old_నిడివి = length();
  ఒకవేళ(old_నిడివి >= లక్ష్యం_నిడివి || !cloneArrayIfNeeded(లక్ష్యం_నిడివి)) {
    తిరిగి అబద్ధం;
  } లేకపోతే {
    // fill in padding character
    char16_t *array = get_Array_ప్రారంభం();
    int32_t length = లక్ష్యం_నిడివి;
    అయితే(--length >= old_నిడివి) {
      array[length] = padChar;
    }
    సమితి_నిడివి(లక్ష్యం_నిడివి);
    తిరిగి నిజం;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::do_Hash_సంకేతం() స్థిరరాశి
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hash_సంకేతం = ustr_hashUCharsN(get_Array_ప్రారంభం(), length());
    ఒకవేళ (hash_సంకేతం == k_Invalid_Hash_సంకేతం) {
        hash_సంకేతం = k_ఖాళీలు_Hash_సంకేతం;
    }
    తిరిగి hash_సంకేతం;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::get_బఫరు(int32_t min_సామర్థ్యం) U_LIFETIME_BOUND {
  ఒకవేళ(min_సామర్థ్యం>=-1 && cloneArrayIfNeeded(min_సామర్థ్యం)) {
    fUnion.fFields.f_నిడివి_And_Flags|=k_Open_Get_బఫరు;
    సమితి_Zero_నిడివి();
    తిరిగి get_Array_ప్రారంభం();
  } లేకపోతే {
    తిరిగి శూన్యసూచిక;
  }
}

శూన్య
UnicodeString::release_బఫరు(int32_t new_నిడివి) {
  ఒకవేళ(fUnion.fFields.f_నిడివి_And_Flags&k_Open_Get_బఫరు && new_నిడివి>=-1) {
    // set the new fLength
    int32_t capacity=get_సామర్థ్యం();
    ఒకవేళ(new_నిడివి==-1) {
      // the new length is the string length, capped by fCapacity
      స్థిరరాశి char16_t *array=get_Array_ప్రారంభం(), *p=array, *హద్దు=array+capacity;
      అయితే(p<హద్దు && *p!=0) {
        ++p;
      }
      new_నిడివి = static_cast<int32_t>(p - array);
    } లేకపోతే ఒకవేళ(new_నిడివి>capacity) {
      new_నిడివి=capacity;
    }
    సమితి_నిడివి(new_నిడివి);
    fUnion.fFields.f_నిడివి_And_Flags&=~k_Open_Get_బఫరు;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t new_సామర్థ్యం,
                                  int32_t grow_సామర్థ్యం,
                                  UBool doCopyArray,
                                  int32_t **p_బఫరు_గమ్యం_Delete,
                                  UBool forceClone) {
  // default parameters need to be static, therefore
  // the defaults are -1 to have convenience defaults
  ఒకవేళ(new_సామర్థ్యం == -1) {
    new_సామర్థ్యం = get_సామర్థ్యం();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  ఒకవేళ(!isWritable()) {
    తిరిగి అబద్ధం;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  ఒకవేళ(forceClone ||
     fUnion.fFields.f_నిడివి_And_Flags & k_బఫరు_Is_Readonly ||
     (fUnion.fFields.f_నిడివి_And_Flags & kRefCounted && ref_లెక్క() > 1) ||
     new_సామర్థ్యం > get_సామర్థ్యం()
  ) {
    // check growCapacity for default value and use of the stack buffer
    ఒకవేళ(grow_సామర్థ్యం < 0) {
      grow_సామర్థ్యం = new_సామర్థ్యం;
    } లేకపోతే ఒకవేళ(new_సామర్థ్యం <= US_STACKBUF_కొలత && grow_సామర్థ్యం > US_STACKBUF_కొలత) {
      grow_సామర్థ్యం = US_STACKBUF_కొలత;
    } లేకపోతే ఒకవేళ(new_సామర్థ్యం > grow_సామర్థ్యం) {
      సమితి_గమ్యం_Bogus();
      తిరిగి అబద్ధం;  // bad inputs
    }
    ఒకవేళ(grow_సామర్థ్యం > k_Max_సామర్థ్యం) {
      సమితి_గమ్యం_Bogus();
      తిరిగి అబద్ధం;
    }

    // save old values
    char16_t old_దొంతర_బఫరు[US_STACKBUF_కొలత];
    char16_t *oldArray;
    int32_t old_నిడివి = length();
    int16_t flags = fUnion.fFields.f_నిడివి_And_Flags;

    ఒకవేళ(flags&k_Using_దొంతర_బఫరు) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      ఒకవేళ(doCopyArray && grow_సామర్థ్యం > US_STACKBUF_కొలత) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_దొంతర_Fields.f_బఫరు, 0, old_దొంతర_బఫరు, 0, old_నిడివి);
        oldArray = old_దొంతర_బఫరు;
      } లేకపోతే {
        oldArray = శూన్యసూచిక; // no need to copy from the stack buffer to itself
      }
    } లేకపోతే {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=శూన్యసూచిక); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    ఒకవేళ(allocate(grow_సామర్థ్యం) ||
       (new_సామర్థ్యం < grow_సామర్థ్యం && allocate(new_సామర్థ్యం))
    ) {
      ఒకవేళ(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t min_నిడివి = old_నిడివి;
        new_సామర్థ్యం = get_సామర్థ్యం();
        ఒకవేళ(new_సామర్థ్యం < min_నిడివి) {
          min_నిడివి = new_సామర్థ్యం;
        }
        ఒకవేళ(oldArray != శూన్యసూచిక) {
          us_arrayCopy(oldArray, 0, get_Array_ప్రారంభం(), 0, min_నిడివి);
        }
        సమితి_నిడివి(min_నిడివి);
      } లేకపోతే {
        సమితి_Zero_నిడివి();
      }

      // release the old array
      ఒకవేళ(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_లెక్క = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        ఒకవేళ(umtx_atomic_dec(p_Ref_లెక్క) == 0) {
          ఒకవేళ (p_బఫరు_గమ్యం_Delete == శూన్యసూచిక) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((శూన్య *)p_Ref_లెక్క);
          } లేకపోతే {
            // the caller requested to delete it himself
            *p_బఫరు_గమ్యం_Delete = reinterpret_cast<int32_t*>(p_Ref_లెక్క);
          }
        }
      }
    } లేకపోతే {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      ఒకవేళ(!(flags&k_Using_దొంతర_బఫరు)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.f_నిడివి_And_Flags = flags;
      సమితి_గమ్యం_Bogus();
      తిరిగి అబద్ధం;
    }
  }
  తిరిగి నిజం;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::append_సంకేతం_Unit(char16_t c) {
  తిరిగి str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::append_సంకేతం_బిందువు(UChar32 c) {
  char16_t బఫరు[U16_MAX_నిడివి];
  int32_t c_నిడివి = 0;
  UBool isError = అబద్ధం;
  U16_APPEND(బఫరు, c_నిడివి, U16_MAX_నిడివి, c, isError);
  తిరిగి !isError && str.doAppend(బఫరు, 0, c_నిడివి).isWritable();
}

UBool
UnicodeStringAppendable::appendString(స్థిరరాశి char16_t *s, int32_t length) {
  తిరిగి str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserve_Append_సామర్థ్యం(int32_t append_సామర్థ్యం) {
  తిరిగి str.cloneArrayIfNeeded(str.length() + append_సామర్థ్యం);
}

char16_t* UnicodeStringAppendable::get_Append_బఫరు(int32_t min_సామర్థ్యం,
                                                   int32_t desired_సామర్థ్యం_Hint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratch_సామర్థ్యం,
                                                   int32_t* ఫలితం_సామర్థ్యం) U_LIFETIME_BOUND {
  ఒకవేళ(min_సామర్థ్యం < 1 || scratch_సామర్థ్యం < min_సామర్థ్యం) {
    *ఫలితం_సామర్థ్యం = 0;
    తిరిగి శూన్యసూచిక;
  }
  int32_t old_నిడివి = str.length();
  ఒకవేళ(min_సామర్థ్యం <= (k_Max_సామర్థ్యం - old_నిడివి) &&
      desired_సామర్థ్యం_Hint <= (k_Max_సామర్థ్యం - old_నిడివి) &&
      str.cloneArrayIfNeeded(old_నిడివి + min_సామర్థ్యం, old_నిడివి + desired_సామర్థ్యం_Hint)) {
    *ఫలితం_సామర్థ్యం = str.get_సామర్థ్యం() - old_నిడివి;
    తిరిగి str.get_Array_ప్రారంభం() + old_నిడివి;
  }
  *ఫలితం_సామర్థ్యం = scratch_సామర్థ్యం;
  తిరిగి scratch;
}

U_NAMESPACE_చివర

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(స్థిరరాశి UElement కీలకం) {
    స్థిరరాశి UnicodeString *str = (స్థిరరాశి UnicodeString*) కీలకం.సూచిక;
    తిరిగి (str == శూన్యసూచిక) ? 0 : str->hash_సంకేతం();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(స్థిరరాశి UElement key1, స్థిరరాశి UElement key2) {
    స్థిరరాశి UnicodeString *str1 = (స్థిరరాశి UnicodeString*) key1.సూచిక;
    స్థిరరాశి UnicodeString *str2 = (స్థిరరాశి UnicodeString*) key2.సూచిక;
    ఒకవేళ (str1 == str2) {
        తిరిగి నిజం;
    }
    ఒకవేళ (str1 == శూన్యసూచిక || str2 == శూన్యసూచిక) {
        తిరిగి అబద్ధం;
    }
    తిరిగి *str1 == *str2;
}

#నిర్వచితమైతే U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#ఒకవేళ defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#సూచన GCC diagnostic push
#సూచన GCC diagnostic ignored "-Wunused-function"
స్థిర శూన్య uprv_UnicodeStringDummy() {
    తొలగించు [] (కొత్త UnicodeString[2]);
}
#సూచన GCC diagnostic pop
#అయితే_ముగింపు
#అయితే_ముగింపు
