// © 2024 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfiterator.h
// created: 2024aug12 Markus W. Scherer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UTFITERATOR_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UTFITERATOR_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_ਸਿਰਲੇਖ_API || !defined(UTYPES_H)

#ਅਨ੍ਤਰ੍ਭਾਵ <iterator>
#ਜੇ __has_include(<ਸੰਸਕਰਣ>)
#ਅਨ੍ਤਰ੍ਭਾਵ <version>
#ਜੇ_ਅੰਤ
#ਜੇ defined(__cpp_lib_ranges)
#ਅਨ੍ਤਰ੍ਭਾਵ <ranges>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <string>
#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>
#ਅਨ੍ਤਰ੍ਭਾਵ <type_traits>
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uversion.h"

/**
 * \file
 * \brief C++ header-only API: C++ iterators over Unicode strings (=UTF-8/16/32 if well-formed).
 *
 * See the User Guide chapter about
 * <a href="https://unicode-org.github.io/icu/userguide/strings/cpp-code-point-iterator.html">C++ Code Point Iterators</a>.
 *
 * Sample code:
 * \code
 * #include <string_view>
 * #include <iostream>
 * #include "unicode/utypes.h"
 * #include "unicode/utfiterator.h"
 *
 * using icu::header::utfIterator;
 * using icu::header::utfStringCodePoints;
 * using icu::header::unsafeUTFIterator;
 * using icu::header::unsafeUTFStringCodePoints;
 *
 * int32_t rangeLoop16(std::u16string_view s) {
 *     // We are just adding up the code points for minimal-code demonstration purposes.
 *     int32_t sum = 0;
 *     for (auto units : utfStringCodePoints<UChar32, UTF_BEHAVIOR_NEGATIVE>(s)) {
 *         sum += units.codePoint();  // < 0 if ill-formed
 *     }
 *     return sum;
 * }
 *
 * int32_t loopIterPlusPlus16(std::u16string_view s) {
 *     auto range = utfStringCodePoints<char32_t, UTF_BEHAVIOR_FFFD>(s);
 *     int32_t sum = 0;
 *     for (auto iter = range.begin(), limit = range.end(); iter != limit;) {
 *         sum += (*iter++).codePoint();  // U+FFFD if ill-formed
 *     }
 *     return sum;
 * }
 *
 * int32_t backwardLoop16(std::u16string_view s) {
 *     auto range = utfStringCodePoints<UChar32, UTF_BEHAVIOR_SURROGATE>(s);
 *     int32_t sum = 0;
 *     for (auto start = range.begin(), iter = range.end(); start != iter;) {
 *         sum += (*--iter).codePoint();  // surrogate code point if unpaired / ill-formed
 *     }
 *     return sum;
 * }
 *
 * int32_t reverseLoop8(std::string_view s) {
 *     auto range = utfStringCodePoints<char32_t, UTF_BEHAVIOR_FFFD>(s);
 *     int32_t sum = 0;
 *     for (auto iter = range.rbegin(), limit = range.rend(); iter != limit; ++iter) {
 *         sum += iter->codePoint();  // U+FFFD if ill-formed
 *     }
 *     return sum;
 * }
 *
 * int32_t countCodePoints16(std::u16string_view s) {
 *     auto range = utfStringCodePoints<UChar32, UTF_BEHAVIOR_SURROGATE>(s);
 *     return std::distance(range.begin(), range.end());
 * }
 *
 * int32_t unsafeRangeLoop16(std::u16string_view s) {
 *     int32_t sum = 0;
 *     for (auto units : unsafeUTFStringCodePoints<UChar32>(s)) {
 *         sum += units.codePoint();
 *     }
 *     return sum;
 * }
 *
 * int32_t unsafeReverseLoop8(std::string_view s) {
 *     auto range = unsafeUTFStringCodePoints<UChar32>(s);
 *     int32_t sum = 0;
 *     for (auto iter = range.rbegin(), limit = range.rend(); iter != limit; ++iter) {
 *         sum += iter->codePoint();
 *     }
 *     return sum;
 * }
 *
 * char32_t firstCodePointOrFFFD16(std::u16string_view s) {
 *     if (s.empty()) { return 0xfffd; }
 *     auto range = utfStringCodePoints<char32_t, UTF_BEHAVIOR_FFFD>(s);
 *     return range.begin()->codePoint();
 * }
 *
 * std::string_view firstSequence8(std::string_view s) {
 *     if (s.empty()) { return {}; }
 *     auto range = utfStringCodePoints<char32_t, UTF_BEHAVIOR_FFFD>(s);
 *     auto units = *(range.begin());
 *     if (units.wellFormed()) {
 *         return units.stringView();
 *     } else {
 *         return {};
 *     }
 * }
 *
 * template<typename InputStream>  // some istream or streambuf
 * std::u32string cpFromInput(InputStream &in) {
 *     // This is a single-pass input_iterator.
 *     std::istreambuf_iterator bufIter(in);
 *     std::istreambuf_iterator<typename InputStream::char_type> bufLimit;
 *     auto iter = utfIterator<char32_t, UTF_BEHAVIOR_FFFD>(bufIter);
 *     auto limit = utfIterator<char32_t, UTF_BEHAVIOR_FFFD>(bufLimit);
 *     std::u32string s32;
 *     for (; iter != limit; ++iter) {
 *         s32.push_back(iter->codePoint());
 *     }
 *     return s32;
 * }
 *
 * std::u32string cpFromStdin() { return cpFromInput(std::cin); }
 * std::u32string cpFromWideStdin() { return cpFromInput(std::wcin); }
 * \endcode
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_HIDE_DRAFT_API

/**
 * Some defined behaviors for handling ill-formed Unicode strings.
 * This is a template parameter for UTFIterator and related classes.
 *
 * When a validating UTFIterator encounters an ill-formed code unit sequence,
 * then CodeUnits.codePoint() is a value according to this parameter.
 *
 * @draft ICU 78
 * @see CodeUnits
 * @see UTFIterator
 * @see UTFStringCodePoints
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UTFIllFormedBehavior {
    /**
     * Returns a negative value (-1=U_SENTINEL) instead of a code point.
     * If the CP32 template parameter for the relevant classes is an unsigned type,
     * then the negative value becomes 0xffffffff=UINT32_MAX.
     *
     * @draft ICU 78
     */
    UTF_BEHAVIOR_NEGATIVE,
    /** Returns U+FFFD Replacement Character. @draft ICU 78 */
    UTF_BEHAVIOR_FFFD,
    /**
     * UTF-8: Not allowed;
     * UTF-16: returns the unpaired surrogate;
     * UTF-32: returns the surrogate code point, or U+FFFD if out of range.
     *
     * @draft ICU 78
     */
    UTF_BEHAVIOR_SURROGATE
} UTFIllFormedBehavior;

namespace U_ਸਿਰਲੇਖ_ONLY_NAMESPACE {

namespace prv {
#ਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ >= 20

/** @internal */
ਸਾਂਚਾ<typename Iter>
using iter_ਮੁੱਲ_t = typename std::iter_ਮੁੱਲ_t<Iter>;

/** @internal */
ਸਾਂਚਾ<typename Iter>
using iter_difference_t = std::iter_difference_t<Iter>;

/** @internal */
ਸਾਂਚਾ<typename Iter>
constexpr bool forward_iterator = std::forward_iterator<Iter>;

/** @internal */
ਸਾਂਚਾ<typename Iter>
constexpr bool bidirectional_iterator = std::bidirectional_iterator<Iter>;

/** @internal */
ਸਾਂਚਾ<typename Range>
constexpr bool range = std::ranges::range<Range>;

#ਨਹੀਂਤਾਂ

/** @internal */
ਸਾਂਚਾ<typename Iter>
using iter_ਮੁੱਲ_t = typename std::iterator_traits<Iter>::ਮੁੱਲ_type;

/** @internal */
ਸਾਂਚਾ<typename Iter>
using iter_difference_t = typename std::iterator_traits<Iter>::difference_type;

/** @internal */
ਸਾਂਚਾ<typename Iter>
constexpr bool forward_iterator =
    std::is_ਅਧਾਰ_ਦਾ_v<
        std::forward_iterator_ਟੈਗ,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
ਸਾਂਚਾ<typename Iter>
constexpr bool bidirectional_iterator =
    std::is_ਅਧਾਰ_ਦਾ_v<
        std::bidirectional_iterator_ਟੈਗ,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
ਸਾਂਚਾ<typename Range, typename = ਖਾਲੀ>
ਬਣਤਰ range_type : std::false_type {};

/** @internal */
ਸਾਂਚਾ<typename Range>
ਬਣਤਰ range_type<
    Range,
    std::void_t<decltype(std::declval<Range>().begin()),
                decltype(std::declval<Range>().ਸਿਰਾ())>> : std::true_type {};

/** @internal */
ਸਾਂਚਾ<typename Range>
constexpr bool range = range_type<Range>::ਮੁੱਲ_2;

#ਜੇ_ਅੰਤ

/** @internal */
ਸਾਂਚਾ <typename T> ਬਣਤਰ is_basic_string_view : std::false_type {};

/** @internal */
ਸਾਂਚਾ <typename... ਦਲੀਲਾਂ>
ਬਣਤਰ is_basic_string_view<std::basic_string_view<ਦਲੀਲਾਂ...>> : std::true_type {};

/** @internal */
ਸਾਂਚਾ <typename T> constexpr bool is_basic_string_view_v = is_basic_string_view<T>::ਮੁੱਲ_2;

/** @internal */
ਸਾਂਚਾ<typename CP32, bool skipSurrogates>
class ਕੋਡ_Points_Iterator {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ਜਨਤਕ:
    /** C++ iterator boilerplate @internal */
    using ਮੁੱਲ_type = CP32;
    /** C++ iterator boilerplate @internal */
    using reference = ਮੁੱਲ_type;
    /** C++ iterator boilerplate @internal */
    using ਸੂਚਕ = CP32 *;
    /** C++ iterator boilerplate @internal */
    using difference_type = int32_t;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::forward_iterator_ਟੈਗ;

    /** @internal */
    ਸਤਰਅੰਦਰ ਕੋਡ_Points_Iterator(CP32 c) : c_(c) {}
    /** @internal */
    ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ==(ਅਚਲ ਕੋਡ_Points_Iterator &other) ਅਚਲ { ਵਾਪਸ c_ == other.c_; }
    /** @internal */
    ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ ਕੋਡ_Points_Iterator &other) ਅਚਲ { ਵਾਪਸ !ਸੰਚਾਲਕ==(other); }
    /** @internal */
    ਸਤਰਅੰਦਰ CP32 ਸੰਚਾਲਕ*() ਅਚਲ { ਵਾਪਸ c_; }
    /** @internal */
    ਸਤਰਅੰਦਰ ਕੋਡ_Points_Iterator& ਸੰਚਾਲਕ++() U_LIFETIME_BOUND {  // pre-increment
        ++c_;
        ਜੇ (skipSurrogates && c_ == 0xd800) {
            c_ = 0xe000;
        }
        ਵਾਪਸ *this;
    }
    /** @internal */
    ਸਤਰਅੰਦਰ ਕੋਡ_Points_Iterator ਸੰਚਾਲਕ++(ਪੂਰਨ) {  // post-increment
        ਕੋਡ_Points_Iterator ਨਤੀਜਾ(*this);
        ++(*this);
        ਵਾਪਸ ਨਤੀਜਾ;
    }

ਨਿਜੀ:
    CP32 c_;
};

}  // namespace prv

/**
 * A C++ "range" over all Unicode code points U+0000..U+10FFFF.
 * https://www.unicode.org/glossary/#code_point
 *
 * Intended for test and builder code.
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t
 * @draft ICU 78
 * @see U_IS_CODE_POINT
 */
