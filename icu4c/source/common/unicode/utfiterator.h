// © 2024 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfiterator.h
// created: 2024aug12 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UTFITERATOR_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UTFITERATOR_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_ಶೀರ್ಷಿಕೆ_API || !defined(UTYPES_H)

#ಅನ್ತರ್ಭಾವ <iterator>
#ಆದರೆ __has_include(<ಆವೃತ್ತಿ>)
#ಅನ್ತರ್ಭಾವ <version>
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(__cpp_lib_ranges)
#ಅನ್ತರ್ಭಾವ <ranges>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <string>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <type_traits>
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/uversion.h"

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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_DRAFT_API

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UTFIllFormedBehavior {
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

namespace U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE {

namespace prv {
#ಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ >= 20

/** @internal */
ಮಾದರಿ<typename Iter>
using iter_ಮೌಲ್ಯ_t = typename std::iter_ಮೌಲ್ಯ_t<Iter>;

/** @internal */
ಮಾದರಿ<typename Iter>
using iter_difference_t = std::iter_difference_t<Iter>;

/** @internal */
ಮಾದರಿ<typename Iter>
constexpr bool forward_iterator = std::forward_iterator<Iter>;

/** @internal */
ಮಾದರಿ<typename Iter>
constexpr bool bidirectional_iterator = std::bidirectional_iterator<Iter>;

/** @internal */
ಮಾದರಿ<typename Range>
constexpr bool range = std::ranges::range<Range>;

#ಇಲ್ಲದಿದ್ದರೆ

/** @internal */
ಮಾದರಿ<typename Iter>
using iter_ಮೌಲ್ಯ_t = typename std::iterator_traits<Iter>::ಮೌಲ್ಯ_type;

/** @internal */
ಮಾದರಿ<typename Iter>
using iter_difference_t = typename std::iterator_traits<Iter>::difference_type;

/** @internal */
ಮಾದರಿ<typename Iter>
constexpr bool forward_iterator =
    std::is_ತಳ_ದ_v<
        std::forward_iterator_ಟ್ಯಾಗ್,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
ಮಾದರಿ<typename Iter>
constexpr bool bidirectional_iterator =
    std::is_ತಳ_ದ_v<
        std::bidirectional_iterator_ಟ್ಯಾಗ್,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
ಮಾದರಿ<typename Range, typename = ಶೂನ್ಯ>
ರಚನೆ range_type : std::false_type {};

/** @internal */
ಮಾದರಿ<typename Range>
ರಚನೆ range_type<
    Range,
    std::void_t<decltype(std::declval<Range>().begin()),
                decltype(std::declval<Range>().ಕೊನೆ())>> : std::true_type {};

/** @internal */
ಮಾದರಿ<typename Range>
constexpr bool range = range_type<Range>::ಮೌಲ್ಯ_2;

#ಆದರೆ_ಅಂತ್ಯ

/** @internal */
ಮಾದರಿ <typename T> ರಚನೆ is_basic_string_view : std::false_type {};

/** @internal */
ಮಾದರಿ <typename... ವಾದಗಳು>
ರಚನೆ is_basic_string_view<std::basic_string_view<ವಾದಗಳು...>> : std::true_type {};

/** @internal */
ಮಾದರಿ <typename T> constexpr bool is_basic_string_view_v = is_basic_string_view<T>::ಮೌಲ್ಯ_2;

/** @internal */
ಮಾದರಿ<typename CP32, bool skipSurrogates>
class ಸಂಕೇತ_Points_Iterator {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ಸಾರ್ವಜನಿಕ:
    /** C++ iterator boilerplate @internal */
    using ಮೌಲ್ಯ_type = CP32;
    /** C++ iterator boilerplate @internal */
    using reference = ಮೌಲ್ಯ_type;
    /** C++ iterator boilerplate @internal */
    using ಸೂಚಕ = CP32 *;
    /** C++ iterator boilerplate @internal */
    using difference_type = int32_t;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::forward_iterator_ಟ್ಯಾಗ್;

    /** @internal */
    ಸಾಲೊಳಗೆ ಸಂಕೇತ_Points_Iterator(CP32 c) : c_(c) {}
    /** @internal */
    ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಸಂಕೇತ_Points_Iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ c_ == other.c_; }
    /** @internal */
    ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ ಸಂಕೇತ_Points_Iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other); }
    /** @internal */
    ಸಾಲೊಳಗೆ CP32 ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ c_; }
    /** @internal */
    ಸಾಲೊಳಗೆ ಸಂಕೇತ_Points_Iterator& ಕ್ರಿಯಾಕಾರಕ++() U_LIFETIME_BOUND {  // pre-increment
        ++c_;
        ಆದರೆ (skipSurrogates && c_ == 0xd800) {
            c_ = 0xe000;
        }
        ಹಿಂತಿರುಗಿ *this;
    }
    /** @internal */
    ಸಾಲೊಳಗೆ ಸಂಕೇತ_Points_Iterator ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) {  // post-increment
        ಸಂಕೇತ_Points_Iterator ಫಲಿತಾಂಶ(*this);
        ++(*this);
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

ಖಾಸಗಿ:
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
ಮಾದರಿ<typename CP32>
class All_ಸಂಕೇತ_Points {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ಸಾರ್ವಜನಿಕ:
    /** Constructor. @draft ICU 78 */
    All_ಸಂಕೇತ_Points() {}
    /**
     * @return an iterator over all Unicode code points.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ prv::ಸಂಕೇತ_Points_Iterator<CP32, false>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode code points.
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ prv::ಸಂಕೇತ_Points_Iterator<CP32, false>(0x110000); }
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
ಮಾದರಿ<typename CP32>
class All_Scalar_ಮೌಲ್ಯಗಳು {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ಸಾರ್ವಜನಿಕ:
    /** Constructor. @draft ICU 78 */
    All_Scalar_ಮೌಲ್ಯಗಳು() {}
    /**
     * @return an iterator over all Unicode scalar values.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ prv::ಸಂಕೇತ_Points_Iterator<CP32, true>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode scalar values.
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ prv::ಸಂಕೇತ_Points_Iterator<CP32, true>(0x110000); }
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
ಮಾದರಿ<typename CP32, typename UnitIter, typename = ಶೂನ್ಯ>
class Unsafe_ಸಂಕೇತ_Units {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Unit = typename prv::iter_ಮೌಲ್ಯ_t<UnitIter>;
ಸಾರ್ವಜನಿಕ:
    /** @internal */
    Unsafe_ಸಂಕೇತ_Units(CP32 ಸಂಕೇತ_ಬಿಂದು, uint8_t ಉದ್ದಳತೆ, UnitIter ಆರಂಭ, UnitIter ಎಲ್ಲೆ) :
            c_(ಸಂಕೇತ_ಬಿಂದು), len_(ಉದ್ದಳತೆ), ಆರಂಭ_3(ಆರಂಭ), ಎಲ್ಲೆ_3(ಎಲ್ಲೆ) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_ಸಂಕೇತ_Units(ಸ್ಥಿರಾಂಕ Unsafe_ಸಂಕೇತ_Units &other) = ಪೂರ್ವನಿಯೋಜಿತ;
    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_ಸಂಕೇತ_Units &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Unsafe_ಸಂಕೇತ_Units &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    /**
     * @return the Unicode code point decoded from the code unit sequence.
     *     If the sequence is ill-formed and the iterator validates,
     *     then this is a replacement value according to the iterator‘s
     *     UTFIllFormedBehavior template parameter.
     * @draft ICU 78
     */
    CP32 ಸಂಕೇತ_ಬಿಂದು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ c_; }

    /**
     * @return the start of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಆರಂಭ_3; }

    /**
     * @return the limit (exclusive end) of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಎಲ್ಲೆ_3; }

    /**
     * @return the length of the code unit sequence for one code point.
     * @draft ICU 78
     */
    uint8_t ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ len_; }

#ಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ >= 20
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    ಮಾದರಿ<std::contiguous_iterator Iter = UnitIter>
    std::basic_string_view<Unit> stringView() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ std::basic_string_view<Unit>(begin(), ಕೊನೆ());
    }
#ಇಲ್ಲದಿದ್ದರೆ
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    ಮಾದರಿ<typename Iter = UnitIter, typename Unit = typename std::iterator_traits<Iter>::ಮೌಲ್ಯ_type>
    std::enable_if_t<std::is_ಸೂಚಕ_v<Iter> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::const_iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::const_iterator>,
                     std::basic_string_view<Unit>>
    stringView() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ std::basic_string_view<Unit>(&*ಆರಂಭ_3, len_);
    }
#ಆದರೆ_ಅಂತ್ಯ

