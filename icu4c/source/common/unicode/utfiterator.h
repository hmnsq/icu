// © 2024 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfiterator.h
// created: 2024aug12 Markus W. Scherer

#جيڪڏهن_وصف_نه_ٿيل __UTFITERATOR_H__
#وصف_ڪريو __UTFITERATOR_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_سرو_API || !defined(UTYPES_H)

#ضمن <iterator>
#إذا __has_include(<نسخو>)
#ضمن <version>
#جيڪڏهن_پورو
#إذا defined(__cpp_lib_ranges)
#ضمن <ranges>
#جيڪڏهن_پورو
#ضمن <string>
#ضمن <string_view>
#ضمن <type_traits>
#ضمن "unicode/utf16.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/uversion.h"

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

#جيڪڏهن_وصف_نه_ٿيل U_HIDE_DRAFT_API

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
تعريفنوع تعداد UTFIllFormedBehavior {
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

namespace U_سرو_ONLY_NAMESPACE {

namespace prv {
#إذا U_CPLUSPLUS_نسخو >= 20

/** @internal */
سانچو<typename Iter>
using iter_قدر_t = typename std::iter_قدر_t<Iter>;

/** @internal */
سانچو<typename Iter>
using iter_difference_t = std::iter_difference_t<Iter>;

/** @internal */
سانچو<typename Iter>
constexpr bool forward_iterator = std::forward_iterator<Iter>;

/** @internal */
سانچو<typename Iter>
constexpr bool bidirectional_iterator = std::bidirectional_iterator<Iter>;

/** @internal */
سانچو<typename Range>
constexpr bool range = std::ranges::range<Range>;

#وإلا

/** @internal */
سانچو<typename Iter>
using iter_قدر_t = typename std::iterator_traits<Iter>::قدر_type;

/** @internal */
سانچو<typename Iter>
using iter_difference_t = typename std::iterator_traits<Iter>::difference_type;

/** @internal */
سانچو<typename Iter>
constexpr bool forward_iterator =
    std::is_بنياد_جو_v<
        std::forward_iterator_ٽيگ,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
سانچو<typename Iter>
constexpr bool bidirectional_iterator =
    std::is_بنياد_جو_v<
        std::bidirectional_iterator_ٽيگ,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
سانچو<typename Range, typename = فراغ>
بنية range_type : std::false_type {};

/** @internal */
سانچو<typename Range>
بنية range_type<
    Range,
    std::void_t<decltype(std::declval<Range>().begin()),
                decltype(std::declval<Range>().پڇاڙي())>> : std::true_type {};

/** @internal */
سانچو<typename Range>
constexpr bool range = range_type<Range>::قدر_2;

#جيڪڏهن_پورو

/** @internal */
سانچو <typename T> بنية is_basic_string_view : std::false_type {};

/** @internal */
سانچو <typename... دليلون>
بنية is_basic_string_view<std::basic_string_view<دليلون...>> : std::true_type {};

/** @internal */
سانچو <typename T> constexpr bool is_basic_string_view_v = is_basic_string_view<T>::قدر_2;

/** @internal */
سانچو<typename CP32, bool skipSurrogates>
class ڪوڊ_Points_Iterator {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    /** C++ iterator boilerplate @internal */
    using قدر_type = CP32;
    /** C++ iterator boilerplate @internal */
    using reference = قدر_type;
    /** C++ iterator boilerplate @internal */
    using اشارو = CP32 *;
    /** C++ iterator boilerplate @internal */
    using difference_type = int32_t;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::forward_iterator_ٽيگ;

    /** @internal */
    مضمن ڪوڊ_Points_Iterator(CP32 c) : c_(c) {}
    /** @internal */
    مضمن bool عامل==(ثابت ڪوڊ_Points_Iterator &other) ثابت { أرجع c_ == other.c_; }
    /** @internal */
    مضمن bool عامل!=(ثابت ڪوڊ_Points_Iterator &other) ثابت { أرجع !عامل==(other); }
    /** @internal */
    مضمن CP32 عامل*() ثابت { أرجع c_; }
    /** @internal */
    مضمن ڪوڊ_Points_Iterator& عامل++() U_LIFETIME_BOUND {  // pre-increment
        ++c_;
        إذا (skipSurrogates && c_ == 0xd800) {
            c_ = 0xe000;
        }
        أرجع *this;
    }
    /** @internal */
    مضمن ڪوڊ_Points_Iterator عامل++(صحيح) {  // post-increment
        ڪوڊ_Points_Iterator نتيجو(*this);
        ++(*this);
        أرجع نتيجو;
    }

خاص:
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
سانچو<typename CP32>
class All_ڪوڊ_Points {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    /** Constructor. @draft ICU 78 */
    All_ڪوڊ_Points() {}
    /**
     * @return an iterator over all Unicode code points.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    خودڪار begin() ثابت { أرجع prv::ڪوڊ_Points_Iterator<CP32, false>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode code points.
     * @draft ICU 78
     */
    خودڪار پڇاڙي() ثابت { أرجع prv::ڪوڊ_Points_Iterator<CP32, false>(0x110000); }
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
سانچو<typename CP32>
class All_Scalar_قدرون {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    /** Constructor. @draft ICU 78 */
    All_Scalar_قدرون() {}
    /**
     * @return an iterator over all Unicode scalar values.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    خودڪار begin() ثابت { أرجع prv::ڪوڊ_Points_Iterator<CP32, true>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode scalar values.
     * @draft ICU 78
     */
    خودڪار پڇاڙي() ثابت { أرجع prv::ڪوڊ_Points_Iterator<CP32, true>(0x110000); }
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
سانچو<typename CP32, typename UnitIter, typename = فراغ>
class Unsafe_ڪوڊ_Units {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Unit = typename prv::iter_قدر_t<UnitIter>;
عام:
    /** @internal */
    Unsafe_ڪوڊ_Units(CP32 ڪوڊ_نقطو, uint8_t ڊيگهائي, UnitIter شروعات, UnitIter حد_بندي) :
            c_(ڪوڊ_نقطو), len_(ڊيگهائي), شروعات_3(شروعات), حد_بندي_3(حد_بندي) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_ڪوڊ_Units(ثابت Unsafe_ڪوڊ_Units &other) = افتراضي;
    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_ڪوڊ_Units &عامل=(ثابت Unsafe_ڪوڊ_Units &other) = افتراضي;

    /**
     * @return the Unicode code point decoded from the code unit sequence.
     *     If the sequence is ill-formed and the iterator validates,
     *     then this is a replacement value according to the iterator‘s
     *     UTFIllFormedBehavior template parameter.
     * @draft ICU 78
     */
    CP32 ڪوڊ_نقطو() ثابت { أرجع c_; }

    /**
     * @return the start of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter begin() ثابت { أرجع شروعات_3; }

    /**
     * @return the limit (exclusive end) of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter پڇاڙي() ثابت { أرجع حد_بندي_3; }

    /**
     * @return the length of the code unit sequence for one code point.
     * @draft ICU 78
     */
    uint8_t ڊيگهائي() ثابت { أرجع len_; }

#إذا U_CPLUSPLUS_نسخو >= 20
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    سانچو<std::contiguous_iterator Iter = UnitIter>
    std::basic_string_view<Unit> stringView() ثابت {
        أرجع std::basic_string_view<Unit>(begin(), پڇاڙي());
    }
#وإلا
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    سانچو<typename Iter = UnitIter, typename Unit = typename std::iterator_traits<Iter>::قدر_type>
    std::enable_if_t<std::is_اشارو_v<Iter> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::const_iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::const_iterator>,
                     std::basic_string_view<Unit>>
    stringView() ثابت {
        أرجع std::basic_string_view<Unit>(&*شروعات_3, len_);
    }
#جيڪڏهن_پورو

خاص:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
    UnitIter شروعات_3;
    UnitIter حد_بندي_3;
};

#جيڪڏهن_وصف_نه_ٿيل U_اندريون_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
سانچو<typename CP32, typename UnitIter>
class Unsafe_ڪوڊ_Units<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    Unsafe_ڪوڊ_Units(CP32 ڪوڊ_نقطو, uint8_t ڊيگهائي) : c_(ڪوڊ_نقطو), len_(ڊيگهائي) {}

    Unsafe_ڪوڊ_Units(ثابت Unsafe_ڪوڊ_Units &other) = افتراضي;
    Unsafe_ڪوڊ_Units &عامل=(ثابت Unsafe_ڪوڊ_Units &other) = افتراضي;

    CP32 ڪوڊ_نقطو() ثابت { أرجع c_; }

    uint8_t ڊيگهائي() ثابت { أرجع len_; }

خاص:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
};
#جيڪڏهن_پورو  // U_IN_DOXYGEN

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
سانچو<typename CP32, typename UnitIter, typename = فراغ>
class ڪوڊ_Units_2 : عام Unsafe_ڪوڊ_Units<CP32, UnitIter> {
عام:
    /** @internal */
    ڪوڊ_Units_2(CP32 ڪوڊ_نقطو, uint8_t ڊيگهائي, bool wellFormed, UnitIter شروعات, UnitIter حد_بندي) :
            Unsafe_ڪوڊ_Units<CP32, UnitIter>(ڪوڊ_نقطو, ڊيگهائي, شروعات, حد_بندي), ok_(wellFormed) {}

    /** Copy constructor. @draft ICU 78 */
    ڪوڊ_Units_2(ثابت ڪوڊ_Units_2 &other) = افتراضي;
    /** Copy assignment operator. @draft ICU 78 */
    ڪوڊ_Units_2 &عامل=(ثابت ڪوڊ_Units_2 &other) = افتراضي;

    /**
     * @return true if the decoded code unit sequence is well-formed.
     * @draft ICU 78
     */
    bool wellFormed() ثابت { أرجع ok_; }

خاص:
    bool ok_;
};

#جيڪڏهن_وصف_نه_ٿيل U_اندريون_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
سانچو<typename CP32, typename UnitIter>
class ڪوڊ_Units_2<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> :
            عام Unsafe_ڪوڊ_Units<CP32, UnitIter> {
عام:
    ڪوڊ_Units_2(CP32 ڪوڊ_نقطو, uint8_t ڊيگهائي, bool wellFormed) :
            Unsafe_ڪوڊ_Units<CP32, UnitIter>(ڪوڊ_نقطو, ڊيگهائي), ok_(wellFormed) {}

    ڪوڊ_Units_2(ثابت ڪوڊ_Units_2 &other) = افتراضي;
    ڪوڊ_Units_2 &عامل=(ثابت ڪوڊ_Units_2 &other) = افتراضي;

    bool wellFormed() ثابت { أرجع ok_; }

خاص:
    bool ok_;
};
#جيڪڏهن_پورو  // U_IN_DOXYGEN

// Validating implementations ---------------------------------------------- ***

#جيڪڏهن_وصف_نه_ٿيل U_اندريون_DOXYGEN
سانچو<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename حد_بندي_Iter = UnitIter, typename = فراغ>
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
سانچو<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename حد_بندي_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, حد_بندي_Iter,
        std::enable_if_t<حجم(typename prv::iter_قدر_t<UnitIter>) == 1>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    static_assert(behavior != UTF_BEHAVIOR_SURROGATE,
                  "For 8-bit strings, the SURROGATE option does not have an equivalent.");
عام:
    // Handle ill-formed UTF-8
    U_FORCE_INLINE ساكن CP32 sub() {
        إذا constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            أرجع U_SENTINEL;
        } وإلا {
            static_assert(behavior == UTF_BEHAVIOR_FFFD);
            أرجع 0xfffd;
        }
    }

    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p, ثابت حد_بندي_Iter &حد_بندي) {
        // Very similar to U8_FWD_1().
        uint8_t b = *p;
        ++p;
        إذا (U8_IS_LEAD(b) && p != حد_بندي) {
            uint8_t t1 = *p;
            إذا ((0xe0 <= b && b < 0xf0)) {
                إذا (U8_IS_VALID_LEAD3_AND_T1(b, t1) &&
                        ++p != حد_بندي && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            } وإلا إذا (b < 0xe0) {
                إذا (U8_IS_TRAIL(t1)) {
                    ++p;
                }
            } وإلا /* b >= 0xf0 */ {
                إذا (U8_IS_VALID_LEAD4_AND_T1(b, t1) &&
                        ++p != حد_بندي && U8_IS_TRAIL(*p) &&
                        ++p != حد_بندي && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            }
        }
    }

    U_FORCE_INLINE ساكن فراغ dec(UnitIter شروعات, UnitIter &p) {
        // Very similar to U8_BACK_1().
        uint8_t c = *--p;
        إذا (U8_IS_TRAIL(c) && p != شروعات) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            إذا (U8_IS_LEAD(b1)) {
                إذا (b1 < 0xe0 ||
                        (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c))) {
                    p = p1;
                    أرجع;
                }
            } وإلا إذا (U8_IS_TRAIL(b1) && p1 != شروعات) {
                uint8_t b2 = *--p1;
                إذا (0xe0 <= b2 && b2 <= 0xf4) {
                    إذا (b2 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b2, b1) :
                            U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        p = p1;
                        أرجع;
                    }
                } وإلا إذا (U8_IS_TRAIL(b2) && p1 != شروعات) {
                    uint8_t b3 = *--p1;
                    إذا (0xf0 <= b3 && b3 <= 0xf4 && U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                        p = p1;
                        أرجع;
                    }
                }
            }
        }
    }

    U_FORCE_INLINE ساكن ڪوڊ_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ثابت حد_بندي_Iter &حد_بندي) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_OR_FFFD().
        CP32 c = uint8_t(*p);
        ++p;
        إذا (U8_IS_SINGLE(c)) {
            إذا constexpr (isMultiPass) {
                أرجع {c, 1, true, p0, p};
            } وإلا {
                أرجع {c, 1, true};
            }
        }
        uint8_t ڊيگهائي = 1;
        uint8_t t = 0;
        إذا (p != حد_بندي &&
                // fetch/validate/assemble all but last trail byte
                (c >= 0xe0 ?
                    (c < 0xf0 ?  // U+0800..U+FFFF except surrogates
                        U8_LEAD3_T1_بٽ[c &= 0xf] & (1 << ((t = *p) >> 5)) &&
                        (t &= 0x3f, 1)
                    :  // U+10000..U+10FFFF
                        (c -= 0xf0) <= 4 &&
                        U8_LEAD4_T1_بٽ[(t = *p) >> 4] & (1 << c) &&
                        (c = (c << 6) | (t & 0x3f), ++ڊيگهائي, ++p != حد_بندي) &&
                        (t = *p - 0x80) <= 0x3f) &&
                    // valid second-to-last trail byte
                    (c = (c << 6) | t, ++ڊيگهائي, ++p != حد_بندي)
                :  // U+0080..U+07FF
                    c >= 0xc2 && (c &= 0x1f, 1)) &&
                // last trail byte
                (t = *p - 0x80) <= 0x3f) {
            c = (c << 6) | t;
            ++ڊيگهائي;
            ++p;
            إذا constexpr (isMultiPass) {
                أرجع {c, ڊيگهائي, true, p0, p};
            } وإلا {
                أرجع {c, ڊيگهائي, true};
            }
        }
        إذا constexpr (isMultiPass) {
            أرجع {sub(), ڊيگهائي, false, p0, p};
        } وإلا {
            أرجع {sub(), ڊيگهائي, false};
        }
    }

    U_FORCE_INLINE ساكن ڪوڊ_Units_2<CP32, UnitIter> decAndRead(UnitIter شروعات, UnitIter &p) {
        // Very similar to U8_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        إذا (U8_IS_SINGLE(c)) {
            أرجع {c, 1, true, p, p0};
        }
        إذا (U8_IS_TRAIL(c) && p != شروعات) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            إذا (U8_IS_LEAD(b1)) {
                إذا (b1 < 0xe0) {
                    p = p1;
                    c = ((b1 - 0xc0) << 6) | (c & 0x3f);
                    أرجع {c, 2, true, p, p0};
                } وإلا إذا (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c)) {
                    // Truncated 3- or 4-byte sequence.
                    p = p1;
                    أرجع {sub(), 2, false, p, p0};
                }
            } وإلا إذا (U8_IS_TRAIL(b1) && p1 != شروعات) {
                // Extract the value bits from the last trail byte.
                c &= 0x3f;
                uint8_t b2 = *--p1;
                إذا (0xe0 <= b2 && b2 <= 0xf4) {
                    إذا (b2 < 0xf0) {
                        b2 &= 0xf;
                        إذا (U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                            p = p1;
                            c = (b2 << 12) | ((b1 & 0x3f) << 6) | c;
                            أرجع {c, 3, true, p, p0};
                        }
                    } وإلا إذا (U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        // Truncated 4-byte sequence.
                        p = p1;
                        أرجع {sub(), 3, false, p, p0};
                    }
                } وإلا إذا (U8_IS_TRAIL(b2) && p1 != شروعات) {
                    uint8_t b3 = *--p1;
                    إذا (0xf0 <= b3 && b3 <= 0xf4) {
                        b3 &= 7;
                        إذا (U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                            p = p1;
                            c = (b3 << 18) | ((b2 & 0x3f) << 12) | ((b1 & 0x3f) << 6) | c;
                            أرجع {c, 4, true, p, p0};
                        }
                    }
                }
            }
        }
        أرجع {sub(), 1, false, p, p0};
    }
};

