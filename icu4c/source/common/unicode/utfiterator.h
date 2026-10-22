// © 2024 and later: Unicode, Inc. and others.
// License & terms of use: https://www.unicode.org/copyright.html

// utfiterator.h
// created: 2024aug12 Markus W. Scherer

#अगर_अघोषित __UTFITERATOR_H__
#घोषणा __UTFITERATOR_H__

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API || !defined(UTYPES_H)

#समावेश <iterator>
#अगर __has_include(<version>)
#समावेश <version>
#पूर्ण_अगर
#अगर defined(__cpp_lib_ranges)
#समावेश <ranges>
#पूर्ण_अगर
#समावेश <string>
#समावेश <string_view>
#समावेश <type_traits>
#समावेश "unicode/utf16.h"
#समावेश "unicode/utf8.h"
#समावेश "unicode/uversion.h"

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

#अगर_अघोषित U_HIDE_DRAFT_API

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
प्रकार क्रमागत UTFIllFormedBehavior {
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

namespace U_HEADER_ONLY_NAMESPACE {

namespace prv {
#अगर U_CPLUSPLUS_VERSION >= 20

/** @internal */
ढाँचा<typename Iter>
using iter_मूल्य_t = typename std::iter_मूल्य_t<Iter>;

/** @internal */
ढाँचा<typename Iter>
using iter_difference_t = std::iter_difference_t<Iter>;

/** @internal */
ढाँचा<typename Iter>
constexpr bool forward_iterator = std::forward_iterator<Iter>;

/** @internal */
ढाँचा<typename Iter>
constexpr bool bidirectional_iterator = std::bidirectional_iterator<Iter>;

/** @internal */
ढाँचा<typename Range>
constexpr bool range = std::ranges::range<Range>;

#अन्यथा

/** @internal */
ढाँचा<typename Iter>
using iter_मूल्य_t = typename std::iterator_traits<Iter>::मूल्य_type;

/** @internal */
ढाँचा<typename Iter>
using iter_difference_t = typename std::iterator_traits<Iter>::difference_type;

/** @internal */
ढाँचा<typename Iter>
constexpr bool forward_iterator =
    std::is_base_of_v<
        std::forward_iterator_tag,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
ढाँचा<typename Iter>
constexpr bool bidirectional_iterator =
    std::is_base_of_v<
        std::bidirectional_iterator_tag,
        typename std::iterator_traits<Iter>::iterator_category>;

/** @internal */
ढाँचा<typename Range, typename = व्योम>
काष्ठा range_type : std::false_type {};

/** @internal */
ढाँचा<typename Range>
काष्ठा range_type<
    Range,
    std::void_t<decltype(std::declval<Range>().begin()),
                decltype(std::declval<Range>().end())>> : std::true_type {};

/** @internal */
ढाँचा<typename Range>
constexpr bool range = range_type<Range>::मूल्य_2;

#पूर्ण_अगर

/** @internal */
ढाँचा <typename T> काष्ठा is_basic_string_view : std::false_type {};

/** @internal */
ढाँचा <typename... Args>
काष्ठा is_basic_string_view<std::basic_string_view<Args...>> : std::true_type {};

/** @internal */
ढाँचा <typename T> constexpr bool is_basic_string_view_v = is_basic_string_view<T>::मूल्य_2;

/** @internal */
ढाँचा<typename CP32, bool skipSurrogates>
class CodePointsIterator {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
खुला:
    /** C++ iterator boilerplate @internal */
    using मूल्य_type = CP32;
    /** C++ iterator boilerplate @internal */
    using reference = मूल्य_type;
    /** C++ iterator boilerplate @internal */
    using pointer = CP32 *;
    /** C++ iterator boilerplate @internal */
    using difference_type = int32_t;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::forward_iterator_tag;

    /** @internal */
    अंतरभूत CodePointsIterator(CP32 c) : c_(c) {}
    /** @internal */
    अंतरभूत bool चालक==(स्थिर CodePointsIterator &other) स्थिर { वापस c_ == other.c_; }
    /** @internal */
    अंतरभूत bool चालक!=(स्थिर CodePointsIterator &other) स्थिर { वापस !चालक==(other); }
    /** @internal */
    अंतरभूत CP32 चालक*() स्थिर { वापस c_; }
    /** @internal */
    अंतरभूत CodePointsIterator& चालक++() U_LIFETIME_BOUND {  // pre-increment
        ++c_;
        अगर (skipSurrogates && c_ == 0xd800) {
            c_ = 0xe000;
        }
        वापस *this;
    }
    /** @internal */
    अंतरभूत CodePointsIterator चालक++(पूर्णांक) {  // post-increment
        CodePointsIterator परिणाम(*this);
        ++(*this);
        वापस परिणाम;
    }

निजी:
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
ढाँचा<typename CP32>
class AllCodePoints {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
खुला:
    /** Constructor. @draft ICU 78 */
    AllCodePoints() {}
    /**
     * @return an iterator over all Unicode code points.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    स्वतः begin() स्थिर { वापस prv::CodePointsIterator<CP32, false>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode code points.
     * @draft ICU 78
     */
    स्वतः end() स्थिर { वापस prv::CodePointsIterator<CP32, false>(0x110000); }
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
ढाँचा<typename CP32>
class All_Scalar_मूल्यावली {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
खुला:
    /** Constructor. @draft ICU 78 */
    All_Scalar_मूल्यावली() {}
    /**
     * @return an iterator over all Unicode scalar values.
     *     The iterator returns CP32 integers.
     * @draft ICU 78
     */
    स्वतः begin() स्थिर { वापस prv::CodePointsIterator<CP32, true>(0); }
    /**
     * @return an exclusive-end iterator over all Unicode scalar values.
     * @draft ICU 78
     */
    स्वतः end() स्थिर { वापस prv::CodePointsIterator<CP32, true>(0x110000); }
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
ढाँचा<typename CP32, typename UnitIter, typename = व्योम>
class UnsafeCodeUnits {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Unit = typename prv::iter_मूल्य_t<UnitIter>;
खुला:
    /** @internal */
    UnsafeCodeUnits(CP32 codePoint, uint8_t लंबाई, UnitIter start, UnitIter limit) :
            c_(codePoint), len_(लंबाई), start_(start), limit_(limit) {}

    /** Copy constructor. @draft ICU 78 */
    UnsafeCodeUnits(स्थिर UnsafeCodeUnits &other) = शेष;
    /** Copy assignment operator. @draft ICU 78 */
    UnsafeCodeUnits &चालक=(स्थिर UnsafeCodeUnits &other) = शेष;

    /**
     * @return the Unicode code point decoded from the code unit sequence.
     *     If the sequence is ill-formed and the iterator validates,
     *     then this is a replacement value according to the iterator‘s
     *     UTFIllFormedBehavior template parameter.
     * @draft ICU 78
     */
    CP32 codePoint() स्थिर { वापस c_; }

    /**
     * @return the start of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter begin() स्थिर { वापस start_; }

    /**
     * @return the limit (exclusive end) of the code unit sequence for one code point.
     * Only enabled if UnitIter is a (multi-pass) forward_iterator or better.
     * @draft ICU 78
     */
    UnitIter end() स्थिर { वापस limit_; }

    /**
     * @return the length of the code unit sequence for one code point.
     * @draft ICU 78
     */
    uint8_t लंबाई() स्थिर { वापस len_; }

#अगर U_CPLUSPLUS_VERSION >= 20
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    ढाँचा<std::contiguous_iterator Iter = UnitIter>
    std::basic_string_view<Unit> stringView() स्थिर {
        वापस std::basic_string_view<Unit>(begin(), end());
    }
#अन्यथा
    /**
     * @return a string_view of the code unit sequence for one code point.
     * Only works if UnitIter is a pointer or a contiguous_iterator.
     * @draft ICU 78
     */
    ढाँचा<typename Iter = UnitIter, typename Unit = typename std::iterator_traits<Iter>::मूल्य_type>
    std::enable_if_t<std::is_pointer_v<Iter> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string<Unit>::const_iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::iterator> ||
                         std::is_same_v<Iter, typename std::basic_string_view<Unit>::const_iterator>,
                     std::basic_string_view<Unit>>
    stringView() स्थिर {
        वापस std::basic_string_view<Unit>(&*start_, len_);
    }
#पूर्ण_अगर

निजी:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
    UnitIter start_;
    UnitIter limit_;
};

#अगर_अघोषित U_IN_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
ढाँचा<typename CP32, typename UnitIter>
class UnsafeCodeUnits<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
खुला:
    UnsafeCodeUnits(CP32 codePoint, uint8_t लंबाई) : c_(codePoint), len_(लंबाई) {}

    UnsafeCodeUnits(स्थिर UnsafeCodeUnits &other) = शेष;
    UnsafeCodeUnits &चालक=(स्थिर UnsafeCodeUnits &other) = शेष;

    CP32 codePoint() स्थिर { वापस c_; }

    uint8_t लंबाई() स्थिर { वापस len_; }

निजी:
    // Order of fields with padding and access frequency in mind.
    CP32 c_;
    uint8_t len_;
};
#पूर्ण_अगर  // U_IN_DOXYGEN

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
ढाँचा<typename CP32, typename UnitIter, typename = व्योम>
class CodeUnits : खुला UnsafeCodeUnits<CP32, UnitIter> {
खुला:
    /** @internal */
    CodeUnits(CP32 codePoint, uint8_t लंबाई, bool wellFormed, UnitIter start, UnitIter limit) :
            UnsafeCodeUnits<CP32, UnitIter>(codePoint, लंबाई, start, limit), ok_(wellFormed) {}

    /** Copy constructor. @draft ICU 78 */
    CodeUnits(स्थिर CodeUnits &other) = शेष;
    /** Copy assignment operator. @draft ICU 78 */
    CodeUnits &चालक=(स्थिर CodeUnits &other) = शेष;

    /**
     * @return true if the decoded code unit sequence is well-formed.
     * @draft ICU 78
     */
    bool wellFormed() स्थिर { वापस ok_; }

निजी:
    bool ok_;
};

#अगर_अघोषित U_IN_DOXYGEN
// Partial template specialization for single-pass input iterator.
// No UnitIter field, no getter for it, no stringView().
ढाँचा<typename CP32, typename UnitIter>
class CodeUnits<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> :
            खुला UnsafeCodeUnits<CP32, UnitIter> {
खुला:
    CodeUnits(CP32 codePoint, uint8_t लंबाई, bool wellFormed) :
            UnsafeCodeUnits<CP32, UnitIter>(codePoint, लंबाई), ok_(wellFormed) {}

    CodeUnits(स्थिर CodeUnits &other) = शेष;
    CodeUnits &चालक=(स्थिर CodeUnits &other) = शेष;

    bool wellFormed() स्थिर { वापस ok_; }

निजी:
    bool ok_;
};
#पूर्ण_अगर  // U_IN_DOXYGEN

// Validating implementations ---------------------------------------------- ***

#अगर_अघोषित U_IN_DOXYGEN
ढाँचा<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename LimitIter = UnitIter, typename = व्योम>
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
ढाँचा<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename LimitIter>
class UTFImpl<
        CP32, behavior,
        UnitIter, LimitIter,
        std::enable_if_t<माप(typename prv::iter_मूल्य_t<UnitIter>) == 1>> {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    static_assert(behavior != UTF_BEHAVIOR_SURROGATE,
                  "For 8-bit strings, the SURROGATE option does not have an equivalent.");
खुला:
    // Handle ill-formed UTF-8
    U_FORCE_INLINE अटल CP32 sub() {
        अगर constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            वापस U_SENTINEL;
        } अन्यथा {
            static_assert(behavior == UTF_BEHAVIOR_FFFD);
            वापस 0xfffd;
        }
    }

    U_FORCE_INLINE अटल व्योम inc(UnitIter &p, स्थिर LimitIter &limit) {
        // Very similar to U8_FWD_1().
        uint8_t b = *p;
        ++p;
        अगर (U8_IS_LEAD(b) && p != limit) {
            uint8_t t1 = *p;
            अगर ((0xe0 <= b && b < 0xf0)) {
                अगर (U8_IS_VALID_LEAD3_AND_T1(b, t1) &&
                        ++p != limit && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            } अन्यथा अगर (b < 0xe0) {
                अगर (U8_IS_TRAIL(t1)) {
                    ++p;
                }
            } अन्यथा /* b >= 0xf0 */ {
                अगर (U8_IS_VALID_LEAD4_AND_T1(b, t1) &&
                        ++p != limit && U8_IS_TRAIL(*p) &&
                        ++p != limit && U8_IS_TRAIL(*p)) {
                    ++p;
                }
            }
        }
    }

    U_FORCE_INLINE अटल व्योम dec(UnitIter start, UnitIter &p) {
        // Very similar to U8_BACK_1().
        uint8_t c = *--p;
        अगर (U8_IS_TRAIL(c) && p != start) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            अगर (U8_IS_LEAD(b1)) {
                अगर (b1 < 0xe0 ||
                        (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c))) {
                    p = p1;
                    वापस;
                }
            } अन्यथा अगर (U8_IS_TRAIL(b1) && p1 != start) {
                uint8_t b2 = *--p1;
                अगर (0xe0 <= b2 && b2 <= 0xf4) {
                    अगर (b2 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b2, b1) :
                            U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        p = p1;
                        वापस;
                    }
                } अन्यथा अगर (U8_IS_TRAIL(b2) && p1 != start) {
                    uint8_t b3 = *--p1;
                    अगर (0xf0 <= b3 && b3 <= 0xf4 && U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                        p = p1;
                        वापस;
                    }
                }
            }
        }
    }

    U_FORCE_INLINE अटल CodeUnits<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, स्थिर LimitIter &limit) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_OR_FFFD().
        CP32 c = uint8_t(*p);
        ++p;
        अगर (U8_IS_SINGLE(c)) {
            अगर constexpr (isMultiPass) {
                वापस {c, 1, true, p0, p};
            } अन्यथा {
                वापस {c, 1, true};
            }
        }
        uint8_t लंबाई = 1;
        uint8_t t = 0;
        अगर (p != limit &&
                // fetch/validate/assemble all but last trail byte
                (c >= 0xe0 ?
                    (c < 0xf0 ?  // U+0800..U+FFFF except surrogates
                        U8_LEAD3_T1_BITS[c &= 0xf] & (1 << ((t = *p) >> 5)) &&
                        (t &= 0x3f, 1)
                    :  // U+10000..U+10FFFF
                        (c -= 0xf0) <= 4 &&
                        U8_LEAD4_T1_BITS[(t = *p) >> 4] & (1 << c) &&
                        (c = (c << 6) | (t & 0x3f), ++लंबाई, ++p != limit) &&
                        (t = *p - 0x80) <= 0x3f) &&
                    // valid second-to-last trail byte
                    (c = (c << 6) | t, ++लंबाई, ++p != limit)
                :  // U+0080..U+07FF
                    c >= 0xc2 && (c &= 0x1f, 1)) &&
                // last trail byte
                (t = *p - 0x80) <= 0x3f) {
            c = (c << 6) | t;
            ++लंबाई;
            ++p;
            अगर constexpr (isMultiPass) {
                वापस {c, लंबाई, true, p0, p};
            } अन्यथा {
                वापस {c, लंबाई, true};
            }
        }
        अगर constexpr (isMultiPass) {
            वापस {sub(), लंबाई, false, p0, p};
        } अन्यथा {
            वापस {sub(), लंबाई, false};
        }
    }

    U_FORCE_INLINE अटल CodeUnits<CP32, UnitIter> decAndRead(UnitIter start, UnitIter &p) {
        // Very similar to U8_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        अगर (U8_IS_SINGLE(c)) {
            वापस {c, 1, true, p, p0};
        }
        अगर (U8_IS_TRAIL(c) && p != start) {
            UnitIter p1 = p;
            uint8_t b1 = *--p1;
            अगर (U8_IS_LEAD(b1)) {
                अगर (b1 < 0xe0) {
                    p = p1;
                    c = ((b1 - 0xc0) << 6) | (c & 0x3f);
                    वापस {c, 2, true, p, p0};
                } अन्यथा अगर (b1 < 0xf0 ?
                            U8_IS_VALID_LEAD3_AND_T1(b1, c) :
                            U8_IS_VALID_LEAD4_AND_T1(b1, c)) {
                    // Truncated 3- or 4-byte sequence.
                    p = p1;
                    वापस {sub(), 2, false, p, p0};
                }
            } अन्यथा अगर (U8_IS_TRAIL(b1) && p1 != start) {
                // Extract the value bits from the last trail byte.
                c &= 0x3f;
                uint8_t b2 = *--p1;
                अगर (0xe0 <= b2 && b2 <= 0xf4) {
                    अगर (b2 < 0xf0) {
                        b2 &= 0xf;
                        अगर (U8_IS_VALID_LEAD3_AND_T1(b2, b1)) {
                            p = p1;
                            c = (b2 << 12) | ((b1 & 0x3f) << 6) | c;
                            वापस {c, 3, true, p, p0};
                        }
                    } अन्यथा अगर (U8_IS_VALID_LEAD4_AND_T1(b2, b1)) {
                        // Truncated 4-byte sequence.
                        p = p1;
                        वापस {sub(), 3, false, p, p0};
                    }
                } अन्यथा अगर (U8_IS_TRAIL(b2) && p1 != start) {
                    uint8_t b3 = *--p1;
                    अगर (0xf0 <= b3 && b3 <= 0xf4) {
                        b3 &= 7;
                        अगर (U8_IS_VALID_LEAD4_AND_T1(b3, b2)) {
                            p = p1;
                            c = (b3 << 18) | ((b2 & 0x3f) << 12) | ((b1 & 0x3f) << 6) | c;
                            वापस {c, 4, true, p, p0};
                        }
                    }
                }
            }
        }
        वापस {sub(), 1, false, p, p0};
    }
};

