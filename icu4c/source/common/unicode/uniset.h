// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
* Copyright (C) 1999-2016, International Business Machines Corporation
* and others. All Rights Reserved.
***************************************************************************
*   Date        Name        Description
*   10/20/99    alan        Creation.
***************************************************************************
*/

#اگر_غیر_تعریف_شدہ UNICODESET_H
#تعریف_کرو UNICODESET_H

#شامل "unicode/utypes.h"

#اگر U_SHOW_CPLUSPLUS_API

#شامل "unicode/ucpmap.h"
#شامل "unicode/unifilt.h"
#شامل "unicode/unistr.h"
#شامل "unicode/uset.h"

/**
 * \file
 * \brief C++ API: Unicode Set
 */

U_NAMESPACE_BEGIN

// Forward Declarations.
class BMPSet;
class ParsePosition;
class RBBIRuleScanner;
class علامت_جدول;
class Unicode_سیٹ_String_وسعت;
class UVector;
class قاعدہ_Character_Iterator;

/**
 * A mutable set of Unicode characters and multicharacter strings. Objects of this class represent
 * <em>character classes</em> used in regular expressions. A UnicodeSet specifies a finite set of
 * Unicode code point sequences, optimized for single code points.
 *
 * <p>Note: method freeze() will not only make the set immutable, but also makes important methods
 * much higher performance: contains(c), containsNone(...), span(...), spanBack(...) etc. After the
 * object is frozen, any subsequent call that wants to change the object will throw
 * UnsupportedOperationException.
 *
 * <p>The UnicodeSet class is not designed to be subclassed.
 *
 * <p><code>UnicodeSet</code> supports two APIs. The first is the <em>operand</em> API that allows
 * the caller to modify the value of a <code>UnicodeSet</code> object. It conforms to Java 2's
 * <code>java.util.Set</code> interface, although <code>UnicodeSet</code> does not actually
 * implement that interface. All methods of <code>Set</code> are supported, with the modification
 * that they take a character range, a single character, or a string instead of an <code>Object
 * </code>, and they take a <code>UnicodeSet</code> instead of a <code>Collection</code>. The
 * operand API may be thought of in terms of boolean logic: a boolean OR is implemented by <code>add
 * </code>, a boolean AND is implemented by <code>retain</code>, a boolean XOR is implemented by
 * <code>complement
 * </code> taking an argument, and a boolean NOT is implemented by <code>complement</code> with no
 * argument. In terms of traditional set theory function names, <code>add</code> is a union, <code>
 * retain</code> is an intersection, <code>remove</code> is an asymmetric difference, and <code>
 * complement</code> with no argument is a set complement with respect to the superset range <code>
 * MIN_VALUE-MAX_VALUE</code>.
 *
 * <p>The second API for building up a UnicodeSet is the <code>applyPattern()</code>/<code>
 * toPattern()</code> API from the <code>
 * java.text.Format</code>-derived classes. Unlike the methods that add characters, add categories,
 * and control the logic of the set, the method <code>applyPattern()</code> sets all attributes of a
 * <code>UnicodeSet</code> at once, based on a pattern string.
 *
 * <p><b>Pattern syntax</b> Pattern strings are accepted by the constructors and the {@link
 * #applyPattern()} methods and returned by the {@link #toPattern()} method. These patterns follow
 * the UnicodeSet syntax defined by <a href="https://www.unicode.org/reports/tr61/">Draft Unicode
 * Technical Standard #61, UnicodeSet notation.</a>
 *
 * <blockquote>
 *
 * <table>
 *     <tr style="vertical-align: top">
 *       <td style="white-space: nowrap; vertical-align: top; horizontal-align: left;"><code>[]</code></td>
 *       <td style="vertical-align: top;">No characters</td>
 *     </tr><tr style="vertical-align: top">
 *       <td style="white-space: nowrap; vertical-align: top; horizontal-align: left;"><code>[a]</code></td>
 *       <td style="vertical-align: top;">The character 'a'</td>
 *     </tr><tr style="vertical-align: top">
 *       <td style="white-space: nowrap; vertical-align: top; horizontal-align: left;"><code>[ae]</code></td>
 *       <td style="vertical-align: top;">The characters 'a' and 'e'</td>
 *     </tr>
 *     <tr>
 *       <td style="white-space: nowrap; vertical-align: top; horizontal-align: left;"><code>[a-e]</code></td>
 *       <td style="vertical-align: top;">The characters 'a' through 'e' inclusive, in Unicode code
 *       point order</td>
 *     </tr>
 *     <tr>
 *       <td style="white-space: nowrap; vertical-align: top; horizontal-align: left;"><code>[\\u4E01]</code></td>
 *       <td style="vertical-align: top;">The character U+4E01</td>
 *     </tr>
 *     <tr>
 *       <td style="white-space: nowrap; vertical-align: top; horizontal-align: left;"><code>[a{ab}{ac}]</code></td>
 *       <td style="vertical-align: top;">The character 'a' and the multicharacter strings &quot;ab&quot; and
 *       &quot;ac&quot;</td>
 *     </tr>
 *     <tr>
 *       <td style="white-space: nowrap; vertical-align: top; horizontal-align: left;"><code>[\\p{Lu}]</code></td>
 *       <td style="vertical-align: top;">All characters in the general category Uppercase Letter</td>
 *     </tr>
 *     <tr>
 *       <td style="white-space: nowrap; vertical-align: top; horizontal-align: left;"><code>[[:Letter:][:Number_Letter:]]</code></td>
 *       <td style="vertical-align: top;">All characters that have a General_Category in the Letter grouping or have the General_Category Number_Letter</td>
 *     </tr>
 *     <tr>
 *       <td style="white-space: nowrap; vertical-align: top; horizontal-align: left;"><code>[[:Word_Break=ALetter:]-[:Letter:]]</code></td>
 *       <td style="vertical-align: top;">All characters with a Word_Break property assignment of ALetter that do not have a General_Category in the Letter grouping</td>
 *     </tr>
 *     <tr>
 *       <td style="white-space: nowrap; vertical-align: top; horizontal-align: left;"><code>[\\p{Greek}&\\p{Letter}]</code></td>
 *       <td style="vertical-align: top;">All characters with a Script property assignment of Greek whose General_Category is in the Letter grouping</td>
 *     </tr>
 *     <tr>
 *       <td style="white-space: nowrap; vertical-align: top; horizontal-align: left;"><code>[^a]</code></td>
 *       <td style="vertical-align: top;">All code points except U+0061 'a'.</td>
 *     </tr>
 *   </table>
 *
 * </blockquote>
 *
 * Most characters may be preceded by a backslash in order to remove any special meaning. Unescaped
 * white space characters, as defined by the Unicode Pattern_White_Space property, are ignored
 * outside of string literals, except insofar as they separate lexical elements.
 *
 * <p>Property queries specify a set of characters having a certain property as defined by the
 * Unicode standard. Both the POSIX-like "[:Lu:]" and the Perl-like syntax "\\p{Lu}" are recognized.
 *
 * <p>As shown in the examples above, UnicodeSet syntax supports some set operations: the union of
 * sets 𝑋 and 𝑌 union is written [𝑋𝑌], their intersection [𝑋&amp;𝑌], and their asymmetric set
 * difference (the complement of 𝑌 in 𝑋) is [𝑋-𝑌]. The code point complement of 𝑋, equivalent
 * to .{@link #complement()}.{@link #removeAllStrings()}, is written [^𝑋].
 *
 * <p>Ranges are indicated by placing a '-' between two characters, as in "a-z". This specifies the
 * range of all characters from the left to the right, in code point order.
 *
 * <p><b>Formal specification</b>
 *
 * <p>This class is a conformant and consistent implementation of UnicodeSet notation as defined in
 * <a href="https://www.unicode.org/reports/tr61/">Draft Unicode Technical Standard #61, UnicodeSet
 * notation</a>. It supports only property queries that are recommended for general-purpose APIs
 * (that is, it does not support the productions with a gray background in the grammar). The set of
 * supported properties, and additional restrictions, are described in the User Guide <a
 * href="https://unicode-org.github.io/icu/userguide/strings/unicodeset#Restrictions">UnicodeSet /
 * Restrictions</a>.
 *
 * <p>It implements a number of pure extensions documented in the User Guide at <a
 * href="https://unicode-org.github.io/icu/userguide/strings/unicodeset#Extensions">UnicodeSet /
 * Extensions</a>. In particular, the support for variables interacts with the API: If a {@link
 * SymbolTable} is passed to the constructor of {@code UnicodeSet}, a lexical element <a
 * href=https://unicode-org.github.io/icu/userguide/strings/unicodeset#variable>variable</a> is
 * introduced, whose definition is given by the implementation of {@link
 * SymbolTable#parseReference}.
 * 
 * <p>Note:
 *  - Most UnicodeSet methods do not take a UErrorCode parameter because
 *   there are usually very few opportunities for failure other than a shortage
 *   of memory, error codes in low-level C++ string methods would be inconvenient,
 *   and the error code as the last parameter (ICU convention) would prevent
 *   the use of default parameter values.
 *   Instead, such methods set the UnicodeSet into a "bogus" state
 *   (see isBogus()) if an error occurs.
 *
 * @author Alan Liu
 * @stable ICU 2.0
 */
