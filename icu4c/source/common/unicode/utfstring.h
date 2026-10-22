// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfstring.h
// created: 2025jul18 Markus W. Scherer

#ifndef __UTFSTRING_H__
#define __UTFSTRING_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API || !defined(UTYPES_H)

#अन्तर्भाव "unicode/utf16.h"

/**
 * \file
 * \brief C++ header-only API: C++ string helper functions.
 */

#ifndef U_HIDE_DRAFT_API

namespace U_HEADER_ONLY_NAMESPACE {
namespace utfstring {

// Write code points to strings -------------------------------------------- ***

#ifndef U_IN_DOXYGEN
namespace prv {

// This function, and the public wrappers,
// want to be U_FORCE_INLINE but the gcc-debug-build-and-test CI check failed with
// error: ‘always_inline’ function might not be inlinable [-Werror=attributes]
ढाँचा <typename StringClass, bool validate>
अन्तःस्थ StringClass& appendCodePoint(StringClass& s U_LIFETIME_BOUND, uint32_t c) {
    using Unit = typename StringClass::मान_type;
    यदि constexpr (परिमाण(Unit) == 1) {
        // UTF-8: Similar to U8_APPEND().
        यदि (c <= 0x7f) {
            s.थप_back(static_cast<Unit>(c));
        } अन्यथा {
            Unit buf[4];
            uint8_t len;
            यदि (c <= 0x7ff) {
                len = 2;
                buf[2] = (c >> 6) | 0xc0;
            } अन्यथा {
                यदि (validate ?
                        c < 0xd800 ||
                            (c < 0xe000 || c > 0x10ffff ? (c = 0xfffd, true) : c <= 0xffff) :
                        c <= 0xffff) {
                    len = 3;
                    buf[1] = (c >> 12) | 0xe0;
                } अन्यथा {
                    len = 4;
                    buf[0] = (c >> 18) | 0xf0;
                    buf[1] = ((c >> 12) & 0x3f) | 0x80;
                }
                buf[2] = ((c >> 6) & 0x3f) | 0x80;
            }
            buf[3] = (c & 0x3f) | 0x80;
            s.append(buf + 4 - len, len);
        }
    } अन्यथा यदि constexpr (परिमाण(Unit) == 2) {
        // UTF-16: Similar to U16_APPEND().
        यदि (validate ?
                c < 0xd800 || (c < 0xe000 || c > 0x10ffff ? (c = 0xfffd, true) : c <= 0xffff) :
                c <= 0xffff) {
            s.थप_back(static_cast<Unit>(c));
        } अन्यथा {
            Unit buf[2] = { U16_LEAD(c), U16_TRAIL(c) };
            s.append(buf, 2);
        }
    } अन्यथा {
        // UTF-32
        s.थप_back(!validate || U_IS_SCALAR_मान(c) ? c : 0xfffd);
    }
    फर्काऊ s;
}

}  // namespace prv
#endif  // U_IN_DOXYGEN

#ifndef U_HIDE_DRAFT_API
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
ढाँचा <typename StringClass>
अन्तःस्थ StringClass& appendOrFFFD(StringClass& s U_LIFETIME_BOUND, UChar32 c) {
    फर्काऊ prv::appendCodePoint<StringClass, true>(s, c);
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
ढाँचा <typename StringClass>
अन्तःस्थ StringClass& appendUnsafe(StringClass& s U_LIFETIME_BOUND, UChar32 c) {
    फर्काऊ prv::appendCodePoint<StringClass, false>(s, c);
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
ढाँचा<typename StringClass>
अन्तःस्थ StringClass encodeOrFFFD(UChar32 c) {
    StringClass s;
    prv::appendCodePoint<StringClass, true>(s, c);
    फर्काऊ s;
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
ढाँचा<typename StringClass>
अन्तःस्थ StringClass encodeUnsafe(UChar32 c) {
    StringClass s;
    prv::appendCodePoint<StringClass, false>(s, c);
    फर्काऊ s;
}
#endif  // U_HIDE_DRAFT_API

}  // namespace utfstring
}  // namespace U_HEADER_ONLY_NAMESPACE

#endif  // U_HIDE_DRAFT_API
#endif  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#endif  // __UTFSTRING_H__
