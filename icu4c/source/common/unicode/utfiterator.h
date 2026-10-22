// © 2024 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfiterator.h
// created: 2024aug12 Markus W. Scherer

#यदि_अपरिभाषितम् __UTFITERATOR_H__
#परिभाषय __UTFITERATOR_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_शीर्षकम्_API || !defined(UTYPES_H)

#अन्तर्भाव <iterator>
#यदि __has_include(<संस्करणम्>)
#अन्तर्भाव <version>
#यदि_अन्तः
#यदि defined(__cpp_lib_ranges)
#अन्तर्भाव <ranges>
#यदि_अन्तः
#अन्तर्भाव <string>
#अन्तर्भाव <string_view>
#अन्तर्भाव <type_traits>
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/uversion.h"

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

#यदि_अपरिभाषितम् U_HIDE_DRAFT_API

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
प्रकारनिर्देश गणना UTFIllFormedBehavior {
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

namespace U_शीर्षकम्_ONLY_NAMESPACE {

namespace prv {
#यदि U_CPLUSPLUS_संस्करणम् >= 20

/** @internal */
प्रतिरूपम्<typename Iter>
using iter_मानम्_t = typename std::iter_मानम्_t<Iter>;

/** @internal */
प्रतिरूपम्<typename Iter>
using iter_difference_t = std::iter_difference_t<Iter>;

/** @internal */
प्रतिरूपम्<typename Iter>
constexpr bool forward_iterator = std::forward_iterator<Iter>;

/** @internal */
प्रतिरूपम्<typename Iter>
constexpr bool bidirectional_iterator = std::bidirectional_iterator<Iter>;

/** @internal */
प्रतिरूपम्<typename Range>
constexpr bool range = std::ranges::range<Range>;

#अन्यथा

/** @internal */
प्रतिरूपम्<typename Iter>
using iter_मानम्_t = typename std::iterator_traits<Iter>::मानम्_type;

/** @internal */
प्रतिरूपम्<typename Iter>
using iter_difference_t = typename std::iterator_traits<Iter>::difference_type;

/** @internal */
प्रतिरूपम्<typename Iter>
constexpr bool forward_iterator =
    std::is_आधारः_सम्बन्धि_v<
        std::forward_iterator_चिह्नकम्,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
प्रतिरूपम्<typename Iter>
constexpr bool bidirectional_iterator =
    std::is_आधारः_सम्बन्धि_v<
        std::bidirectional_iterator_चिह्नकम्,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
प्रतिरूपम्<typename Range, typename = शून्य>
संरचना range_type : std::false_type {};

/** @internal */
प्रतिरूपम्<typename Range>
संरचना range_type<
    Range,
    std::void_t<decltype(std::declval<Range>().begin()),
                decltype(std::declval<Range>().अन्तः())>> : std::true_type {};

/** @internal */
प्रतिरूपम्<typename Range>
constexpr bool range = range_type<Range>::मानम्_2;

#यदि_अन्तः

/** @internal */
प्रतिरूपम् <typename T> संरचना is_basic_string_view : std::false_type {};

/** @internal */
प्रतिरूपम् <typename... तर्काः>
संरचना is_basic_string_view<std::basic_string_view<तर्काः...>> : std::true_type {};

/** @internal */
प्रतिरूपम् <typename T> constexpr bool is_basic_string_view_v = is_basic_string_view<T>::मानम्_2;

/** @internal */
प्रतिरूपम्<typename CP32, bool skipSurrogates>
class संहिता_Points_Iterator {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
सार्वजनिक:
    /** C++ iterator boilerplate @internal */
    using मानम्_type = CP32;
    /** C++ iterator boilerplate @internal */
    using reference = मानम्_type;
    /** C++ iterator boilerplate @internal */
    using pointer = CP32 *;
    /** C++ iterator boilerplate @internal */
    using difference_type = int32_t;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::forward_iterator_चिह्नकम्;

    /** @internal */
    अन्तःस्थ संहिता_Points_Iterator(CP32 c) : c_(c) {}
    /** @internal */
    अन्तःस्थ bool संक्रियकः==(ध्रुव संहिता_Points_Iterator &other) ध्रुव { प्रत्यागम c_ == other.c_; }
    /** @internal */
    अन्तःस्थ bool संक्रियकः!=(ध्रुव संहिता_Points_Iterator &other) ध्रुव { प्रत्यागम !संक्रियकः==(other); }
    /** @internal */
    अन्तःस्थ CP32 संक्रियकः*() ध्रुव { प्रत्यागम c_; }
    /** @internal */
    अन्तःस्थ संहिता_Points_Iterator& संक्रियकः++() U_LIFETIME_BOUND {  // pre-increment
        ++c_;
        यदि (skipSurrogates && c_ == 0xd800) {
            c_ = 0xe000;
        }
        प्रत्यागम *this;
    }
    /** @internal */
    अन्तःस्थ संहिता_Points_Iterator संक्रियकः++(पूर्णाङ्क) {  // post-increment
        संहिता_Points_Iterator परिणाम(*this);
        ++(*this);
        प्रत्यागम परिणाम;
    }

गुप्त:
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
प्रतिरूपम्<typename CP32>
class All_संहिता_Points {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
सार्वजनिक:
    /** Constructor. @draft ICU 78 */
    All_संहिता_Points() {}
    /**
     * @return an iterator over all Unicode code points.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    स्वचालित begin() ध्रुव { प्रत्यागम prv::संहिता_Points_Iterator<CP32, false>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode code points.
     * @draft ICU 78
     */
    स्वचालित अन्तः() ध्रुव { प्रत्यागम prv::संहिता_Points_Iterator<CP32, false>(0x110000); }
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
प्रतिरूपम्<typename CP32>
class All_Scalar_मानानि {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
सार्वजनिक:
    /** Constructor. @draft ICU 78 */
    All_Scalar_मानानि() {}
    /**
     * @return an iterator over all Unicode scalar values.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    स्वचालित begin() ध्रुव { प्रत्यागम prv::संहिता_Points_Iterator<CP32, true>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode scalar values.
     * @draft ICU 78
     */
    स्वचालित अन्तः() ध्रुव { प्रत्यागम prv::संहिता_Points_Iterator<CP32, true>(0x110000); }
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
प्रतिरूपम्<typename CP32, typename UnitIter, typename = शून्य>
class Unsafe_संहिता_Units {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Unit = typename prv::iter_मानम्_t<UnitIter>;
सार्वजनिक:
    /** @internal */
    Unsafe_संहिता_Units(CP32 संहिता_Point, uint8_t दीर्घता, UnitIter start, UnitIter सीमा) :
            c_(संहिता_Point), len_(दीर्घता), start_(start), सीमा_3(सीमा) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_संहिता_Units(ध्रुव Unsafe_संहिता_Units &other) = स्वभाव;
    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_संहिता_Units &संक्रियकः=(ध्रुव Unsafe_संहिता_Units &other) = स्वभाव;

    /**
     * @return the Unicode code point decoded from the code unit sequence.
     *     If the sequence is ill-formed and the iterator validates,
     *     then this is a replacement value according to the iterator‘s
     *     UTFIllFormedBehavior template parameter.
     * @draft ICU 78
     */
    CP32 संहिता_Point() ध्रुव { प्रत्यागम c_; }

    /**
     * @return the start of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter begin() ध्रुव { प्रत्यागम start_; }

    /**
     * @return the limit (exclusive end) of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter अन्तः() ध्रुव { प्रत्यागम सीमा_3; }

    /**
     * @return the length of the code unit sequence for one code point.
     * @draft ICU 78
     */
    uint8_t दीर्घता() ध्रुव { प्रत्यागम len_; }

#यदि U_CPLUSPLUS_संस्करणम् >= 20
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    प्रतिरूपम्<std::contiguous_iterator Iter = UnitIter>
    std::basic_string_view<Unit> stringView() ध्रुव {
        प्रत्यागम std::basic_string_view<Unit>(begin(), अन्तः());
    }
#अन्यथा
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    प्रतिरूपम्<typename Iter = UnitIter, typename Unit = typename std::iterator_traits<Iter>::मानम्_type>
    std::enable_if_t<std::is_pointer_v<Iter> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::const_iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::const_iterator>,
                     std::basic_string_view<Unit>>
    stringView() ध्रुव {
        प्रत्यागम std::basic_string_view<Unit>(&*start_, len_);
    }
#यदि_अन्तः

गुप्त:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
    UnitIter start_;
    UnitIter सीमा_3;
};

#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
प्रतिरूपम्<typename CP32, typename UnitIter>
class Unsafe_संहिता_Units<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
सार्वजनिक:
    Unsafe_संहिता_Units(CP32 संहिता_Point, uint8_t दीर्घता) : c_(संहिता_Point), len_(दीर्घता) {}

    Unsafe_संहिता_Units(ध्रुव Unsafe_संहिता_Units &other) = स्वभाव;
    Unsafe_संहिता_Units &संक्रियकः=(ध्रुव Unsafe_संहिता_Units &other) = स्वभाव;

    CP32 संहिता_Point() ध्रुव { प्रत्यागम c_; }

    uint8_t दीर्घता() ध्रुव { प्रत्यागम len_; }

गुप्त:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
};
#यदि_अन्तः  // U_IN_DOXYGEN

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
प्रतिरूपम्<typename CP32, typename UnitIter, typename = शून्य>
class संहिता_Units_2 : सार्वजनिक Unsafe_संहिता_Units<CP32, UnitIter> {
सार्वजनिक:
    /** @internal */
    संहिता_Units_2(CP32 संहिता_Point, uint8_t दीर्घता, bool wellFormed, UnitIter start, UnitIter सीमा) :
            Unsafe_संहिता_Units<CP32, UnitIter>(संहिता_Point, दीर्घता, start, सीमा), ok_(wellFormed) {}

    /** Copy constructor. @draft ICU 78 */
    संहिता_Units_2(ध्रुव संहिता_Units_2 &other) = स्वभाव;
    /** Copy assignment operator. @draft ICU 78 */
    संहिता_Units_2 &संक्रियकः=(ध्रुव संहिता_Units_2 &other) = स्वभाव;

    /**
     * @return true if the decoded code unit sequence is well-formed.
     * @draft ICU 78
     */
    bool wellFormed() ध्रुव { प्रत्यागम ok_; }

गुप्त:
    bool ok_;
};

#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
प्रतिरूपम्<typename CP32, typename UnitIter>
class संहिता_Units_2<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> :
            सार्वजनिक Unsafe_संहिता_Units<CP32, UnitIter> {
सार्वजनिक:
    संहिता_Units_2(CP32 संहिता_Point, uint8_t दीर्घता, bool wellFormed) :
            Unsafe_संहिता_Units<CP32, UnitIter>(संहिता_Point, दीर्घता), ok_(wellFormed) {}

    संहिता_Units_2(ध्रुव संहिता_Units_2 &other) = स्वभाव;
    संहिता_Units_2 &संक्रियकः=(ध्रुव संहिता_Units_2 &other) = स्वभाव;

    bool wellFormed() ध्रुव { प्रत्यागम ok_; }

गुप्त:
    bool ok_;
};
#यदि_अन्तः  // U_IN_DOXYGEN

// Validating implementations ---------------------------------------------- ***

#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename सीमा_Iter = UnitIter, typename = शून्य>
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
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename सीमा_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, सीमा_Iter,
        std::enable_if_t<परिमाण(typename prv::iter_मानम्_t<UnitIter>) == 1>> {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    static_assert(behavior != UTF_BEHAVIOR_SURROGATE,
                  "For 8-bit strings, the SURROGATE option does not have an equivalent.");
सार्वजनिक:
    // Handle ill-formed UTF-8
    U_FORCE_INLINE स्थिर CP32 sub() {
        यदि constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            प्रत्यागम U_SENTINEL;
        } अन्यथा {
            static_assert(behavior == UTF_BEHAVIOR_FFFD);
            प्रत्यागम 0xfffd;
        }
    }

    U_FORCE_INLINE स्थिर शून्य inc(UnitIter &p, ध्रुव सीमा_Iter &सीमा) {
        // Very similar to U8_FWD_1().
        uint8_t b = *p;
        ++p;
        यदि (U8_IS_LEAD(b) && p != सीमा) {
            uint8_t t1 = *p;
            यदि ((0xe0 <= b && b < 0xf0)) {
                यदि (U8_IS_VALID_LEAD3_AND_T1(b, t1) &&
                        ++p != सीमा && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            } अन्यथा यदि (b < 0xe0) {
                यदि (U8_IS_TRAIL(t1)) {
                    ++p;
                }
            } अन्यथा /* b >= 0xf0 */ {
                यदि (U8_IS_VALID_LEAD4_AND_T1(b, t1) &&
                        ++p != सीमा && U8_IS_TRAIL(*p) &&
                        ++p != सीमा && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            }
        }
    }

    U_FORCE_INLINE स्थिर शून्य dec(UnitIter start, UnitIter &p) {
        // Very similar to U8_BACK_1().
        uint8_t c = *--p;
        यदि (U8_IS_TRAIL(c) && p != start) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            यदि (U8_IS_LEAD(b1)) {
                यदि (b1 < 0xe0 ||
                        (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c))) {
                    p = p1;
                    प्रत्यागम;
                }
            } अन्यथा यदि (U8_IS_TRAIL(b1) && p1 != start) {
                uint8_t b2 = *--p1;
                यदि (0xe0 <= b2 && b2 <= 0xf4) {
                    यदि (b2 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b2, b1) :
                            U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        p = p1;
                        प्रत्यागम;
                    }
                } अन्यथा यदि (U8_IS_TRAIL(b2) && p1 != start) {
                    uint8_t b3 = *--p1;
                    यदि (0xf0 <= b3 && b3 <= 0xf4 && U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                        p = p1;
                        प्रत्यागम;
                    }
                }
            }
        }
    }

    U_FORCE_INLINE स्थिर संहिता_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ध्रुव सीमा_Iter &सीमा) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_OR_FFFD().
        CP32 c = uint8_t(*p);
        ++p;
        यदि (U8_IS_SINGLE(c)) {
            यदि constexpr (isMultiPass) {
                प्रत्यागम {c, 1, true, p0, p};
            } अन्यथा {
                प्रत्यागम {c, 1, true};
            }
        }
        uint8_t दीर्घता = 1;
        uint8_t t = 0;
        यदि (p != सीमा &&
                // fetch/validate/assemble all but last trail byte
                (c >= 0xe0 ?
                    (c < 0xf0 ?  // U+0800..U+FFFF except surrogates
                        U8_LEAD3_T1_द्वयङ्काः[c &= 0xf] & (1 << ((t = *p) >> 5)) &&
                        (t &= 0x3f, 1)
                    :  // U+10000..U+10FFFF
                        (c -= 0xf0) <= 4 &&
                        U8_LEAD4_T1_द्वयङ्काः[(t = *p) >> 4] & (1 << c) &&
                        (c = (c << 6) | (t & 0x3f), ++दीर्घता, ++p != सीमा) &&
                        (t = *p - 0x80) <= 0x3f) &&
                    // valid second-to-last trail byte
                    (c = (c << 6) | t, ++दीर्घता, ++p != सीमा)
                :  // U+0080..U+07FF
                    c >= 0xc2 && (c &= 0x1f, 1)) &&
                // last trail byte
                (t = *p - 0x80) <= 0x3f) {
            c = (c << 6) | t;
            ++दीर्घता;
            ++p;
            यदि constexpr (isMultiPass) {
                प्रत्यागम {c, दीर्घता, true, p0, p};
            } अन्यथा {
                प्रत्यागम {c, दीर्घता, true};
            }
        }
        यदि constexpr (isMultiPass) {
            प्रत्यागम {sub(), दीर्घता, false, p0, p};
        } अन्यथा {
            प्रत्यागम {sub(), दीर्घता, false};
        }
    }

    U_FORCE_INLINE स्थिर संहिता_Units_2<CP32, UnitIter> decAndRead(UnitIter start, UnitIter &p) {
        // Very similar to U8_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        यदि (U8_IS_SINGLE(c)) {
            प्रत्यागम {c, 1, true, p, p0};
        }
        यदि (U8_IS_TRAIL(c) && p != start) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            यदि (U8_IS_LEAD(b1)) {
                यदि (b1 < 0xe0) {
                    p = p1;
                    c = ((b1 - 0xc0) << 6) | (c & 0x3f);
                    प्रत्यागम {c, 2, true, p, p0};
                } अन्यथा यदि (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c)) {
                    // Truncated 3- or 4-byte sequence.
                    p = p1;
                    प्रत्यागम {sub(), 2, false, p, p0};
                }
            } अन्यथा यदि (U8_IS_TRAIL(b1) && p1 != start) {
                // Extract the value bits from the last trail byte.
                c &= 0x3f;
                uint8_t b2 = *--p1;
                यदि (0xe0 <= b2 && b2 <= 0xf4) {
                    यदि (b2 < 0xf0) {
                        b2 &= 0xf;
                        यदि (U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                            p = p1;
                            c = (b2 << 12) | ((b1 & 0x3f) << 6) | c;
                            प्रत्यागम {c, 3, true, p, p0};
                        }
                    } अन्यथा यदि (U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        // Truncated 4-byte sequence.
                        p = p1;
                        प्रत्यागम {sub(), 3, false, p, p0};
                    }
                } अन्यथा यदि (U8_IS_TRAIL(b2) && p1 != start) {
                    uint8_t b3 = *--p1;
                    यदि (0xf0 <= b3 && b3 <= 0xf4) {
                        b3 &= 7;
                        यदि (U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                            p = p1;
                            c = (b3 << 18) | ((b2 & 0x3f) << 12) | ((b1 & 0x3f) << 6) | c;
                            प्रत्यागम {c, 4, true, p, p0};
                        }
                    }
                }
            }
        }
        प्रत्यागम {sub(), 1, false, p, p0};
    }
};