// UTF-16
سانچو<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename حد_بندي_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, حد_بندي_Iter,
        std::enable_if_t<حجم(typename prv::iter_قدر_t<UnitIter>) == 2>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    // Handle ill-formed UTF-16: One unpaired surrogate.
    U_FORCE_INLINE ساكن CP32 sub(CP32 surrogate) {
        إذا constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            أرجع U_SENTINEL;
        } وإلا إذا constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            أرجع 0xfffd;
        } وإلا {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            أرجع surrogate;
        }
    }

    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p, ثابت حد_بندي_Iter &حد_بندي) {
        // Very similar to U16_FWD_1().
        خودڪار c = *p;
        ++p;
        إذا (U16_IS_LEAD(c) && p != حد_بندي && U16_IS_TRAIL(*p)) {
            ++p;
        }
    }

    U_FORCE_INLINE ساكن فراغ dec(UnitIter شروعات, UnitIter &p) {
        // Very similar to U16_BACK_1().
        UnitIter p1;
        إذا (U16_IS_TRAIL(*--p) && p != شروعات && (p1 = p, U16_IS_LEAD(*--p1))) {
            p = p1;
        }
    }

    U_FORCE_INLINE ساكن ڪوڊ_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ثابت حد_بندي_Iter &حد_بندي) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_OR_FFFD().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        إذا (!U16_IS_SURROGATE(c)) {
            إذا constexpr (isMultiPass) {
                أرجع {c, 1, true, p0, p};
            } وإلا {
                أرجع {c, 1, true};
            }
        } وإلا {
            uint16_t c2;
            إذا (U16_IS_SURROGATE_LEAD(c) && p != حد_بندي && U16_IS_TRAIL(c2 = *p)) {
                ++p;
                c = U16_GET_SUPPLEMENTARY(c, c2);
                إذا constexpr (isMultiPass) {
                    أرجع {c, 2, true, p0, p};
                } وإلا {
                    أرجع {c, 2, true};
                }
            } وإلا {
                إذا constexpr (isMultiPass) {
                    أرجع {sub(c), 1, false, p0, p};
                } وإلا {
                    أرجع {sub(c), 1, false};
                }
            }
        }
    }

    U_FORCE_INLINE ساكن ڪوڊ_Units_2<CP32, UnitIter> decAndRead(UnitIter شروعات, UnitIter &p) {
        // Very similar to U16_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        إذا (!U16_IS_SURROGATE(c)) {
            أرجع {c, 1, true, p, p0};
        } وإلا {
            UnitIter p1;
            uint16_t c2;
            إذا (U16_IS_SURROGATE_TRAIL(c) && p != شروعات && (p1 = p, U16_IS_LEAD(c2 = *--p1))) {
                p = p1;
                c = U16_GET_SUPPLEMENTARY(c2, c);
                أرجع {c, 2, true, p, p0};
            } وإلا {
                أرجع {sub(c), 1, false, p, p0};
            }
        }
    }
};