class U_COMMON_API Unicode_سیٹ final : عوامی UnicodeFilter {
نجی:
    /**
     * Enough for sets with few ranges.
     * For example, White_Space has 10 ranges, list length 21.
     */
    ساکن constexpr int32_t INITIAL_استعداد = 25;
    // fFlags constant
    ساکن constexpr uint8_t kIsBogus = 1;  // This set is bogus (i.e. not valid)

    UChar32* list = ڈھیر_List; // MUST be terminated with HIGH
    int32_t استعداد = INITIAL_استعداد; // capacity of list
    int32_t len = 1; // length of list used; 1 <= len <= capacity
    uint8_t fFlags = 0;         // Bit flag (see constants above)

    BMPSet *bmp_سیٹ = nullptr; // The set is frozen iff either bmpSet or stringSpan is not nullptr.
    UChar32* بفر = nullptr; // internal buffer, may be nullptr
    int32_t بفر_استعداد = 0; // capacity of buffer

    /**
     * The pattern representation of this set.  This may not be the
     * most economical pattern.  It is the pattern supplied to
     * applyPattern(), with variables substituted and whitespace
     * removed.  For sets constructed without applyPattern(), or
     * modified using the non-pattern API, this string will be empty,
     * indicating that toPattern() must generate a pattern
     * representation from the inversion list.
     */
    char16_t *pat = nullptr;
    int32_t patLen = 0;

    UVector* لڑیاں_2 = nullptr; // maintained in sorted order
    Unicode_سیٹ_String_وسعت *string_وسعت = nullptr;

    /**
     * Initial list array.
     * Avoids some heap allocations, and list is never nullptr.
     * Increases the object size a bit.
     */
    UChar32 ڈھیر_List[INITIAL_استعداد];

عوامی:
    /**
     * Determine if this object contains a valid set.
     * A bogus set has no value. It is different from an empty set.
     * It can be used to indicate that no set value is available.
     *
     * @return true if the set is bogus/invalid, false otherwise
     * @see setToBogus()
     * @stable ICU 4.0
     */
    اندرونی UBool isBogus() ثابت;

    /**
     * Make this UnicodeSet object invalid.
     * The string will test true with isBogus().
     *
     * A bogus set has no value. It is different from an empty set.
     * It can be used to indicate that no set value is available.
     *
     * This utility function is used throughout the UnicodeSet
     * implementation to indicate that a UnicodeSet operation failed,
     * and may be used in other functions,
     * especially but not exclusively when such functions do not
     * take a UErrorCode for simplicity.
     *
     * @see isBogus()
     * @stable ICU 4.0
     */
    خالی سیٹ_مقصد_Bogus();

عوامی:

    شمار {
        /**
         * Minimum value that can be stored in a UnicodeSet.
         * @stable ICU 2.4
         */
        MIN_قدر = 0,

        /**
         * Maximum value that can be stored in a UnicodeSet.
         * @stable ICU 2.4
         */
        MAX_قدر = 0x10ffff
    };

    //----------------------------------------------------------------
    // Constructors &c
    //----------------------------------------------------------------

عوامی:

    /**
     * Constructs an empty set.
     * @stable ICU 2.0
     */
    Unicode_سیٹ();

    /**
     * Constructs a set containing the given range. If <code>end <
     * start</code> then an empty set is created.
     *
     * @param start first character, inclusive, of range
     * @param end last character, inclusive, of range
     * @stable ICU 2.4
     */
    Unicode_سیٹ(UChar32 آغاز, UChar32 انتہا);

#اگر_غیر_تعریف_شدہ U_HIDE_اندرونی_API
    /**
     * @internal
     */
    شمار ESerialization {
      kSerialized  /* result of serialize() */
    };

    /**
     * Constructs a set from the output of serialize().
     *
     * @param buffer the 16 bit array
     * @param bufferLen the original length returned from serialize()
     * @param serialization the value 'kSerialized'
     * @param status error code
     *
     * @internal
     */
    Unicode_سیٹ(ثابت uint16_t بفر[], int32_t بفر_Len,
               ESerialization serialization, UError_رمز &حالت_2);
#اگر_ختم  /* U_HIDE_INTERNAL_API */

    /**
     * Constructs a set from the given pattern.  See the class
     * description for the syntax of the pattern language.
     * @param pattern a string specifying what characters are in the set
     * @param status returns <code>U_ILLEGAL_ARGUMENT_ERROR</code> if the pattern
     * contains a syntax error.
     * @stable ICU 2.0
     */
    Unicode_سیٹ(ثابت UnicodeString& نمونہ,
               UError_رمز& حالت_2);

#اگر_غیر_تعریف_شدہ U_HIDE_اندرونی_API
    /**
     * Constructs a set from the given pattern.  See the class
     * description for the syntax of the pattern language.
     * @param pattern a string specifying what characters are in the set
     * @param options bitmask for options to apply to the pattern.
     * Valid options are USET_IGNORE_SPACE and
     * at most one of USET_CASE_INSENSITIVE, USET_ADD_CASE_MAPPINGS, USET_SIMPLE_CASE_INSENSITIVE.
     * These case options are mutually exclusive.
     * @param symbols a symbol table mapping variable names to values
     * and stand-in characters to UnicodeSets; may be nullptr
     * @param status returns <code>U_ILLEGAL_ARGUMENT_ERROR</code> if the pattern
     * contains a syntax error.
     * @internal
     */
    Unicode_سیٹ(ثابت UnicodeString& نمونہ,
               uint32_t options,
               ثابت علامت_جدول* symbols,
               UError_رمز& حالت_2);
#اگر_ختم  /* U_HIDE_INTERNAL_API */

    /**
     * Constructs a set from the given pattern.  See the class description
     * for the syntax of the pattern language.
     * @param pattern a string specifying what characters are in the set
     * @param pos on input, the position in pattern at which to start parsing.
     * On output, the position after the last character parsed.
     * @param options bitmask for options to apply to the pattern.
     * Valid options are USET_IGNORE_SPACE and
     * at most one of USET_CASE_INSENSITIVE, USET_ADD_CASE_MAPPINGS, USET_SIMPLE_CASE_INSENSITIVE.
     * These case options are mutually exclusive.
     * @param symbols a symbol table mapping variable names to values
     * and stand-in characters to UnicodeSets; may be nullptr
     * @param status input-output error code
     * @stable ICU 2.8
     */
    Unicode_سیٹ(ثابت UnicodeString& نمونہ, ParsePosition& pos,
               uint32_t options,
               ثابت علامت_جدول* symbols,
               UError_رمز& حالت_2);

    /**
     * Constructs a set that is identical to the given UnicodeSet.
     * @stable ICU 2.0
     */
    Unicode_سیٹ(ثابت Unicode_سیٹ& o);

    /**
     * Destructs the set.
     * @stable ICU 2.0
     */
    مجازی ~Unicode_سیٹ();

    /**
     * Assigns this object to be a copy of another.
     * A frozen set will not be modified.
     * @stable ICU 2.0
     */
    Unicode_سیٹ& عامل=(ثابت Unicode_سیٹ& o) U_LIFETIME_BOUND;

    /**
     * Compares the specified object with this set for equality.  Returns
     * <tt>true</tt> if the two sets
     * have the same size, and every member of the specified set is
     * contained in this set (or equivalently, every member of this set is
     * contained in the specified set).
     *
     * @param o set to be compared for equality with this set.
     * @return <tt>true</tt> if the specified set is equal to this set.
     * @stable ICU 2.0
     */
    bool عامل==(ثابت Unicode_سیٹ& o) ثابت;

    /**
     * Compares the specified object with this set for equality.  Returns
     * <tt>true</tt> if the specified set is not equal to this set.
     * @stable ICU 2.0
     */
    اندرونی bool عامل!=(ثابت Unicode_سیٹ& o) ثابت;

    /**
     * Returns a copy of this object.  All UnicodeFunctor objects have
     * to support cloning in order to allow classes using
     * UnicodeFunctors, such as Transliterator, to implement cloning.
     * If this set is frozen, then the clone will be frozen as well.
     * Use cloneAsThawed() for a mutable clone of a frozen set.
     * @see cloneAsThawed
     * @stable ICU 2.0
     */
    مجازی Unicode_سیٹ* clone() ثابت override;

    /**
     * Returns the hash code value for this set.
     *
     * @return the hash code value for this set.
     * @see Object#hashCode()
     * @stable ICU 2.0
     */
    int32_t hash_رمز() ثابت;

    /**
     * Get a UnicodeSet pointer from a USet
     *
     * @param uset a USet (the ICU plain C type for UnicodeSet)
     * @return the corresponding UnicodeSet pointer.
     *
     * @stable ICU 4.2
     */
    اندرونی ساکن Unicode_سیٹ* fromUSet(USet* uset U_LIFETIME_BOUND);

