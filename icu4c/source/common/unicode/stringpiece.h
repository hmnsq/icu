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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __STRINGPIECE_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __STRINGPIECE_H__

/**
 * \file 
 * \brief C++ API: StringPiece: Read-only byte string wrapper class.
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ <cstddef>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <type_traits>

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/std_string.h"

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
class U_COMMON_API StringPiece : ಸಾರ್ವಜನಿಕ UMemory {
 ಖಾಸಗಿ:
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*   ptr_;
  int32_t       ಉದ್ದಳತೆ_2;

 ಸಾರ್ವಜನಿಕ:
  /**
   * Default constructor, creates an empty StringPiece.
   * @stable ICU 4.2
   */
  StringPiece() : ptr_(nullptr), ಉದ್ದಳತೆ_2(0) { }

  /**
   * Constructs from a NUL-terminated const char * pointer.
   * @param str a NUL-terminated const char * pointer
   * @stable ICU 4.2
   */
  StringPiece(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* str U_LIFETIME_BOUND);
#ಆದರೆ defined(__cpp_char8_t) || defined(U_ಒಳ_DOXYGEN)
  /**
   * Constructs from a NUL-terminated const char8_t * pointer.
   * @param str a NUL-terminated const char8_t * pointer
   * @stable ICU 67
   */
  StringPiece(ಸ್ಥಿರಾಂಕ char8_t* str U_LIFETIME_BOUND) : StringPiece(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(str)) {}
#ಆದರೆ_ಅಂತ್ಯ
  /**
   * Constructs an empty StringPiece.
   * Needed for type disambiguation from multiple other overloads.
   * @param p nullptr
   * @stable ICU 67
   */
  StringPiece(std::nullptr_t p) : ptr_(p), ಉದ್ದಳತೆ_2(0) {}

  /**
   * Constructs from a std::string.
   * @stable ICU 4.2
   */
  StringPiece(ಸ್ಥಿರಾಂಕ std::string& str U_LIFETIME_BOUND)
    : ptr_(str.ದತ್ತಾಂಶ()), ಉದ್ದಳತೆ_2(static_cast<int32_t>(str.ಗಾತ್ರ_3())) { }
#ಆದರೆ defined(__cpp_lib_char8_t) || defined(U_ಒಳ_DOXYGEN)
  /**
   * Constructs from a std::u8string.
   * @stable ICU 67
   */
  StringPiece(ಸ್ಥಿರಾಂಕ std::u8string& str U_LIFETIME_BOUND)
    : ptr_(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(str.ದತ್ತಾಂಶ())),
      ಉದ್ದಳತೆ_2(static_cast<int32_t>(str.ಗಾತ್ರ_3())) { }
#ಆದರೆ_ಅಂತ್ಯ

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
  ಮಾದರಿ <typename T,
            typename = std::enable_if_t<
                (std::is_same_v<decltype(T().ದತ್ತಾಂಶ()), ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>
#ಆದರೆ defined(__cpp_char8_t)
                    || std::is_same_v<decltype(T().ದತ್ತಾಂಶ()), ಸ್ಥಿರಾಂಕ char8_t*>
#ಆದರೆ_ಅಂತ್ಯ
                ) &&
                std::is_same_v<decltype(T().ಗಾತ್ರ_3()), size_t>>>
  StringPiece(T str U_LIFETIME_BOUND)
      : ptr_(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(str.ದತ್ತಾಂಶ())),
        ಉದ್ದಳತೆ_2(static_cast<int32_t>(str.ಗಾತ್ರ_3())) {}

  /**
   * Constructs from a const char * pointer and a specified length.
   * @param offset a const char * pointer (need not be terminated)
   * @param len the length of the string; must be non-negative
   * @stable ICU 4.2
   */
  StringPiece(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಾನಾಂತರ U_LIFETIME_BOUND, int32_t len) : ptr_(ಸ್ಥಾನಾಂತರ), ಉದ್ದಳತೆ_2(len) { }
#ಆದರೆ defined(__cpp_char8_t) || defined(U_ಒಳ_DOXYGEN)
  /**
   * Constructs from a const char8_t * pointer and a specified length.
   * @param str a const char8_t * pointer (need not be terminated)
   * @param len the length of the string; must be non-negative
   * @stable ICU 67
   */
  StringPiece(ಸ್ಥಿರಾಂಕ char8_t* str U_LIFETIME_BOUND, int32_t len) :
      StringPiece(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(str), len) {}
