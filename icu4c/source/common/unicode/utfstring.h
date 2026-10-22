// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfstring.h
// created: 2025jul18 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UTFSTRING_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UTFSTRING_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_ಶೀರ್ಷಿಕೆ_API || !defined(UTYPES_H)

#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"

/**
 * \file
 * \brief C++ header-only API: C++ string helper functions.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_DRAFT_API

namespace U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE {
namespace utfstring {

// Write code points to strings -------------------------------------------- ***

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
namespace prv {

// This function, and the public wrappers,
// want to be U_FORCE_INLINE but the gcc-debug-build-and-test CI check failed with
// error: ‘always_inline’ function might not be inlinable [-Werror=attributes]
ಮಾದರಿ <typename StringClass, bool validate>
ಸಾಲೊಳಗೆ StringClass& append_ಸಂಕೇತ_ಬಿಂದು(StringClass& s U_LIFETIME_BOUND, uint32_t c) {
    using Unit = typename StringClass::ಮೌಲ್ಯ_type;
    ಆದರೆ constexpr (ಗಾತ್ರ(Unit) == 1) {
        // UTF-8: Similar to U8_APPEND().
        ಆದರೆ (c <= 0x7f) {
            s.ತಳ್ಳು_ಹಿಂಭಾಗ(static_cast<Unit>(c));
        } ಇಲ್ಲದಿದ್ದರೆ {
            Unit buf[4];
            uint8_t len;
            ಆದರೆ (c <= 0x7ff) {
                len = 2;
                buf[2] = (c >> 6) | 0xc0;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (validate ?
                        c < 0xd800 ||
                            (c < 0xe000 || c > 0x10ffff ? (c = 0xfffd, true) : c <= 0xffff) :
                        c <= 0xffff) {
                    len = 3;
                    buf[1] = (c >> 12) | 0xe0;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    len = 4;
                    buf[0] = (c >> 18) | 0xf0;
                    buf[1] = ((c >> 12) & 0x3f) | 0x80;
                }
                buf[2] = ((c >> 6) & 0x3f) | 0x80;
            }
            buf[3] = (c & 0x3f) | 0x80;
            s.append(buf + 4 - len, len);
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ constexpr (ಗಾತ್ರ(Unit) == 2) {
        // UTF-16: Similar to U16_APPEND().
        ಆದರೆ (validate ?
                c < 0xd800 || (c < 0xe000 || c > 0x10ffff ? (c = 0xfffd, true) : c <= 0xffff) :
                c <= 0xffff) {
            s.ತಳ್ಳು_ಹಿಂಭಾಗ(static_cast<Unit>(c));
        } ಇಲ್ಲದಿದ್ದರೆ {
            Unit buf[2] = { U16_LEAD(c), U16_TRAIL(c) };
            s.append(buf, 2);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // UTF-32
        s.ತಳ್ಳು_ಹಿಂಭಾಗ(!validate || U_IS_SCALAR_ಮೌಲ್ಯ(c) ? c : 0xfffd);
    }
    ಹಿಂತಿರುಗಿ s;
}

}  // namespace prv
#ಆದರೆ_ಅಂತ್ಯ  // U_IN_DOXYGEN

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_DRAFT_API
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
ಮಾದರಿ <typename StringClass>
ಸಾಲೊಳಗೆ StringClass& appendOrFFFD(StringClass& s U_LIFETIME_BOUND, UChar32 c) {
    ಹಿಂತಿರುಗಿ prv::append_ಸಂಕೇತ_ಬಿಂದು<StringClass, true>(s, c);
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
ಮಾದರಿ <typename StringClass>
ಸಾಲೊಳಗೆ StringClass& appendUnsafe(StringClass& s U_LIFETIME_BOUND, UChar32 c) {
    ಹಿಂತಿರುಗಿ prv::append_ಸಂಕೇತ_ಬಿಂದು<StringClass, false>(s, c);
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
ಮಾದರಿ<typename StringClass>
ಸಾಲೊಳಗೆ StringClass encodeOrFFFD(UChar32 c) {
    StringClass s;
    prv::append_ಸಂಕೇತ_ಬಿಂದು<StringClass, true>(s, c);
    ಹಿಂತಿರುಗಿ s;
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
ಮಾದರಿ<typename StringClass>
ಸಾಲೊಳಗೆ StringClass encodeUnsafe(UChar32 c) {
    StringClass s;
    prv::append_ಸಂಕೇತ_ಬಿಂದು<StringClass, false>(s, c);
    ಹಿಂತಿರುಗಿ s;
}
#ಆದರೆ_ಅಂತ್ಯ  // U_HIDE_DRAFT_API

}  // namespace utfstring
}  // namespace U_HEADER_ONLY_NAMESPACE

#ಆದರೆ_ಅಂತ್ಯ  // U_HIDE_DRAFT_API
#ಆದರೆ_ಅಂತ್ಯ  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#ಆದರೆ_ಅಂತ್ಯ  // __UTFSTRING_H__