// UTF-32: trivial, but still validating
سانچو<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename حد_بندي_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, حد_بندي_Iter,
        std::enable_if_t<حجم(typename prv::iter_قدر_t<UnitIter>) == 4>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    // Handle ill-formed UTF-32
    U_FORCE_INLINE ساكن CP32 sub(bool forSurrogate, CP32 surrogate) {
        إذا constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            أرجع U_SENTINEL;
        } وإلا إذا constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            أرجع 0xfffd;
        } وإلا {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            أرجع forSurrogate ? surrogate : 0xfffd;
        }
    }

    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p, ثابت حد_بندي_Iter &/*حد_بندي*/) {
        ++p;
    }

    U_FORCE_INLINE ساكن فراغ dec(UnitIter /*شروعات*/, UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE ساكن ڪوڊ_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ثابت حد_بندي_Iter &/*حد_بندي*/) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        uint32_t uc = *p;
        CP32 c = uc;
        ++p;
        إذا (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            إذا constexpr (isMultiPass) {
                أرجع {c, 1, true, p0, p};
            } وإلا {
                أرجع {c, 1, true};
            }
        } وإلا {
            إذا constexpr (isMultiPass) {
                أرجع {sub(uc < 0xe000, c), 1, false, p0, p};
            } وإلا {
                أرجع {sub(uc < 0xe000, c), 1, false};
            }
        }
    }

    U_FORCE_INLINE ساكن ڪوڊ_Units_2<CP32, UnitIter> decAndRead(UnitIter /*شروعات*/, UnitIter &p) {
        UnitIter p0 = p;
        uint32_t uc = *--p;
        CP32 c = uc;
        إذا (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            أرجع {c, 1, true, p, p0};
        } وإلا {
            أرجع {sub(uc < 0xe000, c), 1, false, p, p0};
        }
    }
};

// Non-validating implementations ------------------------------------------ ***

سانچو<typename CP32, typename UnitIter, typename = فراغ>
class UnsafeUTFImpl;