// UTF-16
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename सीमा_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, सीमा_Iter,
        std::enable_if_t<परिमाण(typename prv::iter_मानम्_t<UnitIter>) == 2>> {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
सार्वजनिक:
    // Handle ill-formed UTF-16: One unpaired surrogate.
    U_FORCE_INLINE स्थिर CP32 sub(CP32 surrogate) {
        यदि constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            प्रत्यागम U_SENTINEL;
        } अन्यथा यदि constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            प्रत्यागम 0xfffd;
        } अन्यथा {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            प्रत्यागम surrogate;
        }
    }

    U_FORCE_INLINE स्थिर शून्य inc(UnitIter &p, ध्रुव सीमा_Iter &सीमा) {
        // Very similar to U16_FWD_1().
        स्वचालित c = *p;
        ++p;
        यदि (U16_IS_LEAD(c) && p != सीमा && U16_IS_TRAIL(*p)) {
            ++p;
        }
    }

    U_FORCE_INLINE स्थिर शून्य dec(UnitIter start, UnitIter &p) {
        // Very similar to U16_BACK_1().
        UnitIter p1;
        यदि (U16_IS_TRAIL(*--p) && p != start && (p1 = p, U16_IS_LEAD(*--p1))) {
            p = p1;
        }
    }

    U_FORCE_INLINE स्थिर संहिता_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ध्रुव सीमा_Iter &सीमा) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_OR_FFFD().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        यदि (!U16_IS_SURROGATE(c)) {
            यदि constexpr (isMultiPass) {
                प्रत्यागम {c, 1, true, p0, p};
            } अन्यथा {
                प्रत्यागम {c, 1, true};
            }
        } अन्यथा {
            uint16_t c2;
            यदि (U16_IS_SURROGATE_LEAD(c) && p != सीमा && U16_IS_TRAIL(c2 = *p)) {
                ++p;
                c = U16_GET_SUPPLEMENTARY(c, c2);
                यदि constexpr (isMultiPass) {
                    प्रत्यागम {c, 2, true, p0, p};
                } अन्यथा {
                    प्रत्यागम {c, 2, true};
                }
            } अन्यथा {
                यदि constexpr (isMultiPass) {
                    प्रत्यागम {sub(c), 1, false, p0, p};
                } अन्यथा {
                    प्रत्यागम {sub(c), 1, false};
                }
            }
        }
    }

    U_FORCE_INLINE स्थिर संहिता_Units_2<CP32, UnitIter> decAndRead(UnitIter start, UnitIter &p) {
        // Very similar to U16_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        यदि (!U16_IS_SURROGATE(c)) {
            प्रत्यागम {c, 1, true, p, p0};
        } अन्यथा {
            UnitIter p1;
            uint16_t c2;
            यदि (U16_IS_SURROGATE_TRAIL(c) && p != start && (p1 = p, U16_IS_LEAD(c2 = *--p1))) {
                p = p1;
                c = U16_GET_SUPPLEMENTARY(c2, c);
                प्रत्यागम {c, 2, true, p, p0};
            } अन्यथा {
                प्रत्यागम {sub(c), 1, false, p, p0};
            }
        }
    }
};

