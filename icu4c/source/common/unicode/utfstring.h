// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfstring.h
// created: 2025jul18 Markus W. Scherer

#اگر_غیر_تعریف_شدہ __UTFSTRING_H__
#تعریف_کرو __UTFSTRING_H__

#شامل "unicode/utypes.h"

#اگر U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_سرخی_API || !defined(UTYPES_H)

#شامل "unicode/utf16.h"

/**
 * \file
 * \brief C++ header-only API: C++ string helper functions.
 */

#اگر_غیر_تعریف_شدہ U_HIDE_DRAFT_API

namespace U_سرخی_ONLY_NAMESPACE {
namespace utfstring {

// Write code points to strings -------------------------------------------- ***

#اگر_غیر_تعریف_شدہ U_اندر_DOXYGEN
namespace prv {

// This function, and the public wrappers,
// want to be U_FORCE_INLINE but the gcc-debug-build-and-test CI check failed with
// error: ‘always_inline’ function might not be inlinable [-Werror=attributes]
سانچہ <typename StringClass, bool validate>
اندرونی StringClass& append_رمز_نقطہ(StringClass& s U_LIFETIME_BOUND, uint32_t c) {
    using Unit = typename StringClass::قدر_type;
    اگر constexpr (حجم(Unit) == 1) {
        // UTF-8: Similar to U8_APPEND().
        اگر (c <= 0x7f) {
            s.ڈالو_عقب(static_cast<Unit>(c));
        } ورنہ {
            Unit buf[4];
            uint8_t len;
            اگر (c <= 0x7ff) {
                len = 2;
                buf[2] = (c >> 6) | 0xc0;
            } ورنہ {
                اگر (validate ?
                        c < 0xd800 ||
                            (c < 0xe000 || c > 0x10ffff ? (c = 0xfffd, true) : c <= 0xffff) :
                        c <= 0xffff) {
                    len = 3;
                    buf[1] = (c >> 12) | 0xe0;
                } ورنہ {
                    len = 4;
                    buf[0] = (c >> 18) | 0xf0;
                    buf[1] = ((c >> 12) & 0x3f) | 0x80;
                }
                buf[2] = ((c >> 6) & 0x3f) | 0x80;
            }
            buf[3] = (c & 0x3f) | 0x80;
            s.append(buf + 4 - len, len);
        }
    } ورنہ اگر constexpr (حجم(Unit) == 2) {
        // UTF-16: Similar to U16_APPEND().
        اگر (validate ?
                c < 0xd800 || (c < 0xe000 || c > 0x10ffff ? (c = 0xfffd, true) : c <= 0xffff) :
                c <= 0xffff) {
            s.ڈالو_عقب(static_cast<Unit>(c));
        } ورنہ {
            Unit buf[2] = { U16_LEAD(c), U16_TRAIL(c) };
            s.append(buf, 2);
        }
    } ورنہ {
        // UTF-32
        s.ڈالو_عقب(!validate || U_IS_SCALAR_قدر(c) ? c : 0xfffd);
    }
    واپس s;
}

}  // namespace prv
#اگر_ختم  // U_IN_DOXYGEN

#اگر_غیر_تعریف_شدہ U_HIDE_DRAFT_API
/**
 * Appends the code point to the string.
 * Appends the U+FFFD replacement character instead if c is not a scalar value.
 * See https://www.unicode.org/glossary/#unicode_scalar_value
 *
 * @tparam StringClass A version of std::basic_string (or a compatible type)
 * @param s The string to append to
 * @param c The code point to append
 * @return s
 * @draft ICU 78
 * @see U_IS_SCALAR_VALUE
 */
سانچہ <typename StringClass>
اندرونی StringClass& appendOrFFFD(StringClass& s U_LIFETIME_BOUND, UChar32 c) {
    واپس prv::append_رمز_نقطہ<StringClass, true>(s, c);
}

/**
 * Appends the code point to the string.
 * The code point must be a scalar value; otherwise the behavior is undefined.
 * See https://www.unicode.org/glossary/#unicode_scalar_value
 *
 * @tparam StringClass A version of std::basic_string (or a compatible type)
 * @param s The string to append to
 * @param c The code point to append (must be a scalar value)
 * @return s
 * @draft ICU 78
 * @see U_IS_SCALAR_VALUE
 */
سانچہ <typename StringClass>
اندرونی StringClass& appendUnsafe(StringClass& s U_LIFETIME_BOUND, UChar32 c) {
    واپس prv::append_رمز_نقطہ<StringClass, false>(s, c);
}

/**
 * Returns the code point as a string of code units.
 * Returns the U+FFFD replacement character instead if c is not a scalar value.
 * See https://www.unicode.org/glossary/#unicode_scalar_value
 *
 * @tparam StringClass A version of std::basic_string (or a compatible type)
 * @param c The code point
 * @return the string of c's code units
 * @draft ICU 78
 * @see U_IS_SCALAR_VALUE
 */
سانچہ<typename StringClass>
اندرونی StringClass encodeOrFFFD(UChar32 c) {
    StringClass s;
    prv::append_رمز_نقطہ<StringClass, true>(s, c);
    واپس s;
}

/**
 * Returns the code point as a string of code units.
 * The code point must be a scalar value; otherwise the behavior is undefined.
 * See https://www.unicode.org/glossary/#unicode_scalar_value
 *
 * @tparam StringClass A version of std::basic_string (or a compatible type)
 * @param c The code point
 * @return the string of c's code units
 * @draft ICU 78
 * @see U_IS_SCALAR_VALUE
 */
سانچہ<typename StringClass>
اندرونی StringClass encodeUnsafe(UChar32 c) {
    StringClass s;
    prv::append_رمز_نقطہ<StringClass, false>(s, c);
    واپس s;
}
#اگر_ختم  // U_HIDE_DRAFT_API

}  // namespace utfstring
}  // namespace U_HEADER_ONLY_NAMESPACE

#اگر_ختم  // U_HIDE_DRAFT_API
#اگر_ختم  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#اگر_ختم  // __UTFSTRING_H__
