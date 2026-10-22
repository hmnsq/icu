// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.h
*/

#अगर_अघोषित __SIMPLEFORMATTER_H__
#घोषणा __SIMPLEFORMATTER_H__

/**
 * \file
 * \brief C++ API: Simple formatter, minimal subset of MessageFormat.
 */

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

#समावेश "unicode/unistr.h"

U_NAMESPACE_BEGIN

// Forward declaration:
namespace संख्या::impl {
class SimpleModifier;
}

/**
 * Formats simple patterns like "{1} was born in {0}".
 * Minimal subset of MessageFormat; fast, simple, minimal dependencies.
 * Supports only numbered arguments with no type nor style parameters,
 * and formats only string values.
 * Quoting via ASCII apostrophe compatible with ICU MessageFormat default behavior.
 *
 * Factory methods set error codes for syntax errors
 * and for too few or too many arguments/placeholders.
 *
 * SimpleFormatter objects are thread-safe except for assignment and applying new patterns.
 *
 * Example:
 * <pre>
 * UErrorCode errorCode = U_ZERO_ERROR;
 * SimpleFormatter fmt("{1} '{born}' in {0}", errorCode);
 * UnicodeString result;
 *
 * // Output: "paul {born} in england"
 * fmt.format("england", "paul", result, errorCode);
 * </pre>
 *
 * This class is not intended for public subclassing.
 *
 * @see MessageFormat
 * @see UMessagePatternApostropheMode
 * @stable ICU 57
 */
class U_COMMON_API SimpleFormatter final : खुला UMemory {
खुला:
    /**
     * Default constructor.
     * @stable ICU 57
     */
    SimpleFormatter() : compiledPattern(static_cast<char16_t>(0)) {}

    /**
     * Constructs a formatter from the pattern string.
     *
     * @param pattern The pattern string.
     * @param errorCode ICU error code in/out parameter.
     *                  Must fulfill U_SUCCESS before the function call.
     *                  Set to U_ILLEGAL_ARGUMENT_ERROR for bad argument syntax.
     * @stable ICU 57
     */
    SimpleFormatter(स्थिर UnicodeString& pattern, UErrorCode &errorCode) {
        applyPattern(pattern, errorCode);
    }

    /**
     * Constructs a formatter from the pattern string.
     * The number of arguments checked against the given limits is the
     * highest argument number plus one, not the number of occurrences of arguments.
     *
     * @param pattern The pattern string.
     * @param min The pattern must have at least this many arguments.
     * @param max The pattern must have at most this many arguments.
     * @param errorCode ICU error code in/out parameter.
     *                  Must fulfill U_SUCCESS before the function call.
     *                  Set to U_ILLEGAL_ARGUMENT_ERROR for bad argument syntax and
     *                  too few or too many arguments.
     * @stable ICU 57
     */
    SimpleFormatter(स्थिर UnicodeString& pattern, int32_t min, int32_t max,
                    UErrorCode &errorCode) {
        applyPatternMinMaxArguments(pattern, min, max, errorCode);
    }

    /**
     * Copy constructor.
     * @stable ICU 57
     */
    SimpleFormatter(स्थिर SimpleFormatter& other)
            : compiledPattern(other.compiledPattern) {}

    /**
     * Assignment operator.
     * @stable ICU 57
     */
    SimpleFormatter& चालक=(स्थिर SimpleFormatter& other) U_LIFETIME_BOUND;

    /**
     * Destructor.
     * @stable ICU 57
     */
    ~SimpleFormatter();

    /**
     * Changes this object according to the new pattern.
     *
     * @param pattern The pattern string.
     * @param errorCode ICU error code in/out parameter.
     *                  Must fulfill U_SUCCESS before the function call.
     *                  Set to U_ILLEGAL_ARGUMENT_ERROR for bad argument syntax.
     * @return true if U_SUCCESS(errorCode).
     * @stable ICU 57
     */
    UBool applyPattern(स्थिर UnicodeString &pattern, UErrorCode &errorCode) {
        वापस applyPatternMinMaxArguments(pattern, 0, INT32_MAX, errorCode);
    }

