// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  messagepattern.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011mar14
*   created by: Markus W. Scherer
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __MESSAGEPATTERN_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __MESSAGEPATTERN_H__

/**
 * \file
 * \brief C++ API: MessagePattern class: Parses and represents ICU MessageFormat patterns.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API

#ਜੇ !UCONFIG_NO_FORMATTING

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parseerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"

/**
 * Mode for when an apostrophe starts quoted literal text for MessageFormat output.
 * The default is DOUBLE_OPTIONAL unless overridden via uconfig.h
 * (UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE).
 * <p>
 * A pair of adjacent apostrophes always results in a single apostrophe in the output,
 * even when the pair is between two single, text-quoting apostrophes.
 * <p>
 * The following table shows examples of desired MessageFormat.format() output
 * with the pattern strings that yield that output.
 * <p>
 * <table>
 *   <tr>
 *     <th>Desired output</th>
 *     <th>DOUBLE_OPTIONAL</th>
 *     <th>DOUBLE_REQUIRED</th>
 *   </tr>
 *   <tr>
 *     <td>I see {many}</td>
 *     <td>I see '{many}'</td>
 *     <td>(same)</td>
 *   </tr>
 *   <tr>
 *     <td>I said {'Wow!'}</td>
 *     <td>I said '{''Wow!''}'</td>
 *     <td>(same)</td>
 *   </tr>
 *   <tr>
 *     <td>I don't know</td>
 *     <td>I don't know OR<br> I don''t know</td>
 *     <td>I don''t know</td>
 *   </tr>
 * </table>
 * @stable ICU 4.8
 * @see UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE
 */
ਗਿਣਤੀ UMessage_ਨਮੂਨਾ_Apostrophe_ਬਹੁਲਕ {
    /**
     * A literal apostrophe is represented by
     * either a single or a double apostrophe pattern character.
     * Within a MessageFormat pattern, a single apostrophe only starts quoted literal text
     * if it immediately precedes a curly brace {},
     * or a pipe symbol | if inside a choice format,
     * or a pound symbol # if inside a plural format.
     * <p>
     * This is the default behavior starting with ICU 4.8.
     * @stable ICU 4.8
     */
    UMSGPAT_APOS_DOUBLE_OPTIONAL,
    /**
     * A literal apostrophe must be represented by
     * a double apostrophe pattern character.
     * A single apostrophe always starts quoted literal text.
     * <p>
     * This is the behavior of ICU 4.6 and earlier, and of the JDK.
     * @stable ICU 4.8
     */
    UMSGPAT_APOS_DOUBLE_REQUIRED
};
/**
 * @stable ICU 4.8
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UMessage_ਨਮੂਨਾ_Apostrophe_ਬਹੁਲਕ UMessage_ਨਮੂਨਾ_Apostrophe_ਬਹੁਲਕ;

/**
 * MessagePattern::Part type constants.
 * @stable ICU 4.8
 */