// UTF-8
سانچو<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<حجم(typename prv::iter_قدر_t<UnitIter>) == 1>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p) {
        // Very similar to U8_FWD_1_UNSAFE().
        uint8_t b = *p;
        std::advance(p, 1 + U8_ڳڻپ_TRAIL_بائيٽون_UNSAFE(b));
    }

    U_FORCE_INLINE ساكن فراغ dec(UnitIter &p) {
        // Very similar to U8_BACK_1_UNSAFE().
        طالما (U8_IS_TRAIL(*--p)) {}
    }

    U_FORCE_INLINE ساكن Unsafe_ڪوڊ_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_UNSAFE().
        CP32 c = uint8_t(*p);
        ++p;
        إذا (U8_IS_SINGLE(c)) {
            إذا constexpr (isMultiPass) {
                أرجع {c, 1, p0, p};
            } وإلا {
                أرجع {c, 1};
            }
        } وإلا إذا (c < 0xe0) {
            c = ((c & 0x1f) << 6) | (*p & 0x3f);
            ++p;
            إذا constexpr (isMultiPass) {
                أرجع {c, 2, p0, p};
            } وإلا {
                أرجع {c, 2};
            }
        } وإلا إذا (c < 0xf0) {
            // No need for (c&0xf) because the upper bits are truncated
            // after <<12 in the cast to uint16_t.
            c = uint16_t(c << 12) | ((*p & 0x3f) << 6);
            ++p;
            c |= *p & 0x3f;
            ++p;
            إذا constexpr (isMultiPass) {
                أرجع {c, 3, p0, p};
            } وإلا {
                أرجع {c, 3};
            }
        } وإلا {
            c = ((c & 7) << 18) | ((*p & 0x3f) << 12);
            ++p;
            c |= (*p & 0x3f) << 6;
            ++p;
            c |= *p & 0x3f;
            ++p;
            إذا constexpr (isMultiPass) {
                أرجع {c, 4, p0, p};
            } وإلا {
                أرجع {c, 4};
            }
        }
    }

    U_FORCE_INLINE ساكن Unsafe_ڪوڊ_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U8_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        إذا (U8_IS_SINGLE(c)) {
            أرجع {c, 1, p, p0};
        }
        // U8_IS_TRAIL(c) if well-formed
        c &= 0x3f;
        uint8_t ڳڻپ = 1;
        لكل (uint8_t سرڪايو = 6;;) {
            uint8_t b = *--p;
            إذا (b >= 0xc0) {
                U8_نقاب_LEAD_BYTE(b, ڳڻپ);
                c |= uint32_t{b} << سرڪايو;
                اكسر;
            } وإلا {
                c |= (uint32_t{b} & 0x3f) << سرڪايو;
                ++ڳڻپ;
                سرڪايو += 6;
            }
        }
        ++ڳڻپ;
        أرجع {c, ڳڻپ, p, p0};
    }
};

// UTF-16
سانچو<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<حجم(typename prv::iter_قدر_t<UnitIter>) == 2>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p) {
        // Very similar to U16_FWD_1_UNSAFE().
        خودڪار c = *p;
        ++p;
        إذا (U16_IS_LEAD(c)) {
            ++p;
        }
    }

    U_FORCE_INLINE ساكن فراغ dec(UnitIter &p) {
        // Very similar to U16_BACK_1_UNSAFE().
        إذا (U16_IS_TRAIL(*--p)) {
            --p;
        }
    }

    U_FORCE_INLINE ساكن Unsafe_ڪوڊ_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_UNSAFE().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        إذا (!U16_IS_LEAD(c)) {
            إذا constexpr (isMultiPass) {
                أرجع {c, 1, p0, p};
            } وإلا {
                أرجع {c, 1};
            }
        } وإلا {
            uint16_t c2 = *p;
            ++p;
            c = U16_GET_SUPPLEMENTARY(c, c2);
            إذا constexpr (isMultiPass) {
                أرجع {c, 2, p0, p};
            } وإلا {
                أرجع {c, 2};
            }
        }
    }

    U_FORCE_INLINE ساكن Unsafe_ڪوڊ_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U16_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        إذا (!U16_IS_TRAIL(c)) {
            أرجع {c, 1, p, p0};
        } وإلا {
            uint16_t c2 = *--p;
            c = U16_GET_SUPPLEMENTARY(c2, c);
            أرجع {c, 2, p, p0};
        }
    }
};

// UTF-32: trivial
سانچو<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<حجم(typename prv::iter_قدر_t<UnitIter>) == 4>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p) {
        ++p;
    }

    U_FORCE_INLINE ساكن فراغ dec(UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE ساكن Unsafe_ڪوڊ_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        CP32 c = *p;
        ++p;
        إذا constexpr (isMultiPass) {
            أرجع {c, 1, p0, p};
        } وإلا {
            أرجع {c, 1};
        }
    }

    U_FORCE_INLINE ساكن Unsafe_ڪوڊ_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        UnitIter p0 = p;
        CP32 c = *--p;
        أرجع {c, 1, p, p0};
    }
};

#جيڪڏهن_پورو

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
سانچو<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename حد_بندي_Iter = UnitIter, typename = فراغ>
class UTFIterator {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, حد_بندي_Iter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    عام:
        explicit Proxy(ڪوڊ_Units_2<CP32, UnitIter> &units) : units_(units) {}
        ڪوڊ_Units_2<CP32, UnitIter>& عامل*() U_LIFETIME_BOUND { أرجع units_; }
        ڪوڊ_Units_2<CP32, UnitIter>* عامل->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        ڪوڊ_Units_2<CP32, UnitIter> units_;
    };