    /**
     * Get a UnicodeSet pointer from a const USet
     *
     * @param uset a const USet (the ICU plain C type for UnicodeSet)
     * @return the corresponding UnicodeSet pointer.
     *
     * @stable ICU 4.2
     */
    اندرونی ساکن ثابت Unicode_سیٹ* fromUSet(ثابت USet* uset U_LIFETIME_BOUND);

    /**
     * Produce a USet * pointer for this UnicodeSet.
     * USet is the plain C type for UnicodeSet
     *
     * @return a USet pointer for this UnicodeSet
     * @stable ICU 4.2
     */
    اندرونی USet* مقصد_USet() U_LIFETIME_BOUND;

    /**
     * Produce a const USet * pointer for this UnicodeSet.
     * USet is the plain C type for UnicodeSet
     *
     * @return a const USet pointer for this UnicodeSet
     * @stable ICU 4.2
     */
    اندرونی ثابت USet* مقصد_USet() ثابت U_LIFETIME_BOUND;


    //----------------------------------------------------------------
    // Freezable API
    //----------------------------------------------------------------

    /**
     * Determines whether the set has been frozen (made immutable) or not.
     * See the ICU4J Freezable interface for details.
     * @return true/false for whether the set has been frozen
     * @see freeze
     * @see cloneAsThawed
     * @stable ICU 3.8
     */
    اندرونی UBool isFrozen() ثابت;

    /**
     * Freeze the set (make it immutable).
     * Once frozen, it cannot be unfrozen and is therefore thread-safe
     * until it is deleted.
     * See the ICU4J Freezable interface for details.
     * Freezing the set may also make some operations faster, for example
     * contains() and span().
     * A frozen set will not be modified. (It remains frozen.)
     * @return this set.
     * @see isFrozen
     * @see cloneAsThawed
     * @stable ICU 3.8
     */
    Unicode_سیٹ* freeze() U_LIFETIME_BOUND;

    /**
     * Clone the set and make the clone mutable.
     * See the ICU4J Freezable interface for details.
     * @return the mutable clone
     * @see freeze
     * @see isFrozen
     * @stable ICU 3.8
     */
    Unicode_سیٹ *cloneAsThawed() ثابت;

    //----------------------------------------------------------------
    // Public API
    //----------------------------------------------------------------

    /**
     * Make this object represent the range `start - end`.
     * If `start > end` then this object is set to an empty range.
     * A frozen set will not be modified.
     *
     * @param start first character in the set, inclusive
     * @param end last character in the set, inclusive
     * @stable ICU 2.4
     */
    Unicode_سیٹ& سیٹ(UChar32 آغاز, UChar32 انتہا) U_LIFETIME_BOUND;

    /**
     * Return true if the given position, in the given pattern, appears
     * to be the start of a UnicodeSet pattern.
     * @stable ICU 2.4
     */
    ساکن UBool resembles_نمونہ(ثابت UnicodeString& نمونہ,
                                  int32_t pos);

    /**
     * Modifies this set to represent the set specified by the given
     * pattern, ignoring Unicode Pattern_White_Space characters.
     * See the class description for the syntax of the pattern language.
     * A frozen set will not be modified.
     * @param pattern a string specifying what characters are in the set
     * @param status returns <code>U_ILLEGAL_ARGUMENT_ERROR</code> if the pattern
     * contains a syntax error.
     * <em> Empties the set passed before applying the pattern.</em>
     * @return a reference to this
     * @stable ICU 2.0
     */
    Unicode_سیٹ& apply_نمونہ(ثابت UnicodeString& نمونہ, UError_رمز& حالت_2) U_LIFETIME_BOUND;

#اگر_غیر_تعریف_شدہ U_HIDE_اندرونی_API
    /**
     * Modifies this set to represent the set specified by the given
     * pattern, optionally ignoring Unicode Pattern_White_Space characters.
     * See the class description for the syntax of the pattern language.
     * A frozen set will not be modified.
     * @param pattern a string specifying what characters are in the set
     * @param options bitmask for options to apply to the pattern.
     * Valid options are USET_IGNORE_SPACE and
     * at most one of USET_CASE_INSENSITIVE, USET_ADD_CASE_MAPPINGS, USET_SIMPLE_CASE_INSENSITIVE.
     * These case options are mutually exclusive.
     * @param symbols a symbol table mapping variable names to
     * values and stand-ins to UnicodeSets; may be nullptr
     * @param status returns <code>U_ILLEGAL_ARGUMENT_ERROR</code> if the pattern
     * contains a syntax error.
     *<em> Empties the set passed before applying the pattern.</em>
     * @return a reference to this
     * @internal
     */
    Unicode_سیٹ& apply_نمونہ(ثابت UnicodeString& نمونہ,
                             uint32_t options,
                             ثابت علامت_جدول* symbols,
                             UError_رمز& حالت_2) U_LIFETIME_BOUND;
#اگر_ختم  /* U_HIDE_INTERNAL_API */

    /**
     * Parses the given pattern, starting at the given position.  The
     * character at pattern.charAt(pos.getIndex()) must be '[', or the
     * parse fails.  Parsing continues until the corresponding closing
     * ']'.  If a syntax error is encountered between the opening and
     * closing brace, the parse fails.  Upon return from a successful
     * parse, the ParsePosition is updated to point to the character
     * following the closing ']', and a StringBuffer containing a
     * pairs list for the parsed pattern is returned.  This method calls
     * itself recursively to parse embedded subpatterns.
     *<em> Empties the set passed before applying the pattern.</em>
     * A frozen set will not be modified.
     *
     * @param pattern the string containing the pattern to be parsed.
     * The portion of the string from pos.getIndex(), which must be a
     * '[', to the corresponding closing ']', is parsed.
     * @param pos upon entry, the position at which to being parsing.
     * The character at pattern.charAt(pos.getIndex()) must be a '['.
     * Upon return from a successful parse, pos.getIndex() is either
     * the character after the closing ']' of the parsed pattern, or
     * pattern.length() if the closing ']' is the last character of
     * the pattern string.
     * @param options bitmask for options to apply to the pattern.
     * Valid options are USET_IGNORE_SPACE and
     * at most one of USET_CASE_INSENSITIVE, USET_ADD_CASE_MAPPINGS, USET_SIMPLE_CASE_INSENSITIVE.
     * These case options are mutually exclusive.
     * @param symbols a symbol table mapping variable names to
     * values and stand-ins to UnicodeSets; may be nullptr
     * @param status returns <code>U_ILLEGAL_ARGUMENT_ERROR</code> if the pattern
     * contains a syntax error.
     * @return a reference to this
     * @stable ICU 2.8
     */
    Unicode_سیٹ& apply_نمونہ(ثابت UnicodeString& نمونہ,
                             ParsePosition& pos,
                             uint32_t options,
                             ثابت علامت_جدول* symbols,
                             UError_رمز& حالت_2) U_LIFETIME_BOUND;

    /**
     * Returns a string representation of this set.  If the result of
     * calling this function is passed to a UnicodeSet constructor, it
     * will produce another set that is equal to this one.
     * A frozen set will not be modified.
     * @param result the string to receive the rules.  Previous
     * contents will be deleted.
     * @param escapeUnprintable if true then convert unprintable
     * character to their hex escape representations, \\uxxxx or
     * \\Uxxxxxxxx.  Unprintable characters are those other than
     * U+000A, U+0020..U+007E.
     * @stable ICU 2.0
     */
    مجازی UnicodeString& مقصد_نمونہ(UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                     UBool escapeUnprintable = false) ثابت override;

    /**
     * Modifies this set to contain those code points which have the given value
     * for the given binary or enumerated property, as returned by
     * u_getIntPropertyValue.  Prior contents of this set are lost.
     * A frozen set will not be modified.
     *
     * @param prop a property in the range UCHAR_BIN_START..UCHAR_BIN_LIMIT-1
     * or UCHAR_INT_START..UCHAR_INT_LIMIT-1
     * or UCHAR_MASK_START..UCHAR_MASK_LIMIT-1.
     *
     * @param value a value in the range u_getIntPropertyMinValue(prop)..
     * u_getIntPropertyMaxValue(prop), with one exception.  If prop is
     * UCHAR_GENERAL_CATEGORY_MASK, then value should not be a UCharCategory, but
     * rather a mask value produced by U_GET_GC_MASK().  This allows grouped
     * categories such as [:L:] to be represented.
     *
     * @param ec error code input/output parameter
     *
     * @return a reference to this set
     *
     * @stable ICU 2.4
     */
    Unicode_سیٹ& apply_Int_Property_قدر(UProperty prop,
                                      int32_t قدر_2,
                                      UError_رمز& ec) U_LIFETIME_BOUND;