// UTF-16
ढाँचा<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename LimitIter>
class UTFImpl<
        CP32, behavior,
        UnitIter, LimitIter,
        std::enable_if_t<माप(typename prv::iter_मूल्य_t<UnitIter>) == 2>> {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
खुला:
    // Handle ill-formed UTF-16: One unpaired surrogate.
    U_FORCE_INLINE अटल CP32 sub(CP32 surrogate) {
        अगर constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            वापस U_SENTINEL;
        } अन्यथा अगर constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            वापस 0xfffd;
        } अन्यथा {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            वापस surrogate;
        }
    }

    U_FORCE_INLINE अटल व्योम inc(UnitIter &p, स्थिर LimitIter &limit) {
        // Very similar to U16_FWD_1().
        स्वतः c = *p;
        ++p;
        अगर (U16_IS_LEAD(c) && p != limit && U16_IS_TRAIL(*p)) {
            ++p;
        }
    }

    U_FORCE_INLINE अटल व्योम dec(UnitIter start, UnitIter &p) {
        // Very similar to U16_BACK_1().
        UnitIter p1;
        अगर (U16_IS_TRAIL(*--p) && p != start && (p1 = p, U16_IS_LEAD(*--p1))) {
            p = p1;
        }
    }

    U_FORCE_INLINE अटल CodeUnits<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, स्थिर LimitIter &limit) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_OR_FFFD().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        अगर (!U16_IS_SURROGATE(c)) {
            अगर constexpr (isMultiPass) {
                वापस {c, 1, true, p0, p};
            } अन्यथा {
                वापस {c, 1, true};
            }
        } अन्यथा {
            uint16_t c2;
            अगर (U16_IS_SURROGATE_LEAD(c) && p != limit && U16_IS_TRAIL(c2 = *p)) {
                ++p;
                c = U16_GET_SUPPLEMENTARY(c, c2);
                अगर constexpr (isMultiPass) {
                    वापस {c, 2, true, p0, p};
                } अन्यथा {
                    वापस {c, 2, true};
                }
            } अन्यथा {
                अगर constexpr (isMultiPass) {
                    वापस {sub(c), 1, false, p0, p};
                } अन्यथा {
                    वापस {sub(c), 1, false};
                }
            }
        }
    }

    U_FORCE_INLINE अटल CodeUnits<CP32, UnitIter> decAndRead(UnitIter start, UnitIter &p) {
        // Very similar to U16_PREV_OR_FFFD().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        अगर (!U16_IS_SURROGATE(c)) {
            वापस {c, 1, true, p, p0};
        } अन्यथा {
            UnitIter p1;
            uint16_t c2;
            अगर (U16_IS_SURROGATE_TRAIL(c) && p != start && (p1 = p, U16_IS_LEAD(c2 = *--p1))) {
                p = p1;
                c = U16_GET_SUPPLEMENTARY(c2, c);
                वापस {c, 2, true, p, p0};
            } अन्यथा {
                वापस {sub(c), 1, false, p, p0};
            }
        }
    }
};