ਸਾਂਚਾ<typename CP32>
class All_ਕੋਡ_Points {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ਜਨਤਕ:
    /** Constructor. @draft ICU 78 */
    All_ਕੋਡ_Points() {}
    /**
     * @return an iterator over all Unicode code points.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ begin() ਅਚਲ { ਵਾਪਸ prv::ਕੋਡ_Points_Iterator<CP32, false>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode code points.
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ prv::ਕੋਡ_Points_Iterator<CP32, false>(0x110000); }
};

/**
 * A C++ "range" over all Unicode scalar values U+0000..U+D7FF & U+E000..U+10FFFF.
 * That is, all code points except surrogates.
 * Only scalar values can be represented in well-formed UTF-8/16/32.
 * https://www.unicode.org/glossary/#unicode_scalar_value
 *
 * Intended for test and builder code.
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t
 * @draft ICU 78
 * @see U_IS_SCALAR_VALUE
 */
ਸਾਂਚਾ<typename CP32>
class All_Scalar_ਮੁੱਲਾਂ {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ਜਨਤਕ:
    /** Constructor. @draft ICU 78 */
    All_Scalar_ਮੁੱਲਾਂ() {}
    /**
     * @return an iterator over all Unicode scalar values.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ begin() ਅਚਲ { ਵਾਪਸ prv::ਕੋਡ_Points_Iterator<CP32, true>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode scalar values.
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ prv::ਕੋਡ_Points_Iterator<CP32, true>(0x110000); }
};

/**
 * Result of decoding a code unit sequence for one code point.
 * Returned from non-validating Unicode string code point iterators.
 * Base class for class CodeUnits which is returned from validating iterators.
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t;
 *              should be signed if UTF_BEHAVIOR_NEGATIVE
 * @tparam UnitIter An iterator (often a pointer) that returns a code unit type:
 *     UTF-8: char or char8_t or uint8_t;
 *     UTF-16: char16_t or uint16_t or (on Windows) wchar_t;
 *     UTF-32: char32_t or UChar32=int32_t or (on Linux) wchar_t
 * @see UnsafeUTFIterator
 * @see UnsafeUTFStringCodePoints
 * @draft ICU 78
 */
ਸਾਂਚਾ<typename CP32, typename UnitIter, typename = ਖਾਲੀ>
class Unsafe_ਕੋਡ_Units {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Unit = typename prv::iter_ਮੁੱਲ_t<UnitIter>;
ਜਨਤਕ:
    /** @internal */
    Unsafe_ਕੋਡ_Units(CP32 ਕੋਡ_ਬਿੰਦੂ, uint8_t ਲੰਮਾਈ, UnitIter ਸ਼ੁਰੂ, UnitIter ਹੱਦਬੰਦੀ) :
            c_(ਕੋਡ_ਬਿੰਦੂ), len_(ਲੰਮਾਈ), ਸ਼ੁਰੂ_3(ਸ਼ੁਰੂ), ਹੱਦਬੰਦੀ_3(ਹੱਦਬੰਦੀ) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_ਕੋਡ_Units(ਅਚਲ Unsafe_ਕੋਡ_Units &other) = ਮੂਲ;
    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_ਕੋਡ_Units &ਸੰਚਾਲਕ=(ਅਚਲ Unsafe_ਕੋਡ_Units &other) = ਮੂਲ;

    /**
     * @return the Unicode code point decoded from the code unit sequence.
     *     If the sequence is ill-formed and the iterator validates,
     *     then this is a replacement value according to the iterator‘s
     *     UTFIllFormedBehavior template parameter.
     * @draft ICU 78
     */
    CP32 ਕੋਡ_ਬਿੰਦੂ() ਅਚਲ { ਵਾਪਸ c_; }

    /**
     * @return the start of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter begin() ਅਚਲ { ਵਾਪਸ ਸ਼ੁਰੂ_3; }

    /**
     * @return the limit (exclusive end) of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ ਹੱਦਬੰਦੀ_3; }

    /**
     * @return the length of the code unit sequence for one code point.
     * @draft ICU 78
     */
    uint8_t ਲੰਮਾਈ() ਅਚਲ { ਵਾਪਸ len_; }

#ਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ >= 20
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    ਸਾਂਚਾ<std::contiguous_iterator Iter = UnitIter>
    std::basic_string_view<Unit> stringView() ਅਚਲ {
        ਵਾਪਸ std::basic_string_view<Unit>(begin(), ਸਿਰਾ());
    }
#ਨਹੀਂਤਾਂ
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename Iter = UnitIter, typename Unit = typename std::iterator_traits<Iter>::ਮੁੱਲ_type>
    std::enable_if_t<std::is_ਸੂਚਕ_v<Iter> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::const_iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::const_iterator>,
                     std::basic_string_view<Unit>>
    stringView() ਅਚਲ {
        ਵਾਪਸ std::basic_string_view<Unit>(&*ਸ਼ੁਰੂ_3, len_);
    }
#ਜੇ_ਅੰਤ

ਨਿਜੀ:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
    UnitIter ਸ਼ੁਰੂ_3;
    UnitIter ਹੱਦਬੰਦੀ_3;
};

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_ਵਿੱਚ_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
ਸਾਂਚਾ<typename CP32, typename UnitIter>
class Unsafe_ਕੋਡ_Units<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ਜਨਤਕ:
    Unsafe_ਕੋਡ_Units(CP32 ਕੋਡ_ਬਿੰਦੂ, uint8_t ਲੰਮਾਈ) : c_(ਕੋਡ_ਬਿੰਦੂ), len_(ਲੰਮਾਈ) {}

    Unsafe_ਕੋਡ_Units(ਅਚਲ Unsafe_ਕੋਡ_Units &other) = ਮੂਲ;
    Unsafe_ਕੋਡ_Units &ਸੰਚਾਲਕ=(ਅਚਲ Unsafe_ਕੋਡ_Units &other) = ਮੂਲ;

    CP32 ਕੋਡ_ਬਿੰਦੂ() ਅਚਲ { ਵਾਪਸ c_; }

    uint8_t ਲੰਮਾਈ() ਅਚਲ { ਵਾਪਸ len_; }

ਨਿਜੀ:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
};
#ਜੇ_ਅੰਤ  // U_IN_DOXYGEN

/**
 * Result of validating and decoding a code unit sequence for one code point.
 * Returned from validating Unicode string code point iterators.
 * Adds function wellFormed() to base class UnsafeCodeUnits.
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t;
 *              should be signed if UTF_BEHAVIOR_NEGATIVE
 * @tparam UnitIter An iterator (often a pointer) that returns a code unit type:
 *     UTF-8: char or char8_t or uint8_t;
 *     UTF-16: char16_t or uint16_t or (on Windows) wchar_t;
 *     UTF-32: char32_t or UChar32=int32_t or (on Linux) wchar_t
 * @see UTFIterator
 * @see UTFStringCodePoints
 * @draft ICU 78
 */
ਸਾਂਚਾ<typename CP32, typename UnitIter, typename = ਖਾਲੀ>
class ਕੋਡ_Units_2 : ਜਨਤਕ Unsafe_ਕੋਡ_Units<CP32, UnitIter> {
ਜਨਤਕ:
    /** @internal */
    ਕੋਡ_Units_2(CP32 ਕੋਡ_ਬਿੰਦੂ, uint8_t ਲੰਮਾਈ, bool wellFormed, UnitIter ਸ਼ੁਰੂ, UnitIter ਹੱਦਬੰਦੀ) :
            Unsafe_ਕੋਡ_Units<CP32, UnitIter>(ਕੋਡ_ਬਿੰਦੂ, ਲੰਮਾਈ, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ), ok_(wellFormed) {}

    /** Copy constructor. @draft ICU 78 */
    ਕੋਡ_Units_2(ਅਚਲ ਕੋਡ_Units_2 &other) = ਮੂਲ;
    /** Copy assignment operator. @draft ICU 78 */
    ਕੋਡ_Units_2 &ਸੰਚਾਲਕ=(ਅਚਲ ਕੋਡ_Units_2 &other) = ਮੂਲ;