عام:
    /** C++ iterator boilerplate @internal */
    using قدر_type = ڪوڊ_Units_2<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = قدر_type;
    /** C++ iterator boilerplate @internal */
    using اشارو = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_ٽيگ,
        std::forward_iterator_ٽيگ>;

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
    U_FORCE_INLINE UTFIterator(UnitIter شروعات, UnitIter p, حد_بندي_Iter حد_بندي) :
            p_(p), شروعات_3(شروعات), حد_بندي_3(حد_بندي), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE UTFIterator(UnitIter p, حد_بندي_Iter حد_بندي) :
            p_(p), شروعات_3(p), حد_بندي_3(حد_بندي), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(p), شروعات_3(p), حد_بندي_3(p), units_(0, 0, false, p, p) {}
    /**
     * Default constructor. Makes a non-functional iterator.
     *
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator() : p_{}, شروعات_3{}, حد_بندي_3{}, units_(0, 0, false, p_, p_) {}

    /** Move constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = افتراضي;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &عامل=(UTFIterator &&src) noexcept = افتراضي;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(ثابت UTFIterator &other) = افتراضي;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &عامل=(ثابت UTFIterator &other) = افتراضي;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool عامل==(ثابت UTFIterator &other) ثابت {
        أرجع بنياد() == other.بنياد();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool عامل!=(ثابت UTFIterator &other) ثابت { أرجع !عامل==(other); }

    // Asymmetric equality & nonequality with a sentinel type.

    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل==(ثابت UTFIterator &iter, ثابت Sentinel &s) {
        أرجع iter.بنياد() == s;
    }

#إذا U_CPLUSPLUS_نسخو < 20
    // C++17: Need to define all four combinations of == / != vs. parameter order.
    // Once we require C++20, we could remove all but the first == because
    // the compiler would generate the rest.

    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل==(ثابت Sentinel &s, ثابت UTFIterator &iter) {
        أرجع iter.بنياد() == s;
    }
    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل!=(ثابت UTFIterator &iter, ثابت Sentinel &s) { أرجع !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل!=(ثابت Sentinel &s, ثابت UTFIterator &iter) { أرجع !(iter == s); }
#جيڪڏهن_پورو  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter بنياد() ثابت {
        // Return the logical position.
        أرجع state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE ڪوڊ_Units_2<CP32, UnitIter> عامل*() ثابت {
        إذا (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, حد_بندي_3);
            state_ = 1;
        }
        أرجع units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy عامل->() ثابت {
        إذا (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, حد_بندي_3);
            state_ = 1;
        }
        أرجع Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator& عامل++() U_LIFETIME_BOUND {  // pre-increment
        إذا (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } وإلا إذا (state_ == 0) {
            Impl::inc(p_, حد_بندي_3);
        } وإلا /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.پڇاڙي();
            state_ = 0;
        }
        أرجع *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator عامل++(صحيح) {  // post-increment
        إذا (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UTFIterator نتيجو(*this);
            state_ = 0;
            أرجع نتيجو;
        } وإلا إذا (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, حد_بندي_3);
            UTFIterator نتيجو(*this);
            نتيجو.state_ = 1;
            // keep this->state_ == 0
            أرجع نتيجو;
        } وإلا /* state_ < 0 */ {
            UTFIterator نتيجو(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.پڇاڙي();
            state_ = 0;
            أرجع نتيجو;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    سانچو <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator&>
    عامل--() U_LIFETIME_BOUND {  // pre-decrement
        إذا (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(شروعات_3, p_);
        state_ = -1;
        أرجع *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    سانچو<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator>
    عامل--(صحيح) {  // post-decrement
        UTFIterator نتيجو(*this);
        عامل--();
        أرجع نتيجو;
    }

خاص:
    دوست class std::ابتو_iterator<UTFIterator<CP32, behavior, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter شروعات_3;
    حد_بندي_Iter حد_بندي_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable ڪوڊ_Units_2<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#جيڪڏهن_وصف_نه_ٿيل U_اندريون_DOXYGEN
// Partial template specialization for single-pass input iterator.
سانچو<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename حد_بندي_Iter>
class UTFIterator<
        CP32, behavior,
        UnitIter, حد_بندي_Iter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, حد_بندي_Iter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    عام:
        explicit Proxy(ڪوڊ_Units_2<CP32, UnitIter> &units) : units_(units) {}
        ڪوڊ_Units_2<CP32, UnitIter>& عامل*() U_LIFETIME_BOUND { أرجع units_; }
        ڪوڊ_Units_2<CP32, UnitIter>* عامل->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        ڪوڊ_Units_2<CP32, UnitIter> units_;
    };

عام:
    using قدر_type = ڪوڊ_Units_2<CP32, UnitIter>;
    using reference = قدر_type;
    using اشارو = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::داخلا_iterator_ٽيگ;

    U_FORCE_INLINE UTFIterator(UnitIter p, حد_بندي_Iter حد_بندي) : p_(std::move(p)), حد_بندي_3(std::move(حد_بندي)) {}

    // Constructs an iterator start or limit sentinel.
    // Requires p to be copyable.
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(std::move(p)), حد_بندي_3(p_) {}

    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = افتراضي;
    U_FORCE_INLINE UTFIterator &عامل=(UTFIterator &&src) noexcept = افتراضي;

    U_FORCE_INLINE UTFIterator(ثابت UTFIterator &other) = افتراضي;
    U_FORCE_INLINE UTFIterator &عامل=(ثابت UTFIterator &other) = افتراضي;

    U_FORCE_INLINE bool عامل==(ثابت UTFIterator &other) ثابت {
        أرجع p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool عامل!=(ثابت UTFIterator &other) ثابت { أرجع !عامل==(other); }

    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل==(ثابت UTFIterator &iter, ثابت Sentinel &s) {
        أرجع !iter.ahead_ && iter.p_ == s;
    }

#إذا U_CPLUSPLUS_نسخو < 20
    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل==(ثابت Sentinel &s, ثابت UTFIterator &iter) {
        أرجع !iter.ahead_ && iter.p_ == s;
    }

    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل!=(ثابت UTFIterator &iter, ثابت Sentinel &s) { أرجع !(iter == s); }

    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل!=(ثابت Sentinel &s, ثابت UTFIterator &iter) { أرجع !(iter == s); }
#جيڪڏهن_پورو  // C++17

    U_FORCE_INLINE ڪوڊ_Units_2<CP32, UnitIter> عامل*() ثابت {
        إذا (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, حد_بندي_3);
            ahead_ = true;
        }
        أرجع units_;
    }

    U_FORCE_INLINE Proxy عامل->() ثابت {
        إذا (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, حد_بندي_3);
            ahead_ = true;
        }
        أرجع Proxy(units_);
    }

    U_FORCE_INLINE UTFIterator& عامل++() U_LIFETIME_BOUND {  // pre-increment
        إذا (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } وإلا {
            Impl::inc(p_, حد_بندي_3);
        }
        أرجع *this;
    }

    U_FORCE_INLINE Proxy عامل++(صحيح) {  // post-increment
        إذا (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } وإلا {
            units_ = Impl::readAndInc(p_, p_, حد_بندي_3);
            // keep this->ahead_ == false
        }
        أرجع Proxy(units_);
    }

خاص:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need limit_ so that when we read a code point
    // we can test if there are enough code units.
    حد_بندي_Iter حد_بندي_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable ڪوڊ_Units_2<CP32, UnitIter> units_ = {0, 0, false};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#جيڪڏهن_پورو  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#جيڪڏهن_وصف_نه_ٿيل U_اندريون_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
سانچو<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
class std::ابتو_iterator<U_سرو_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_سرو_ONLY_NAMESPACE::UTFImpl<CP32, behavior, UnitIter>;
    using ڪوڊ_Units_3 = U_سرو_ONLY_NAMESPACE::ڪوڊ_Units_2<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    عام:
        explicit Proxy(ڪوڊ_Units_3 units) : units_(units) {}
        ڪوڊ_Units_3& عامل*() U_LIFETIME_BOUND { أرجع units_; }
        ڪوڊ_Units_3* عامل->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        ڪوڊ_Units_3 units_;
    };

عام:
    using قدر_type = ڪوڊ_Units_3;
    using reference = قدر_type;
    using اشارو = Proxy;
    using difference_type = U_سرو_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_ٽيگ;

    U_FORCE_INLINE explicit ابتو_iterator(U_سرو_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> iter) :
            p_(iter.بنياد()), شروعات_3(iter.شروعات_3), حد_بندي_3(iter.حد_بندي_3),
            units_(0, 0, false, p_, p_) {}
    U_FORCE_INLINE ابتو_iterator() : p_{}, شروعات_3{}, حد_بندي_3{}, units_(0, 0, false, p_, p_) {}

    U_FORCE_INLINE ابتو_iterator(ابتو_iterator &&src) noexcept = افتراضي;
    U_FORCE_INLINE ابتو_iterator &عامل=(ابتو_iterator &&src) noexcept = افتراضي;

    U_FORCE_INLINE ابتو_iterator(ثابت ابتو_iterator &other) = افتراضي;
    U_FORCE_INLINE ابتو_iterator &عامل=(ثابت ابتو_iterator &other) = افتراضي;

    U_FORCE_INLINE bool عامل==(ثابت ابتو_iterator &other) ثابت {
        أرجع getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool عامل!=(ثابت ابتو_iterator &other) ثابت { أرجع !عامل==(other); }

    U_FORCE_INLINE U_سرو_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> بنياد() ثابت {
        أرجع U_سرو_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>(
            شروعات_3, getLogicalPosition(), حد_بندي_3);
    }

    U_FORCE_INLINE ڪوڊ_Units_3 عامل*() ثابت {
        إذا (state_ == 0) {
            units_ = Impl::decAndRead(شروعات_3, p_);
            state_ = -1;
        }
        أرجع units_;
    }

    U_FORCE_INLINE Proxy عامل->() ثابت {
        إذا (state_ == 0) {
            units_ = Impl::decAndRead(شروعات_3, p_);
            state_ = -1;
        }
        أرجع Proxy(units_);
    }

    U_FORCE_INLINE ابتو_iterator& عامل++() U_LIFETIME_BOUND {  // pre-increment
        إذا (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } وإلا إذا (state_ == 0) {
            Impl::dec(شروعات_3, p_);
        } وإلا /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        أرجع *this;
    }

    U_FORCE_INLINE ابتو_iterator عامل++(صحيح) {  // post-increment
        إذا (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            ابتو_iterator نتيجو(*this);
            state_ = 0;
            أرجع نتيجو;
        } وإلا إذا (state_ == 0) {
            units_ = Impl::decAndRead(شروعات_3, p_);
            ابتو_iterator نتيجو(*this);
            نتيجو.state_ = -1;
            // keep this->state_ == 0
            أرجع نتيجو;
        } وإلا /* state_ > 0 */ {
            ابتو_iterator نتيجو(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            أرجع نتيجو;
        }
    }

    U_FORCE_INLINE ابتو_iterator& عامل--() U_LIFETIME_BOUND {  // pre-decrement
        إذا (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.پڇاڙي();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_, حد_بندي_3);
        state_ = 1;
        أرجع *this;
    }

    U_FORCE_INLINE ابتو_iterator عامل--(صحيح) {  // post-decrement
        ابتو_iterator نتيجو(*this);
        عامل--();
        أرجع نتيجو;
    }

خاص:
    U_FORCE_INLINE UnitIter getLogicalPosition() ثابت {
        أرجع state_ >= 0 ? p_ : units_.پڇاڙي();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter شروعات_3;
    UnitIter حد_بندي_3;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable ڪوڊ_Units_3 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#جيڪڏهن_پورو  // U_IN_DOXYGEN

namespace U_سرو_ONLY_NAMESPACE {

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
سانچو<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename حد_بندي_Iter = UnitIter>
خودڪار utfIterator(UnitIter شروعات, UnitIter p, حد_بندي_Iter حد_بندي) {
    أرجع UTFIterator<CP32, behavior, UnitIter, حد_بندي_Iter>(
        std::move(شروعات), std::move(p), std::move(حد_بندي));
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
سانچو<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename حد_بندي_Iter = UnitIter>
خودڪار utfIterator(UnitIter p, حد_بندي_Iter حد_بندي) {
    أرجع UTFIterator<CP32, behavior, UnitIter, حد_بندي_Iter>(
        std::move(p), std::move(حد_بندي));
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
سانچو<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
خودڪار utfIterator(UnitIter p) {
    أرجع UTFIterator<CP32, behavior, UnitIter>(std::move(p));
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
سانچو<typename CP32, UTFIllFormedBehavior behavior, typename Range>
#إذا defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#جيڪڏهن_پورو
class UTFString_ڪوڊ_Points {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    UTFString_ڪوڊ_Points() = افتراضي;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    سانچو<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit UTFString_ڪوڊ_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C+20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    سانچو<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = فراغ>
    explicit UTFString_ڪوڊ_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    UTFString_ڪوڊ_Points(ثابت UTFString_ڪوڊ_Points &other) = افتراضي;

    /** Copy assignment operator. @draft ICU 78 */
    UTFString_ڪوڊ_Points &عامل=(ثابت UTFString_ڪوڊ_Points &other) = افتراضي;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    خودڪار begin() {
        أرجع utfIterator<CP32, behavior>(unitRange.begin(), unitRange.پڇاڙي());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    سانچو<typename R = Range, typename = std::enable_if_t<prv::range<ثابت R>>>
    خودڪار begin() ثابت {
        أرجع utfIterator<CP32, behavior>(unitRange.begin(), unitRange.پڇاڙي());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    خودڪار پڇاڙي() {
        using UnitIter = decltype(unitRange.begin());
        using حد_بندي_Iter = decltype(unitRange.پڇاڙي());
        إذا constexpr (!std::is_same_v<UnitIter, حد_بندي_Iter>) {
            // Return the code unit sentinel.
            أرجع unitRange.پڇاڙي();
        } وإلا إذا constexpr (prv::bidirectional_iterator<UnitIter>) {
            أرجع utfIterator<CP32, behavior>(unitRange.begin(), unitRange.پڇاڙي(), unitRange.پڇاڙي());
        } وإلا {
            // The input iterator specialization has no three-argument constructor.
            أرجع utfIterator<CP32, behavior>(unitRange.پڇاڙي(), unitRange.پڇاڙي());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    سانچو<typename R = Range, typename = std::enable_if_t<prv::range<ثابت R>>>
    خودڪار پڇاڙي() ثابت {
        using UnitIter = decltype(unitRange.begin());
        using حد_بندي_Iter = decltype(unitRange.پڇاڙي());
        إذا constexpr (!std::is_same_v<UnitIter, حد_بندي_Iter>) {
            // Return the code unit sentinel.
            أرجع unitRange.پڇاڙي();
        } وإلا إذا constexpr (prv::bidirectional_iterator<UnitIter>) {
            أرجع utfIterator<CP32, behavior>(unitRange.begin(), unitRange.پڇاڙي(), unitRange.پڇاڙي());
        } وإلا {
            // The input iterator specialization has no three-argument constructor.
            أرجع utfIterator<CP32, behavior>(unitRange.پڇاڙي(), unitRange.پڇاڙي());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    خودڪار rbegin() ثابت {
        أرجع std::make_ابتو_iterator(پڇاڙي());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    خودڪار rend() ثابت {
        أرجع std::make_ابتو_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    خودڪار front() ثابت {
        أرجع *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    خودڪار پٺ() ثابت {
        أرجع *(--پڇاڙي());
    }

خاص:
    Range unitRange;
};

/** @internal */
سانچو<typename CP32, UTFIllFormedBehavior behavior>
بنية UTFString_ڪوڊ_Points_Adaptor
#إذا U_CPLUSPLUS_نسخو >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_گهيرو<UTFString_ڪوڊ_Points_Adaptor<CP32, behavior>>
#جيڪڏهن_پورو
{
    /** @internal */
    سانچو<typename Range>
    خودڪار عامل()(Range &&unitRange) ثابت {
#إذا defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        أرجع UTFString_ڪوڊ_Points<CP32, behavior, std::ranges::views::all_t<Range>>(
            std::forward<Range>(unitRange));
#وإلا
        إذا constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            أرجع UTFString_ڪوڊ_Points<CP32, behavior, std::decay_t<Range>>(
                std::forward<Range>(unitRange));
        } وإلا {
            أرجع UTFString_ڪوڊ_Points<CP32, behavior, Range>(std::forward<Range>(unitRange));
        }
#جيڪڏهن_پورو
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
سانچو<typename CP32, UTFIllFormedBehavior behavior>
constexpr UTFString_ڪوڊ_Points_Adaptor<CP32, behavior> utf_String_ڪوڊ_Points;

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
سانچو<typename CP32, typename UnitIter, typename = فراغ>
class UnsafeUTFIterator {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    عام:
        explicit Proxy(Unsafe_ڪوڊ_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_ڪوڊ_Units<CP32, UnitIter>& عامل*() U_LIFETIME_BOUND { أرجع units_; }
        Unsafe_ڪوڊ_Units<CP32, UnitIter>* عامل->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        Unsafe_ڪوڊ_Units<CP32, UnitIter> units_;
    };

عام:
    /** C++ iterator boilerplate @internal */
    using قدر_type = Unsafe_ڪوڊ_Units<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = قدر_type;
    /** C++ iterator boilerplate @internal */
    using اشارو = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_ٽيگ,
        std::forward_iterator_ٽيگ>;

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
    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = افتراضي;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &عامل=(UnsafeUTFIterator &&src) noexcept = افتراضي;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator(ثابت UnsafeUTFIterator &other) = افتراضي;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &عامل=(ثابت UnsafeUTFIterator &other) = افتراضي;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool عامل==(ثابت UnsafeUTFIterator &other) ثابت {
        أرجع بنياد() == other.بنياد();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool عامل!=(ثابت UnsafeUTFIterator &other) ثابت { أرجع !عامل==(other); }

    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل==(ثابت UnsafeUTFIterator &iter, ثابت Sentinel &s) {
        أرجع iter.بنياد() == s;
    }

#إذا U_CPLUSPLUS_نسخو < 20
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل==(ثابت Sentinel &s, ثابت UnsafeUTFIterator &iter) {
        أرجع iter.بنياد() == s;
    }
    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل!=(ثابت UnsafeUTFIterator &iter, ثابت Sentinel &s) { أرجع !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل!=(ثابت Sentinel &s, ثابت UnsafeUTFIterator &iter) { أرجع !(iter == s); }
#جيڪڏهن_پورو  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter بنياد() ثابت {
        // Return the logical position.
        أرجع state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE Unsafe_ڪوڊ_Units<CP32, UnitIter> عامل*() ثابت {
        إذا (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        أرجع units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy عامل->() ثابت {
        إذا (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        أرجع Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator& عامل++() U_LIFETIME_BOUND {  // pre-increment
        إذا (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } وإلا إذا (state_ == 0) {
            Impl::inc(p_);
        } وإلا /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.پڇاڙي();
            state_ = 0;
        }
        أرجع *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator عامل++(صحيح) {  // post-increment
        إذا (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UnsafeUTFIterator نتيجو(*this);
            state_ = 0;
            أرجع نتيجو;
        } وإلا إذا (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            UnsafeUTFIterator نتيجو(*this);
            نتيجو.state_ = 1;
            // keep this->state_ == 0
            أرجع نتيجو;
        } وإلا /* state_ < 0 */ {
            UnsafeUTFIterator نتيجو(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.پڇاڙي();
            state_ = 0;
            أرجع نتيجو;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    سانچو <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator&>
    عامل--() U_LIFETIME_BOUND {  // pre-decrement
        إذا (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(p_);
        state_ = -1;
        أرجع *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    سانچو<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator>
    عامل--(صحيح) {  // post-decrement
        UnsafeUTFIterator نتيجو(*this);
        عامل--();
        أرجع نتيجو;
    }

خاص:
    دوست class std::ابتو_iterator<UnsafeUTFIterator<CP32, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_ڪوڊ_Units<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#جيڪڏهن_وصف_نه_ٿيل U_اندريون_DOXYGEN
// Partial template specialization for single-pass input iterator.
سانچو<typename CP32, typename UnitIter>
class UnsafeUTFIterator<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    عام:
        explicit Proxy(Unsafe_ڪوڊ_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_ڪوڊ_Units<CP32, UnitIter>& عامل*() U_LIFETIME_BOUND { أرجع units_; }
        Unsafe_ڪوڊ_Units<CP32, UnitIter>* عامل->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        Unsafe_ڪوڊ_Units<CP32, UnitIter> units_;
    };

عام:
    using قدر_type = Unsafe_ڪوڊ_Units<CP32, UnitIter>;
    using reference = قدر_type;
    using اشارو = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::داخلا_iterator_ٽيگ;

    U_FORCE_INLINE explicit UnsafeUTFIterator(UnitIter p) : p_(std::move(p)) {}

    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = افتراضي;
    U_FORCE_INLINE UnsafeUTFIterator &عامل=(UnsafeUTFIterator &&src) noexcept = افتراضي;

    U_FORCE_INLINE UnsafeUTFIterator(ثابت UnsafeUTFIterator &other) = افتراضي;
    U_FORCE_INLINE UnsafeUTFIterator &عامل=(ثابت UnsafeUTFIterator &other) = افتراضي;

    U_FORCE_INLINE bool عامل==(ثابت UnsafeUTFIterator &other) ثابت {
        أرجع p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool عامل!=(ثابت UnsafeUTFIterator &other) ثابت { أرجع !عامل==(other); }

    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل==(ثابت UnsafeUTFIterator &iter, ثابت Sentinel &s) {
        أرجع !iter.ahead_ && iter.p_ == s;
    }

#إذا U_CPLUSPLUS_نسخو < 20
    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل==(ثابت Sentinel &s, ثابت UnsafeUTFIterator &iter) {
        أرجع !iter.ahead_ && iter.p_ == s;
    }

    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل!=(ثابت UnsafeUTFIterator &iter, ثابت Sentinel &s) { أرجع !(iter == s); }

    سانچو<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عامل!=(ثابت Sentinel &s, ثابت UnsafeUTFIterator &iter) { أرجع !(iter == s); }
#جيڪڏهن_پورو  // C++17

    U_FORCE_INLINE Unsafe_ڪوڊ_Units<CP32, UnitIter> عامل*() ثابت {
        إذا (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        أرجع units_;
    }

    U_FORCE_INLINE Proxy عامل->() ثابت {
        إذا (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        أرجع Proxy(units_);
    }

    U_FORCE_INLINE UnsafeUTFIterator& عامل++() U_LIFETIME_BOUND {  // pre-increment
        إذا (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } وإلا {
            Impl::inc(p_);
        }
        أرجع *this;
    }

    U_FORCE_INLINE Proxy عامل++(صحيح) {  // post-increment
        إذا (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } وإلا {
            units_ = Impl::readAndInc(p_, p_);
            // keep this->ahead_ == false
        }
        أرجع Proxy(units_);
    }

خاص:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable Unsafe_ڪوڊ_Units<CP32, UnitIter> units_ = {0, 0};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#جيڪڏهن_پورو  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#جيڪڏهن_وصف_نه_ٿيل U_اندريون_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
سانچو<typename CP32, typename UnitIter>
class std::ابتو_iterator<U_سرو_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_سرو_ONLY_NAMESPACE::UnsafeUTFImpl<CP32, UnitIter>;
    using Unsafe_ڪوڊ_Units_2 = U_سرو_ONLY_NAMESPACE::Unsafe_ڪوڊ_Units<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    عام:
        explicit Proxy(Unsafe_ڪوڊ_Units_2 units) : units_(units) {}
        Unsafe_ڪوڊ_Units_2& عامل*() U_LIFETIME_BOUND { أرجع units_; }
        Unsafe_ڪوڊ_Units_2* عامل->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        Unsafe_ڪوڊ_Units_2 units_;
    };

عام:
    using قدر_type = Unsafe_ڪوڊ_Units_2;
    using reference = قدر_type;
    using اشارو = Proxy;
    using difference_type = U_سرو_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_ٽيگ;

    U_FORCE_INLINE explicit ابتو_iterator(U_سرو_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> iter) :
            p_(iter.بنياد()), units_(0, 0, p_, p_) {}
    U_FORCE_INLINE ابتو_iterator() : p_{}, units_(0, 0, p_, p_) {}

    U_FORCE_INLINE ابتو_iterator(ابتو_iterator &&src) noexcept = افتراضي;
    U_FORCE_INLINE ابتو_iterator &عامل=(ابتو_iterator &&src) noexcept = افتراضي;

    U_FORCE_INLINE ابتو_iterator(ثابت ابتو_iterator &other) = افتراضي;
    U_FORCE_INLINE ابتو_iterator &عامل=(ثابت ابتو_iterator &other) = افتراضي;

    U_FORCE_INLINE bool عامل==(ثابت ابتو_iterator &other) ثابت {
        أرجع getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool عامل!=(ثابت ابتو_iterator &other) ثابت { أرجع !عامل==(other); }

    U_FORCE_INLINE U_سرو_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> بنياد() ثابت {
        أرجع U_سرو_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>(
            getLogicalPosition());
    }

    U_FORCE_INLINE Unsafe_ڪوڊ_Units_2 عامل*() ثابت {
        إذا (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        أرجع units_;
    }

    U_FORCE_INLINE Proxy عامل->() ثابت {
        إذا (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        أرجع Proxy(units_);
    }

    U_FORCE_INLINE ابتو_iterator& عامل++() U_LIFETIME_BOUND {  // pre-increment
        إذا (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } وإلا إذا (state_ == 0) {
            Impl::dec(p_);
        } وإلا /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        أرجع *this;
    }

    U_FORCE_INLINE ابتو_iterator عامل++(صحيح) {  // post-increment
        إذا (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            ابتو_iterator نتيجو(*this);
            state_ = 0;
            أرجع نتيجو;
        } وإلا إذا (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            ابتو_iterator نتيجو(*this);
            نتيجو.state_ = -1;
            // keep this->state_ == 0
            أرجع نتيجو;
        } وإلا /* state_ > 0 */ {
            ابتو_iterator نتيجو(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            أرجع نتيجو;
        }
    }

    U_FORCE_INLINE ابتو_iterator& عامل--() U_LIFETIME_BOUND {  // pre-decrement
        إذا (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.پڇاڙي();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_);
        state_ = 1;
        أرجع *this;
    }

    U_FORCE_INLINE ابتو_iterator عامل--(صحيح) {  // post-decrement
        ابتو_iterator نتيجو(*this);
        عامل--();
        أرجع نتيجو;
    }

خاص:
    U_FORCE_INLINE UnitIter getLogicalPosition() ثابت {
        أرجع state_ >= 0 ? p_ : units_.پڇاڙي();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_ڪوڊ_Units_2 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#جيڪڏهن_پورو  // U_IN_DOXYGEN

namespace U_سرو_ONLY_NAMESPACE {

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
سانچو<typename CP32, typename UnitIter>
خودڪار unsafeUTFIterator(UnitIter iter) {
    أرجع UnsafeUTFIterator<CP32, UnitIter>(std::move(iter));
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
سانچو<typename CP32, typename Range>
#إذا defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#جيڪڏهن_پورو
class Unsafe_UTFString_ڪوڊ_Points {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    Unsafe_UTFString_ڪوڊ_Points() = افتراضي;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    سانچو<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit Unsafe_UTFString_ڪوڊ_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C++20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    سانچو<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = فراغ>
    explicit Unsafe_UTFString_ڪوڊ_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_UTFString_ڪوڊ_Points(ثابت Unsafe_UTFString_ڪوڊ_Points &other) = افتراضي;

    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_UTFString_ڪوڊ_Points &عامل=(ثابت Unsafe_UTFString_ڪوڊ_Points &other) = افتراضي;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    خودڪار begin() {
        أرجع unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    سانچو<typename R = Range, typename = std::enable_if_t<prv::range<ثابت R>>>
    خودڪار begin() ثابت {
        أرجع unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    خودڪار پڇاڙي() {
        using UnitIter = decltype(unitRange.begin());
        using حد_بندي_Iter = decltype(unitRange.پڇاڙي());
        إذا constexpr (!std::is_same_v<UnitIter, حد_بندي_Iter>) {
            // Return the code unit sentinel.
            أرجع unitRange.پڇاڙي();
        } وإلا {
            أرجع unsafeUTFIterator<CP32>(unitRange.پڇاڙي());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    سانچو<typename R = Range, typename = std::enable_if_t<prv::range<ثابت R>>>
    خودڪار پڇاڙي() ثابت {
        using UnitIter = decltype(unitRange.begin());
        using حد_بندي_Iter = decltype(unitRange.پڇاڙي());
        إذا constexpr (!std::is_same_v<UnitIter, حد_بندي_Iter>) {
            // Return the code unit sentinel.
            أرجع unitRange.پڇاڙي();
        } وإلا {
            أرجع unsafeUTFIterator<CP32>(unitRange.پڇاڙي());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    خودڪار rbegin() ثابت {
        أرجع std::make_ابتو_iterator(پڇاڙي());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    خودڪار rend() ثابت {
        أرجع std::make_ابتو_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    خودڪار front() ثابت {
        أرجع *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    خودڪار پٺ() ثابت {
        أرجع *(--پڇاڙي());
    }

خاص:
    Range unitRange;
};

/** @internal */
سانچو<typename CP32>
بنية Unsafe_UTFString_ڪوڊ_Points_Adaptor
#إذا U_CPLUSPLUS_نسخو >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_گهيرو<Unsafe_UTFString_ڪوڊ_Points_Adaptor<CP32>>
#جيڪڏهن_پورو
{
    /** @internal */
    سانچو<typename Range>
    خودڪار عامل()(Range &&unitRange) ثابت {
#إذا defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        أرجع Unsafe_UTFString_ڪوڊ_Points<CP32, std::ranges::views::all_t<Range>>(std::forward<Range>(unitRange));
#وإلا
        إذا constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            أرجع Unsafe_UTFString_ڪوڊ_Points<CP32, std::decay_t<Range>>(std::forward<Range>(unitRange));
        } وإلا {
            أرجع Unsafe_UTFString_ڪوڊ_Points<CP32, Range>(std::forward<Range>(unitRange));
        }
#جيڪڏهن_پورو
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
سانچو<typename CP32>
constexpr Unsafe_UTFString_ڪوڊ_Points_Adaptor<CP32> unsafe_UTFString_ڪوڊ_Points;

}  // namespace U_HEADER_ONLY_NAMESPACE


#إذا defined(__cpp_lib_ranges)
سانچو <typename CP32, UTFIllFormedBehavior behavior, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_سرو_ONLY_NAMESPACE::UTFString_ڪوڊ_Points<CP32, behavior, Range>> =
    std::ranges::enable_borrowed_range<Range>;

سانچو <typename CP32, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_سرو_ONLY_NAMESPACE::Unsafe_UTFString_ڪوڊ_Points<CP32, Range>> =
    std::ranges::enable_borrowed_range<Range>;
#جيڪڏهن_پورو

#جيڪڏهن_پورو  // U_HIDE_DRAFT_API
#جيڪڏهن_پورو  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#جيڪڏهن_پورو  // __UTFITERATOR_H__
