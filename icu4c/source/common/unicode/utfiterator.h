// © 2024 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfiterator.h
// created: 2024aug12 Markus W. Scherer

#నిర్వచితం_కాకపోతే __UTFITERATOR_H__
#నిర్వచించు __UTFITERATOR_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_శీర్షిక_API || !defined(UTYPES_H)

#అన్తర్భావ <iterator>
#ఒకవేళ __has_include(<సంస్కరణ>)
#అన్తర్భావ <version>
#అయితే_ముగింపు
#ఒకవేళ defined(__cpp_lib_ranges)
#అన్తర్భావ <ranges>
#అయితే_ముగింపు
#అన్తర్భావ <string>
#అన్తర్భావ <string_view>
#అన్తర్భావ <type_traits>
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/uversion.h"

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

#నిర్వచితం_కాకపోతే U_HIDE_DRAFT_API

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
రకనిర్వచనం గణన UTFIllFormedBehavior {
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

namespace U_శీర్షిక_ONLY_NAMESPACE {

namespace prv {
#ఒకవేళ U_CPLUSPLUS_సంస్కరణ >= 20

/** @internal */
మూస<typename Iter>
using iter_విలువ_t = typename std::iter_విలువ_t<Iter>;

/** @internal */
మూస<typename Iter>
using iter_difference_t = std::iter_difference_t<Iter>;

/** @internal */
మూస<typename Iter>
constexpr bool forward_iterator = std::forward_iterator<Iter>;

/** @internal */
మూస<typename Iter>
constexpr bool bidirectional_iterator = std::bidirectional_iterator<Iter>;

/** @internal */
మూస<typename Range>
constexpr bool range = std::ranges::range<Range>;

#లేకపోతే

/** @internal */
మూస<typename Iter>
using iter_విలువ_t = typename std::iterator_traits<Iter>::విలువ_type;

/** @internal */
మూస<typename Iter>
using iter_difference_t = typename std::iterator_traits<Iter>::difference_type;

/** @internal */
మూస<typename Iter>
constexpr bool forward_iterator =
    std::is_పునాది_యొక్క_v<
        std::forward_iterator_ట్యాగ్,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
మూస<typename Iter>
constexpr bool bidirectional_iterator =
    std::is_పునాది_యొక్క_v<
        std::bidirectional_iterator_ట్యాగ్,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
మూస<typename Range, typename = శూన్య>
నిర్మాణం range_type : std::false_type {};

/** @internal */
మూస<typename Range>
నిర్మాణం range_type<
    Range,
    std::void_t<decltype(std::declval<Range>().begin()),
                decltype(std::declval<Range>().చివర())>> : std::true_type {};

/** @internal */
మూస<typename Range>
constexpr bool range = range_type<Range>::విలువ_2;

#అయితే_ముగింపు

/** @internal */
మూస <typename T> నిర్మాణం is_basic_string_view : std::false_type {};

/** @internal */
మూస <typename... వాదనలు>
నిర్మాణం is_basic_string_view<std::basic_string_view<వాదనలు...>> : std::true_type {};

/** @internal */
మూస <typename T> constexpr bool is_basic_string_view_v = is_basic_string_view<T>::విలువ_2;

/** @internal */
మూస<typename CP32, bool skipSurrogates>
class సంకేతం_Points_Iterator {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
బహిరంగ:
    /** C++ iterator boilerplate @internal */
    using విలువ_type = CP32;
    /** C++ iterator boilerplate @internal */
    using reference = విలువ_type;
    /** C++ iterator boilerplate @internal */
    using సూచిక = CP32 *;
    /** C++ iterator boilerplate @internal */
    using difference_type = int32_t;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::forward_iterator_ట్యాగ్;

    /** @internal */
    అంతఃవరుస సంకేతం_Points_Iterator(CP32 c) : c_(c) {}
    /** @internal */
    అంతఃవరుస bool క్రియాకారకం==(స్థిరరాశి సంకేతం_Points_Iterator &other) స్థిరరాశి { తిరిగి c_ == other.c_; }
    /** @internal */
    అంతఃవరుస bool క్రియాకారకం!=(స్థిరరాశి సంకేతం_Points_Iterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }
    /** @internal */
    అంతఃవరుస CP32 క్రియాకారకం*() స్థిరరాశి { తిరిగి c_; }
    /** @internal */
    అంతఃవరుస సంకేతం_Points_Iterator& క్రియాకారకం++() U_LIFETIME_BOUND {  // pre-increment
        ++c_;
        ఒకవేళ (skipSurrogates && c_ == 0xd800) {
            c_ = 0xe000;
        }
        తిరిగి *this;
    }
    /** @internal */
    అంతఃవరుస సంకేతం_Points_Iterator క్రియాకారకం++(పూర్ణాంకం) {  // post-increment
        సంకేతం_Points_Iterator ఫలితం(*this);
        ++(*this);
        తిరిగి ఫలితం;
    }

అంతరంగ:
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
మూస<typename CP32>
class All_సంకేతం_Points {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
బహిరంగ:
    /** Constructor. @draft ICU 78 */
    All_సంకేతం_Points() {}
    /**
     * @return an iterator over all Unicode code points.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    స్వయంచాలక begin() స్థిరరాశి { తిరిగి prv::సంకేతం_Points_Iterator<CP32, false>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode code points.
     * @draft ICU 78
     */
    స్వయంచాలక చివర() స్థిరరాశి { తిరిగి prv::సంకేతం_Points_Iterator<CP32, false>(0x110000); }
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
మూస<typename CP32>
class All_Scalar_విలువలు {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
బహిరంగ:
    /** Constructor. @draft ICU 78 */
    All_Scalar_విలువలు() {}
    /**
     * @return an iterator over all Unicode scalar values.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    స్వయంచాలక begin() స్థిరరాశి { తిరిగి prv::సంకేతం_Points_Iterator<CP32, true>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode scalar values.
     * @draft ICU 78
     */
    స్వయంచాలక చివర() స్థిరరాశి { తిరిగి prv::సంకేతం_Points_Iterator<CP32, true>(0x110000); }
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
మూస<typename CP32, typename UnitIter, typename = శూన్య>
class Unsafe_సంకేతం_Units {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Unit = typename prv::iter_విలువ_t<UnitIter>;
బహిరంగ:
    /** @internal */
    Unsafe_సంకేతం_Units(CP32 సంకేతం_బిందువు, uint8_t నిడివి, UnitIter ప్రారంభం, UnitIter హద్దు) :
            c_(సంకేతం_బిందువు), len_(నిడివి), ప్రారంభం_3(ప్రారంభం), హద్దు_3(హద్దు) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_సంకేతం_Units(స్థిరరాశి Unsafe_సంకేతం_Units &other) = అప్రమేయ;
    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_సంకేతం_Units &క్రియాకారకం=(స్థిరరాశి Unsafe_సంకేతం_Units &other) = అప్రమేయ;

    /**
     * @return the Unicode code point decoded from the code unit sequence.
     *     If the sequence is ill-formed and the iterator validates,
     *     then this is a replacement value according to the iterator‘s
     *     UTFIllFormedBehavior template parameter.
     * @draft ICU 78
     */
    CP32 సంకేతం_బిందువు() స్థిరరాశి { తిరిగి c_; }

    /**
     * @return the start of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter begin() స్థిరరాశి { తిరిగి ప్రారంభం_3; }

    /**
     * @return the limit (exclusive end) of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter చివర() స్థిరరాశి { తిరిగి హద్దు_3; }

    /**
     * @return the length of the code unit sequence for one code point.
     * @draft ICU 78
     */
    uint8_t నిడివి() స్థిరరాశి { తిరిగి len_; }

#ఒకవేళ U_CPLUSPLUS_సంస్కరణ >= 20
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    మూస<std::contiguous_iterator Iter = UnitIter>
    std::basic_string_view<Unit> stringView() స్థిరరాశి {
        తిరిగి std::basic_string_view<Unit>(begin(), చివర());
    }
#లేకపోతే
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    మూస<typename Iter = UnitIter, typename Unit = typename std::iterator_traits<Iter>::విలువ_type>
    std::enable_if_t<std::is_సూచిక_v<Iter> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::const_iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::const_iterator>,
                     std::basic_string_view<Unit>>
    stringView() స్థిరరాశి {
        తిరిగి std::basic_string_view<Unit>(&*ప్రారంభం_3, len_);
    }
#అయితే_ముగింపు

అంతరంగ:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
    UnitIter ప్రారంభం_3;
    UnitIter హద్దు_3;
};

#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
మూస<typename CP32, typename UnitIter>
class Unsafe_సంకేతం_Units<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
బహిరంగ:
    Unsafe_సంకేతం_Units(CP32 సంకేతం_బిందువు, uint8_t నిడివి) : c_(సంకేతం_బిందువు), len_(నిడివి) {}

    Unsafe_సంకేతం_Units(స్థిరరాశి Unsafe_సంకేతం_Units &other) = అప్రమేయ;
    Unsafe_సంకేతం_Units &క్రియాకారకం=(స్థిరరాశి Unsafe_సంకేతం_Units &other) = అప్రమేయ;

    CP32 సంకేతం_బిందువు() స్థిరరాశి { తిరిగి c_; }

    uint8_t నిడివి() స్థిరరాశి { తిరిగి len_; }

అంతరంగ:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
};
#అయితే_ముగింపు  // U_IN_DOXYGEN

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
మూస<typename CP32, typename UnitIter, typename = శూన్య>
class సంకేతం_Units_2 : బహిరంగ Unsafe_సంకేతం_Units<CP32, UnitIter> {
బహిరంగ:
    /** @internal */
    సంకేతం_Units_2(CP32 సంకేతం_బిందువు, uint8_t నిడివి, bool wellFormed, UnitIter ప్రారంభం, UnitIter హద్దు) :
            Unsafe_సంకేతం_Units<CP32, UnitIter>(సంకేతం_బిందువు, నిడివి, ప్రారంభం, హద్దు), ok_(wellFormed) {}

    /** Copy constructor. @draft ICU 78 */
    సంకేతం_Units_2(స్థిరరాశి సంకేతం_Units_2 &other) = అప్రమేయ;
    /** Copy assignment operator. @draft ICU 78 */
    సంకేతం_Units_2 &క్రియాకారకం=(స్థిరరాశి సంకేతం_Units_2 &other) = అప్రమేయ;

    /**
     * @return true if the decoded code unit sequence is well-formed.
     * @draft ICU 78
     */
    bool wellFormed() స్థిరరాశి { తిరిగి ok_; }

అంతరంగ:
    bool ok_;
};

#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
మూస<typename CP32, typename UnitIter>
class సంకేతం_Units_2<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> :
            బహిరంగ Unsafe_సంకేతం_Units<CP32, UnitIter> {
బహిరంగ:
    సంకేతం_Units_2(CP32 సంకేతం_బిందువు, uint8_t నిడివి, bool wellFormed) :
            Unsafe_సంకేతం_Units<CP32, UnitIter>(సంకేతం_బిందువు, నిడివి), ok_(wellFormed) {}

    సంకేతం_Units_2(స్థిరరాశి సంకేతం_Units_2 &other) = అప్రమేయ;
    సంకేతం_Units_2 &క్రియాకారకం=(స్థిరరాశి సంకేతం_Units_2 &other) = అప్రమేయ;

    bool wellFormed() స్థిరరాశి { తిరిగి ok_; }

అంతరంగ:
    bool ok_;
};
#అయితే_ముగింపు  // U_IN_DOXYGEN

// Validating implementations ---------------------------------------------- ***

#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
మూస<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename హద్దు_Iter = UnitIter, typename = శూన్య>
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
మూస<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename హద్దు_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, హద్దు_Iter,
        std::enable_if_t<పరిమాణం(typename prv::iter_విలువ_t<UnitIter>) == 1>> {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    static_assert(behavior != UTF_BEHAVIOR_SURROGATE,
                  "For 8-bit strings, the SURROGATE option does not have an equivalent.");
బహిరంగ:
    // Handle ill-formed UTF-8
    U_FORCE_INLINE స్థిర CP32 sub() {
        ఒకవేళ constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            తిరిగి U_SENTINEL;
        } లేకపోతే {
            static_assert(behavior == UTF_BEHAVIOR_FFFD);
            తిరిగి 0xfffd;
        }
    }

    U_FORCE_INLINE స్థిర శూన్య inc(UnitIter &p, స్థిరరాశి హద్దు_Iter &హద్దు) {
        // Very similar to U8_FWD_1().
        uint8_t b = *p;
        ++p;
        ఒకవేళ (U8_IS_LEAD(b) && p != హద్దు) {
            uint8_t t1 = *p;
            ఒకవేళ ((0xe0 <= b && b < 0xf0)) {
                ఒకవేళ (U8_IS_VALID_LEAD3_AND_T1(b, t1) &&
                        ++p != హద్దు && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            } లేకపోతే ఒకవేళ (b < 0xe0) {
                ఒకవేళ (U8_IS_TRAIL(t1)) {
                    ++p;
                }
            } లేకపోతే /* b >= 0xf0 */ {
                ఒకవేళ (U8_IS_VALID_LEAD4_AND_T1(b, t1) &&
                        ++p != హద్దు && U8_IS_TRAIL(*p) &&
                        ++p != హద్దు && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            }
        }
    }

    U_FORCE_INLINE స్థిర శూన్య dec(UnitIter ప్రారంభం, UnitIter &p) {
        // Very similar to U8_BACK_1().
        uint8_t c = *--p;
        ఒకవేళ (U8_IS_TRAIL(c) && p != ప్రారంభం) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            ఒకవేళ (U8_IS_LEAD(b1)) {
                ఒకవేళ (b1 < 0xe0 ||
                        (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c))) {
                    p = p1;
                    తిరిగి;
                }
            } లేకపోతే ఒకవేళ (U8_IS_TRAIL(b1) && p1 != ప్రారంభం) {
                uint8_t b2 = *--p1;
                ఒకవేళ (0xe0 <= b2 && b2 <= 0xf4) {
                    ఒకవేళ (b2 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b2, b1) :
                            U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        p = p1;
                        తిరిగి;
                    }
                } లేకపోతే ఒకవేళ (U8_IS_TRAIL(b2) && p1 != ప్రారంభం) {
                    uint8_t b3 = *--p1;
                    ఒకవేళ (0xf0 <= b3 && b3 <= 0xf4 && U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                        p = p1;
                        తిరిగి;
                    }
                }
            }
        }
    }

    U_FORCE_INLINE స్థిర సంకేతం_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, స్థిరరాశి హద్దు_Iter &హద్దు) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_OR_FFFD().
        CP32 c = uint8_t(*p);
        ++p;
        ఒకవేళ (U8_IS_SINGLE(c)) {
            ఒకవేళ constexpr (isMultiPass) {
                తిరిగి {c, 1, true, p0, p};
            } లేకపోతే {
                తిరిగి {c, 1, true};
            }
        }
        uint8_t నిడివి = 1;
        uint8_t t = 0;
        ఒకవేళ (p != హద్దు &&
                // fetch/validate/assemble all but last trail byte
                (c >= 0xe0 ?
                    (c < 0xf0 ?  // U+0800..U+FFFF except surrogates
                        U8_LEAD3_T1_బిట్లు[c &= 0xf] & (1 << ((t = *p) >> 5)) &&
                        (t &= 0x3f, 1)
                    :  // U+10000..U+10FFFF
                        (c -= 0xf0) <= 4 &&
                        U8_LEAD4_T1_బిట్లు[(t = *p) >> 4] & (1 << c) &&
                        (c = (c << 6) | (t & 0x3f), ++నిడివి, ++p != హద్దు) &&
                        (t = *p - 0x80) <= 0x3f) &&
                    // valid second-to-last trail byte
                    (c = (c << 6) | t, ++నిడివి, ++p != హద్దు)
                :  // U+0080..U+07FF
                    c >= 0xc2 && (c &= 0x1f, 1)) &&
                // last trail byte
                (t = *p - 0x80) <= 0x3f) {
            c = (c << 6) | t;
            ++నిడివి;
            ++p;
            ఒకవేళ constexpr (isMultiPass) {
                తిరిగి {c, నిడివి, true, p0, p};
            } లేకపోతే {
                తిరిగి {c, నిడివి, true};
            }
        }
        ఒకవేళ constexpr (isMultiPass) {
            తిరిగి {sub(), నిడివి, false, p0, p};
        } లేకపోతే {
            తిరిగి {sub(), నిడివి, false};
        }
    }

    U_FORCE_INLINE స్థిర సంకేతం_Units_2<CP32, UnitIter> decAndRead(UnitIter ప్రారంభం, UnitIter &p) {
        // Very similar to U8_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        ఒకవేళ (U8_IS_SINGLE(c)) {
            తిరిగి {c, 1, true, p, p0};
        }
        ఒకవేళ (U8_IS_TRAIL(c) && p != ప్రారంభం) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            ఒకవేళ (U8_IS_LEAD(b1)) {
                ఒకవేళ (b1 < 0xe0) {
                    p = p1;
                    c = ((b1 - 0xc0) << 6) | (c & 0x3f);
                    తిరిగి {c, 2, true, p, p0};
                } లేకపోతే ఒకవేళ (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c)) {
                    // Truncated 3- or 4-byte sequence.
                    p = p1;
                    తిరిగి {sub(), 2, false, p, p0};
                }
            } లేకపోతే ఒకవేళ (U8_IS_TRAIL(b1) && p1 != ప్రారంభం) {
                // Extract the value bits from the last trail byte.
                c &= 0x3f;
                uint8_t b2 = *--p1;
                ఒకవేళ (0xe0 <= b2 && b2 <= 0xf4) {
                    ఒకవేళ (b2 < 0xf0) {
                        b2 &= 0xf;
                        ఒకవేళ (U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                            p = p1;
                            c = (b2 << 12) | ((b1 & 0x3f) << 6) | c;
                            తిరిగి {c, 3, true, p, p0};
                        }
                    } లేకపోతే ఒకవేళ (U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        // Truncated 4-byte sequence.
                        p = p1;
                        తిరిగి {sub(), 3, false, p, p0};
                    }
                } లేకపోతే ఒకవేళ (U8_IS_TRAIL(b2) && p1 != ప్రారంభం) {
                    uint8_t b3 = *--p1;
                    ఒకవేళ (0xf0 <= b3 && b3 <= 0xf4) {
                        b3 &= 7;
                        ఒకవేళ (U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                            p = p1;
                            c = (b3 << 18) | ((b2 & 0x3f) << 12) | ((b1 & 0x3f) << 6) | c;
                            తిరిగి {c, 4, true, p, p0};
                        }
                    }
                }
            }
        }
        తిరిగి {sub(), 1, false, p, p0};
    }
};

