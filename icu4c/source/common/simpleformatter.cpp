// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/simpleformatter.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "uassert.h"

U_NAMESPACE_BEGIN

நாமாகாஶ {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
மாறிலி int32_t ARG_இலக்கம்_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
மாறிலி char16_t SEGMENT_LENGTH_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
மாறிலி int32_t MAX_SEGMENT_LENGTH = SEGMENT_LENGTH_PLACEHOLDER_CHAR - ARG_இலக்கம்_LIMIT;

எண்ணிக்கை {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

உள்வரி UBool isInvalidArray(மாறிலி வெறுமை *array, int32_t length) {
   திரும்பு (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::operator=(மாறிலி SimpleFormatter& other) U_LIFETIME_BOUND {
    எனில் (இது == &other) {
        திரும்பு *இது;
    }
    compiledPattern = other.compiledPattern;
    திரும்பு *இது;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        மாறிலி UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு பொய்;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    மாறிலி char16_t *patternBuffer = pattern.getBuffer();
    int32_t patternLength = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t textLength = 0;
    int32_t maxArg = -1;
    UBool inQuote = பொய்;
    க்கு (int32_t i = 0; i < patternLength;) {
        char16_t c = patternBuffer[i++];
        எனில் (c == APOS) {
            எனில் (i < patternLength && (c = patternBuffer[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } இல்லையெனில் எனில் (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = பொய்;
                தொடர்;
            } இல்லையெனில் எனில் (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = உண்மை;
            } இல்லையெனில் {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } இல்லையெனில் எனில் (!inQuote && c == OPEN_BRACE) {
            எனில் (textLength > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                          static_cast<char16_t>(ARG_இலக்கம்_LIMIT + textLength));
                textLength = 0;
            }
            int32_t arg_எண்;
            எனில் ((i + 1) < patternLength &&
                    0 <= (arg_எண் = patternBuffer[i] - DIGIT_ZERO) && arg_எண் <= 9 &&
                    patternBuffer[i + 1] == CLOSE_BRACE) {
                i += 2;
            } இல்லையெனில் {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_எண் = -1;
                எனில் (i < patternLength && DIGIT_ONE <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                    arg_எண் = c - DIGIT_ZERO;
                    போது (i < patternLength &&
                            DIGIT_ZERO <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                        arg_எண் = arg_எண் * 10 + (c - DIGIT_ZERO);
                        எனில் (arg_எண் >= ARG_இலக்கம்_LIMIT) {
                            நிறுத்து;
                        }
                    }
                }
                எனில் (arg_எண் < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    திரும்பு பொய்;
                }
            }
            எனில் (arg_எண் > maxArg) {
                maxArg = arg_எண்;
            }
            compiledPattern.append(static_cast<char16_t>(arg_எண்));
            தொடர்;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        எனில் (textLength == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_LENGTH_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        எனில் (++textLength == MAX_SEGMENT_LENGTH) {
            textLength = 0;
        }
    }
    எனில் (textLength > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                  static_cast<char16_t>(ARG_இலக்கம்_LIMIT + textLength));
    }
    int32_t arg_மொத்தம் = maxArg + 1;
    எனில் (arg_மொத்தம் < min || max < arg_மொத்தம்) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு பொய்;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(arg_மொத்தம்));
    திரும்பு உண்மை;
}

UnicodeString& SimpleFormatter::format(
        மாறிலி UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) மாறிலி {
    மாறிலி UnicodeString *மதிப்புகள்[] = { &value0 };
    திரும்பு formatAndAppend(மதிப்புகள், 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        மாறிலி UnicodeString& value0,
        மாறிலி UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) மாறிலி {
    மாறிலி UnicodeString *மதிப்புகள்[] = { &value0, &value1 };
    திரும்பு formatAndAppend(மதிப்புகள், 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        மாறிலி UnicodeString& value0,
        மாறிலி UnicodeString& value1,
        மாறிலி UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) மாறிலி {
    மாறிலி UnicodeString *மதிப்புகள்[] = { &value0, &value1, &value2 };
    திரும்பு formatAndAppend(மதிப்புகள், 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        மாறிலி UnicodeString* மாறிலி* மதிப்புகள்,
        int32_t மதிப்புகள்_Length,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு appendTo;
    }
    எனில் (isInvalidArray(மதிப்புகள், மதிப்புகள்_Length) || isInvalidArray(offsets, offsetsLength) ||
            மதிப்புகள்_Length < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு appendTo;
    }
    திரும்பு format(compiledPattern.getBuffer(), compiledPattern.length(), மதிப்புகள்,
                  appendTo, nullptr, உண்மை,
                  offsets, offsetsLength, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        மாறிலி UnicodeString* மாறிலி* மதிப்புகள்,
        int32_t மதிப்புகள்_Length,
        UnicodeString& result U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு result;
    }
    எனில் (isInvalidArray(மதிப்புகள், மதிப்புகள்_Length) || isInvalidArray(offsets, offsetsLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு result;
    }
    மாறிலி char16_t *cp = compiledPattern.getBuffer();
    int32_t cpLength = compiledPattern.length();
    எனில் (மதிப்புகள்_Length < getArgumentLimit(cp, cpLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு result;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString resultCopy;
    எனில் (getArgumentLimit(cp, cpLength) > 0) {
        க்கு (int32_t i = 1; i < cpLength;) {
            int32_t n = cp[i++];
            எனில் (n < ARG_இலக்கம்_LIMIT) {
                எனில் (மதிப்புகள்[n] == &result) {
                    எனில் (i == 2) {
                        firstArg = n;
                    } இல்லையெனில் எனில் (resultCopy.isEmpty() && !result.isEmpty()) {
                        resultCopy = result;
                    }
                }
            } இல்லையெனில் {
                i += n - ARG_இலக்கம்_LIMIT;
            }
        }
    }
    எனில் (firstArg < 0) {
        result.remove();
    }
    திரும்பு format(cp, cpLength, மதிப்புகள்,
                  result, &resultCopy, பொய்,
                  offsets, offsetsLength, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        மாறிலி char16_t *compiledPattern,
        int32_t compiledPatternLength,
        int32_t* offsets,
        int32_t offsetsLength) {
    க்கு (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiledPatternLength - 1 -
            getArgumentLimit(compiledPattern, compiledPatternLength);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    க்கு (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        எனில் (n > ARG_இலக்கம்_LIMIT) {
            n -= ARG_இலக்கம்_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } இல்லையெனில் எனில் (n < offsetsLength) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    திரும்பு sb;
}

UnicodeString& SimpleFormatter::format(
        மாறிலி char16_t* compiledPattern,
        int32_t compiledPatternLength,
        மாறிலி UnicodeString* மாறிலி* மதிப்புகள்,
        UnicodeString& result U_LIFETIME_BOUND,
        மாறிலி UnicodeString* resultCopy,
        UBool forbid_Result_As_மதிப்பு,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு result;
    }
    க்கு (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    க்கு (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        எனில் (n < ARG_இலக்கம்_LIMIT) {
            மாறிலி UnicodeString *value = மதிப்புகள்[n];
            எனில் (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                திரும்பு result;
            }
            எனில் (value == &result) {
                எனில் (forbid_Result_As_மதிப்பு) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    திரும்பு result;
                }
                எனில் (i == 2) {
                    // We are appending to result which is also the first value object.
                    எனில் (n < offsetsLength) {
                        offsets[n] = 0;
                    }
                } இல்லையெனில் {
                    எனில் (n < offsetsLength) {
                        offsets[n] = result.length();
                    }
                    result.append(*resultCopy);
                }
            } இல்லையெனில் {
                எனில் (n < offsetsLength) {
                    offsets[n] = result.length();
                }
                result.append(*value);
            }
        } இல்லையெனில் {
            int32_t length = n - ARG_இலக்கம்_LIMIT;
            result.append(compiledPattern + i, length);
            i += length;
        }
    }
    திரும்பு result;
}

U_NAMESPACE_END