// UTF-32: trivial, but still validating
ढाँचा<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename LimitIter>
class UTFImpl<
        CP32, behavior,
        UnitIter, LimitIter,
        std::enable_if_t<माप(typename prv::iter_मूल्य_t<UnitIter>) == 4>> {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
खुला:
    // Handle ill-formed UTF-32
    U_FORCE_INLINE अटल CP32 sub(bool forSurrogate, CP32 surrogate) {
        अगर constexpr (behavior == UTF_BEHAVIOR_NEGATIVE) {
            वापस U_SENTINEL;
        } अन्यथा अगर constexpr (behavior == UTF_BEHAVIOR_FFFD) {
            वापस 0xfffd;
        } अन्यथा {
            static_assert(behavior == UTF_BEHAVIOR_SURROGATE);
            वापस forSurrogate ? surrogate : 0xfffd;
        }
    }

    U_FORCE_INLINE अटल व्योम inc(UnitIter &p, स्थिर LimitIter &/*limit*/) {
        ++p;
    }

    U_FORCE_INLINE अटल व्योम dec(UnitIter /*start*/, UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE अटल CodeUnits<CP32, UnitIter> readAndInc(
            UnitIter &p0, UnitIter &p, स्थिर LimitIter &/*limit*/) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        uint32_t uc = *p;
        CP32 c = uc;
        ++p;
        अगर (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            अगर constexpr (isMultiPass) {
                वापस {c, 1, true, p0, p};
            } अन्यथा {
                वापस {c, 1, true};
            }
        } अन्यथा {
            अगर constexpr (isMultiPass) {
                वापस {sub(uc < 0xe000, c), 1, false, p0, p};
            } अन्यथा {
                वापस {sub(uc < 0xe000, c), 1, false};
            }
        }
    }

    U_FORCE_INLINE अटल CodeUnits<CP32, UnitIter> decAndRead(UnitIter /*start*/, UnitIter &p) {
        UnitIter p0 = p;
        uint32_t uc = *--p;
        CP32 c = uc;
        अगर (uc < 0xd800 || (0xe000 <= uc && uc <= 0x10ffff)) {
            वापस {c, 1, true, p, p0};
        } अन्यथा {
            वापस {sub(uc < 0xe000, c), 1, false, p, p0};
        }
    }
};

// Non-validating implementations ------------------------------------------ ***

ढाँचा<typename CP32, typename UnitIter, typename = व्योम>
class UnsafeUTFImpl;

// UTF-8
ढाँचा<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<माप(typename prv::iter_मूल्य_t<UnitIter>) == 1>> {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
खुला:
    U_FORCE_INLINE अटल व्योम inc(UnitIter &p) {
        // Very similar to U8_FWD_1_UNSAFE().
        uint8_t b = *p;
        std::advance(p, 1 + U8_गिनती_TRAIL_BYTES_UNSAFE(b));
    }

    U_FORCE_INLINE अटल व्योम dec(UnitIter &p) {
        // Very similar to U8_BACK_1_UNSAFE().
        जबतक (U8_IS_TRAIL(*--p)) {}
    }

    U_FORCE_INLINE अटल UnsafeCodeUnits<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U8_NEXT_UNSAFE().
        CP32 c = uint8_t(*p);
        ++p;
        अगर (U8_IS_SINGLE(c)) {
            अगर constexpr (isMultiPass) {
                वापस {c, 1, p0, p};
            } अन्यथा {
                वापस {c, 1};
            }
        } अन्यथा अगर (c < 0xe0) {
            c = ((c & 0x1f) << 6) | (*p & 0x3f);
            ++p;
            अगर constexpr (isMultiPass) {
                वापस {c, 2, p0, p};
            } अन्यथा {
                वापस {c, 2};
            }
        } अन्यथा अगर (c < 0xf0) {
            // No need for (c&0xf) because the upper bits are truncated
            // after <<12 in the cast to uint16_t.
            c = uint16_t(c << 12) | ((*p & 0x3f) << 6);
            ++p;
            c |= *p & 0x3f;
            ++p;
            अगर constexpr (isMultiPass) {
                वापस {c, 3, p0, p};
            } अन्यथा {
                वापस {c, 3};
            }
        } अन्यथा {
            c = ((c & 7) << 18) | ((*p & 0x3f) << 12);
            ++p;
            c |= (*p & 0x3f) << 6;
            ++p;
            c |= *p & 0x3f;
            ++p;
            अगर constexpr (isMultiPass) {
                वापस {c, 4, p0, p};
            } अन्यथा {
                वापस {c, 4};
            }
        }
    }

    U_FORCE_INLINE अटल UnsafeCodeUnits<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U8_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = uint8_t(*--p);
        अगर (U8_IS_SINGLE(c)) {
            वापस {c, 1, p, p0};
        }
        // U8_IS_TRAIL(c) if well-formed
        c &= 0x3f;
        uint8_t गिनती = 1;
        क्रम (uint8_t shift = 6;;) {
            uint8_t b = *--p;
            अगर (b >= 0xc0) {
                U8_MASK_LEAD_BYTE(b, गिनती);
                c |= uint32_t{b} << shift;
                अवरोध;
            } अन्यथा {
                c |= (uint32_t{b} & 0x3f) << shift;
                ++गिनती;
                shift += 6;
            }
        }
        ++गिनती;
        वापस {c, गिनती, p, p0};
    }
};