    /**
     * Modifies this set to contain those code points which have the
     * given value for the given property.  Prior contents of this
     * set are lost.
     * A frozen set will not be modified.
     *
     * @param prop a property alias, either short or long.  The name is matched
     * loosely.  See PropertyAliases.txt for names and a description of loose
     * matching.  If the value string is empty, then this string is interpreted
     * as either a General_Category value alias, a Script value alias, a binary
     * property alias, or a special ID.  Special IDs are matched loosely and
     * correspond to the following sets:
     *
     * "ANY" = [\\u0000-\\U0010FFFF],
     * "ASCII" = [\\u0000-\\u007F],
     * "Assigned" = [:^Cn:].
     *
     * @param value a value alias, either short or long.  The name is matched
     * loosely.  See PropertyValueAliases.txt for names and a description of
     * loose matching.  In addition to aliases listed, numeric values and
     * canonical combining classes may be expressed numerically, e.g., ("nv",
     * "0.5") or ("ccc", "220").  The value string may also be empty.
     *
     * @param ec error code input/output parameter
     *
     * @return a reference to this set
     *
     * @stable ICU 2.4
     */
    Unicode_سیٹ& applyPropertyAlias(ثابت UnicodeString& prop,
                                   ثابت UnicodeString& قدر_2,
                                   UError_رمز& ec) U_LIFETIME_BOUND;

    /**
     * Returns the number of elements in this set (its cardinality).
     * Note than the elements of a set may include both individual
     * codepoints and strings.
     *
     * This is slower than getRangeCount() because
     * it counts the code points of all ranges.
     *
     * @return the number of elements in this set (its cardinality).
     * @stable ICU 2.0
     * @see getRangeCount
     */
    int32_t سائز_2() ثابت;

    /**
     * Returns <tt>true</tt> if this set contains no elements.
     *
     * @return <tt>true</tt> if this set contains no elements.
     * @stable ICU 2.0
     */
    UBool is_خالی_خانے() ثابت;

    /**
     * @return true if this set contains multi-character strings or the empty string.
     * @stable ICU 70
     */
    UBool has_لڑیاں() ثابت;

    /**
     * Returns true if this set contains the given character.
     * This function works faster with a frozen set.
     * @param c character to be checked for containment
     * @return true if the test condition is met
     * @stable ICU 2.0
     */
    مجازی UBool شامل_ہے(UChar32 c) ثابت override;

    /**
     * Returns true if this set contains every character
     * of the given range.
     * @param start first character, inclusive, of the range
     * @param end last character, inclusive, of the range
     * @return true if the test condition is met
     * @stable ICU 2.0
     */
    UBool شامل_ہے(UChar32 آغاز, UChar32 انتہا) ثابت;

    /**
     * Returns <tt>true</tt> if this set contains the given
     * multicharacter string.
     * @param s string to be checked for containment
     * @return <tt>true</tt> if this set contains the specified string
     * @stable ICU 2.4
     */
    UBool شامل_ہے(ثابت UnicodeString& s) ثابت;

    /**
     * Returns true if this set contains all the characters and strings
     * of the given set.
     * @param c set to be checked for containment
     * @return true if the test condition is met
     * @stable ICU 2.4
     */
    UBool شامل_ہے_All(ثابت Unicode_سیٹ& c) ثابت;

    /**
     * Returns true if this set contains all the characters
     * of the given string.
     * @param s string containing characters to be checked for containment
     * @return true if the test condition is met
     * @stable ICU 2.4
     */
    UBool شامل_ہے_All(ثابت UnicodeString& s) ثابت;

    /**
     * Returns true if this set contains none of the characters
     * of the given range.
     * @param start first character, inclusive, of the range
     * @param end last character, inclusive, of the range
     * @return true if the test condition is met
     * @stable ICU 2.4
     */
    UBool شامل_ہے_None(UChar32 آغاز, UChar32 انتہا) ثابت;

    /**
     * Returns true if this set contains none of the characters and strings
     * of the given set.
     * @param c set to be checked for containment
     * @return true if the test condition is met
     * @stable ICU 2.4
     */
    UBool شامل_ہے_None(ثابت Unicode_سیٹ& c) ثابت;

    /**
     * Returns true if this set contains none of the characters
     * of the given string.
     * @param s string containing characters to be checked for containment
     * @return true if the test condition is met
     * @stable ICU 2.4
     */
    UBool شامل_ہے_None(ثابت UnicodeString& s) ثابت;

    /**
     * Returns true if this set contains one or more of the characters
     * in the given range.
     * @param start first character, inclusive, of the range
     * @param end last character, inclusive, of the range
     * @return true if the condition is met
     * @stable ICU 2.4
     */
    اندرونی UBool شامل_ہے_Some(UChar32 آغاز, UChar32 انتہا) ثابت;

    /**
     * Returns true if this set contains one or more of the characters
     * and strings of the given set.
     * @param s The set to be checked for containment
     * @return true if the condition is met
     * @stable ICU 2.4
     */
    اندرونی UBool شامل_ہے_Some(ثابت Unicode_سیٹ& s) ثابت;

    /**
     * Returns true if this set contains one or more of the characters
     * of the given string.
     * @param s string containing characters to be checked for containment
     * @return true if the condition is met
     * @stable ICU 2.4
     */
    اندرونی UBool شامل_ہے_Some(ثابت UnicodeString& s) ثابت;

    /**
     * Returns the length of the initial substring of the input string which
     * consists only of characters and strings that are contained in this set
     * (USET_SPAN_CONTAINED, USET_SPAN_SIMPLE),
     * or only of characters and strings that are not contained
     * in this set (USET_SPAN_NOT_CONTAINED).
     * See USetSpanCondition for details.
     * Similar to the strspn() C library function.
     * Unpaired surrogates are treated according to contains() of their surrogate code points.
     * This function works faster with a frozen set and with a non-negative string length argument.
     * @param s start of the string
     * @param length of the string; can be -1 for NUL-terminated
     * @param spanCondition specifies the containment condition
     * @return the length of the initial substring according to the spanCondition;
     *         0 if the start of the string does not fit the spanCondition
     * @stable ICU 3.8
     * @see USetSpanCondition
     */
    int32_t وسعت(ثابت char16_t *s, int32_t لمبائی, USet_وسعت_Condition وسعت_Condition) ثابت;

    /**
     * Returns the end of the substring of the input string according to the USetSpanCondition.
     * Same as <code>start+span(s.getBuffer()+start, s.length()-start, spanCondition)</code>
     * after pinning start to 0<=start<=s.length().
     * @param s the string
     * @param start the start index in the string for the span operation
     * @param spanCondition specifies the containment condition
     * @return the exclusive end of the substring according to the spanCondition;
     *         the substring s.tempSubStringBetween(start, end) fulfills the spanCondition
     * @stable ICU 4.4
     * @see USetSpanCondition
     */
    اندرونی int32_t وسعت(ثابت UnicodeString &s, int32_t آغاز, USet_وسعت_Condition وسعت_Condition) ثابت;

    /**
     * Returns the start of the trailing substring of the input string which
     * consists only of characters and strings that are contained in this set
     * (USET_SPAN_CONTAINED, USET_SPAN_SIMPLE),
     * or only of characters and strings that are not contained
     * in this set (USET_SPAN_NOT_CONTAINED).
     * See USetSpanCondition for details.
     * Unpaired surrogates are treated according to contains() of their surrogate code points.
     * This function works faster with a frozen set and with a non-negative string length argument.
     * @param s start of the string
     * @param length of the string; can be -1 for NUL-terminated
     * @param spanCondition specifies the containment condition
     * @return the start of the trailing substring according to the spanCondition;
     *         the string length if the end of the string does not fit the spanCondition
     * @stable ICU 3.8
     * @see USetSpanCondition
     */
    int32_t وسعت_عقب(ثابت char16_t *s, int32_t لمبائی, USet_وسعت_Condition وسعت_Condition) ثابت;

    /**
     * Returns the start of the substring of the input string according to the USetSpanCondition.
     * Same as <code>spanBack(s.getBuffer(), limit, spanCondition)</code>
     * after pinning limit to 0<=end<=s.length().
     * @param s the string
     * @param limit the exclusive-end index in the string for the span operation
     *              (use s.length() or INT32_MAX for spanning back from the end of the string)
     * @param spanCondition specifies the containment condition
     * @return the start of the substring according to the spanCondition;
     *         the substring s.tempSubStringBetween(start, limit) fulfills the spanCondition
     * @stable ICU 4.4
     * @see USetSpanCondition
     */
    اندرونی int32_t وسعت_عقب(ثابت UnicodeString &s, int32_t حد_بندی, USet_وسعت_Condition وسعت_Condition) ثابت;

    /**
     * Returns the length of the initial substring of the input string which
     * consists only of characters and strings that are contained in this set
     * (USET_SPAN_CONTAINED, USET_SPAN_SIMPLE),
     * or only of characters and strings that are not contained
     * in this set (USET_SPAN_NOT_CONTAINED).
     * See USetSpanCondition for details.
     * Similar to the strspn() C library function.
     * Malformed byte sequences are treated according to contains(0xfffd).
     * This function works faster with a frozen set and with a non-negative string length argument.
     * @param s start of the string (UTF-8)
     * @param length of the string; can be -1 for NUL-terminated
     * @param spanCondition specifies the containment condition
     * @return the length of the initial substring according to the spanCondition;
     *         0 if the start of the string does not fit the spanCondition
     * @stable ICU 3.8
     * @see USetSpanCondition
     */
    int32_t وسعت_UTF8(ثابت حرف *s, int32_t لمبائی, USet_وسعت_Condition وسعت_Condition) ثابت;

