// © 2024 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfiterator.h
// created: 2024aug12 Markus W. Scherer

#اگر_تعریف_نشده __UTFITERATOR_H__
#تعریف_کن __UTFITERATOR_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_سرآیند_API || !defined(UTYPES_H)

#ضمن <iterator>
#إذا __has_include(<نسخه>)
#ضمن <version>
#پایان_اگر
#إذا defined(__cpp_lib_ranges)
#ضمن <ranges>
#پایان_اگر
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

#اگر_تعریف_نشده U_HIDE_DRAFT_API

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

namespace U_سرآیند_ONLY_NAMESPACE {

namespace prv {
#إذا U_CPLUSPLUS_نسخه >= 20

/** @internal */
قالب<typename Iter>
using iter_مقدار_t = typename std::iter_مقدار_t<Iter>;

/** @internal */
قالب<typename Iter>
using iter_difference_t = std::iter_difference_t<Iter>;

/** @internal */
قالب<typename Iter>
constexpr bool forward_iterator = std::forward_iterator<Iter>;

/** @internal */
قالب<typename Iter>
constexpr bool bidirectional_iterator = std::bidirectional_iterator<Iter>;

/** @internal */
قالب<typename Range>
constexpr bool range = std::ranges::range<Range>;

#وإلا

/** @internal */
قالب<typename Iter>
using iter_مقدار_t = typename std::iterator_traits<Iter>::مقدار_type;

/** @internal */
قالب<typename Iter>
using iter_difference_t = typename std::iterator_traits<Iter>::difference_type;

/** @internal */
قالب<typename Iter>
constexpr bool forward_iterator =
    std::is_پایه_از_v<
        std::forward_iterator_برچسب,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
قالب<typename Iter>
constexpr bool bidirectional_iterator =
    std::is_پایه_از_v<
        std::bidirectional_iterator_برچسب,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
قالب<typename Range, typename = فراغ>
بنية range_type : std::false_type {};

/** @internal */
قالب<typename Range>
بنية range_type<
    Range,
    std::void_t<decltype(std::declval<Range>().begin()),
                decltype(std::declval<Range>().انتها())>> : std::true_type {};

/** @internal */
قالب<typename Range>
constexpr bool range = range_type<Range>::مقدار_2;

#پایان_اگر

/** @internal */
قالب <typename T> بنية is_basic_string_view : std::false_type {};

/** @internal */
قالب <typename... آرگومانها>
بنية is_basic_string_view<std::basic_string_view<آرگومانها...>> : std::true_type {};

/** @internal */
قالب <typename T> constexpr bool is_basic_string_view_v = is_basic_string_view<T>::مقدار_2;

/** @internal */
قالب<typename CP32, bool skipSurrogates>
class کد_Points_Iterator {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    /** C++ iterator boilerplate @internal */
    using مقدار_type = CP32;
    /** C++ iterator boilerplate @internal */
    using reference = مقدار_type;
    /** C++ iterator boilerplate @internal */
    using اشاره_گر = CP32 *;
    /** C++ iterator boilerplate @internal */
    using difference_type = int32_t;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::forward_iterator_برچسب;

