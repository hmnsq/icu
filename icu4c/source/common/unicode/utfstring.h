// © 2025 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfstring.h
// created: 2025jul18 Markus W. Scherer

#నిర్వచితం_కాకపోతే __UTFSTRING_H__
#నిర్వచించు __UTFSTRING_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_శీర్షిక_API || !defined(UTYPES_H)

#అన్తర్భావ "unicode/utf16.h"

/**
 * \file
 * \brief C++ header-only API: C++ string helper functions.
 */

#నిర్వచితం_కాకపోతే U_HIDE_DRAFT_API

namespace U_శీర్షిక_ONLY_NAMESPACE {
namespace utfstring {

// Write code points to strings -------------------------------------------- ***

#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
namespace prv {

// This function, and the public wrappers,
// want to be U_FORCE_INLINE but the gcc-debug-build-and-test CI check failed with
// error: ‘always_inline’ function might not be inlinable [-Werror=attributes]
మూస <typename StringClass, bool validate>
అంతఃవరుస StringClass& append_సంకేతం_బిందువు(StringClass& s U_LIFETIME_BOUND, uint32_t c) {
    using Unit = typename StringClass::విలువ_type;
    ఒకవేళ constexpr (పరిమాణం(Unit) == 1) {
        // UTF-8: Similar to U8_APPEND().
        ఒకవేళ (c <= 0x7f) {
            s.నెట్టు_వెనుక(static_cast<Unit>(c));
        } లేకపోతే {
            Unit buf[4];
            uint8_t len;
            ఒకవేళ (c <= 0x7ff) {
                len = 2;
                buf[2] = (c >> 6) | 0xc0;
            } లేకపోతే {
                ఒకవేళ (validate ?
                        c < 0xd800 ||
                            (c < 0xe000 || c > 0x10ffff ? (c = 0xfffd, true) : c <= 0xffff) :
                        c <= 0xffff) {
                    len = 3;
                    buf[1] = (c >> 12) | 0xe0;
                } లేకపోతే {
                    len = 4;
                    buf[0] = (c >> 18) | 0xf0;
                    buf[1] = ((c >> 12) & 0x3f) | 0x80;
                }
                buf[2] = ((c >> 6) & 0x3f) | 0x80;
            }
            buf[3] = (c & 0x3f) | 0x80;
            s.append(buf + 4 - len, len);
        }
    } లేకపోతే ఒకవేళ constexpr (పరిమాణం(Unit) == 2) {
        // UTF-16: Similar to U16_APPEND().
        ఒకవేళ (validate ?
                c < 0xd800 || (c < 0xe000 || c > 0x10ffff ? (c = 0xfffd, true) : c <= 0xffff) :
                c <= 0xffff) {
            s.నెట్టు_వెనుక(static_cast<Unit>(c));
        } లేకపోతే {
            Unit buf[2] = { U16_LEAD(c), U16_TRAIL(c) };
            s.append(buf, 2);
        }
    } లేకపోతే {
        // UTF-32
        s.నెట్టు_వెనుక(!validate || U_IS_SCALAR_విలువ(c) ? c : 0xfffd);
    }
    తిరిగి s;
}

}  // namespace prv
#అయితే_ముగింపు  // U_IN_DOXYGEN

#నిర్వచితం_కాకపోతే U_HIDE_DRAFT_API
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
మూస <typename StringClass>
అంతఃవరుస StringClass& appendOrFFFD(StringClass& s U_LIFETIME_BOUND, UChar32 c) {
    తిరిగి prv::append_సంకేతం_బిందువు<StringClass, true>(s, c);
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
మూస <typename StringClass>
అంతఃవరుస StringClass& appendUnsafe(StringClass& s U_LIFETIME_BOUND, UChar32 c) {
    తిరిగి prv::append_సంకేతం_బిందువు<StringClass, false>(s, c);
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
మూస<typename StringClass>
అంతఃవరుస StringClass encodeOrFFFD(UChar32 c) {
    StringClass s;
    prv::append_సంకేతం_బిందువు<StringClass, true>(s, c);
    తిరిగి s;
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
మూస<typename StringClass>
అంతఃవరుస StringClass encodeUnsafe(UChar32 c) {
    StringClass s;
    prv::append_సంకేతం_బిందువు<StringClass, false>(s, c);
    తిరిగి s;
}
#అయితే_ముగింపు  // U_HIDE_DRAFT_API

}  // namespace utfstring
}  // namespace U_HEADER_ONLY_NAMESPACE

#అయితే_ముగింపు  // U_HIDE_DRAFT_API
#అయితే_ముగింపు  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#అయితే_ముగింపు  // __UTFSTRING_H__
