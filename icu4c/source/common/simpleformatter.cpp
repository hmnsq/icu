// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/simpleformatter.h"
#ضمن "unicode/unistr.h"
#ضمن "uassert.h"

U_NAMESPACE_BEGIN

نطاق {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
ثابت int32_t ARG_NUM_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ثابت char16_t SEGMENT_LENGTH_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ثابت int32_t MAX_SEGMENT_LENGTH = SEGMENT_LENGTH_PLACEHOLDER_CHAR - ARG_NUM_LIMIT;

تعداد {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

مضمن UBool isInvalidArray(ثابت فراغ *array, int32_t length) {
   ܦܢܐ (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::operator=(ثابت SimpleFormatter& other) U_LIFETIME_BOUND {
    ܐܢ (هذا == &other) {
        ܦܢܐ *هذا;
    }
    compiledPattern = other.compiledPattern;
    ܦܢܐ *هذا;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        ثابت UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    ܐܢ (U_FAILURE(errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ثابت char16_t *patternBuffer = pattern.getBuffer();
    int32_t patternLength = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t textLength = 0;
    int32_t maxArg = -1;
    UBool inQuote = ܕܓܠܐ;
    لكل (int32_t i = 0; i < patternLength;) {
        char16_t c = patternBuffer[i++];
        ܐܢ (c == APOS) {
            ܐܢ (i < patternLength && (c = patternBuffer[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } ܐܚܪܢܐ ܐܢ (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = ܕܓܠܐ;
                استمر;
            } ܐܚܪܢܐ ܐܢ (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = ܫܪܪܐ;
            } ܐܚܪܢܐ {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } ܐܚܪܢܐ ܐܢ (!inQuote && c == OPEN_BRACE) {
            ܐܢ (textLength > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                          static_cast<char16_t>(ARG_NUM_LIMIT + textLength));
                textLength = 0;
            }
            int32_t argNumber;
            ܐܢ ((i + 1) < patternLength &&
                    0 <= (argNumber = patternBuffer[i] - DIGIT_ZERO) && argNumber <= 9 &&
                    patternBuffer[i + 1] == CLOSE_BRACE) {
                i += 2;
            } ܐܚܪܢܐ {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                argNumber = -1;
                ܐܢ (i < patternLength && DIGIT_ONE <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                    argNumber = c - DIGIT_ZERO;
                    ܟܡܐ (i < patternLength &&
                            DIGIT_ZERO <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                        argNumber = argNumber * 10 + (c - DIGIT_ZERO);
                        ܐܢ (argNumber >= ARG_NUM_LIMIT) {
                            اكسر;
                        }
                    }
                }
                ܐܢ (argNumber < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ܦܢܐ ܕܓܠܐ;
                }
            }
            ܐܢ (argNumber > maxArg) {
                maxArg = argNumber;
            }
            compiledPattern.append(static_cast<char16_t>(argNumber));
            استمر;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        ܐܢ (textLength == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_LENGTH_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        ܐܢ (++textLength == MAX_SEGMENT_LENGTH) {
            textLength = 0;
        }
    }
    ܐܢ (textLength > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                  static_cast<char16_t>(ARG_NUM_LIMIT + textLength));
    }
    int32_t argCount = maxArg + 1;
    ܐܢ (argCount < min || max < argCount) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ ܕܓܠܐ;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(argCount));
    ܦܢܐ ܫܪܪܐ;
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ثابت {
    ثابت UnicodeString *values[] = { &value0 };
    ܦܢܐ formatAndAppend(values, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ثابت {
    ثابت UnicodeString *values[] = { &value0, &value1 };
    ܦܢܐ formatAndAppend(values, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        ثابت UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ثابت {
    ثابت UnicodeString *values[] = { &value0, &value1, &value2 };
    ܦܢܐ formatAndAppend(values, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ثابت UnicodeString* ثابت* values,
        int32_t valuesLength,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ثابت {
    ܐܢ (U_FAILURE(errorCode)) {
        ܦܢܐ appendTo;
    }
    ܐܢ (isInvalidArray(values, valuesLength) || isInvalidArray(offsets, offsetsLength) ||
            valuesLength < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ appendTo;
    }
    ܦܢܐ format(compiledPattern.getBuffer(), compiledPattern.length(), values,
                  appendTo, nullptr, ܫܪܪܐ,
                  offsets, offsetsLength, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ثابت UnicodeString* ثابت* values,
        int32_t valuesLength,
        UnicodeString& result U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ثابت {
    ܐܢ (U_FAILURE(errorCode)) {
        ܦܢܐ result;
    }
    ܐܢ (isInvalidArray(values, valuesLength) || isInvalidArray(offsets, offsetsLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ result;
    }
    ثابت char16_t *cp = compiledPattern.getBuffer();
    int32_t cpLength = compiledPattern.length();
    ܐܢ (valuesLength < getArgumentLimit(cp, cpLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ result;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString resultCopy;
    ܐܢ (getArgumentLimit(cp, cpLength) > 0) {
        لكل (int32_t i = 1; i < cpLength;) {
            int32_t n = cp[i++];
            ܐܢ (n < ARG_NUM_LIMIT) {
                ܐܢ (values[n] == &result) {
                    ܐܢ (i == 2) {
                        firstArg = n;
                    } ܐܚܪܢܐ ܐܢ (resultCopy.isEmpty() && !result.isEmpty()) {
                        resultCopy = result;
                    }
                }
            } ܐܚܪܢܐ {
                i += n - ARG_NUM_LIMIT;
            }
        }
    }
    ܐܢ (firstArg < 0) {
        result.remove();
    }
    ܦܢܐ format(cp, cpLength, values,
                  result, &resultCopy, ܕܓܠܐ,
                  offsets, offsetsLength, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        ثابت char16_t *compiledPattern,
        int32_t compiledPatternLength,
        int32_t* offsets,
        int32_t offsetsLength) {
    لكل (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiledPatternLength - 1 -
            getArgumentLimit(compiledPattern, compiledPatternLength);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    لكل (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        ܐܢ (n > ARG_NUM_LIMIT) {
            n -= ARG_NUM_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } ܐܚܪܢܐ ܐܢ (n < offsetsLength) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    ܦܢܐ sb;
}

UnicodeString& SimpleFormatter::format(
        ثابت char16_t* compiledPattern,
        int32_t compiledPatternLength,
        ثابت UnicodeString* ثابت* values,
        UnicodeString& result U_LIFETIME_BOUND,
        ثابت UnicodeString* resultCopy,
        UBool forbidResultAsValue,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) {
    ܐܢ (U_FAILURE(errorCode)) {
        ܦܢܐ result;
    }
    لكل (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    لكل (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        ܐܢ (n < ARG_NUM_LIMIT) {
            ثابت UnicodeString *value = values[n];
            ܐܢ (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ܦܢܐ result;
            }
            ܐܢ (value == &result) {
                ܐܢ (forbidResultAsValue) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ܦܢܐ result;
                }
                ܐܢ (i == 2) {
                    // We are appending to result which is also the first value object.
                    ܐܢ (n < offsetsLength) {
                        offsets[n] = 0;
                    }
                } ܐܚܪܢܐ {
                    ܐܢ (n < offsetsLength) {
                        offsets[n] = result.length();
                    }
                    result.append(*resultCopy);
                }
            } ܐܚܪܢܐ {
                ܐܢ (n < offsetsLength) {
                    offsets[n] = result.length();
                }
                result.append(*value);
            }
        } ܐܚܪܢܐ {
            int32_t length = n - ARG_NUM_LIMIT;
            result.append(compiledPattern + i, length);
            i += length;
        }
    }
    ܦܢܐ result;
}

U_NAMESPACE_END