// UTF-16
మూస<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename హద్దు_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, హద్దు_Iter,
        std::enable_if_t<పరిమాణం(typename prv::iter_విలువ_t<UnitIter>) == 2>> {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
బహిరంగ:
    // Handle ill-formed UTF-16: One unpaired surrogate.
    U_FORCE_INLINE స్థిర CP32 sub(CP32 surrogate) {
        ఒకవేళ constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            తిరిగి U_SENTINEL;
        } లేకపోతే ఒకవేళ constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            తిరిగి 0xfffd;
        } లేకపోతే {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            తిరిగి surrogate;
        }
    }

    U_FORCE_INLINE స్థిర శూన్య inc(UnitIter &p, స్థిరరాశి హద్దు_Iter &హద్దు) {
        // Very similar to U16_FWD_1().
        స్వయంచాలక c = *p;
        ++p;
        ఒకవేళ (U16_IS_LEAD(c) && p != హద్దు && U16_IS_TRAIL(*p)) {
            ++p;
        }
    }

    U_FORCE_INLINE స్థిర శూన్య dec(UnitIter ప్రారంభం, UnitIter &p) {
        // Very similar to U16_BACK_1().
        UnitIter p1;
        ఒకవేళ (U16_IS_TRAIL(*--p) && p != ప్రారంభం && (p1 = p, U16_IS_LEAD(*--p1))) {
            p = p1;
        }
    }

    U_FORCE_INLINE స్థిర సంకేతం_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, స్థిరరాశి హద్దు_Iter &హద్దు) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_OR_FFFD().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        ఒకవేళ (!U16_IS_SURROGATE(c)) {
            ఒకవేళ constexpr (isMultiPass) {
                తిరిగి {c, 1, true, p0, p};
            } లేకపోతే {
                తిరిగి {c, 1, true};
            }
        } లేకపోతే {
            uint16_t c2;
            ఒకవేళ (U16_IS_SURROGATE_LEAD(c) && p != హద్దు && U16_IS_TRAIL(c2 = *p)) {
                ++p;
                c = U16_GET_SUPPLEMENTARY(c, c2);
                ఒకవేళ constexpr (isMultiPass) {
                    తిరిగి {c, 2, true, p0, p};
                } లేకపోతే {
                    తిరిగి {c, 2, true};
                }
            } లేకపోతే {
                ఒకవేళ constexpr (isMultiPass) {
                    తిరిగి {sub(c), 1, false, p0, p};
                } లేకపోతే {
                    తిరిగి {sub(c), 1, false};
                }
            }
        }
    }

    U_FORCE_INLINE స్థిర సంకేతం_Units_2<CP32, UnitIter> decAndRead(UnitIter ప్రారంభం, UnitIter &p) {
        // Very similar to U16_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        ఒకవేళ (!U16_IS_SURROGATE(c)) {
            తిరిగి {c, 1, true, p, p0};
        } లేకపోతే {
            UnitIter p1;
            uint16_t c2;
            ఒకవేళ (U16_IS_SURROGATE_TRAIL(c) && p != ప్రారంభం && (p1 = p, U16_IS_LEAD(c2 = *--p1))) {
                p = p1;
                c = U16_GET_SUPPLEMENTARY(c2, c);
                తిరిగి {c, 2, true, p, p0};
            } లేకపోతే {
                తిరిగి {sub(c), 1, false, p, p0};
            }
        }
    }
};

