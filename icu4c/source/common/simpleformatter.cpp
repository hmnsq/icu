// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#include "unicode/utypes.h"
#include "unicode/simpleformatter.h"
#include "unicode/unistr.h"
#include "uassert.h"

U_NAMESPACE_BEGIN

namespace {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
const int32_t ARG_NUM_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
const char16_t SEGMENT_LENGTH_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
const int32_t MAX_SEGMENT_LENGTH = SEGMENT_LENGTH_PLACEHOLDER_CHAR - ARG_NUM_LIMIT;

ga_nanaa {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

anta_hstha UBool isInvalidArray(const void *array, int32_t length) {
   ཕྱིར (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::operator=(const SimpleFormatter& other) U_LIFETIME_BOUND {
    གལ་ཏེ (this == &other) {
        ཕྱིར *this;
    }
    compiledPattern = other.compiledPattern;
    ཕྱིར *this;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        const UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    གལ་ཏེ (U_FAILURE(errorCode)) {
        ཕྱིར false;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    const char16_t *patternBuffer = pattern.getBuffer();
    int32_t patternLength = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t textLength = 0;
    int32_t maxArg = -1;
    UBool inQuote = false;
    ཆེད་དུ (int32_t i = 0; i < patternLength;) {
        char16_t c = patternBuffer[i++];
        གལ་ཏེ (c == APOS) {
            གལ་ཏེ (i < patternLength && (c = patternBuffer[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } དེ་མིན གལ་ཏེ (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = false;
                continue;
            } དེ་མིན གལ་ཏེ (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = true;
            } དེ་མིན {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } དེ་མིན གལ་ཏེ (!inQuote && c == OPEN_BRACE) {
            གལ་ཏེ (textLength > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                          static_cast<char16_t>(ARG_NUM_LIMIT + textLength));
                textLength = 0;
            }
            int32_t argNumber;
            གལ་ཏེ ((i + 1) < patternLength &&
                    0 <= (argNumber = patternBuffer[i] - DIGIT_ZERO) && argNumber <= 9 &&
                    patternBuffer[i + 1] == CLOSE_BRACE) {
                i += 2;
            } དེ་མིན {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                argNumber = -1;
                གལ་ཏེ (i < patternLength && DIGIT_ONE <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                    argNumber = c - DIGIT_ZERO;
                    སྐབས (i < patternLength &&
                            DIGIT_ZERO <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                        argNumber = argNumber * 10 + (c - DIGIT_ZERO);
                        གལ་ཏེ (argNumber >= ARG_NUM_LIMIT) {
                            bha_nga;
                        }
                    }
                }
                གལ་ཏེ (argNumber < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ཕྱིར false;
                }
            }
            གལ་ཏེ (argNumber > maxArg) {
                maxArg = argNumber;
            }
            compiledPattern.append(static_cast<char16_t>(argNumber));
            continue;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        གལ་ཏེ (textLength == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_LENGTH_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        གལ་ཏེ (++textLength == MAX_SEGMENT_LENGTH) {
            textLength = 0;
        }
    }
    གལ་ཏེ (textLength > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                  static_cast<char16_t>(ARG_NUM_LIMIT + textLength));
    }
    int32_t argCount = maxArg + 1;
    གལ་ཏེ (argCount < min || max < argCount) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ཕྱིར false;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(argCount));
    ཕྱིར true;
}

UnicodeString& SimpleFormatter::format(
        const UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) const {
    const UnicodeString *values[] = { &value0 };
    ཕྱིར formatAndAppend(values, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        const UnicodeString& value0,
        const UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) const {
    const UnicodeString *values[] = { &value0, &value1 };
    ཕྱིར formatAndAppend(values, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        const UnicodeString& value0,
        const UnicodeString& value1,
        const UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) const {
    const UnicodeString *values[] = { &value0, &value1, &value2 };
    ཕྱིར formatAndAppend(values, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        const UnicodeString* const* values,
        int32_t valuesLength,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) const {
    གལ་ཏེ (U_FAILURE(errorCode)) {
        ཕྱིར appendTo;
    }
    གལ་ཏེ (isInvalidArray(values, valuesLength) || isInvalidArray(offsets, offsetsLength) ||
            valuesLength < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ཕྱིར appendTo;
    }
    ཕྱིར format(compiledPattern.getBuffer(), compiledPattern.length(), values,
                  appendTo, nullptr, true,
                  offsets, offsetsLength, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        const UnicodeString* const* values,
        int32_t valuesLength,
        UnicodeString& result U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) const {
    གལ་ཏེ (U_FAILURE(errorCode)) {
        ཕྱིར result;
    }
    གལ་ཏེ (isInvalidArray(values, valuesLength) || isInvalidArray(offsets, offsetsLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ཕྱིར result;
    }
    const char16_t *cp = compiledPattern.getBuffer();
    int32_t cpLength = compiledPattern.length();
    གལ་ཏེ (valuesLength < getArgumentLimit(cp, cpLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ཕྱིར result;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString resultCopy;
    གལ་ཏེ (getArgumentLimit(cp, cpLength) > 0) {
        ཆེད་དུ (int32_t i = 1; i < cpLength;) {
            int32_t n = cp[i++];
            གལ་ཏེ (n < ARG_NUM_LIMIT) {
                གལ་ཏེ (values[n] == &result) {
                    གལ་ཏེ (i == 2) {
                        firstArg = n;
                    } དེ་མིན གལ་ཏེ (resultCopy.isEmpty() && !result.isEmpty()) {
                        resultCopy = result;
                    }
                }
            } དེ་མིན {
                i += n - ARG_NUM_LIMIT;
            }
        }
    }
    གལ་ཏེ (firstArg < 0) {
        result.remove();
    }
    ཕྱིར format(cp, cpLength, values,
                  result, &resultCopy, false,
                  offsets, offsetsLength, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        const char16_t *compiledPattern,
        int32_t compiledPatternLength,
        int32_t* offsets,
        int32_t offsetsLength) {
    ཆེད་དུ (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiledPatternLength - 1 -
            getArgumentLimit(compiledPattern, compiledPatternLength);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    ཆེད་དུ (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        གལ་ཏེ (n > ARG_NUM_LIMIT) {
            n -= ARG_NUM_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } དེ་མིན གལ་ཏེ (n < offsetsLength) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    ཕྱིར sb;
}

UnicodeString& SimpleFormatter::format(
        const char16_t* compiledPattern,
        int32_t compiledPatternLength,
        const UnicodeString* const* values,
        UnicodeString& result U_LIFETIME_BOUND,
        const UnicodeString* resultCopy,
        UBool forbidResultAsValue,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) {
    གལ་ཏེ (U_FAILURE(errorCode)) {
        ཕྱིར result;
    }
    ཆེད་དུ (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    ཆེད་དུ (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        གལ་ཏེ (n < ARG_NUM_LIMIT) {
            const UnicodeString *value = values[n];
            གལ་ཏེ (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ཕྱིར result;
            }
            གལ་ཏེ (value == &result) {
                གལ་ཏེ (forbidResultAsValue) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ཕྱིར result;
                }
                གལ་ཏེ (i == 2) {
                    // We are appending to result which is also the first value object.
                    གལ་ཏེ (n < offsetsLength) {
                        offsets[n] = 0;
                    }
                } དེ་མིན {
                    གལ་ཏེ (n < offsetsLength) {
                        offsets[n] = result.length();
                    }
                    result.append(*resultCopy);
                }
            } དེ་མིན {
                གལ་ཏེ (n < offsetsLength) {
                    offsets[n] = result.length();
                }
                result.append(*value);
            }
        } དེ་མིན {
            int32_t length = n - ARG_NUM_LIMIT;
            result.append(compiledPattern + i, length);
            i += length;
        }
    }
    ཕྱིར result;
}

U_NAMESPACE_END