    /**
     * Returns the start of the trailing substring of the input string which
     * consists only of characters and strings that are contained in this set
     * (USET_SPAN_CONTAINED, USET_SPAN_SIMPLE),
     * or only of characters and strings that are not contained
     * in this set (USET_SPAN_NOT_CONTAINED).
     * See USetSpanCondition for details.
     * Malformed byte sequences are treated according to contains(0xfffd).
     * This function works faster with a frozen set and with a non-negative string length argument.
     * @param s start of the string (UTF-8)
     * @param length of the string; can be -1 for NUL-terminated
     * @param spanCondition specifies the containment condition
     * @return the start of the trailing substring according to the spanCondition;
     *         the string length if the end of the string does not fit the spanCondition
     * @stable ICU 3.8
     * @see USetSpanCondition
     */
    int32_t وسعت_عقب_UTF8(ثابت حرف *s, int32_t لمبائی, USet_وسعت_Condition وسعت_Condition) ثابت;

    /**
     * Implement UnicodeMatcher::matches()
     * @stable ICU 2.4
     */
    UMatchDegree matches(ثابت Replaceable& متن,
                         int32_t& انحراف,
                         int32_t حد_بندی,
                         UBool incremental) override;

نجی:
    /**
     * Returns the longest match for s in text at the given position.
     * If limit > start then match forward from start+1 to limit
     * matching all characters except s.charAt(0).  If limit < start,
     * go backward starting from start-1 matching all characters
     * except s.charAt(s.length()-1).  This method assumes that the
     * first character, text.charAt(start), matches s, so it does not
     * check it.
     * @param text the text to match
     * @param start the first character to match.  In the forward
     * direction, text.charAt(start) is matched against s.charAt(0).
     * In the reverse direction, it is matched against
     * s.charAt(s.length()-1).
     * @param limit the limit offset for matching, either last+1 in
     * the forward direction, or last-1 in the reverse direction,
     * where last is the index of the last character to match.
     * @param s
     * @return If part of s matches up to the limit, return |limit -
     * start|.  If all of s matches before reaching the limit, return
     * s.length().  If there is a mismatch between s and text, return
     * 0
     */
    ساکن int32_t مطابقت_Rest(ثابت Replaceable& متن,
                             int32_t آغاز, int32_t حد_بندی,
                             ثابت UnicodeString& s);

    /**
     * Returns the smallest value i such that c < list[i].  Caller
     * must ensure that c is a legal value or this method will enter
     * an infinite loop.  This method performs a binary search.
     * @param c a character in the range MIN_VALUE..MAX_VALUE
     * inclusive
     * @return the smallest integer i in the range 0..len-1,
     * inclusive, such that c < list[i]
     */
    int32_t ڈھونڈو_رمز_نقطہ(UChar32 c) ثابت;

عوامی:

    /**
     * Implementation of UnicodeMatcher API.  Union the set of all
     * characters that may be matched by this object into the given
     * set.
     * @param toUnionTo the set into which to union the source characters
     * @stable ICU 2.4
     */
    مجازی خالی جوڑو_مطابقت_سیٹ_مقصد(Unicode_سیٹ& مقصد_Union_مقصد) ثابت override;

    /**
     * Returns the index of the given character within this set, where
     * the set is ordered by ascending code point.  If the character
     * is not in this set, return -1.  The inverse of this method is
     * <code>charAt()</code>.
     * @return an index from 0..size()-1, or -1
     * @stable ICU 2.4
     */
    int32_t اشاریہ_کا(UChar32 c) ثابت;

    /**
     * Returns the character at the given index within this set, where
     * the set is ordered by ascending code point.  If the index is
     * out of range for characters, returns (UChar32)-1.
     * The inverse of this method is <code>indexOf()</code>.
     *
     * For iteration, this is slower than UnicodeSetIterator or
     * getRangeCount()/getRangeStart()/getRangeEnd(),
     * because for each call it skips linearly over <code>index</code>
     * characters in the ranges.
     *
     * @param index an index from 0..size()-1
     * @return the character at the given index, or (UChar32)-1.
     * @stable ICU 2.4
     */
    UChar32 charAt(int32_t اشاریہ) ثابت;

    /**
     * Returns a C++ "range" for iterating over the code points of this set.
     *
     * \code
     * UnicodeSet set(u"[abcçカ🚴]", errorCode);
     * for (UChar32 c : set.codePoints()) {
     *     printf("set.codePoint U+%04lx\n", (long)c);
     * }
     * \endcode
     *
     * @return a "range" object for iterating over the code points of this set.
     * @stable ICU 76
     * @see ranges
     * @see strings
     * @see begin
     * @see end
     */
    اندرونی U_سرخی_NESTED_NAMESPACE::USet_رمز_Points رمز_Points() ثابت U_LIFETIME_BOUND {
        واپس U_سرخی_NESTED_NAMESPACE::USet_رمز_Points(مقصد_USet());
    }

    /**
     * Returns a C++ "range" for iterating over the code point ranges of this set.
     *
     * \code
     * UnicodeSet set(u"[abcçカ🚴]", errorCode);
     * for (auto [start, end] : set.ranges()) {
     *     printf("set.range U+%04lx..U+%04lx\n", (long)start, (long)end);
     * }
     * for (auto range : set.ranges()) {
     *     for (UChar32 c : range) {
     *         printf("set.range.c U+%04lx\n", (long)c);
     *     }
     * }
     * \endcode
     *
     * @return a "range" object for iterating over the code point ranges of this set.
     * @stable ICU 76
     * @see codePoints
     * @see strings
     * @see begin
     * @see end
     */
    اندرونی U_سرخی_NESTED_NAMESPACE::USetRanges ranges() ثابت U_LIFETIME_BOUND {
        واپس U_سرخی_NESTED_NAMESPACE::USetRanges(مقصد_USet());
    }

    /**
     * Returns a C++ "range" for iterating over the empty and multi-character strings of this set.
     * Returns each string as a std::u16string_view without copying its contents.
     *
     * \code
     * UnicodeSet set(u"[abcçカ🚴{}{abc}{de}]", errorCode);
     * for (auto s : set.strings()) {
     *     UnicodeString us(s);
     *     std::string u8;
     *     printf("set.string length %ld \"%s\"\n", (long)s.length(), us.toUTF8String(u8).c_str());
     * }
     * \endcode
     *
     * @return a "range" object for iterating over the strings of this set.
     * @stable ICU 76
     * @see codePoints
     * @see ranges
     * @see begin
     * @see end
     */
    اندرونی U_سرخی_NESTED_NAMESPACE::USet_لڑیاں لڑیاں() ثابت U_LIFETIME_BOUND {
        واپس U_سرخی_NESTED_NAMESPACE::USet_لڑیاں(مقصد_USet());
    }

    /**
     * Returns a C++ iterator for iterating over all of the elements of this set.
     * Convenient all-in one iteration, but creates a std::u16string for each
     * code point or string.
     * (Similar to how Java UnicodeSet *is an* Iterable&lt;String&gt;.)
     *
     * Code points are returned first, then empty and multi-character strings.
     *
     * \code
     * UnicodeSet set(u"[abcçカ🚴{}{abc}{de}]", errorCode);
     * for (auto el : set) {
     *     UnicodeString us(el);
     *     std::string u8;
     *     printf("set.element length %ld \"%s\"\n", (long)us.length(), us.toUTF8String(u8).c_str());
     * }
     * \endcode
     *
     * @return an all-elements iterator.
     * @stable ICU 77
     * @see end
     * @see codePoints
     * @see ranges
     * @see strings
     */
    اندرونی U_سرخی_NESTED_NAMESPACE::USetElementIterator begin() ثابت U_LIFETIME_BOUND {
        واپس U_سرخی_NESTED_NAMESPACE::USetElements(مقصد_USet()).begin();
    }

    /**
     * @return an exclusive-end sentinel for iterating over all of the elements of this set.
     * @stable ICU 77
     * @see begin
     * @see codePoints
     * @see ranges
     * @see strings
     */
    اندرونی U_سرخی_NESTED_NAMESPACE::USetElementIterator انتہا() ثابت U_LIFETIME_BOUND {
        واپس U_سرخی_NESTED_NAMESPACE::USetElements(مقصد_USet()).انتہا();
    }