// UTF-32: trivial, but still validating
మూస<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename హద్దు_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, హద్దు_Iter,
        std::enable_if_t<పరిమాణం(typename prv::iter_విలువ_t<UnitIter>) == 4>> {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
బహిరంగ:
    // Handle ill-formed UTF-32
    U_FORCE_INLINE స్థిర CP32 sub(bool forSurrogate, CP32 surrogate) {
        ఒకవేళ constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            తిరిగి U_SENTINEL;
        } లేకపోతే ఒకవేళ constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            తిరిగి 0xfffd;
        } లేకపోతే {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            తిరిగి forSurrogate ? surrogate : 0xfffd;
        }
    }

    U_FORCE_INLINE స్థిర శూన్య inc(UnitIter &p, స్థిరరాశి హద్దు_Iter &/*హద్దు*/) {
        ++p;
    }

    U_FORCE_INLINE స్థిర శూన్య dec(UnitIter /*ప్రారంభం*/, UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE స్థిర సంకేతం_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, స్థిరరాశి హద్దు_Iter &/*హద్దు*/) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        uint32_t uc = *p;
        CP32 c = uc;
        ++p;
        ఒకవేళ (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            ఒకవేళ constexpr (isMultiPass) {
                తిరిగి {c, 1, true, p0, p};
            } లేకపోతే {
                తిరిగి {c, 1, true};
            }
        } లేకపోతే {
            ఒకవేళ constexpr (isMultiPass) {
                తిరిగి {sub(uc < 0xe000, c), 1, false, p0, p};
            } లేకపోతే {
                తిరిగి {sub(uc < 0xe000, c), 1, false};
            }
        }
    }

    U_FORCE_INLINE స్థిర సంకేతం_Units_2<CP32, UnitIter> decAndRead(UnitIter /*ప్రారంభం*/, UnitIter &p) {
        UnitIter p0 = p;
        uint32_t uc = *--p;
        CP32 c = uc;
        ఒకవేళ (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            తిరిగి {c, 1, true, p, p0};
        } లేకపోతే {
            తిరిగి {sub(uc < 0xe000, c), 1, false, p, p0};
        }
    }
};

// Non-validating implementations ------------------------------------------ ***

మూస<typename CP32, typename UnitIter, typename = శూన్య>
class UnsafeUTFImpl;