// UTF-16
ढाँचा<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<माप(typename prv::iter_मूल्य_t<UnitIter>) == 2>> {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
खुला:
    U_FORCE_INLINE अटल व्योम inc(UnitIter &p) {
        // Very similar to U16_FWD_1_UNSAFE().
        स्वतः c = *p;
        ++p;
        अगर (U16_IS_LEAD(c)) {
            ++p;
        }
    }

    U_FORCE_INLINE अटल व्योम dec(UnitIter &p) {
        // Very similar to U16_BACK_1_UNSAFE().
        अगर (U16_IS_TRAIL(*--p)) {
            --p;
        }
    }

    U_FORCE_INLINE अटल UnsafeCodeUnits<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        // Very similar to U16_NEXT_UNSAFE().
        CP32 c = static_cast<CP32>(*p);
        ++p;
        अगर (!U16_IS_LEAD(c)) {
            अगर constexpr (isMultiPass) {
                वापस {c, 1, p0, p};
            } अन्यथा {
                वापस {c, 1};
            }
        } अन्यथा {
            uint16_t c2 = *p;
            ++p;
            c = U16_GET_SUPPLEMENTARY(c, c2);
            अगर constexpr (isMultiPass) {
                वापस {c, 2, p0, p};
            } अन्यथा {
                वापस {c, 2};
            }
        }
    }

    U_FORCE_INLINE अटल UnsafeCodeUnits<CP32, UnitIter> decAndRead(UnitIter &p) {
        // Very similar to U16_PREV_UNSAFE().
        UnitIter p0 = p;
        CP32 c = static_cast<CP32>(*--p);
        अगर (!U16_IS_TRAIL(c)) {
            वापस {c, 1, p, p0};
        } अन्यथा {
            uint16_t c2 = *--p;
            c = U16_GET_SUPPLEMENTARY(c2, c);
            वापस {c, 2, p, p0};
        }
    }
};

// UTF-32: trivial
ढाँचा<typename CP32, typename UnitIter>
class UnsafeUTFImpl<
        CP32,
        UnitIter,
        std::enable_if_t<माप(typename prv::iter_मूल्य_t<UnitIter>) == 4>> {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
खुला:
    U_FORCE_INLINE अटल व्योम inc(UnitIter &p) {
        ++p;
    }

    U_FORCE_INLINE अटल व्योम dec(UnitIter &p) {
        --p;
    }

    U_FORCE_INLINE अटल UnsafeCodeUnits<CP32, UnitIter> readAndInc(UnitIter &p0, UnitIter &p) {
        constexpr bool isMultiPass = prv::forward_iterator<UnitIter>;
        CP32 c = *p;
        ++p;
        अगर constexpr (isMultiPass) {
            वापस {c, 1, p0, p};
        } अन्यथा {
            वापस {c, 1};
        }
    }

    U_FORCE_INLINE अटल UnsafeCodeUnits<CP32, UnitIter> decAndRead(UnitIter &p) {
        UnitIter p0 = p;
        CP32 c = *--p;
        वापस {c, 1, p, p0};
    }
};

#पूर्ण_अगर

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
ढाँचा<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename LimitIter = UnitIter, typename = व्योम>
class UTFIterator {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, LimitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    खुला:
        explicit Proxy(CodeUnits<CP32, UnitIter> &units) : units_(units) {}
        CodeUnits<CP32, UnitIter>& चालक*() U_LIFETIME_BOUND { वापस units_; }
        CodeUnits<CP32, UnitIter>* चालक->() U_LIFETIME_BOUND { वापस &units_; }
    निजी:
        CodeUnits<CP32, UnitIter> units_;
    };