ਗਿਣਤੀ UMessage_ਨਮੂਨਾ_Part_Type {
    /**
     * Start of a message pattern (main or nested).
     * The length is 0 for the top-level message
     * and for a choice argument sub-message, otherwise 1 for the '{'.
     * The value indicates the nesting level, starting with 0 for the main message.
     * <p>
     * There is always a later MSG_LIMIT part.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ਸੁਨੇਹਾ_ਸ਼ੁਰੂ,
    /**
     * End of a message pattern (main or nested).
     * The length is 0 for the top-level message and
     * the last sub-message of a choice argument,
     * otherwise 1 for the '}' or (in a choice argument style) the '|'.
     * The value indicates the nesting level, starting with 0 for the main message.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ਸੁਨੇਹਾ_ਹੱਦਬੰਦੀ,
    /**
     * Indicates a substring of the pattern string which is to be skipped when formatting.
     * For example, an apostrophe that begins or ends quoted text
     * would be indicated with such a part.
     * The value is undefined and currently always 0.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_SKIP_SYNTAX,
    /**
     * Indicates that a syntax character needs to be inserted for auto-quoting.
     * The length is 0.
     * The value is the character code of the insertion character. (U+0027=APOSTROPHE)
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ਪਾਓ_CHAR,
    /**
     * Indicates a syntactic (non-escaped) # symbol in a plural variant.
     * When formatting, replace this part's substring with the
     * (value-offset) for the plural argument value.
     * The value is undefined and currently always 0.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_REPLACE_ਸੰਖਿਆ,
    /**
     * Start of an argument.
     * The length is 1 for the '{'.
     * The value is the ordinal value of the ArgType. Use getArgType().
     * <p>
     * This part is followed by either an ARG_NUMBER or ARG_NAME,
     * followed by optional argument sub-parts (see UMessagePatternArgType constants)
     * and finally an ARG_LIMIT part.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ARG_ਸ਼ੁਰੂ,
    /**
     * End of an argument.
     * The length is 1 for the '}'.
     * The value is the ordinal value of the ArgType. Use getArgType().
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ARG_ਹੱਦਬੰਦੀ,
    /**
     * The argument number, provided by the value.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ARG_ਸੰਖਿਆ,
    /**
     * The argument name.
     * The value is undefined and currently always 0.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ARG_NAME,
    /**
     * The argument type.
     * The value is undefined and currently always 0.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ARG_TYPE,
    /**
     * The argument style text.
     * The value is undefined and currently always 0.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ARG_STYLE,
    /**
     * A selector substring in a "complex" argument style.
     * The value is undefined and currently always 0.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ARG_SELECTOR,
    /**
     * An integer value, for example the offset or an explicit selector value
     * in a PluralFormat style.
     * The part value is the integer value.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ARG_INT,
    /**
     * A numeric value, for example the offset or an explicit selector value
     * in a PluralFormat style.
     * The part value is an index into an internal array of numeric values;
     * use getNumericValue().
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ARG_DOUBLE
};
/**
 * @stable ICU 4.8
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UMessage_ਨਮੂਨਾ_Part_Type UMessage_ਨਮੂਨਾ_Part_Type;

/**
 * Argument type constants.
 * Returned by Part.getArgType() for ARG_START and ARG_LIMIT parts.
 *
 * Messages nested inside an argument are each delimited by MSG_START and MSG_LIMIT,
 * with a nesting level one greater than the surrounding message.
 * @stable ICU 4.8
 */
ਗਿਣਤੀ UMessage_ਨਮੂਨਾ_Arg_Type {
    /**
     * The argument has no specified type.
     * @stable ICU 4.8
     */
    UMSGPAT_ARG_TYPE_NONE,
    /**
     * The argument has a "simple" type which is provided by the ARG_TYPE part.
     * An ARG_STYLE part might follow that.
     * @stable ICU 4.8
     */
    UMSGPAT_ARG_TYPE_SIMPLE,
    /**
     * The argument is a ChoiceFormat with one or more
     * ((ARG_INT | ARG_DOUBLE), ARG_SELECTOR, message) tuples.
     * @stable ICU 4.8
     */
    UMSGPAT_ARG_TYPE_CHOICE,
    /**
     * The argument is a cardinal-number PluralFormat with an optional ARG_INT or ARG_DOUBLE offset
     * (e.g., offset:1)
     * and one or more (ARG_SELECTOR [explicit-value] message) tuples.
     * If the selector has an explicit value (e.g., =2), then
     * that value is provided by the ARG_INT or ARG_DOUBLE part preceding the message.
     * Otherwise the message immediately follows the ARG_SELECTOR.
     * @stable ICU 4.8
     */
    UMSGPAT_ARG_TYPE_PLURAL,
    /**
     * The argument is a SelectFormat with one or more (ARG_SELECTOR, message) pairs.
     * @stable ICU 4.8
     */
    UMSGPAT_ARG_TYPE_SELECT,
    /**
     * The argument is an ordinal-number PluralFormat
     * with the same style parts sequence and semantics as UMSGPAT_ARG_TYPE_PLURAL.
     * @stable ICU 50
     */
    UMSGPAT_ARG_TYPE_SELECTORDINAL
};
/**
 * @stable ICU 4.8
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UMessage_ਨਮੂਨਾ_Arg_Type UMessage_ਨਮੂਨਾ_Arg_Type;

/**
 * \def UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE
 * Returns true if the argument type has a plural style part sequence and semantics,
 * for example UMSGPAT_ARG_TYPE_PLURAL and UMSGPAT_ARG_TYPE_SELECTORDINAL.
 * @stable ICU 50
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) \
    ((argType)==UMSGPAT_ARG_TYPE_PLURAL || (argType)==UMSGPAT_ARG_TYPE_SELECTORDINAL)

ਗਿਣਤੀ {
    /**
     * Return value from MessagePattern.validateArgumentName() for when
     * the string is a valid "pattern identifier" but not a number.
     * @stable ICU 4.8
     */
    UMSGPAT_ARG_NAME_NOT_ਸੰਖਿਆ=-1,

    /**
     * Return value from MessagePattern.validateArgumentName() for when
     * the string is invalid.
     * It might not be a valid "pattern identifier",
     * or it have only ASCII digits but there is a leading zero or the number is too large.
     * @stable ICU 4.8
     */
    UMSGPAT_ARG_NAME_NOT_VALID=-2
};