    /**
     * Changes this object according to the new pattern.
     * The number of arguments checked against the given limits is the
     * highest argument number plus one, not the number of occurrences of arguments.
     *
     * @param pattern The pattern string.
     * @param min The pattern must have at least this many arguments.
     * @param max The pattern must have at most this many arguments.
     * @param errorCode ICU error code in/out parameter.
     *                  Must fulfill U_SUCCESS before the function call.
     *                  Set to U_ILLEGAL_ARGUMENT_ERROR for bad argument syntax and
     *                  too few or too many arguments.
     * @return true if U_SUCCESS(errorCode).
     * @stable ICU 57
     */
    UBool applyPatternMinMaxArguments(स्थिर UnicodeString &pattern,
                                      int32_t min, int32_t max, UErrorCode &errorCode);

    /**
     * @return The max argument number + 1.
     * @stable ICU 57
     */
    int32_t getArgumentLimit() स्थिर {
        वापस getArgumentLimit(compiledPattern.get_बफर(), compiledPattern.लंबाई());
    }

    /**
     * Formats the given value, appending to the appendTo builder.
     * The argument value must not be the same object as appendTo.
     * getArgumentLimit() must be at most 1.
     *
     * @param value0 Value for argument {0}.
     * @param appendTo Gets the formatted pattern and value appended.
     * @param errorCode ICU error code in/out parameter.
     *                  Must fulfill U_SUCCESS before the function call.
     * @return appendTo
     * @stable ICU 57
     */
    UnicodeString& format(
            स्थिर UnicodeString& value0,
            UnicodeString& appendTo U_LIFETIME_BOUND,
            UErrorCode& errorCode) स्थिर;

    /**
     * Formats the given values, appending to the appendTo builder.
     * An argument value must not be the same object as appendTo.
     * getArgumentLimit() must be at most 2.
     *
     * @param value0 Value for argument {0}.
     * @param value1 Value for argument {1}.
     * @param appendTo Gets the formatted pattern and values appended.
     * @param errorCode ICU error code in/out parameter.
     *                  Must fulfill U_SUCCESS before the function call.
     * @return appendTo
     * @stable ICU 57
     */
    UnicodeString& format(
            स्थिर UnicodeString& value0,
            स्थिर UnicodeString& value1,
            UnicodeString& appendTo U_LIFETIME_BOUND,
            UErrorCode& errorCode) स्थिर;

    /**
     * Formats the given values, appending to the appendTo builder.
     * An argument value must not be the same object as appendTo.
     * getArgumentLimit() must be at most 3.
     *
     * @param value0 Value for argument {0}.
     * @param value1 Value for argument {1}.
     * @param value2 Value for argument {2}.
     * @param appendTo Gets the formatted pattern and values appended.
     * @param errorCode ICU error code in/out parameter.
     *                  Must fulfill U_SUCCESS before the function call.
     * @return appendTo
     * @stable ICU 57
     */
    UnicodeString& format(
            स्थिर UnicodeString& value0,
            स्थिर UnicodeString& value1,
            स्थिर UnicodeString& value2,
            UnicodeString& appendTo U_LIFETIME_BOUND,
            UErrorCode& errorCode) स्थिर;

    /**
     * Formats the given values, appending to the appendTo string.
     *
     * @param values The argument values.
     *               An argument value must not be the same object as appendTo.
     *               Can be nullptr if valuesLength==getArgumentLimit()==0.
     * @param valuesLength The length of the values array.
     *                     Must be at least getArgumentLimit().
     * @param appendTo Gets the formatted pattern and values appended.
     * @param offsets offsets[i] receives the offset of where
     *                values[i] replaced pattern argument {i}.
     *                Can be shorter or longer than values. Can be nullptr if offsetsLength==0.
     *                If there is no {i} in the pattern, then offsets[i] is set to -1.
     * @param offsetsLength The length of the offsets array.
     * @param errorCode ICU error code in/out parameter.
     *                  Must fulfill U_SUCCESS before the function call.
     * @return appendTo
     * @stable ICU 57
     */
    UnicodeString& formatAndAppend(
            स्थिर UnicodeString* स्थिर* मूल्यावली,
            int32_t मूल्यावली_लंबाई,
            UnicodeString& appendTo U_LIFETIME_BOUND,
            int32_t* offsets,
            int32_t offsets_लंबाई,
            UErrorCode& errorCode) स्थिर;