ಖಾಸಗಿ:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
    UnitIter ಆರಂಭ_3;
    UnitIter ಎಲ್ಲೆ_3;
};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
ಮಾದರಿ<typename CP32, typename UnitIter>
class Unsafe_ಸಂಕೇತ_Units<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ಸಾರ್ವಜನಿಕ:
    Unsafe_ಸಂಕೇತ_Units(CP32 ಸಂಕೇತ_ಬಿಂದು, uint8_t ಉದ್ದಳತೆ) : c_(ಸಂಕೇತ_ಬಿಂದು), len_(ಉದ್ದಳತೆ) {}

    Unsafe_ಸಂಕೇತ_Units(ಸ್ಥಿರಾಂಕ Unsafe_ಸಂಕೇತ_Units &other) = ಪೂರ್ವನಿಯೋಜಿತ;
    Unsafe_ಸಂಕೇತ_Units &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Unsafe_ಸಂಕೇತ_Units &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    CP32 ಸಂಕೇತ_ಬಿಂದು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ c_; }

    uint8_t ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ len_; }

ಖಾಸಗಿ:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
};
#ಆದರೆ_ಅಂತ್ಯ  // U_IN_DOXYGEN

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
ಮಾದರಿ<typename CP32, typename UnitIter, typename = ಶೂನ್ಯ>
class ಸಂಕೇತ_Units_2 : ಸಾರ್ವಜನಿಕ Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> {
ಸಾರ್ವಜನಿಕ:
    /** @internal */
    ಸಂಕೇತ_Units_2(CP32 ಸಂಕೇತ_ಬಿಂದು, uint8_t ಉದ್ದಳತೆ, bool wellFormed, UnitIter ಆರಂಭ, UnitIter ಎಲ್ಲೆ) :
            Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter>(ಸಂಕೇತ_ಬಿಂದು, ಉದ್ದಳತೆ, ಆರಂಭ, ಎಲ್ಲೆ), ok_(wellFormed) {}

    /** Copy constructor. @draft ICU 78 */
    ಸಂಕೇತ_Units_2(ಸ್ಥಿರಾಂಕ ಸಂಕೇತ_Units_2 &other) = ಪೂರ್ವನಿಯೋಜಿತ;
    /** Copy assignment operator. @draft ICU 78 */
    ಸಂಕೇತ_Units_2 &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಸಂಕೇತ_Units_2 &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    /**
     * @return true if the decoded code unit sequence is well-formed.
     * @draft ICU 78
     */
    bool wellFormed() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ok_; }

ಖಾಸಗಿ:
    bool ok_;
};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
ಮಾದರಿ<typename CP32, typename UnitIter>
class ಸಂಕೇತ_Units_2<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> :
            ಸಾರ್ವಜನಿಕ Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> {
ಸಾರ್ವಜನಿಕ:
    ಸಂಕೇತ_Units_2(CP32 ಸಂಕೇತ_ಬಿಂದು, uint8_t ಉದ್ದಳತೆ, bool wellFormed) :
            Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter>(ಸಂಕೇತ_ಬಿಂದು, ಉದ್ದಳತೆ), ok_(wellFormed) {}

    ಸಂಕೇತ_Units_2(ಸ್ಥಿರಾಂಕ ಸಂಕೇತ_Units_2 &other) = ಪೂರ್ವನಿಯೋಜಿತ;
    ಸಂಕೇತ_Units_2 &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಸಂಕೇತ_Units_2 &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    bool wellFormed() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ok_; }

ಖಾಸಗಿ:
    bool ok_;
};
#ಆದರೆ_ಅಂತ್ಯ  // U_IN_DOXYGEN