// UTF-8
మూస<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<పరిమాణం(typename prv::iter_విలువ_t<UnitIter>) == 1>> {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
బహిరంగ:
    U_FORCE_INLINE స్థిర శూన్య inc(UnitIter &p) {
        // Very similar to U8_FWD_1_UNSAFE().
        uint8_t b = *p;
        std::advance(p, 1 + U8_లెక్క_TRAIL_బైట్లు_UNSAFE(b));
    }

    U_FORCE_INLINE స్థిర శూన్య dec(UnitIter &p) {
        // Very similar to U8_BACK_1_UNSAFE().
        అయితే (U8_IS_TRAIL(*--p)) {}
    }

    U_FORCE_INLINE స్థిర Unsafe_సంకేతం_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_UNSAFE().
        CP32 c = uint8_t(*p);
        ++p;
        ఒకవేళ (U8_IS_SINGLE(c)) {
            ఒకవేళ constexpr (isMultiPass) {
                తిరిగి {c, 1, p0, p};
            } లేకపోతే {
                తిరిగి {c, 1};
            }
        } లేకపోతే ఒకవేళ (c < 0xe0) {
            c = ((c & 0x1f) << 6) | (*p & 0x3f);
            ++p;
            ఒకవేళ constexpr (isMultiPass) {
                తిరిగి {c, 2, p0, p};
            } లేకపోతే {
                తిరిగి {c, 2};
            }
        } లేకపోతే ఒకవేళ (c < 0xf0) {
            // No need for (c&0xf) because the upper bits are truncated
            // after <<12 in the cast to uint16_t.
            c = uint16_t(c << 12) | ((*p & 0x3f) << 6);
            ++p;
            c |= *p & 0x3f;
            ++p;
            ఒకవేళ constexpr (isMultiPass) {
                తిరిగి {c, 3, p0, p};
            } లేకపోతే {
                తిరిగి {c, 3};
            }
        } లేకపోతే {
            c = ((c & 7) << 18) | ((*p & 0x3f) << 12);
            ++p;
            c |= (*p & 0x3f) << 6;
            ++p;
            c |= *p & 0x3f;
            ++p;
            ఒకవేళ constexpr (isMultiPass) {
                తిరిగి {c, 4, p0, p};
            } లేకపోతే {
                తిరిగి {c, 4};
            }
        }
    }

    U_FORCE_INLINE స్థిర Unsafe_సంకేతం_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U8_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        ఒకవేళ (U8_IS_SINGLE(c)) {
            తిరిగి {c, 1, p, p0};
        }
        // U8_IS_TRAIL(c) if well-formed
        c &= 0x3f;
        uint8_t లెక్క = 1;
        కొరకు (uint8_t జరుపు = 6;;) {
            uint8_t b = *--p;
            ఒకవేళ (b >= 0xc0) {
                U8_ముసుగు_LEAD_BYTE(b, లెక్క);
                c |= uint32_t{b} << జరుపు;
                ఆపు;
            } లేకపోతే {
                c |= (uint32_t{b} & 0x3f) << జరుపు;
                ++లెక్క;
                జరుపు += 6;
            }
        }
        ++లెక్క;
        తిరిగి {c, లెక్క, p, p0};
    }
};

// UTF-16
మూస<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<పరిమాణం(typename prv::iter_విలువ_t<UnitIter>) == 2>> {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
బహిరంగ:
    U_FORCE_INLINE స్థిర శూన్య inc(UnitIter &p) {
        // Very similar to U16_FWD_1_UNSAFE().
        స్వయంచాలక c = *p;
        ++p;
        ఒకవేళ (U16_IS_LEAD(c)) {
            ++p;
        }
    }

    U_FORCE_INLINE స్థిర శూన్య dec(UnitIter &p) {
        // Very similar to U16_BACK_1_UNSAFE().
        ఒకవేళ (U16_IS_TRAIL(*--p)) {
            --p;
        }
    }

    U_FORCE_INLINE స్థిర Unsafe_సంకేతం_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_UNSAFE().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        ఒకవేళ (!U16_IS_LEAD(c)) {
            ఒకవేళ constexpr (isMultiPass) {
                తిరిగి {c, 1, p0, p};
            } లేకపోతే {
                తిరిగి {c, 1};
            }
        } లేకపోతే {
            uint16_t c2 = *p;
            ++p;
            c = U16_GET_SUPPLEMENTARY(c, c2);
            ఒకవేళ constexpr (isMultiPass) {
                తిరిగి {c, 2, p0, p};
            } లేకపోతే {
                తిరిగి {c, 2};
            }
        }
    }

    U_FORCE_INLINE స్థిర Unsafe_సంకేతం_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U16_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        ఒకవేళ (!U16_IS_TRAIL(c)) {
            తిరిగి {c, 1, p, p0};
        } లేకపోతే {
            uint16_t c2 = *--p;
            c = U16_GET_SUPPLEMENTARY(c2, c);
            తిరిగి {c, 2, p, p0};
        }
    }
};

// UTF-32: trivial
మూస<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<పరిమాణం(typename prv::iter_విలువ_t<UnitIter>) == 4>> {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
బహిరంగ:
    U_FORCE_INLINE స్థిర శూన్య inc(UnitIter &p) {
        ++p;
    }

    U_FORCE_INLINE స్థిర శూన్య dec(UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE స్థిర Unsafe_సంకేతం_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        CP32 c = *p;
        ++p;
        ఒకవేళ constexpr (isMultiPass) {
            తిరిగి {c, 1, p0, p};
        } లేకపోతే {
            తిరిగి {c, 1};
        }
    }

    U_FORCE_INLINE స్థిర Unsafe_సంకేతం_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        UnitIter p0 = p;
        CP32 c = *--p;
        తిరిగి {c, 1, p, p0};
    }
};

#అయితే_ముగింపు

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
మూస<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename హద్దు_Iter = UnitIter, typename = శూన్య>
class UTFIterator {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, హద్దు_Iter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    బహిరంగ:
        explicit Proxy(సంకేతం_Units_2<CP32, UnitIter> &units) : units_(units) {}
        సంకేతం_Units_2<CP32, UnitIter>& క్రియాకారకం*() U_LIFETIME_BOUND { తిరిగి units_; }
        సంకేతం_Units_2<CP32, UnitIter>* క్రియాకారకం->() U_LIFETIME_BOUND { తిరిగి &units_; }
    అంతరంగ:
        సంకేతం_Units_2<CP32, UnitIter> units_;
    };

