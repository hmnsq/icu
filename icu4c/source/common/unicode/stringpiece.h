// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
// Copyright (C) 2009-2013, International Business Machines
// Corporation and others. All Rights Reserved.
//
// Copyright 2001 and onwards Google Inc.
// Author: Sanjay Ghemawat

// This code is a contribution of Google code, and the style used here is
// a compromise between the original Google code and the ICU coding guidelines.
// For example, data types are ICU-ified (size_t,int->int32_t),
// and API comments doxygen-ified, but function names and behavior are
// as in the original, if possible.
// Assertion-style error handling, not available in ICU, was changed to
// parameter "pinning" similar to UnicodeString.
//
// In addition, this is only a partial port of the original Google code,
// limited to what was needed so far. The (nearly) complete original code
// is in the ICU svn repository at icuhtml/trunk/design/strings/contrib
// (see ICU ticket 6765, r25517).

#నిర్వచితం_కాకపోతే __STRINGPIECE_H__
#నిర్వచించు __STRINGPIECE_H__

/**
 * \file 
 * \brief C++ API: StringPiece: Read-only byte string wrapper class.
 */

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ <cstddef>
#అన్తర్భావ <string_view>
#అన్తర్భావ <type_traits>

#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/std_string.h"

// Arghh!  I wish C++ literals were "string".

U_NAMESPACE_BEGIN

/**
 * A string-like object that points to a sized piece of memory.
 *
 * We provide non-explicit singleton constructors so users can pass
 * in a "const char*" or a "string" wherever a "StringPiece" is
 * expected.
 *
 * Functions or methods may use StringPiece parameters to accept either a
 * "const char*" or a "string" value that will be implicitly converted to a
 * StringPiece.
 *
 * Systematic usage of StringPiece is encouraged as it will reduce unnecessary
 * conversions from "const char*" to "string" and back again.
 *
 * @stable ICU 4.2
 */
class U_COMMON_API StringPiece : బహిరంగ UMemory {
 అంతరంగ:
  స్థిరరాశి అక్షరం*   ptr_;
  int32_t       నిడివి_2;

 బహిరంగ:
  /**
   * Default constructor, creates an empty StringPiece.
   * @stable ICU 4.2
   */
  StringPiece() : ptr_(nullptr), నిడివి_2(0) { }

  /**
   * Constructs from a NUL-terminated const char * pointer.
   * @param str a NUL-terminated const char * pointer
   * @stable ICU 4.2
   */
  StringPiece(స్థిరరాశి అక్షరం* str U_LIFETIME_BOUND);
#ఒకవేళ defined(__cpp_char8_t) || defined(U_లోపల_DOXYGEN)
  /**
   * Constructs from a NUL-terminated const char8_t * pointer.
   * @param str a NUL-terminated const char8_t * pointer
   * @stable ICU 67
   */
  StringPiece(స్థిరరాశి char8_t* str U_LIFETIME_BOUND) : StringPiece(reinterpret_cast<స్థిరరాశి అక్షరం*>(str)) {}
#అయితే_ముగింపు
  /**
   * Constructs an empty StringPiece.
   * Needed for type disambiguation from multiple other overloads.
   * @param p nullptr
   * @stable ICU 67
   */
  StringPiece(std::nullptr_t p) : ptr_(p), నిడివి_2(0) {}

  /**
   * Constructs from a std::string.
   * @stable ICU 4.2
   */
  StringPiece(స్థిరరాశి std::string& str U_LIFETIME_BOUND)
    : ptr_(str.దత్తాంశం()), నిడివి_2(static_cast<int32_t>(str.కొలత_2())) { }
#ఒకవేళ defined(__cpp_lib_char8_t) || defined(U_లోపల_DOXYGEN)
  /**
   * Constructs from a std::u8string.
   * @stable ICU 67
   */
  StringPiece(స్థిరరాశి std::u8string& str U_LIFETIME_BOUND)
    : ptr_(reinterpret_cast<స్థిరరాశి అక్షరం*>(str.దత్తాంశం())),
      నిడివి_2(static_cast<int32_t>(str.కొలత_2())) { }
#అయితే_ముగింపు