#ಆದರೆ_ಅಂತ್ಯ

  /**
   * Substring of another StringPiece.
   * @param x the other StringPiece
   * @param pos start position in x; must be non-negative and <= x.length().
   * @stable ICU 4.2
   */
  StringPiece(ಸ್ಥಿರಾಂಕ StringPiece& x U_LIFETIME_BOUND, int32_t pos);
  /**
   * Substring of another StringPiece.
   * @param x the other StringPiece
   * @param pos start position in x; must be non-negative and <= x.length().
   * @param len length of the substring;
   *            must be non-negative and will be pinned to at most x.length() - pos.
   * @stable ICU 4.2
   */
  StringPiece(ಸ್ಥಿರಾಂಕ StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_ಆಂತರಿಕ_API
  /**
   * Converts to a std::string_view().
   * @internal
   */
  ಸಾಲೊಳಗೆ ಕ್ರಿಯಾಕಾರಕ std::string_view() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ {ದತ್ತಾಂಶ(), static_cast<std::string_view::ಗಾತ್ರ_type>(ಗಾತ್ರ_3())};
  }
#ಆದರೆ_ಅಂತ್ಯ  // U_HIDE_INTERNAL_API

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
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ದತ್ತಾಂಶ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ptr_; }
  /**
   * Returns the string length. Same as length().
   * @return the string length
   * @stable ICU 4.2
   */
  int32_t ಗಾತ್ರ_3() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ_2; }
  /**
   * Returns the string length. Same as size().
   * @return the string length
   * @stable ICU 4.2
   */
  int32_t ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ_2; }
  /**
   * Returns whether the string is empty.
   * @return true if the string is empty
   * @stable ICU 4.2
   */
  UBool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ_2 == 0; }

  /**
   * Sets to an empty string.
   * @stable ICU 4.2
   */
  ಶೂನ್ಯ clear() { ptr_ = nullptr; ಉದ್ದಳತೆ_2 = 0; }

  /**
   * Reset the stringpiece to refer to new data.
   * @param xdata pointer the new string data.  Need not be nul terminated.
   * @param len the length of the new data
   * @stable ICU 4.8
   */
  ಶೂನ್ಯ ಗಣ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* xdata, int32_t len) { ptr_ = xdata; ಉದ್ದಳತೆ_2 = len; }

  /**
   * Reset the stringpiece to refer to new data.
   * @param str a pointer to a NUL-terminated string. 
   * @stable ICU 4.8
   */
  ಶೂನ್ಯ ಗಣ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* str);

#ಆದರೆ defined(__cpp_char8_t) || defined(U_ಒಳ_DOXYGEN)
  /**
   * Resets the stringpiece to refer to new data.
   * @param xdata pointer the new string data. Need not be NUL-terminated.
   * @param len the length of the new data
   * @stable ICU 67
   */
  ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಗಣ(ಸ್ಥಿರಾಂಕ char8_t* xdata, int32_t len) {
      ಗಣ(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(xdata), len);
  }

  /**
   * Resets the stringpiece to refer to new data.
   * @param str a pointer to a NUL-terminated string.
   * @stable ICU 67
   */
  ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಗಣ(ಸ್ಥಿರಾಂಕ char8_t* str) {
      ಗಣ(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(str));
  }
#ಆದರೆ_ಅಂತ್ಯ

  /**
   * Removes the first n string units.
   * @param n prefix length, must be non-negative and <=length()
   * @stable ICU 4.2
   */
  ಶೂನ್ಯ remove_ಪೂರ್ವಪ್ರತ್ಯಯ(int32_t n) {
    ಆದರೆ (n >= 0) {
      ಆದರೆ (n > ಉದ್ದಳತೆ_2) {
        n = ಉದ್ದಳತೆ_2;
      }
      ptr_ += n;
      ಉದ್ದಳತೆ_2 -= n;
    }
  }

  /**
   * Removes the last n string units.
   * @param n suffix length, must be non-negative and <=length()
   * @stable ICU 4.2
   */
  ಶೂನ್ಯ remove_suffix(int32_t n) {
    ಆದರೆ (n >= 0) {
      ಆದರೆ (n <= ಉದ್ದಳತೆ_2) {
        ಉದ್ದಳತೆ_2 -= n;
      } ಇಲ್ಲದಿದ್ದರೆ {
        ಉದ್ದಳತೆ_2 = 0;
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
  int32_t ಕಂಡುಹಿಡಿ(StringPiece needle, int32_t ಸ್ಥಾನಾಂತರ);

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
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t npos; // = 0x7fffffff;

  /**
   * Returns a substring of this StringPiece.
   * @param pos start position; must be non-negative and <= length().
   * @param len length of the substring;
   *            must be non-negative and will be pinned to at most length() - pos.
   * @return the substring StringPiece
   * @stable ICU 4.2
   */
  StringPiece substr(int32_t pos, int32_t len = npos) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ StringPiece(*this, pos, len);
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
ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ StringPiece& x, ಸ್ಥಿರಾಂಕ StringPiece& y);

/**
 * Global operator != for StringPiece
 * @param x The first StringPiece to compare.
 * @param y The second StringPiece to compare.
 * @return true if the string data is not equal
 * @stable ICU 4.8
 */
ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ StringPiece& x, ಸ್ಥಿರಾಂಕ StringPiece& y) {
  ಹಿಂತಿರುಗಿ !(x == y);
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* U_SHOW_CPLUSPLUS_API */

#ಆದರೆ_ಅಂತ್ಯ  // __STRINGPIECE_H__