    /**
     * Adds the specified range to this set if it is not already
     * present.  If this set already contains the specified range,
     * the call leaves this set unchanged.  If <code>start > end</code>
     * then an empty range is added, leaving the set unchanged.
     * This is equivalent to a boolean logic OR, or a set UNION.
     * A frozen set will not be modified.
     *
     * @param start first character, inclusive, of range to be added
     * to this set.
     * @param end last character, inclusive, of range to be added
     * to this set.
     * @stable ICU 2.0
     */
    Unicode_سیٹ& جوڑو(UChar32 آغاز, UChar32 انتہا) U_LIFETIME_BOUND;

    /**
     * Adds the specified character to this set if it is not already
     * present.  If this set already contains the specified character,
     * the call leaves this set unchanged.
     * A frozen set will not be modified.
     *
     * @param c the character (code point)
     * @return this object, for chaining
     * @stable ICU 2.0
     */
    Unicode_سیٹ& جوڑو(UChar32 c) U_LIFETIME_BOUND;

    /**
     * Adds the specified multicharacter to this set if it is not already
     * present.  If this set already contains the multicharacter,
     * the call leaves this set unchanged.
     * Thus "ch" => {"ch"}
     * A frozen set will not be modified.
     *
     * @param s the source string
     * @return this object, for chaining
     * @stable ICU 2.4
     */
    Unicode_سیٹ& جوڑو(ثابت UnicodeString& s) U_LIFETIME_BOUND;

 نجی:
    /**
     * @return a code point IF the string consists of a single one.
     * otherwise returns -1.
     * @param s string to test
     */
    ساکن int32_t getSingleCP(ثابت UnicodeString& s);

    خالی جوڑو_2(ثابت UnicodeString& s);

 عوامی:
    /**
     * Adds each of the characters in this string to the set. Note: "ch" => {"c", "h"}
     * If this set already contains any particular character, it has no effect on that character.
     * A frozen set will not be modified.
     * @param s the source string
     * @return this object, for chaining
     * @stable ICU 2.4
     */
    Unicode_سیٹ& جوڑو_All(ثابت UnicodeString& s) U_LIFETIME_BOUND;

    /**
     * Retains EACH of the characters in this string. Note: "ch" == {"c", "h"}
     * A frozen set will not be modified.
     * @param s the source string
     * @return this object, for chaining
     * @stable ICU 2.4
     */
    Unicode_سیٹ& retainAll(ثابت UnicodeString& s) U_LIFETIME_BOUND;

    /**
     * Complement EACH of the characters in this string. Note: "ch" == {"c", "h"}
     * A frozen set will not be modified.
     * @param s the source string
     * @return this object, for chaining
     * @stable ICU 2.4
     */
    Unicode_سیٹ& complementAll(ثابت UnicodeString& s) U_LIFETIME_BOUND;

    /**
     * Remove EACH of the characters in this string. Note: "ch" == {"c", "h"}
     * A frozen set will not be modified.
     * @param s the source string
     * @return this object, for chaining
     * @stable ICU 2.4
     */
    Unicode_سیٹ& removeAll(ثابت UnicodeString& s) U_LIFETIME_BOUND;

    /**
     * Makes a set from a multicharacter string. Thus "ch" => {"ch"}
     *
     * @param s the source string
     * @return a newly created set containing the given string.
     * The caller owns the return object and is responsible for deleting it.
     * @stable ICU 2.4
     */
    ساکن Unicode_سیٹ* U_EXPORT2 createFrom(ثابت UnicodeString& s);


    /**
     * Makes a set from each of the characters in the string. Thus "ch" => {"c", "h"}
     * @param s the source string
     * @return a newly created set containing the given characters
     * The caller owns the return object and is responsible for deleting it.
     * @stable ICU 2.4
     */
    ساکن Unicode_سیٹ* U_EXPORT2 createFromAll(ثابت UnicodeString& s);

    /**
     * Retain only the elements in this set that are contained in the
     * specified range.  If <code>start > end</code> then an empty range is
     * retained, leaving the set empty.  This is equivalent to
     * a boolean logic AND, or a set INTERSECTION.
     * A frozen set will not be modified.
     *
     * @param start first character, inclusive, of range
     * @param end last character, inclusive, of range
     * @stable ICU 2.0
     */
    Unicode_سیٹ& retain(UChar32 آغاز, UChar32 انتہا) U_LIFETIME_BOUND;


    /**
     * Retain the specified character from this set if it is present.
     * A frozen set will not be modified.
     *
     * @param c the character (code point)
     * @return this object, for chaining
     * @stable ICU 2.0
     */
    Unicode_سیٹ& retain(UChar32 c) U_LIFETIME_BOUND;

    /**
     * Retains only the specified string from this set if it is present.
     * Upon return this set will be empty if it did not contain s, or
     * will only contain s if it did contain s.
     * A frozen set will not be modified.
     *
     * @param s the source string
     * @return this object, for chaining
     * @stable ICU 69
     */
    Unicode_سیٹ& retain(ثابت UnicodeString& s) U_LIFETIME_BOUND;

    /**
     * Removes the specified range from this set if it is present.
     * The set will not contain the specified range once the call
     * returns.  If <code>start > end</code> then an empty range is
     * removed, leaving the set unchanged.
     * A frozen set will not be modified.
     *
     * @param start first character, inclusive, of range to be removed
     * from this set.
     * @param end last character, inclusive, of range to be removed
     * from this set.
     * @stable ICU 2.0
     */
    Unicode_سیٹ& remove(UChar32 آغاز, UChar32 انتہا) U_LIFETIME_BOUND;

    /**
     * Removes the specified character from this set if it is present.
     * The set will not contain the specified range once the call
     * returns.
     * A frozen set will not be modified.
     *
     * @param c the character (code point)
     * @return this object, for chaining
     * @stable ICU 2.0
     */
    Unicode_سیٹ& remove(UChar32 c) U_LIFETIME_BOUND;

    /**
     * Removes the specified string from this set if it is present.
     * The set will not contain the specified character once the call
     * returns.
     * A frozen set will not be modified.
     * @param s the source string
     * @return this object, for chaining
     * @stable ICU 2.4
     */
    Unicode_سیٹ& remove(ثابت UnicodeString& s) U_LIFETIME_BOUND;

    /**
     * This is equivalent to
     * <code>complement(MIN_VALUE, MAX_VALUE)</code>.
     *
     * <strong>Note:</strong> This performs a symmetric difference with all code points
     * <em>and thus retains all multicharacter strings</em>.
     * In order to achieve a “code point complement” (all code points minus this set),
     * the easiest is to <code>.complement().removeAllStrings()</code>.
     *
     * A frozen set will not be modified.
     * @stable ICU 2.0
     */
    Unicode_سیٹ& complement() U_LIFETIME_BOUND;

    /**
     * Complements the specified range in this set.  Any character in
     * the range will be removed if it is in this set, or will be
     * added if it is not in this set.  If <code>start > end</code>
     * then an empty range is complemented, leaving the set unchanged.
     * This is equivalent to a boolean logic XOR.
     * A frozen set will not be modified.
     *
     * @param start first character, inclusive, of range
     * @param end last character, inclusive, of range
     * @stable ICU 2.0
     */
    Unicode_سیٹ& complement(UChar32 آغاز, UChar32 انتہا) U_LIFETIME_BOUND;

    /**
     * Complements the specified character in this set.  The character
     * will be removed if it is in this set, or will be added if it is
     * not in this set.
     * A frozen set will not be modified.
     *
     * @param c the character (code point)
     * @return this object, for chaining
     * @stable ICU 2.0
     */
    Unicode_سیٹ& complement(UChar32 c) U_LIFETIME_BOUND;

    /**
     * Complement the specified string in this set.
     * The string will be removed if it is in this set, or will be added if it is not in this set.
     * A frozen set will not be modified.
     *
     * @param s the string to complement
     * @return this object, for chaining
     * @stable ICU 2.4
     */
    Unicode_سیٹ& complement(ثابت UnicodeString& s) U_LIFETIME_BOUND;

    /**
     * Adds all of the elements in the specified set to this set if
     * they're not already present.  This operation effectively
     * modifies this set so that its value is the <i>union</i> of the two
     * sets.  The behavior of this operation is unspecified if the specified
     * collection is modified while the operation is in progress.
     * A frozen set will not be modified.
     *
     * @param c set whose elements are to be added to this set.
     * @see #add(UChar32, UChar32)
     * @stable ICU 2.0
     */
    Unicode_سیٹ& جوڑو_All(ثابت Unicode_سیٹ& c) U_LIFETIME_BOUND;

    /**
     * Retains only the elements in this set that are contained in the
     * specified set.  In other words, removes from this set all of
     * its elements that are not contained in the specified set.  This
     * operation effectively modifies this set so that its value is
     * the <i>intersection</i> of the two sets.
     * A frozen set will not be modified.
     *
     * @param c set that defines which elements this set will retain.
     * @stable ICU 2.0
     */
    Unicode_سیٹ& retainAll(ثابت Unicode_سیٹ& c) U_LIFETIME_BOUND;