    /**
     * @return true if the decoded code unit sequence is well-formed.
     * @draft ICU 78
     */
    bool wellFormed() ਅਚਲ { ਵਾਪਸ ok_; }

ਨਿਜੀ:
    bool ok_;
};

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_ਵਿੱਚ_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
ਸਾਂਚਾ<typename CP32, typename UnitIter>
class ਕੋਡ_Units_2<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> :
            ਜਨਤਕ Unsafe_ਕੋਡ_Units<CP32, UnitIter> {
ਜਨਤਕ:
    ਕੋਡ_Units_2(CP32 ਕੋਡ_ਬਿੰਦੂ, uint8_t ਲੰਮਾਈ, bool wellFormed) :
            Unsafe_ਕੋਡ_Units<CP32, UnitIter>(ਕੋਡ_ਬਿੰਦੂ, ਲੰਮਾਈ), ok_(wellFormed) {}

    ਕੋਡ_Units_2(ਅਚਲ ਕੋਡ_Units_2 &other) = ਮੂਲ;
    ਕੋਡ_Units_2 &ਸੰਚਾਲਕ=(ਅਚਲ ਕੋਡ_Units_2 &other) = ਮੂਲ;

    bool wellFormed() ਅਚਲ { ਵਾਪਸ ok_; }

ਨਿਜੀ:
    bool ok_;
};
#ਜੇ_ਅੰਤ  // U_IN_DOXYGEN

// Validating implementations ---------------------------------------------- ***

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_ਵਿੱਚ_DOXYGEN
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename ਹੱਦਬੰਦੀ_Iter = UnitIter, typename = ਖਾਲੀ>
class UTFImpl;

// Note: readAndInc() functions take both a p0 and a p iterator.
// They must have the same value.
// For a multi-pass UnitIter, the caller must copy its p into a local variable p0,
// and readAndInc() copies p0 and the incremented p into the CodeUnits.
// For a single-pass UnitIter, which may not be default-constructible nor coypable,
// the caller can pass p into both references, and readAndInc() does not use p0
// and constructs CodeUnits without them.
// Moving the p0 variable into the call site avoids having to declare it inside readAndInc()
// which may not be possible for a single-pass iterator.

// UTF-8
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename ਹੱਦਬੰਦੀ_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, ਹੱਦਬੰਦੀ_Iter,
        std::enable_if_t<ਆਕਾਰ(typename prv::iter_ਮੁੱਲ_t<UnitIter>) == 1>> {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    static_assert(behavior != UTF_BEHAVIOR_SURROGATE,
                  "For 8-bit strings, the SURROGATE option does not have an equivalent.");
ਜਨਤਕ:
    // Handle ill-formed UTF-8
    U_FORCE_INLINE ਸਥਿਰ CP32 sub() {
        ਜੇ constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            ਵਾਪਸ U_SENTINEL;
        } ਨਹੀਂਤਾਂ {
            static_assert(behavior == UTF_BEHAVIOR_FFFD);
            ਵਾਪਸ 0xfffd;
        }
    }

    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ inc(UnitIter &p, ਅਚਲ ਹੱਦਬੰਦੀ_Iter &ਹੱਦਬੰਦੀ) {
        // Very similar to U8_FWD_1().
        uint8_t b = *p;
        ++p;
        ਜੇ (U8_IS_LEAD(b) && p != ਹੱਦਬੰਦੀ) {
            uint8_t t1 = *p;
            ਜੇ ((0xe0 <= b && b < 0xf0)) {
                ਜੇ (U8_IS_VALID_LEAD3_AND_T1(b, t1) &&
                        ++p != ਹੱਦਬੰਦੀ && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (b < 0xe0) {
                ਜੇ (U8_IS_TRAIL(t1)) {
                    ++p;
                }
            } ਨਹੀਂਤਾਂ /* b >= 0xf0 */ {
                ਜੇ (U8_IS_VALID_LEAD4_AND_T1(b, t1) &&
                        ++p != ਹੱਦਬੰਦੀ && U8_IS_TRAIL(*p) &&
                        ++p != ਹੱਦਬੰਦੀ && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            }
        }
    }

    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ dec(UnitIter ਸ਼ੁਰੂ, UnitIter &p) {
        // Very similar to U8_BACK_1().
        uint8_t c = *--p;
        ਜੇ (U8_IS_TRAIL(c) && p != ਸ਼ੁਰੂ) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            ਜੇ (U8_IS_LEAD(b1)) {
                ਜੇ (b1 < 0xe0 ||
                        (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c))) {
                    p = p1;
                    ਵਾਪਸ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (U8_IS_TRAIL(b1) && p1 != ਸ਼ੁਰੂ) {
                uint8_t b2 = *--p1;
                ਜੇ (0xe0 <= b2 && b2 <= 0xf4) {
                    ਜੇ (b2 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b2, b1) :
                            U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        p = p1;
                        ਵਾਪਸ;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ (U8_IS_TRAIL(b2) && p1 != ਸ਼ੁਰੂ) {
                    uint8_t b3 = *--p1;
                    ਜੇ (0xf0 <= b3 && b3 <= 0xf4 && U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                        p = p1;
                        ਵਾਪਸ;
                    }
                }
            }
        }
    }

    U_FORCE_INLINE ਸਥਿਰ ਕੋਡ_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ਅਚਲ ਹੱਦਬੰਦੀ_Iter &ਹੱਦਬੰਦੀ) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_OR_FFFD().
        CP32 c = uint8_t(*p);
        ++p;
        ਜੇ (U8_IS_SINGLE(c)) {
            ਜੇ constexpr (isMultiPass) {
                ਵਾਪਸ {c, 1, true, p0, p};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {c, 1, true};
            }
        }
        uint8_t ਲੰਮਾਈ = 1;
        uint8_t t = 0;
        ਜੇ (p != ਹੱਦਬੰਦੀ &&
                // fetch/validate/assemble all but last trail byte
                (c >= 0xe0 ?
                    (c < 0xf0 ?  // U+0800..U+FFFF except surrogates
                        U8_LEAD3_T1_ਬਿੱਟ[c &= 0xf] & (1 << ((t = *p) >> 5)) &&
                        (t &= 0x3f, 1)
                    :  // U+10000..U+10FFFF
                        (c -= 0xf0) <= 4 &&
                        U8_LEAD4_T1_ਬਿੱਟ[(t = *p) >> 4] & (1 << c) &&
                        (c = (c << 6) | (t & 0x3f), ++ਲੰਮਾਈ, ++p != ਹੱਦਬੰਦੀ) &&
                        (t = *p - 0x80) <= 0x3f) &&
                    // valid second-to-last trail byte
                    (c = (c << 6) | t, ++ਲੰਮਾਈ, ++p != ਹੱਦਬੰਦੀ)
                :  // U+0080..U+07FF
                    c >= 0xc2 && (c &= 0x1f, 1)) &&
                // last trail byte
                (t = *p - 0x80) <= 0x3f) {
            c = (c << 6) | t;
            ++ਲੰਮਾਈ;
            ++p;
            ਜੇ constexpr (isMultiPass) {
                ਵਾਪਸ {c, ਲੰਮਾਈ, true, p0, p};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {c, ਲੰਮਾਈ, true};
            }
        }
        ਜੇ constexpr (isMultiPass) {
            ਵਾਪਸ {sub(), ਲੰਮਾਈ, false, p0, p};
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ {sub(), ਲੰਮਾਈ, false};
        }
    }

    U_FORCE_INLINE ਸਥਿਰ ਕੋਡ_Units_2<CP32, UnitIter> decAndRead(UnitIter ਸ਼ੁਰੂ, UnitIter &p) {
        // Very similar to U8_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        ਜੇ (U8_IS_SINGLE(c)) {
            ਵਾਪਸ {c, 1, true, p, p0};
        }
        ਜੇ (U8_IS_TRAIL(c) && p != ਸ਼ੁਰੂ) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            ਜੇ (U8_IS_LEAD(b1)) {
                ਜੇ (b1 < 0xe0) {
                    p = p1;
                    c = ((b1 - 0xc0) << 6) | (c & 0x3f);
                    ਵਾਪਸ {c, 2, true, p, p0};
                } ਨਹੀਂਤਾਂ ਜੇ (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c)) {
                    // Truncated 3- or 4-byte sequence.
                    p = p1;
                    ਵਾਪਸ {sub(), 2, false, p, p0};
                }
            } ਨਹੀਂਤਾਂ ਜੇ (U8_IS_TRAIL(b1) && p1 != ਸ਼ੁਰੂ) {
                // Extract the value bits from the last trail byte.
                c &= 0x3f;
                uint8_t b2 = *--p1;
                ਜੇ (0xe0 <= b2 && b2 <= 0xf4) {
                    ਜੇ (b2 < 0xf0) {
                        b2 &= 0xf;
                        ਜੇ (U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                            p = p1;
                            c = (b2 << 12) | ((b1 & 0x3f) << 6) | c;
                            ਵਾਪਸ {c, 3, true, p, p0};
                        }
                    } ਨਹੀਂਤਾਂ ਜੇ (U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        // Truncated 4-byte sequence.
                        p = p1;
                        ਵਾਪਸ {sub(), 3, false, p, p0};
                    }
                } ਨਹੀਂਤਾਂ ਜੇ (U8_IS_TRAIL(b2) && p1 != ਸ਼ੁਰੂ) {
                    uint8_t b3 = *--p1;
                    ਜੇ (0xf0 <= b3 && b3 <= 0xf4) {
                        b3 &= 7;
                        ਜੇ (U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                            p = p1;
                            c = (b3 << 18) | ((b2 & 0x3f) << 12) | ((b1 & 0x3f) << 6) | c;
                            ਵਾਪਸ {c, 4, true, p, p0};
                        }
                    }
                }
            }
        }
        ਵਾਪਸ {sub(), 1, false, p, p0};
    }
};

// UTF-16
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename ਹੱਦਬੰਦੀ_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, ਹੱਦਬੰਦੀ_Iter,
        std::enable_if_t<ਆਕਾਰ(typename prv::iter_ਮੁੱਲ_t<UnitIter>) == 2>> {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ਜਨਤਕ:
    // Handle ill-formed UTF-16: One unpaired surrogate.
    U_FORCE_INLINE ਸਥਿਰ CP32 sub(CP32 surrogate) {
        ਜੇ constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            ਵਾਪਸ U_SENTINEL;
        } ਨਹੀਂਤਾਂ ਜੇ constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            ਵਾਪਸ 0xfffd;
        } ਨਹੀਂਤਾਂ {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            ਵਾਪਸ surrogate;
        }
    }

    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ inc(UnitIter &p, ਅਚਲ ਹੱਦਬੰਦੀ_Iter &ਹੱਦਬੰਦੀ) {
        // Very similar to U16_FWD_1().
        ਸਵੈਚਾਲਿਤ c = *p;
        ++p;
        ਜੇ (U16_IS_LEAD(c) && p != ਹੱਦਬੰਦੀ && U16_IS_TRAIL(*p)) {
            ++p;
        }
    }

    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ dec(UnitIter ਸ਼ੁਰੂ, UnitIter &p) {
        // Very similar to U16_BACK_1().
        UnitIter p1;
        ਜੇ (U16_IS_TRAIL(*--p) && p != ਸ਼ੁਰੂ && (p1 = p, U16_IS_LEAD(*--p1))) {
            p = p1;
        }
    }

    U_FORCE_INLINE ਸਥਿਰ ਕੋਡ_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ਅਚਲ ਹੱਦਬੰਦੀ_Iter &ਹੱਦਬੰਦੀ) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_OR_FFFD().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        ਜੇ (!U16_IS_SURROGATE(c)) {
            ਜੇ constexpr (isMultiPass) {
                ਵਾਪਸ {c, 1, true, p0, p};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {c, 1, true};
            }
        } ਨਹੀਂਤਾਂ {
            uint16_t c2;
            ਜੇ (U16_IS_SURROGATE_LEAD(c) && p != ਹੱਦਬੰਦੀ && U16_IS_TRAIL(c2 = *p)) {
                ++p;
                c = U16_GET_SUPPLEMENTARY(c, c2);
                ਜੇ constexpr (isMultiPass) {
                    ਵਾਪਸ {c, 2, true, p0, p};
                } ਨਹੀਂਤਾਂ {
                    ਵਾਪਸ {c, 2, true};
                }
            } ਨਹੀਂਤਾਂ {
                ਜੇ constexpr (isMultiPass) {
                    ਵਾਪਸ {sub(c), 1, false, p0, p};
                } ਨਹੀਂਤਾਂ {
                    ਵਾਪਸ {sub(c), 1, false};
                }
            }
        }
    }

    U_FORCE_INLINE ਸਥਿਰ ਕੋਡ_Units_2<CP32, UnitIter> decAndRead(UnitIter ਸ਼ੁਰੂ, UnitIter &p) {
        // Very similar to U16_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        ਜੇ (!U16_IS_SURROGATE(c)) {
            ਵਾਪਸ {c, 1, true, p, p0};
        } ਨਹੀਂਤਾਂ {
            UnitIter p1;
            uint16_t c2;
            ਜੇ (U16_IS_SURROGATE_TRAIL(c) && p != ਸ਼ੁਰੂ && (p1 = p, U16_IS_LEAD(c2 = *--p1))) {
                p = p1;
                c = U16_GET_SUPPLEMENTARY(c2, c);
                ਵਾਪਸ {c, 2, true, p, p0};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {sub(c), 1, false, p, p0};
            }
        }
    }
};

// UTF-32: trivial, but still validating
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename ਹੱਦਬੰਦੀ_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, ਹੱਦਬੰਦੀ_Iter,
        std::enable_if_t<ਆਕਾਰ(typename prv::iter_ਮੁੱਲ_t<UnitIter>) == 4>> {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ਜਨਤਕ:
    // Handle ill-formed UTF-32
    U_FORCE_INLINE ਸਥਿਰ CP32 sub(bool forSurrogate, CP32 surrogate) {
        ਜੇ constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            ਵਾਪਸ U_SENTINEL;
        } ਨਹੀਂਤਾਂ ਜੇ constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            ਵਾਪਸ 0xfffd;
        } ਨਹੀਂਤਾਂ {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            ਵਾਪਸ forSurrogate ? surrogate : 0xfffd;
        }
    }

    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ inc(UnitIter &p, ਅਚਲ ਹੱਦਬੰਦੀ_Iter &/*ਹੱਦਬੰਦੀ*/) {
        ++p;
    }

    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ dec(UnitIter /*ਸ਼ੁਰੂ*/, UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE ਸਥਿਰ ਕੋਡ_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ਅਚਲ ਹੱਦਬੰਦੀ_Iter &/*ਹੱਦਬੰਦੀ*/) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        uint32_t uc = *p;
        CP32 c = uc;
        ++p;
        ਜੇ (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            ਜੇ constexpr (isMultiPass) {
                ਵਾਪਸ {c, 1, true, p0, p};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {c, 1, true};
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ constexpr (isMultiPass) {
                ਵਾਪਸ {sub(uc < 0xe000, c), 1, false, p0, p};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {sub(uc < 0xe000, c), 1, false};
            }
        }
    }

    U_FORCE_INLINE ਸਥਿਰ ਕੋਡ_Units_2<CP32, UnitIter> decAndRead(UnitIter /*ਸ਼ੁਰੂ*/, UnitIter &p) {
        UnitIter p0 = p;
        uint32_t uc = *--p;
        CP32 c = uc;
        ਜੇ (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            ਵਾਪਸ {c, 1, true, p, p0};
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ {sub(uc < 0xe000, c), 1, false, p, p0};
        }
    }
};

// Non-validating implementations ------------------------------------------ ***

ਸਾਂਚਾ<typename CP32, typename UnitIter, typename = ਖਾਲੀ>
class UnsafeUTFImpl;

