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
ثابت int32_t ARG_اڠک_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ثابت char16_t SEGMENT_LENGTH_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ثابت int32_t MAX_SEGMENT_LENGTH = SEGMENT_LENGTH_PLACEHOLDER_CHAR - ARG_اڠک_LIMIT;

تعداد {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

مضمن UBool isInvalidArray(ثابت فراغ *array, int32_t length) {
   أرجع (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::operator=(ثابت SimpleFormatter& other) U_LIFETIME_BOUND {
    إذا (هذا == &other) {
        أرجع *هذا;
    }
    compiledPattern = other.compiledPattern;
    أرجع *هذا;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        ثابت UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode)) {
        أرجع خطأ;
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
    UBool inQuote = خطأ;
    لكل (int32_t i = 0; i < patternLength;) {
        char16_t c = patternBuffer[i++];
        إذا (c == APOS) {
            إذا (i < patternLength && (c = patternBuffer[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } وإلا إذا (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = خطأ;
                استمر;
            } وإلا إذا (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = true;
            } وإلا {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } وإلا إذا (!inQuote && c == OPEN_BRACE) {
            إذا (textLength > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                          static_cast<char16_t>(ARG_اڠک_LIMIT + textLength));
                textLength = 0;
            }
            int32_t arg_نومبور;
            إذا ((i + 1) < patternLength &&
                    0 <= (arg_نومبور = patternBuffer[i] - DIGIT_ZERO) && arg_نومبور <= 9 &&
                    patternBuffer[i + 1] == CLOSE_BRACE) {
                i += 2;
            } وإلا {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_نومبور = -1;
                إذا (i < patternLength && DIGIT_ONE <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                    arg_نومبور = c - DIGIT_ZERO;
                    طالما (i < patternLength &&
                            DIGIT_ZERO <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                        arg_نومبور = arg_نومبور * 10 + (c - DIGIT_ZERO);
                        إذا (arg_نومبور >= ARG_اڠک_LIMIT) {
                            اكسر;
                        }
                    }
                }
                إذا (arg_نومبور < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    أرجع خطأ;
                }
            }
            إذا (arg_نومبور > maxArg) {
                maxArg = arg_نومبور;
            }
            compiledPattern.append(static_cast<char16_t>(arg_نومبور));
            استمر;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        إذا (textLength == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_LENGTH_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        إذا (++textLength == MAX_SEGMENT_LENGTH) {
            textLength = 0;
        }
    }
    إذا (textLength > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                  static_cast<char16_t>(ARG_اڠک_LIMIT + textLength));
    }
    int32_t arg_کيراءن = maxArg + 1;
    إذا (arg_کيراءن < min || max < arg_کيراءن) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خطأ;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(arg_کيراءن));
    أرجع true;
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ثابت {
    ثابت UnicodeString *نيلاي_سموا[] = { &value0 };
    أرجع formatAndAppend(نيلاي_سموا, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ثابت {
    ثابت UnicodeString *نيلاي_سموا[] = { &value0, &value1 };
    أرجع formatAndAppend(نيلاي_سموا, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ثابت UnicodeString& value0,
        ثابت UnicodeString& value1,
        ثابت UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ثابت {
    ثابت UnicodeString *نيلاي_سموا[] = { &value0, &value1, &value2 };
    أرجع formatAndAppend(نيلاي_سموا, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ثابت UnicodeString* ثابت* نيلاي_سموا,
        int32_t نيلاي_سموا_Length,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ثابت {
    إذا (U_FAILURE(errorCode)) {
        أرجع appendTo;
    }
    إذا (isInvalidArray(نيلاي_سموا, نيلاي_سموا_Length) || isInvalidArray(offsets, offsetsLength) ||
            نيلاي_سموا_Length < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع appendTo;
    }
    أرجع format(compiledPattern.getBuffer(), compiledPattern.length(), نيلاي_سموا,
                  appendTo, nullptr, true,
                  offsets, offsetsLength, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ثابت UnicodeString* ثابت* نيلاي_سموا,
        int32_t نيلاي_سموا_Length,
        UnicodeString& result U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ثابت {
    إذا (U_FAILURE(errorCode)) {
        أرجع result;
    }
    إذا (isInvalidArray(نيلاي_سموا, نيلاي_سموا_Length) || isInvalidArray(offsets, offsetsLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع result;
    }
    ثابت char16_t *cp = compiledPattern.getBuffer();
    int32_t cpLength = compiledPattern.length();
    إذا (نيلاي_سموا_Length < getArgumentLimit(cp, cpLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع result;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString resultCopy;
    إذا (getArgumentLimit(cp, cpLength) > 0) {
        لكل (int32_t i = 1; i < cpLength;) {
            int32_t n = cp[i++];
            إذا (n < ARG_اڠک_LIMIT) {
                إذا (نيلاي_سموا[n] == &result) {
                    إذا (i == 2) {
                        firstArg = n;
                    } وإلا إذا (resultCopy.isEmpty() && !result.isEmpty()) {
                        resultCopy = result;
                    }
                }
            } وإلا {
                i += n - ARG_اڠک_LIMIT;
            }
        }
    }
    إذا (firstArg < 0) {
        result.remove();
    }
    أرجع format(cp, cpLength, نيلاي_سموا,
                  result, &resultCopy, خطأ,
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
        إذا (n > ARG_اڠک_LIMIT) {
            n -= ARG_اڠک_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } وإلا إذا (n < offsetsLength) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    أرجع sb;
}

UnicodeString& SimpleFormatter::format(
        ثابت char16_t* compiledPattern,
        int32_t compiledPatternLength,
        ثابت UnicodeString* ثابت* نيلاي_سموا,
        UnicodeString& result U_LIFETIME_BOUND,
        ثابت UnicodeString* resultCopy,
        UBool forbid_Result_As_نيلاي,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) {
    إذا (U_FAILURE(errorCode)) {
        أرجع result;
    }
    لكل (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    لكل (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        إذا (n < ARG_اڠک_LIMIT) {
            ثابت UnicodeString *value = نيلاي_سموا[n];
            إذا (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                أرجع result;
            }
            إذا (value == &result) {
                إذا (forbid_Result_As_نيلاي) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    أرجع result;
                }
                إذا (i == 2) {
                    // We are appending to result which is also the first value object.
                    إذا (n < offsetsLength) {
                        offsets[n] = 0;
                    }
                } وإلا {
                    إذا (n < offsetsLength) {
                        offsets[n] = result.length();
                    }
                    result.append(*resultCopy);
                }
            } وإلا {
                إذا (n < offsetsLength) {
                    offsets[n] = result.length();
                }
                result.append(*value);
            }
        } وإلا {
            int32_t length = n - ARG_اڠک_LIMIT;
            result.append(compiledPattern + i, length);
            i += length;
        }
    }
    أرجع result;
}

U_NAMESPACE_END