  /**
   * Constructs from some other implementation of a string piece class, from any
   * C++ record type that has these two methods:
   *
   * \code{.cpp}
   *
   *   struct OtherStringPieceClass {
   *     const char* data();  // or const char8_t*
   *     size_t size();
   *   };
   *
   * \endcode
   *
   * The other string piece class will typically be std::string_view from C++17
   * or absl::string_view from Abseil.
   *
   * Starting with C++20, data() may also return a const char8_t* pointer,
   * as from std::u8string_view.
   *
   * @param str the other string piece
   * @stable ICU 65
   */
  మూస <typename T,
            typename = std::enable_if_t<
                (std::is_same_v<decltype(T().దత్తాంశం()), స్థిరరాశి అక్షరం*>
#ఒకవేళ defined(__cpp_char8_t)
                    || std::is_same_v<decltype(T().దత్తాంశం()), స్థిరరాశి char8_t*>
#అయితే_ముగింపు
                ) &&
                std::is_same_v<decltype(T().కొలత_2()), size_t>>>
  StringPiece(T str U_LIFETIME_BOUND)
      : ptr_(reinterpret_cast<స్థిరరాశి అక్షరం*>(str.దత్తాంశం())),
        నిడివి_2(static_cast<int32_t>(str.కొలత_2())) {}

  /**
   * Constructs from a const char * pointer and a specified length.
   * @param offset a const char * pointer (need not be terminated)
   * @param len the length of the string; must be non-negative
   * @stable ICU 4.2
   */
  StringPiece(స్థిరరాశి అక్షరం* స్థానభ్రంశం U_LIFETIME_BOUND, int32_t len) : ptr_(స్థానభ్రంశం), నిడివి_2(len) { }
#ఒకవేళ defined(__cpp_char8_t) || defined(U_లోపల_DOXYGEN)
  /**
   * Constructs from a const char8_t * pointer and a specified length.
   * @param str a const char8_t * pointer (need not be terminated)
   * @param len the length of the string; must be non-negative
   * @stable ICU 67
   */
  StringPiece(స్థిరరాశి char8_t* str U_LIFETIME_BOUND, int32_t len) :
      StringPiece(reinterpret_cast<స్థిరరాశి అక్షరం*>(str), len) {}
#అయితే_ముగింపు

  /**
   * Substring of another StringPiece.
   * @param x the other StringPiece
   * @param pos start position in x; must be non-negative and <= x.length().
   * @stable ICU 4.2
   */
  StringPiece(స్థిరరాశి StringPiece& x U_LIFETIME_BOUND, int32_t pos);
  /**
   * Substring of another StringPiece.
   * @param x the other StringPiece
   * @param pos start position in x; must be non-negative and <= x.length().
   * @param len length of the substring;
   *            must be non-negative and will be pinned to at most x.length() - pos.
   * @stable ICU 4.2
   */
  StringPiece(స్థిరరాశి StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len);

#నిర్వచితం_కాకపోతే U_HIDE_అంతర్గత_API
  /**
   * Converts to a std::string_view().
   * @internal
   */
  అంతఃవరుస క్రియాకారకం std::string_view() స్థిరరాశి {
    తిరిగి {దత్తాంశం(), static_cast<std::string_view::కొలత_type>(కొలత_2())};
  }
#అయితే_ముగింపు  // U_HIDE_INTERNAL_API

  /**
   * Returns the string pointer. May be nullptr if it is empty.
   *
   * data() may return a pointer to a buffer with embedded NULs, and the
   * returned buffer may or may not be null terminated.  Therefore it is
   * typically a mistake to pass data() to a routine that expects a NUL
   * terminated string.
   * @return the string pointer
   * @stable ICU 4.2
   */
  స్థిరరాశి అక్షరం* దత్తాంశం() స్థిరరాశి { తిరిగి ptr_; }
  /**
   * Returns the string length. Same as length().
   * @return the string length
   * @stable ICU 4.2
   */
  int32_t కొలత_2() స్థిరరాశి { తిరిగి నిడివి_2; }
  /**
   * Returns the string length. Same as size().
   * @return the string length
   * @stable ICU 4.2
   */
  int32_t నిడివి() స్థిరరాశి { తిరిగి నిడివి_2; }
  /**
   * Returns whether the string is empty.
   * @return true if the string is empty
   * @stable ICU 4.2
   */
  UBool ఖాళీలు() స్థిరరాశి { తిరిగి నిడివి_2 == 0; }

  /**
   * Sets to an empty string.
   * @stable ICU 4.2
   */
  శూన్య clear() { ptr_ = nullptr; నిడివి_2 = 0; }