బహిరంగ:
    /** C++ iterator boilerplate @internal */
    using విలువ_type = సంకేతం_Units_2<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = విలువ_type;
    /** C++ iterator boilerplate @internal */
    using సూచిక = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_ట్యాగ్,
        std::forward_iterator_ట్యాగ్>;

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
    U_FORCE_INLINE UTFIterator(UnitIter ప్రారంభం, UnitIter p, హద్దు_Iter హద్దు) :
            p_(p), ప్రారంభం_3(ప్రారంభం), హద్దు_3(హద్దు), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE UTFIterator(UnitIter p, హద్దు_Iter హద్దు) :
            p_(p), ప్రారంభం_3(p), హద్దు_3(హద్దు), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(p), ప్రారంభం_3(p), హద్దు_3(p), units_(0, 0, false, p, p) {}
    /**
     * Default constructor. Makes a non-functional iterator.
     *
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator() : p_{}, ప్రారంభం_3{}, హద్దు_3{}, units_(0, 0, false, p_, p_) {}

    /** Move constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = అప్రమేయ;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &క్రియాకారకం=(UTFIterator &&src) noexcept = అప్రమేయ;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(స్థిరరాశి UTFIterator &other) = అప్రమేయ;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &క్రియాకారకం=(స్థిరరాశి UTFIterator &other) = అప్రమేయ;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool క్రియాకారకం==(స్థిరరాశి UTFIterator &other) స్థిరరాశి {
        తిరిగి పునాది() == other.పునాది();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool క్రియాకారకం!=(స్థిరరాశి UTFIterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }

    // Asymmetric equality & nonequality with a sentinel type.

    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం==(స్థిరరాశి UTFIterator &iter, స్థిరరాశి Sentinel &s) {
        తిరిగి iter.పునాది() == s;
    }

#ఒకవేళ U_CPLUSPLUS_సంస్కరణ < 20
    // C++17: Need to define all four combinations of == / != vs. parameter order.
    // Once we require C++20, we could remove all but the first == because
    // the compiler would generate the rest.

    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం==(స్థిరరాశి Sentinel &s, స్థిరరాశి UTFIterator &iter) {
        తిరిగి iter.పునాది() == s;
    }
    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం!=(స్థిరరాశి UTFIterator &iter, స్థిరరాశి Sentinel &s) { తిరిగి !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం!=(స్థిరరాశి Sentinel &s, స్థిరరాశి UTFIterator &iter) { తిరిగి !(iter == s); }
#అయితే_ముగింపు  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter పునాది() స్థిరరాశి {
        // Return the logical position.
        తిరిగి state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE సంకేతం_Units_2<CP32, UnitIter> క్రియాకారకం*() స్థిరరాశి {
        ఒకవేళ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, హద్దు_3);
            state_ = 1;
        }
        తిరిగి units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy క్రియాకారకం->() స్థిరరాశి {
        ఒకవేళ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, హద్దు_3);
            state_ = 1;
        }
        తిరిగి Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator& క్రియాకారకం++() U_LIFETIME_BOUND {  // pre-increment
        ఒకవేళ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } లేకపోతే ఒకవేళ (state_ == 0) {
            Impl::inc(p_, హద్దు_3);
        } లేకపోతే /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.చివర();
            state_ = 0;
        }
        తిరిగి *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator క్రియాకారకం++(పూర్ణాంకం) {  // post-increment
        ఒకవేళ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UTFIterator ఫలితం(*this);
            state_ = 0;
            తిరిగి ఫలితం;
        } లేకపోతే ఒకవేళ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, హద్దు_3);
            UTFIterator ఫలితం(*this);
            ఫలితం.state_ = 1;
            // keep this->state_ == 0
            తిరిగి ఫలితం;
        } లేకపోతే /* state_ < 0 */ {
            UTFIterator ఫలితం(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.చివర();
            state_ = 0;
            తిరిగి ఫలితం;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    మూస <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator&>
    క్రియాకారకం--() U_LIFETIME_BOUND {  // pre-decrement
        ఒకవేళ (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(ప్రారంభం_3, p_);
        state_ = -1;
        తిరిగి *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    మూస<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator>
    క్రియాకారకం--(పూర్ణాంకం) {  // post-decrement
        UTFIterator ఫలితం(*this);
        క్రియాకారకం--();
        తిరిగి ఫలితం;
    }

అంతరంగ:
    మిత్ర class std::తిరగేయి_iterator<UTFIterator<CP32, behavior, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter ప్రారంభం_3;
    హద్దు_Iter హద్దు_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable సంకేతం_Units_2<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
// Partial template specialization for single-pass input iterator.
మూస<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename హద్దు_Iter>
class UTFIterator<
        CP32, behavior,
        UnitIter, హద్దు_Iter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, హద్దు_Iter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    బహిరంగ:
        explicit Proxy(సంకేతం_Units_2<CP32, UnitIter> &units) : units_(units) {}
        సంకేతం_Units_2<CP32, UnitIter>& క్రియాకారకం*() U_LIFETIME_BOUND { తిరిగి units_; }
        సంకేతం_Units_2<CP32, UnitIter>* క్రియాకారకం->() U_LIFETIME_BOUND { తిరిగి &units_; }
    అంతరంగ:
        సంకేతం_Units_2<CP32, UnitIter> units_;
    };

బహిరంగ:
    using విలువ_type = సంకేతం_Units_2<CP32, UnitIter>;
    using reference = విలువ_type;
    using సూచిక = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::ఆదానం_iterator_ట్యాగ్;

    U_FORCE_INLINE UTFIterator(UnitIter p, హద్దు_Iter హద్దు) : p_(std::move(p)), హద్దు_3(std::move(హద్దు)) {}

    // Constructs an iterator start or limit sentinel.
    // Requires p to be copyable.
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(std::move(p)), హద్దు_3(p_) {}

    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = అప్రమేయ;
    U_FORCE_INLINE UTFIterator &క్రియాకారకం=(UTFIterator &&src) noexcept = అప్రమేయ;

    U_FORCE_INLINE UTFIterator(స్థిరరాశి UTFIterator &other) = అప్రమేయ;
    U_FORCE_INLINE UTFIterator &క్రియాకారకం=(స్థిరరాశి UTFIterator &other) = అప్రమేయ;

    U_FORCE_INLINE bool క్రియాకారకం==(స్థిరరాశి UTFIterator &other) స్థిరరాశి {
        తిరిగి p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool క్రియాకారకం!=(స్థిరరాశి UTFIterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }

    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం==(స్థిరరాశి UTFIterator &iter, స్థిరరాశి Sentinel &s) {
        తిరిగి !iter.ahead_ && iter.p_ == s;
    }

#ఒకవేళ U_CPLUSPLUS_సంస్కరణ < 20
    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం==(స్థిరరాశి Sentinel &s, స్థిరరాశి UTFIterator &iter) {
        తిరిగి !iter.ahead_ && iter.p_ == s;
    }

    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం!=(స్థిరరాశి UTFIterator &iter, స్థిరరాశి Sentinel &s) { తిరిగి !(iter == s); }

    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం!=(స్థిరరాశి Sentinel &s, స్థిరరాశి UTFIterator &iter) { తిరిగి !(iter == s); }
#అయితే_ముగింపు  // C++17

    U_FORCE_INLINE సంకేతం_Units_2<CP32, UnitIter> క్రియాకారకం*() స్థిరరాశి {
        ఒకవేళ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, హద్దు_3);
            ahead_ = true;
        }
        తిరిగి units_;
    }

    U_FORCE_INLINE Proxy క్రియాకారకం->() స్థిరరాశి {
        ఒకవేళ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, హద్దు_3);
            ahead_ = true;
        }
        తిరిగి Proxy(units_);
    }

    U_FORCE_INLINE UTFIterator& క్రియాకారకం++() U_LIFETIME_BOUND {  // pre-increment
        ఒకవేళ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } లేకపోతే {
            Impl::inc(p_, హద్దు_3);
        }
        తిరిగి *this;
    }

    U_FORCE_INLINE Proxy క్రియాకారకం++(పూర్ణాంకం) {  // post-increment
        ఒకవేళ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } లేకపోతే {
            units_ = Impl::readAndInc(p_, p_, హద్దు_3);
            // keep this->ahead_ == false
        }
        తిరిగి Proxy(units_);
    }

అంతరంగ:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need limit_ so that when we read a code point
    // we can test if there are enough code units.
    హద్దు_Iter హద్దు_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable సంకేతం_Units_2<CP32, UnitIter> units_ = {0, 0, false};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#అయితే_ముగింపు  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
మూస<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
class std::తిరగేయి_iterator<U_శీర్షిక_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>> {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_శీర్షిక_ONLY_NAMESPACE::UTFImpl<CP32, behavior, UnitIter>;
    using సంకేతం_Units_3 = U_శీర్షిక_ONLY_NAMESPACE::సంకేతం_Units_2<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    బహిరంగ:
        explicit Proxy(సంకేతం_Units_3 units) : units_(units) {}
        సంకేతం_Units_3& క్రియాకారకం*() U_LIFETIME_BOUND { తిరిగి units_; }
        సంకేతం_Units_3* క్రియాకారకం->() U_LIFETIME_BOUND { తిరిగి &units_; }
    అంతరంగ:
        సంకేతం_Units_3 units_;
    };