// Validating implementations ---------------------------------------------- ***

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename ಎಲ್ಲೆ_Iter = UnitIter, typename = ಶೂನ್ಯ>
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
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename ಎಲ್ಲೆ_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, ಎಲ್ಲೆ_Iter,
        std::enable_if_t<ಗಾತ್ರ(typename prv::iter_ಮೌಲ್ಯ_t<UnitIter>) == 1>> {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    static_assert(behavior != UTF_BEHAVIOR_SURROGATE,
                  "For 8-bit strings, the SURROGATE option does not have an equivalent.");
ಸಾರ್ವಜನಿಕ:
    // Handle ill-formed UTF-8
    U_FORCE_INLINE ಸ್ಥಿರ CP32 sub() {
        ಆದರೆ constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            ಹಿಂತಿರುಗಿ U_SENTINEL;
        } ಇಲ್ಲದಿದ್ದರೆ {
            static_assert(behavior == UTF_BEHAVIOR_FFFD);
            ಹಿಂತಿರುಗಿ 0xfffd;
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ inc(UnitIter &p, ಸ್ಥಿರಾಂಕ ಎಲ್ಲೆ_Iter &ಎಲ್ಲೆ) {
        // Very similar to U8_FWD_1().
        uint8_t b = *p;
        ++p;
        ಆದರೆ (U8_IS_LEAD(b) && p != ಎಲ್ಲೆ) {
            uint8_t t1 = *p;
            ಆದರೆ ((0xe0 <= b && b < 0xf0)) {
                ಆದರೆ (U8_IS_VALID_LEAD3_AND_T1(b, t1) &&
                        ++p != ಎಲ್ಲೆ && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b < 0xe0) {
                ಆದರೆ (U8_IS_TRAIL(t1)) {
                    ++p;
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* b >= 0xf0 */ {
                ಆದರೆ (U8_IS_VALID_LEAD4_AND_T1(b, t1) &&
                        ++p != ಎಲ್ಲೆ && U8_IS_TRAIL(*p) &&
                        ++p != ಎಲ್ಲೆ && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            }
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ dec(UnitIter ಆರಂಭ, UnitIter &p) {
        // Very similar to U8_BACK_1().
        uint8_t c = *--p;
        ಆದರೆ (U8_IS_TRAIL(c) && p != ಆರಂಭ) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            ಆದರೆ (U8_IS_LEAD(b1)) {
                ಆದರೆ (b1 < 0xe0 ||
                        (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c))) {
                    p = p1;
                    ಹಿಂತಿರುಗಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (U8_IS_TRAIL(b1) && p1 != ಆರಂಭ) {
                uint8_t b2 = *--p1;
                ಆದರೆ (0xe0 <= b2 && b2 <= 0xf4) {
                    ಆದರೆ (b2 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b2, b1) :
                            U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        p = p1;
                        ಹಿಂತಿರುಗಿ;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (U8_IS_TRAIL(b2) && p1 != ಆರಂಭ) {
                    uint8_t b3 = *--p1;
                    ಆದರೆ (0xf0 <= b3 && b3 <= 0xf4 && U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                        p = p1;
                        ಹಿಂತಿರುಗಿ;
                    }
                }
            }
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಸಂಕೇತ_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ಸ್ಥಿರಾಂಕ ಎಲ್ಲೆ_Iter &ಎಲ್ಲೆ) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_OR_FFFD().
        CP32 c = uint8_t(*p);
        ++p;
        ಆದರೆ (U8_IS_SINGLE(c)) {
            ಆದರೆ constexpr (isMultiPass) {
                ಹಿಂತಿರುಗಿ {c, 1, true, p0, p};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {c, 1, true};
            }
        }
        uint8_t ಉದ್ದಳತೆ = 1;
        uint8_t t = 0;
        ಆದರೆ (p != ಎಲ್ಲೆ &&
                // fetch/validate/assemble all but last trail byte
                (c >= 0xe0 ?
                    (c < 0xf0 ?  // U+0800..U+FFFF except surrogates
                        U8_LEAD3_T1_ಬಿಟ್ಟುಗಳು[c &= 0xf] & (1 << ((t = *p) >> 5)) &&
                        (t &= 0x3f, 1)
                    :  // U+10000..U+10FFFF
                        (c -= 0xf0) <= 4 &&
                        U8_LEAD4_T1_ಬಿಟ್ಟುಗಳು[(t = *p) >> 4] & (1 << c) &&
                        (c = (c << 6) | (t & 0x3f), ++ಉದ್ದಳತೆ, ++p != ಎಲ್ಲೆ) &&
                        (t = *p - 0x80) <= 0x3f) &&
                    // valid second-to-last trail byte
                    (c = (c << 6) | t, ++ಉದ್ದಳತೆ, ++p != ಎಲ್ಲೆ)
                :  // U+0080..U+07FF
                    c >= 0xc2 && (c &= 0x1f, 1)) &&
                // last trail byte
                (t = *p - 0x80) <= 0x3f) {
            c = (c << 6) | t;
            ++ಉದ್ದಳತೆ;
            ++p;
            ಆದರೆ constexpr (isMultiPass) {
                ಹಿಂತಿರುಗಿ {c, ಉದ್ದಳತೆ, true, p0, p};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {c, ಉದ್ದಳತೆ, true};
            }
        }
        ಆದರೆ constexpr (isMultiPass) {
            ಹಿಂತಿರುಗಿ {sub(), ಉದ್ದಳತೆ, false, p0, p};
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ {sub(), ಉದ್ದಳತೆ, false};
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಸಂಕೇತ_Units_2<CP32, UnitIter> decAndRead(UnitIter ಆರಂಭ, UnitIter &p) {
        // Very similar to U8_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        ಆದರೆ (U8_IS_SINGLE(c)) {
            ಹಿಂತಿರುಗಿ {c, 1, true, p, p0};
        }
        ಆದರೆ (U8_IS_TRAIL(c) && p != ಆರಂಭ) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            ಆದರೆ (U8_IS_LEAD(b1)) {
                ಆದರೆ (b1 < 0xe0) {
                    p = p1;
                    c = ((b1 - 0xc0) << 6) | (c & 0x3f);
                    ಹಿಂತಿರುಗಿ {c, 2, true, p, p0};
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c)) {
                    // Truncated 3- or 4-byte sequence.
                    p = p1;
                    ಹಿಂತಿರುಗಿ {sub(), 2, false, p, p0};
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (U8_IS_TRAIL(b1) && p1 != ಆರಂಭ) {
                // Extract the value bits from the last trail byte.
                c &= 0x3f;
                uint8_t b2 = *--p1;
                ಆದರೆ (0xe0 <= b2 && b2 <= 0xf4) {
                    ಆದರೆ (b2 < 0xf0) {
                        b2 &= 0xf;
                        ಆದರೆ (U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                            p = p1;
                            c = (b2 << 12) | ((b1 & 0x3f) << 6) | c;
                            ಹಿಂತಿರುಗಿ {c, 3, true, p, p0};
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        // Truncated 4-byte sequence.
                        p = p1;
                        ಹಿಂತಿರುಗಿ {sub(), 3, false, p, p0};
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (U8_IS_TRAIL(b2) && p1 != ಆರಂಭ) {
                    uint8_t b3 = *--p1;
                    ಆದರೆ (0xf0 <= b3 && b3 <= 0xf4) {
                        b3 &= 7;
                        ಆದರೆ (U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                            p = p1;
                            c = (b3 << 18) | ((b2 & 0x3f) << 12) | ((b1 & 0x3f) << 6) | c;
                            ಹಿಂತಿರುಗಿ {c, 4, true, p, p0};
                        }
                    }
                }
            }
        }
        ಹಿಂತಿರುಗಿ {sub(), 1, false, p, p0};
    }
};

// UTF-16
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename ಎಲ್ಲೆ_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, ಎಲ್ಲೆ_Iter,
        std::enable_if_t<ಗಾತ್ರ(typename prv::iter_ಮೌಲ್ಯ_t<UnitIter>) == 2>> {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ಸಾರ್ವಜನಿಕ:
    // Handle ill-formed UTF-16: One unpaired surrogate.
    U_FORCE_INLINE ಸ್ಥಿರ CP32 sub(CP32 surrogate) {
        ಆದರೆ constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            ಹಿಂತಿರುಗಿ U_SENTINEL;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            ಹಿಂತಿರುಗಿ 0xfffd;
        } ಇಲ್ಲದಿದ್ದರೆ {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            ಹಿಂತಿರುಗಿ surrogate;
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ inc(UnitIter &p, ಸ್ಥಿರಾಂಕ ಎಲ್ಲೆ_Iter &ಎಲ್ಲೆ) {
        // Very similar to U16_FWD_1().
        ಸ್ವಯಂಚಾಲಿತ c = *p;
        ++p;
        ಆದರೆ (U16_IS_LEAD(c) && p != ಎಲ್ಲೆ && U16_IS_TRAIL(*p)) {
            ++p;
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ dec(UnitIter ಆರಂಭ, UnitIter &p) {
        // Very similar to U16_BACK_1().
        UnitIter p1;
        ಆದರೆ (U16_IS_TRAIL(*--p) && p != ಆರಂಭ && (p1 = p, U16_IS_LEAD(*--p1))) {
            p = p1;
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಸಂಕೇತ_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ಸ್ಥಿರಾಂಕ ಎಲ್ಲೆ_Iter &ಎಲ್ಲೆ) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_OR_FFFD().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        ಆದರೆ (!U16_IS_SURROGATE(c)) {
            ಆದರೆ constexpr (isMultiPass) {
                ಹಿಂತಿರುಗಿ {c, 1, true, p0, p};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {c, 1, true};
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            uint16_t c2;
            ಆದರೆ (U16_IS_SURROGATE_LEAD(c) && p != ಎಲ್ಲೆ && U16_IS_TRAIL(c2 = *p)) {
                ++p;
                c = U16_GET_SUPPLEMENTARY(c, c2);
                ಆದರೆ constexpr (isMultiPass) {
                    ಹಿಂತಿರುಗಿ {c, 2, true, p0, p};
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಹಿಂತಿರುಗಿ {c, 2, true};
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ constexpr (isMultiPass) {
                    ಹಿಂತಿರುಗಿ {sub(c), 1, false, p0, p};
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಹಿಂತಿರುಗಿ {sub(c), 1, false};
                }
            }
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಸಂಕೇತ_Units_2<CP32, UnitIter> decAndRead(UnitIter ಆರಂಭ, UnitIter &p) {
        // Very similar to U16_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        ಆದರೆ (!U16_IS_SURROGATE(c)) {
            ಹಿಂತಿರುಗಿ {c, 1, true, p, p0};
        } ಇಲ್ಲದಿದ್ದರೆ {
            UnitIter p1;
            uint16_t c2;
            ಆದರೆ (U16_IS_SURROGATE_TRAIL(c) && p != ಆರಂಭ && (p1 = p, U16_IS_LEAD(c2 = *--p1))) {
                p = p1;
                c = U16_GET_SUPPLEMENTARY(c2, c);
                ಹಿಂತಿರುಗಿ {c, 2, true, p, p0};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {sub(c), 1, false, p, p0};
            }
        }
    }
};

// UTF-32: trivial, but still validating
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename ಎಲ್ಲೆ_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, ಎಲ್ಲೆ_Iter,
        std::enable_if_t<ಗಾತ್ರ(typename prv::iter_ಮೌಲ್ಯ_t<UnitIter>) == 4>> {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ಸಾರ್ವಜನಿಕ:
    // Handle ill-formed UTF-32
    U_FORCE_INLINE ಸ್ಥಿರ CP32 sub(bool forSurrogate, CP32 surrogate) {
        ಆದರೆ constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            ಹಿಂತಿರುಗಿ U_SENTINEL;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            ಹಿಂತಿರುಗಿ 0xfffd;
        } ಇಲ್ಲದಿದ್ದರೆ {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            ಹಿಂತಿರುಗಿ forSurrogate ? surrogate : 0xfffd;
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ inc(UnitIter &p, ಸ್ಥಿರಾಂಕ ಎಲ್ಲೆ_Iter &/*ಎಲ್ಲೆ*/) {
        ++p;
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ dec(UnitIter /*ಆರಂಭ*/, UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಸಂಕೇತ_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ಸ್ಥಿರಾಂಕ ಎಲ್ಲೆ_Iter &/*ಎಲ್ಲೆ*/) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        uint32_t uc = *p;
        CP32 c = uc;
        ++p;
        ಆದರೆ (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            ಆದರೆ constexpr (isMultiPass) {
                ಹಿಂತಿರುಗಿ {c, 1, true, p0, p};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {c, 1, true};
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ constexpr (isMultiPass) {
                ಹಿಂತಿರುಗಿ {sub(uc < 0xe000, c), 1, false, p0, p};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {sub(uc < 0xe000, c), 1, false};
            }
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಸಂಕೇತ_Units_2<CP32, UnitIter> decAndRead(UnitIter /*ಆರಂಭ*/, UnitIter &p) {
        UnitIter p0 = p;
        uint32_t uc = *--p;
        CP32 c = uc;
        ಆದರೆ (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            ಹಿಂತಿರುಗಿ {c, 1, true, p, p0};
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ {sub(uc < 0xe000, c), 1, false, p, p0};
        }
    }
};

// Non-validating implementations ------------------------------------------ ***

ಮಾದರಿ<typename CP32, typename UnitIter, typename = ಶೂನ್ಯ>
class UnsafeUTFImpl;

// UTF-8
ಮಾದರಿ<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<ಗಾತ್ರ(typename prv::iter_ಮೌಲ್ಯ_t<UnitIter>) == 1>> {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ಸಾರ್ವಜನಿಕ:
    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ inc(UnitIter &p) {
        // Very similar to U8_FWD_1_UNSAFE().
        uint8_t b = *p;
        std::advance(p, 1 + U8_ಎಣಿಕೆ_TRAIL_ಬೈಟುಗಳು_UNSAFE(b));
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ dec(UnitIter &p) {
        // Very similar to U8_BACK_1_UNSAFE().
        ಆಗ (U8_IS_TRAIL(*--p)) {}
    }

    U_FORCE_INLINE ಸ್ಥಿರ Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_UNSAFE().
        CP32 c = uint8_t(*p);
        ++p;
        ಆದರೆ (U8_IS_SINGLE(c)) {
            ಆದರೆ constexpr (isMultiPass) {
                ಹಿಂತಿರುಗಿ {c, 1, p0, p};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {c, 1};
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c < 0xe0) {
            c = ((c & 0x1f) << 6) | (*p & 0x3f);
            ++p;
            ಆದರೆ constexpr (isMultiPass) {
                ಹಿಂತಿರುಗಿ {c, 2, p0, p};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {c, 2};
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c < 0xf0) {
            // No need for (c&0xf) because the upper bits are truncated
            // after <<12 in the cast to uint16_t.
            c = uint16_t(c << 12) | ((*p & 0x3f) << 6);
            ++p;
            c |= *p & 0x3f;
            ++p;
            ಆದರೆ constexpr (isMultiPass) {
                ಹಿಂತಿರುಗಿ {c, 3, p0, p};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {c, 3};
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            c = ((c & 7) << 18) | ((*p & 0x3f) << 12);
            ++p;
            c |= (*p & 0x3f) << 6;
            ++p;
            c |= *p & 0x3f;
            ++p;
            ಆದರೆ constexpr (isMultiPass) {
                ಹಿಂತಿರುಗಿ {c, 4, p0, p};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {c, 4};
            }
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U8_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        ಆದರೆ (U8_IS_SINGLE(c)) {
            ಹಿಂತಿರುಗಿ {c, 1, p, p0};
        }
        // U8_IS_TRAIL(c) if well-formed
        c &= 0x3f;
        uint8_t ಎಣಿಕೆ_2 = 1;
        ಗಾಗಿ (uint8_t ಸರಿಸು = 6;;) {
            uint8_t b = *--p;
            ಆದರೆ (b >= 0xc0) {
                U8_ಮುಖವಾಡ_LEAD_BYTE(b, ಎಣಿಕೆ_2);
                c |= uint32_t{b} << ಸರಿಸು;
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                c |= (uint32_t{b} & 0x3f) << ಸರಿಸು;
                ++ಎಣಿಕೆ_2;
                ಸರಿಸು += 6;
            }
        }
        ++ಎಣಿಕೆ_2;
        ಹಿಂತಿರುಗಿ {c, ಎಣಿಕೆ_2, p, p0};
    }
};

// UTF-16
ಮಾದರಿ<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<ಗಾತ್ರ(typename prv::iter_ಮೌಲ್ಯ_t<UnitIter>) == 2>> {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ಸಾರ್ವಜನಿಕ:
    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ inc(UnitIter &p) {
        // Very similar to U16_FWD_1_UNSAFE().
        ಸ್ವಯಂಚಾಲಿತ c = *p;
        ++p;
        ಆದರೆ (U16_IS_LEAD(c)) {
            ++p;
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ dec(UnitIter &p) {
        // Very similar to U16_BACK_1_UNSAFE().
        ಆದರೆ (U16_IS_TRAIL(*--p)) {
            --p;
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_UNSAFE().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        ಆದರೆ (!U16_IS_LEAD(c)) {
            ಆದರೆ constexpr (isMultiPass) {
                ಹಿಂತಿರುಗಿ {c, 1, p0, p};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {c, 1};
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            uint16_t c2 = *p;
            ++p;
            c = U16_GET_SUPPLEMENTARY(c, c2);
            ಆದರೆ constexpr (isMultiPass) {
                ಹಿಂತಿರುಗಿ {c, 2, p0, p};
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ {c, 2};
            }
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U16_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        ಆದರೆ (!U16_IS_TRAIL(c)) {
            ಹಿಂತಿರುಗಿ {c, 1, p, p0};
        } ಇಲ್ಲದಿದ್ದರೆ {
            uint16_t c2 = *--p;
            c = U16_GET_SUPPLEMENTARY(c2, c);
            ಹಿಂತಿರುಗಿ {c, 2, p, p0};
        }
    }
};

// UTF-32: trivial
ಮಾದರಿ<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<ಗಾತ್ರ(typename prv::iter_ಮೌಲ್ಯ_t<UnitIter>) == 4>> {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ಸಾರ್ವಜನಿಕ:
    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ inc(UnitIter &p) {
        ++p;
    }

    U_FORCE_INLINE ಸ್ಥಿರ ಶೂನ್ಯ dec(UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE ಸ್ಥಿರ Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        CP32 c = *p;
        ++p;
        ಆದರೆ constexpr (isMultiPass) {
            ಹಿಂತಿರುಗಿ {c, 1, p0, p};
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ {c, 1};
        }
    }

    U_FORCE_INLINE ಸ್ಥಿರ Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        UnitIter p0 = p;
        CP32 c = *--p;
        ಹಿಂತಿರುಗಿ {c, 1, p, p0};
    }
};

#ಆದರೆ_ಅಂತ್ಯ

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
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename ಎಲ್ಲೆ_Iter = UnitIter, typename = ಶೂನ್ಯ>
class UTFIterator {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, ಎಲ್ಲೆ_Iter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    ಸಾರ್ವಜನಿಕ:
        explicit Proxy(ಸಂಕೇತ_Units_2<CP32, UnitIter> &units) : units_(units) {}
        ಸಂಕೇತ_Units_2<CP32, UnitIter>& ಕ್ರಿಯಾಕಾರಕ*() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ units_; }
        ಸಂಕೇತ_Units_2<CP32, UnitIter>* ಕ್ರಿಯಾಕಾರಕ->() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ &units_; }
    ಖಾಸಗಿ:
        ಸಂಕೇತ_Units_2<CP32, UnitIter> units_;
    };

ಸಾರ್ವಜನಿಕ:
    /** C++ iterator boilerplate @internal */
    using ಮೌಲ್ಯ_type = ಸಂಕೇತ_Units_2<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = ಮೌಲ್ಯ_type;
    /** C++ iterator boilerplate @internal */
    using ಸೂಚಕ = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_ಟ್ಯಾಗ್,
        std::forward_iterator_ಟ್ಯಾಗ್>;

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
    U_FORCE_INLINE UTFIterator(UnitIter ಆರಂಭ, UnitIter p, ಎಲ್ಲೆ_Iter ಎಲ್ಲೆ) :
            p_(p), ಆರಂಭ_3(ಆರಂಭ), ಎಲ್ಲೆ_3(ಎಲ್ಲೆ), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE UTFIterator(UnitIter p, ಎಲ್ಲೆ_Iter ಎಲ್ಲೆ) :
            p_(p), ಆರಂಭ_3(p), ಎಲ್ಲೆ_3(ಎಲ್ಲೆ), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(p), ಆರಂಭ_3(p), ಎಲ್ಲೆ_3(p), units_(0, 0, false, p, p) {}
    /**
     * Default constructor. Makes a non-functional iterator.
     *
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator() : p_{}, ಆರಂಭ_3{}, ಎಲ್ಲೆ_3{}, units_(0, 0, false, p_, p_) {}

    /** Move constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &ಕ್ರಿಯಾಕಾರಕ=(UTFIterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(ಸ್ಥಿರಾಂಕ UTFIterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UTFIterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UTFIterator &other) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ ತಳ() == other.ತಳ();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UTFIterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other); }

    // Asymmetric equality & nonequality with a sentinel type.

    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UTFIterator &iter, ಸ್ಥಿರಾಂಕ Sentinel &s) {
        ಹಿಂತಿರುಗಿ iter.ತಳ() == s;
    }

#ಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ < 20
    // C++17: Need to define all four combinations of == / != vs. parameter order.
    // Once we require C++20, we could remove all but the first == because
    // the compiler would generate the rest.

    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Sentinel &s, ಸ್ಥಿರಾಂಕ UTFIterator &iter) {
        ಹಿಂತಿರುಗಿ iter.ತಳ() == s;
    }
    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UTFIterator &iter, ಸ್ಥಿರಾಂಕ Sentinel &s) { ಹಿಂತಿರುಗಿ !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Sentinel &s, ಸ್ಥಿರಾಂಕ UTFIterator &iter) { ಹಿಂತಿರುಗಿ !(iter == s); }
#ಆದರೆ_ಅಂತ್ಯ  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter ತಳ() ಸ್ಥಿರಾಂಕ {
        // Return the logical position.
        ಹಿಂತಿರುಗಿ state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE ಸಂಕೇತ_Units_2<CP32, UnitIter> ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, ಎಲ್ಲೆ_3);
            state_ = 1;
        }
        ಹಿಂತಿರುಗಿ units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, ಎಲ್ಲೆ_3);
            state_ = 1;
        }
        ಹಿಂತಿರುಗಿ Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator& ಕ್ರಿಯಾಕಾರಕ++() U_LIFETIME_BOUND {  // pre-increment
        ಆದರೆ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ == 0) {
            Impl::inc(p_, ಎಲ್ಲೆ_3);
        } ಇಲ್ಲದಿದ್ದರೆ /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.ಕೊನೆ();
            state_ = 0;
        }
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) {  // post-increment
        ಆದರೆ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UTFIterator ಫಲಿತಾಂಶ(*this);
            state_ = 0;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, ಎಲ್ಲೆ_3);
            UTFIterator ಫಲಿತಾಂಶ(*this);
            ಫಲಿತಾಂಶ.state_ = 1;
            // keep this->state_ == 0
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ /* state_ < 0 */ {
            UTFIterator ಫಲಿತಾಂಶ(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.ಕೊನೆ();
            state_ = 0;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    ಮಾದರಿ <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator&>
    ಕ್ರಿಯಾಕಾರಕ--() U_LIFETIME_BOUND {  // pre-decrement
        ಆದರೆ (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(ಆರಂಭ_3, p_);
        state_ = -1;
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    ಮಾದರಿ<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator>
    ಕ್ರಿಯಾಕಾರಕ--(ಪೂರ್ಣಾಂಕ) {  // post-decrement
        UTFIterator ಫಲಿತಾಂಶ(*this);
        ಕ್ರಿಯಾಕಾರಕ--();
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

ಖಾಸಗಿ:
    ಮಿತ್ರ class std::ಹಿಮ್ಮುಖ_iterator<UTFIterator<CP32, behavior, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter ಆರಂಭ_3;
    ಎಲ್ಲೆ_Iter ಎಲ್ಲೆ_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable ಸಂಕೇತ_Units_2<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
// Partial template specialization for single-pass input iterator.
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename ಎಲ್ಲೆ_Iter>
class UTFIterator<
        CP32, behavior,
        UnitIter, ಎಲ್ಲೆ_Iter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, ಎಲ್ಲೆ_Iter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    ಸಾರ್ವಜನಿಕ:
        explicit Proxy(ಸಂಕೇತ_Units_2<CP32, UnitIter> &units) : units_(units) {}
        ಸಂಕೇತ_Units_2<CP32, UnitIter>& ಕ್ರಿಯಾಕಾರಕ*() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ units_; }
        ಸಂಕೇತ_Units_2<CP32, UnitIter>* ಕ್ರಿಯಾಕಾರಕ->() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ &units_; }
    ಖಾಸಗಿ:
        ಸಂಕೇತ_Units_2<CP32, UnitIter> units_;
    };

ಸಾರ್ವಜನಿಕ:
    using ಮೌಲ್ಯ_type = ಸಂಕೇತ_Units_2<CP32, UnitIter>;
    using reference = ಮೌಲ್ಯ_type;
    using ಸೂಚಕ = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::ಒಳಹರಿವು_iterator_ಟ್ಯಾಗ್;

    U_FORCE_INLINE UTFIterator(UnitIter p, ಎಲ್ಲೆ_Iter ಎಲ್ಲೆ) : p_(std::move(p)), ಎಲ್ಲೆ_3(std::move(ಎಲ್ಲೆ)) {}

    // Constructs an iterator start or limit sentinel.
    // Requires p to be copyable.
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(std::move(p)), ಎಲ್ಲೆ_3(p_) {}

    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;
    U_FORCE_INLINE UTFIterator &ಕ್ರಿಯಾಕಾರಕ=(UTFIterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;

    U_FORCE_INLINE UTFIterator(ಸ್ಥಿರಾಂಕ UTFIterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;
    U_FORCE_INLINE UTFIterator &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UTFIterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UTFIterator &other) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UTFIterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other); }

    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UTFIterator &iter, ಸ್ಥಿರಾಂಕ Sentinel &s) {
        ಹಿಂತಿರುಗಿ !iter.ahead_ && iter.p_ == s;
    }

#ಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ < 20
    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Sentinel &s, ಸ್ಥಿರಾಂಕ UTFIterator &iter) {
        ಹಿಂತಿರುಗಿ !iter.ahead_ && iter.p_ == s;
    }

    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UTFIterator &iter, ಸ್ಥಿರಾಂಕ Sentinel &s) { ಹಿಂತಿರುಗಿ !(iter == s); }

    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Sentinel &s, ಸ್ಥಿರಾಂಕ UTFIterator &iter) { ಹಿಂತಿರುಗಿ !(iter == s); }
#ಆದರೆ_ಅಂತ್ಯ  // C++17

    U_FORCE_INLINE ಸಂಕೇತ_Units_2<CP32, UnitIter> ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, ಎಲ್ಲೆ_3);
            ahead_ = true;
        }
        ಹಿಂತಿರುಗಿ units_;
    }

    U_FORCE_INLINE Proxy ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, ಎಲ್ಲೆ_3);
            ahead_ = true;
        }
        ಹಿಂತಿರುಗಿ Proxy(units_);
    }

    U_FORCE_INLINE UTFIterator& ಕ್ರಿಯಾಕಾರಕ++() U_LIFETIME_BOUND {  // pre-increment
        ಆದರೆ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } ಇಲ್ಲದಿದ್ದರೆ {
            Impl::inc(p_, ಎಲ್ಲೆ_3);
        }
        ಹಿಂತಿರುಗಿ *this;
    }

    U_FORCE_INLINE Proxy ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) {  // post-increment
        ಆದರೆ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } ಇಲ್ಲದಿದ್ದರೆ {
            units_ = Impl::readAndInc(p_, p_, ಎಲ್ಲೆ_3);
            // keep this->ahead_ == false
        }
        ಹಿಂತಿರುಗಿ Proxy(units_);
    }

ಖಾಸಗಿ:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need limit_ so that when we read a code point
    // we can test if there are enough code units.
    ಎಲ್ಲೆ_Iter ಎಲ್ಲೆ_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable ಸಂಕೇತ_Units_2<CP32, UnitIter> units_ = {0, 0, false};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#ಆದರೆ_ಅಂತ್ಯ  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
class std::ಹಿಮ್ಮುಖ_iterator<U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>> {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::UTFImpl<CP32, behavior, UnitIter>;
    using ಸಂಕೇತ_Units_3 = U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::ಸಂಕೇತ_Units_2<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    ಸಾರ್ವಜನಿಕ:
        explicit Proxy(ಸಂಕೇತ_Units_3 units) : units_(units) {}
        ಸಂಕೇತ_Units_3& ಕ್ರಿಯಾಕಾರಕ*() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ units_; }
        ಸಂಕೇತ_Units_3* ಕ್ರಿಯಾಕಾರಕ->() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ &units_; }
    ಖಾಸಗಿ:
        ಸಂಕೇತ_Units_3 units_;
    };

ಸಾರ್ವಜನಿಕ:
    using ಮೌಲ್ಯ_type = ಸಂಕೇತ_Units_3;
    using reference = ಮೌಲ್ಯ_type;
    using ಸೂಚಕ = Proxy;
    using difference_type = U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_ಟ್ಯಾಗ್;

    U_FORCE_INLINE explicit ಹಿಮ್ಮುಖ_iterator(U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> iter) :
            p_(iter.ತಳ()), ಆರಂಭ_3(iter.ಆರಂಭ_3), ಎಲ್ಲೆ_3(iter.ಎಲ್ಲೆ_3),
            units_(0, 0, false, p_, p_) {}
    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator() : p_{}, ಆರಂಭ_3{}, ಎಲ್ಲೆ_3{}, units_(0, 0, false, p_, p_) {}

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator(ಹಿಮ್ಮುಖ_iterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;
    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator &ಕ್ರಿಯಾಕಾರಕ=(ಹಿಮ್ಮುಖ_iterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator(ಸ್ಥಿರಾಂಕ ಹಿಮ್ಮುಖ_iterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;
    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಹಿಮ್ಮುಖ_iterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಹಿಮ್ಮುಖ_iterator &other) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ ಹಿಮ್ಮುಖ_iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other); }

    U_FORCE_INLINE U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> ತಳ() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>(
            ಆರಂಭ_3, getLogicalPosition(), ಎಲ್ಲೆ_3);
    }

    U_FORCE_INLINE ಸಂಕೇತ_Units_3 ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (state_ == 0) {
            units_ = Impl::decAndRead(ಆರಂಭ_3, p_);
            state_ = -1;
        }
        ಹಿಂತಿರುಗಿ units_;
    }

    U_FORCE_INLINE Proxy ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (state_ == 0) {
            units_ = Impl::decAndRead(ಆರಂಭ_3, p_);
            state_ = -1;
        }
        ಹಿಂತಿರುಗಿ Proxy(units_);
    }

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator& ಕ್ರಿಯಾಕಾರಕ++() U_LIFETIME_BOUND {  // pre-increment
        ಆದರೆ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ == 0) {
            Impl::dec(ಆರಂಭ_3, p_);
        } ಇಲ್ಲದಿದ್ದರೆ /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        ಹಿಂತಿರುಗಿ *this;
    }

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) {  // post-increment
        ಆದರೆ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            ಹಿಮ್ಮುಖ_iterator ಫಲಿತಾಂಶ(*this);
            state_ = 0;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ == 0) {
            units_ = Impl::decAndRead(ಆರಂಭ_3, p_);
            ಹಿಮ್ಮುಖ_iterator ಫಲಿತಾಂಶ(*this);
            ಫಲಿತಾಂಶ.state_ = -1;
            // keep this->state_ == 0
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ /* state_ > 0 */ {
            ಹಿಮ್ಮುಖ_iterator ಫಲಿತಾಂಶ(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
    }

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator& ಕ್ರಿಯಾಕಾರಕ--() U_LIFETIME_BOUND {  // pre-decrement
        ಆದರೆ (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.ಕೊನೆ();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_, ಎಲ್ಲೆ_3);
        state_ = 1;
        ಹಿಂತಿರುಗಿ *this;
    }

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator ಕ್ರಿಯಾಕಾರಕ--(ಪೂರ್ಣಾಂಕ) {  // post-decrement
        ಹಿಮ್ಮುಖ_iterator ಫಲಿತಾಂಶ(*this);
        ಕ್ರಿಯಾಕಾರಕ--();
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

ಖಾಸಗಿ:
    U_FORCE_INLINE UnitIter getLogicalPosition() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ state_ >= 0 ? p_ : units_.ಕೊನೆ();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter ಆರಂಭ_3;
    UnitIter ಎಲ್ಲೆ_3;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable ಸಂಕೇತ_Units_3 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#ಆದರೆ_ಅಂತ್ಯ  // U_IN_DOXYGEN

namespace U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE {

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
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename ಎಲ್ಲೆ_Iter = UnitIter>
ಸ್ವಯಂಚಾಲಿತ utfIterator(UnitIter ಆರಂಭ, UnitIter p, ಎಲ್ಲೆ_Iter ಎಲ್ಲೆ) {
    ಹಿಂತಿರುಗಿ UTFIterator<CP32, behavior, UnitIter, ಎಲ್ಲೆ_Iter>(
        std::move(ಆರಂಭ), std::move(p), std::move(ಎಲ್ಲೆ));
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
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename ಎಲ್ಲೆ_Iter = UnitIter>
ಸ್ವಯಂಚಾಲಿತ utfIterator(UnitIter p, ಎಲ್ಲೆ_Iter ಎಲ್ಲೆ) {
    ಹಿಂತಿರುಗಿ UTFIterator<CP32, behavior, UnitIter, ಎಲ್ಲೆ_Iter>(
        std::move(p), std::move(ಎಲ್ಲೆ));
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
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
ಸ್ವಯಂಚಾಲಿತ utfIterator(UnitIter p) {
    ಹಿಂತಿರುಗಿ UTFIterator<CP32, behavior, UnitIter>(std::move(p));
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
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior, typename Range>
#ಆದರೆ defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#ಆದರೆ_ಅಂತ್ಯ
class UTFString_ಸಂಕೇತ_Points {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ಸಾರ್ವಜನಿಕ:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    UTFString_ಸಂಕೇತ_Points() = ಪೂರ್ವನಿಯೋಜಿತ;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    ಮಾದರಿ<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit UTFString_ಸಂಕೇತ_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C+20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    ಮಾದರಿ<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = ಶೂನ್ಯ>
    explicit UTFString_ಸಂಕೇತ_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    UTFString_ಸಂಕೇತ_Points(ಸ್ಥಿರಾಂಕ UTFString_ಸಂಕೇತ_Points &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    /** Copy assignment operator. @draft ICU 78 */
    UTFString_ಸಂಕೇತ_Points &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UTFString_ಸಂಕೇತ_Points &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ begin() {
        ಹಿಂತಿರುಗಿ utfIterator<CP32, behavior>(unitRange.begin(), unitRange.ಕೊನೆ());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    ಮಾದರಿ<typename R = Range, typename = std::enable_if_t<prv::range<ಸ್ಥಿರಾಂಕ R>>>
    ಸ್ವಯಂಚಾಲಿತ begin() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ utfIterator<CP32, behavior>(unitRange.begin(), unitRange.ಕೊನೆ());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ ಕೊನೆ() {
        using UnitIter = decltype(unitRange.begin());
        using ಎಲ್ಲೆ_Iter = decltype(unitRange.ಕೊನೆ());
        ಆದರೆ constexpr (!std::is_same_v<UnitIter, ಎಲ್ಲೆ_Iter>) {
            // Return the code unit sentinel.
            ಹಿಂತಿರುಗಿ unitRange.ಕೊನೆ();
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ constexpr (prv::bidirectional_iterator<UnitIter>) {
            ಹಿಂತಿರುಗಿ utfIterator<CP32, behavior>(unitRange.begin(), unitRange.ಕೊನೆ(), unitRange.ಕೊನೆ());
        } ಇಲ್ಲದಿದ್ದರೆ {
            // The input iterator specialization has no three-argument constructor.
            ಹಿಂತಿರುಗಿ utfIterator<CP32, behavior>(unitRange.ಕೊನೆ(), unitRange.ಕೊನೆ());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    ಮಾದರಿ<typename R = Range, typename = std::enable_if_t<prv::range<ಸ್ಥಿರಾಂಕ R>>>
    ಸ್ವಯಂಚಾಲಿತ ಕೊನೆ() ಸ್ಥಿರಾಂಕ {
        using UnitIter = decltype(unitRange.begin());
        using ಎಲ್ಲೆ_Iter = decltype(unitRange.ಕೊನೆ());
        ಆದರೆ constexpr (!std::is_same_v<UnitIter, ಎಲ್ಲೆ_Iter>) {
            // Return the code unit sentinel.
            ಹಿಂತಿರುಗಿ unitRange.ಕೊನೆ();
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ constexpr (prv::bidirectional_iterator<UnitIter>) {
            ಹಿಂತಿರುಗಿ utfIterator<CP32, behavior>(unitRange.begin(), unitRange.ಕೊನೆ(), unitRange.ಕೊನೆ());
        } ಇಲ್ಲದಿದ್ದರೆ {
            // The input iterator specialization has no three-argument constructor.
            ಹಿಂತಿರುಗಿ utfIterator<CP32, behavior>(unitRange.ಕೊನೆ(), unitRange.ಕೊನೆ());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ rbegin() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ std::make_ಹಿಮ್ಮುಖ_iterator(ಕೊನೆ());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ rend() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ std::make_ಹಿಮ್ಮುಖ_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    ಸ್ವಯಂಚಾಲಿತ front() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    ಸ್ವಯಂಚಾಲಿತ ಹಿಂಭಾಗ() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ *(--ಕೊನೆ());
    }

ಖಾಸಗಿ:
    Range unitRange;
};

/** @internal */
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior>
ರಚನೆ UTFString_ಸಂಕೇತ_Points_Adaptor
#ಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_ಆವರಣ<UTFString_ಸಂಕೇತ_Points_Adaptor<CP32, behavior>>
#ಆದರೆ_ಅಂತ್ಯ
{
    /** @internal */
    ಮಾದರಿ<typename Range>
    ಸ್ವಯಂಚಾಲಿತ ಕ್ರಿಯಾಕಾರಕ()(Range &&unitRange) ಸ್ಥಿರಾಂಕ {
#ಆದರೆ defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        ಹಿಂತಿರುಗಿ UTFString_ಸಂಕೇತ_Points<CP32, behavior, std::ranges::views::all_t<Range>>(
            std::forward<Range>(unitRange));
#ಇಲ್ಲದಿದ್ದರೆ
        ಆದರೆ constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            ಹಿಂತಿರುಗಿ UTFString_ಸಂಕೇತ_Points<CP32, behavior, std::decay_t<Range>>(
                std::forward<Range>(unitRange));
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ UTFString_ಸಂಕೇತ_Points<CP32, behavior, Range>(std::forward<Range>(unitRange));
        }
#ಆದರೆ_ಅಂತ್ಯ
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
ಮಾದರಿ<typename CP32, UTFIllFormedBehavior behavior>
constexpr UTFString_ಸಂಕೇತ_Points_Adaptor<CP32, behavior> utf_String_ಸಂಕೇತ_Points;

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
ಮಾದರಿ<typename CP32, typename UnitIter, typename = ಶೂನ್ಯ>
class UnsafeUTFIterator {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    ಸಾರ್ವಜನಿಕ:
        explicit Proxy(Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter>& ಕ್ರಿಯಾಕಾರಕ*() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ units_; }
        Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter>* ಕ್ರಿಯಾಕಾರಕ->() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ &units_; }
    ಖಾಸಗಿ:
        Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> units_;
    };

ಸಾರ್ವಜನಿಕ:
    /** C++ iterator boilerplate @internal */
    using ಮೌಲ್ಯ_type = Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = ಮೌಲ್ಯ_type;
    /** C++ iterator boilerplate @internal */
    using ಸೂಚಕ = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_ಟ್ಯಾಗ್,
        std::forward_iterator_ಟ್ಯಾಗ್>;

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
    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &ಕ್ರಿಯಾಕಾರಕ=(UnsafeUTFIterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &other) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ ತಳ() == other.ತಳ();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other); }

    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &iter, ಸ್ಥಿರಾಂಕ Sentinel &s) {
        ಹಿಂತಿರುಗಿ iter.ತಳ() == s;
    }

#ಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ < 20
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Sentinel &s, ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &iter) {
        ಹಿಂತಿರುಗಿ iter.ತಳ() == s;
    }
    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &iter, ಸ್ಥಿರಾಂಕ Sentinel &s) { ಹಿಂತಿರುಗಿ !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Sentinel &s, ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &iter) { ಹಿಂತಿರುಗಿ !(iter == s); }
#ಆದರೆ_ಅಂತ್ಯ  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter ತಳ() ಸ್ಥಿರಾಂಕ {
        // Return the logical position.
        ಹಿಂತಿರುಗಿ state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        ಹಿಂತಿರುಗಿ units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        ಹಿಂತಿರುಗಿ Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator& ಕ್ರಿಯಾಕಾರಕ++() U_LIFETIME_BOUND {  // pre-increment
        ಆದರೆ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ == 0) {
            Impl::inc(p_);
        } ಇಲ್ಲದಿದ್ದರೆ /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.ಕೊನೆ();
            state_ = 0;
        }
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) {  // post-increment
        ಆದರೆ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UnsafeUTFIterator ಫಲಿತಾಂಶ(*this);
            state_ = 0;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            UnsafeUTFIterator ಫಲಿತಾಂಶ(*this);
            ಫಲಿತಾಂಶ.state_ = 1;
            // keep this->state_ == 0
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ /* state_ < 0 */ {
            UnsafeUTFIterator ಫಲಿತಾಂಶ(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.ಕೊನೆ();
            state_ = 0;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    ಮಾದರಿ <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator&>
    ಕ್ರಿಯಾಕಾರಕ--() U_LIFETIME_BOUND {  // pre-decrement
        ಆದರೆ (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(p_);
        state_ = -1;
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    ಮಾದರಿ<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator>
    ಕ್ರಿಯಾಕಾರಕ--(ಪೂರ್ಣಾಂಕ) {  // post-decrement
        UnsafeUTFIterator ಫಲಿತಾಂಶ(*this);
        ಕ್ರಿಯಾಕಾರಕ--();
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

ಖಾಸಗಿ:
    ಮಿತ್ರ class std::ಹಿಮ್ಮುಖ_iterator<UnsafeUTFIterator<CP32, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
// Partial template specialization for single-pass input iterator.
ಮಾದರಿ<typename CP32, typename UnitIter>
class UnsafeUTFIterator<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    ಸಾರ್ವಜನಿಕ:
        explicit Proxy(Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter>& ಕ್ರಿಯಾಕಾರಕ*() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ units_; }
        Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter>* ಕ್ರಿಯಾಕಾರಕ->() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ &units_; }
    ಖಾಸಗಿ:
        Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> units_;
    };

ಸಾರ್ವಜನಿಕ:
    using ಮೌಲ್ಯ_type = Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter>;
    using reference = ಮೌಲ್ಯ_type;
    using ಸೂಚಕ = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::ಒಳಹರಿವು_iterator_ಟ್ಯಾಗ್;

    U_FORCE_INLINE explicit UnsafeUTFIterator(UnitIter p) : p_(std::move(p)) {}

    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;
    U_FORCE_INLINE UnsafeUTFIterator &ಕ್ರಿಯಾಕಾರಕ=(UnsafeUTFIterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;

    U_FORCE_INLINE UnsafeUTFIterator(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;
    U_FORCE_INLINE UnsafeUTFIterator &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &other) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other); }

    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &iter, ಸ್ಥಿರಾಂಕ Sentinel &s) {
        ಹಿಂತಿರುಗಿ !iter.ahead_ && iter.p_ == s;
    }

#ಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ < 20
    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Sentinel &s, ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &iter) {
        ಹಿಂತಿರುಗಿ !iter.ahead_ && iter.p_ == s;
    }

    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &iter, ಸ್ಥಿರಾಂಕ Sentinel &s) { ಹಿಂತಿರುಗಿ !(iter == s); }

    ಮಾದರಿ<typename Sentinel> U_FORCE_INLINE ಮಿತ್ರ
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Sentinel &s, ಸ್ಥಿರಾಂಕ UnsafeUTFIterator &iter) { ಹಿಂತಿರುಗಿ !(iter == s); }
#ಆದರೆ_ಅಂತ್ಯ  // C++17

    U_FORCE_INLINE Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        ಹಿಂತಿರುಗಿ units_;
    }

    U_FORCE_INLINE Proxy ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        ಹಿಂತಿರುಗಿ Proxy(units_);
    }

    U_FORCE_INLINE UnsafeUTFIterator& ಕ್ರಿಯಾಕಾರಕ++() U_LIFETIME_BOUND {  // pre-increment
        ಆದರೆ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } ಇಲ್ಲದಿದ್ದರೆ {
            Impl::inc(p_);
        }
        ಹಿಂತಿರುಗಿ *this;
    }

    U_FORCE_INLINE Proxy ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) {  // post-increment
        ಆದರೆ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } ಇಲ್ಲದಿದ್ದರೆ {
            units_ = Impl::readAndInc(p_, p_);
            // keep this->ahead_ == false
        }
        ಹಿಂತಿರುಗಿ Proxy(units_);
    }

ಖಾಸಗಿ:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter> units_ = {0, 0};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#ಆದರೆ_ಅಂತ್ಯ  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
ಮಾದರಿ<typename CP32, typename UnitIter>
class std::ಹಿಮ್ಮುಖ_iterator<U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>> {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::UnsafeUTFImpl<CP32, UnitIter>;
    using Unsafe_ಸಂಕೇತ_Units_2 = U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::Unsafe_ಸಂಕೇತ_Units<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    ಸಾರ್ವಜನಿಕ:
        explicit Proxy(Unsafe_ಸಂಕೇತ_Units_2 units) : units_(units) {}
        Unsafe_ಸಂಕೇತ_Units_2& ಕ್ರಿಯಾಕಾರಕ*() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ units_; }
        Unsafe_ಸಂಕೇತ_Units_2* ಕ್ರಿಯಾಕಾರಕ->() U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ &units_; }
    ಖಾಸಗಿ:
        Unsafe_ಸಂಕೇತ_Units_2 units_;
    };

ಸಾರ್ವಜನಿಕ:
    using ಮೌಲ್ಯ_type = Unsafe_ಸಂಕೇತ_Units_2;
    using reference = ಮೌಲ್ಯ_type;
    using ಸೂಚಕ = Proxy;
    using difference_type = U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_ಟ್ಯಾಗ್;

    U_FORCE_INLINE explicit ಹಿಮ್ಮುಖ_iterator(U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> iter) :
            p_(iter.ತಳ()), units_(0, 0, p_, p_) {}
    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator() : p_{}, units_(0, 0, p_, p_) {}

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator(ಹಿಮ್ಮುಖ_iterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;
    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator &ಕ್ರಿಯಾಕಾರಕ=(ಹಿಮ್ಮುಖ_iterator &&src) noexcept = ಪೂರ್ವನಿಯೋಜಿತ;

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator(ಸ್ಥಿರಾಂಕ ಹಿಮ್ಮುಖ_iterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;
    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಹಿಮ್ಮುಖ_iterator &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಹಿಮ್ಮುಖ_iterator &other) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ ಹಿಮ್ಮುಖ_iterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other); }

    U_FORCE_INLINE U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> ತಳ() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>(
            getLogicalPosition());
    }

    U_FORCE_INLINE Unsafe_ಸಂಕೇತ_Units_2 ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        ಹಿಂತಿರುಗಿ units_;
    }

    U_FORCE_INLINE Proxy ಕ್ರಿಯಾಕಾರಕ->() ಸ್ಥಿರಾಂಕ {
        ಆದರೆ (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        ಹಿಂತಿರುಗಿ Proxy(units_);
    }

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator& ಕ್ರಿಯಾಕಾರಕ++() U_LIFETIME_BOUND {  // pre-increment
        ಆದರೆ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ == 0) {
            Impl::dec(p_);
        } ಇಲ್ಲದಿದ್ದರೆ /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        ಹಿಂತಿರುಗಿ *this;
    }

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) {  // post-increment
        ಆದರೆ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            ಹಿಮ್ಮುಖ_iterator ಫಲಿತಾಂಶ(*this);
            state_ = 0;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            ಹಿಮ್ಮುಖ_iterator ಫಲಿತಾಂಶ(*this);
            ಫಲಿತಾಂಶ.state_ = -1;
            // keep this->state_ == 0
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        } ಇಲ್ಲದಿದ್ದರೆ /* state_ > 0 */ {
            ಹಿಮ್ಮುಖ_iterator ಫಲಿತಾಂಶ(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
    }

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator& ಕ್ರಿಯಾಕಾರಕ--() U_LIFETIME_BOUND {  // pre-decrement
        ಆದರೆ (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.ಕೊನೆ();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_);
        state_ = 1;
        ಹಿಂತಿರುಗಿ *this;
    }

    U_FORCE_INLINE ಹಿಮ್ಮುಖ_iterator ಕ್ರಿಯಾಕಾರಕ--(ಪೂರ್ಣಾಂಕ) {  // post-decrement
        ಹಿಮ್ಮುಖ_iterator ಫಲಿತಾಂಶ(*this);
        ಕ್ರಿಯಾಕಾರಕ--();
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

ಖಾಸಗಿ:
    U_FORCE_INLINE UnitIter getLogicalPosition() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ state_ >= 0 ? p_ : units_.ಕೊನೆ();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_ಸಂಕೇತ_Units_2 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#ಆದರೆ_ಅಂತ್ಯ  // U_IN_DOXYGEN

namespace U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE {

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
ಮಾದರಿ<typename CP32, typename UnitIter>
ಸ್ವಯಂಚಾಲಿತ unsafeUTFIterator(UnitIter iter) {
    ಹಿಂತಿರುಗಿ UnsafeUTFIterator<CP32, UnitIter>(std::move(iter));
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
ಮಾದರಿ<typename CP32, typename Range>
#ಆದರೆ defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#ಆದರೆ_ಅಂತ್ಯ
class Unsafe_UTFString_ಸಂಕೇತ_Points {
    static_assert(ಗಾತ್ರ(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
ಸಾರ್ವಜನಿಕ:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    Unsafe_UTFString_ಸಂಕೇತ_Points() = ಪೂರ್ವನಿಯೋಜಿತ;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    ಮಾದರಿ<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit Unsafe_UTFString_ಸಂಕೇತ_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C++20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    ಮಾದರಿ<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = ಶೂನ್ಯ>
    explicit Unsafe_UTFString_ಸಂಕೇತ_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_UTFString_ಸಂಕೇತ_Points(ಸ್ಥಿರಾಂಕ Unsafe_UTFString_ಸಂಕೇತ_Points &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_UTFString_ಸಂಕೇತ_Points &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Unsafe_UTFString_ಸಂಕೇತ_Points &other) = ಪೂರ್ವನಿಯೋಜಿತ;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ begin() {
        ಹಿಂತಿರುಗಿ unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    ಮಾದರಿ<typename R = Range, typename = std::enable_if_t<prv::range<ಸ್ಥಿರಾಂಕ R>>>
    ಸ್ವಯಂಚಾಲಿತ begin() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ ಕೊನೆ() {
        using UnitIter = decltype(unitRange.begin());
        using ಎಲ್ಲೆ_Iter = decltype(unitRange.ಕೊನೆ());
        ಆದರೆ constexpr (!std::is_same_v<UnitIter, ಎಲ್ಲೆ_Iter>) {
            // Return the code unit sentinel.
            ಹಿಂತಿರುಗಿ unitRange.ಕೊನೆ();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ unsafeUTFIterator<CP32>(unitRange.ಕೊನೆ());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    ಮಾದರಿ<typename R = Range, typename = std::enable_if_t<prv::range<ಸ್ಥಿರಾಂಕ R>>>
    ಸ್ವಯಂಚಾಲಿತ ಕೊನೆ() ಸ್ಥಿರಾಂಕ {
        using UnitIter = decltype(unitRange.begin());
        using ಎಲ್ಲೆ_Iter = decltype(unitRange.ಕೊನೆ());
        ಆದರೆ constexpr (!std::is_same_v<UnitIter, ಎಲ್ಲೆ_Iter>) {
            // Return the code unit sentinel.
            ಹಿಂತಿರುಗಿ unitRange.ಕೊನೆ();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ unsafeUTFIterator<CP32>(unitRange.ಕೊನೆ());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ rbegin() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ std::make_ಹಿಮ್ಮುಖ_iterator(ಕೊನೆ());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    ಸ್ವಯಂಚಾಲಿತ rend() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ std::make_ಹಿಮ್ಮುಖ_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    ಸ್ವಯಂಚಾಲಿತ front() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    ಸ್ವಯಂಚಾಲಿತ ಹಿಂಭಾಗ() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ *(--ಕೊನೆ());
    }

ಖಾಸಗಿ:
    Range unitRange;
};

/** @internal */
ಮಾದರಿ<typename CP32>
ರಚನೆ Unsafe_UTFString_ಸಂಕೇತ_Points_Adaptor
#ಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_ಆವರಣ<Unsafe_UTFString_ಸಂಕೇತ_Points_Adaptor<CP32>>
#ಆದರೆ_ಅಂತ್ಯ
{
    /** @internal */
    ಮಾದರಿ<typename Range>
    ಸ್ವಯಂಚಾಲಿತ ಕ್ರಿಯಾಕಾರಕ()(Range &&unitRange) ಸ್ಥಿರಾಂಕ {
#ಆದರೆ defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        ಹಿಂತಿರುಗಿ Unsafe_UTFString_ಸಂಕೇತ_Points<CP32, std::ranges::views::all_t<Range>>(std::forward<Range>(unitRange));
#ಇಲ್ಲದಿದ್ದರೆ
        ಆದರೆ constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            ಹಿಂತಿರುಗಿ Unsafe_UTFString_ಸಂಕೇತ_Points<CP32, std::decay_t<Range>>(std::forward<Range>(unitRange));
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ Unsafe_UTFString_ಸಂಕೇತ_Points<CP32, Range>(std::forward<Range>(unitRange));
        }
#ಆದರೆ_ಅಂತ್ಯ
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
ಮಾದರಿ<typename CP32>
constexpr Unsafe_UTFString_ಸಂಕೇತ_Points_Adaptor<CP32> unsafe_UTFString_ಸಂಕೇತ_Points;

}  // namespace U_HEADER_ONLY_NAMESPACE


#ಆದರೆ defined(__cpp_lib_ranges)
ಮಾದರಿ <typename CP32, UTFIllFormedBehavior behavior, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::UTFString_ಸಂಕೇತ_Points<CP32, behavior, Range>> =
    std::ranges::enable_borrowed_range<Range>;

ಮಾದರಿ <typename CP32, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_ಶೀರ್ಷಿಕೆ_ONLY_NAMESPACE::Unsafe_UTFString_ಸಂಕೇತ_Points<CP32, Range>> =
    std::ranges::enable_borrowed_range<Range>;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ  // U_HIDE_DRAFT_API
#ಆದರೆ_ಅಂತ್ಯ  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#ಆದರೆ_ಅಂತ್ಯ  // __UTFITERATOR_H__