/**
 * Special value that is returned by getNumericValue(Part) when no
 * numeric value is defined for a part.
 * @see MessagePattern.getNumericValue()
 * @stable ICU 4.8
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UMSGPAT_NO_NUMERIC_ਮੁੱਲ ((ਦੁੱਗਣਾ)(-123456789))

U_NAMESPACE_BEGIN

class ਸੁਨੇਹਾ_ਨਮੂਨਾ_Double_List;
class ਸੁਨੇਹਾ_ਨਮੂਨਾ_Parts_List;

/**
 * Parses and represents ICU MessageFormat patterns.
 * Also handles patterns for ChoiceFormat, PluralFormat and SelectFormat.
 * Used in the implementations of those classes as well as in tools
 * for message validation, translation and format conversion.
 * <p>
 * The parser handles all syntax relevant for identifying message arguments.
 * This includes "complex" arguments whose style strings contain
 * nested MessageFormat pattern substrings.
 * For "simple" arguments (with no nested MessageFormat pattern substrings),
 * the argument style is not parsed any further.
 * <p>
 * The parser handles named and numbered message arguments and allows both in one message.
 * <p>
 * Once a pattern has been parsed successfully, iterate through the parsed data
 * with countParts(), getPart() and related methods.
 * <p>
 * The data logically represents a parse tree, but is stored and accessed
 * as a list of "parts" for fast and simple parsing and to minimize object allocations.
 * Arguments and nested messages are best handled via recursion.
 * For every _START "part", MessagePattern.getLimitPartIndex() efficiently returns
 * the index of the corresponding _LIMIT "part".
 * <p>
 * List of "parts":
 * <pre>
 * message = MSG_START (SKIP_SYNTAX | INSERT_CHAR | REPLACE_NUMBER | argument)* MSG_LIMIT
 * argument = noneArg | simpleArg | complexArg
 * complexArg = choiceArg | pluralArg | selectArg
 *
 * noneArg = ARG_START.NONE (ARG_NAME | ARG_NUMBER) ARG_LIMIT.NONE
 * simpleArg = ARG_START.SIMPLE (ARG_NAME | ARG_NUMBER) ARG_TYPE [ARG_STYLE] ARG_LIMIT.SIMPLE
 * choiceArg = ARG_START.CHOICE (ARG_NAME | ARG_NUMBER) choiceStyle ARG_LIMIT.CHOICE
 * pluralArg = ARG_START.PLURAL (ARG_NAME | ARG_NUMBER) pluralStyle ARG_LIMIT.PLURAL
 * selectArg = ARG_START.SELECT (ARG_NAME | ARG_NUMBER) selectStyle ARG_LIMIT.SELECT
 *
 * choiceStyle = ((ARG_INT | ARG_DOUBLE) ARG_SELECTOR message)+
 * pluralStyle = [ARG_INT | ARG_DOUBLE] (ARG_SELECTOR [ARG_INT | ARG_DOUBLE] message)+
 * selectStyle = (ARG_SELECTOR message)+
 * </pre>
 * <ul>
 *   <li>Literal output text is not represented directly by "parts" but accessed
 *       between parts of a message, from one part's getLimit() to the next part's getIndex().
 *   <li><code>ARG_START.CHOICE</code> stands for an ARG_START Part with ArgType CHOICE.
 *   <li>In the choiceStyle, the ARG_SELECTOR has the '<', the '#' or
 *       the less-than-or-equal-to sign (U+2264).
 *   <li>In the pluralStyle, the first, optional numeric Part has the "offset:" value.
 *       The optional numeric Part between each (ARG_SELECTOR, message) pair
 *       is the value of an explicit-number selector like "=2",
 *       otherwise the selector is a non-numeric identifier.
 *   <li>The REPLACE_NUMBER Part can occur only in an immediate sub-message of the pluralStyle.
 * </ul>
 * <p>
 * This class is not intended for public subclassing.
 *
 * @stable ICU 4.8
 */
