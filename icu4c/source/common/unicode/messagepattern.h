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

#جے_بنا_تعریف __MESSAGEPATTERN_H__
#تعریف_کرو __MESSAGEPATTERN_H__

/**
 * \file
 * \brief C++ API: MessagePattern class: Parses and represents ICU MessageFormat patterns.
 */

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#إذا !UCONFIG_NO_FORMATTING

#ضمن "unicode/parseerr.h"
#ضمن "unicode/unistr.h"

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
تعداد UMessage_نمونہ_Apostrophe_منوال {
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
تعريفنوع تعداد UMessage_نمونہ_Apostrophe_منوال UMessage_نمونہ_Apostrophe_منوال;

/**
 * MessagePattern::Part type constants.
 * @stable ICU 4.8
 */
تعداد UMessage_نمونہ_Part_Type {
    /**
     * Start of a message pattern (main or nested).
     * The length is 0 for the top-level message
     * and for a choice argument sub-message, otherwise 1 for the '{'.
     * The value indicates the nesting level, starting with 0 for the main message.
     * <p>
     * There is always a later MSG_LIMIT part.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_پیغام_شروع,
    /**
     * End of a message pattern (main or nested).
     * The length is 0 for the top-level message and
     * the last sub-message of a choice argument,
     * otherwise 1 for the '}' or (in a choice argument style) the '|'.
     * The value indicates the nesting level, starting with 0 for the main message.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_پیغام_حد_بندی,
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
    UMSGPAT_PART_TYPE_داخل_کرو_CHAR,
    /**
     * Indicates a syntactic (non-escaped) # symbol in a plural variant.
     * When formatting, replace this part's substring with the
     * (value-offset) for the plural argument value.
     * The value is undefined and currently always 0.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_REPLACE_نمبر,
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
    UMSGPAT_PART_TYPE_ARG_شروع,
    /**
     * End of an argument.
     * The length is 1 for the '}'.
     * The value is the ordinal value of the ArgType. Use getArgType().
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ARG_حد_بندی,
    /**
     * The argument number, provided by the value.
     * @stable ICU 4.8
     */
    UMSGPAT_PART_TYPE_ARG_نمبر,
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
تعريفنوع تعداد UMessage_نمونہ_Part_Type UMessage_نمونہ_Part_Type;

/**
 * Argument type constants.
 * Returned by Part.getArgType() for ARG_START and ARG_LIMIT parts.
 *
 * Messages nested inside an argument are each delimited by MSG_START and MSG_LIMIT,
 * with a nesting level one greater than the surrounding message.
 * @stable ICU 4.8
 */
تعداد UMessage_نمونہ_Arg_Type {
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
تعريفنوع تعداد UMessage_نمونہ_Arg_Type UMessage_نمونہ_Arg_Type;

/**
 * \def UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE
 * Returns true if the argument type has a plural style part sequence and semantics,
 * for example UMSGPAT_ARG_TYPE_PLURAL and UMSGPAT_ARG_TYPE_SELECTORDINAL.
 * @stable ICU 50
 */
#تعریف_کرو UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) \
    ((argType)==UMSGPAT_ARG_TYPE_PLURAL || (argType)==UMSGPAT_ARG_TYPE_SELECTORDINAL)

تعداد {
    /**
     * Return value from MessagePattern.validateArgumentName() for when
     * the string is a valid "pattern identifier" but not a number.
     * @stable ICU 4.8
     */
    UMSGPAT_ARG_NAME_NOT_نمبر=-1,

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
#تعریف_کرو UMSGPAT_NO_NUMERIC_قدر ((مزدوج)(-123456789))

U_NAMESPACE_BEGIN

class سنیہا_نمونہ_Double_List;
class سنیہا_نمونہ_Parts_List;

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
class U_COMMON_API سنیہا_نمونہ : عام UObject {
عام:
    /**
     * Constructs an empty MessagePattern with default UMessagePatternApostropheMode.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @stable ICU 4.8
     */
    سنیہا_نمونہ(UError_کوڈ &error_کوڈ);

    /**
     * Constructs an empty MessagePattern.
     * @param mode Explicit UMessagePatternApostropheMode.
     * @param errorCode Standard ICU error code. Its input value must
     *                  pass the U_SUCCESS() test, or else the function returns
     *                  immediately. Check for U_FAILURE() on output or use with
     *                  function chaining. (See User Guide for details.)
     * @stable ICU 4.8
     */
    سنیہا_نمونہ(UMessage_نمونہ_Apostrophe_منوال منوال, UError_کوڈ &error_کوڈ);

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
    سنیہا_نمونہ(ثابت UnicodeString &نمونہ, UParseError *parseError, UError_کوڈ &error_کوڈ);

    /**
     * Copy constructor.
     * @param other Object to copy.
     * @stable ICU 4.8
     */
    سنیہا_نمونہ(ثابت سنیہا_نمونہ &other);

    /**
     * Assignment operator.
     * @param other Object to copy.
     * @return *this=other
     * @stable ICU 4.8
     */
    سنیہا_نمونہ& عامل=(ثابت سنیہا_نمونہ& other) U_LIFETIME_BOUND;

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    مجازی ~سنیہا_نمونہ();

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
    سنیہا_نمونہ& parse(ثابت UnicodeString& نمونہ,
                          UParseError* parseError,
                          UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND;

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
    سنیہا_نمونہ& parseChoiceStyle(ثابت UnicodeString& نمونہ,
                                     UParseError* parseError,
                                     UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND;

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
    سنیہا_نمونہ& parsePluralStyle(ثابت UnicodeString& نمونہ,
                                     UParseError* parseError,
                                     UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND;

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
    سنیہا_نمونہ& parseSelectStyle(ثابت UnicodeString& نمونہ,
                                     UParseError* parseError,
                                     UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND;

    /**
     * Clears this MessagePattern.
     * countParts() will return 0.
     * @stable ICU 4.8
     */
    فراغ clear();

    /**
     * Clears this MessagePattern and sets the UMessagePatternApostropheMode.
     * countParts() will return 0.
     * @param mode The new UMessagePatternApostropheMode.
     * @stable ICU 4.8
     */
    فراغ clear_نمونہ_And_سیٹ_Apostrophe_منوال(UMessage_نمونہ_Apostrophe_منوال منوال) {
        clear();
        apos_منوال=منوال;
    }

    /**
     * @param other another object to compare with.
     * @return true if this object is equivalent to the other one.
     * @stable ICU 4.8
     */
    bool عامل==(ثابت سنیہا_نمونہ &other) ثابت;

    /**
     * @param other another object to compare with.
     * @return false if this object is equivalent to the other one.
     * @stable ICU 4.8
     */
    مضمن bool عامل!=(ثابت سنیہا_نمونہ &other) ثابت {
        أرجع !عامل==(other);
    }

    /**
     * @return A hash code for this object.
     * @stable ICU 4.8
     */
    int32_t hash_کوڈ() ثابت;

    /**
     * @return this instance's UMessagePatternApostropheMode.
     * @stable ICU 4.8
     */
    UMessage_نمونہ_Apostrophe_منوال get_Apostrophe_منوال() ثابت {
        أرجع apos_منوال;
    }

    // Java has package-private jdkAposMode() here.
    // In C++, this is declared in the MessageImpl class.

    /**
     * @return the parsed pattern string (null if none was parsed).
     * @stable ICU 4.8
     */
    ثابت UnicodeString& get_نمونہ_String() ثابت U_LIFETIME_BOUND {
        أرجع پیغام;
    }

    /**
     * Does the parsed pattern have named arguments like {first_name}?
     * @return true if the parsed pattern has at least one named argument.
     * @stable ICU 4.8
     */
    UBool hasNamedArguments() ثابت {
        أرجع has_Arg_ناں;
    }

    /**
     * Does the parsed pattern have numbered arguments like {2}?
     * @return true if the parsed pattern has at least one numbered argument.
     * @stable ICU 4.8
     */
    UBool hasNumberedArguments() ثابت {
        أرجع hasArgNumbers;
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
    ساكن int32_t validate_دلیل_Name(ثابت UnicodeString &name);

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
    UnicodeString autoQuoteApostropheDeep() ثابت;

    class Part;

    /**
     * Returns the number of "parts" created by parsing the pattern string.
     * Returns 0 if no pattern has been parsed or clear() was called.
     * @return the number of pattern parts.
     * @stable ICU 4.8
     */
    int32_t گنتی_Parts() ثابت {
        أرجع parts_لمائی;
    }

    /**
     * Gets the i-th pattern "part".
     * @param i The index of the Part data. (0..countParts()-1)
     * @return the i-th pattern "part".
     * @stable ICU 4.8
     */
    ثابت Part& getPart(int32_t i) ثابت U_LIFETIME_BOUND {
        أرجع parts[i];
    }

    /**
     * Returns the UMessagePatternPartType of the i-th pattern "part".
     * Convenience method for getPart(i).getType().
     * @param i The index of the Part data. (0..countParts()-1)
     * @return The UMessagePatternPartType of the i-th Part.
     * @stable ICU 4.8
     */
    UMessage_نمونہ_Part_Type getPartType(int32_t i) ثابت {
        أرجع getPart(i).type;
    }

    /**
     * Returns the pattern index of the specified pattern "part".
     * Convenience method for getPart(partIndex).getIndex().
     * @param partIndex The index of the Part data. (0..countParts()-1)
     * @return The pattern index of this Part.
     * @stable ICU 4.8
     */
    int32_t get_نمونہ_اشاریہ(int32_t part_اشاریہ) ثابت {
        أرجع getPart(part_اشاریہ).اشاریہ;
    }

    /**
     * Returns the substring of the pattern string indicated by the Part.
     * Convenience method for getPatternString().substring(part.getIndex(), part.getLimit()).
     * @param part a part of this MessagePattern.
     * @return the substring associated with part.
     * @stable ICU 4.8
     */
    UnicodeString getSubstring(ثابت Part& part) ثابت U_LIFETIME_BOUND {
        أرجع پیغام.عارضی_Sub_String(part.اشاریہ, part.لمائی);
    }

    /**
     * Compares the part's substring with the input string s.
     * @param part a part of this MessagePattern.
     * @param s a string.
     * @return true if getSubstring(part).equals(s).
     * @stable ICU 4.8
     */
    UBool partSubstringMatches(ثابت Part &part, ثابت UnicodeString &s) ثابت {
        أرجع 0==پیغام.compare(part.اشاریہ, part.لمائی, s);
    }

    /**
     * Returns the numeric value associated with an ARG_INT or ARG_DOUBLE.
     * @param part a part of this MessagePattern.
     * @return the part's numeric value, or UMSGPAT_NO_NUMERIC_VALUE if this is not a numeric part.
     * @stable ICU 4.8
     */
    مزدوج get_Numeric_قدر(ثابت Part &part) ثابت;

    /**
     * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
     * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
     * @return the "offset:" value.
     * @stable ICU 4.8
     */
    مزدوج get_Plural_وتھ(int32_t plural_شروع) ثابت;

    /**
     * Returns the index of the ARG|MSG_LIMIT part corresponding to the ARG|MSG_START at start.
     * @param start The index of some Part data (0..countParts()-1);
     *        this Part should be of Type ARG_START or MSG_START.
     * @return The first i>start where getPart(i).getType()==ARG|MSG_LIMIT at the same nesting level,
     *         or start itself if getPartType(msgStart)!=ARG|MSG_START.
     * @stable ICU 4.8
     */
    int32_t get_حد_بندی_Part_اشاریہ(int32_t شروع) ثابت {
        int32_t حد_بندی=getPart(شروع).حد_بندی_Part_اشاریہ;
        إذا(حد_بندی<شروع) {
            أرجع شروع;
        }
        أرجع حد_بندی;
    }

    /**
     * A message pattern "part", representing a pattern parsing event.
     * There is a part for the start and end of a message or argument,
     * for quoting and escaping of and with ASCII apostrophes,
     * and for syntax elements of "complex" arguments.
     * @stable ICU 4.8
     */
    class Part : عام UMemory {
    عام:
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
        UMessage_نمونہ_Part_Type getType() ثابت {
            أرجع type;
        }

        /**
         * Returns the pattern string index associated with this Part.
         * @return this part's pattern string index.
         * @stable ICU 4.8
         */
        int32_t get_اشاریہ() ثابت {
            أرجع اشاریہ;
        }

        /**
         * Returns the length of the pattern substring associated with this Part.
         * This is 0 for some parts.
         * @return this part's pattern substring length.
         * @stable ICU 4.8
         */
        int32_t get_لمائی() ثابت {
            أرجع لمائی;
        }

        /**
         * Returns the pattern string limit (exclusive-end) index associated with this Part.
         * Convenience method for getIndex()+getLength().
         * @return this part's pattern string limit index, same as getIndex()+getLength().
         * @stable ICU 4.8
         */
        int32_t get_حد_بندی() ثابت {
            أرجع اشاریہ+لمائی;
        }

        /**
         * Returns a value associated with this part.
         * See the documentation of each part type for details.
         * @return the part value.
         * @stable ICU 4.8
         */
        int32_t get_قدر() ثابت {
            أرجع قدر_2;
        }

        /**
         * Returns the argument type if this part is of type ARG_START or ARG_LIMIT,
         * otherwise UMSGPAT_ARG_TYPE_NONE.
         * @return the argument type for this part.
         * @stable ICU 4.8
         */
        UMessage_نمونہ_Arg_Type getArgType() ثابت {
            UMessage_نمونہ_Part_Type پیغام_Type=getType();
            إذا(پیغام_Type ==UMSGPAT_PART_TYPE_ARG_شروع || پیغام_Type ==UMSGPAT_PART_TYPE_ARG_حد_بندی) {
                أرجع static_cast<UMessage_نمونہ_Arg_Type>(قدر_2);
            } وإلا {
                أرجع UMSGPAT_ARG_TYPE_NONE;
            }
        }

        /**
         * Indicates whether the Part type has a numeric value.
         * If so, then that numeric value can be retrieved via MessagePattern.getNumericValue().
         * @param type The Part type to be tested.
         * @return true if the Part type has a numeric value.
         * @stable ICU 4.8
         */
        ساكن UBool has_Numeric_قدر(UMessage_نمونہ_Part_Type type) {
            أرجع type==UMSGPAT_PART_TYPE_ARG_INT || type==UMSGPAT_PART_TYPE_ARG_DOUBLE;
        }

        /**
         * @param other another object to compare with.
         * @return true if this object is equivalent to the other one.
         * @stable ICU 4.8
         */
        bool عامل==(ثابت Part &other) ثابت;

        /**
         * @param other another object to compare with.
         * @return false if this object is equivalent to the other one.
         * @stable ICU 4.8
         */
        مضمن bool عامل!=(ثابت Part &other) ثابت {
            أرجع !عامل==(other);
        }

        /**
         * @return A hash code for this object.
         * @stable ICU 4.8
         */
        int32_t hash_کوڈ() ثابت {
            أرجع ((type*37+اشاریہ)*37+لمائی)*37+قدر_2;
        }

    خاص:
        یار class سنیہا_نمونہ;

        ساكن ثابت int32_t MAX_لمائی=0xffff;
        ساكن ثابت int32_t MAX_قدر=0x7fff;
        ساكن ثابت int32_t MAX_NESTED_پدھراں=0x03ff;

        // Some fields are not final because they are modified during pattern parsing.
        // After pattern parsing, the parts are effectively immutable.
        UMessage_نمونہ_Part_Type type;
        int32_t اشاریہ;
        uint16_t لمائی;
        int16_t قدر_2;
        int32_t حد_بندی_Part_اشاریہ;
    };

خاص:
    فراغ preParse(ثابت UnicodeString &نمونہ, UParseError *parseError, UError_کوڈ &error_کوڈ);

    فراغ postParse();

    int32_t parse_سنیہا(int32_t اشاریہ, int32_t پیغام_شروع_لمائی,
                         int32_t nestingLevel, UMessage_نمونہ_Arg_Type ماپا_Type,
                         UParseError *parseError, UError_کوڈ &error_کوڈ);

    int32_t parseArg(int32_t اشاریہ, int32_t arg_شروع_لمائی, int32_t nestingLevel,
                     UParseError *parseError, UError_کوڈ &error_کوڈ);

    int32_t parseSimpleStyle(int32_t اشاریہ, UParseError *parseError, UError_کوڈ &error_کوڈ);

    int32_t parseChoiceStyle(int32_t اشاریہ, int32_t nestingLevel,
                             UParseError *parseError, UError_کوڈ &error_کوڈ);

    int32_t parsePluralOrSelectStyle(UMessage_نمونہ_Arg_Type argType, int32_t اشاریہ, int32_t nestingLevel,
                                     UParseError *parseError, UError_کوڈ &error_کوڈ);

    /**
     * Validates and parses an argument name or argument number string.
     * This internal method assumes that the input substring is a "pattern identifier".
     * @return &gt;=0 if the name is a valid number,
     *         ARG_NAME_NOT_NUMBER (-1) if it is a "pattern identifier" but not all ASCII digits,
     *         ARG_NAME_NOT_VALID (-2) if it is neither.
     * @see #validateArgumentName(String)
     */
    ساكن int32_t parse_Arg_نمبر(ثابت UnicodeString &s, int32_t شروع, int32_t حد_بندی);

    int32_t parse_Arg_نمبر(int32_t شروع, int32_t حد_بندی) {
        أرجع parse_Arg_نمبر(پیغام, شروع, حد_بندی);
    }

    /**
     * Parses a number from the specified message substring.
     * @param start start index into the message string
     * @param limit limit index into the message string, must be start<limit
     * @param allowInfinity true if U+221E is allowed (for ChoiceFormat)
     * @param parseError
     * @param errorCode
     */
    فراغ parseDouble(int32_t شروع, int32_t حد_بندی, UBool allowInfinity,
                     UParseError *parseError, UError_کوڈ &error_کوڈ);

    // Java has package-private appendReducedApostrophes() here.
    // In C++, this is declared in the MessageImpl class.

    int32_t skipWhiteSpace(int32_t اشاریہ);

    int32_t skipIdentifier(int32_t اشاریہ);

    /**
     * Skips a sequence of characters that could occur in a double value.
     * Does not fully parse or validate the value.
     */
    int32_t skipDouble(int32_t اشاریہ);

    ساكن UBool isArgTypeChar(UChar32 c);

    UBool isChoice(int32_t اشاریہ);

    UBool isPlural(int32_t اشاریہ);

    UBool isSelect(int32_t اشاریہ);

    UBool isOrdinal(int32_t اشاریہ);

    /**
     * @return true if we are inside a MessageFormat (sub-)pattern,
     *         as opposed to inside a top-level choice/plural/select pattern.
     */
    UBool وچ_سنیہا_Format_نمونہ(int32_t nestingLevel);

    /**
     * @return true if we are in a MessageFormat sub-pattern
     *         of a top-level ChoiceFormat pattern.
     */
    UBool وچ_چوٹی_Level_Choice_سنیہا(int32_t nestingLevel, UMessage_نمونہ_Arg_Type ماپا_Type);

    فراغ جوڑو_Part(UMessage_نمونہ_Part_Type type, int32_t اشاریہ, int32_t لمائی,
                 int32_t قدر_2, UError_کوڈ &error_کوڈ);

    فراغ جوڑو_حد_بندی_Part(int32_t شروع,
                      UMessage_نمونہ_Part_Type type, int32_t اشاریہ, int32_t لمائی,
                      int32_t قدر_2, UError_کوڈ &error_کوڈ);

    فراغ جوڑو_Arg_Double_Part(مزدوج numeric_قدر, int32_t شروع, int32_t لمائی, UError_کوڈ &error_کوڈ);

    فراغ سیٹ_Parse_Error(UParseError *parseError, int32_t اشاریہ);

    UBool init(UError_کوڈ &error_کوڈ);
    UBool copyStorage(ثابت سنیہا_نمونہ &other, UError_کوڈ &error_کوڈ);

    UMessage_نمونہ_Apostrophe_منوال apos_منوال;
    UnicodeString پیغام;
    // ArrayList<Part> parts=new ArrayList<Part>();
    سنیہا_نمونہ_Parts_List *partsList;
    Part *parts;
    int32_t parts_لمائی;
    // ArrayList<Double> numericValues;
    سنیہا_نمونہ_Double_List *numeric_اقدار_List;
    مزدوج *numeric_اقدار;
    int32_t numeric_اقدار_لمائی;
    UBool has_Arg_ناں;
    UBool hasArgNumbers;
    UBool needsAutoQuoting;
};

U_NAMESPACE_سرا

#جے_انت  // !UCONFIG_NO_FORMATTING

#جے_انت /* U_SHOW_CPLUSPLUS_API */

#جے_انت  // __MESSAGEPATTERN_H__