// UTF-8
ਸਾਂਚਾ<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<ਆਕਾਰ(typename prv::iter_ਮੁੱਲ_t<UnitIter>) == 1>> {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ਜਨਤਕ:
    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ inc(UnitIter &p) {
        // Very similar to U8_FWD_1_UNSAFE().
        uint8_t b = *p;
        std::advance(p, 1 + U8_ਗਿਣਤੀ_TRAIL_ਬਾਈਟਾਂ_UNSAFE(b));
    }

    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ dec(UnitIter &p) {
        // Very similar to U8_BACK_1_UNSAFE().
        ਜਦੋਂ (U8_IS_TRAIL(*--p)) {}
    }

    U_FORCE_INLINE ਸਥਿਰ Unsafe_ਕੋਡ_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_UNSAFE().
        CP32 c = uint8_t(*p);
        ++p;
        ਜੇ (U8_IS_SINGLE(c)) {
            ਜੇ constexpr (isMultiPass) {
                ਵਾਪਸ {c, 1, p0, p};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {c, 1};
            }
        } ਨਹੀਂਤਾਂ ਜੇ (c < 0xe0) {
            c = ((c & 0x1f) << 6) | (*p & 0x3f);
            ++p;
            ਜੇ constexpr (isMultiPass) {
                ਵਾਪਸ {c, 2, p0, p};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {c, 2};
            }
        } ਨਹੀਂਤਾਂ ਜੇ (c < 0xf0) {
            // No need for (c&0xf) because the upper bits are truncated
            // after <<12 in the cast to uint16_t.
            c = uint16_t(c << 12) | ((*p & 0x3f) << 6);
            ++p;
            c |= *p & 0x3f;
            ++p;
            ਜੇ constexpr (isMultiPass) {
                ਵਾਪਸ {c, 3, p0, p};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {c, 3};
            }
        } ਨਹੀਂਤਾਂ {
            c = ((c & 7) << 18) | ((*p & 0x3f) << 12);
            ++p;
            c |= (*p & 0x3f) << 6;
            ++p;
            c |= *p & 0x3f;
            ++p;
            ਜੇ constexpr (isMultiPass) {
                ਵਾਪਸ {c, 4, p0, p};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {c, 4};
            }
        }
    }

    U_FORCE_INLINE ਸਥਿਰ Unsafe_ਕੋਡ_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U8_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        ਜੇ (U8_IS_SINGLE(c)) {
            ਵਾਪਸ {c, 1, p, p0};
        }
        // U8_IS_TRAIL(c) if well-formed
        c &= 0x3f;
        uint8_t ਗਿਣਤੀ_2 = 1;
        ਲਈ (uint8_t ਖਿਸਕਾਓ = 6;;) {
            uint8_t b = *--p;
            ਜੇ (b >= 0xc0) {
                U8_ਨਕਾਬ_LEAD_BYTE(b, ਗਿਣਤੀ_2);
                c |= uint32_t{b} << ਖਿਸਕਾਓ;
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ {
                c |= (uint32_t{b} & 0x3f) << ਖਿਸਕਾਓ;
                ++ਗਿਣਤੀ_2;
                ਖਿਸਕਾਓ += 6;
            }
        }
        ++ਗਿਣਤੀ_2;
        ਵਾਪਸ {c, ਗਿਣਤੀ_2, p, p0};
    }
};

// UTF-16
ਸਾਂਚਾ<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<ਆਕਾਰ(typename prv::iter_ਮੁੱਲ_t<UnitIter>) == 2>> {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ਜਨਤਕ:
    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ inc(UnitIter &p) {
        // Very similar to U16_FWD_1_UNSAFE().
        ਸਵੈਚਾਲਿਤ c = *p;
        ++p;
        ਜੇ (U16_IS_LEAD(c)) {
            ++p;
        }
    }

    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ dec(UnitIter &p) {
        // Very similar to U16_BACK_1_UNSAFE().
        ਜੇ (U16_IS_TRAIL(*--p)) {
            --p;
        }
    }

    U_FORCE_INLINE ਸਥਿਰ Unsafe_ਕੋਡ_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_UNSAFE().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        ਜੇ (!U16_IS_LEAD(c)) {
            ਜੇ constexpr (isMultiPass) {
                ਵਾਪਸ {c, 1, p0, p};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {c, 1};
            }
        } ਨਹੀਂਤਾਂ {
            uint16_t c2 = *p;
            ++p;
            c = U16_GET_SUPPLEMENTARY(c, c2);
            ਜੇ constexpr (isMultiPass) {
                ਵਾਪਸ {c, 2, p0, p};
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ {c, 2};
            }
        }
    }

    U_FORCE_INLINE ਸਥਿਰ Unsafe_ਕੋਡ_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U16_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        ਜੇ (!U16_IS_TRAIL(c)) {
            ਵਾਪਸ {c, 1, p, p0};
        } ਨਹੀਂਤਾਂ {
            uint16_t c2 = *--p;
            c = U16_GET_SUPPLEMENTARY(c2, c);
            ਵਾਪਸ {c, 2, p, p0};
        }
    }
};

// UTF-32: trivial
ਸਾਂਚਾ<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<ਆਕਾਰ(typename prv::iter_ਮੁੱਲ_t<UnitIter>) == 4>> {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ਜਨਤਕ:
    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ inc(UnitIter &p) {
        ++p;
    }

    U_FORCE_INLINE ਸਥਿਰ ਖਾਲੀ dec(UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE ਸਥਿਰ Unsafe_ਕੋਡ_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        CP32 c = *p;
        ++p;
        ਜੇ constexpr (isMultiPass) {
            ਵਾਪਸ {c, 1, p0, p};
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ {c, 1};
        }
    }

    U_FORCE_INLINE ਸਥਿਰ Unsafe_ਕੋਡ_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        UnitIter p0 = p;
        CP32 c = *--p;
        ਵਾਪਸ {c, 1, p, p0};
    }
};

#ਜੇ_ਅੰਤ

// Validating iterators ---------------------------------------------------- ***

/**
 * Validating iterator over the code points in a Unicode string.
 *
 * The UnitIter can be
 * an input_iterator, a forward_iterator, or a bidirectional_iterator (including a pointer).
 * The UTFIterator will have the corresponding iterator_category.
 *
 * Call utfIterator() to have the compiler deduce the UnitIter and LimitIter types.
 *
 * For reverse iteration, either use this iterator directly as in <code>*--iter</code>
 * or wrap it using std::make_reverse_iterator(iter).
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t;
 *              should be signed if UTF_BEHAVIOR_NEGATIVE
 * @tparam behavior How to handle ill-formed Unicode strings
 * @tparam UnitIter An iterator (often a pointer) that returns a code unit type:
 *     UTF-8: char or char8_t or uint8_t;
 *     UTF-16: char16_t or uint16_t or (on Windows) wchar_t;
 *     UTF-32: char32_t or UChar32=int32_t or (on Linux) wchar_t
 * @tparam LimitIter Either the same as UnitIter, or an iterator sentinel type.
 * @draft ICU 78
 * @see utfIterator
 */
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename ਹੱਦਬੰਦੀ_Iter = UnitIter, typename = ਖਾਲੀ>
class UTFIterator {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, ਹੱਦਬੰਦੀ_Iter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    ਜਨਤਕ:
        explicit Proxy(ਕੋਡ_Units_2<CP32, UnitIter> &units) : units_(units) {}
        ਕੋਡ_Units_2<CP32, UnitIter>& ਸੰਚਾਲਕ*() U_LIFETIME_BOUND { ਵਾਪਸ units_; }
        ਕੋਡ_Units_2<CP32, UnitIter>* ਸੰਚਾਲਕ->() U_LIFETIME_BOUND { ਵਾਪਸ &units_; }
    ਨਿਜੀ:
        ਕੋਡ_Units_2<CP32, UnitIter> units_;
    };