// UTF-32: trivial, but still validating
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename सीमा_Iter>
class UTFImpl<
        CP32, behavior,
        UnitIter, सीमा_Iter,
        std::enable_if_t<परिमाण(typename prv::iter_मानम्_t<UnitIter>) == 4>> {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
सार्वजनिक:
    // Handle ill-formed UTF-32
    U_FORCE_INLINE स्थिर CP32 sub(bool forSurrogate, CP32 surrogate) {
        यदि constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            प्रत्यागम U_SENTINEL;
        } अन्यथा यदि constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            प्रत्यागम 0xfffd;
        } अन्यथा {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            प्रत्यागम forSurrogate ? surrogate : 0xfffd;
        }
    }

    U_FORCE_INLINE स्थिर शून्य inc(UnitIter &p, ध्रुव सीमा_Iter &/*सीमा*/) {
        ++p;
    }

    U_FORCE_INLINE स्थिर शून्य dec(UnitIter /*start*/, UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE स्थिर संहिता_Units_2<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, ध्रुव सीमा_Iter &/*सीमा*/) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        uint32_t uc = *p;
        CP32 c = uc;
        ++p;
        यदि (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            यदि constexpr (isMultiPass) {
                प्रत्यागम {c, 1, true, p0, p};
            } अन्यथा {
                प्रत्यागम {c, 1, true};
            }
        } अन्यथा {
            यदि constexpr (isMultiPass) {
                प्रत्यागम {sub(uc < 0xe000, c), 1, false, p0, p};
            } अन्यथा {
                प्रत्यागम {sub(uc < 0xe000, c), 1, false};
            }
        }
    }

    U_FORCE_INLINE स्थिर संहिता_Units_2<CP32, UnitIter> decAndRead(UnitIter /*start*/, UnitIter &p) {
        UnitIter p0 = p;
        uint32_t uc = *--p;
        CP32 c = uc;
        यदि (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            प्रत्यागम {c, 1, true, p, p0};
        } अन्यथा {
            प्रत्यागम {sub(uc < 0xe000, c), 1, false, p, p0};
        }
    }
};

// Non-validating implementations ------------------------------------------ ***

प्रतिरूपम्<typename CP32, typename UnitIter, typename = शून्य>
class UnsafeUTFImpl;