  /**
   * Reset the stringpiece to refer to new data.
   * @param xdata pointer the new string data.  Need not be nul terminated.
   * @param len the length of the new data
   * @stable ICU 4.8
   */
  శూన్య సమితి(స్థిరరాశి అక్షరం* xdata, int32_t len) { ptr_ = xdata; నిడివి_2 = len; }

  /**
   * Reset the stringpiece to refer to new data.
   * @param str a pointer to a NUL-terminated string. 
   * @stable ICU 4.8
   */
  శూన్య సమితి(స్థిరరాశి అక్షరం* str);

#ఒకవేళ defined(__cpp_char8_t) || defined(U_లోపల_DOXYGEN)
  /**
   * Resets the stringpiece to refer to new data.
   * @param xdata pointer the new string data. Need not be NUL-terminated.
   * @param len the length of the new data
   * @stable ICU 67
   */
  అంతఃవరుస శూన్య సమితి(స్థిరరాశి char8_t* xdata, int32_t len) {
      సమితి(reinterpret_cast<స్థిరరాశి అక్షరం*>(xdata), len);
  }

  /**
   * Resets the stringpiece to refer to new data.
   * @param str a pointer to a NUL-terminated string.
   * @stable ICU 67
   */
  అంతఃవరుస శూన్య సమితి(స్థిరరాశి char8_t* str) {
      సమితి(reinterpret_cast<స్థిరరాశి అక్షరం*>(str));
  }
#అయితే_ముగింపు

  /**
   * Removes the first n string units.
   * @param n prefix length, must be non-negative and <=length()
   * @stable ICU 4.2
   */
  శూన్య remove_ఉపసర్గ(int32_t n) {
    ఒకవేళ (n >= 0) {
      ఒకవేళ (n > నిడివి_2) {
        n = నిడివి_2;
      }
      ptr_ += n;
      నిడివి_2 -= n;
    }
  }

  /**
   * Removes the last n string units.
   * @param n suffix length, must be non-negative and <=length()
   * @stable ICU 4.2
   */
  శూన్య remove_suffix(int32_t n) {
    ఒకవేళ (n >= 0) {
      ఒకవేళ (n <= నిడివి_2) {
        నిడివి_2 -= n;
      } లేకపోతే {
        నిడివి_2 = 0;
      }
    }
  }

  /**
   * Searches the StringPiece for the given search string (needle);
   * @param needle The string for which to search.
   * @param offset Where to start searching within this string (haystack).
   * @return The offset of needle in haystack, or -1 if not found.
   * @stable ICU 67
   */
  int32_t కనుగొను(StringPiece needle, int32_t స్థానభ్రంశం);

  /**
   * Compares this StringPiece with the other StringPiece, with semantics
   * similar to std::string::compare().
   * @param other The string to compare to.
   * @return below zero if this < other; above zero if this > other; 0 if this == other.
   * @stable ICU 67
   */
  int32_t compare(StringPiece other);

  /**
   * Maximum integer, used as a default value for substring methods.
   * @stable ICU 4.2
   */
  స్థిర స్థిరరాశి int32_t npos; // = 0x7fffffff;

  /**
   * Returns a substring of this StringPiece.
   * @param pos start position; must be non-negative and <= length().
   * @param len length of the substring;
   *            must be non-negative and will be pinned to at most length() - pos.
   * @return the substring StringPiece
   * @stable ICU 4.2
   */
  StringPiece substr(int32_t pos, int32_t len = npos) స్థిరరాశి {
    తిరిగి StringPiece(*this, pos, len);
  }
};

/**
 * Global operator == for StringPiece
 * @param x The first StringPiece to compare.
 * @param y The second StringPiece to compare.
 * @return true if the string data is equal
 * @stable ICU 4.8
 */
U_COMMON_API UBool U_EXPORT2 
క్రియాకారకం==(స్థిరరాశి StringPiece& x, స్థిరరాశి StringPiece& y);

/**
 * Global operator != for StringPiece
 * @param x The first StringPiece to compare.
 * @param y The second StringPiece to compare.
 * @return true if the string data is not equal
 * @stable ICU 4.8
 */
అంతఃవరుస bool క్రియాకారకం!=(స్థిరరాశి StringPiece& x, స్థిరరాశి StringPiece& y) {
  తిరిగి !(x == y);
}

U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు  // __STRINGPIECE_H__