    /** @internal */
    مضمن کد_Points_Iterator(CP32 c) : c_(c) {}
    /** @internal */
    مضمن bool عملگر==(ثابت کد_Points_Iterator &other) ثابت { أرجع c_ == other.c_; }
    /** @internal */
    مضمن bool عملگر!=(ثابت کد_Points_Iterator &other) ثابت { أرجع !عملگر==(other); }
    /** @internal */
    مضمن CP32 عملگر*() ثابت { أرجع c_; }
    /** @internal */
    مضمن کد_Points_Iterator& عملگر++() U_LIFETIME_BOUND {  // pre-increment
        ++c_;
        إذا (skipSurrogates && c_ == 0xd800) {
            c_ = 0xe000;
        }
        أرجع *this;
    }
    /** @internal */
    مضمن کد_Points_Iterator عملگر++(صحيح) {  // post-increment
        کد_Points_Iterator نتیجه(*this);
        ++(*this);
        أرجع نتیجه;
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
قالب<typename CP32>
class All_کد_Points {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    /** Constructor. @draft ICU 78 */
    All_کد_Points() {}
    /**
     * @return an iterator over all Unicode code points.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    خودکار begin() ثابت { أرجع prv::کد_Points_Iterator<CP32, false>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode code points.
     * @draft ICU 78
     */
    خودکار انتها() ثابت { أرجع prv::کد_Points_Iterator<CP32, false>(0x110000); }
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
قالب<typename CP32>
class All_Scalar_مقادیر {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    /** Constructor. @draft ICU 78 */
    All_Scalar_مقادیر() {}
    /**
     * @return an iterator over all Unicode scalar values.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    خودکار begin() ثابت { أرجع prv::کد_Points_Iterator<CP32, true>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode scalar values.
     * @draft ICU 78
     */
    خودکار انتها() ثابت { أرجع prv::کد_Points_Iterator<CP32, true>(0x110000); }
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
قالب<typename CP32, typename UnitIter, typename = فراغ>
class Unsafe_کد_Units {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Unit = typename prv::iter_مقدار_t<UnitIter>;
عام:
    /** @internal */
    Unsafe_کد_Units(CP32 کد_نقطه, uint8_t درازی, UnitIter شروع, UnitIter حد) :
            c_(کد_نقطه), len_(درازی), شروع_3(شروع), حد_3(حد) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_کد_Units(ثابت Unsafe_کد_Units &other) = افتراضي;
    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_کد_Units &عملگر=(ثابت Unsafe_کد_Units &other) = افتراضي;

    /**
     * @return the Unicode code point decoded from the code unit sequence.
     *     If the sequence is ill-formed and the iterator validates,
     *     then this is a replacement value according to the iterator‘s
     *     UTFIllFormedBehavior template parameter.
     * @draft ICU 78
     */
    CP32 کد_نقطه() ثابت { أرجع c_; }

    /**
     * @return the start of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter begin() ثابت { أرجع شروع_3; }

    /**
     * @return the limit (exclusive end) of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter انتها() ثابت { أرجع حد_3; }

    /**
     * @return the length of the code unit sequence for one code point.
     * @draft ICU 78
     */
    uint8_t درازی() ثابت { أرجع len_; }

#إذا U_CPLUSPLUS_نسخه >= 20
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    قالب<std::contiguous_iterator Iter = UnitIter>
    std::basic_string_view<Unit> stringView() ثابت {
        أرجع std::basic_string_view<Unit>(begin(), انتها());
    }
#وإلا
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    قالب<typename Iter = UnitIter, typename Unit = typename std::iterator_traits<Iter>::مقدار_type>
    std::enable_if_t<std::is_اشاره_گر_v<Iter> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::const_iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::const_iterator>,
                     std::basic_string_view<Unit>>
    stringView() ثابت {
        أرجع std::basic_string_view<Unit>(&*شروع_3, len_);
    }
#پایان_اگر

خاص:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
    UnitIter شروع_3;
    UnitIter حد_3;
};

#اگر_تعریف_نشده U_درون_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
قالب<typename CP32, typename UnitIter>
class Unsafe_کد_Units<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    Unsafe_کد_Units(CP32 کد_نقطه, uint8_t درازی) : c_(کد_نقطه), len_(درازی) {}

    Unsafe_کد_Units(ثابت Unsafe_کد_Units &other) = افتراضي;
    Unsafe_کد_Units &عملگر=(ثابت Unsafe_کد_Units &other) = افتراضي;

    CP32 کد_نقطه() ثابت { أرجع c_; }

    uint8_t درازی() ثابت { أرجع len_; }

خاص:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
};
#پایان_اگر  // U_IN_DOXYGEN

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
قالب<typename CP32, typename UnitIter, typename = فراغ>
class کد_Units_2 : عام Unsafe_کد_Units<CP32, UnitIter> {
عام:
    /** @internal */
    کد_Units_2(CP32 کد_نقطه, uint8_t درازی, bool wellFormed, UnitIter شروع, UnitIter حد) :
            Unsafe_کد_Units<CP32, UnitIter>(کد_نقطه, درازی, شروع, حد), ok_(wellFormed) {}

    /** Copy constructor. @draft ICU 78 */
    کد_Units_2(ثابت کد_Units_2 &other) = افتراضي;
    /** Copy assignment operator. @draft ICU 78 */
    کد_Units_2 &عملگر=(ثابت کد_Units_2 &other) = افتراضي;

    /**
     * @return true if the decoded code unit sequence is well-formed.
     * @draft ICU 78
     */
    bool wellFormed() ثابت { أرجع ok_; }

خاص:
    bool ok_;
};

#اگر_تعریف_نشده U_درون_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
قالب<typename CP32, typename UnitIter>
class کد_Units_2<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> :
            عام Unsafe_کد_Units<CP32, UnitIter> {
عام:
    کد_Units_2(CP32 کد_نقطه, uint8_t درازی, bool wellFormed) :
            Unsafe_کد_Units<CP32, UnitIter>(کد_نقطه, درازی), ok_(wellFormed) {}

    کد_Units_2(ثابت کد_Units_2 &other) = افتراضي;
    کد_Units_2 &عملگر=(ثابت کد_Units_2 &other) = افتراضي;

    bool wellFormed() ثابت { أرجع ok_; }

خاص:
    bool ok_;
};
#پایان_اگر  // U_IN_DOXYGEN

// Validating implementations ---------------------------------------------- ***

#اگر_تعریف_نشده U_درون_DOXYGEN
قالب<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename حد_Iter = UnitIter, typename = فراغ>
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
قالب<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename حد_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, حد_Iter,
        std::enable_if_t<حجم(typename prv::iter_مقدار_t<UnitIter>) == 1>> {
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

    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p, ثابت حد_Iter &حد) {
        // Very similar to U8_FWD_1().
        uint8_t b = *p;
        ++p;
        إذا (U8_IS_LEAD(b) && p != حد) {
            uint8_t t1 = *p;
            إذا ((0xe0 <= b && b < 0xf0)) {
                إذا (U8_IS_VALID_LEAD3_AND_T1(b, t1) &&
                        ++p != حد && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            } وإلا إذا (b < 0xe0) {
                إذا (U8_IS_TRAIL(t1)) {
                    ++p;
                }
            } وإلا /* b >= 0xf0 */ {
                إذا (U8_IS_VALID_LEAD4_AND_T1(b, t1) &&
                        ++p != حد && U8_IS_TRAIL(*p) &&
                        ++p != حد && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            }
        }
    }

    U_FORCE_INLINE ساكن فراغ dec(UnitIter شروع, UnitIter &p) {
        // Very similar to U8_BACK_1().
        uint8_t c = *--p;
        إذا (U8_IS_TRAIL(c) && p != شروع) {
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
            } وإلا إذا (U8_IS_TRAIL(b1) && p1 != شروع) {
                uint8_t b2 = *--p1;
                إذا (0xe0 <= b2 && b2 <= 0xf4) {
                    إذا (b2 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b2, b1) :
                            U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        p = p1;
                        أرجع;
                    }
                } وإلا إذا (U8_IS_TRAIL(b2) && p1 != شروع) {
                    uint8_t b3 = *--p1;
                    إذا (0xf0 <= b3 && b3 <= 0xf4 && U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                        p = p1;
                        أرجع;
                    }
                }
            }
        }
    }

    U_FORCE_INLINE ساكن کد_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ثابت حد_Iter &حد) {
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
        uint8_t درازی = 1;
        uint8_t t = 0;
        إذا (p != حد &&
                // fetch/validate/assemble all but last trail byte
                (c >= 0xe0 ?
                    (c < 0xf0 ?  // U+0800..U+FFFF except surrogates
                        U8_LEAD3_T1_بیتها[c &= 0xf] & (1 << ((t = *p) >> 5)) &&
                        (t &= 0x3f, 1)
                    :  // U+10000..U+10FFFF
                        (c -= 0xf0) <= 4 &&
                        U8_LEAD4_T1_بیتها[(t = *p) >> 4] & (1 << c) &&
                        (c = (c << 6) | (t & 0x3f), ++درازی, ++p != حد) &&
                        (t = *p - 0x80) <= 0x3f) &&
                    // valid second-to-last trail byte
                    (c = (c << 6) | t, ++درازی, ++p != حد)
                :  // U+0080..U+07FF
                    c >= 0xc2 && (c &= 0x1f, 1)) &&
                // last trail byte
                (t = *p - 0x80) <= 0x3f) {
            c = (c << 6) | t;
            ++درازی;
            ++p;
            إذا constexpr (isMultiPass) {
                أرجع {c, درازی, true, p0, p};
            } وإلا {
                أرجع {c, درازی, true};
            }
        }
        إذا constexpr (isMultiPass) {
            أرجع {sub(), درازی, false, p0, p};
        } وإلا {
            أرجع {sub(), درازی, false};
        }
    }

    U_FORCE_INLINE ساكن کد_Units_2<CP32, UnitIter> decAndRead(UnitIter شروع, UnitIter &p) {
        // Very similar to U8_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        إذا (U8_IS_SINGLE(c)) {
            أرجع {c, 1, true, p, p0};
        }
        إذا (U8_IS_TRAIL(c) && p != شروع) {
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
            } وإلا إذا (U8_IS_TRAIL(b1) && p1 != شروع) {
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
                } وإلا إذا (U8_IS_TRAIL(b2) && p1 != شروع) {
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
قالب<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename حد_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, حد_Iter,
        std::enable_if_t<حجم(typename prv::iter_مقدار_t<UnitIter>) == 2>> {
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

    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p, ثابت حد_Iter &حد) {
        // Very similar to U16_FWD_1().
        خودکار c = *p;
        ++p;
        إذا (U16_IS_LEAD(c) && p != حد && U16_IS_TRAIL(*p)) {
            ++p;
        }
    }

    U_FORCE_INLINE ساكن فراغ dec(UnitIter شروع, UnitIter &p) {
        // Very similar to U16_BACK_1().
        UnitIter p1;
        إذا (U16_IS_TRAIL(*--p) && p != شروع && (p1 = p, U16_IS_LEAD(*--p1))) {
            p = p1;
        }
    }

    U_FORCE_INLINE ساكن کد_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ثابت حد_Iter &حد) {
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
            إذا (U16_IS_SURROGATE_LEAD(c) && p != حد && U16_IS_TRAIL(c2 = *p)) {
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

    U_FORCE_INLINE ساكن کد_Units_2<CP32, UnitIter> decAndRead(UnitIter شروع, UnitIter &p) {
        // Very similar to U16_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        إذا (!U16_IS_SURROGATE(c)) {
            أرجع {c, 1, true, p, p0};
        } وإلا {
            UnitIter p1;
            uint16_t c2;
            إذا (U16_IS_SURROGATE_TRAIL(c) && p != شروع && (p1 = p, U16_IS_LEAD(c2 = *--p1))) {
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
قالب<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename حد_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, حد_Iter,
        std::enable_if_t<حجم(typename prv::iter_مقدار_t<UnitIter>) == 4>> {
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

    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p, ثابت حد_Iter &/*حد*/) {
        ++p;
    }

    U_FORCE_INLINE ساكن فراغ dec(UnitIter /*شروع*/, UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE ساكن کد_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ثابت حد_Iter &/*حد*/) {
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

    U_FORCE_INLINE ساكن کد_Units_2<CP32, UnitIter> decAndRead(UnitIter /*شروع*/, UnitIter &p) {
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

قالب<typename CP32, typename UnitIter, typename = فراغ>
class UnsafeUTFImpl;

// UTF-8
قالب<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<حجم(typename prv::iter_مقدار_t<UnitIter>) == 1>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p) {
        // Very similar to U8_FWD_1_UNSAFE().
        uint8_t b = *p;
        std::advance(p, 1 + U8_شمار_TRAIL_بایتها_UNSAFE(b));
    }

    U_FORCE_INLINE ساكن فراغ dec(UnitIter &p) {
        // Very similar to U8_BACK_1_UNSAFE().
        طالما (U8_IS_TRAIL(*--p)) {}
    }

    U_FORCE_INLINE ساكن Unsafe_کد_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
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

    U_FORCE_INLINE ساكن Unsafe_کد_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U8_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        إذا (U8_IS_SINGLE(c)) {
            أرجع {c, 1, p, p0};
        }
        // U8_IS_TRAIL(c) if well-formed
        c &= 0x3f;
        uint8_t شمار = 1;
        لكل (uint8_t لغزش = 6;;) {
            uint8_t b = *--p;
            إذا (b >= 0xc0) {
                U8_ماسک_LEAD_BYTE(b, شمار);
                c |= uint32_t{b} << لغزش;
                اكسر;
            } وإلا {
                c |= (uint32_t{b} & 0x3f) << لغزش;
                ++شمار;
                لغزش += 6;
            }
        }
        ++شمار;
        أرجع {c, شمار, p, p0};
    }
};

// UTF-16
قالب<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<حجم(typename prv::iter_مقدار_t<UnitIter>) == 2>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p) {
        // Very similar to U16_FWD_1_UNSAFE().
        خودکار c = *p;
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

    U_FORCE_INLINE ساكن Unsafe_کد_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
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

    U_FORCE_INLINE ساكن Unsafe_کد_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
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
قالب<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<حجم(typename prv::iter_مقدار_t<UnitIter>) == 4>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    U_FORCE_INLINE ساكن فراغ inc(UnitIter &p) {
        ++p;
    }

    U_FORCE_INLINE ساكن فراغ dec(UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE ساكن Unsafe_کد_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        CP32 c = *p;
        ++p;
        إذا constexpr (isMultiPass) {
            أرجع {c, 1, p0, p};
        } وإلا {
            أرجع {c, 1};
        }
    }

    U_FORCE_INLINE ساكن Unsafe_کد_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        UnitIter p0 = p;
        CP32 c = *--p;
        أرجع {c, 1, p, p0};
    }
};

#پایان_اگر

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
قالب<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename حد_Iter = UnitIter, typename = فراغ>
class UTFIterator {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, حد_Iter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    عام:
        explicit Proxy(کد_Units_2<CP32, UnitIter> &units) : units_(units) {}
        کد_Units_2<CP32, UnitIter>& عملگر*() U_LIFETIME_BOUND { أرجع units_; }
        کد_Units_2<CP32, UnitIter>* عملگر->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        کد_Units_2<CP32, UnitIter> units_;
    };

عام:
    /** C++ iterator boilerplate @internal */
    using مقدار_type = کد_Units_2<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = مقدار_type;
    /** C++ iterator boilerplate @internal */
    using اشاره_گر = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_برچسب,
        std::forward_iterator_برچسب>;

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
    U_FORCE_INLINE UTFIterator(UnitIter شروع, UnitIter p, حد_Iter حد) :
            p_(p), شروع_3(شروع), حد_3(حد), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE UTFIterator(UnitIter p, حد_Iter حد) :
            p_(p), شروع_3(p), حد_3(حد), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(p), شروع_3(p), حد_3(p), units_(0, 0, false, p, p) {}
    /**
     * Default constructor. Makes a non-functional iterator.
     *
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator() : p_{}, شروع_3{}, حد_3{}, units_(0, 0, false, p_, p_) {}

    /** Move constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = افتراضي;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &عملگر=(UTFIterator &&src) noexcept = افتراضي;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(ثابت UTFIterator &other) = افتراضي;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &عملگر=(ثابت UTFIterator &other) = افتراضي;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool عملگر==(ثابت UTFIterator &other) ثابت {
        أرجع پایه() == other.پایه();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool عملگر!=(ثابت UTFIterator &other) ثابت { أرجع !عملگر==(other); }

    // Asymmetric equality & nonequality with a sentinel type.

    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر==(ثابت UTFIterator &iter, ثابت Sentinel &s) {
        أرجع iter.پایه() == s;
    }

#إذا U_CPLUSPLUS_نسخه < 20
    // C++17: Need to define all four combinations of == / != vs. parameter order.
    // Once we require C++20, we could remove all but the first == because
    // the compiler would generate the rest.

    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر==(ثابت Sentinel &s, ثابت UTFIterator &iter) {
        أرجع iter.پایه() == s;
    }
    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر!=(ثابت UTFIterator &iter, ثابت Sentinel &s) { أرجع !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر!=(ثابت Sentinel &s, ثابت UTFIterator &iter) { أرجع !(iter == s); }
#پایان_اگر  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter پایه() ثابت {
        // Return the logical position.
        أرجع state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE کد_Units_2<CP32, UnitIter> عملگر*() ثابت {
        إذا (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, حد_3);
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
    U_FORCE_INLINE Proxy عملگر->() ثابت {
        إذا (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, حد_3);
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
    U_FORCE_INLINE UTFIterator& عملگر++() U_LIFETIME_BOUND {  // pre-increment
        إذا (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } وإلا إذا (state_ == 0) {
            Impl::inc(p_, حد_3);
        } وإلا /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.انتها();
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
    U_FORCE_INLINE UTFIterator عملگر++(صحيح) {  // post-increment
        إذا (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UTFIterator نتیجه(*this);
            state_ = 0;
            أرجع نتیجه;
        } وإلا إذا (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, حد_3);
            UTFIterator نتیجه(*this);
            نتیجه.state_ = 1;
            // keep this->state_ == 0
            أرجع نتیجه;
        } وإلا /* state_ < 0 */ {
            UTFIterator نتیجه(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.انتها();
            state_ = 0;
            أرجع نتیجه;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    قالب <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator&>
    عملگر--() U_LIFETIME_BOUND {  // pre-decrement
        إذا (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(شروع_3, p_);
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
    قالب<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator>
    عملگر--(صحيح) {  // post-decrement
        UTFIterator نتیجه(*this);
        عملگر--();
        أرجع نتیجه;
    }

خاص:
    دوست class std::معکوس_iterator<UTFIterator<CP32, behavior, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter شروع_3;
    حد_Iter حد_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable کد_Units_2<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#اگر_تعریف_نشده U_درون_DOXYGEN
// Partial template specialization for single-pass input iterator.
قالب<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename حد_Iter>
class UTFIterator<
        CP32, behavior,
        UnitIter, حد_Iter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, حد_Iter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    عام:
        explicit Proxy(کد_Units_2<CP32, UnitIter> &units) : units_(units) {}
        کد_Units_2<CP32, UnitIter>& عملگر*() U_LIFETIME_BOUND { أرجع units_; }
        کد_Units_2<CP32, UnitIter>* عملگر->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        کد_Units_2<CP32, UnitIter> units_;
    };

عام:
    using مقدار_type = کد_Units_2<CP32, UnitIter>;
    using reference = مقدار_type;
    using اشاره_گر = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::درون_داد_iterator_برچسب;

    U_FORCE_INLINE UTFIterator(UnitIter p, حد_Iter حد) : p_(std::move(p)), حد_3(std::move(حد)) {}

    // Constructs an iterator start or limit sentinel.
    // Requires p to be copyable.
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(std::move(p)), حد_3(p_) {}

    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = افتراضي;
    U_FORCE_INLINE UTFIterator &عملگر=(UTFIterator &&src) noexcept = افتراضي;

    U_FORCE_INLINE UTFIterator(ثابت UTFIterator &other) = افتراضي;
    U_FORCE_INLINE UTFIterator &عملگر=(ثابت UTFIterator &other) = افتراضي;

    U_FORCE_INLINE bool عملگر==(ثابت UTFIterator &other) ثابت {
        أرجع p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool عملگر!=(ثابت UTFIterator &other) ثابت { أرجع !عملگر==(other); }

    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر==(ثابت UTFIterator &iter, ثابت Sentinel &s) {
        أرجع !iter.ahead_ && iter.p_ == s;
    }

#إذا U_CPLUSPLUS_نسخه < 20
    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر==(ثابت Sentinel &s, ثابت UTFIterator &iter) {
        أرجع !iter.ahead_ && iter.p_ == s;
    }

    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر!=(ثابت UTFIterator &iter, ثابت Sentinel &s) { أرجع !(iter == s); }

    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر!=(ثابت Sentinel &s, ثابت UTFIterator &iter) { أرجع !(iter == s); }
#پایان_اگر  // C++17

    U_FORCE_INLINE کد_Units_2<CP32, UnitIter> عملگر*() ثابت {
        إذا (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, حد_3);
            ahead_ = true;
        }
        أرجع units_;
    }

    U_FORCE_INLINE Proxy عملگر->() ثابت {
        إذا (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, حد_3);
            ahead_ = true;
        }
        أرجع Proxy(units_);
    }

    U_FORCE_INLINE UTFIterator& عملگر++() U_LIFETIME_BOUND {  // pre-increment
        إذا (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } وإلا {
            Impl::inc(p_, حد_3);
        }
        أرجع *this;
    }

    U_FORCE_INLINE Proxy عملگر++(صحيح) {  // post-increment
        إذا (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } وإلا {
            units_ = Impl::readAndInc(p_, p_, حد_3);
            // keep this->ahead_ == false
        }
        أرجع Proxy(units_);
    }

خاص:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need limit_ so that when we read a code point
    // we can test if there are enough code units.
    حد_Iter حد_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable کد_Units_2<CP32, UnitIter> units_ = {0, 0, false};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#پایان_اگر  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#اگر_تعریف_نشده U_درون_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
قالب<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
class std::معکوس_iterator<U_سرآیند_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_سرآیند_ONLY_NAMESPACE::UTFImpl<CP32, behavior, UnitIter>;
    using کد_Units_3 = U_سرآیند_ONLY_NAMESPACE::کد_Units_2<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    عام:
        explicit Proxy(کد_Units_3 units) : units_(units) {}
        کد_Units_3& عملگر*() U_LIFETIME_BOUND { أرجع units_; }
        کد_Units_3* عملگر->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        کد_Units_3 units_;
    };

عام:
    using مقدار_type = کد_Units_3;
    using reference = مقدار_type;
    using اشاره_گر = Proxy;
    using difference_type = U_سرآیند_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_برچسب;

    U_FORCE_INLINE explicit معکوس_iterator(U_سرآیند_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> iter) :
            p_(iter.پایه()), شروع_3(iter.شروع_3), حد_3(iter.حد_3),
            units_(0, 0, false, p_, p_) {}
    U_FORCE_INLINE معکوس_iterator() : p_{}, شروع_3{}, حد_3{}, units_(0, 0, false, p_, p_) {}

    U_FORCE_INLINE معکوس_iterator(معکوس_iterator &&src) noexcept = افتراضي;
    U_FORCE_INLINE معکوس_iterator &عملگر=(معکوس_iterator &&src) noexcept = افتراضي;

    U_FORCE_INLINE معکوس_iterator(ثابت معکوس_iterator &other) = افتراضي;
    U_FORCE_INLINE معکوس_iterator &عملگر=(ثابت معکوس_iterator &other) = افتراضي;

    U_FORCE_INLINE bool عملگر==(ثابت معکوس_iterator &other) ثابت {
        أرجع getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool عملگر!=(ثابت معکوس_iterator &other) ثابت { أرجع !عملگر==(other); }

    U_FORCE_INLINE U_سرآیند_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> پایه() ثابت {
        أرجع U_سرآیند_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>(
            شروع_3, getLogicalPosition(), حد_3);
    }

    U_FORCE_INLINE کد_Units_3 عملگر*() ثابت {
        إذا (state_ == 0) {
            units_ = Impl::decAndRead(شروع_3, p_);
            state_ = -1;
        }
        أرجع units_;
    }

    U_FORCE_INLINE Proxy عملگر->() ثابت {
        إذا (state_ == 0) {
            units_ = Impl::decAndRead(شروع_3, p_);
            state_ = -1;
        }
        أرجع Proxy(units_);
    }

    U_FORCE_INLINE معکوس_iterator& عملگر++() U_LIFETIME_BOUND {  // pre-increment
        إذا (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } وإلا إذا (state_ == 0) {
            Impl::dec(شروع_3, p_);
        } وإلا /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        أرجع *this;
    }

    U_FORCE_INLINE معکوس_iterator عملگر++(صحيح) {  // post-increment
        إذا (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            معکوس_iterator نتیجه(*this);
            state_ = 0;
            أرجع نتیجه;
        } وإلا إذا (state_ == 0) {
            units_ = Impl::decAndRead(شروع_3, p_);
            معکوس_iterator نتیجه(*this);
            نتیجه.state_ = -1;
            // keep this->state_ == 0
            أرجع نتیجه;
        } وإلا /* state_ > 0 */ {
            معکوس_iterator نتیجه(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            أرجع نتیجه;
        }
    }

    U_FORCE_INLINE معکوس_iterator& عملگر--() U_LIFETIME_BOUND {  // pre-decrement
        إذا (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.انتها();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_, حد_3);
        state_ = 1;
        أرجع *this;
    }

    U_FORCE_INLINE معکوس_iterator عملگر--(صحيح) {  // post-decrement
        معکوس_iterator نتیجه(*this);
        عملگر--();
        أرجع نتیجه;
    }

خاص:
    U_FORCE_INLINE UnitIter getLogicalPosition() ثابت {
        أرجع state_ >= 0 ? p_ : units_.انتها();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter شروع_3;
    UnitIter حد_3;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable کد_Units_3 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#پایان_اگر  // U_IN_DOXYGEN

namespace U_سرآیند_ONLY_NAMESPACE {

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
قالب<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename حد_Iter = UnitIter>
خودکار utfIterator(UnitIter شروع, UnitIter p, حد_Iter حد) {
    أرجع UTFIterator<CP32, behavior, UnitIter, حد_Iter>(
        std::move(شروع), std::move(p), std::move(حد));
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
قالب<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename حد_Iter = UnitIter>
خودکار utfIterator(UnitIter p, حد_Iter حد) {
    أرجع UTFIterator<CP32, behavior, UnitIter, حد_Iter>(
        std::move(p), std::move(حد));
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
قالب<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
خودکار utfIterator(UnitIter p) {
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
قالب<typename CP32, UTFIllFormedBehavior behavior, typename Range>
#إذا defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#پایان_اگر
class UTFString_کد_Points {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    UTFString_کد_Points() = افتراضي;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    قالب<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit UTFString_کد_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C+20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    قالب<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = فراغ>
    explicit UTFString_کد_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    UTFString_کد_Points(ثابت UTFString_کد_Points &other) = افتراضي;

    /** Copy assignment operator. @draft ICU 78 */
    UTFString_کد_Points &عملگر=(ثابت UTFString_کد_Points &other) = افتراضي;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    خودکار begin() {
        أرجع utfIterator<CP32, behavior>(unitRange.begin(), unitRange.انتها());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    قالب<typename R = Range, typename = std::enable_if_t<prv::range<ثابت R>>>
    خودکار begin() ثابت {
        أرجع utfIterator<CP32, behavior>(unitRange.begin(), unitRange.انتها());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    خودکار انتها() {
        using UnitIter = decltype(unitRange.begin());
        using حد_Iter = decltype(unitRange.انتها());
        إذا constexpr (!std::is_same_v<UnitIter, حد_Iter>) {
            // Return the code unit sentinel.
            أرجع unitRange.انتها();
        } وإلا إذا constexpr (prv::bidirectional_iterator<UnitIter>) {
            أرجع utfIterator<CP32, behavior>(unitRange.begin(), unitRange.انتها(), unitRange.انتها());
        } وإلا {
            // The input iterator specialization has no three-argument constructor.
            أرجع utfIterator<CP32, behavior>(unitRange.انتها(), unitRange.انتها());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    قالب<typename R = Range, typename = std::enable_if_t<prv::range<ثابت R>>>
    خودکار انتها() ثابت {
        using UnitIter = decltype(unitRange.begin());
        using حد_Iter = decltype(unitRange.انتها());
        إذا constexpr (!std::is_same_v<UnitIter, حد_Iter>) {
            // Return the code unit sentinel.
            أرجع unitRange.انتها();
        } وإلا إذا constexpr (prv::bidirectional_iterator<UnitIter>) {
            أرجع utfIterator<CP32, behavior>(unitRange.begin(), unitRange.انتها(), unitRange.انتها());
        } وإلا {
            // The input iterator specialization has no three-argument constructor.
            أرجع utfIterator<CP32, behavior>(unitRange.انتها(), unitRange.انتها());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    خودکار rbegin() ثابت {
        أرجع std::make_معکوس_iterator(انتها());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    خودکار rend() ثابت {
        أرجع std::make_معکوس_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    خودکار front() ثابت {
        أرجع *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    خودکار پشت() ثابت {
        أرجع *(--انتها());
    }

خاص:
    Range unitRange;
};

/** @internal */
قالب<typename CP32, UTFIllFormedBehavior behavior>
بنية UTFString_کد_Points_Adaptor
#إذا U_CPLUSPLUS_نسخه >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_بستگی<UTFString_کد_Points_Adaptor<CP32, behavior>>
#پایان_اگر
{
    /** @internal */
    قالب<typename Range>
    خودکار عملگر()(Range &&unitRange) ثابت {
#إذا defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        أرجع UTFString_کد_Points<CP32, behavior, std::ranges::views::all_t<Range>>(
            std::forward<Range>(unitRange));
#وإلا
        إذا constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            أرجع UTFString_کد_Points<CP32, behavior, std::decay_t<Range>>(
                std::forward<Range>(unitRange));
        } وإلا {
            أرجع UTFString_کد_Points<CP32, behavior, Range>(std::forward<Range>(unitRange));
        }
#پایان_اگر
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
قالب<typename CP32, UTFIllFormedBehavior behavior>
constexpr UTFString_کد_Points_Adaptor<CP32, behavior> utf_String_کد_Points;

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
قالب<typename CP32, typename UnitIter, typename = فراغ>
class UnsafeUTFIterator {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    عام:
        explicit Proxy(Unsafe_کد_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_کد_Units<CP32, UnitIter>& عملگر*() U_LIFETIME_BOUND { أرجع units_; }
        Unsafe_کد_Units<CP32, UnitIter>* عملگر->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        Unsafe_کد_Units<CP32, UnitIter> units_;
    };

عام:
    /** C++ iterator boilerplate @internal */
    using مقدار_type = Unsafe_کد_Units<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = مقدار_type;
    /** C++ iterator boilerplate @internal */
    using اشاره_گر = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_برچسب,
        std::forward_iterator_برچسب>;

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
    U_FORCE_INLINE UnsafeUTFIterator &عملگر=(UnsafeUTFIterator &&src) noexcept = افتراضي;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator(ثابت UnsafeUTFIterator &other) = افتراضي;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &عملگر=(ثابت UnsafeUTFIterator &other) = افتراضي;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool عملگر==(ثابت UnsafeUTFIterator &other) ثابت {
        أرجع پایه() == other.پایه();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool عملگر!=(ثابت UnsafeUTFIterator &other) ثابت { أرجع !عملگر==(other); }

    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر==(ثابت UnsafeUTFIterator &iter, ثابت Sentinel &s) {
        أرجع iter.پایه() == s;
    }

#إذا U_CPLUSPLUS_نسخه < 20
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر==(ثابت Sentinel &s, ثابت UnsafeUTFIterator &iter) {
        أرجع iter.پایه() == s;
    }
    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر!=(ثابت UnsafeUTFIterator &iter, ثابت Sentinel &s) { أرجع !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر!=(ثابت Sentinel &s, ثابت UnsafeUTFIterator &iter) { أرجع !(iter == s); }
#پایان_اگر  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter پایه() ثابت {
        // Return the logical position.
        أرجع state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE Unsafe_کد_Units<CP32, UnitIter> عملگر*() ثابت {
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
    U_FORCE_INLINE Proxy عملگر->() ثابت {
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
    U_FORCE_INLINE UnsafeUTFIterator& عملگر++() U_LIFETIME_BOUND {  // pre-increment
        إذا (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } وإلا إذا (state_ == 0) {
            Impl::inc(p_);
        } وإلا /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.انتها();
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
    U_FORCE_INLINE UnsafeUTFIterator عملگر++(صحيح) {  // post-increment
        إذا (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UnsafeUTFIterator نتیجه(*this);
            state_ = 0;
            أرجع نتیجه;
        } وإلا إذا (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            UnsafeUTFIterator نتیجه(*this);
            نتیجه.state_ = 1;
            // keep this->state_ == 0
            أرجع نتیجه;
        } وإلا /* state_ < 0 */ {
            UnsafeUTFIterator نتیجه(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.انتها();
            state_ = 0;
            أرجع نتیجه;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    قالب <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator&>
    عملگر--() U_LIFETIME_BOUND {  // pre-decrement
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
    قالب<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator>
    عملگر--(صحيح) {  // post-decrement
        UnsafeUTFIterator نتیجه(*this);
        عملگر--();
        أرجع نتیجه;
    }

خاص:
    دوست class std::معکوس_iterator<UnsafeUTFIterator<CP32, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_کد_Units<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#اگر_تعریف_نشده U_درون_DOXYGEN
// Partial template specialization for single-pass input iterator.
قالب<typename CP32, typename UnitIter>
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
        explicit Proxy(Unsafe_کد_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_کد_Units<CP32, UnitIter>& عملگر*() U_LIFETIME_BOUND { أرجع units_; }
        Unsafe_کد_Units<CP32, UnitIter>* عملگر->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        Unsafe_کد_Units<CP32, UnitIter> units_;
    };

عام:
    using مقدار_type = Unsafe_کد_Units<CP32, UnitIter>;
    using reference = مقدار_type;
    using اشاره_گر = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::درون_داد_iterator_برچسب;

    U_FORCE_INLINE explicit UnsafeUTFIterator(UnitIter p) : p_(std::move(p)) {}

    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = افتراضي;
    U_FORCE_INLINE UnsafeUTFIterator &عملگر=(UnsafeUTFIterator &&src) noexcept = افتراضي;

    U_FORCE_INLINE UnsafeUTFIterator(ثابت UnsafeUTFIterator &other) = افتراضي;
    U_FORCE_INLINE UnsafeUTFIterator &عملگر=(ثابت UnsafeUTFIterator &other) = افتراضي;

    U_FORCE_INLINE bool عملگر==(ثابت UnsafeUTFIterator &other) ثابت {
        أرجع p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool عملگر!=(ثابت UnsafeUTFIterator &other) ثابت { أرجع !عملگر==(other); }

    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر==(ثابت UnsafeUTFIterator &iter, ثابت Sentinel &s) {
        أرجع !iter.ahead_ && iter.p_ == s;
    }

#إذا U_CPLUSPLUS_نسخه < 20
    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر==(ثابت Sentinel &s, ثابت UnsafeUTFIterator &iter) {
        أرجع !iter.ahead_ && iter.p_ == s;
    }

    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر!=(ثابت UnsafeUTFIterator &iter, ثابت Sentinel &s) { أرجع !(iter == s); }

    قالب<typename Sentinel> U_FORCE_INLINE دوست
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    عملگر!=(ثابت Sentinel &s, ثابت UnsafeUTFIterator &iter) { أرجع !(iter == s); }
#پایان_اگر  // C++17

    U_FORCE_INLINE Unsafe_کد_Units<CP32, UnitIter> عملگر*() ثابت {
        إذا (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        أرجع units_;
    }

    U_FORCE_INLINE Proxy عملگر->() ثابت {
        إذا (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        أرجع Proxy(units_);
    }

    U_FORCE_INLINE UnsafeUTFIterator& عملگر++() U_LIFETIME_BOUND {  // pre-increment
        إذا (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } وإلا {
            Impl::inc(p_);
        }
        أرجع *this;
    }

    U_FORCE_INLINE Proxy عملگر++(صحيح) {  // post-increment
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
    mutable Unsafe_کد_Units<CP32, UnitIter> units_ = {0, 0};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#پایان_اگر  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#اگر_تعریف_نشده U_درون_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
قالب<typename CP32, typename UnitIter>
class std::معکوس_iterator<U_سرآیند_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>> {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_سرآیند_ONLY_NAMESPACE::UnsafeUTFImpl<CP32, UnitIter>;
    using Unsafe_کد_Units_2 = U_سرآیند_ONLY_NAMESPACE::Unsafe_کد_Units<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    عام:
        explicit Proxy(Unsafe_کد_Units_2 units) : units_(units) {}
        Unsafe_کد_Units_2& عملگر*() U_LIFETIME_BOUND { أرجع units_; }
        Unsafe_کد_Units_2* عملگر->() U_LIFETIME_BOUND { أرجع &units_; }
    خاص:
        Unsafe_کد_Units_2 units_;
    };

عام:
    using مقدار_type = Unsafe_کد_Units_2;
    using reference = مقدار_type;
    using اشاره_گر = Proxy;
    using difference_type = U_سرآیند_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_برچسب;

    U_FORCE_INLINE explicit معکوس_iterator(U_سرآیند_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> iter) :
            p_(iter.پایه()), units_(0, 0, p_, p_) {}
    U_FORCE_INLINE معکوس_iterator() : p_{}, units_(0, 0, p_, p_) {}

    U_FORCE_INLINE معکوس_iterator(معکوس_iterator &&src) noexcept = افتراضي;
    U_FORCE_INLINE معکوس_iterator &عملگر=(معکوس_iterator &&src) noexcept = افتراضي;

    U_FORCE_INLINE معکوس_iterator(ثابت معکوس_iterator &other) = افتراضي;
    U_FORCE_INLINE معکوس_iterator &عملگر=(ثابت معکوس_iterator &other) = افتراضي;

    U_FORCE_INLINE bool عملگر==(ثابت معکوس_iterator &other) ثابت {
        أرجع getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool عملگر!=(ثابت معکوس_iterator &other) ثابت { أرجع !عملگر==(other); }

    U_FORCE_INLINE U_سرآیند_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> پایه() ثابت {
        أرجع U_سرآیند_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>(
            getLogicalPosition());
    }

    U_FORCE_INLINE Unsafe_کد_Units_2 عملگر*() ثابت {
        إذا (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        أرجع units_;
    }

    U_FORCE_INLINE Proxy عملگر->() ثابت {
        إذا (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        أرجع Proxy(units_);
    }

    U_FORCE_INLINE معکوس_iterator& عملگر++() U_LIFETIME_BOUND {  // pre-increment
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

    U_FORCE_INLINE معکوس_iterator عملگر++(صحيح) {  // post-increment
        إذا (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            معکوس_iterator نتیجه(*this);
            state_ = 0;
            أرجع نتیجه;
        } وإلا إذا (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            معکوس_iterator نتیجه(*this);
            نتیجه.state_ = -1;
            // keep this->state_ == 0
            أرجع نتیجه;
        } وإلا /* state_ > 0 */ {
            معکوس_iterator نتیجه(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            أرجع نتیجه;
        }
    }

    U_FORCE_INLINE معکوس_iterator& عملگر--() U_LIFETIME_BOUND {  // pre-decrement
        إذا (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.انتها();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_);
        state_ = 1;
        أرجع *this;
    }

    U_FORCE_INLINE معکوس_iterator عملگر--(صحيح) {  // post-decrement
        معکوس_iterator نتیجه(*this);
        عملگر--();
        أرجع نتیجه;
    }

خاص:
    U_FORCE_INLINE UnitIter getLogicalPosition() ثابت {
        أرجع state_ >= 0 ? p_ : units_.انتها();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_کد_Units_2 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#پایان_اگر  // U_IN_DOXYGEN

namespace U_سرآیند_ONLY_NAMESPACE {

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
قالب<typename CP32, typename UnitIter>
خودکار unsafeUTFIterator(UnitIter iter) {
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
قالب<typename CP32, typename Range>
#إذا defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#پایان_اگر
class Unsafe_UTFString_کد_Points {
    static_assert(حجم(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
عام:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    Unsafe_UTFString_کد_Points() = افتراضي;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    قالب<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit Unsafe_UTFString_کد_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C++20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    قالب<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = فراغ>
    explicit Unsafe_UTFString_کد_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_UTFString_کد_Points(ثابت Unsafe_UTFString_کد_Points &other) = افتراضي;

    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_UTFString_کد_Points &عملگر=(ثابت Unsafe_UTFString_کد_Points &other) = افتراضي;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    خودکار begin() {
        أرجع unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    قالب<typename R = Range, typename = std::enable_if_t<prv::range<ثابت R>>>
    خودکار begin() ثابت {
        أرجع unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    خودکار انتها() {
        using UnitIter = decltype(unitRange.begin());
        using حد_Iter = decltype(unitRange.انتها());
        إذا constexpr (!std::is_same_v<UnitIter, حد_Iter>) {
            // Return the code unit sentinel.
            أرجع unitRange.انتها();
        } وإلا {
            أرجع unsafeUTFIterator<CP32>(unitRange.انتها());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    قالب<typename R = Range, typename = std::enable_if_t<prv::range<ثابت R>>>
    خودکار انتها() ثابت {
        using UnitIter = decltype(unitRange.begin());
        using حد_Iter = decltype(unitRange.انتها());
        إذا constexpr (!std::is_same_v<UnitIter, حد_Iter>) {
            // Return the code unit sentinel.
            أرجع unitRange.انتها();
        } وإلا {
            أرجع unsafeUTFIterator<CP32>(unitRange.انتها());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    خودکار rbegin() ثابت {
        أرجع std::make_معکوس_iterator(انتها());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    خودکار rend() ثابت {
        أرجع std::make_معکوس_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    خودکار front() ثابت {
        أرجع *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    خودکار پشت() ثابت {
        أرجع *(--انتها());
    }

خاص:
    Range unitRange;
};

/** @internal */
قالب<typename CP32>
بنية Unsafe_UTFString_کد_Points_Adaptor
#إذا U_CPLUSPLUS_نسخه >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_بستگی<Unsafe_UTFString_کد_Points_Adaptor<CP32>>
#پایان_اگر
{
    /** @internal */
    قالب<typename Range>
    خودکار عملگر()(Range &&unitRange) ثابت {
#إذا defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        أرجع Unsafe_UTFString_کد_Points<CP32, std::ranges::views::all_t<Range>>(std::forward<Range>(unitRange));
#وإلا
        إذا constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            أرجع Unsafe_UTFString_کد_Points<CP32, std::decay_t<Range>>(std::forward<Range>(unitRange));
        } وإلا {
            أرجع Unsafe_UTFString_کد_Points<CP32, Range>(std::forward<Range>(unitRange));
        }
#پایان_اگر
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
قالب<typename CP32>
constexpr Unsafe_UTFString_کد_Points_Adaptor<CP32> unsafe_UTFString_کد_Points;

}  // namespace U_HEADER_ONLY_NAMESPACE


#إذا defined(__cpp_lib_ranges)
قالب <typename CP32, UTFIllFormedBehavior behavior, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_سرآیند_ONLY_NAMESPACE::UTFString_کد_Points<CP32, behavior, Range>> =
    std::ranges::enable_borrowed_range<Range>;

قالب <typename CP32, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_سرآیند_ONLY_NAMESPACE::Unsafe_UTFString_کد_Points<CP32, Range>> =
    std::ranges::enable_borrowed_range<Range>;
#پایان_اگر

#پایان_اگر  // U_HIDE_DRAFT_API
#پایان_اگر  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#پایان_اگر  // __UTFITERATOR_H__
