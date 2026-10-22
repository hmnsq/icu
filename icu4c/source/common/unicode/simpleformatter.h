// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.h
*/

#यदि_अपरिभाषितम् __SIMPLEFORMATTER_H__
#परिभाषय __SIMPLEFORMATTER_H__

/**
 * \file
 * \brief C++ API: Simple formatter, minimal subset of MessageFormat.
 */

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API

#अन्तर्भाव "unicode/unistr.h"

U_NAMESPACE_BEGIN

// Forward declaration:
namespace सङ्ख्या::impl {
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
class U_COMMON_API SimpleFormatter final : सार्वजनिक UMemory {
सार्वजनिक:
    /**
     * Default constructor.
     * @stable ICU 57
     */
    SimpleFormatter() : compiled_आदर्शः(static_cast<char16_t>(0)) {}

    /**
     * Constructs a formatter from the pattern string.
     *
     * @param pattern The pattern string.
     * @param errorCode ICU error code in/out parameter.
     *                  Must fulfill U_SUCCESS before the function call.
     *                  Set to U_ILLEGAL_ARGUMENT_ERROR for bad argument syntax.
     * @stable ICU 57
     */
    SimpleFormatter(ध्रुव UnicodeString& आदर्शः, UError_संहिता &error_संहिता) {
        apply_आदर्शः(आदर्शः, error_संहिता);
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
    SimpleFormatter(ध्रुव UnicodeString& आदर्शः, int32_t min, int32_t max,
                    UError_संहिता &error_संहिता) {
        apply_आदर्शः_Min_Max_Arguments(आदर्शः, min, max, error_संहिता);
    }

    /**
     * Copy constructor.
     * @stable ICU 57
     */
    SimpleFormatter(ध्रुव SimpleFormatter& other)
            : compiled_आदर्शः(other.compiled_आदर्शः) {}

    /**
     * Assignment operator.
     * @stable ICU 57
     */
    SimpleFormatter& संक्रियकः=(ध्रुव SimpleFormatter& other) U_LIFETIME_BOUND;

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
    UBool apply_आदर्शः(ध्रुव UnicodeString &आदर्शः, UError_संहिता &error_संहिता) {
        प्रत्यागम apply_आदर्शः_Min_Max_Arguments(आदर्शः, 0, INT32_MAX, error_संहिता);
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
    UBool apply_आदर्शः_Min_Max_Arguments(ध्रुव UnicodeString &आदर्शः,
                                      int32_t min, int32_t max, UError_संहिता &error_संहिता);

    /**
     * @return The max argument number + 1.
     * @stable ICU 57
     */
    int32_t get_तर्कः_सीमा() ध्रुव {
        प्रत्यागम get_तर्कः_सीमा(compiled_आदर्शः.get_बफर(), compiled_आदर्शः.दीर्घता());
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
            ध्रुव UnicodeString& value0,
            UnicodeString& append_गन्तव्यम् U_LIFETIME_BOUND,
            UError_संहिता& error_संहिता) ध्रुव;

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
            ध्रुव UnicodeString& value0,
            ध्रुव UnicodeString& value1,
            UnicodeString& append_गन्तव्यम् U_LIFETIME_BOUND,
            UError_संहिता& error_संहिता) ध्रुव;

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
            ध्रुव UnicodeString& value0,
            ध्रुव UnicodeString& value1,
            ध्रुव UnicodeString& value2,
            UnicodeString& append_गन्तव्यम् U_LIFETIME_BOUND,
            UError_संहिता& error_संहिता) ध्रुव;

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
            ध्रुव UnicodeString* ध्रुव* मानानि,
            int32_t मानानि_दीर्घता,
            UnicodeString& append_गन्तव्यम् U_LIFETIME_BOUND,
            int32_t* offsets,
            int32_t offsets_दीर्घता,
            UError_संहिता& error_संहिता) ध्रुव;

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
            ध्रुव UnicodeString* ध्रुव* मानानि,
            int32_t मानानि_दीर्घता,
            UnicodeString& परिणाम U_LIFETIME_BOUND,
            int32_t* offsets,
            int32_t offsets_दीर्घता,
            UError_संहिता& error_संहिता) ध्रुव;

    /**
     * Returns the pattern text with none of the arguments.
     * Like formatting with all-empty string values.
     * @stable ICU 57
     */
    UnicodeString getTextWithNoArguments() ध्रुव {
        प्रत्यागम getTextWithNoArguments(
            compiled_आदर्शः.get_बफर(),
            compiled_आदर्शः.दीर्घता(),
            nullptr,
            0);
    }

#यदि_अपरिभाषितम् U_HIDE_आन्तरिकम्_API
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
    UnicodeString getTextWithNoArguments(int32_t *offsets, int32_t offsets_दीर्घता) ध्रुव {
        प्रत्यागम getTextWithNoArguments(
            compiled_आदर्शः.get_बफर(),
            compiled_आदर्शः.दीर्घता(),
            offsets,
            offsets_दीर्घता);
    }
#यदि_अन्तः // U_HIDE_INTERNAL_API

गुप्त:
    /**
     * Binary representation of the compiled pattern.
     * Index 0: One more than the highest argument number.
     * Followed by zero or more arguments or literal-text segments.
     *
     * An argument is stored as its number, less than ARG_NUM_LIMIT.
     * A literal-text segment is stored as its length (at least 1) offset by ARG_NUM_LIMIT,
     * followed by that many chars.
     */
    UnicodeString compiled_आदर्शः;

    स्थिर अन्तःस्थ int32_t get_तर्कः_सीमा(ध्रुव char16_t *compiled_आदर्शः,
                                              int32_t compiled_आदर्शः_दीर्घता) {
        प्रत्यागम compiled_आदर्शः_दीर्घता == 0 ? 0 : compiled_आदर्शः[0];
    }

    स्थिर UnicodeString getTextWithNoArguments(
        ध्रुव char16_t *compiled_आदर्शः,
        int32_t compiled_आदर्शः_दीर्घता,
        int32_t *offsets,
        int32_t offsets_दीर्घता);

    स्थिर UnicodeString& format(
            ध्रुव char16_t* compiled_आदर्शः,
            int32_t compiled_आदर्शः_दीर्घता,
            ध्रुव UnicodeString* ध्रुव* मानानि,
            UnicodeString& परिणाम U_LIFETIME_BOUND,
            ध्रुव UnicodeString* परिणाम_Copy,
            UBool forbid_परिणाम_As_मानम्,
            int32_t* offsets,
            int32_t offsets_दीर्घता,
            UError_संहिता& error_संहिता);

    // Give access to internals to SimpleModifier for number formatting
    मित्रम् class सङ्ख्या::impl::SimpleModifier;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः /* U_SHOW_CPLUSPLUS_API */

#यदि_अन्तः  // __SIMPLEFORMATTER_H__
