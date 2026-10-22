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

#अगर_अघोषित __STRINGPIECE_H__
#घोषणा __STRINGPIECE_H__

/**
 * \file 
 * \brief C++ API: StringPiece: Read-only byte string wrapper class.
 */

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

#समावेश <cstddef>
#समावेश <string_view>
#समावेश <type_traits>

#समावेश "unicode/uobject.h"
#समावेश "unicode/std_string.h"

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
class U_COMMON_API StringPiece : खुला UMemory {
 निजी:
  स्थिर अक्षर*   ptr_;
  int32_t       लंबाई_2;

 खुला:
  /**
   * Default constructor, creates an empty StringPiece.
   * @stable ICU 4.2
   */
  StringPiece() : ptr_(nullptr), लंबाई_2(0) { }

  /**
   * Constructs from a NUL-terminated const char * pointer.
   * @param str a NUL-terminated const char * pointer
   * @stable ICU 4.2
   */
  StringPiece(स्थिर अक्षर* str U_LIFETIME_BOUND);
#अगर defined(__cpp_char8_t) || defined(U_IN_DOXYGEN)
  /**
   * Constructs from a NUL-terminated const char8_t * pointer.
   * @param str a NUL-terminated const char8_t * pointer
   * @stable ICU 67
   */
  StringPiece(स्थिर char8_t* str U_LIFETIME_BOUND) : StringPiece(reinterpret_cast<स्थिर अक्षर*>(str)) {}
#पूर्ण_अगर
  /**
   * Constructs an empty StringPiece.
   * Needed for type disambiguation from multiple other overloads.
   * @param p nullptr
   * @stable ICU 67
   */
  StringPiece(std::nullptr_t p) : ptr_(p), लंबाई_2(0) {}

  /**
   * Constructs from a std::string.
   * @stable ICU 4.2
   */
  StringPiece(स्थिर std::string& str U_LIFETIME_BOUND)
    : ptr_(str.तथ्य()), लंबाई_2(static_cast<int32_t>(str.आकार_2())) { }
#अगर defined(__cpp_lib_char8_t) || defined(U_IN_DOXYGEN)
  /**
   * Constructs from a std::u8string.
   * @stable ICU 67
   */
  StringPiece(स्थिर std::u8string& str U_LIFETIME_BOUND)
    : ptr_(reinterpret_cast<स्थिर अक्षर*>(str.तथ्य())),
      लंबाई_2(static_cast<int32_t>(str.आकार_2())) { }
#पूर्ण_अगर

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
  ढाँचा <typename T,
            typename = std::enable_if_t<
                (std::is_same_v<decltype(T().तथ्य()), स्थिर अक्षर*>
#अगर defined(__cpp_char8_t)
                    || std::is_same_v<decltype(T().तथ्य()), स्थिर char8_t*>
#पूर्ण_अगर
                ) &&
                std::is_same_v<decltype(T().आकार_2()), माप_प्रकार>>>
  StringPiece(T str U_LIFETIME_BOUND)
      : ptr_(reinterpret_cast<स्थिर अक्षर*>(str.तथ्य())),
        लंबाई_2(static_cast<int32_t>(str.आकार_2())) {}

  /**
   * Constructs from a const char * pointer and a specified length.
   * @param offset a const char * pointer (need not be terminated)
   * @param len the length of the string; must be non-negative
   * @stable ICU 4.2
   */
  StringPiece(स्थिर अक्षर* offset U_LIFETIME_BOUND, int32_t len) : ptr_(offset), लंबाई_2(len) { }
#अगर defined(__cpp_char8_t) || defined(U_IN_DOXYGEN)
  /**
   * Constructs from a const char8_t * pointer and a specified length.
   * @param str a const char8_t * pointer (need not be terminated)
   * @param len the length of the string; must be non-negative
   * @stable ICU 67
   */
  StringPiece(स्थिर char8_t* str U_LIFETIME_BOUND, int32_t len) :
      StringPiece(reinterpret_cast<स्थिर अक्षर*>(str), len) {}
#पूर्ण_अगर

