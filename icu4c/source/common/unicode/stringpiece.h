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

#ifndef __STRINGPIECE_H__
#define __STRINGPIECE_H__

/**
 * \file 
 * \brief C++ API: StringPiece: Read-only byte string wrapper class.
 */

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_SHOW_CPLUSPLUS_API

#অন্তর্ভাব <cstddef>
#অন্তর্ভাব <string_view>
#অন্তর্ভাব <type_traits>

#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/std_string.h"

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
class U_COMMON_API StringPiece : সর্বজনীন UMemory {
 ব্যক্তিগত:
  ধ্রুবক অক্ষর*   ptr_;
  int32_t       দৈর্ঘ্য_2;

 সর্বজনীন:
  /**
   * Default constructor, creates an empty StringPiece.
   * @stable ICU 4.2
   */
  StringPiece() : ptr_(nullptr), দৈর্ঘ্য_2(0) { }

  /**
   * Constructs from a NUL-terminated const char * pointer.
   * @param str a NUL-terminated const char * pointer
   * @stable ICU 4.2
   */
  StringPiece(ধ্রুবক অক্ষর* str U_LIFETIME_BOUND);
#যদি defined(__cpp_char8_t) || defined(U_IN_DOXYGEN)
  /**
   * Constructs from a NUL-terminated const char8_t * pointer.
   * @param str a NUL-terminated const char8_t * pointer
   * @stable ICU 67
   */
  StringPiece(ধ্রুবক char8_t* str U_LIFETIME_BOUND) : StringPiece(reinterpret_cast<ধ্রুবক অক্ষর*>(str)) {}
#endif
  /**
   * Constructs an empty StringPiece.
   * Needed for type disambiguation from multiple other overloads.
   * @param p nullptr
   * @stable ICU 67
   */
  StringPiece(std::nullptr_t p) : ptr_(p), দৈর্ঘ্য_2(0) {}

  /**
   * Constructs from a std::string.
   * @stable ICU 4.2
   */
  StringPiece(ধ্রুবক std::string& str U_LIFETIME_BOUND)
    : ptr_(str.তথ্য()), দৈর্ঘ্য_2(static_cast<int32_t>(str.আকার_3())) { }
#যদি defined(__cpp_lib_char8_t) || defined(U_IN_DOXYGEN)
  /**
   * Constructs from a std::u8string.
   * @stable ICU 67
   */
  StringPiece(ধ্রুবক std::u8string& str U_LIFETIME_BOUND)
    : ptr_(reinterpret_cast<ধ্রুবক অক্ষর*>(str.তথ্য())),
      দৈর্ঘ্য_2(static_cast<int32_t>(str.আকার_3())) { }
#endif

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
  template <typename T,
            typename = std::enable_if_t<
                (std::is_same_v<decltype(T().তথ্য()), ধ্রুবক অক্ষর*>
#যদি defined(__cpp_char8_t)
                    || std::is_same_v<decltype(T().তথ্য()), ধ্রুবক char8_t*>
#endif
                ) &&
                std::is_same_v<decltype(T().আকার_3()), size_t>>>
  StringPiece(T str U_LIFETIME_BOUND)
      : ptr_(reinterpret_cast<ধ্রুবক অক্ষর*>(str.তথ্য())),
        দৈর্ঘ্য_2(static_cast<int32_t>(str.আকার_3())) {}

  /**
   * Constructs from a const char * pointer and a specified length.
   * @param offset a const char * pointer (need not be terminated)
   * @param len the length of the string; must be non-negative
   * @stable ICU 4.2
   */
  StringPiece(ধ্রুবক অক্ষর* offset U_LIFETIME_BOUND, int32_t len) : ptr_(offset), দৈর্ঘ্য_2(len) { }
#যদি defined(__cpp_char8_t) || defined(U_IN_DOXYGEN)
  /**
   * Constructs from a const char8_t * pointer and a specified length.
   * @param str a const char8_t * pointer (need not be terminated)
   * @param len the length of the string; must be non-negative
   * @stable ICU 67
   */
  StringPiece(ধ্রুবক char8_t* str U_LIFETIME_BOUND, int32_t len) :
      StringPiece(reinterpret_cast<ধ্রুবক অক্ষর*>(str), len) {}
#endif

