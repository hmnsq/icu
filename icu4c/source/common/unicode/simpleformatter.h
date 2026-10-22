// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.h
*/

#నిర్వచితం_కాకపోతే __SIMPLEFORMATTER_H__
#నిర్వచించు __SIMPLEFORMATTER_H__

/**
 * \file
 * \brief C++ API: Simple formatter, minimal subset of MessageFormat.
 */

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ "unicode/unistr.h"

U_NAMESPACE_BEGIN

// Forward declaration:
namespace సంఖ్య::impl {
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
class U_COMMON_API SimpleFormatter final : బహిరంగ UMemory {
బహిరంగ:
    /**
     * Default constructor.
     * @stable ICU 57
     */
    SimpleFormatter() : compiled_నమూనా(static_cast<char16_t>(0)) {}

    /**
     * Constructs a formatter from the pattern string.
     *
     * @param pattern The pattern string.
     * @param errorCode ICU error code in/out parameter.
     *                  Must fulfill U_SUCCESS before the function call.
     *                  Set to U_ILLEGAL_ARGUMENT_ERROR for bad argument syntax.
     * @stable ICU 57
     */
    SimpleFormatter(స్థిరరాశి UnicodeString& నమూనా, UError_సంకేతం &error_సంకేతం) {
        apply_నమూనా(నమూనా, error_సంకేతం);
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
    SimpleFormatter(స్థిరరాశి UnicodeString& నమూనా, int32_t min, int32_t max,
                    UError_సంకేతం &error_సంకేతం) {
        apply_నమూనా_Min_Max_Arguments(నమూనా, min, max, error_సంకేతం);
    }

    /**
     * Copy constructor.
     * @stable ICU 57
     */
    SimpleFormatter(స్థిరరాశి SimpleFormatter& other)
            : compiled_నమూనా(other.compiled_నమూనా) {}

    /**
     * Assignment operator.
     * @stable ICU 57
     */
    SimpleFormatter& క్రియాకారకం=(స్థిరరాశి SimpleFormatter& other) U_LIFETIME_BOUND;

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
    UBool apply_నమూనా(స్థిరరాశి UnicodeString &నమూనా, UError_సంకేతం &error_సంకేతం) {
        తిరిగి apply_నమూనా_Min_Max_Arguments(నమూనా, 0, INT32_MAX, error_సంకేతం);
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
    UBool apply_నమూనా_Min_Max_Arguments(స్థిరరాశి UnicodeString &నమూనా,
                                      int32_t min, int32_t max, UError_సంకేతం &error_సంకేతం);

    /**
     * @return The max argument number + 1.
     * @stable ICU 57
     */
    int32_t get_వాదం_హద్దు() స్థిరరాశి {
        తిరిగి get_వాదం_హద్దు(compiled_నమూనా.get_బఫరు(), compiled_నమూనా.నిడివి());
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
            స్థిరరాశి UnicodeString& value0,
            UnicodeString& append_గమ్యం U_LIFETIME_BOUND,
            UError_సంకేతం& error_సంకేతం) స్థిరరాశి;

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
            స్థిరరాశి UnicodeString& value0,
            స్థిరరాశి UnicodeString& value1,
            UnicodeString& append_గమ్యం U_LIFETIME_BOUND,
            UError_సంకేతం& error_సంకేతం) స్థిరరాశి;

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
            స్థిరరాశి UnicodeString& value0,
            స్థిరరాశి UnicodeString& value1,
            స్థిరరాశి UnicodeString& value2,
            UnicodeString& append_గమ్యం U_LIFETIME_BOUND,
            UError_సంకేతం& error_సంకేతం) స్థిరరాశి;

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
            స్థిరరాశి UnicodeString* స్థిరరాశి* విలువలు,
            int32_t విలువలు_నిడివి,
            UnicodeString& append_గమ్యం U_LIFETIME_BOUND,
            int32_t* offsets,
            int32_t offsets_నిడివి,
            UError_సంకేతం& error_సంకేతం) స్థిరరాశి;

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
            స్థిరరాశి UnicodeString* స్థిరరాశి* విలువలు,
            int32_t విలువలు_నిడివి,
            UnicodeString& ఫలితం U_LIFETIME_BOUND,
            int32_t* offsets,
            int32_t offsets_నిడివి,
            UError_సంకేతం& error_సంకేతం) స్థిరరాశి;

    /**
     * Returns the pattern text with none of the arguments.
     * Like formatting with all-empty string values.
     * @stable ICU 57
     */
    UnicodeString get_పాఠ్యం_With_No_Arguments() స్థిరరాశి {
        తిరిగి get_పాఠ్యం_With_No_Arguments(
            compiled_నమూనా.get_బఫరు(),
            compiled_నమూనా.నిడివి(),
            nullptr,
            0);
    }

#నిర్వచితం_కాకపోతే U_HIDE_అంతర్గత_API
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
    UnicodeString get_పాఠ్యం_With_No_Arguments(int32_t *offsets, int32_t offsets_నిడివి) స్థిరరాశి {
        తిరిగి get_పాఠ్యం_With_No_Arguments(
            compiled_నమూనా.get_బఫరు(),
            compiled_నమూనా.నిడివి(),
            offsets,
            offsets_నిడివి);
    }
#అయితే_ముగింపు // U_HIDE_INTERNAL_API

అంతరంగ:
    /**
     * Binary representation of the compiled pattern.
     * Index 0: One more than the highest argument number.
     * Followed by zero or more arguments or literal-text segments.
     *
     * An argument is stored as its number, less than ARG_NUM_LIMIT.
     * A literal-text segment is stored as its length (at least 1) offset by ARG_NUM_LIMIT,
     * followed by that many chars.
     */
    UnicodeString compiled_నమూనా;

    స్థిర అంతఃవరుస int32_t get_వాదం_హద్దు(స్థిరరాశి char16_t *compiled_నమూనా,
                                              int32_t compiled_నమూనా_నిడివి) {
        తిరిగి compiled_నమూనా_నిడివి == 0 ? 0 : compiled_నమూనా[0];
    }

    స్థిర UnicodeString get_పాఠ్యం_With_No_Arguments(
        స్థిరరాశి char16_t *compiled_నమూనా,
        int32_t compiled_నమూనా_నిడివి,
        int32_t *offsets,
        int32_t offsets_నిడివి);

    స్థిర UnicodeString& format(
            స్థిరరాశి char16_t* compiled_నమూనా,
            int32_t compiled_నమూనా_నిడివి,
            స్థిరరాశి UnicodeString* స్థిరరాశి* విలువలు,
            UnicodeString& ఫలితం U_LIFETIME_BOUND,
            స్థిరరాశి UnicodeString* ఫలితం_Copy,
            UBool forbid_ఫలితం_As_విలువ,
            int32_t* offsets,
            int32_t offsets_నిడివి,
            UError_సంకేతం& error_సంకేతం);

    // Give access to internals to SimpleModifier for number formatting
    మిత్ర class సంఖ్య::impl::SimpleModifier;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు  // __SIMPLEFORMATTER_H__