బహిరంగ:
    using విలువ_type = సంకేతం_Units_3;
    using reference = విలువ_type;
    using సూచిక = Proxy;
    using difference_type = U_శీర్షిక_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_ట్యాగ్;

    U_FORCE_INLINE explicit తిరగేయి_iterator(U_శీర్షిక_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> iter) :
            p_(iter.పునాది()), ప్రారంభం_3(iter.ప్రారంభం_3), హద్దు_3(iter.హద్దు_3),
            units_(0, 0, false, p_, p_) {}
    U_FORCE_INLINE తిరగేయి_iterator() : p_{}, ప్రారంభం_3{}, హద్దు_3{}, units_(0, 0, false, p_, p_) {}

    U_FORCE_INLINE తిరగేయి_iterator(తిరగేయి_iterator &&src) noexcept = అప్రమేయ;
    U_FORCE_INLINE తిరగేయి_iterator &క్రియాకారకం=(తిరగేయి_iterator &&src) noexcept = అప్రమేయ;

    U_FORCE_INLINE తిరగేయి_iterator(స్థిరరాశి తిరగేయి_iterator &other) = అప్రమేయ;
    U_FORCE_INLINE తిరగేయి_iterator &క్రియాకారకం=(స్థిరరాశి తిరగేయి_iterator &other) = అప్రమేయ;

    U_FORCE_INLINE bool క్రియాకారకం==(స్థిరరాశి తిరగేయి_iterator &other) స్థిరరాశి {
        తిరిగి getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool క్రియాకారకం!=(స్థిరరాశి తిరగేయి_iterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }

    U_FORCE_INLINE U_శీర్షిక_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> పునాది() స్థిరరాశి {
        తిరిగి U_శీర్షిక_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>(
            ప్రారంభం_3, getLogicalPosition(), హద్దు_3);
    }

    U_FORCE_INLINE సంకేతం_Units_3 క్రియాకారకం*() స్థిరరాశి {
        ఒకవేళ (state_ == 0) {
            units_ = Impl::decAndRead(ప్రారంభం_3, p_);
            state_ = -1;
        }
        తిరిగి units_;
    }

    U_FORCE_INLINE Proxy క్రియాకారకం->() స్థిరరాశి {
        ఒకవేళ (state_ == 0) {
            units_ = Impl::decAndRead(ప్రారంభం_3, p_);
            state_ = -1;
        }
        తిరిగి Proxy(units_);
    }

    U_FORCE_INLINE తిరగేయి_iterator& క్రియాకారకం++() U_LIFETIME_BOUND {  // pre-increment
        ఒకవేళ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } లేకపోతే ఒకవేళ (state_ == 0) {
            Impl::dec(ప్రారంభం_3, p_);
        } లేకపోతే /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        తిరిగి *this;
    }

    U_FORCE_INLINE తిరగేయి_iterator క్రియాకారకం++(పూర్ణాంకం) {  // post-increment
        ఒకవేళ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            తిరగేయి_iterator ఫలితం(*this);
            state_ = 0;
            తిరిగి ఫలితం;
        } లేకపోతే ఒకవేళ (state_ == 0) {
            units_ = Impl::decAndRead(ప్రారంభం_3, p_);
            తిరగేయి_iterator ఫలితం(*this);
            ఫలితం.state_ = -1;
            // keep this->state_ == 0
            తిరిగి ఫలితం;
        } లేకపోతే /* state_ > 0 */ {
            తిరగేయి_iterator ఫలితం(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            తిరిగి ఫలితం;
        }
    }

    U_FORCE_INLINE తిరగేయి_iterator& క్రియాకారకం--() U_LIFETIME_BOUND {  // pre-decrement
        ఒకవేళ (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.చివర();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_, హద్దు_3);
        state_ = 1;
        తిరిగి *this;
    }

    U_FORCE_INLINE తిరగేయి_iterator క్రియాకారకం--(పూర్ణాంకం) {  // post-decrement
        తిరగేయి_iterator ఫలితం(*this);
        క్రియాకారకం--();
        తిరిగి ఫలితం;
    }