  /**
   * Substring of another StringPiece.
   * @param x the other StringPiece
   * @param pos start position in x; must be non-negative and <= x.length().
   * @stable ICU 4.2
   */
  StringPiece(ধ্রুবক StringPiece& x U_LIFETIME_BOUND, int32_t pos);
  /**
   * Substring of another StringPiece.
   * @param x the other StringPiece
   * @param pos start position in x; must be non-negative and <= x.length().
   * @param len length of the substring;
   *            must be non-negative and will be pinned to at most x.length() - pos.
   * @stable ICU 4.2
   */
  StringPiece(ধ্রুবক StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len);

#ifndef U_HIDE_INTERNAL_API
  /**
   * Converts to a std::string_view().
   * @internal
   */
  অন্তঃরেখ operator std::string_view() ধ্রুবক {
    ফেরত {তথ্য(), static_cast<std::string_view::আকার_type>(আকার_3())};
  }
#endif  // U_HIDE_INTERNAL_API

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
  ধ্রুবক অক্ষর* তথ্য() ধ্রুবক { ফেরত ptr_; }
  /**
   * Returns the string length. Same as length().
   * @return the string length
   * @stable ICU 4.2
   */
  int32_t আকার_3() ধ্রুবক { ফেরত দৈর্ঘ্য_2; }
  /**
   * Returns the string length. Same as size().
   * @return the string length
   * @stable ICU 4.2
   */
  int32_t দৈর্ঘ্য() ধ্রুবক { ফেরত দৈর্ঘ্য_2; }
  /**
   * Returns whether the string is empty.
   * @return true if the string is empty
   * @stable ICU 4.2
   */
  UBool empty() ধ্রুবক { ফেরত দৈর্ঘ্য_2 == 0; }

  /**
   * Sets to an empty string.
   * @stable ICU 4.2
   */
  শূন্য clear() { ptr_ = nullptr; দৈর্ঘ্য_2 = 0; }

  /**
   * Reset the stringpiece to refer to new data.
   * @param xdata pointer the new string data.  Need not be nul terminated.
   * @param len the length of the new data
   * @stable ICU 4.8
   */
  শূন্য set(ধ্রুবক অক্ষর* xdata, int32_t len) { ptr_ = xdata; দৈর্ঘ্য_2 = len; }

  /**
   * Reset the stringpiece to refer to new data.
   * @param str a pointer to a NUL-terminated string. 
   * @stable ICU 4.8
   */
  শূন্য set(ধ্রুবক অক্ষর* str);

#যদি defined(__cpp_char8_t) || defined(U_IN_DOXYGEN)
  /**
   * Resets the stringpiece to refer to new data.
   * @param xdata pointer the new string data. Need not be NUL-terminated.
   * @param len the length of the new data
   * @stable ICU 67
   */
  অন্তঃরেখ শূন্য set(ধ্রুবক char8_t* xdata, int32_t len) {
      set(reinterpret_cast<ধ্রুবক অক্ষর*>(xdata), len);
  }

  /**
   * Resets the stringpiece to refer to new data.
   * @param str a pointer to a NUL-terminated string.
   * @stable ICU 67
   */
  অন্তঃরেখ শূন্য set(ধ্রুবক char8_t* str) {
      set(reinterpret_cast<ধ্রুবক অক্ষর*>(str));
  }
#endif

  /**
   * Removes the first n string units.
   * @param n prefix length, must be non-negative and <=length()
   * @stable ICU 4.2
   */
  শূন্য remove_prefix(int32_t n) {
    যদি (n >= 0) {
      যদি (n > দৈর্ঘ্য_2) {
        n = দৈর্ঘ্য_2;
      }
      ptr_ += n;
      দৈর্ঘ্য_2 -= n;
    }
  }

  /**
   * Removes the last n string units.
   * @param n suffix length, must be non-negative and <=length()
   * @stable ICU 4.2
   */
  শূন্য remove_suffix(int32_t n) {
    যদি (n >= 0) {
      যদি (n <= দৈর্ঘ্য_2) {
        দৈর্ঘ্য_2 -= n;
      } অন্যথা {
        দৈর্ঘ্য_2 = 0;
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
  int32_t find(StringPiece needle, int32_t offset);

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
  স্থির ধ্রুবক int32_t npos; // = 0x7fffffff;

  /**
   * Returns a substring of this StringPiece.
   * @param pos start position; must be non-negative and <= length().
   * @param len length of the substring;
   *            must be non-negative and will be pinned to at most length() - pos.
   * @return the substring StringPiece
   * @stable ICU 4.2
   */
  StringPiece substr(int32_t pos, int32_t len = npos) ধ্রুবক {
    ফেরত StringPiece(*this, pos, len);
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
operator==(ধ্রুবক StringPiece& x, ধ্রুবক StringPiece& y);

/**
 * Global operator != for StringPiece
 * @param x The first StringPiece to compare.
 * @param y The second StringPiece to compare.
 * @return true if the string data is not equal
 * @stable ICU 4.8
 */
অন্তঃরেখ bool operator!=(ধ্রুবক StringPiece& x, ধ্রুবক StringPiece& y) {
  ফেরত !(x == y);
}

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __STRINGPIECE_H__