// UTF-8
प्रतिरूपम्<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<परिमाण(typename prv::iter_मानम्_t<UnitIter>) == 1>> {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
सार्वजनिक:
    U_FORCE_INLINE स्थिर शून्य inc(UnitIter &p) {
        // Very similar to U8_FWD_1_UNSAFE().
        uint8_t b = *p;
        std::advance(p, 1 + U8_गणनम्_TRAIL_अष्टकानि_UNSAFE(b));
    }

    U_FORCE_INLINE स्थिर शून्य dec(UnitIter &p) {
        // Very similar to U8_BACK_1_UNSAFE().
        यावत् (U8_IS_TRAIL(*--p)) {}
    }

    U_FORCE_INLINE स्थिर Unsafe_संहिता_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_UNSAFE().
        CP32 c = uint8_t(*p);
        ++p;
        यदि (U8_IS_SINGLE(c)) {
            यदि constexpr (isMultiPass) {
                प्रत्यागम {c, 1, p0, p};
            } अन्यथा {
                प्रत्यागम {c, 1};
            }
        } अन्यथा यदि (c < 0xe0) {
            c = ((c & 0x1f) << 6) | (*p & 0x3f);
            ++p;
            यदि constexpr (isMultiPass) {
                प्रत्यागम {c, 2, p0, p};
            } अन्यथा {
                प्रत्यागम {c, 2};
            }
        } अन्यथा यदि (c < 0xf0) {
            // No need for (c&0xf) because the upper bits are truncated
            // after <<12 in the cast to uint16_t.
            c = uint16_t(c << 12) | ((*p & 0x3f) << 6);
            ++p;
            c |= *p & 0x3f;
            ++p;
            यदि constexpr (isMultiPass) {
                प्रत्यागम {c, 3, p0, p};
            } अन्यथा {
                प्रत्यागम {c, 3};
            }
        } अन्यथा {
            c = ((c & 7) << 18) | ((*p & 0x3f) << 12);
            ++p;
            c |= (*p & 0x3f) << 6;
            ++p;
            c |= *p & 0x3f;
            ++p;
            यदि constexpr (isMultiPass) {
                प्रत्यागम {c, 4, p0, p};
            } अन्यथा {
                प्रत्यागम {c, 4};
            }
        }
    }

    U_FORCE_INLINE स्थिर Unsafe_संहिता_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U8_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        यदि (U8_IS_SINGLE(c)) {
            प्रत्यागम {c, 1, p, p0};
        }
        // U8_IS_TRAIL(c) if well-formed
        c &= 0x3f;
        uint8_t गणनम् = 1;
        प्रति (uint8_t सरणम् = 6;;) {
            uint8_t b = *--p;
            यदि (b >= 0xc0) {
                U8_आवरणम्_LEAD_BYTE(b, गणनम्);
                c |= uint32_t{b} << सरणम्;
                भङ्ग;
            } अन्यथा {
                c |= (uint32_t{b} & 0x3f) << सरणम्;
                ++गणनम्;
                सरणम् += 6;
            }
        }
        ++गणनम्;
        प्रत्यागम {c, गणनम्, p, p0};
    }
};

// UTF-16
प्रतिरूपम्<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<परिमाण(typename prv::iter_मानम्_t<UnitIter>) == 2>> {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
सार्वजनिक:
    U_FORCE_INLINE स्थिर शून्य inc(UnitIter &p) {
        // Very similar to U16_FWD_1_UNSAFE().
        स्वचालित c = *p;
        ++p;
        यदि (U16_IS_LEAD(c)) {
            ++p;
        }
    }

    U_FORCE_INLINE स्थिर शून्य dec(UnitIter &p) {
        // Very similar to U16_BACK_1_UNSAFE().
        यदि (U16_IS_TRAIL(*--p)) {
            --p;
        }
    }

    U_FORCE_INLINE स्थिर Unsafe_संहिता_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_UNSAFE().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        यदि (!U16_IS_LEAD(c)) {
            यदि constexpr (isMultiPass) {
                प्रत्यागम {c, 1, p0, p};
            } अन्यथा {
                प्रत्यागम {c, 1};
            }
        } अन्यथा {
            uint16_t c2 = *p;
            ++p;
            c = U16_GET_SUPPLEMENTARY(c, c2);
            यदि constexpr (isMultiPass) {
                प्रत्यागम {c, 2, p0, p};
            } अन्यथा {
                प्रत्यागम {c, 2};
            }
        }
    }

    U_FORCE_INLINE स्थिर Unsafe_संहिता_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U16_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        यदि (!U16_IS_TRAIL(c)) {
            प्रत्यागम {c, 1, p, p0};
        } अन्यथा {
            uint16_t c2 = *--p;
            c = U16_GET_SUPPLEMENTARY(c2, c);
            प्रत्यागम {c, 2, p, p0};
        }
    }
};

// UTF-32: trivial
प्रतिरूपम्<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<परिमाण(typename prv::iter_मानम्_t<UnitIter>) == 4>> {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
सार्वजनिक:
    U_FORCE_INLINE स्थिर शून्य inc(UnitIter &p) {
        ++p;
    }

    U_FORCE_INLINE स्थिर शून्य dec(UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE स्थिर Unsafe_संहिता_Units<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        CP32 c = *p;
        ++p;
        यदि constexpr (isMultiPass) {
            प्रत्यागम {c, 1, p0, p};
        } अन्यथा {
            प्रत्यागम {c, 1};
        }
    }

    U_FORCE_INLINE स्थिर Unsafe_संहिता_Units<CP32, UnitIter> decAndRead(UnitIter &p) {
        UnitIter p0 = p;
        CP32 c = *--p;
        प्रत्यागम {c, 1, p, p0};
    }
};

#यदि_अन्तः

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
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename सीमा_Iter = UnitIter, typename = शून्य>
class UTFIterator {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, सीमा_Iter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    सार्वजनिक:
        explicit Proxy(संहिता_Units_2<CP32, UnitIter> &units) : units_(units) {}
        संहिता_Units_2<CP32, UnitIter>& संक्रियकः*() U_LIFETIME_BOUND { प्रत्यागम units_; }
        संहिता_Units_2<CP32, UnitIter>* संक्रियकः->() U_LIFETIME_BOUND { प्रत्यागम &units_; }
    गुप्त:
        संहिता_Units_2<CP32, UnitIter> units_;
    };