అంతరంగ:
    U_FORCE_INLINE UnitIter getLogicalPosition() స్థిరరాశి {
        తిరిగి state_ >= 0 ? p_ : units_.చివర();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter ప్రారంభం_3;
    UnitIter హద్దు_3;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable సంకేతం_Units_3 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#అయితే_ముగింపు  // U_IN_DOXYGEN

namespace U_శీర్షిక_ONLY_NAMESPACE {

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
మూస<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename హద్దు_Iter = UnitIter>
స్వయంచాలక utfIterator(UnitIter ప్రారంభం, UnitIter p, హద్దు_Iter హద్దు) {
    తిరిగి UTFIterator<CP32, behavior, UnitIter, హద్దు_Iter>(
        std::move(ప్రారంభం), std::move(p), std::move(హద్దు));
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
మూస<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename హద్దు_Iter = UnitIter>
స్వయంచాలక utfIterator(UnitIter p, హద్దు_Iter హద్దు) {
    తిరిగి UTFIterator<CP32, behavior, UnitIter, హద్దు_Iter>(
        std::move(p), std::move(హద్దు));
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
మూస<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
స్వయంచాలక utfIterator(UnitIter p) {
    తిరిగి UTFIterator<CP32, behavior, UnitIter>(std::move(p));
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
మూస<typename CP32, UTFIllFormedBehavior behavior, typename Range>
#ఒకవేళ defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#అయితే_ముగింపు
class UTFString_సంకేతం_Points {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
బహిరంగ:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    UTFString_సంకేతం_Points() = అప్రమేయ;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    మూస<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit UTFString_సంకేతం_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C+20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    మూస<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = శూన్య>
    explicit UTFString_సంకేతం_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    UTFString_సంకేతం_Points(స్థిరరాశి UTFString_సంకేతం_Points &other) = అప్రమేయ;

    /** Copy assignment operator. @draft ICU 78 */
    UTFString_సంకేతం_Points &క్రియాకారకం=(స్థిరరాశి UTFString_సంకేతం_Points &other) = అప్రమేయ;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    స్వయంచాలక begin() {
        తిరిగి utfIterator<CP32, behavior>(unitRange.begin(), unitRange.చివర());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    మూస<typename R = Range, typename = std::enable_if_t<prv::range<స్థిరరాశి R>>>
    స్వయంచాలక begin() స్థిరరాశి {
        తిరిగి utfIterator<CP32, behavior>(unitRange.begin(), unitRange.చివర());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    స్వయంచాలక చివర() {
        using UnitIter = decltype(unitRange.begin());
        using హద్దు_Iter = decltype(unitRange.చివర());
        ఒకవేళ constexpr (!std::is_same_v<UnitIter, హద్దు_Iter>) {
            // Return the code unit sentinel.
            తిరిగి unitRange.చివర();
        } లేకపోతే ఒకవేళ constexpr (prv::bidirectional_iterator<UnitIter>) {
            తిరిగి utfIterator<CP32, behavior>(unitRange.begin(), unitRange.చివర(), unitRange.చివర());
        } లేకపోతే {
            // The input iterator specialization has no three-argument constructor.
            తిరిగి utfIterator<CP32, behavior>(unitRange.చివర(), unitRange.చివర());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    మూస<typename R = Range, typename = std::enable_if_t<prv::range<స్థిరరాశి R>>>
    స్వయంచాలక చివర() స్థిరరాశి {
        using UnitIter = decltype(unitRange.begin());
        using హద్దు_Iter = decltype(unitRange.చివర());
        ఒకవేళ constexpr (!std::is_same_v<UnitIter, హద్దు_Iter>) {
            // Return the code unit sentinel.
            తిరిగి unitRange.చివర();
        } లేకపోతే ఒకవేళ constexpr (prv::bidirectional_iterator<UnitIter>) {
            తిరిగి utfIterator<CP32, behavior>(unitRange.begin(), unitRange.చివర(), unitRange.చివర());
        } లేకపోతే {
            // The input iterator specialization has no three-argument constructor.
            తిరిగి utfIterator<CP32, behavior>(unitRange.చివర(), unitRange.చివర());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    స్వయంచాలక rbegin() స్థిరరాశి {
        తిరిగి std::make_తిరగేయి_iterator(చివర());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    స్వయంచాలక rend() స్థిరరాశి {
        తిరిగి std::make_తిరగేయి_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    స్వయంచాలక front() స్థిరరాశి {
        తిరిగి *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    స్వయంచాలక వెనుక() స్థిరరాశి {
        తిరిగి *(--చివర());
    }

అంతరంగ:
    Range unitRange;
};

/** @internal */
మూస<typename CP32, UTFIllFormedBehavior behavior>
నిర్మాణం UTFString_సంకేతం_Points_Adaptor
#ఒకవేళ U_CPLUSPLUS_సంస్కరణ >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_సంవృతం<UTFString_సంకేతం_Points_Adaptor<CP32, behavior>>
#అయితే_ముగింపు
{
    /** @internal */
    మూస<typename Range>
    స్వయంచాలక క్రియాకారకం()(Range &&unitRange) స్థిరరాశి {
#ఒకవేళ defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        తిరిగి UTFString_సంకేతం_Points<CP32, behavior, std::ranges::views::all_t<Range>>(
            std::forward<Range>(unitRange));
#లేకపోతే
        ఒకవేళ constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            తిరిగి UTFString_సంకేతం_Points<CP32, behavior, std::decay_t<Range>>(
                std::forward<Range>(unitRange));
        } లేకపోతే {
            తిరిగి UTFString_సంకేతం_Points<CP32, behavior, Range>(std::forward<Range>(unitRange));
        }
#అయితే_ముగింపు
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
మూస<typename CP32, UTFIllFormedBehavior behavior>
constexpr UTFString_సంకేతం_Points_Adaptor<CP32, behavior> utf_String_సంకేతం_Points;

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
మూస<typename CP32, typename UnitIter, typename = శూన్య>
class UnsafeUTFIterator {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    బహిరంగ:
        explicit Proxy(Unsafe_సంకేతం_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_సంకేతం_Units<CP32, UnitIter>& క్రియాకారకం*() U_LIFETIME_BOUND { తిరిగి units_; }
        Unsafe_సంకేతం_Units<CP32, UnitIter>* క్రియాకారకం->() U_LIFETIME_BOUND { తిరిగి &units_; }
    అంతరంగ:
        Unsafe_సంకేతం_Units<CP32, UnitIter> units_;
    };

బహిరంగ:
    /** C++ iterator boilerplate @internal */
    using విలువ_type = Unsafe_సంకేతం_Units<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = విలువ_type;
    /** C++ iterator boilerplate @internal */
    using సూచిక = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_ట్యాగ్,
        std::forward_iterator_ట్యాగ్>;

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
    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = అప్రమేయ;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &క్రియాకారకం=(UnsafeUTFIterator &&src) noexcept = అప్రమేయ;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator(స్థిరరాశి UnsafeUTFIterator &other) = అప్రమేయ;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &క్రియాకారకం=(స్థిరరాశి UnsafeUTFIterator &other) = అప్రమేయ;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool క్రియాకారకం==(స్థిరరాశి UnsafeUTFIterator &other) స్థిరరాశి {
        తిరిగి పునాది() == other.పునాది();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool క్రియాకారకం!=(స్థిరరాశి UnsafeUTFIterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }

    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం==(స్థిరరాశి UnsafeUTFIterator &iter, స్థిరరాశి Sentinel &s) {
        తిరిగి iter.పునాది() == s;
    }

#ఒకవేళ U_CPLUSPLUS_సంస్కరణ < 20
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం==(స్థిరరాశి Sentinel &s, స్థిరరాశి UnsafeUTFIterator &iter) {
        తిరిగి iter.పునాది() == s;
    }
    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం!=(స్థిరరాశి UnsafeUTFIterator &iter, స్థిరరాశి Sentinel &s) { తిరిగి !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం!=(స్థిరరాశి Sentinel &s, స్థిరరాశి UnsafeUTFIterator &iter) { తిరిగి !(iter == s); }
#అయితే_ముగింపు  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter పునాది() స్థిరరాశి {
        // Return the logical position.
        తిరిగి state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE Unsafe_సంకేతం_Units<CP32, UnitIter> క్రియాకారకం*() స్థిరరాశి {
        ఒకవేళ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        తిరిగి units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy క్రియాకారకం->() స్థిరరాశి {
        ఒకవేళ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        తిరిగి Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator& క్రియాకారకం++() U_LIFETIME_BOUND {  // pre-increment
        ఒకవేళ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } లేకపోతే ఒకవేళ (state_ == 0) {
            Impl::inc(p_);
        } లేకపోతే /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.చివర();
            state_ = 0;
        }
        తిరిగి *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator క్రియాకారకం++(పూర్ణాంకం) {  // post-increment
        ఒకవేళ (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UnsafeUTFIterator ఫలితం(*this);
            state_ = 0;
            తిరిగి ఫలితం;
        } లేకపోతే ఒకవేళ (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            UnsafeUTFIterator ఫలితం(*this);
            ఫలితం.state_ = 1;
            // keep this->state_ == 0
            తిరిగి ఫలితం;
        } లేకపోతే /* state_ < 0 */ {
            UnsafeUTFIterator ఫలితం(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.చివర();
            state_ = 0;
            తిరిగి ఫలితం;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    మూస <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator&>
    క్రియాకారకం--() U_LIFETIME_BOUND {  // pre-decrement
        ఒకవేళ (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(p_);
        state_ = -1;
        తిరిగి *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    మూస<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator>
    క్రియాకారకం--(పూర్ణాంకం) {  // post-decrement
        UnsafeUTFIterator ఫలితం(*this);
        క్రియాకారకం--();
        తిరిగి ఫలితం;
    }

అంతరంగ:
    మిత్ర class std::తిరగేయి_iterator<UnsafeUTFIterator<CP32, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_సంకేతం_Units<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
// Partial template specialization for single-pass input iterator.
మూస<typename CP32, typename UnitIter>
class UnsafeUTFIterator<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    బహిరంగ:
        explicit Proxy(Unsafe_సంకేతం_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_సంకేతం_Units<CP32, UnitIter>& క్రియాకారకం*() U_LIFETIME_BOUND { తిరిగి units_; }
        Unsafe_సంకేతం_Units<CP32, UnitIter>* క్రియాకారకం->() U_LIFETIME_BOUND { తిరిగి &units_; }
    అంతరంగ:
        Unsafe_సంకేతం_Units<CP32, UnitIter> units_;
    };

బహిరంగ:
    using విలువ_type = Unsafe_సంకేతం_Units<CP32, UnitIter>;
    using reference = విలువ_type;
    using సూచిక = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::ఆదానం_iterator_ట్యాగ్;

    U_FORCE_INLINE explicit UnsafeUTFIterator(UnitIter p) : p_(std::move(p)) {}

    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = అప్రమేయ;
    U_FORCE_INLINE UnsafeUTFIterator &క్రియాకారకం=(UnsafeUTFIterator &&src) noexcept = అప్రమేయ;

    U_FORCE_INLINE UnsafeUTFIterator(స్థిరరాశి UnsafeUTFIterator &other) = అప్రమేయ;
    U_FORCE_INLINE UnsafeUTFIterator &క్రియాకారకం=(స్థిరరాశి UnsafeUTFIterator &other) = అప్రమేయ;

    U_FORCE_INLINE bool క్రియాకారకం==(స్థిరరాశి UnsafeUTFIterator &other) స్థిరరాశి {
        తిరిగి p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool క్రియాకారకం!=(స్థిరరాశి UnsafeUTFIterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }

    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం==(స్థిరరాశి UnsafeUTFIterator &iter, స్థిరరాశి Sentinel &s) {
        తిరిగి !iter.ahead_ && iter.p_ == s;
    }

#ఒకవేళ U_CPLUSPLUS_సంస్కరణ < 20
    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం==(స్థిరరాశి Sentinel &s, స్థిరరాశి UnsafeUTFIterator &iter) {
        తిరిగి !iter.ahead_ && iter.p_ == s;
    }

    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం!=(స్థిరరాశి UnsafeUTFIterator &iter, స్థిరరాశి Sentinel &s) { తిరిగి !(iter == s); }

    మూస<typename Sentinel> U_FORCE_INLINE మిత్ర
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    క్రియాకారకం!=(స్థిరరాశి Sentinel &s, స్థిరరాశి UnsafeUTFIterator &iter) { తిరిగి !(iter == s); }
#అయితే_ముగింపు  // C++17

    U_FORCE_INLINE Unsafe_సంకేతం_Units<CP32, UnitIter> క్రియాకారకం*() స్థిరరాశి {
        ఒకవేళ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        తిరిగి units_;
    }

    U_FORCE_INLINE Proxy క్రియాకారకం->() స్థిరరాశి {
        ఒకవేళ (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        తిరిగి Proxy(units_);
    }

    U_FORCE_INLINE UnsafeUTFIterator& క్రియాకారకం++() U_LIFETIME_BOUND {  // pre-increment
        ఒకవేళ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } లేకపోతే {
            Impl::inc(p_);
        }
        తిరిగి *this;
    }

    U_FORCE_INLINE Proxy క్రియాకారకం++(పూర్ణాంకం) {  // post-increment
        ఒకవేళ (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } లేకపోతే {
            units_ = Impl::readAndInc(p_, p_);
            // keep this->ahead_ == false
        }
        తిరిగి Proxy(units_);
    }

అంతరంగ:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable Unsafe_సంకేతం_Units<CP32, UnitIter> units_ = {0, 0};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#అయితే_ముగింపు  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
మూస<typename CP32, typename UnitIter>
class std::తిరగేయి_iterator<U_శీర్షిక_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>> {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_శీర్షిక_ONLY_NAMESPACE::UnsafeUTFImpl<CP32, UnitIter>;
    using Unsafe_సంకేతం_Units_2 = U_శీర్షిక_ONLY_NAMESPACE::Unsafe_సంకేతం_Units<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    బహిరంగ:
        explicit Proxy(Unsafe_సంకేతం_Units_2 units) : units_(units) {}
        Unsafe_సంకేతం_Units_2& క్రియాకారకం*() U_LIFETIME_BOUND { తిరిగి units_; }
        Unsafe_సంకేతం_Units_2* క్రియాకారకం->() U_LIFETIME_BOUND { తిరిగి &units_; }
    అంతరంగ:
        Unsafe_సంకేతం_Units_2 units_;
    };

బహిరంగ:
    using విలువ_type = Unsafe_సంకేతం_Units_2;
    using reference = విలువ_type;
    using సూచిక = Proxy;
    using difference_type = U_శీర్షిక_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_ట్యాగ్;

    U_FORCE_INLINE explicit తిరగేయి_iterator(U_శీర్షిక_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> iter) :
            p_(iter.పునాది()), units_(0, 0, p_, p_) {}
    U_FORCE_INLINE తిరగేయి_iterator() : p_{}, units_(0, 0, p_, p_) {}

    U_FORCE_INLINE తిరగేయి_iterator(తిరగేయి_iterator &&src) noexcept = అప్రమేయ;
    U_FORCE_INLINE తిరగేయి_iterator &క్రియాకారకం=(తిరగేయి_iterator &&src) noexcept = అప్రమేయ;

    U_FORCE_INLINE తిరగేయి_iterator(స్థిరరాశి తిరగేయి_iterator &other) = అప్రమేయ;
    U_FORCE_INLINE తిరగేయి_iterator &క్రియాకారకం=(స్థిరరాశి తిరగేయి_iterator &other) = అప్రమేయ;

    U_FORCE_INLINE bool క్రియాకారకం==(స్థిరరాశి తిరగేయి_iterator &other) స్థిరరాశి {
        తిరిగి getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool క్రియాకారకం!=(స్థిరరాశి తిరగేయి_iterator &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }

    U_FORCE_INLINE U_శీర్షిక_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> పునాది() స్థిరరాశి {
        తిరిగి U_శీర్షిక_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>(
            getLogicalPosition());
    }

    U_FORCE_INLINE Unsafe_సంకేతం_Units_2 క్రియాకారకం*() స్థిరరాశి {
        ఒకవేళ (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        తిరిగి units_;
    }

    U_FORCE_INLINE Proxy క్రియాకారకం->() స్థిరరాశి {
        ఒకవేళ (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        తిరిగి Proxy(units_);
    }

    U_FORCE_INLINE తిరగేయి_iterator& క్రియాకారకం++() U_LIFETIME_BOUND {  // pre-increment
        ఒకవేళ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } లేకపోతే ఒకవేళ (state_ == 0) {
            Impl::dec(p_);
        } లేకపోతే /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        తిరిగి *this;
    }

    U_FORCE_INLINE తిరగేయి_iterator క్రియాకారకం++(పూర్ణాంకం) {  // post-increment
        ఒకవేళ (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            తిరగేయి_iterator ఫలితం(*this);
            state_ = 0;
            తిరిగి ఫలితం;
        } లేకపోతే ఒకవేళ (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            తిరగేయి_iterator ఫలితం(*this);
            ఫలితం.state_ = -1;
            // keep this->state_ == 0
            తిరిగి ఫలితం;
        } లేకపోతే /* state_ > 0 */ {
            తిరగేయి_iterator ఫలితం(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            తిరిగి ఫలితం;
        }
    }

    U_FORCE_INLINE తిరగేయి_iterator& క్రియాకారకం--() U_LIFETIME_BOUND {  // pre-decrement
        ఒకవేళ (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.చివర();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_);
        state_ = 1;
        తిరిగి *this;
    }

    U_FORCE_INLINE తిరగేయి_iterator క్రియాకారకం--(పూర్ణాంకం) {  // post-decrement
        తిరగేయి_iterator ఫలితం(*this);
        క్రియాకారకం--();
        తిరిగి ఫలితం;
    }

అంతరంగ:
    U_FORCE_INLINE UnitIter getLogicalPosition() స్థిరరాశి {
        తిరిగి state_ >= 0 ? p_ : units_.చివర();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_సంకేతం_Units_2 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#అయితే_ముగింపు  // U_IN_DOXYGEN

namespace U_శీర్షిక_ONLY_NAMESPACE {

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
మూస<typename CP32, typename UnitIter>
స్వయంచాలక unsafeUTFIterator(UnitIter iter) {
    తిరిగి UnsafeUTFIterator<CP32, UnitIter>(std::move(iter));
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
మూస<typename CP32, typename Range>
#ఒకవేళ defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#అయితే_ముగింపు
class Unsafe_UTFString_సంకేతం_Points {
    static_assert(పరిమాణం(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
బహిరంగ:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    Unsafe_UTFString_సంకేతం_Points() = అప్రమేయ;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    మూస<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit Unsafe_UTFString_సంకేతం_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C++20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    మూస<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = శూన్య>
    explicit Unsafe_UTFString_సంకేతం_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_UTFString_సంకేతం_Points(స్థిరరాశి Unsafe_UTFString_సంకేతం_Points &other) = అప్రమేయ;

    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_UTFString_సంకేతం_Points &క్రియాకారకం=(స్థిరరాశి Unsafe_UTFString_సంకేతం_Points &other) = అప్రమేయ;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    స్వయంచాలక begin() {
        తిరిగి unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    మూస<typename R = Range, typename = std::enable_if_t<prv::range<స్థిరరాశి R>>>
    స్వయంచాలక begin() స్థిరరాశి {
        తిరిగి unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    స్వయంచాలక చివర() {
        using UnitIter = decltype(unitRange.begin());
        using హద్దు_Iter = decltype(unitRange.చివర());
        ఒకవేళ constexpr (!std::is_same_v<UnitIter, హద్దు_Iter>) {
            // Return the code unit sentinel.
            తిరిగి unitRange.చివర();
        } లేకపోతే {
            తిరిగి unsafeUTFIterator<CP32>(unitRange.చివర());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    మూస<typename R = Range, typename = std::enable_if_t<prv::range<స్థిరరాశి R>>>
    స్వయంచాలక చివర() స్థిరరాశి {
        using UnitIter = decltype(unitRange.begin());
        using హద్దు_Iter = decltype(unitRange.చివర());
        ఒకవేళ constexpr (!std::is_same_v<UnitIter, హద్దు_Iter>) {
            // Return the code unit sentinel.
            తిరిగి unitRange.చివర();
        } లేకపోతే {
            తిరిగి unsafeUTFIterator<CP32>(unitRange.చివర());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    స్వయంచాలక rbegin() స్థిరరాశి {
        తిరిగి std::make_తిరగేయి_iterator(చివర());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    స్వయంచాలక rend() స్థిరరాశి {
        తిరిగి std::make_తిరగేయి_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    స్వయంచాలక front() స్థిరరాశి {
        తిరిగి *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    స్వయంచాలక వెనుక() స్థిరరాశి {
        తిరిగి *(--చివర());
    }

అంతరంగ:
    Range unitRange;
};

/** @internal */
మూస<typename CP32>
నిర్మాణం Unsafe_UTFString_సంకేతం_Points_Adaptor
#ఒకవేళ U_CPLUSPLUS_సంస్కరణ >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_సంవృతం<Unsafe_UTFString_సంకేతం_Points_Adaptor<CP32>>
#అయితే_ముగింపు
{
    /** @internal */
    మూస<typename Range>
    స్వయంచాలక క్రియాకారకం()(Range &&unitRange) స్థిరరాశి {
#ఒకవేళ defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        తిరిగి Unsafe_UTFString_సంకేతం_Points<CP32, std::ranges::views::all_t<Range>>(std::forward<Range>(unitRange));
#లేకపోతే
        ఒకవేళ constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            తిరిగి Unsafe_UTFString_సంకేతం_Points<CP32, std::decay_t<Range>>(std::forward<Range>(unitRange));
        } లేకపోతే {
            తిరిగి Unsafe_UTFString_సంకేతం_Points<CP32, Range>(std::forward<Range>(unitRange));
        }
#అయితే_ముగింపు
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
మూస<typename CP32>
constexpr Unsafe_UTFString_సంకేతం_Points_Adaptor<CP32> unsafe_UTFString_సంకేతం_Points;

}  // namespace U_HEADER_ONLY_NAMESPACE


#ఒకవేళ defined(__cpp_lib_ranges)
మూస <typename CP32, UTFIllFormedBehavior behavior, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_శీర్షిక_ONLY_NAMESPACE::UTFString_సంకేతం_Points<CP32, behavior, Range>> =
    std::ranges::enable_borrowed_range<Range>;

మూస <typename CP32, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_శీర్షిక_ONLY_NAMESPACE::Unsafe_UTFString_సంకేతం_Points<CP32, Range>> =
    std::ranges::enable_borrowed_range<Range>;
#అయితే_ముగింపు

#అయితే_ముగింపు  // U_HIDE_DRAFT_API
#అయితే_ముగింపు  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#అయితే_ముగింపు  // __UTFITERATOR_H__