ਜਨਤਕ:
    /** C++ iterator boilerplate @internal */
    using ਮੁੱਲ_type = ਕੋਡ_Units_2<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = ਮੁੱਲ_type;
    /** C++ iterator boilerplate @internal */
    using ਸੂਚਕ = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_ਟੈਗ,
        std::forward_iterator_ਟੈਗ>;

    /**
     * Constructor with start <= p < limit.
     * All of these iterators/pointers should be at code point boundaries.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     *
     * When using a code unit sentinel (UnitIter≠LimitIter),
     * then that sentinel also works as a sentinel for this code point iterator.
     *
     * @param start Start of the range
     * @param p Initial position inside the range
     * @param limit Limit (exclusive end) of the range
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator(UnitIter ਸ਼ੁਰੂ, UnitIter p, ਹੱਦਬੰਦੀ_Iter ਹੱਦਬੰਦੀ) :
            p_(p), ਸ਼ੁਰੂ_3(ਸ਼ੁਰੂ), ਹੱਦਬੰਦੀ_3(ਹੱਦਬੰਦੀ), units_(0, 0, false, p, p) {}
    /**
     * Constructor with start == p < limit.
     * All of these iterators/pointers should be at code point boundaries.
     *
     * When using a code unit sentinel (UnitIter≠LimitIter),
     * then that sentinel also works as a sentinel for this code point iterator.
     *
     * @param p Start of the range, and the initial position
     * @param limit Limit (exclusive end) of the range
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator(UnitIter p, ਹੱਦਬੰਦੀ_Iter ਹੱਦਬੰਦੀ) :
            p_(p), ਸ਼ੁਰੂ_3(p), ਹੱਦਬੰਦੀ_3(ਹੱਦਬੰਦੀ), units_(0, 0, false, p, p) {}
    /**
     * Constructs an iterator start or limit sentinel.
     * The iterator/pointer should be at a code point boundary.
     * Requires UnitIter to be copyable.
     *
     * When using a code unit sentinel (UnitIter≠LimitIter),
     * then that sentinel also works as a sentinel for this code point iterator.
     *
     * @param p Range start or limit
     * @draft ICU 78
     */
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(p), ਸ਼ੁਰੂ_3(p), ਹੱਦਬੰਦੀ_3(p), units_(0, 0, false, p, p) {}
    /**
     * Default constructor. Makes a non-functional iterator.
     *
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator() : p_{}, ਸ਼ੁਰੂ_3{}, ਹੱਦਬੰਦੀ_3{}, units_(0, 0, false, p_, p_) {}

    /** Move constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = ਮੂਲ;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &ਸੰਚਾਲਕ=(UTFIterator &&src) noexcept = ਮੂਲ;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(ਅਚਲ UTFIterator &other) = ਮੂਲ;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &ਸੰਚਾਲਕ=(ਅਚਲ UTFIterator &other) = ਮੂਲ;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool ਸੰਚਾਲਕ==(ਅਚਲ UTFIterator &other) ਅਚਲ {
        ਵਾਪਸ ਅਧਾਰ() == other.ਅਧਾਰ();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool ਸੰਚਾਲਕ!=(ਅਚਲ UTFIterator &other) ਅਚਲ { ਵਾਪਸ !ਸੰਚਾਲਕ==(other); }

    // Asymmetric equality & nonequality with a sentinel type.

    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ==(ਅਚਲ UTFIterator &iter, ਅਚਲ Sentinel &s) {
        ਵਾਪਸ iter.ਅਧਾਰ() == s;
    }

#ਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ < 20
    // C++17: Need to define all four combinations of == / != vs. parameter order.
    // Once we require C++20, we could remove all but the first == because
    // the compiler would generate the rest.

    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ==(ਅਚਲ Sentinel &s, ਅਚਲ UTFIterator &iter) {
        ਵਾਪਸ iter.ਅਧਾਰ() == s;
    }
    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ!=(ਅਚਲ UTFIterator &iter, ਅਚਲ Sentinel &s) { ਵਾਪਸ !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ!=(ਅਚਲ Sentinel &s, ਅਚਲ UTFIterator &iter) { ਵਾਪਸ !(iter == s); }
#ਜੇ_ਅੰਤ  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter ਅਧਾਰ() ਅਚਲ {
        // Return the logical position.
        ਵਾਪਸ state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE ਕੋਡ_Units_2<CP32, UnitIter> ਸੰਚਾਲਕ*() ਅਚਲ {
        ਜੇ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, ਹੱਦਬੰਦੀ_3);
            state_ = 1;
        }
        ਵਾਪਸ units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy ਸੰਚਾਲਕ->() ਅਚਲ {
        ਜੇ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, ਹੱਦਬੰਦੀ_3);
            state_ = 1;
        }
        ਵਾਪਸ Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator& ਸੰਚਾਲਕ++() U_LIFETIME_BOUND {  // pre-increment
        ਜੇ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } ਨਹੀਂਤਾਂ ਜੇ (state_ == 0) {
            Impl::inc(p_, ਹੱਦਬੰਦੀ_3);
        } ਨਹੀਂਤਾਂ /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.ਸਿਰਾ();
            state_ = 0;
        }
        ਵਾਪਸ *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator ਸੰਚਾਲਕ++(ਪੂਰਨ) {  // post-increment
        ਜੇ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UTFIterator ਨਤੀਜਾ(*this);
            state_ = 0;
            ਵਾਪਸ ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ ਜੇ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, ਹੱਦਬੰਦੀ_3);
            UTFIterator ਨਤੀਜਾ(*this);
            ਨਤੀਜਾ.state_ = 1;
            // keep this->state_ == 0
            ਵਾਪਸ ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ /* state_ < 0 */ {
            UTFIterator ਨਤੀਜਾ(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.ਸਿਰਾ();
            state_ = 0;
            ਵਾਪਸ ਨਤੀਜਾ;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    ਸਾਂਚਾ <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator&>
    ਸੰਚਾਲਕ--() U_LIFETIME_BOUND {  // pre-decrement
        ਜੇ (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(ਸ਼ੁਰੂ_3, p_);
        state_ = -1;
        ਵਾਪਸ *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator>
    ਸੰਚਾਲਕ--(ਪੂਰਨ) {  // post-decrement
        UTFIterator ਨਤੀਜਾ(*this);
        ਸੰਚਾਲਕ--();
        ਵਾਪਸ ਨਤੀਜਾ;
    }

ਨਿਜੀ:
    ਮਿੱਤਰ class std::ਉਲਟ_iterator<UTFIterator<CP32, behavior, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter ਸ਼ੁਰੂ_3;
    ਹੱਦਬੰਦੀ_Iter ਹੱਦਬੰਦੀ_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable ਕੋਡ_Units_2<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_ਵਿੱਚ_DOXYGEN
// Partial template specialization for single-pass input iterator.
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename ਹੱਦਬੰਦੀ_Iter>
class UTFIterator<
        CP32, behavior,
        UnitIter, ਹੱਦਬੰਦੀ_Iter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, ਹੱਦਬੰਦੀ_Iter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    ਜਨਤਕ:
        explicit Proxy(ਕੋਡ_Units_2<CP32, UnitIter> &units) : units_(units) {}
        ਕੋਡ_Units_2<CP32, UnitIter>& ਸੰਚਾਲਕ*() U_LIFETIME_BOUND { ਵਾਪਸ units_; }
        ਕੋਡ_Units_2<CP32, UnitIter>* ਸੰਚਾਲਕ->() U_LIFETIME_BOUND { ਵਾਪਸ &units_; }
    ਨਿਜੀ:
        ਕੋਡ_Units_2<CP32, UnitIter> units_;
    };

ਜਨਤਕ:
    using ਮੁੱਲ_type = ਕੋਡ_Units_2<CP32, UnitIter>;
    using reference = ਮੁੱਲ_type;
    using ਸੂਚਕ = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::ਨਿਵੇਸ਼_iterator_ਟੈਗ;

    U_FORCE_INLINE UTFIterator(UnitIter p, ਹੱਦਬੰਦੀ_Iter ਹੱਦਬੰਦੀ) : p_(std::move(p)), ਹੱਦਬੰਦੀ_3(std::move(ਹੱਦਬੰਦੀ)) {}

    // Constructs an iterator start or limit sentinel.
    // Requires p to be copyable.
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(std::move(p)), ਹੱਦਬੰਦੀ_3(p_) {}

    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = ਮੂਲ;
    U_FORCE_INLINE UTFIterator &ਸੰਚਾਲਕ=(UTFIterator &&src) noexcept = ਮੂਲ;

    U_FORCE_INLINE UTFIterator(ਅਚਲ UTFIterator &other) = ਮੂਲ;
    U_FORCE_INLINE UTFIterator &ਸੰਚਾਲਕ=(ਅਚਲ UTFIterator &other) = ਮੂਲ;

    U_FORCE_INLINE bool ਸੰਚਾਲਕ==(ਅਚਲ UTFIterator &other) ਅਚਲ {
        ਵਾਪਸ p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool ਸੰਚਾਲਕ!=(ਅਚਲ UTFIterator &other) ਅਚਲ { ਵਾਪਸ !ਸੰਚਾਲਕ==(other); }

    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ==(ਅਚਲ UTFIterator &iter, ਅਚਲ Sentinel &s) {
        ਵਾਪਸ !iter.ahead_ && iter.p_ == s;
    }

#ਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ < 20
    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ==(ਅਚਲ Sentinel &s, ਅਚਲ UTFIterator &iter) {
        ਵਾਪਸ !iter.ahead_ && iter.p_ == s;
    }

    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ!=(ਅਚਲ UTFIterator &iter, ਅਚਲ Sentinel &s) { ਵਾਪਸ !(iter == s); }

    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ!=(ਅਚਲ Sentinel &s, ਅਚਲ UTFIterator &iter) { ਵਾਪਸ !(iter == s); }
#ਜੇ_ਅੰਤ  // C++17

    U_FORCE_INLINE ਕੋਡ_Units_2<CP32, UnitIter> ਸੰਚਾਲਕ*() ਅਚਲ {
        ਜੇ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, ਹੱਦਬੰਦੀ_3);
            ahead_ = true;
        }
        ਵਾਪਸ units_;
    }

    U_FORCE_INLINE Proxy ਸੰਚਾਲਕ->() ਅਚਲ {
        ਜੇ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, ਹੱਦਬੰਦੀ_3);
            ahead_ = true;
        }
        ਵਾਪਸ Proxy(units_);
    }

    U_FORCE_INLINE UTFIterator& ਸੰਚਾਲਕ++() U_LIFETIME_BOUND {  // pre-increment
        ਜੇ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } ਨਹੀਂਤਾਂ {
            Impl::inc(p_, ਹੱਦਬੰਦੀ_3);
        }
        ਵਾਪਸ *this;
    }

    U_FORCE_INLINE Proxy ਸੰਚਾਲਕ++(ਪੂਰਨ) {  // post-increment
        ਜੇ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } ਨਹੀਂਤਾਂ {
            units_ = Impl::readAndInc(p_, p_, ਹੱਦਬੰਦੀ_3);
            // keep this->ahead_ == false
        }
        ਵਾਪਸ Proxy(units_);
    }

ਨਿਜੀ:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need limit_ so that when we read a code point
    // we can test if there are enough code units.
    ਹੱਦਬੰਦੀ_Iter ਹੱਦਬੰਦੀ_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable ਕੋਡ_Units_2<CP32, UnitIter> units_ = {0, 0, false};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#ਜੇ_ਅੰਤ  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_ਵਿੱਚ_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
class std::ਉਲਟ_iterator<U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>> {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::UTFImpl<CP32, behavior, UnitIter>;
    using ਕੋਡ_Units_3 = U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::ਕੋਡ_Units_2<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    ਜਨਤਕ:
        explicit Proxy(ਕੋਡ_Units_3 units) : units_(units) {}
        ਕੋਡ_Units_3& ਸੰਚਾਲਕ*() U_LIFETIME_BOUND { ਵਾਪਸ units_; }
        ਕੋਡ_Units_3* ਸੰਚਾਲਕ->() U_LIFETIME_BOUND { ਵਾਪਸ &units_; }
    ਨਿਜੀ:
        ਕੋਡ_Units_3 units_;
    };