खुला:
    /** C++ iterator boilerplate @internal */
    using मूल्य_type = CodeUnits<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = मूल्य_type;
    /** C++ iterator boilerplate @internal */
    using pointer = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_tag,
        std::forward_iterator_tag>;

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
    U_FORCE_INLINE UTFIterator(UnitIter start, UnitIter p, LimitIter limit) :
            p_(p), start_(start), limit_(limit), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE UTFIterator(UnitIter p, LimitIter limit) :
            p_(p), start_(p), limit_(limit), units_(0, 0, false, p, p) {}
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
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(p), start_(p), limit_(p), units_(0, 0, false, p, p) {}
    /**
     * Default constructor. Makes a non-functional iterator.
     *
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator() : p_{}, start_{}, limit_{}, units_(0, 0, false, p_, p_) {}

    /** Move constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = शेष;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &चालक=(UTFIterator &&src) noexcept = शेष;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator(स्थिर UTFIterator &other) = शेष;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UTFIterator &चालक=(स्थिर UTFIterator &other) = शेष;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool चालक==(स्थिर UTFIterator &other) स्थिर {
        वापस base() == other.base();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool चालक!=(स्थिर UTFIterator &other) स्थिर { वापस !चालक==(other); }

    // Asymmetric equality & nonequality with a sentinel type.

    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक==(स्थिर UTFIterator &iter, स्थिर Sentinel &s) {
        वापस iter.base() == s;
    }

#अगर U_CPLUSPLUS_VERSION < 20
    // C++17: Need to define all four combinations of == / != vs. parameter order.
    // Once we require C++20, we could remove all but the first == because
    // the compiler would generate the rest.

    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक==(स्थिर Sentinel &s, स्थिर UTFIterator &iter) {
        वापस iter.base() == s;
    }
    /**
     * @param iter A UTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक!=(स्थिर UTFIterator &iter, स्थिर Sentinel &s) { वापस !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter A UTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक!=(स्थिर Sentinel &s, स्थिर UTFIterator &iter) { वापस !(iter == s); }
#पूर्ण_अगर  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter base() स्थिर {
        // Return the logical position.
        वापस state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE CodeUnits<CP32, UnitIter> चालक*() स्थिर {
        अगर (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, limit_);
            state_ = 1;
        }
        वापस units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy चालक->() स्थिर {
        अगर (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, limit_);
            state_ = 1;
        }
        वापस Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator& चालक++() U_LIFETIME_BOUND {  // pre-increment
        अगर (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } अन्यथा अगर (state_ == 0) {
            Impl::inc(p_, limit_);
        } अन्यथा /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.end();
            state_ = 0;
        }
        वापस *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UTFIterator चालक++(पूर्णांक) {  // post-increment
        अगर (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UTFIterator परिणाम(*this);
            state_ = 0;
            वापस परिणाम;
        } अन्यथा अगर (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_, limit_);
            UTFIterator परिणाम(*this);
            परिणाम.state_ = 1;
            // keep this->state_ == 0
            वापस परिणाम;
        } अन्यथा /* state_ < 0 */ {
            UTFIterator परिणाम(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.end();
            state_ = 0;
            वापस परिणाम;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    ढाँचा <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator&>
    चालक--() U_LIFETIME_BOUND {  // pre-decrement
        अगर (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(start_, p_);
        state_ = -1;
        वापस *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    ढाँचा<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UTFIterator>
    चालक--(पूर्णांक) {  // post-decrement
        UTFIterator परिणाम(*this);
        चालक--();
        वापस परिणाम;
    }

निजी:
    मित्र class std::reverse_iterator<UTFIterator<CP32, behavior, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter start_;
    LimitIter limit_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable CodeUnits<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#अगर_अघोषित U_IN_DOXYGEN
// Partial template specialization for single-pass input iterator.
ढाँचा<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter, typename LimitIter>
class UTFIterator<
        CP32, behavior,
        UnitIter, LimitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UTFImpl<CP32, behavior, UnitIter, LimitIter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    खुला:
        explicit Proxy(CodeUnits<CP32, UnitIter> &units) : units_(units) {}
        CodeUnits<CP32, UnitIter>& चालक*() U_LIFETIME_BOUND { वापस units_; }
        CodeUnits<CP32, UnitIter>* चालक->() U_LIFETIME_BOUND { वापस &units_; }
    निजी:
        CodeUnits<CP32, UnitIter> units_;
    };

खुला:
    using मूल्य_type = CodeUnits<CP32, UnitIter>;
    using reference = मूल्य_type;
    using pointer = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::निवेश_iterator_tag;

    U_FORCE_INLINE UTFIterator(UnitIter p, LimitIter limit) : p_(std::move(p)), limit_(std::move(limit)) {}

    // Constructs an iterator start or limit sentinel.
    // Requires p to be copyable.
    U_FORCE_INLINE explicit UTFIterator(UnitIter p) : p_(std::move(p)), limit_(p_) {}

    U_FORCE_INLINE UTFIterator(UTFIterator &&src) noexcept = शेष;
    U_FORCE_INLINE UTFIterator &चालक=(UTFIterator &&src) noexcept = शेष;

    U_FORCE_INLINE UTFIterator(स्थिर UTFIterator &other) = शेष;
    U_FORCE_INLINE UTFIterator &चालक=(स्थिर UTFIterator &other) = शेष;

    U_FORCE_INLINE bool चालक==(स्थिर UTFIterator &other) स्थिर {
        वापस p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool चालक!=(स्थिर UTFIterator &other) स्थिर { वापस !चालक==(other); }

    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक==(स्थिर UTFIterator &iter, स्थिर Sentinel &s) {
        वापस !iter.ahead_ && iter.p_ == s;
    }

#अगर U_CPLUSPLUS_VERSION < 20
    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक==(स्थिर Sentinel &s, स्थिर UTFIterator &iter) {
        वापस !iter.ahead_ && iter.p_ == s;
    }

    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक!=(स्थिर UTFIterator &iter, स्थिर Sentinel &s) { वापस !(iter == s); }

    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक!=(स्थिर Sentinel &s, स्थिर UTFIterator &iter) { वापस !(iter == s); }
#पूर्ण_अगर  // C++17

    U_FORCE_INLINE CodeUnits<CP32, UnitIter> चालक*() स्थिर {
        अगर (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, limit_);
            ahead_ = true;
        }
        वापस units_;
    }

    U_FORCE_INLINE Proxy चालक->() स्थिर {
        अगर (!ahead_) {
            units_ = Impl::readAndInc(p_, p_, limit_);
            ahead_ = true;
        }
        वापस Proxy(units_);
    }

    U_FORCE_INLINE UTFIterator& चालक++() U_LIFETIME_BOUND {  // pre-increment
        अगर (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } अन्यथा {
            Impl::inc(p_, limit_);
        }
        वापस *this;
    }

    U_FORCE_INLINE Proxy चालक++(पूर्णांक) {  // post-increment
        अगर (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } अन्यथा {
            units_ = Impl::readAndInc(p_, p_, limit_);
            // keep this->ahead_ == false
        }
        वापस Proxy(units_);
    }

निजी:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need limit_ so that when we read a code point
    // we can test if there are enough code units.
    LimitIter limit_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable CodeUnits<CP32, UnitIter> units_ = {0, 0, false};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#पूर्ण_अगर  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#अगर_अघोषित U_IN_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
ढाँचा<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
class std::reverse_iterator<U_HEADER_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>> {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_HEADER_ONLY_NAMESPACE::UTFImpl<CP32, behavior, UnitIter>;
    using CodeUnits_ = U_HEADER_ONLY_NAMESPACE::CodeUnits<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning CodeUnits.
    class Proxy {
    खुला:
        explicit Proxy(CodeUnits_ units) : units_(units) {}
        CodeUnits_& चालक*() U_LIFETIME_BOUND { वापस units_; }
        CodeUnits_* चालक->() U_LIFETIME_BOUND { वापस &units_; }
    निजी:
        CodeUnits_ units_;
    };

खुला:
    using मूल्य_type = CodeUnits_;
    using reference = मूल्य_type;
    using pointer = Proxy;
    using difference_type = U_HEADER_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_tag;

    U_FORCE_INLINE explicit reverse_iterator(U_HEADER_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> iter) :
            p_(iter.base()), start_(iter.start_), limit_(iter.limit_),
            units_(0, 0, false, p_, p_) {}
    U_FORCE_INLINE reverse_iterator() : p_{}, start_{}, limit_{}, units_(0, 0, false, p_, p_) {}

    U_FORCE_INLINE reverse_iterator(reverse_iterator &&src) noexcept = शेष;
    U_FORCE_INLINE reverse_iterator &चालक=(reverse_iterator &&src) noexcept = शेष;

    U_FORCE_INLINE reverse_iterator(स्थिर reverse_iterator &other) = शेष;
    U_FORCE_INLINE reverse_iterator &चालक=(स्थिर reverse_iterator &other) = शेष;

    U_FORCE_INLINE bool चालक==(स्थिर reverse_iterator &other) स्थिर {
        वापस getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool चालक!=(स्थिर reverse_iterator &other) स्थिर { वापस !चालक==(other); }

    U_FORCE_INLINE U_HEADER_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter> base() स्थिर {
        वापस U_HEADER_ONLY_NAMESPACE::UTFIterator<CP32, behavior, UnitIter>(
            start_, getLogicalPosition(), limit_);
    }

    U_FORCE_INLINE CodeUnits_ चालक*() स्थिर {
        अगर (state_ == 0) {
            units_ = Impl::decAndRead(start_, p_);
            state_ = -1;
        }
        वापस units_;
    }

    U_FORCE_INLINE Proxy चालक->() स्थिर {
        अगर (state_ == 0) {
            units_ = Impl::decAndRead(start_, p_);
            state_ = -1;
        }
        वापस Proxy(units_);
    }

    U_FORCE_INLINE reverse_iterator& चालक++() U_LIFETIME_BOUND {  // pre-increment
        अगर (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } अन्यथा अगर (state_ == 0) {
            Impl::dec(start_, p_);
        } अन्यथा /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        वापस *this;
    }

    U_FORCE_INLINE reverse_iterator चालक++(पूर्णांक) {  // post-increment
        अगर (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            reverse_iterator परिणाम(*this);
            state_ = 0;
            वापस परिणाम;
        } अन्यथा अगर (state_ == 0) {
            units_ = Impl::decAndRead(start_, p_);
            reverse_iterator परिणाम(*this);
            परिणाम.state_ = -1;
            // keep this->state_ == 0
            वापस परिणाम;
        } अन्यथा /* state_ > 0 */ {
            reverse_iterator परिणाम(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            वापस परिणाम;
        }
    }

    U_FORCE_INLINE reverse_iterator& चालक--() U_LIFETIME_BOUND {  // pre-decrement
        अगर (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.end();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_, limit_);
        state_ = 1;
        वापस *this;
    }

    U_FORCE_INLINE reverse_iterator चालक--(पूर्णांक) {  // post-decrement
        reverse_iterator परिणाम(*this);
        चालक--();
        वापस परिणाम;
    }

निजी:
    U_FORCE_INLINE UnitIter getLogicalPosition() स्थिर {
        वापस state_ >= 0 ? p_ : units_.end();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // In a validating iterator, we need start_ & limit_ so that when we read a code point
    // (forward or backward) we can test if there are enough code units.
    UnitIter start_;
    UnitIter limit_;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable CodeUnits_ units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#पूर्ण_अगर  // U_IN_DOXYGEN

namespace U_HEADER_ONLY_NAMESPACE {

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
ढाँचा<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename LimitIter = UnitIter>
स्वतः utfIterator(UnitIter start, UnitIter p, LimitIter limit) {
    वापस UTFIterator<CP32, behavior, UnitIter, LimitIter>(
        std::move(start), std::move(p), std::move(limit));
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
ढाँचा<typename CP32, UTFIllFormedBehavior behavior,
         typename UnitIter, typename LimitIter = UnitIter>
स्वतः utfIterator(UnitIter p, LimitIter limit) {
    वापस UTFIterator<CP32, behavior, UnitIter, LimitIter>(
        std::move(p), std::move(limit));
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
ढाँचा<typename CP32, UTFIllFormedBehavior behavior, typename UnitIter>
स्वतः utfIterator(UnitIter p) {
    वापस UTFIterator<CP32, behavior, UnitIter>(std::move(p));
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
ढाँचा<typename CP32, UTFIllFormedBehavior behavior, typename Range>
#अगर defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#पूर्ण_अगर
class UTFStringCodePoints {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
खुला:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    UTFStringCodePoints() = शेष;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    ढाँचा<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit UTFStringCodePoints(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C+20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    ढाँचा<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = व्योम>
    explicit UTFStringCodePoints(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    UTFStringCodePoints(स्थिर UTFStringCodePoints &other) = शेष;

    /** Copy assignment operator. @draft ICU 78 */
    UTFStringCodePoints &चालक=(स्थिर UTFStringCodePoints &other) = शेष;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    स्वतः begin() {
        वापस utfIterator<CP32, behavior>(unitRange.begin(), unitRange.end());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    ढाँचा<typename R = Range, typename = std::enable_if_t<prv::range<स्थिर R>>>
    स्वतः begin() स्थिर {
        वापस utfIterator<CP32, behavior>(unitRange.begin(), unitRange.end());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    स्वतः end() {
        using UnitIter = decltype(unitRange.begin());
        using LimitIter = decltype(unitRange.end());
        अगर constexpr (!std::is_same_v<UnitIter, LimitIter>) {
            // Return the code unit sentinel.
            वापस unitRange.end();
        } अन्यथा अगर constexpr (prv::bidirectional_iterator<UnitIter>) {
            वापस utfIterator<CP32, behavior>(unitRange.begin(), unitRange.end(), unitRange.end());
        } अन्यथा {
            // The input iterator specialization has no three-argument constructor.
            वापस utfIterator<CP32, behavior>(unitRange.end(), unitRange.end());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    ढाँचा<typename R = Range, typename = std::enable_if_t<prv::range<स्थिर R>>>
    स्वतः end() स्थिर {
        using UnitIter = decltype(unitRange.begin());
        using LimitIter = decltype(unitRange.end());
        अगर constexpr (!std::is_same_v<UnitIter, LimitIter>) {
            // Return the code unit sentinel.
            वापस unitRange.end();
        } अन्यथा अगर constexpr (prv::bidirectional_iterator<UnitIter>) {
            वापस utfIterator<CP32, behavior>(unitRange.begin(), unitRange.end(), unitRange.end());
        } अन्यथा {
            // The input iterator specialization has no three-argument constructor.
            वापस utfIterator<CP32, behavior>(unitRange.end(), unitRange.end());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    स्वतः rbegin() स्थिर {
        वापस std::make_reverse_iterator(end());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    स्वतः rend() स्थिर {
        वापस std::make_reverse_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    स्वतः front() स्थिर {
        वापस *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    स्वतः back() स्थिर {
        वापस *(--end());
    }

निजी:
    Range unitRange;
};

/** @internal */
ढाँचा<typename CP32, UTFIllFormedBehavior behavior>
काष्ठा UTFStringCodePointsAdaptor
#अगर U_CPLUSPLUS_VERSION >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_closure<UTFStringCodePointsAdaptor<CP32, behavior>>
#पूर्ण_अगर
{
    /** @internal */
    ढाँचा<typename Range>
    स्वतः चालक()(Range &&unitRange) स्थिर {
#अगर defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        वापस UTFStringCodePoints<CP32, behavior, std::ranges::views::all_t<Range>>(
            std::forward<Range>(unitRange));
#अन्यथा
        अगर constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            वापस UTFStringCodePoints<CP32, behavior, std::decay_t<Range>>(
                std::forward<Range>(unitRange));
        } अन्यथा {
            वापस UTFStringCodePoints<CP32, behavior, Range>(std::forward<Range>(unitRange));
        }
#पूर्ण_अगर
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
ढाँचा<typename CP32, UTFIllFormedBehavior behavior>
constexpr UTFStringCodePointsAdaptor<CP32, behavior> utfStringCodePoints;

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
ढाँचा<typename CP32, typename UnitIter, typename = व्योम>
class UnsafeUTFIterator {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    खुला:
        explicit Proxy(UnsafeCodeUnits<CP32, UnitIter> &units) : units_(units) {}
        UnsafeCodeUnits<CP32, UnitIter>& चालक*() U_LIFETIME_BOUND { वापस units_; }
        UnsafeCodeUnits<CP32, UnitIter>* चालक->() U_LIFETIME_BOUND { वापस &units_; }
    निजी:
        UnsafeCodeUnits<CP32, UnitIter> units_;
    };

खुला:
    /** C++ iterator boilerplate @internal */
    using मूल्य_type = UnsafeCodeUnits<CP32, UnitIter>;
    /** C++ iterator boilerplate @internal */
    using reference = मूल्य_type;
    /** C++ iterator boilerplate @internal */
    using pointer = Proxy;
    /** C++ iterator boilerplate @internal */
    using difference_type = prv::iter_difference_t<UnitIter>;
    /** C++ iterator boilerplate @internal */
    using iterator_category = std::conditional_t<
        prv::bidirectional_iterator<UnitIter>,
        std::bidirectional_iterator_tag,
        std::forward_iterator_tag>;

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
    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = शेष;
    /** Move assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &चालक=(UnsafeUTFIterator &&src) noexcept = शेष;

    /** Copy constructor. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator(स्थिर UnsafeUTFIterator &other) = शेष;
    /** Copy assignment operator. @draft ICU 78 */
    U_FORCE_INLINE UnsafeUTFIterator &चालक=(स्थिर UnsafeUTFIterator &other) = शेष;

    /**
     * @param other Another iterator
     * @return true if this iterator is at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool चालक==(स्थिर UnsafeUTFIterator &other) स्थिर {
        वापस base() == other.base();
    }
    /**
     * @param other Another iterator
     * @return true if this iterator is not at the same position as the other one
     * @draft ICU 78
     */
    U_FORCE_INLINE bool चालक!=(स्थिर UnsafeUTFIterator &other) स्थिर { वापस !चालक==(other); }

    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is equal to the sentinel
     * @draft ICU 78
     */
    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक==(स्थिर UnsafeUTFIterator &iter, स्थिर Sentinel &s) {
        वापस iter.base() == s;
    }

#अगर U_CPLUSPLUS_VERSION < 20
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is equal to the sentinel
     * @internal
     */
    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक==(स्थिर Sentinel &s, स्थिर UnsafeUTFIterator &iter) {
        वापस iter.base() == s;
    }
    /**
     * @param iter An UnsafeUTFIterator
     * @param s A unit iterator sentinel
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक!=(स्थिर UnsafeUTFIterator &iter, स्थिर Sentinel &s) { वापस !(iter == s); }
    /**
     * @param s A unit iterator sentinel
     * @param iter An UnsafeUTFIterator
     * @return true if the iterator’s position is not equal to the sentinel
     * @internal
     */
    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक!=(स्थिर Sentinel &s, स्थिर UnsafeUTFIterator &iter) { वापस !(iter == s); }
#पूर्ण_अगर  // C++17

    /**
     * Returns the current position as a code unit iterator.
     * Similar to iter->begin() but also works at the exclusive end().
     *
     * @return current position as a code unit iterator
     * @draft ICU 79
     */
    U_FORCE_INLINE UnitIter base() स्थिर {
        // Return the logical position.
        वापस state_ <= 0 ? p_ : units_.begin();
    }

    /**
     * Decodes the code unit sequence at the current position.
     *
     * @return CodeUnits with the decoded code point etc.
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeCodeUnits<CP32, UnitIter> चालक*() स्थिर {
        अगर (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        वापस units_;
    }

    /**
     * Decodes the code unit sequence at the current position.
     * Used like <code>iter->codePoint()</code> or <code>iter->stringView()</code> etc.
     *
     * @return CodeUnits with the decoded code point etc., wrapped into
     *     an opaque proxy object so that <code>iter->codePoint()</code> etc. works.
     * @draft ICU 78
     */
    U_FORCE_INLINE Proxy चालक->() स्थिर {
        अगर (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            state_ = 1;
        }
        वापस Proxy(units_);
    }

    /**
     * Pre-increment operator.
     *
     * @return this iterator
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator& चालक++() U_LIFETIME_BOUND {  // pre-increment
        अगर (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            state_ = 0;
        } अन्यथा अगर (state_ == 0) {
            Impl::inc(p_);
        } अन्यथा /* state_ < 0 */ {
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.end();
            state_ = 0;
        }
        वापस *this;
    }

    /**
     * Post-increment operator.
     *
     * @return a copy of this iterator from before the increment.
     *     If UnitIter is a single-pass input_iterator, then this function
     *     returns an opaque proxy object so that <code>*iter++</code> still works.
     * @draft ICU 78
     */
    U_FORCE_INLINE UnsafeUTFIterator चालक++(पूर्णांक) {  // post-increment
        अगर (state_ > 0) {
            // operator*() called readAndInc() so p_ is already ahead.
            UnsafeUTFIterator परिणाम(*this);
            state_ = 0;
            वापस परिणाम;
        } अन्यथा अगर (state_ == 0) {
            UnitIter p0 = p_;
            units_ = Impl::readAndInc(p0, p_);
            UnsafeUTFIterator परिणाम(*this);
            परिणाम.state_ = 1;
            // keep this->state_ == 0
            वापस परिणाम;
        } अन्यथा /* state_ < 0 */ {
            UnsafeUTFIterator परिणाम(*this);
            // operator--() called decAndRead() so we know how far to skip.
            p_ = units_.end();
            state_ = 0;
            वापस परिणाम;
        }
    }

    /**
     * Pre-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return this iterator
     * @draft ICU 78
     */
    ढाँचा <typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator&>
    चालक--() U_LIFETIME_BOUND {  // pre-decrement
        अगर (state_ > 0) {
            // operator*() called readAndInc() so p_ is ahead of the logical position.
            p_ = units_.begin();
        }
        units_ = Impl::decAndRead(p_);
        state_ = -1;
        वापस *this;
    }

    /**
     * Post-decrement operator.
     * Only enabled if UnitIter is a bidirectional_iterator (including a pointer).
     *
     * @return a copy of this iterator from before the decrement.
     * @draft ICU 78
     */
    ढाँचा<typename Iter = UnitIter>
    U_FORCE_INLINE
    std::enable_if_t<prv::bidirectional_iterator<Iter>, UnsafeUTFIterator>
    चालक--(पूर्णांक) {  // post-decrement
        UnsafeUTFIterator परिणाम(*this);
        चालक--();
        वापस परिणाम;
    }

निजी:
    मित्र class std::reverse_iterator<UnsafeUTFIterator<CP32, UnitIter>>;

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable UnsafeCodeUnits<CP32, UnitIter> units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    mutable int8_t state_ = 0;
};

#अगर_अघोषित U_IN_DOXYGEN
// Partial template specialization for single-pass input iterator.
ढाँचा<typename CP32, typename UnitIter>
class UnsafeUTFIterator<
        CP32,
        UnitIter,
        std::enable_if_t<!prv::forward_iterator<UnitIter>>> {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = UnsafeUTFImpl<CP32, UnitIter>;

    // Proxy type for post-increment return value, to make *iter++ work.
    // Also for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    खुला:
        explicit Proxy(UnsafeCodeUnits<CP32, UnitIter> &units) : units_(units) {}
        UnsafeCodeUnits<CP32, UnitIter>& चालक*() U_LIFETIME_BOUND { वापस units_; }
        UnsafeCodeUnits<CP32, UnitIter>* चालक->() U_LIFETIME_BOUND { वापस &units_; }
    निजी:
        UnsafeCodeUnits<CP32, UnitIter> units_;
    };

खुला:
    using मूल्य_type = UnsafeCodeUnits<CP32, UnitIter>;
    using reference = मूल्य_type;
    using pointer = Proxy;
    using difference_type = prv::iter_difference_t<UnitIter>;
    using iterator_category = std::निवेश_iterator_tag;

    U_FORCE_INLINE explicit UnsafeUTFIterator(UnitIter p) : p_(std::move(p)) {}

    U_FORCE_INLINE UnsafeUTFIterator(UnsafeUTFIterator &&src) noexcept = शेष;
    U_FORCE_INLINE UnsafeUTFIterator &चालक=(UnsafeUTFIterator &&src) noexcept = शेष;

    U_FORCE_INLINE UnsafeUTFIterator(स्थिर UnsafeUTFIterator &other) = शेष;
    U_FORCE_INLINE UnsafeUTFIterator &चालक=(स्थिर UnsafeUTFIterator &other) = शेष;

    U_FORCE_INLINE bool चालक==(स्थिर UnsafeUTFIterator &other) स्थिर {
        वापस p_ == other.p_ && ahead_ == other.ahead_;
        // Strictly speaking, we should check if the logical position is the same.
        // However, we cannot advance, or do arithmetic with, a single-pass UnitIter.
    }
    U_FORCE_INLINE bool चालक!=(स्थिर UnsafeUTFIterator &other) स्थिर { वापस !चालक==(other); }

    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक==(स्थिर UnsafeUTFIterator &iter, स्थिर Sentinel &s) {
        वापस !iter.ahead_ && iter.p_ == s;
    }

#अगर U_CPLUSPLUS_VERSION < 20
    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक==(स्थिर Sentinel &s, स्थिर UnsafeUTFIterator &iter) {
        वापस !iter.ahead_ && iter.p_ == s;
    }

    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक!=(स्थिर UnsafeUTFIterator &iter, स्थिर Sentinel &s) { वापस !(iter == s); }

    ढाँचा<typename Sentinel> U_FORCE_INLINE मित्र
    std::enable_if_t<
        !std::is_same_v<Sentinel, UnsafeUTFIterator> && !std::is_same_v<Sentinel, UnitIter>,
        bool>
    चालक!=(स्थिर Sentinel &s, स्थिर UnsafeUTFIterator &iter) { वापस !(iter == s); }
#पूर्ण_अगर  // C++17

    U_FORCE_INLINE UnsafeCodeUnits<CP32, UnitIter> चालक*() स्थिर {
        अगर (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        वापस units_;
    }

    U_FORCE_INLINE Proxy चालक->() स्थिर {
        अगर (!ahead_) {
            units_ = Impl::readAndInc(p_, p_);
            ahead_ = true;
        }
        वापस Proxy(units_);
    }

    U_FORCE_INLINE UnsafeUTFIterator& चालक++() U_LIFETIME_BOUND {  // pre-increment
        अगर (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } अन्यथा {
            Impl::inc(p_);
        }
        वापस *this;
    }

    U_FORCE_INLINE Proxy चालक++(पूर्णांक) {  // post-increment
        अगर (ahead_) {
            // operator*() called readAndInc() so p_ is already ahead.
            ahead_ = false;
        } अन्यथा {
            units_ = Impl::readAndInc(p_, p_);
            // keep this->ahead_ == false
        }
        वापस Proxy(units_);
    }

निजी:
    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call readAndInc() only once for both operator*() and ++
    // so that we can use a single-pass input iterator for UnitIter.
    mutable UnsafeCodeUnits<CP32, UnitIter> units_ = {0, 0};
    // true: units_ = readAndInc(), p_ = units limit
    //     which means that p_ is ahead of its logical position
    // false: initial state
    mutable bool ahead_ = false;
};
#पूर्ण_अगर  // U_IN_DOXYGEN

}  // namespace U_HEADER_ONLY_NAMESPACE

#अगर_अघोषित U_IN_DOXYGEN
// Bespoke specialization of reverse_iterator.
// The default implementation implements reverse operator*() and ++ in a way
// that does most of the same work twice for reading variable-length sequences.
ढाँचा<typename CP32, typename UnitIter>
class std::reverse_iterator<U_HEADER_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>> {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
    using Impl = U_HEADER_ONLY_NAMESPACE::UnsafeUTFImpl<CP32, UnitIter>;
    using UnsafeCodeUnits_ = U_HEADER_ONLY_NAMESPACE::UnsafeCodeUnits<CP32, UnitIter>;

    // Proxy type for operator->() (required by LegacyInputIterator)
    // so that we don't promise always returning UnsafeCodeUnits.
    class Proxy {
    खुला:
        explicit Proxy(UnsafeCodeUnits_ units) : units_(units) {}
        UnsafeCodeUnits_& चालक*() U_LIFETIME_BOUND { वापस units_; }
        UnsafeCodeUnits_* चालक->() U_LIFETIME_BOUND { वापस &units_; }
    निजी:
        UnsafeCodeUnits_ units_;
    };

खुला:
    using मूल्य_type = UnsafeCodeUnits_;
    using reference = मूल्य_type;
    using pointer = Proxy;
    using difference_type = U_HEADER_ONLY_NAMESPACE::prv::iter_difference_t<UnitIter>;
    using iterator_category = std::bidirectional_iterator_tag;

    U_FORCE_INLINE explicit reverse_iterator(U_HEADER_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> iter) :
            p_(iter.base()), units_(0, 0, p_, p_) {}
    U_FORCE_INLINE reverse_iterator() : p_{}, units_(0, 0, p_, p_) {}

    U_FORCE_INLINE reverse_iterator(reverse_iterator &&src) noexcept = शेष;
    U_FORCE_INLINE reverse_iterator &चालक=(reverse_iterator &&src) noexcept = शेष;

    U_FORCE_INLINE reverse_iterator(स्थिर reverse_iterator &other) = शेष;
    U_FORCE_INLINE reverse_iterator &चालक=(स्थिर reverse_iterator &other) = शेष;

    U_FORCE_INLINE bool चालक==(स्थिर reverse_iterator &other) स्थिर {
        वापस getLogicalPosition() == other.getLogicalPosition();
    }
    U_FORCE_INLINE bool चालक!=(स्थिर reverse_iterator &other) स्थिर { वापस !चालक==(other); }

    U_FORCE_INLINE U_HEADER_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter> base() स्थिर {
        वापस U_HEADER_ONLY_NAMESPACE::UnsafeUTFIterator<CP32, UnitIter>(
            getLogicalPosition());
    }

    U_FORCE_INLINE UnsafeCodeUnits_ चालक*() स्थिर {
        अगर (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        वापस units_;
    }

    U_FORCE_INLINE Proxy चालक->() स्थिर {
        अगर (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            state_ = -1;
        }
        वापस Proxy(units_);
    }

    U_FORCE_INLINE reverse_iterator& चालक++() U_LIFETIME_BOUND {  // pre-increment
        अगर (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            state_ = 0;
        } अन्यथा अगर (state_ == 0) {
            Impl::dec(p_);
        } अन्यथा /* state_ > 0 */ {
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
        }
        वापस *this;
    }

    U_FORCE_INLINE reverse_iterator चालक++(पूर्णांक) {  // post-increment
        अगर (state_ < 0) {
            // operator*() called decAndRead() so p_ is already behind.
            reverse_iterator परिणाम(*this);
            state_ = 0;
            वापस परिणाम;
        } अन्यथा अगर (state_ == 0) {
            units_ = Impl::decAndRead(p_);
            reverse_iterator परिणाम(*this);
            परिणाम.state_ = -1;
            // keep this->state_ == 0
            वापस परिणाम;
        } अन्यथा /* state_ > 0 */ {
            reverse_iterator परिणाम(*this);
            // operator--() called readAndInc() so we know how far to skip.
            p_ = units_.begin();
            state_ = 0;
            वापस परिणाम;
        }
    }

    U_FORCE_INLINE reverse_iterator& चालक--() U_LIFETIME_BOUND {  // pre-decrement
        अगर (state_ < 0) {
            // operator*() called decAndRead() so p_ is behind the logical position.
            p_ = units_.end();
        }
        UnitIter p0 = p_;
        units_ = Impl::readAndInc(p0, p_);
        state_ = 1;
        वापस *this;
    }

    U_FORCE_INLINE reverse_iterator चालक--(पूर्णांक) {  // post-decrement
        reverse_iterator परिणाम(*this);
        चालक--();
        वापस परिणाम;
    }

निजी:
    U_FORCE_INLINE UnitIter getLogicalPosition() स्थिर {
        वापस state_ >= 0 ? p_ : units_.end();
    }

    // operator*() etc. are logically const.
    mutable UnitIter p_;
    // Keep state so that we call decAndRead() only once for both operator*() and ++
    // to make it easy for the compiler to optimize.
    mutable UnsafeCodeUnits_ units_;
    // >0: units_ = readAndInc(), p_ = units limit
    //  0: initial state
    // <0: units_ = decAndRead(), p_ = units start
    //     which means that p_ is behind its logical position
    mutable int8_t state_ = 0;
};
#पूर्ण_अगर  // U_IN_DOXYGEN

namespace U_HEADER_ONLY_NAMESPACE {

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
ढाँचा<typename CP32, typename UnitIter>
स्वतः unsafeUTFIterator(UnitIter iter) {
    वापस UnsafeUTFIterator<CP32, UnitIter>(std::move(iter));
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
ढाँचा<typename CP32, typename Range>
#अगर defined(__cpp_lib_ranges)
    requires std::ranges::range<Range>
#पूर्ण_अगर
class UnsafeUTFStringCodePoints {
    static_assert(माप(CP32) == 4, "CP32 must be a 32-bit type to hold a code point");
खुला:
    /**
     * Constructs an empty C++ "range" object.
     * @draft ICU 78
     */
    UnsafeUTFStringCodePoints() = शेष;

    /**
     * Constructs a C++ "range" object over the code points in the string.
     * @param unitRange input range
     * @draft ICU 78
     */
    ढाँचा<typename R = Range, typename = std::enable_if_t<!std::is_reference_v<R>>>
    explicit UnsafeUTFStringCodePoints(Range unitRange) : unitRange(std::move(unitRange)) {}
    /**
     * Constructs a C++ "range" object over the code points in the string,
     * keeping a reference to the code unit range.  This overload is used by
     * utfStringCodePoints in C++17; in C++20, a ref_view is used instead (via
     * views::all).
     * @param unitRange input range
     * @draft ICU 78
     */
    ढाँचा<typename R = Range, typename = std::enable_if_t<std::is_reference_v<R>>, typename = व्योम>
    explicit UnsafeUTFStringCodePoints(Range unitRange U_LIFETIME_BOUND) : unitRange(unitRange) {}

    /** Copy constructor. @draft ICU 78 */
    UnsafeUTFStringCodePoints(स्थिर UnsafeUTFStringCodePoints &other) = शेष;

    /** Copy assignment operator. @draft ICU 78 */
    UnsafeUTFStringCodePoints &चालक=(स्थिर UnsafeUTFStringCodePoints &other) = शेष;

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    स्वतः begin() {
        वापस unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range start iterator
     * @draft ICU 78
     */
    ढाँचा<typename R = Range, typename = std::enable_if_t<prv::range<स्थिर R>>>
    स्वतः begin() स्थिर {
        वापस unsafeUTFIterator<CP32>(unitRange.begin());
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    स्वतः end() {
        using UnitIter = decltype(unitRange.begin());
        using LimitIter = decltype(unitRange.end());
        अगर constexpr (!std::is_same_v<UnitIter, LimitIter>) {
            // Return the code unit sentinel.
            वापस unitRange.end();
        } अन्यथा {
            वापस unsafeUTFIterator<CP32>(unitRange.end());
        }
    }

    /**
     * @return the range limit (exclusive end) iterator
     * @draft ICU 78
     */
    ढाँचा<typename R = Range, typename = std::enable_if_t<prv::range<स्थिर R>>>
    स्वतः end() स्थिर {
        using UnitIter = decltype(unitRange.begin());
        using LimitIter = decltype(unitRange.end());
        अगर constexpr (!std::is_same_v<UnitIter, LimitIter>) {
            // Return the code unit sentinel.
            वापस unitRange.end();
        } अन्यथा {
            वापस unsafeUTFIterator<CP32>(unitRange.end());
        }
    }

    /**
     * @return std::reverse_iterator(end())
     * @draft ICU 78
     */
    स्वतः rbegin() स्थिर {
        वापस std::make_reverse_iterator(end());
    }

    /**
     * @return std::reverse_iterator(begin())
     * @draft ICU 78
     */
    स्वतः rend() स्थिर {
        वापस std::make_reverse_iterator(begin());
    }

    /**
     * Returns the CodeUnits for the first character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the first character/code point.
     * @draft ICU 79
     */
    स्वतः front() स्थिर {
        वापस *begin();
    }

    /**
     * Returns the CodeUnits for the last character/code point.
     * Requires that the range is not empty.
     *
     * @return the CodeUnits for the last character/code point.
     * @draft ICU 79
     */
    स्वतः back() स्थिर {
        वापस *(--end());
    }

निजी:
    Range unitRange;
};

/** @internal */
ढाँचा<typename CP32>
काष्ठा UnsafeUTFStringCodePointsAdaptor
#अगर U_CPLUSPLUS_VERSION >= 23 && __cpp_lib_ranges >= 2022'02 &&                                         \
    __cpp_lib_bind_back >= 2022'02 // http://wg21.link/P2387R3.
    : std::ranges::range_adaptor_closure<UnsafeUTFStringCodePointsAdaptor<CP32>>
#पूर्ण_अगर
{
    /** @internal */
    ढाँचा<typename Range>
    स्वतः चालक()(Range &&unitRange) स्थिर {
#अगर defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 2021'10  // We need https://wg21.link/P2415R2.
        वापस UnsafeUTFStringCodePoints<CP32, std::ranges::views::all_t<Range>>(std::forward<Range>(unitRange));
#अन्यथा
        अगर constexpr (prv::is_basic_string_view_v<std::decay_t<Range>>) {
            // Take basic_string_view by copy, not by reference.  In C++20 this is handled by
            // all_t<Range>, which is Range if Range is a view.
            वापस UnsafeUTFStringCodePoints<CP32, std::decay_t<Range>>(std::forward<Range>(unitRange));
        } अन्यथा {
            वापस UnsafeUTFStringCodePoints<CP32, Range>(std::forward<Range>(unitRange));
        }
#पूर्ण_अगर
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
ढाँचा<typename CP32>
constexpr UnsafeUTFStringCodePointsAdaptor<CP32> unsafeUTFStringCodePoints;

}  // namespace U_HEADER_ONLY_NAMESPACE


#अगर defined(__cpp_lib_ranges)
ढाँचा <typename CP32, UTFIllFormedBehavior behavior, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_HEADER_ONLY_NAMESPACE::UTFStringCodePoints<CP32, behavior, Range>> =
    std::ranges::enable_borrowed_range<Range>;

ढाँचा <typename CP32, typename Range>
constexpr bool std::ranges::enable_borrowed_range<
    U_HEADER_ONLY_NAMESPACE::UnsafeUTFStringCodePoints<CP32, Range>> =
    std::ranges::enable_borrowed_range<Range>;
#पूर्ण_अगर

#पूर्ण_अगर  // U_HIDE_DRAFT_API
#पूर्ण_अगर  // U_SHOW_CPLUSPLUS_API || U_SHOW_CPLUSPLUS_HEADER_API
#पूर्ण_अगर  // __UTFITERATOR_H__
