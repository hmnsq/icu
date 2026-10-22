// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfstring.h
// created: 2025jul18 Markus W. Scherer

#جے_بنا_تعریف __UTFSTRING_H__
#تعریف_کرو __UTFSTRING_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_سرخی_API || !defined(UTYPES_H)

#ضمن "unicode/utf16.h"

/**
 * \file
 * \brief C++ header-only API: C++ string helper functions.
 */

#جے_بنا_تعریف U_HIDE_DRAFT_API

namespace U_سرخی_ONLY_NAMESPACE {
namespace utfstring {

// Write code points to strings -------------------------------------------- ***

#جے_بنا_تعریف U_وچ_DOXYGEN
namespace prv {

// This function, and the public wrappers,
// want to be U_FORCE_INLINE but the gcc-debug-build-and-test CI check failed with
// error: ‘always_inline’ function might not be inlinable [-Werror=attributes]
سانچا <typename StringClass, bool validate>
مضمن StringClass& append_کوڈ_نقطہ(StringClass& s U_LIFETIME_BOUND, uint32_t c) {
    using Unit = typename StringClass::قدر_type;
    إذا constexpr (حجم(Unit) == 1) {
        // UTF-8: Similar to U8_APPEND().
        إذا (c <= 0x7f) {
            s.ڈالو_پٹھ(static_cast<Unit>(c));
        } وإلا {
            Unit buf[4];
            uint8_t len;
            إذا (c <= 0x7ff) {
                len = 2;
                buf[2] = (c >> 6) | 0xc0;
            } وإلا {
                إذا (validate ?
                        c < 0xd800 ||
                            (c < 0xe000 || c > 0x10ffff ? (c = 0xfffd, true) : c <= 0xffff) :
                        c <= 0xffff) {
                    len = 3;
                    buf[1] = (c >> 12) | 0xe0;
                } وإلا {
                    len = 4;
                    buf[0] = (c >> 18) | 0xf0;
                    buf[1] = ((c >> 12) & 0x3f) | 0x80;
                }
                buf[2] = ((c >> 6) & 0x3f) | 0x80;
            }
            buf[3] = (c & 0x3f) | 0x80;
            s.append(buf + 4 - len, len);
        }
    } وإلا إذا constexpr (حجم(Unit) == 2) {
        // UTF-16: Similar to U16_APPEND().
        إذا (validate ?
                c < 0xd800 || (c < 0xe000 || c > 0x10ffff ? (c = 0xfffd, true) : c <= 0xffff) :
                c <= 0xffff) {
            s.ڈالو_پٹھ(static_cast<Unit>(c));
        } وإلا {
            Unit buf[2] = { U16_LEAD(c), U16_TRAIL(c) };
            s.append(buf, 2);
        }
    } وإلا {
        // UTF-32
        s.ڈالو_پٹھ(!validate || U_IS_SCALAR_قدر(c) ? c : 0xfffd);
    }
    أرجع s;
}

}  // namespace prv
#جے_انت  // U_IN_DOXYGEN

#جے_بنا_تعریف U_HIDE_DRAFT_API
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
سانچا <typename StringClass>
مضمن StringClass& appendOrFFFD(StringClass& s U_LIFETIME_BOUND, UChar32 c) {
    أرجع prv::append_کوڈ_نقطہ<StringClass, true>(s, c);
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
سانچا <typename StringClass>
مضمن StringClass& appendUnsafe(StringClass& s U_LIFETIME_BOUND, UChar32 c) {
    أرجع prv::append_کوڈ_نقطہ<StringClass, false>(s, c);
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
سانچا<typename StringClass>
مضمن StringClass encodeOrFFFD(UChar32 c) {
    StringClass s;
    prv::append_کوڈ_نقطہ<StringClass, true>(s, c);
    أرجع s;
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
سانچا<typename StringClass>
مضمن StringClass encodeUnsafe(UChar32 c) {
    StringClass s;
    prv::append_کوڈ_نقطہ<StringClass, false>(s, c);
    أرجع s;
}
#جے_انت  // U_HIDE_DRAFT_API

}  // namespace utfstring
}  // namespace U_HEADER_ONLY_NAMESPACE

#جے_انت  // U_HIDE_DRAFT_API
#جے_انت  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#جے_انت  // __UTFSTRING_H__