ਜਨਤਕ:
    using ਮੁੱਲ_type = ਕੋਡ_Units_3;
    using reference = ਮੁੱਲ_type;
    using ਸੂਚਕ = Proxy;
    using difference_type = U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_ਟੈਗ;

    U_FORCE_INLINE explicit ਉਲਟ_iterator(U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> iter) :
            p_(iter.ਅਧਾਰ()), ਸ਼ੁਰੂ_3(iter.ਸ਼ੁਰੂ_3), ਹੱਦਬੰਦੀ_3(iter.ਹੱਦਬੰਦੀ_3),
            units_(0, 0, false, p_, p_) {}
    U_FORCE_INLINE ਉਲਟ_iterator() : p_{}, ਸ਼ੁਰੂ_3{}, ਹੱਦਬੰਦੀ_3{}, units_(0, 0, false, p_, p_) {}

    U_FORCE_INLINE ਉਲਟ_iterator(ਉਲਟ_iterator &&src) noexcept = ਮੂਲ;
    U_FORCE_INLINE ਉਲਟ_iterator &ਸੰਚਾਲਕ=(ਉਲਟ_iterator &&src) noexcept = ਮੂਲ;

    U_FORCE_INLINE ਉਲਟ_iterator(ਅਚਲ ਉਲਟ_iterator &other) = ਮੂਲ;
    U_FORCE_INLINE ਉਲਟ_iterator &ਸੰਚਾਲਕ=(ਅਚਲ ਉਲਟ_iterator &other) = ਮੂਲ;

    U_FORCE_INLINE bool ਸੰਚਾਲਕ==(ਅਚਲ ਉਲਟ_iterator &other) ਅਚਲ {
        ਵਾਪਸ getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool ਸੰਚਾਲਕ!=(ਅਚਲ ਉਲਟ_iterator &other) ਅਚਲ { ਵਾਪਸ !ਸੰਚਾਲਕ==(other); }

    U_FORCE_INLINE U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> ਅਧਾਰ() ਅਚਲ {
        ਵਾਪਸ U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>(
            ਸ਼ੁਰੂ_3, getLogicalPosition(), ਹੱਦਬੰਦੀ_3);
    }

    U_FORCE_INLINE ਕੋਡ_Units_3 ਸੰਚਾਲਕ*() ਅਚਲ {
        ਜੇ (state_ == 0) {
            units_ = Impl::decAndRead(ਸ਼ੁਰੂ_3, p_);
            state_ = -1;
        }
        ਵਾਪਸ units_;
    }

    U_FORCE_INLINE Proxy ਸੰਚਾਲਕ->() ਅਚਲ {
        ਜੇ (state_ == 0) {
            units_ = Impl::decAndRead(ਸ਼ੁਰੂ_3, p_);
            state_ = -1;
        }
        ਵਾਪਸ Proxy(units_);
    }

    U_FORCE_INLINE ਉਲਟ_iterator& ਸੰਚਾਲਕ++() U_LIFETIME_BOUND {  // pre-increment
        ਜੇ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } ਨਹੀਂਤਾਂ ਜੇ (state_ == 0) {
            Impl::dec(ਸ਼ੁਰੂ_3, p_);
        } ਨਹੀਂਤਾਂ /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        ਵਾਪਸ *this;
    }

    U_FORCE_INLINE ਉਲਟ_iterator ਸੰਚਾਲਕ++(ਪੂਰਨ) {  // post-increment
        ਜੇ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            ਉਲਟ_iterator ਨਤੀਜਾ(*this);
            state_ = 0;
            ਵਾਪਸ ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ ਜੇ (state_ == 0) {
            units_ = Impl::decAndRead(ਸ਼ੁਰੂ_3, p_);
            ਉਲਟ_iterator ਨਤੀਜਾ(*this);
            ਨਤੀਜਾ.state_ = -1;
            // keep this->state_ == 0
            ਵਾਪਸ ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ /* state_ > 0 */ {
            ਉਲਟ_iterator ਨਤੀਜਾ(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            ਵਾਪਸ ਨਤੀਜਾ;
        }
    }

    U_FORCE_INLINE ਉਲਟ_iterator& ਸੰਚਾਲਕ--() U_LIFETIME_BOUND {  // pre-decrement
        ਜੇ (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.ਸਿਰਾ();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_, ਹੱਦਬੰਦੀ_3);
        state_ = 1;
        ਵਾਪਸ *this;
    }

    U_FORCE_INLINE ਉਲਟ_iterator ਸੰਚਾਲਕ--(ਪੂਰਨ) {  // post-decrement
        ਉਲਟ_iterator ਨਤੀਜਾ(*this);
        ਸੰਚਾਲਕ--();
        ਵਾਪਸ ਨਤੀਜਾ;
    }

ਨਿਜੀ:
    U_FORCE_INLINE UnitIter getLogicalPosition() ਅਚਲ {
        ਵਾਪਸ state_ >= 0 ? p_ : units_.ਸਿਰਾ();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter ਸ਼ੁਰੂ_3;
    UnitIter ਹੱਦਬੰਦੀ_3;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable ਕੋਡ_Units_3 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#ਜੇ_ਅੰਤ  // U_IN_DOXYGEN

namespace U_ਸਿਰਲੇਖ_ONLY_NAMESPACE {

/**
 * UTFIterator factory function for start <= p < limit.
 * Deduces the UnitIter and LimitIter template parameters from the inputs.
 * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t
 * @tparam behavior How to handle ill-formed Unicode strings
 * @tparam UnitIter Can usually be omitted/deduced:
 *     An iterator (often a pointer) that returns a code unit type:
 *     UTF-8: char or char8_t or uint8_t;
 *     UTF-16: char16_t or uint16_t or (on Windows) wchar_t;
 *     UTF-32: char32_t or UChar32=int32_t or (on Linux) wchar_t
 * @tparam LimitIter Either the same as UnitIter, or an iterator sentinel type.
 * @param start start code unit iterator
 * @param p current-position code unit iterator
 * @param limit limit (exclusive-end) code unit iterator.
 *     When using a code unit sentinel (UnitIter≠LimitIter),
 *     then that sentinel also works as a sentinel for the code point iterator.
 * @return a UTFIterator&lt;CP32, behavior, UnitIter&gt;
 *     for the given code unit iterators or character pointers
 * @draft ICU 78
 */
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename ਹੱਦਬੰਦੀ_Iter = UnitIter>
ਸਵੈਚਾਲਿਤ utfIterator(UnitIter ਸ਼ੁਰੂ, UnitIter p, ਹੱਦਬੰਦੀ_Iter ਹੱਦਬੰਦੀ) {
    ਵਾਪਸ UTFIterator<CP32, behavior, UnitIter, ਹੱਦਬੰਦੀ_Iter>(
        std::move(ਸ਼ੁਰੂ), std::move(p), std::move(ਹੱਦਬੰਦੀ));
}

/**
 * UTFIterator factory function for start = p < limit.
 * Deduces the UnitIter and LimitIter template parameters from the inputs.
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t
 * @tparam behavior How to handle ill-formed Unicode strings
 * @tparam UnitIter Can usually be omitted/deduced:
 *     An iterator (often a pointer) that returns a code unit type:
 *     UTF-8: char or char8_t or uint8_t;
 *     UTF-16: char16_t or uint16_t or (on Windows) wchar_t;
 *     UTF-32: char32_t or UChar32=int32_t or (on Linux) wchar_t
 * @tparam LimitIter Either the same as UnitIter, or an iterator sentinel type.
 * @param p start and current-position code unit iterator
 * @param limit limit (exclusive-end) code unit iterator.
 *     When using a code unit sentinel (UnitIter≠LimitIter),
 *     then that sentinel also works as a sentinel for the code point iterator.
 * @return a UTFIterator&lt;CP32, behavior, UnitIter&gt;
 *     for the given code unit iterators or character pointers
 * @draft ICU 78
 */
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename ਹੱਦਬੰਦੀ_Iter = UnitIter>
ਸਵੈਚਾਲਿਤ utfIterator(UnitIter p, ਹੱਦਬੰਦੀ_Iter ਹੱਦਬੰਦੀ) {
    ਵਾਪਸ UTFIterator<CP32, behavior, UnitIter, ਹੱਦਬੰਦੀ_Iter>(
        std::move(p), std::move(ਹੱਦਬੰਦੀ));
}

// Note: We should only enable the following factory function for a copyable UnitIter.
// In C++17, we would have to partially specialize with enable_if_t testing for forward_iterator,
// but a function template partial specialization is not allowed.
// In C++20, we might be able to require the std::copyable concept.

/**
 * UTFIterator factory function for a start or limit sentinel.
 * Deduces the UnitIter template parameter from the input.
 * Requires UnitIter to be copyable.
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t
 * @tparam behavior How to handle ill-formed Unicode strings
 * @tparam UnitIter Can usually be omitted/deduced:
 *     An iterator (often a pointer) that returns a code unit type:
 *     UTF-8: char or char8_t or uint8_t;
 *     UTF-16: char16_t or uint16_t or (on Windows) wchar_t;
 *     UTF-32: char32_t or UChar32=int32_t or (on Linux) wchar_t
 * @param p code unit iterator.
 *     When using a code unit sentinel,
 *     then that sentinel also works as a sentinel for the code point iterator.
 * @return a UTFIterator&lt;CP32, behavior, UnitIter&gt;
 *     for the given code unit iterator or character pointer
 * @draft ICU 78
 */
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
ਸਵੈਚਾਲਿਤ utfIterator(UnitIter p) {
    ਵਾਪਸ UTFIterator<CP32, behavior, UnitIter>(std::move(p));
}

/**
 * A C++ "range" for validating iteration over all of the code points of a code unit range.
 *
 * Call utfStringCodePoints() to have the compiler deduce the Range type.
 *
 * UTFStringCodePoints is conditionally borrowed; that is, if Range is a borrowed range
 * so is UTFStringCodePoints<CP32, behavior, Range>.
 * Note that when given a range r that is an lvalue and is not a view,  utfStringCodePoints(r) uses a
 * ref_view of r as the Range type, which is a borrowed range.
 * In practice, this means that given a container variable r, the iterators of utfStringCodePoints(r) can
 * be used as long as iterators on r are valid, without having to keep utfStringCodePoints(r) around.
 * For instance:
 * \code
 *     std::u8string s = "𒇧𒇧";
 *     // it outlives utfStringCodePoints<char32_t>(s).
 *     auto it = utfStringCodePoints<char32_t>(s).begin();
 *     ++it;
 *     char32_t second_code_point = it->codePoint();  // OK.
 * \endcode
 * 
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t;
 *              should be signed if UTF_BEHAVIOR_NEGATIVE
 * @tparam behavior How to handle ill-formed Unicode strings
 * @tparam Range A C++ "range" of Unicode UTF-8/16/32 code units
 * @draft ICU 78
 * @see utfStringCodePoints
 */
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior, typename Range>
#ਜੇ defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#ਜੇ_ਅੰਤ
class UTFString_ਕੋਡ_Points {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ਜਨਤਕ:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    UTFString_ਕੋਡ_Points() = ਮੂਲ;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit UTFString_ਕੋਡ_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C+20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = ਖਾਲੀ>
    explicit UTFString_ਕੋਡ_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    UTFString_ਕੋਡ_Points(ਅਚਲ UTFString_ਕੋਡ_Points &other) = ਮੂਲ;

    /** Copy assignment operator. @draft ICU 78 */
    UTFString_ਕੋਡ_Points &ਸੰਚਾਲਕ=(ਅਚਲ UTFString_ਕੋਡ_Points &other) = ਮੂਲ;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ begin() {
        ਵਾਪਸ utfIterator<CP32, behavior>(unitRange.begin(), unitRange.ਸਿਰਾ());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename R = Range, typename = std::enable_if_t<prv::range<ਅਚਲ R>>>
    ਸਵੈਚਾਲਿਤ begin() ਅਚਲ {
        ਵਾਪਸ utfIterator<CP32, behavior>(unitRange.begin(), unitRange.ਸਿਰਾ());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ ਸਿਰਾ() {
        using UnitIter = decltype(unitRange.begin());
        using ਹੱਦਬੰਦੀ_Iter = decltype(unitRange.ਸਿਰਾ());
        ਜੇ constexpr (!std::is_same_v<UnitIter, ਹੱਦਬੰਦੀ_Iter>) {
            // Return the code unit sentinel.
            ਵਾਪਸ unitRange.ਸਿਰਾ();
        } ਨਹੀਂਤਾਂ ਜੇ constexpr (prv::bidirectional_iterator<UnitIter>) {
            ਵਾਪਸ utfIterator<CP32, behavior>(unitRange.begin(), unitRange.ਸਿਰਾ(), unitRange.ਸਿਰਾ());
        } ਨਹੀਂਤਾਂ {
            // The input iterator specialization has no three-argument constructor.
            ਵਾਪਸ utfIterator<CP32, behavior>(unitRange.ਸਿਰਾ(), unitRange.ਸਿਰਾ());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename R = Range, typename = std::enable_if_t<prv::range<ਅਚਲ R>>>
    ਸਵੈਚਾਲਿਤ ਸਿਰਾ() ਅਚਲ {
        using UnitIter = decltype(unitRange.begin());
        using ਹੱਦਬੰਦੀ_Iter = decltype(unitRange.ਸਿਰਾ());
        ਜੇ constexpr (!std::is_same_v<UnitIter, ਹੱਦਬੰਦੀ_Iter>) {
            // Return the code unit sentinel.
            ਵਾਪਸ unitRange.ਸਿਰਾ();
        } ਨਹੀਂਤਾਂ ਜੇ constexpr (prv::bidirectional_iterator<UnitIter>) {
            ਵਾਪਸ utfIterator<CP32, behavior>(unitRange.begin(), unitRange.ਸਿਰਾ(), unitRange.ਸਿਰਾ());
        } ਨਹੀਂਤਾਂ {
            // The input iterator specialization has no three-argument constructor.
            ਵਾਪਸ utfIterator<CP32, behavior>(unitRange.ਸਿਰਾ(), unitRange.ਸਿਰਾ());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ rbegin() ਅਚਲ {
        ਵਾਪਸ std::make_ਉਲਟ_iterator(ਸਿਰਾ());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ rend() ਅਚਲ {
        ਵਾਪਸ std::make_ਉਲਟ_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    ਸਵੈਚਾਲਿਤ front() ਅਚਲ {
        ਵਾਪਸ *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    ਸਵੈਚਾਲਿਤ ਪਿੱਠ() ਅਚਲ {
        ਵਾਪਸ *(--ਸਿਰਾ());
    }

ਨਿਜੀ:
    Range unitRange;
};

/** @internal */
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior>
ਬਣਤਰ UTFString_ਕੋਡ_Points_Adaptor
#ਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_ਘੇਰਾ<UTFString_ਕੋਡ_Points_Adaptor<CP32, behavior>>
#ਜੇ_ਅੰਤ
{
    /** @internal */
    ਸਾਂਚਾ<typename Range>
    ਸਵੈਚਾਲਿਤ ਸੰਚਾਲਕ()(Range &&unitRange) ਅਚਲ {
#ਜੇ defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        ਵਾਪਸ UTFString_ਕੋਡ_Points<CP32, behavior, std::ranges::views::all_t<Range>>(
            std::forward<Range>(unitRange));
#ਨਹੀਂਤਾਂ
        ਜੇ constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            ਵਾਪਸ UTFString_ਕੋਡ_Points<CP32, behavior, std::decay_t<Range>>(
                std::forward<Range>(unitRange));
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ UTFString_ਕੋਡ_Points<CP32, behavior, Range>(std::forward<Range>(unitRange));
        }
#ਜੇ_ਅੰਤ
    }
};

/**
 * Range adaptor function object returning a UTFStringCodePoints object that represents a "range" of code
 * points in a code unit range, which validates while decoding.
 * Deduces the Range template parameter from the input, taking into account the value category: the
 * code units will be referenced if possible, and moved if necessary.
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t;
 *              should be signed if UTF_BEHAVIOR_NEGATIVE
 * @tparam behavior How to handle ill-formed Unicode strings
 * @tparam Range A C++ "range" of Unicode UTF-8/16/32 code units
 * @param unitRange input range
 * @return a UTFStringCodePoints&lt;CP32, behavior, Range&gt; for the given unitRange
 * @draft ICU 78
 */
ਸਾਂਚਾ<typename CP32, UTFIllFormedBehavior behavior>
constexpr UTFString_ਕੋਡ_Points_Adaptor<CP32, behavior> utf_String_ਕੋਡ_Points;

// Non-validating iterators ------------------------------------------------ ***

/**
 * Non-validating iterator over the code points in a Unicode string.
 * The string must be well-formed.
 *
 * The UnitIter can be
 * an input_iterator, a forward_iterator, or a bidirectional_iterator (including a pointer).
 * The UTFIterator will have the corresponding iterator_category.
 *
 * Call unsafeUTFIterator() to have the compiler deduce the UnitIter type.
 *
 * For reverse iteration, either use this iterator directly as in <code>*--iter</code>
 * or wrap it using std::make_reverse_iterator(iter).
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t
 * @tparam UnitIter An iterator (often a pointer) that returns a code unit type:
 *     UTF-8: char or char8_t or uint8_t;
 *     UTF-16: char16_t or uint16_t or (on Windows) wchar_t;
 *     UTF-32: char32_t or UChar32=int32_t or (on Linux) wchar_t
 * @draft ICU 78
 * @see unsafeUTFIterator
 */
ਸਾਂਚਾ<typename CP32, typename UnitIter, typename = ਖਾਲੀ>
class UnsafeUTFIterator {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    ਜਨਤਕ:
        explicit Proxy(Unsafe_ਕੋਡ_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_ਕੋਡ_Units<CP32, UnitIter>& ਸੰਚਾਲਕ*() U_LIFETIME_BOUND { ਵਾਪਸ units_; }
        Unsafe_ਕੋਡ_Units<CP32, UnitIter>* ਸੰਚਾਲਕ->() U_LIFETIME_BOUND { ਵਾਪਸ &units_; }
    ਨਿਜੀ:
        Unsafe_ਕੋਡ_Units<CP32, UnitIter> units_;
    };

ਜਨਤਕ:
    /** C++ iterator boilerplate @internal */
    using ਮੁੱਲ_type = Unsafe_ਕੋਡ_Units<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = ਮੁੱਲ_type;
    /** C++ iterator boilerplate @internal */
    using ਸੂਚਕ = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_ਟੈਗ,
        std::forward_iterator_ਟੈਗ>;

    /**
     * Constructor; the iterator/pointer should be at a code point boundary.
     *
     * When using a code unit sentinel,
     * then that sentinel also works as a sentinel for this code point iterator.
     *
     * @param p Initial position inside the range, or a range sentinel
     * @draft ICU 78
     */
    U_FORCE_INLINE explicit UnsafeUTFIterator(UnitIter p) : p_(p), units_(0, 0, p, p) {}
    /**
     * Default constructor. Makes a non-functional iterator.
     *
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator() : p_{}, units_(0, 0, p_, p_) {}

    /** Move constructor. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = ਮੂਲ;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &ਸੰਚਾਲਕ=(UnsafeUTFIterator &&src) noexcept = ਮੂਲ;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator(ਅਚਲ UnsafeUTFIterator &other) = ਮੂਲ;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &ਸੰਚਾਲਕ=(ਅਚਲ UnsafeUTFIterator &other) = ਮੂਲ;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool ਸੰਚਾਲਕ==(ਅਚਲ UnsafeUTFIterator &other) ਅਚਲ {
        ਵਾਪਸ ਅਧਾਰ() == other.ਅਧਾਰ();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool ਸੰਚਾਲਕ!=(ਅਚਲ UnsafeUTFIterator &other) ਅਚਲ { ਵਾਪਸ !ਸੰਚਾਲਕ==(other); }

    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ==(ਅਚਲ UnsafeUTFIterator &iter, ਅਚਲ Sentinel &s) {
        ਵਾਪਸ iter.ਅਧਾਰ() == s;
    }

#ਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ < 20
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ==(ਅਚਲ Sentinel &s, ਅਚਲ UnsafeUTFIterator &iter) {
        ਵਾਪਸ iter.ਅਧਾਰ() == s;
    }
    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ!=(ਅਚਲ UnsafeUTFIterator &iter, ਅਚਲ Sentinel &s) { ਵਾਪਸ !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ!=(ਅਚਲ Sentinel &s, ਅਚਲ UnsafeUTFIterator &iter) { ਵਾਪਸ !(iter == s); }
#ਜੇ_ਅੰਤ  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter ਅਧਾਰ() ਅਚਲ {
        // Return the logical position.
        ਵਾਪਸ state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE Unsafe_ਕੋਡ_Units<CP32, UnitIter> ਸੰਚਾਲਕ*() ਅਚਲ {
        ਜੇ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        ਵਾਪਸ units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy ਸੰਚਾਲਕ->() ਅਚਲ {
        ਜੇ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        ਵਾਪਸ Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator& ਸੰਚਾਲਕ++() U_LIFETIME_BOUND {  // pre-increment
        ਜੇ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } ਨਹੀਂਤਾਂ ਜੇ (state_ == 0) {
            Impl::inc(p_);
        } ਨਹੀਂਤਾਂ /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.ਸਿਰਾ();
            state_ = 0;
        }
        ਵਾਪਸ *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator ਸੰਚਾਲਕ++(ਪੂਰਨ) {  // post-increment
        ਜੇ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UnsafeUTFIterator ਨਤੀਜਾ(*this);
            state_ = 0;
            ਵਾਪਸ ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ ਜੇ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            UnsafeUTFIterator ਨਤੀਜਾ(*this);
            ਨਤੀਜਾ.state_ = 1;
            // keep this->state_ == 0
            ਵਾਪਸ ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ /* state_ < 0 */ {
            UnsafeUTFIterator ਨਤੀਜਾ(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.ਸਿਰਾ();
            state_ = 0;
            ਵਾਪਸ ਨਤੀਜਾ;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    ਸਾਂਚਾ <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator&>
    ਸੰਚਾਲਕ--() U_LIFETIME_BOUND {  // pre-decrement
        ਜੇ (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(p_);
        state_ = -1;
        ਵਾਪਸ *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator>
    ਸੰਚਾਲਕ--(ਪੂਰਨ) {  // post-decrement
        UnsafeUTFIterator ਨਤੀਜਾ(*this);
        ਸੰਚਾਲਕ--();
        ਵਾਪਸ ਨਤੀਜਾ;
    }

ਨਿਜੀ:
    ਮਿੱਤਰ class std::ਉਲਟ_iterator<UnsafeUTFIterator<CP32, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_ਕੋਡ_Units<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_ਵਿੱਚ_DOXYGEN
// Partial template specialization for single-pass input iterator.
ਸਾਂਚਾ<typename CP32, typename UnitIter>
class UnsafeUTFIterator<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    ਜਨਤਕ:
        explicit Proxy(Unsafe_ਕੋਡ_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_ਕੋਡ_Units<CP32, UnitIter>& ਸੰਚਾਲਕ*() U_LIFETIME_BOUND { ਵਾਪਸ units_; }
        Unsafe_ਕੋਡ_Units<CP32, UnitIter>* ਸੰਚਾਲਕ->() U_LIFETIME_BOUND { ਵਾਪਸ &units_; }
    ਨਿਜੀ:
        Unsafe_ਕੋਡ_Units<CP32, UnitIter> units_;
    };

ਜਨਤਕ:
    using ਮੁੱਲ_type = Unsafe_ਕੋਡ_Units<CP32, UnitIter>;
    using reference = ਮੁੱਲ_type;
    using ਸੂਚਕ = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::ਨਿਵੇਸ਼_iterator_ਟੈਗ;

    U_FORCE_INLINE explicit UnsafeUTFIterator(UnitIter p) : p_(std::move(p)) {}

    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = ਮੂਲ;
    U_FORCE_INLINE UnsafeUTFIterator &ਸੰਚਾਲਕ=(UnsafeUTFIterator &&src) noexcept = ਮੂਲ;

    U_FORCE_INLINE UnsafeUTFIterator(ਅਚਲ UnsafeUTFIterator &other) = ਮੂਲ;
    U_FORCE_INLINE UnsafeUTFIterator &ਸੰਚਾਲਕ=(ਅਚਲ UnsafeUTFIterator &other) = ਮੂਲ;

    U_FORCE_INLINE bool ਸੰਚਾਲਕ==(ਅਚਲ UnsafeUTFIterator &other) ਅਚਲ {
        ਵਾਪਸ p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool ਸੰਚਾਲਕ!=(ਅਚਲ UnsafeUTFIterator &other) ਅਚਲ { ਵਾਪਸ !ਸੰਚਾਲਕ==(other); }

    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ==(ਅਚਲ UnsafeUTFIterator &iter, ਅਚਲ Sentinel &s) {
        ਵਾਪਸ !iter.ahead_ && iter.p_ == s;
    }

#ਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ < 20
    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ==(ਅਚਲ Sentinel &s, ਅਚਲ UnsafeUTFIterator &iter) {
        ਵਾਪਸ !iter.ahead_ && iter.p_ == s;
    }

    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ!=(ਅਚਲ UnsafeUTFIterator &iter, ਅਚਲ Sentinel &s) { ਵਾਪਸ !(iter == s); }

    ਸਾਂਚਾ<typename Sentinel> U_FORCE_INLINE ਮਿੱਤਰ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ਸੰਚਾਲਕ!=(ਅਚਲ Sentinel &s, ਅਚਲ UnsafeUTFIterator &iter) { ਵਾਪਸ !(iter == s); }
#ਜੇ_ਅੰਤ  // C++17

    U_FORCE_INLINE Unsafe_ਕੋਡ_Units<CP32, UnitIter> ਸੰਚਾਲਕ*() ਅਚਲ {
        ਜੇ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        ਵਾਪਸ units_;
    }

    U_FORCE_INLINE Proxy ਸੰਚਾਲਕ->() ਅਚਲ {
        ਜੇ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        ਵਾਪਸ Proxy(units_);
    }

    U_FORCE_INLINE UnsafeUTFIterator& ਸੰਚਾਲਕ++() U_LIFETIME_BOUND {  // pre-increment
        ਜੇ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } ਨਹੀਂਤਾਂ {
            Impl::inc(p_);
        }
        ਵਾਪਸ *this;
    }

    U_FORCE_INLINE Proxy ਸੰਚਾਲਕ++(ਪੂਰਨ) {  // post-increment
        ਜੇ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } ਨਹੀਂਤਾਂ {
            units_ = Impl::readAndInc(p_, p_);
            // keep this->ahead_ == false
        }
        ਵਾਪਸ Proxy(units_);
    }

ਨਿਜੀ:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable Unsafe_ਕੋਡ_Units<CP32, UnitIter> units_ = {0, 0};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#ਜੇ_ਅੰਤ  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_ਵਿੱਚ_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
ਸਾਂਚਾ<typename CP32, typename UnitIter>
class std::ਉਲਟ_iterator<U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>> {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::UnsafeUTFImpl<CP32, UnitIter>;
    using Unsafe_ਕੋਡ_Units_2 = U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::Unsafe_ਕੋਡ_Units<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    ਜਨਤਕ:
        explicit Proxy(Unsafe_ਕੋਡ_Units_2 units) : units_(units) {}
        Unsafe_ਕੋਡ_Units_2& ਸੰਚਾਲਕ*() U_LIFETIME_BOUND { ਵਾਪਸ units_; }
        Unsafe_ਕੋਡ_Units_2* ਸੰਚਾਲਕ->() U_LIFETIME_BOUND { ਵਾਪਸ &units_; }
    ਨਿਜੀ:
        Unsafe_ਕੋਡ_Units_2 units_;
    };

ਜਨਤਕ:
    using ਮੁੱਲ_type = Unsafe_ਕੋਡ_Units_2;
    using reference = ਮੁੱਲ_type;
    using ਸੂਚਕ = Proxy;
    using difference_type = U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_ਟੈਗ;

    U_FORCE_INLINE explicit ਉਲਟ_iterator(U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> iter) :
            p_(iter.ਅਧਾਰ()), units_(0, 0, p_, p_) {}
    U_FORCE_INLINE ਉਲਟ_iterator() : p_{}, units_(0, 0, p_, p_) {}

    U_FORCE_INLINE ਉਲਟ_iterator(ਉਲਟ_iterator &&src) noexcept = ਮੂਲ;
    U_FORCE_INLINE ਉਲਟ_iterator &ਸੰਚਾਲਕ=(ਉਲਟ_iterator &&src) noexcept = ਮੂਲ;

    U_FORCE_INLINE ਉਲਟ_iterator(ਅਚਲ ਉਲਟ_iterator &other) = ਮੂਲ;
    U_FORCE_INLINE ਉਲਟ_iterator &ਸੰਚਾਲਕ=(ਅਚਲ ਉਲਟ_iterator &other) = ਮੂਲ;

    U_FORCE_INLINE bool ਸੰਚਾਲਕ==(ਅਚਲ ਉਲਟ_iterator &other) ਅਚਲ {
        ਵਾਪਸ getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool ਸੰਚਾਲਕ!=(ਅਚਲ ਉਲਟ_iterator &other) ਅਚਲ { ਵਾਪਸ !ਸੰਚਾਲਕ==(other); }

    U_FORCE_INLINE U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> ਅਧਾਰ() ਅਚਲ {
        ਵਾਪਸ U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>(
            getLogicalPosition());
    }

    U_FORCE_INLINE Unsafe_ਕੋਡ_Units_2 ਸੰਚਾਲਕ*() ਅਚਲ {
        ਜੇ (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        ਵਾਪਸ units_;
    }

    U_FORCE_INLINE Proxy ਸੰਚਾਲਕ->() ਅਚਲ {
        ਜੇ (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        ਵਾਪਸ Proxy(units_);
    }

    U_FORCE_INLINE ਉਲਟ_iterator& ਸੰਚਾਲਕ++() U_LIFETIME_BOUND {  // pre-increment
        ਜੇ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } ਨਹੀਂਤਾਂ ਜੇ (state_ == 0) {
            Impl::dec(p_);
        } ਨਹੀਂਤਾਂ /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        ਵਾਪਸ *this;
    }

    U_FORCE_INLINE ਉਲਟ_iterator ਸੰਚਾਲਕ++(ਪੂਰਨ) {  // post-increment
        ਜੇ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            ਉਲਟ_iterator ਨਤੀਜਾ(*this);
            state_ = 0;
            ਵਾਪਸ ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ ਜੇ (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            ਉਲਟ_iterator ਨਤੀਜਾ(*this);
            ਨਤੀਜਾ.state_ = -1;
            // keep this->state_ == 0
            ਵਾਪਸ ਨਤੀਜਾ;
        } ਨਹੀਂਤਾਂ /* state_ > 0 */ {
            ਉਲਟ_iterator ਨਤੀਜਾ(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            ਵਾਪਸ ਨਤੀਜਾ;
        }
    }

    U_FORCE_INLINE ਉਲਟ_iterator& ਸੰਚਾਲਕ--() U_LIFETIME_BOUND {  // pre-decrement
        ਜੇ (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.ਸਿਰਾ();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_);
        state_ = 1;
        ਵਾਪਸ *this;
    }

    U_FORCE_INLINE ਉਲਟ_iterator ਸੰਚਾਲਕ--(ਪੂਰਨ) {  // post-decrement
        ਉਲਟ_iterator ਨਤੀਜਾ(*this);
        ਸੰਚਾਲਕ--();
        ਵਾਪਸ ਨਤੀਜਾ;
    }

ਨਿਜੀ:
    U_FORCE_INLINE UnitIter getLogicalPosition() ਅਚਲ {
        ਵਾਪਸ state_ >= 0 ? p_ : units_.ਸਿਰਾ();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_ਕੋਡ_Units_2 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#ਜੇ_ਅੰਤ  // U_IN_DOXYGEN

namespace U_ਸਿਰਲੇਖ_ONLY_NAMESPACE {

/**
 * UnsafeUTFIterator factory function.
 * Deduces the UnitIter template parameter from the input.
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t
 * @tparam UnitIter Can usually be omitted/deduced:
 *     An iterator (often a pointer) that returns a code unit type:
 *     UTF-8: char or char8_t or uint8_t;
 *     UTF-16: char16_t or uint16_t or (on Windows) wchar_t;
 *     UTF-32: char32_t or UChar32=int32_t or (on Linux) wchar_t
 * @param iter code unit iterator
 * @return an UnsafeUTFIterator&lt;CP32, UnitIter&gt;
 *     for the given code unit iterator or character pointer
 * @draft ICU 78
 */
ਸਾਂਚਾ<typename CP32, typename UnitIter>
ਸਵੈਚਾਲਿਤ unsafeUTFIterator(UnitIter iter) {
    ਵਾਪਸ UnsafeUTFIterator<CP32, UnitIter>(std::move(iter));
}

/**
 * A C++ "range" for non-validating iteration over all of the code points of a code unit range.
 * The string must be well-formed.
 *
 * Call unsafeUTFStringCodePoints() to have the compiler deduce the Range type.
 *
 * UnsafeUTFStringCodePoints is conditionally borrowed; that is, if Range is a borrowed range
 * so is UnsafeUTFStringCodePoints<CP32, behavior, Range>.
 * Note that when given a range r that is an lvalue and is not a view,  unsafeUTFStringCodePoints(r) uses
 * a ref_view of r as the Range type, which is a borrowed range.
 * In practice, this means that given a container variable r, the iterators of
 * unsafeUTFStringCodePoints(r) can be used as long as iterators on r are valid, without having to keep
 * unsafeUTFStringCodePoints(r) around.
 * For instance:
 * \code
 *     std::u8string s = "𒇧𒇧";
 *     // it outlives unsafeUTFStringCodePoints<char32_t>(s).
 *     auto it = unsafeUTFStringCodePoints<char32_t>(s).begin();
 *     ++it;
 *     char32_t second_code_point = it->codePoint();  // OK.
 * \endcode
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t
 * @tparam Range A C++ "range" of Unicode UTF-8/16/32 code units
 * @draft ICU 78
 * @see unsafeUTFStringCodePoints
 */
ਸਾਂਚਾ<typename CP32, typename Range>
#ਜੇ defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#ਜੇ_ਅੰਤ
class Unsafe_UTFString_ਕੋਡ_Points {
    static_assert(ਆਕਾਰ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ਜਨਤਕ:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    Unsafe_UTFString_ਕੋਡ_Points() = ਮੂਲ;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit Unsafe_UTFString_ਕੋਡ_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C++20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = ਖਾਲੀ>
    explicit Unsafe_UTFString_ਕੋਡ_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_UTFString_ਕੋਡ_Points(ਅਚਲ Unsafe_UTFString_ਕੋਡ_Points &other) = ਮੂਲ;

    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_UTFString_ਕੋਡ_Points &ਸੰਚਾਲਕ=(ਅਚਲ Unsafe_UTFString_ਕੋਡ_Points &other) = ਮੂਲ;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ begin() {
        ਵਾਪਸ unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename R = Range, typename = std::enable_if_t<prv::range<ਅਚਲ R>>>
    ਸਵੈਚਾਲਿਤ begin() ਅਚਲ {
        ਵਾਪਸ unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ ਸਿਰਾ() {
        using UnitIter = decltype(unitRange.begin());
        using ਹੱਦਬੰਦੀ_Iter = decltype(unitRange.ਸਿਰਾ());
        ਜੇ constexpr (!std::is_same_v<UnitIter, ਹੱਦਬੰਦੀ_Iter>) {
            // Return the code unit sentinel.
            ਵਾਪਸ unitRange.ਸਿਰਾ();
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ unsafeUTFIterator<CP32>(unitRange.ਸਿਰਾ());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    ਸਾਂਚਾ<typename R = Range, typename = std::enable_if_t<prv::range<ਅਚਲ R>>>
    ਸਵੈਚਾਲਿਤ ਸਿਰਾ() ਅਚਲ {
        using UnitIter = decltype(unitRange.begin());
        using ਹੱਦਬੰਦੀ_Iter = decltype(unitRange.ਸਿਰਾ());
        ਜੇ constexpr (!std::is_same_v<UnitIter, ਹੱਦਬੰਦੀ_Iter>) {
            // Return the code unit sentinel.
            ਵਾਪਸ unitRange.ਸਿਰਾ();
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ unsafeUTFIterator<CP32>(unitRange.ਸਿਰਾ());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ rbegin() ਅਚਲ {
        ਵਾਪਸ std::make_ਉਲਟ_iterator(ਸਿਰਾ());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    ਸਵੈਚਾਲਿਤ rend() ਅਚਲ {
        ਵਾਪਸ std::make_ਉਲਟ_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    ਸਵੈਚਾਲਿਤ front() ਅਚਲ {
        ਵਾਪਸ *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    ਸਵੈਚਾਲਿਤ ਪਿੱਠ() ਅਚਲ {
        ਵਾਪਸ *(--ਸਿਰਾ());
    }

ਨਿਜੀ:
    Range unitRange;
};

/** @internal */
ਸਾਂਚਾ<typename CP32>
ਬਣਤਰ Unsafe_UTFString_ਕੋਡ_Points_Adaptor
#ਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_ਘੇਰਾ<Unsafe_UTFString_ਕੋਡ_Points_Adaptor<CP32>>
#ਜੇ_ਅੰਤ
{
    /** @internal */
    ਸਾਂਚਾ<typename Range>
    ਸਵੈਚਾਲਿਤ ਸੰਚਾਲਕ()(Range &&unitRange) ਅਚਲ {
#ਜੇ defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        ਵਾਪਸ Unsafe_UTFString_ਕੋਡ_Points<CP32, std::ranges::views::all_t<Range>>(std::forward<Range>(unitRange));
#ਨਹੀਂਤਾਂ
        ਜੇ constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            ਵਾਪਸ Unsafe_UTFString_ਕੋਡ_Points<CP32, std::decay_t<Range>>(std::forward<Range>(unitRange));
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ Unsafe_UTFString_ਕੋਡ_Points<CP32, Range>(std::forward<Range>(unitRange));
        }
#ਜੇ_ਅੰਤ
    }
};


/**
 * Range adaptor function object returning an UnsafeUTFStringCodePoints object that represents a
 * "range" of code points in a code unit range. The string must be well-formed.
 * Deduces the Range template parameter from the input, taking into account the value category: the
 * code units will be referenced if possible, and moved if necessary.
 *
 * @tparam CP32 Code point type: UChar32 (=int32_t) or char32_t or uint32_t
 * @tparam Range A C++ "range" of Unicode UTF-8/16/32 code units
 * @param unitRange input range
 * @return an UnsafeUTFStringCodePoints&lt;CP32, Range&gt; for the given unitRange
 * @draft ICU 78
 */
ਸਾਂਚਾ<typename CP32>
constexpr Unsafe_UTFString_ਕੋਡ_Points_Adaptor<CP32> unsafe_UTFString_ਕੋਡ_Points;

}  // namespace U_HEADER_ONLY_NAMESPACE


#ਜੇ defined(__cpp_lib_ranges)
ਸਾਂਚਾ <typename CP32, UTFIllFormedBehavior behavior, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::UTFString_ਕੋਡ_Points<CP32, behavior, Range>> =
    std::ranges::enable_borrowed_range<Range>;

ਸਾਂਚਾ <typename CP32, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_ਸਿਰਲੇਖ_ONLY_NAMESPACE::Unsafe_UTFString_ਕੋਡ_Points<CP32, Range>> =
    std::ranges::enable_borrowed_range<Range>;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ  // U_HIDE_DRAFT_API
#ਜੇ_ਅੰਤ  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#ਜੇ_ਅੰਤ  // __UTFITERATOR_H__