class U_COMMON_API ਸੁਨੇਹਾ_ਨਮੂਨਾ : ਜਨਤਕ UObject {
ਜਨਤਕ:
    /**
     * Constructs an empty MessagePattern with default UMessagePatternApostropheMode.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @stable ICU 4.8
     */
    ਸੁਨੇਹਾ_ਨਮੂਨਾ(UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Constructs an empty MessagePattern.
     * @param mode Explicit UMessagePatternApostropheMode.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @stable ICU 4.8
     */
    ਸੁਨੇਹਾ_ਨਮੂਨਾ(UMessage_ਨਮੂਨਾ_Apostrophe_ਬਹੁਲਕ ਬਹੁਲਕ, UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Constructs a MessagePattern with default UMessagePatternApostropheMode and
     * parses the MessageFormat pattern string.
     * @param pattern a MessageFormat pattern string
     * @param parseError Struct to receive information on the position
     *                   of an error within the pattern.
     *                   Can be nullptr.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * TODO: turn @throws into UErrorCode specifics?
     * @throws IllegalArgumentException for syntax errors in the pattern string
     * @throws IndexOutOfBoundsException if certain limits are exceeded
     *         (e.g., argument number too high, argument name too long, etc.)
     * @throws NumberFormatException if a number could not be parsed
     * @stable ICU 4.8
     */
    ਸੁਨੇਹਾ_ਨਮੂਨਾ(ਅਚਲ UnicodeString &ਨਮੂਨਾ, UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Copy constructor.
     * @param other Object to copy.
     * @stable ICU 4.8
     */
    ਸੁਨੇਹਾ_ਨਮੂਨਾ(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ &other);

    /**
     * Assignment operator.
     * @param other Object to copy.
     * @return *this=other
     * @stable ICU 4.8
     */
    ਸੁਨੇਹਾ_ਨਮੂਨਾ& ਸੰਚਾਲਕ=(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ& other) U_LIFETIME_BOUND;

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    ਆਭਾਸੀ ~ਸੁਨੇਹਾ_ਨਮੂਨਾ();

    /**
     * Parses a MessageFormat pattern string.
     * @param pattern a MessageFormat pattern string
     * @param parseError Struct to receive information on the position
     *                   of an error within the pattern.
     *                   Can be nullptr.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return *this
     * @throws IllegalArgumentException for syntax errors in the pattern string
     * @throws IndexOutOfBoundsException if certain limits are exceeded
     *         (e.g., argument number too high, argument name too long, etc.)
     * @throws NumberFormatException if a number could not be parsed
     * @stable ICU 4.8
     */
    ਸੁਨੇਹਾ_ਨਮੂਨਾ& parse(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                          UParseError* parseError,
                          UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;

    /**
     * Parses a ChoiceFormat pattern string.
     * @param pattern a ChoiceFormat pattern string
     * @param parseError Struct to receive information on the position
     *                   of an error within the pattern.
     *                   Can be nullptr.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return *this
     * @throws IllegalArgumentException for syntax errors in the pattern string
     * @throws IndexOutOfBoundsException if certain limits are exceeded
     *         (e.g., argument number too high, argument name too long, etc.)
     * @throws NumberFormatException if a number could not be parsed
     * @stable ICU 4.8
     */
    ਸੁਨੇਹਾ_ਨਮੂਨਾ& parseChoiceStyle(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                                     UParseError* parseError,
                                     UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;

    /**
     * Parses a PluralFormat pattern string.
     * @param pattern a PluralFormat pattern string
     * @param parseError Struct to receive information on the position
     *                   of an error within the pattern.
     *                   Can be nullptr.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return *this
     * @throws IllegalArgumentException for syntax errors in the pattern string
     * @throws IndexOutOfBoundsException if certain limits are exceeded
     *         (e.g., argument number too high, argument name too long, etc.)
     * @throws NumberFormatException if a number could not be parsed
     * @stable ICU 4.8
     */
    ਸੁਨੇਹਾ_ਨਮੂਨਾ& parsePluralStyle(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                                     UParseError* parseError,
                                     UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;

    /**
     * Parses a SelectFormat pattern string.
     * @param pattern a SelectFormat pattern string
     * @param parseError Struct to receive information on the position
     *                   of an error within the pattern.
     *                   Can be nullptr.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @return *this
     * @throws IllegalArgumentException for syntax errors in the pattern string
     * @throws IndexOutOfBoundsException if certain limits are exceeded
     *         (e.g., argument number too high, argument name too long, etc.)
     * @throws NumberFormatException if a number could not be parsed
     * @stable ICU 4.8
     */
    ਸੁਨੇਹਾ_ਨਮੂਨਾ& parseSelectStyle(ਅਚਲ UnicodeString& ਨਮੂਨਾ,
                                     UParseError* parseError,
                                     UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;

    /**
     * Clears this MessagePattern.
     * countParts() will return 0.
     * @stable ICU 4.8
     */
    ਖਾਲੀ clear();

    /**
     * Clears this MessagePattern and sets the UMessagePatternApostropheMode.
     * countParts() will return 0.
     * @param mode The new UMessagePatternApostropheMode.
     * @stable ICU 4.8
     */
    ਖਾਲੀ clear_ਨਮੂਨਾ_And_ਸੈੱਟ_Apostrophe_ਬਹੁਲਕ(UMessage_ਨਮੂਨਾ_Apostrophe_ਬਹੁਲਕ ਬਹੁਲਕ) {
        clear();
        apos_ਬਹੁਲਕ=ਬਹੁਲਕ;
    }

    /**
     * @param other another object to compare with.
     * @return true if this object is equivalent to the other one.
     * @stable ICU 4.8
     */
    bool ਸੰਚਾਲਕ==(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ &other) ਅਚਲ;

    /**
     * @param other another object to compare with.
     * @return false if this object is equivalent to the other one.
     * @stable ICU 4.8
     */
    ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ &other) ਅਚਲ {
        ਵਾਪਸ !ਸੰਚਾਲਕ==(other);
    }

    /**
     * @return A hash code for this object.
     * @stable ICU 4.8
     */
    int32_t hash_ਕੋਡ() ਅਚਲ;

    /**
     * @return this instance's UMessagePatternApostropheMode.
     * @stable ICU 4.8
     */
    UMessage_ਨਮੂਨਾ_Apostrophe_ਬਹੁਲਕ get_Apostrophe_ਬਹੁਲਕ() ਅਚਲ {
        ਵਾਪਸ apos_ਬਹੁਲਕ;
    }

    // Java has package-private jdkAposMode() here.
    // In C++, this is declared in the MessageImpl class.

    /**
     * @return the parsed pattern string (null if none was parsed).
     * @stable ICU 4.8
     */
    ਅਚਲ UnicodeString& get_ਨਮੂਨਾ_String() ਅਚਲ U_LIFETIME_BOUND {
        ਵਾਪਸ ਸੁਨੇਹਾ;
    }

    /**
     * Does the parsed pattern have named arguments like {first_name}?
     * @return true if the parsed pattern has at least one named argument.
     * @stable ICU 4.8
     */
    UBool hasNamedArguments() ਅਚਲ {
        ਵਾਪਸ has_Arg_ਨਾਂ;
    }

    /**
     * Does the parsed pattern have numbered arguments like {2}?
     * @return true if the parsed pattern has at least one numbered argument.
     * @stable ICU 4.8
     */
    UBool hasNumberedArguments() ਅਚਲ {
        ਵਾਪਸ hasArgNumbers;
    }

    /**
     * Validates and parses an argument name or argument number string.
     * An argument name must be a "pattern identifier", that is, it must contain
     * no Unicode Pattern_Syntax or Pattern_White_Space characters.
     * If it only contains ASCII digits, then it must be a small integer with no leading zero.
     * @param name Input string.
     * @return &gt;=0 if the name is a valid number,
     *         ARG_NAME_NOT_NUMBER (-1) if it is a "pattern identifier" but not all ASCII digits,
     *         ARG_NAME_NOT_VALID (-2) if it is neither.
     * @stable ICU 4.8
     */
    ਸਥਿਰ int32_t validate_ਦਲੀਲ_Name(ਅਚਲ UnicodeString &name);

    /**
     * Returns a version of the parsed pattern string where each ASCII apostrophe
     * is doubled (escaped) if it is not already, and if it is not interpreted as quoting syntax.
     * <p>
     * For example, this turns "I don't '{know}' {gender,select,female{h''er}other{h'im}}."
     * into "I don''t '{know}' {gender,select,female{h''er}other{h''im}}."
     * @return the deep-auto-quoted version of the parsed pattern string.
     * @see MessageFormat.autoQuoteApostrophe()
     * @stable ICU 4.8
     */
    UnicodeString autoQuoteApostropheDeep() ਅਚਲ;

    class Part;

    /**
     * Returns the number of "parts" created by parsing the pattern string.
     * Returns 0 if no pattern has been parsed or clear() was called.
     * @return the number of pattern parts.
     * @stable ICU 4.8
     */
    int32_t ਗਿਣਤੀ_Parts() ਅਚਲ {
        ਵਾਪਸ parts_ਲੰਮਾਈ;
    }

    /**
     * Gets the i-th pattern "part".
     * @param i The index of the Part data. (0..countParts()-1)
     * @return the i-th pattern "part".
     * @stable ICU 4.8
     */
    ਅਚਲ Part& getPart(int32_t i) ਅਚਲ U_LIFETIME_BOUND {
        ਵਾਪਸ parts[i];
    }

    /**
     * Returns the UMessagePatternPartType of the i-th pattern "part".
     * Convenience method for getPart(i).getType().
     * @param i The index of the Part data. (0..countParts()-1)
     * @return The UMessagePatternPartType of the i-th Part.
     * @stable ICU 4.8
     */
    UMessage_ਨਮੂਨਾ_Part_Type getPartType(int32_t i) ਅਚਲ {
        ਵਾਪਸ getPart(i).type;
    }

    /**
     * Returns the pattern index of the specified pattern "part".
     * Convenience method for getPart(partIndex).getIndex().
     * @param partIndex The index of the Part data. (0..countParts()-1)
     * @return The pattern index of this Part.
     * @stable ICU 4.8
     */
    int32_t get_ਨਮੂਨਾ_ਸੂਚਕਾਂਕ(int32_t part_ਸੂਚਕਾਂਕ) ਅਚਲ {
        ਵਾਪਸ getPart(part_ਸੂਚਕਾਂਕ).ਸੂਚਕਾਂਕ;
    }

    /**
     * Returns the substring of the pattern string indicated by the Part.
     * Convenience method for getPatternString().substring(part.getIndex(), part.getLimit()).
     * @param part a part of this MessagePattern.
     * @return the substring associated with part.
     * @stable ICU 4.8
     */
    UnicodeString getSubstring(ਅਚਲ Part& part) ਅਚਲ U_LIFETIME_BOUND {
        ਵਾਪਸ ਸੁਨੇਹਾ.ਆਰਜ਼ੀ_Sub_String(part.ਸੂਚਕਾਂਕ, part.ਲੰਮਾਈ);
    }

    /**
     * Compares the part's substring with the input string s.
     * @param part a part of this MessagePattern.
     * @param s a string.
     * @return true if getSubstring(part).equals(s).
     * @stable ICU 4.8
     */
    UBool partSubstringMatches(ਅਚਲ Part &part, ਅਚਲ UnicodeString &s) ਅਚਲ {
        ਵਾਪਸ 0==ਸੁਨੇਹਾ.compare(part.ਸੂਚਕਾਂਕ, part.ਲੰਮਾਈ, s);
    }

    /**
     * Returns the numeric value associated with an ARG_INT or ARG_DOUBLE.
     * @param part a part of this MessagePattern.
     * @return the part's numeric value, or UMSGPAT_NO_NUMERIC_VALUE if this is not a numeric part.
     * @stable ICU 4.8
     */
    ਦੁੱਗਣਾ get_Numeric_ਮੁੱਲ(ਅਚਲ Part &part) ਅਚਲ;

    /**
     * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
     * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
     * @return the "offset:" value.
     * @stable ICU 4.8
     */
    ਦੁੱਗਣਾ get_Plural_ਵਿੱਥ(int32_t plural_ਸ਼ੁਰੂ) ਅਚਲ;

    /**
     * Returns the index of the ARG|MSG_LIMIT part corresponding to the ARG|MSG_START at start.
     * @param start The index of some Part data (0..countParts()-1);
     *        this Part should be of Type ARG_START or MSG_START.
     * @return The first i>start where getPart(i).getType()==ARG|MSG_LIMIT at the same nesting level,
     *         or start itself if getPartType(msgStart)!=ARG|MSG_START.
     * @stable ICU 4.8
     */
    int32_t get_ਹੱਦਬੰਦੀ_Part_ਸੂਚਕਾਂਕ(int32_t ਸ਼ੁਰੂ) ਅਚਲ {
        int32_t ਹੱਦਬੰਦੀ=getPart(ਸ਼ੁਰੂ).ਹੱਦਬੰਦੀ_Part_ਸੂਚਕਾਂਕ;
        ਜੇ(ਹੱਦਬੰਦੀ<ਸ਼ੁਰੂ) {
            ਵਾਪਸ ਸ਼ੁਰੂ;
        }
        ਵਾਪਸ ਹੱਦਬੰਦੀ;
    }

    /**
     * A message pattern "part", representing a pattern parsing event.
     * There is a part for the start and end of a message or argument,
     * for quoting and escaping of and with ASCII apostrophes,
     * and for syntax elements of "complex" arguments.
     * @stable ICU 4.8
     */
    class Part : ਜਨਤਕ UMemory {
    ਜਨਤਕ:
        /**
         * Default constructor, do not use.
         * @internal
         */
        Part() {}

        /**
         * Returns the type of this part.
         * @return the part type.
         * @stable ICU 4.8
         */
        UMessage_ਨਮੂਨਾ_Part_Type getType() ਅਚਲ {
            ਵਾਪਸ type;
        }

        /**
         * Returns the pattern string index associated with this Part.
         * @return this part's pattern string index.
         * @stable ICU 4.8
         */
        int32_t get_ਸੂਚਕਾਂਕ() ਅਚਲ {
            ਵਾਪਸ ਸੂਚਕਾਂਕ;
        }

        /**
         * Returns the length of the pattern substring associated with this Part.
         * This is 0 for some parts.
         * @return this part's pattern substring length.
         * @stable ICU 4.8
         */
        int32_t get_ਲੰਮਾਈ() ਅਚਲ {
            ਵਾਪਸ ਲੰਮਾਈ;
        }

        /**
         * Returns the pattern string limit (exclusive-end) index associated with this Part.
         * Convenience method for getIndex()+getLength().
         * @return this part's pattern string limit index, same as getIndex()+getLength().
         * @stable ICU 4.8
         */
        int32_t get_ਹੱਦਬੰਦੀ() ਅਚਲ {
            ਵਾਪਸ ਸੂਚਕਾਂਕ+ਲੰਮਾਈ;
        }

        /**
         * Returns a value associated with this part.
         * See the documentation of each part type for details.
         * @return the part value.
         * @stable ICU 4.8
         */
        int32_t get_ਮੁੱਲ() ਅਚਲ {
            ਵਾਪਸ ਮੁੱਲ_2;
        }

        /**
         * Returns the argument type if this part is of type ARG_START or ARG_LIMIT,
         * otherwise UMSGPAT_ARG_TYPE_NONE.
         * @return the argument type for this part.
         * @stable ICU 4.8
         */
        UMessage_ਨਮੂਨਾ_Arg_Type getArgType() ਅਚਲ {
            UMessage_ਨਮੂਨਾ_Part_Type ਸੁਨੇਹਾ_Type=getType();
            ਜੇ(ਸੁਨੇਹਾ_Type ==UMSGPAT_PART_TYPE_ARG_ਸ਼ੁਰੂ || ਸੁਨੇਹਾ_Type ==UMSGPAT_PART_TYPE_ARG_ਹੱਦਬੰਦੀ) {
                ਵਾਪਸ static_cast<UMessage_ਨਮੂਨਾ_Arg_Type>(ਮੁੱਲ_2);
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ UMSGPAT_ARG_TYPE_NONE;
            }
        }

        /**
         * Indicates whether the Part type has a numeric value.
         * If so, then that numeric value can be retrieved via MessagePattern.getNumericValue().
         * @param type The Part type to be tested.
         * @return true if the Part type has a numeric value.
         * @stable ICU 4.8
         */
        ਸਥਿਰ UBool has_Numeric_ਮੁੱਲ(UMessage_ਨਮੂਨਾ_Part_Type type) {
            ਵਾਪਸ type==UMSGPAT_PART_TYPE_ARG_INT || type==UMSGPAT_PART_TYPE_ARG_DOUBLE;
        }

        /**
         * @param other another object to compare with.
         * @return true if this object is equivalent to the other one.
         * @stable ICU 4.8
         */
        bool ਸੰਚਾਲਕ==(ਅਚਲ Part &other) ਅਚਲ;

        /**
         * @param other another object to compare with.
         * @return false if this object is equivalent to the other one.
         * @stable ICU 4.8
         */
        ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ Part &other) ਅਚਲ {
            ਵਾਪਸ !ਸੰਚਾਲਕ==(other);
        }

        /**
         * @return A hash code for this object.
         * @stable ICU 4.8
         */
        int32_t hash_ਕੋਡ() ਅਚਲ {
            ਵਾਪਸ ((type*37+ਸੂਚਕਾਂਕ)*37+ਲੰਮਾਈ)*37+ਮੁੱਲ_2;
        }

    ਨਿਜੀ:
        ਮਿੱਤਰ class ਸੁਨੇਹਾ_ਨਮੂਨਾ;

        ਸਥਿਰ ਅਚਲ int32_t MAX_ਲੰਮਾਈ=0xffff;
        ਸਥਿਰ ਅਚਲ int32_t MAX_ਮੁੱਲ=0x7fff;
        ਸਥਿਰ ਅਚਲ int32_t MAX_NESTED_ਪੱਧਰ=0x03ff;

        // Some fields are not final because they are modified during pattern parsing.
        // After pattern parsing, the parts are effectively immutable.
        UMessage_ਨਮੂਨਾ_Part_Type type;
        int32_t ਸੂਚਕਾਂਕ;
        uint16_t ਲੰਮਾਈ;
        int16_t ਮੁੱਲ_2;
        int32_t ਹੱਦਬੰਦੀ_Part_ਸੂਚਕਾਂਕ;
    };

ਨਿਜੀ:
    ਖਾਲੀ preParse(ਅਚਲ UnicodeString &ਨਮੂਨਾ, UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ);

    ਖਾਲੀ postParse();

    int32_t parse_ਸੁਨੇਹਾ(int32_t ਸੂਚਕਾਂਕ, int32_t ਸੁਨੇਹਾ_ਸ਼ੁਰੂ_ਲੰਮਾਈ,
                         int32_t nestingLevel, UMessage_ਨਮੂਨਾ_Arg_Type ਮਾਪਾ_Type,
                         UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ);

    int32_t parseArg(int32_t ਸੂਚਕਾਂਕ, int32_t arg_ਸ਼ੁਰੂ_ਲੰਮਾਈ, int32_t nestingLevel,
                     UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ);

    int32_t parseSimpleStyle(int32_t ਸੂਚਕਾਂਕ, UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ);

    int32_t parseChoiceStyle(int32_t ਸੂਚਕਾਂਕ, int32_t nestingLevel,
                             UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ);

    int32_t parsePluralOrSelectStyle(UMessage_ਨਮੂਨਾ_Arg_Type argType, int32_t ਸੂਚਕਾਂਕ, int32_t nestingLevel,
                                     UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Validates and parses an argument name or argument number string.
     * This internal method assumes that the input substring is a "pattern identifier".
     * @return &gt;=0 if the name is a valid number,
     *         ARG_NAME_NOT_NUMBER (-1) if it is a "pattern identifier" but not all ASCII digits,
     *         ARG_NAME_NOT_VALID (-2) if it is neither.
     * @see #validateArgumentName(String)
     */
    ਸਥਿਰ int32_t parse_Arg_ਸੰਖਿਆ(ਅਚਲ UnicodeString &s, int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ);

    int32_t parse_Arg_ਸੰਖਿਆ(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ parse_Arg_ਸੰਖਿਆ(ਸੁਨੇਹਾ, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ);
    }

    /**
     * Parses a number from the specified message substring.
     * @param start start index into the message string
     * @param limit limit index into the message string, must be start<limit
     * @param allowInfinity true if U+221E is allowed (for ChoiceFormat)
     * @param parseError
     * @param errorCode
     */
    ਖਾਲੀ parseDouble(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, UBool allowInfinity,
                     UParseError *parseError, UError_ਕੋਡ &error_ਕੋਡ);

    // Java has package-private appendReducedApostrophes() here.
    // In C++, this is declared in the MessageImpl class.

    int32_t skipWhiteSpace(int32_t ਸੂਚਕਾਂਕ);

    int32_t skipIdentifier(int32_t ਸੂਚਕਾਂਕ);

    /**
     * Skips a sequence of characters that could occur in a double value.
     * Does not fully parse or validate the value.
     */
    int32_t skipDouble(int32_t ਸੂਚਕਾਂਕ);

    ਸਥਿਰ UBool isArgTypeChar(UChar32 c);

    UBool isChoice(int32_t ਸੂਚਕਾਂਕ);

    UBool isPlural(int32_t ਸੂਚਕਾਂਕ);

    UBool isSelect(int32_t ਸੂਚਕਾਂਕ);

    UBool isOrdinal(int32_t ਸੂਚਕਾਂਕ);

    /**
     * @return true if we are inside a MessageFormat (sub-)pattern,
     *         as opposed to inside a top-level choice/plural/select pattern.
     */
    UBool ਵਿੱਚ_ਸੁਨੇਹਾ_Format_ਨਮੂਨਾ(int32_t nestingLevel);

    /**
     * @return true if we are in a MessageFormat sub-pattern
     *         of a top-level ChoiceFormat pattern.
     */
    UBool ਵਿੱਚ_ਸਿਖਰ_Level_Choice_ਸੁਨੇਹਾ(int32_t nestingLevel, UMessage_ਨਮੂਨਾ_Arg_Type ਮਾਪਾ_Type);

    ਖਾਲੀ ਜੋੜੋ_Part(UMessage_ਨਮੂਨਾ_Part_Type type, int32_t ਸੂਚਕਾਂਕ, int32_t ਲੰਮਾਈ,
                 int32_t ਮੁੱਲ_2, UError_ਕੋਡ &error_ਕੋਡ);

    ਖਾਲੀ ਜੋੜੋ_ਹੱਦਬੰਦੀ_Part(int32_t ਸ਼ੁਰੂ,
                      UMessage_ਨਮੂਨਾ_Part_Type type, int32_t ਸੂਚਕਾਂਕ, int32_t ਲੰਮਾਈ,
                      int32_t ਮੁੱਲ_2, UError_ਕੋਡ &error_ਕੋਡ);

    ਖਾਲੀ ਜੋੜੋ_Arg_Double_Part(ਦੁੱਗਣਾ numeric_ਮੁੱਲ, int32_t ਸ਼ੁਰੂ, int32_t ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ);

    ਖਾਲੀ ਸੈੱਟ_Parse_Error(UParseError *parseError, int32_t ਸੂਚਕਾਂਕ);

    UBool init(UError_ਕੋਡ &error_ਕੋਡ);
    UBool copyStorage(ਅਚਲ ਸੁਨੇਹਾ_ਨਮੂਨਾ &other, UError_ਕੋਡ &error_ਕੋਡ);

    UMessage_ਨਮੂਨਾ_Apostrophe_ਬਹੁਲਕ apos_ਬਹੁਲਕ;
    UnicodeString ਸੁਨੇਹਾ;
    // ArrayList<Part> parts=new ArrayList<Part>();
    ਸੁਨੇਹਾ_ਨਮੂਨਾ_Parts_List *partsList;
    Part *parts;
    int32_t parts_ਲੰਮਾਈ;
    // ArrayList<Double> numericValues;
    ਸੁਨੇਹਾ_ਨਮੂਨਾ_Double_List *numeric_ਮੁੱਲਾਂ_List;
    ਦੁੱਗਣਾ *numeric_ਮੁੱਲਾਂ;
    int32_t numeric_ਮੁੱਲਾਂ_ਲੰਮਾਈ;
    UBool has_Arg_ਨਾਂ;
    UBool hasArgNumbers;
    UBool needsAutoQuoting;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_FORMATTING

#ਜੇ_ਅੰਤ /* U_SHOW_CPLUSPLUS_API */

#ਜੇ_ਅੰਤ  // __MESSAGEPATTERN_H__