  /**
   * Substring of another StringPiece.
   * @param x the other StringPiece
   * @param pos start position in x; must be non-negative and <= x.length().
   * @stable ICU 4.2
   */
  StringPiece(स्थिर StringPiece& x U_LIFETIME_BOUND, int32_t pos);
  /**
   * Substring of another StringPiece.
   * @param x the other StringPiece
   * @param pos start position in x; must be non-negative and <= x.length().
   * @param len length of the substring;
   *            must be non-negative and will be pinned to at most x.length() - pos.
   * @stable ICU 4.2
   */
  StringPiece(स्थिर StringPiece& x U_LIFETIME_BOUND, int32_t pos, int32_t len);

#अगर_अघोषित U_HIDE_INTERNAL_API
  /**
   * Converts to a std::string_view().
   * @internal
   */
  अंतरभूत चालक std::string_view() स्थिर {
    वापस {तथ्य(), static_cast<std::string_view::आकार_type>(आकार_2())};
  }
#पूर्ण_अगर  // U_HIDE_INTERNAL_API

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
  स्थिर अक्षर* तथ्य() स्थिर { वापस ptr_; }
  /**
   * Returns the string length. Same as length().
   * @return the string length
   * @stable ICU 4.2
   */
  int32_t आकार_2() स्थिर { वापस लंबाई_2; }
  /**
   * Returns the string length. Same as size().
   * @return the string length
   * @stable ICU 4.2
   */
  int32_t लंबाई() स्थिर { वापस लंबाई_2; }
  /**
   * Returns whether the string is empty.
   * @return true if the string is empty
   * @stable ICU 4.2
   */
  UBool empty() स्थिर { वापस लंबाई_2 == 0; }

  /**
   * Sets to an empty string.
   * @stable ICU 4.2
   */
  व्योम clear() { ptr_ = nullptr; लंबाई_2 = 0; }

  /**
   * Reset the stringpiece to refer to new data.
   * @param xdata pointer the new string data.  Need not be nul terminated.
   * @param len the length of the new data
   * @stable ICU 4.8
   */
  व्योम set(स्थिर अक्षर* xdata, int32_t len) { ptr_ = xdata; लंबाई_2 = len; }

  /**
   * Reset the stringpiece to refer to new data.
   * @param str a pointer to a NUL-terminated string. 
   * @stable ICU 4.8
   */
  व्योम set(स्थिर अक्षर* str);

#अगर defined(__cpp_char8_t) || defined(U_IN_DOXYGEN)
  /**
   * Resets the stringpiece to refer to new data.
   * @param xdata pointer the new string data. Need not be NUL-terminated.
   * @param len the length of the new data
   * @stable ICU 67
   */
  अंतरभूत व्योम set(स्थिर char8_t* xdata, int32_t len) {
      set(reinterpret_cast<स्थिर अक्षर*>(xdata), len);
  }

  /**
   * Resets the stringpiece to refer to new data.
   * @param str a pointer to a NUL-terminated string.
   * @stable ICU 67
   */
  अंतरभूत व्योम set(स्थिर char8_t* str) {
      set(reinterpret_cast<स्थिर अक्षर*>(str));
  }
#पूर्ण_अगर

  /**
   * Removes the first n string units.
   * @param n prefix length, must be non-negative and <=length()
   * @stable ICU 4.2
   */
  व्योम remove_prefix(int32_t n) {
    अगर (n >= 0) {
      अगर (n > लंबाई_2) {
        n = लंबाई_2;
      }
      ptr_ += n;
      लंबाई_2 -= n;
    }
  }

  /**
   * Removes the last n string units.
   * @param n suffix length, must be non-negative and <=length()
   * @stable ICU 4.2
   */
  व्योम remove_suffix(int32_t n) {
    अगर (n >= 0) {
      अगर (n <= लंबाई_2) {
        लंबाई_2 -= n;
      } अन्यथा {
        लंबाई_2 = 0;
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
  अटल स्थिर int32_t npos; // = 0x7fffffff;

  /**
   * Returns a substring of this StringPiece.
   * @param pos start position; must be non-negative and <= length().
   * @param len length of the substring;
   *            must be non-negative and will be pinned to at most length() - pos.
   * @return the substring StringPiece
   * @stable ICU 4.2
   */
  StringPiece substr(int32_t pos, int32_t len = npos) स्थिर {
    वापस StringPiece(*this, pos, len);
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
चालक==(स्थिर StringPiece& x, स्थिर StringPiece& y);

/**
 * Global operator != for StringPiece
 * @param x The first StringPiece to compare.
 * @param y The second StringPiece to compare.
 * @return true if the string data is not equal
 * @stable ICU 4.8
 */
अंतरभूत bool चालक!=(स्थिर StringPiece& x, स्थिर StringPiece& y) {
  वापस !(x == y);
}

U_NAMESPACE_END

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */

#पूर्ण_अगर  // __STRINGPIECE_H__