    /**
     * Removes from this set all of its elements that are contained in the
     * specified set.  This operation effectively modifies this
     * set so that its value is the <i>asymmetric set difference</i> of
     * the two sets.
     * A frozen set will not be modified.
     *
     * @param c set that defines which elements will be removed from
     *          this set.
     * @stable ICU 2.0
     */
    Unicode_سیٹ& removeAll(ثابت Unicode_سیٹ& c) U_LIFETIME_BOUND;

    /**
     * Complements in this set all elements contained in the specified
     * set.  Any character in the other set will be removed if it is
     * in this set, or will be added if it is not in this set.
     * A frozen set will not be modified.
     *
     * @param c set that defines which elements will be xor'ed from
     *          this set.
     * @stable ICU 2.4
     */
    Unicode_سیٹ& complementAll(ثابت Unicode_سیٹ& c) U_LIFETIME_BOUND;

    /**
     * Removes all of the elements from this set.  This set will be
     * empty after this call returns.
     * A frozen set will not be modified.
     * @stable ICU 2.0
     */
    Unicode_سیٹ& clear() U_LIFETIME_BOUND;

    /**
     * Close this set over the given attribute.  For the attribute
     * USET_CASE_INSENSITIVE, the result is to modify this set so that:
     *
     * 1. For each character or string 'a' in this set, all strings or
     * characters 'b' such that foldCase(a) == foldCase(b) are added
     * to this set.
     *
     * 2. For each string 'e' in the resulting set, if e !=
     * foldCase(e), 'e' will be removed.
     *
     * Example: [aq\\u00DF{Bc}{bC}{Fi}] => [aAqQ\\u00DF\\uFB01{ss}{bc}{fi}]
     *
     * (Here foldCase(x) refers to the operation u_strFoldCase, and a
     * == b denotes that the contents are the same, not pointer
     * comparison.)
     *
     * A frozen set will not be modified.
     *
     * @param attribute bitmask for attributes to close over.
     * Valid options:
     * At most one of USET_CASE_INSENSITIVE, USET_ADD_CASE_MAPPINGS, USET_SIMPLE_CASE_INSENSITIVE.
     * These case options are mutually exclusive.
     * Unrelated options bits are ignored.
     * @return a reference to this set.
     * @stable ICU 4.2
     */
    Unicode_سیٹ& closeOver(int32_t attribute) U_LIFETIME_BOUND;

    /**
     * Remove all strings from this set.
     *
     * @return a reference to this set.
     * @stable ICU 4.2
     */
    Unicode_سیٹ& remove_All_لڑیاں() U_LIFETIME_BOUND;

    /**
     * Iteration method that returns the number of ranges contained in
     * this set.
     * @see #getRangeStart
     * @see #getRangeEnd
     * @stable ICU 2.4
     */
    int32_t get_Range_گنتی() ثابت;

    /**
     * Iteration method that returns the first character in the
     * specified range of this set.
     * @see #getRangeCount
     * @see #getRangeEnd
     * @stable ICU 2.4
     */
    UChar32 get_Range_آغاز(int32_t اشاریہ) ثابت;

    /**
     * Iteration method that returns the last character in the
     * specified range of this set.
     * @see #getRangeStart
     * @see #getRangeEnd
     * @stable ICU 2.4
     */
    UChar32 get_Range_انتہا(int32_t اشاریہ) ثابت;

    /**
     * Serializes this set into an array of 16-bit integers.  Serialization
     * (currently) only records the characters in the set; multicharacter
     * strings are ignored.
     *
     * The array has following format (each line is one 16-bit
     * integer):
     *
     *  length     = (n+2*m) | (m!=0?0x8000:0)
     *  bmpLength  = n; present if m!=0
     *  bmp[0]
     *  bmp[1]
     *  ...
     *  bmp[n-1]
     *  supp-high[0]
     *  supp-low[0]
     *  supp-high[1]
     *  supp-low[1]
     *  ...
     *  supp-high[m-1]
     *  supp-low[m-1]
     *
     * The array starts with a header.  After the header are n bmp
     * code points, then m supplementary code points.  Either n or m
     * or both may be zero.  n+2*m is always <= 0x7FFF.
     *
     * If there are no supplementary characters (if m==0) then the
     * header is one 16-bit integer, 'length', with value n.
     *
     * If there are supplementary characters (if m!=0) then the header
     * is two 16-bit integers.  The first, 'length', has value
     * (n+2*m)|0x8000.  The second, 'bmpLength', has value n.
     *
     * After the header the code points are stored in ascending order.
     * Supplementary code points are stored as most significant 16
     * bits followed by least significant 16 bits.
     *
     * @param dest pointer to buffer of destCapacity 16-bit integers.
     * May be nullptr only if destCapacity is zero.
     * @param destCapacity size of dest, or zero.  Must not be negative.
     * @param ec error code.  Will be set to U_INDEX_OUTOFBOUNDS_ERROR
     * if n+2*m > 0x7FFF.  Will be set to U_BUFFER_OVERFLOW_ERROR if
     * n+2*m+(m!=0?2:1) > destCapacity.
     * @return the total length of the serialized format, including
     * the header, that is, n+2*m+(m!=0?2:1), or 0 on error other
     * than U_BUFFER_OVERFLOW_ERROR.
     * @stable ICU 2.4
     */
    int32_t serialize(uint16_t *dest, int32_t dest_استعداد, UError_رمز& ec) ثابت;

    /**
     * Reallocate this objects internal structures to take up the least
     * possible space, without changing this object's value.
     * A frozen set will not be modified.
     * @stable ICU 2.4
     */
    Unicode_سیٹ& compact() U_LIFETIME_BOUND;

    /**
     * Return the class ID for this class.  This is useful only for
     * comparing to a return value from getDynamicClassID().  For example:
     * <pre>
     * .      Base* polymorphic_pointer = createPolymorphicObject();
     * .      if (polymorphic_pointer->getDynamicClassID() ==
     * .          Derived::getStaticClassID()) ...
     * </pre>
     * @return          The class ID for all objects of this class.
     * @stable ICU 2.0
     */
    ساکن UClassID U_EXPORT2 getStaticClassID();

    /**
     * Implement UnicodeFunctor API.
     *
     * @return The class ID for this object. All objects of a given
     * class have the same class ID.  Objects of other classes have
     * different class IDs.
     * @stable ICU 2.4
     */
    مجازی UClassID getDynamicClassID() ثابت override;

  نجی:

    // Private API for the USet API

    دوست class USetAccess;

    ثابت UnicodeString* getString(int32_t اشاریہ) ثابت U_LIFETIME_BOUND;

    //----------------------------------------------------------------
    // RuleBasedTransliterator support
    //----------------------------------------------------------------

نجی:

    /**
     * Returns <tt>true</tt> if this set contains any character whose low byte
     * is the given value.  This is used by <tt>RuleBasedTransliterator</tt> for
     * indexing.
     */
    مجازی UBool matches_اشاریہ_قدر(uint8_t v) ثابت override;

نجی:
    دوست class RBBIRuleScanner;

    //----------------------------------------------------------------
    // Implementation: Clone as thawed (see ICU4J Freezable)
    //----------------------------------------------------------------

    Unicode_سیٹ(ثابت Unicode_سیٹ& o, UBool /* asThawed */);
    Unicode_سیٹ& copyFrom(ثابت Unicode_سیٹ& o, UBool asThawed) U_LIFETIME_BOUND;

    //----------------------------------------------------------------
    // Implementation: Pattern parsing
    //----------------------------------------------------------------

    خالی apply_نمونہ_Ignore_Space(ثابت UnicodeString& نمونہ,
                                 ParsePosition& pos,
                                 ثابت علامت_جدول* symbols,
                                 UError_رمز& حالت_2);

    خالی apply_نمونہ(ثابت UnicodeString &نمونہ,
                      ثابت ParsePosition& parsePosition,
                      قاعدہ_Character_Iterator &chars,
                      ثابت علامت_جدول *symbols,
                      UnicodeString &rebuiltPat,
                      uint32_t options,
                      Unicode_سیٹ &(Unicode_سیٹ::*case_احاطہ)(int32_t attribute),
                      UError_رمز &ec);

    // Recursive-descent predictive parsing.  These functions parse the syntactic categories
    // matching their name in the base grammar of PD UTR #56 (before the highlighted changes are
    // applied).
    // See https://www.unicode.org/reports/tr61/tr61-1.html#Set-Operations.
    // `parseUnicodeSet` clears `*this` and makes it represent the parsed UnicodeSet; all other functions
    // add the set represented by the parsed construct to `*this`.

    class Lexer;

    ساکن Unicode_سیٹ parse_Unicode_سیٹ(Lexer &lexer,
                                      UnicodeString &rebuiltPat,
                                      uint32_t options,
                                      Unicode_سیٹ &(Unicode_سیٹ::*case_احاطہ)(int32_t attribute),
                                      int32_t گہرائی,
                                      UError_رمز &ec);

    ساکن Unicode_سیٹ parseContent(Lexer &lexer,
                                   UnicodeString &rebuiltPat,
                                   uint32_t options,
                                   Unicode_سیٹ &(Unicode_سیٹ::*case_احاطہ)(int32_t attribute),
                                   int32_t گہرائی,
                                   bool &شامل_ہے_سیٹ_Operation,
                                   UError_رمز &ec);