    /**
     * Formats the given values, replacing the contents of the result string.
     * May optimize by actually appending to the result if it is the same object
     * as the value corresponding to the initial argument in the pattern.
     *
     * @param values The argument values.
     *               An argument value may be the same object as result.
     *               Can be nullptr if valuesLength==getArgumentLimit()==0.
     * @param valuesLength The length of the values array.
     *                     Must be at least getArgumentLimit().
     * @param result Gets its contents replaced by the formatted pattern and values.
     * @param offsets offsets[i] receives the offset of where
     *                values[i] replaced pattern argument {i}.
     *                Can be shorter or longer than values. Can be nullptr if offsetsLength==0.
     *                If there is no {i} in the pattern, then offsets[i] is set to -1.
     * @param offsetsLength The length of the offsets array.
     * @param errorCode ICU error code in/out parameter.
     *                  Must fulfill U_SUCCESS before the function call.
     * @return result
     * @stable ICU 57
     */
    UnicodeString& formatAndReplace(
            स्थिर UnicodeString* स्थिर* मूल्यावली,
            int32_t मूल्यावली_लंबाई,
            UnicodeString& परिणाम U_LIFETIME_BOUND,
            int32_t* offsets,
            int32_t offsets_लंबाई,
            UErrorCode& errorCode) स्थिर;

    /**
     * Returns the pattern text with none of the arguments.
     * Like formatting with all-empty string values.
     * @stable ICU 57
     */
    UnicodeString getTextWithNoArguments() स्थिर {
        वापस getTextWithNoArguments(
            compiledPattern.get_बफर(),
            compiledPattern.लंबाई(),
            nullptr,
            0);
    }

#अगर_अघोषित U_HIDE_INTERNAL_API
    /**
     * Returns the pattern text with none of the arguments.
     * Like formatting with all-empty string values.
     *
     * TODO(ICU-20406): Replace this with an Iterator interface.
     *
     * @param offsets offsets[i] receives the offset of where {i} was located
     *                before it was replaced by an empty string.
     *                For example, "a{0}b{1}" produces offset 1 for i=0 and 2 for i=1.
     *                Can be nullptr if offsetsLength==0.
     *                If there is no {i} in the pattern, then offsets[i] is set to -1.
     * @param offsetsLength The length of the offsets array.
     *
     * @internal
     */
    UnicodeString getTextWithNoArguments(int32_t *offsets, int32_t offsets_लंबाई) स्थिर {
        वापस getTextWithNoArguments(
            compiledPattern.get_बफर(),
            compiledPattern.लंबाई(),
            offsets,
            offsets_लंबाई);
    }
#पूर्ण_अगर // U_HIDE_INTERNAL_API

निजी:
    /**
     * Binary representation of the compiled pattern.
     * Index 0: One more than the highest argument number.
     * Followed by zero or more arguments or literal-text segments.
     *
     * An argument is stored as its number, less than ARG_NUM_LIMIT.
     * A literal-text segment is stored as its length (at least 1) offset by ARG_NUM_LIMIT,
     * followed by that many chars.
     */
    UnicodeString compiledPattern;

    अटल अंतरभूत int32_t getArgumentLimit(स्थिर char16_t *compiledPattern,
                                              int32_t compiled_Pattern_लंबाई) {
        वापस compiled_Pattern_लंबाई == 0 ? 0 : compiledPattern[0];
    }

    अटल UnicodeString getTextWithNoArguments(
        स्थिर char16_t *compiledPattern,
        int32_t compiled_Pattern_लंबाई,
        int32_t *offsets,
        int32_t offsets_लंबाई);

    अटल UnicodeString& format(
            स्थिर char16_t* compiledPattern,
            int32_t compiled_Pattern_लंबाई,
            स्थिर UnicodeString* स्थिर* मूल्यावली,
            UnicodeString& परिणाम U_LIFETIME_BOUND,
            स्थिर UnicodeString* परिणाम_Copy,
            UBool forbid_परिणाम_As_मूल्य,
            int32_t* offsets,
            int32_t offsets_लंबाई,
            UErrorCode& errorCode);

    // Give access to internals to SimpleModifier for number formatting
    मित्र class संख्या::impl::SimpleModifier;
};

U_NAMESPACE_END

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */

#पूर्ण_अगर  // __SIMPLEFORMATTER_H__