सार्वजनिक:
    /** C++ iterator boilerplate @internal */
    using मानम्_type = संहिता_Units_2<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = मानम्_type;
    /** C++ iterator boilerplate @internal */
    using pointer = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_चिह्नकम्,
        std::forward_iterator_चिह्नकम्>;

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
    U_FORCE_INLINE UTFIterator(UnitIter start, UnitIter p, सीमा_Iter सीमा) :
            p_(p), start_(start), सीमा_3(सीमा), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE UTFIterator(UnitIter p, सीमा_Iter सीमा) :
            p_(p), start_(p), सीमा_3(सीमा), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(p), start_(p), सीमा_3(p), units_(0, 0, false, p, p) {}
    /**
     * Default constructor. Makes a non-functional iterator.
     *
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator() : p_{}, start_{}, सीमा_3{}, units_(0, 0, false, p_, p_) {}

    /** Move constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = स्वभाव;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &संक्रियकः=(UTFIterator &&src) noexcept = स्वभाव;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(ध्रुव UTFIterator &other) = स्वभाव;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &संक्रियकः=(ध्रुव UTFIterator &other) = स्वभाव;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool संक्रियकः==(ध्रुव UTFIterator &other) ध्रुव {
        प्रत्यागम आधारः() == other.आधारः();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool संक्रियकः!=(ध्रुव UTFIterator &other) ध्रुव { प्रत्यागम !संक्रियकः==(other); }

    // Asymmetric equality & nonequality with a sentinel type.

    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः==(ध्रुव UTFIterator &iter, ध्रुव Sentinel &s) {
        प्रत्यागम iter.आधारः() == s;
    }

#यदि U_CPLUSPLUS_संस्करणम् < 20
    // C++17: Need to define all four combinations of == / != vs. parameter order.
    // Once we require C++20, we could remove all but the first == because
    // the compiler would generate the rest.

    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः==(ध्रुव Sentinel &s, ध्रुव UTFIterator &iter) {
        प्रत्यागम iter.आधारः() == s;
    }
    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः!=(ध्रुव UTFIterator &iter, ध्रुव Sentinel &s) { प्रत्यागम !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः!=(ध्रुव Sentinel &s, ध्रुव UTFIterator &iter) { प्रत्यागम !(iter == s); }
#यदि_अन्तः  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter आधारः() ध्रुव {
        // Return the logical position.
        प्रत्यागम state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE संहिता_Units_2<CP32, UnitIter> संक्रियकः*() ध्रुव {
        यदि (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, सीमा_3);
            state_ = 1;
        }
        प्रत्यागम units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy संक्रियकः->() ध्रुव {
        यदि (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, सीमा_3);
            state_ = 1;
        }
        प्रत्यागम Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator& संक्रियकः++() U_LIFETIME_BOUND {  // pre-increment
        यदि (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } अन्यथा यदि (state_ == 0) {
            Impl::inc(p_, सीमा_3);
        } अन्यथा /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.अन्तः();
            state_ = 0;
        }
        प्रत्यागम *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator संक्रियकः++(पूर्णाङ्क) {  // post-increment
        यदि (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UTFIterator परिणाम(*this);
            state_ = 0;
            प्रत्यागम परिणाम;
        } अन्यथा यदि (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, सीमा_3);
            UTFIterator परिणाम(*this);
            परिणाम.state_ = 1;
            // keep this->state_ == 0
            प्रत्यागम परिणाम;
        } अन्यथा /* state_ < 0 */ {
            UTFIterator परिणाम(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.अन्तः();
            state_ = 0;
            प्रत्यागम परिणाम;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    प्रतिरूपम् <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator&>
    संक्रियकः--() U_LIFETIME_BOUND {  // pre-decrement
        यदि (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(start_, p_);
        state_ = -1;
        प्रत्यागम *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    प्रतिरूपम्<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator>
    संक्रियकः--(पूर्णाङ्क) {  // post-decrement
        UTFIterator परिणाम(*this);
        संक्रियकः--();
        प्रत्यागम परिणाम;
    }

गुप्त:
    मित्रम् class std::reverse_iterator<UTFIterator<CP32, behavior, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter start_;
    सीमा_Iter सीमा_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable संहिता_Units_2<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
// Partial template specialization for single-pass input iterator.
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename सीमा_Iter>
class UTFIterator<
        CP32, behavior,
        UnitIter, सीमा_Iter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, सीमा_Iter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    सार्वजनिक:
        explicit Proxy(संहिता_Units_2<CP32, UnitIter> &units) : units_(units) {}
        संहिता_Units_2<CP32, UnitIter>& संक्रियकः*() U_LIFETIME_BOUND { प्रत्यागम units_; }
        संहिता_Units_2<CP32, UnitIter>* संक्रियकः->() U_LIFETIME_BOUND { प्रत्यागम &units_; }
    गुप्त:
        संहिता_Units_2<CP32, UnitIter> units_;
    };

सार्वजनिक:
    using मानम्_type = संहिता_Units_2<CP32, UnitIter>;
    using reference = मानम्_type;
    using pointer = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::निवेशः_iterator_चिह्नकम्;

    U_FORCE_INLINE UTFIterator(UnitIter p, सीमा_Iter सीमा) : p_(std::move(p)), सीमा_3(std::move(सीमा)) {}

    // Constructs an iterator start or limit sentinel.
    // Requires p to be copyable.
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(std::move(p)), सीमा_3(p_) {}

    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = स्वभाव;
    U_FORCE_INLINE UTFIterator &संक्रियकः=(UTFIterator &&src) noexcept = स्वभाव;

    U_FORCE_INLINE UTFIterator(ध्रुव UTFIterator &other) = स्वभाव;
    U_FORCE_INLINE UTFIterator &संक्रियकः=(ध्रुव UTFIterator &other) = स्वभाव;

    U_FORCE_INLINE bool संक्रियकः==(ध्रुव UTFIterator &other) ध्रुव {
        प्रत्यागम p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool संक्रियकः!=(ध्रुव UTFIterator &other) ध्रुव { प्रत्यागम !संक्रियकः==(other); }

    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः==(ध्रुव UTFIterator &iter, ध्रुव Sentinel &s) {
        प्रत्यागम !iter.ahead_ && iter.p_ == s;
    }

#यदि U_CPLUSPLUS_संस्करणम् < 20
    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः==(ध्रुव Sentinel &s, ध्रुव UTFIterator &iter) {
        प्रत्यागम !iter.ahead_ && iter.p_ == s;
    }

    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः!=(ध्रुव UTFIterator &iter, ध्रुव Sentinel &s) { प्रत्यागम !(iter == s); }

    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः!=(ध्रुव Sentinel &s, ध्रुव UTFIterator &iter) { प्रत्यागम !(iter == s); }
#यदि_अन्तः  // C++17

    U_FORCE_INLINE संहिता_Units_2<CP32, UnitIter> संक्रियकः*() ध्रुव {
        यदि (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, सीमा_3);
            ahead_ = true;
        }
        प्रत्यागम units_;
    }

    U_FORCE_INLINE Proxy संक्रियकः->() ध्रुव {
        यदि (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, सीमा_3);
            ahead_ = true;
        }
        प्रत्यागम Proxy(units_);
    }

    U_FORCE_INLINE UTFIterator& संक्रियकः++() U_LIFETIME_BOUND {  // pre-increment
        यदि (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } अन्यथा {
            Impl::inc(p_, सीमा_3);
        }
        प्रत्यागम *this;
    }

    U_FORCE_INLINE Proxy संक्रियकः++(पूर्णाङ्क) {  // post-increment
        यदि (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } अन्यथा {
            units_ = Impl::readAndInc(p_, p_, सीमा_3);
            // keep this->ahead_ == false
        }
        प्रत्यागम Proxy(units_);
    }

गुप्त:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need limit_ so that when we read a code point
    // we can test if there are enough code units.
    सीमा_Iter सीमा_3;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable संहिता_Units_2<CP32, UnitIter> units_ = {0, 0, false};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#यदि_अन्तः  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
class std::reverse_iterator<U_शीर्षकम्_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>> {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_शीर्षकम्_ONLY_NAMESPACE::UTFImpl<CP32, behavior, UnitIter>;
    using संहिता_Units_3 = U_शीर्षकम्_ONLY_NAMESPACE::संहिता_Units_2<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    सार्वजनिक:
        explicit Proxy(संहिता_Units_3 units) : units_(units) {}
        संहिता_Units_3& संक्रियकः*() U_LIFETIME_BOUND { प्रत्यागम units_; }
        संहिता_Units_3* संक्रियकः->() U_LIFETIME_BOUND { प्रत्यागम &units_; }
    गुप्त:
        संहिता_Units_3 units_;
    };

सार्वजनिक:
    using मानम्_type = संहिता_Units_3;
    using reference = मानम्_type;
    using pointer = Proxy;
    using difference_type = U_शीर्षकम्_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_चिह्नकम्;

    U_FORCE_INLINE explicit reverse_iterator(U_शीर्षकम्_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> iter) :
            p_(iter.आधारः()), start_(iter.start_), सीमा_3(iter.सीमा_3),
            units_(0, 0, false, p_, p_) {}
    U_FORCE_INLINE reverse_iterator() : p_{}, start_{}, सीमा_3{}, units_(0, 0, false, p_, p_) {}

    U_FORCE_INLINE reverse_iterator(reverse_iterator &&src) noexcept = स्वभाव;
    U_FORCE_INLINE reverse_iterator &संक्रियकः=(reverse_iterator &&src) noexcept = स्वभाव;

    U_FORCE_INLINE reverse_iterator(ध्रुव reverse_iterator &other) = स्वभाव;
    U_FORCE_INLINE reverse_iterator &संक्रियकः=(ध्रुव reverse_iterator &other) = स्वभाव;

    U_FORCE_INLINE bool संक्रियकः==(ध्रुव reverse_iterator &other) ध्रुव {
        प्रत्यागम getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool संक्रियकः!=(ध्रुव reverse_iterator &other) ध्रुव { प्रत्यागम !संक्रियकः==(other); }

    U_FORCE_INLINE U_शीर्षकम्_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> आधारः() ध्रुव {
        प्रत्यागम U_शीर्षकम्_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>(
            start_, getLogicalPosition(), सीमा_3);
    }

    U_FORCE_INLINE संहिता_Units_3 संक्रियकः*() ध्रुव {
        यदि (state_ == 0) {
            units_ = Impl::decAndRead(start_, p_);
            state_ = -1;
        }
        प्रत्यागम units_;
    }

    U_FORCE_INLINE Proxy संक्रियकः->() ध्रुव {
        यदि (state_ == 0) {
            units_ = Impl::decAndRead(start_, p_);
            state_ = -1;
        }
        प्रत्यागम Proxy(units_);
    }

    U_FORCE_INLINE reverse_iterator& संक्रियकः++() U_LIFETIME_BOUND {  // pre-increment
        यदि (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } अन्यथा यदि (state_ == 0) {
            Impl::dec(start_, p_);
        } अन्यथा /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        प्रत्यागम *this;
    }

    U_FORCE_INLINE reverse_iterator संक्रियकः++(पूर्णाङ्क) {  // post-increment
        यदि (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            reverse_iterator परिणाम(*this);
            state_ = 0;
            प्रत्यागम परिणाम;
        } अन्यथा यदि (state_ == 0) {
            units_ = Impl::decAndRead(start_, p_);
            reverse_iterator परिणाम(*this);
            परिणाम.state_ = -1;
            // keep this->state_ == 0
            प्रत्यागम परिणाम;
        } अन्यथा /* state_ > 0 */ {
            reverse_iterator परिणाम(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            प्रत्यागम परिणाम;
        }
    }

    U_FORCE_INLINE reverse_iterator& संक्रियकः--() U_LIFETIME_BOUND {  // pre-decrement
        यदि (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.अन्तः();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_, सीमा_3);
        state_ = 1;
        प्रत्यागम *this;
    }

    U_FORCE_INLINE reverse_iterator संक्रियकः--(पूर्णाङ्क) {  // post-decrement
        reverse_iterator परिणाम(*this);
        संक्रियकः--();
        प्रत्यागम परिणाम;
    }

गुप्त:
    U_FORCE_INLINE UnitIter getLogicalPosition() ध्रुव {
        प्रत्यागम state_ >= 0 ? p_ : units_.अन्तः();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter start_;
    UnitIter सीमा_3;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable संहिता_Units_3 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#यदि_अन्तः  // U_IN_DOXYGEN

namespace U_शीर्षकम्_ONLY_NAMESPACE {

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
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename सीमा_Iter = UnitIter>
स्वचालित utfIterator(UnitIter start, UnitIter p, सीमा_Iter सीमा) {
    प्रत्यागम UTFIterator<CP32, behavior, UnitIter, सीमा_Iter>(
        std::move(start), std::move(p), std::move(सीमा));
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
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename सीमा_Iter = UnitIter>
स्वचालित utfIterator(UnitIter p, सीमा_Iter सीमा) {
    प्रत्यागम UTFIterator<CP32, behavior, UnitIter, सीमा_Iter>(
        std::move(p), std::move(सीमा));
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
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
स्वचालित utfIterator(UnitIter p) {
    प्रत्यागम UTFIterator<CP32, behavior, UnitIter>(std::move(p));
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
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior, typename Range>
#यदि defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#यदि_अन्तः
class UTFString_संहिता_Points {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
सार्वजनिक:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    UTFString_संहिता_Points() = स्वभाव;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    प्रतिरूपम्<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit UTFString_संहिता_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C+20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    प्रतिरूपम्<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = शून्य>
    explicit UTFString_संहिता_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    UTFString_संहिता_Points(ध्रुव UTFString_संहिता_Points &other) = स्वभाव;

    /** Copy assignment operator. @draft ICU 78 */
    UTFString_संहिता_Points &संक्रियकः=(ध्रुव UTFString_संहिता_Points &other) = स्वभाव;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    स्वचालित begin() {
        प्रत्यागम utfIterator<CP32, behavior>(unitRange.begin(), unitRange.अन्तः());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    प्रतिरूपम्<typename R = Range, typename = std::enable_if_t<prv::range<ध्रुव R>>>
    स्वचालित begin() ध्रुव {
        प्रत्यागम utfIterator<CP32, behavior>(unitRange.begin(), unitRange.अन्तः());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    स्वचालित अन्तः() {
        using UnitIter = decltype(unitRange.begin());
        using सीमा_Iter = decltype(unitRange.अन्तः());
        यदि constexpr (!std::is_same_v<UnitIter, सीमा_Iter>) {
            // Return the code unit sentinel.
            प्रत्यागम unitRange.अन्तः();
        } अन्यथा यदि constexpr (prv::bidirectional_iterator<UnitIter>) {
            प्रत्यागम utfIterator<CP32, behavior>(unitRange.begin(), unitRange.अन्तः(), unitRange.अन्तः());
        } अन्यथा {
            // The input iterator specialization has no three-argument constructor.
            प्रत्यागम utfIterator<CP32, behavior>(unitRange.अन्तः(), unitRange.अन्तः());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    प्रतिरूपम्<typename R = Range, typename = std::enable_if_t<prv::range<ध्रुव R>>>
    स्वचालित अन्तः() ध्रुव {
        using UnitIter = decltype(unitRange.begin());
        using सीमा_Iter = decltype(unitRange.अन्तः());
        यदि constexpr (!std::is_same_v<UnitIter, सीमा_Iter>) {
            // Return the code unit sentinel.
            प्रत्यागम unitRange.अन्तः();
        } अन्यथा यदि constexpr (prv::bidirectional_iterator<UnitIter>) {
            प्रत्यागम utfIterator<CP32, behavior>(unitRange.begin(), unitRange.अन्तः(), unitRange.अन्तः());
        } अन्यथा {
            // The input iterator specialization has no three-argument constructor.
            प्रत्यागम utfIterator<CP32, behavior>(unitRange.अन्तः(), unitRange.अन्तः());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    स्वचालित rbegin() ध्रुव {
        प्रत्यागम std::make_reverse_iterator(अन्तः());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    स्वचालित rend() ध्रुव {
        प्रत्यागम std::make_reverse_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    स्वचालित front() ध्रुव {
        प्रत्यागम *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    स्वचालित पृष्ठभागः() ध्रुव {
        प्रत्यागम *(--अन्तः());
    }

गुप्त:
    Range unitRange;
};

/** @internal */
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior>
संरचना UTFString_संहिता_Points_Adaptor
#यदि U_CPLUSPLUS_संस्करणम् >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_संवृतिः<UTFString_संहिता_Points_Adaptor<CP32, behavior>>
#यदि_अन्तः
{
    /** @internal */
    प्रतिरूपम्<typename Range>
    स्वचालित संक्रियकः()(Range &&unitRange) ध्रुव {
#यदि defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        प्रत्यागम UTFString_संहिता_Points<CP32, behavior, std::ranges::views::all_t<Range>>(
            std::forward<Range>(unitRange));
#अन्यथा
        यदि constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            प्रत्यागम UTFString_संहिता_Points<CP32, behavior, std::decay_t<Range>>(
                std::forward<Range>(unitRange));
        } अन्यथा {
            प्रत्यागम UTFString_संहिता_Points<CP32, behavior, Range>(std::forward<Range>(unitRange));
        }
#यदि_अन्तः
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
प्रतिरूपम्<typename CP32, UTFIllFormedBehavior behavior>
constexpr UTFString_संहिता_Points_Adaptor<CP32, behavior> utf_String_संहिता_Points;

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
प्रतिरूपम्<typename CP32, typename UnitIter, typename = शून्य>
class UnsafeUTFIterator {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    सार्वजनिक:
        explicit Proxy(Unsafe_संहिता_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_संहिता_Units<CP32, UnitIter>& संक्रियकः*() U_LIFETIME_BOUND { प्रत्यागम units_; }
        Unsafe_संहिता_Units<CP32, UnitIter>* संक्रियकः->() U_LIFETIME_BOUND { प्रत्यागम &units_; }
    गुप्त:
        Unsafe_संहिता_Units<CP32, UnitIter> units_;
    };

सार्वजनिक:
    /** C++ iterator boilerplate @internal */
    using मानम्_type = Unsafe_संहिता_Units<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = मानम्_type;
    /** C++ iterator boilerplate @internal */
    using pointer = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_चिह्नकम्,
        std::forward_iterator_चिह्नकम्>;

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
    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = स्वभाव;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &संक्रियकः=(UnsafeUTFIterator &&src) noexcept = स्वभाव;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator(ध्रुव UnsafeUTFIterator &other) = स्वभाव;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &संक्रियकः=(ध्रुव UnsafeUTFIterator &other) = स्वभाव;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool संक्रियकः==(ध्रुव UnsafeUTFIterator &other) ध्रुव {
        प्रत्यागम आधारः() == other.आधारः();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool संक्रियकः!=(ध्रुव UnsafeUTFIterator &other) ध्रुव { प्रत्यागम !संक्रियकः==(other); }

    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः==(ध्रुव UnsafeUTFIterator &iter, ध्रुव Sentinel &s) {
        प्रत्यागम iter.आधारः() == s;
    }

#यदि U_CPLUSPLUS_संस्करणम् < 20
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः==(ध्रुव Sentinel &s, ध्रुव UnsafeUTFIterator &iter) {
        प्रत्यागम iter.आधारः() == s;
    }
    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः!=(ध्रुव UnsafeUTFIterator &iter, ध्रुव Sentinel &s) { प्रत्यागम !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः!=(ध्रुव Sentinel &s, ध्रुव UnsafeUTFIterator &iter) { प्रत्यागम !(iter == s); }
#यदि_अन्तः  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter आधारः() ध्रुव {
        // Return the logical position.
        प्रत्यागम state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE Unsafe_संहिता_Units<CP32, UnitIter> संक्रियकः*() ध्रुव {
        यदि (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        प्रत्यागम units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy संक्रियकः->() ध्रुव {
        यदि (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        प्रत्यागम Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator& संक्रियकः++() U_LIFETIME_BOUND {  // pre-increment
        यदि (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } अन्यथा यदि (state_ == 0) {
            Impl::inc(p_);
        } अन्यथा /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.अन्तः();
            state_ = 0;
        }
        प्रत्यागम *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator संक्रियकः++(पूर्णाङ्क) {  // post-increment
        यदि (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UnsafeUTFIterator परिणाम(*this);
            state_ = 0;
            प्रत्यागम परिणाम;
        } अन्यथा यदि (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            UnsafeUTFIterator परिणाम(*this);
            परिणाम.state_ = 1;
            // keep this->state_ == 0
            प्रत्यागम परिणाम;
        } अन्यथा /* state_ < 0 */ {
            UnsafeUTFIterator परिणाम(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.अन्तः();
            state_ = 0;
            प्रत्यागम परिणाम;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    प्रतिरूपम् <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator&>
    संक्रियकः--() U_LIFETIME_BOUND {  // pre-decrement
        यदि (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(p_);
        state_ = -1;
        प्रत्यागम *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    प्रतिरूपम्<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator>
    संक्रियकः--(पूर्णाङ्क) {  // post-decrement
        UnsafeUTFIterator परिणाम(*this);
        संक्रियकः--();
        प्रत्यागम परिणाम;
    }

गुप्त:
    मित्रम् class std::reverse_iterator<UnsafeUTFIterator<CP32, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_संहिता_Units<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
// Partial template specialization for single-pass input iterator.
प्रतिरूपम्<typename CP32, typename UnitIter>
class UnsafeUTFIterator<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    सार्वजनिक:
        explicit Proxy(Unsafe_संहिता_Units<CP32, UnitIter> &units) : units_(units) {}
        Unsafe_संहिता_Units<CP32, UnitIter>& संक्रियकः*() U_LIFETIME_BOUND { प्रत्यागम units_; }
        Unsafe_संहिता_Units<CP32, UnitIter>* संक्रियकः->() U_LIFETIME_BOUND { प्रत्यागम &units_; }
    गुप्त:
        Unsafe_संहिता_Units<CP32, UnitIter> units_;
    };

सार्वजनिक:
    using मानम्_type = Unsafe_संहिता_Units<CP32, UnitIter>;
    using reference = मानम्_type;
    using pointer = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::निवेशः_iterator_चिह्नकम्;

    U_FORCE_INLINE explicit UnsafeUTFIterator(UnitIter p) : p_(std::move(p)) {}

    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = स्वभाव;
    U_FORCE_INLINE UnsafeUTFIterator &संक्रियकः=(UnsafeUTFIterator &&src) noexcept = स्वभाव;

    U_FORCE_INLINE UnsafeUTFIterator(ध्रुव UnsafeUTFIterator &other) = स्वभाव;
    U_FORCE_INLINE UnsafeUTFIterator &संक्रियकः=(ध्रुव UnsafeUTFIterator &other) = स्वभाव;

    U_FORCE_INLINE bool संक्रियकः==(ध्रुव UnsafeUTFIterator &other) ध्रुव {
        प्रत्यागम p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool संक्रियकः!=(ध्रुव UnsafeUTFIterator &other) ध्रुव { प्रत्यागम !संक्रियकः==(other); }

    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः==(ध्रुव UnsafeUTFIterator &iter, ध्रुव Sentinel &s) {
        प्रत्यागम !iter.ahead_ && iter.p_ == s;
    }

#यदि U_CPLUSPLUS_संस्करणम् < 20
    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः==(ध्रुव Sentinel &s, ध्रुव UnsafeUTFIterator &iter) {
        प्रत्यागम !iter.ahead_ && iter.p_ == s;
    }

    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः!=(ध्रुव UnsafeUTFIterator &iter, ध्रुव Sentinel &s) { प्रत्यागम !(iter == s); }

    प्रतिरूपम्<typename Sentinel> U_FORCE_INLINE मित्रम्
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    संक्रियकः!=(ध्रुव Sentinel &s, ध्रुव UnsafeUTFIterator &iter) { प्रत्यागम !(iter == s); }
#यदि_अन्तः  // C++17

    U_FORCE_INLINE Unsafe_संहिता_Units<CP32, UnitIter> संक्रियकः*() ध्रुव {
        यदि (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        प्रत्यागम units_;
    }

    U_FORCE_INLINE Proxy संक्रियकः->() ध्रुव {
        यदि (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        प्रत्यागम Proxy(units_);
    }

    U_FORCE_INLINE UnsafeUTFIterator& संक्रियकः++() U_LIFETIME_BOUND {  // pre-increment
        यदि (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } अन्यथा {
            Impl::inc(p_);
        }
        प्रत्यागम *this;
    }

    U_FORCE_INLINE Proxy संक्रियकः++(पूर्णाङ्क) {  // post-increment
        यदि (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } अन्यथा {
            units_ = Impl::readAndInc(p_, p_);
            // keep this->ahead_ == false
        }
        प्रत्यागम Proxy(units_);
    }

गुप्त:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable Unsafe_संहिता_Units<CP32, UnitIter> units_ = {0, 0};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#यदि_अन्तः  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
प्रतिरूपम्<typename CP32, typename UnitIter>
class std::reverse_iterator<U_शीर्षकम्_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>> {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_शीर्षकम्_ONLY_NAMESPACE::UnsafeUTFImpl<CP32, UnitIter>;
    using Unsafe_संहिता_Units_2 = U_शीर्षकम्_ONLY_NAMESPACE::Unsafe_संहिता_Units<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    सार्वजनिक:
        explicit Proxy(Unsafe_संहिता_Units_2 units) : units_(units) {}
        Unsafe_संहिता_Units_2& संक्रियकः*() U_LIFETIME_BOUND { प्रत्यागम units_; }
        Unsafe_संहिता_Units_2* संक्रियकः->() U_LIFETIME_BOUND { प्रत्यागम &units_; }
    गुप्त:
        Unsafe_संहिता_Units_2 units_;
    };

सार्वजनिक:
    using मानम्_type = Unsafe_संहिता_Units_2;
    using reference = मानम्_type;
    using pointer = Proxy;
    using difference_type = U_शीर्षकम्_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_चिह्नकम्;

    U_FORCE_INLINE explicit reverse_iterator(U_शीर्षकम्_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> iter) :
            p_(iter.आधारः()), units_(0, 0, p_, p_) {}
    U_FORCE_INLINE reverse_iterator() : p_{}, units_(0, 0, p_, p_) {}

    U_FORCE_INLINE reverse_iterator(reverse_iterator &&src) noexcept = स्वभाव;
    U_FORCE_INLINE reverse_iterator &संक्रियकः=(reverse_iterator &&src) noexcept = स्वभाव;

    U_FORCE_INLINE reverse_iterator(ध्रुव reverse_iterator &other) = स्वभाव;
    U_FORCE_INLINE reverse_iterator &संक्रियकः=(ध्रुव reverse_iterator &other) = स्वभाव;

    U_FORCE_INLINE bool संक्रियकः==(ध्रुव reverse_iterator &other) ध्रुव {
        प्रत्यागम getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool संक्रियकः!=(ध्रुव reverse_iterator &other) ध्रुव { प्रत्यागम !संक्रियकः==(other); }

    U_FORCE_INLINE U_शीर्षकम्_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> आधारः() ध्रुव {
        प्रत्यागम U_शीर्षकम्_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>(
            getLogicalPosition());
    }

    U_FORCE_INLINE Unsafe_संहिता_Units_2 संक्रियकः*() ध्रुव {
        यदि (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        प्रत्यागम units_;
    }

    U_FORCE_INLINE Proxy संक्रियकः->() ध्रुव {
        यदि (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        प्रत्यागम Proxy(units_);
    }

    U_FORCE_INLINE reverse_iterator& संक्रियकः++() U_LIFETIME_BOUND {  // pre-increment
        यदि (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } अन्यथा यदि (state_ == 0) {
            Impl::dec(p_);
        } अन्यथा /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        प्रत्यागम *this;
    }

    U_FORCE_INLINE reverse_iterator संक्रियकः++(पूर्णाङ्क) {  // post-increment
        यदि (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            reverse_iterator परिणाम(*this);
            state_ = 0;
            प्रत्यागम परिणाम;
        } अन्यथा यदि (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            reverse_iterator परिणाम(*this);
            परिणाम.state_ = -1;
            // keep this->state_ == 0
            प्रत्यागम परिणाम;
        } अन्यथा /* state_ > 0 */ {
            reverse_iterator परिणाम(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            प्रत्यागम परिणाम;
        }
    }

    U_FORCE_INLINE reverse_iterator& संक्रियकः--() U_LIFETIME_BOUND {  // pre-decrement
        यदि (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.अन्तः();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_);
        state_ = 1;
        प्रत्यागम *this;
    }

    U_FORCE_INLINE reverse_iterator संक्रियकः--(पूर्णाङ्क) {  // post-decrement
        reverse_iterator परिणाम(*this);
        संक्रियकः--();
        प्रत्यागम परिणाम;
    }

गुप्त:
    U_FORCE_INLINE UnitIter getLogicalPosition() ध्रुव {
        प्रत्यागम state_ >= 0 ? p_ : units_.अन्तः();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable Unsafe_संहिता_Units_2 units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#यदि_अन्तः  // U_IN_DOXYGEN

namespace U_शीर्षकम्_ONLY_NAMESPACE {

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
प्रतिरूपम्<typename CP32, typename UnitIter>
स्वचालित unsafeUTFIterator(UnitIter iter) {
    प्रत्यागम UnsafeUTFIterator<CP32, UnitIter>(std::move(iter));
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
प्रतिरूपम्<typename CP32, typename Range>
#यदि defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#यदि_अन्तः
class Unsafe_UTFString_संहिता_Points {
    static_assert(परिमाण(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
सार्वजनिक:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    Unsafe_UTFString_संहिता_Points() = स्वभाव;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    प्रतिरूपम्<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit Unsafe_UTFString_संहिता_Points(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C++20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    प्रतिरूपम्<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = शून्य>
    explicit Unsafe_UTFString_संहिता_Points(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    Unsafe_UTFString_संहिता_Points(ध्रुव Unsafe_UTFString_संहिता_Points &other) = स्वभाव;

    /** Copy assignment operator. @draft ICU 78 */
    Unsafe_UTFString_संहिता_Points &संक्रियकः=(ध्रुव Unsafe_UTFString_संहिता_Points &other) = स्वभाव;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    स्वचालित begin() {
        प्रत्यागम unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    प्रतिरूपम्<typename R = Range, typename = std::enable_if_t<prv::range<ध्रुव R>>>
    स्वचालित begin() ध्रुव {
        प्रत्यागम unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    स्वचालित अन्तः() {
        using UnitIter = decltype(unitRange.begin());
        using सीमा_Iter = decltype(unitRange.अन्तः());
        यदि constexpr (!std::is_same_v<UnitIter, सीमा_Iter>) {
            // Return the code unit sentinel.
            प्रत्यागम unitRange.अन्तः();
        } अन्यथा {
            प्रत्यागम unsafeUTFIterator<CP32>(unitRange.अन्तः());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    प्रतिरूपम्<typename R = Range, typename = std::enable_if_t<prv::range<ध्रुव R>>>
    स्वचालित अन्तः() ध्रुव {
        using UnitIter = decltype(unitRange.begin());
        using सीमा_Iter = decltype(unitRange.अन्तः());
        यदि constexpr (!std::is_same_v<UnitIter, सीमा_Iter>) {
            // Return the code unit sentinel.
            प्रत्यागम unitRange.अन्तः();
        } अन्यथा {
            प्रत्यागम unsafeUTFIterator<CP32>(unitRange.अन्तः());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    स्वचालित rbegin() ध्रुव {
        प्रत्यागम std::make_reverse_iterator(अन्तः());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    स्वचालित rend() ध्रुव {
        प्रत्यागम std::make_reverse_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    स्वचालित front() ध्रुव {
        प्रत्यागम *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    स्वचालित पृष्ठभागः() ध्रुव {
        प्रत्यागम *(--अन्तः());
    }

गुप्त:
    Range unitRange;
};

/** @internal */
प्रतिरूपम्<typename CP32>
संरचना Unsafe_UTFString_संहिता_Points_Adaptor
#यदि U_CPLUSPLUS_संस्करणम् >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_संवृतिः<Unsafe_UTFString_संहिता_Points_Adaptor<CP32>>
#यदि_अन्तः
{
    /** @internal */
    प्रतिरूपम्<typename Range>
    स्वचालित संक्रियकः()(Range &&unitRange) ध्रुव {
#यदि defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        प्रत्यागम Unsafe_UTFString_संहिता_Points<CP32, std::ranges::views::all_t<Range>>(std::forward<Range>(unitRange));
#अन्यथा
        यदि constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            प्रत्यागम Unsafe_UTFString_संहिता_Points<CP32, std::decay_t<Range>>(std::forward<Range>(unitRange));
        } अन्यथा {
            प्रत्यागम Unsafe_UTFString_संहिता_Points<CP32, Range>(std::forward<Range>(unitRange));
        }
#यदि_अन्तः
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
प्रतिरूपम्<typename CP32>
constexpr Unsafe_UTFString_संहिता_Points_Adaptor<CP32> unsafe_UTFString_संहिता_Points;

}  // namespace U_HEADER_ONLY_NAMESPACE


#यदि defined(__cpp_lib_ranges)
प्रतिरूपम् <typename CP32, UTFIllFormedBehavior behavior, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_शीर्षकम्_ONLY_NAMESPACE::UTFString_संहिता_Points<CP32, behavior, Range>> =
    std::ranges::enable_borrowed_range<Range>;

प्रतिरूपम् <typename CP32, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_शीर्षकम्_ONLY_NAMESPACE::Unsafe_UTFString_संहिता_Points<CP32, Range>> =
    std::ranges::enable_borrowed_range<Range>;
#यदि_अन्तः

#यदि_अन्तः  // U_HIDE_DRAFT_API
#यदि_अन्तः  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#यदि_अन्तः  // __UTFITERATOR_H__