    خالی parseMutation(Lexer &lexer,
                       UnicodeString &rebuiltPat,
                       uint32_t options,
                       Unicode_سیٹ &(Unicode_سیٹ::*case_احاطہ)(int32_t attribute),
                       int32_t گہرائی,
                       bool &شامل_ہے_سیٹ_Operation,
                       UError_رمز &ec);

    خالی parse_سیٹ_Operations(Lexer &lexer,
                            UnicodeString &rebuiltPat,
                            uint32_t options,
                            Unicode_سیٹ &(Unicode_سیٹ::*case_احاطہ)(int32_t attribute),
                            int32_t گہرائی,
                            UError_رمز &ec);

    ساکن Unicode_سیٹ parseElements(Lexer &lexer,
                                    UnicodeString &rebuiltPat,
                                    UError_رمز &ec);


    خالی closeOverCaseInsensitive(bool simple);
    خالی close_Over_جوڑو_Case_Mappings();

    //----------------------------------------------------------------
    // Implementation: Utility methods
    //----------------------------------------------------------------

    ساکن int32_t اگلا_استعداد(int32_t min_استعداد);

    bool ensure_استعداد(int32_t newLen);

    bool ensure_بفر_استعداد(int32_t newLen);

    خالی تبادلہ_Buffers();

    UBool allocate_لڑیاں(UError_رمز &حالت_2);
    int32_t لڑیاں_سائز() ثابت;
    UBool لڑیاں_شامل_ہے(ثابت UnicodeString &s) ثابت;

    UnicodeString& مقصد_نمونہ_2(UnicodeString& نتیجہ U_LIFETIME_BOUND,
                              UBool escapeUnprintable) ثابت;

    UnicodeString& generate_نمونہ(UnicodeString& نتیجہ U_LIFETIME_BOUND,
                                    UBool escapeUnprintable) ثابت;

    ساکن خالی append_مقصد_Pat(UnicodeString& buf, ثابت UnicodeString& s, UBool escapeUnprintable);

    ساکن خالی append_مقصد_Pat(UnicodeString& buf, UChar32 c, UBool escapeUnprintable);

    ساکن خالی append_مقصد_Pat(UnicodeString &نتیجہ, UChar32 آغاز, UChar32 انتہا,
                             UBool escapeUnprintable);

    //----------------------------------------------------------------
    // Implementation: Fundamental operators
    //----------------------------------------------------------------

    خالی exclusiveOr(ثابت UChar32* other, int32_t otherLen, int8_t polarity);

    خالی جوڑو(ثابت UChar32* other, int32_t otherLen, int8_t polarity);

    خالی retain(ثابت UChar32* other, int32_t otherLen, int8_t polarity);

    /**
     * Return true if the given position, in the given pattern, appears
     * to be the start of a property set pattern [:foo:], \\p{foo}, or
     * \\P{foo}, or \\N{name}.
     */
    ساکن UBool resembles_Property_نمونہ(ثابت UnicodeString& نمونہ,
                                          int32_t pos);

    /**
     * Parse the given property pattern at the given parse position
     * and set this UnicodeSet to the result.
     *
     * The original design document is out of date, but still useful.
     * Ignore the property and value names:
     * https://htmlpreview.github.io/?https://github.com/unicode-org/icu-docs/blob/main/design/unicodeset_properties.html
     *
     * Recognized syntax:
     *
     * [:foo:] [:^foo:] - white space not allowed within "[:" or ":]"
     * \\p{foo} \\P{foo}  - white space not allowed within "\\p" or "\\P"
     * \\N{name}         - white space not allowed within "\\N"
     *
     * Other than the above restrictions, Unicode Pattern_White_Space characters are ignored.
     * Case is ignored except in "\\p" and "\\P" and "\\N".  In 'name' leading
     * and trailing space is deleted, and internal runs of whitespace
     * are collapsed to a single space.
     *
     * We support binary properties, enumerated properties, and the
     * following non-enumerated properties:
     *
     *  Numeric_Value
     *  Name
     *  Unicode_1_Name
     *
     * @param pattern the pattern string
     * @param ppos on entry, the position at which to begin parsing.
     * This should be one of the locations marked '^':
     *
     *   [:blah:]     \\p{blah}     \\P{blah}     \\N{name}
     *   ^       %    ^       %    ^       %    ^       %
     *
     * On return, the position after the last character parsed, that is,
     * the locations marked '%'.  If the parse fails, ppos is returned
     * unchanged.
     * @param ec status
     * @return a reference to this.
     */
    Unicode_سیٹ& apply_Property_نمونہ(ثابت UnicodeString& نمونہ,
                                     ParsePosition& ppos,
                                     UError_رمز& ec) U_LIFETIME_BOUND;

    /**
     * A filter that returns true if the given code point should be
     * included in the UnicodeSet being constructed.
     */
    قسمتعریف UBool (*Filter)(UChar32 رمز_نقطہ, خالی* context);

    /**
     * Given a filter, set this UnicodeSet to the code points
     * contained by that filter.  The filter MUST be
     * property-conformant.  That is, if it returns value v for one
     * code point, then it must return v for all affiliated code
     * points, as defined by the inclusions list.  See
     * getInclusions().
     * src is a UPropertySource value.
     */
    خالی applyFilter(Filter filter,
                     خالی* context,
                     ثابت Unicode_سیٹ* inclusions,
                     UError_رمز &حالت_2);

    /**
     * Set the new pattern to cache.
     */
    خالی سیٹ_نمونہ(ثابت UnicodeString& newPat) {
        سیٹ_نمونہ(newPat.get_بفر(), newPat.لمبائی());
    }
    خالی سیٹ_نمونہ(ثابت char16_t *newPat, int32_t newPatLen);
    /**
     * Release existing cached pattern.
     */
    خالی release_نمونہ();

    دوست class Unicode_سیٹ_Iterator;
};



اندرونی bool Unicode_سیٹ::عامل!=(ثابت Unicode_سیٹ& o) ثابت {
    واپس !عامل==(o);
}

اندرونی UBool Unicode_سیٹ::isFrozen() ثابت {
    واپس bmp_سیٹ != nullptr || string_وسعت != nullptr;
}

اندرونی UBool Unicode_سیٹ::شامل_ہے_Some(UChar32 آغاز, UChar32 انتہا) ثابت {
    واپس !شامل_ہے_None(آغاز, انتہا);
}

اندرونی UBool Unicode_سیٹ::شامل_ہے_Some(ثابت Unicode_سیٹ& s) ثابت {
    واپس !شامل_ہے_None(s);
}

اندرونی UBool Unicode_سیٹ::شامل_ہے_Some(ثابت UnicodeString& s) ثابت {
    واپس !شامل_ہے_None(s);
}

اندرونی UBool Unicode_سیٹ::isBogus() ثابت {
    واپس fFlags & kIsBogus;
}

اندرونی Unicode_سیٹ* Unicode_سیٹ::fromUSet(USet* uset U_LIFETIME_BOUND) {
    واپس reinterpret_cast<Unicode_سیٹ *>(uset);
}

اندرونی ثابت Unicode_سیٹ* Unicode_سیٹ::fromUSet(ثابت USet* uset U_LIFETIME_BOUND) {
    واپس reinterpret_cast<ثابت Unicode_سیٹ *>(uset);
}

اندرونی USet* Unicode_سیٹ::مقصد_USet() U_LIFETIME_BOUND {
    واپس reinterpret_cast<USet*>(this);
}

اندرونی ثابت USet* Unicode_سیٹ::مقصد_USet() ثابت U_LIFETIME_BOUND {
    واپس reinterpret_cast<ثابت USet *>(this);
}

اندرونی int32_t Unicode_سیٹ::وسعت(ثابت UnicodeString &s, int32_t آغاز, USet_وسعت_Condition وسعت_Condition) ثابت {
    int32_t s_لمبائی=s.لمبائی();
    اگر(آغاز<0) {
        آغاز=0;
    } ورنہ اگر(آغاز>s_لمبائی) {
        آغاز=s_لمبائی;
    }
    واپس آغاز+وسعت(s.get_بفر()+آغاز, s_لمبائی-آغاز, وسعت_Condition);
}

اندرونی int32_t Unicode_سیٹ::وسعت_عقب(ثابت UnicodeString &s, int32_t حد_بندی, USet_وسعت_Condition وسعت_Condition) ثابت {
    int32_t s_لمبائی=s.لمبائی();
    اگر(حد_بندی<0) {
        حد_بندی=0;
    } ورنہ اگر(حد_بندی>s_لمبائی) {
        حد_بندی=s_لمبائی;
    }
    واپس وسعت_عقب(s.get_بفر(), حد_بندی, وسعت_Condition);
}

U_NAMESPACE_انتہا

#اگر_ختم /* U_SHOW_CPLUSPLUS_API */

#اگر_ختم
