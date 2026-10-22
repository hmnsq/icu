// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/simpleformatter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

U_NAMESPACE_BEGIN

ନାମାକାଶ {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
ଅଚଳ int32_t ARG_ଅଙ୍କ_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ଅଚଳ char16_t SEGMENT_LENGTH_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ଅଚଳ int32_t MAX_SEGMENT_LENGTH = SEGMENT_LENGTH_PLACEHOLDER_CHAR - ARG_ଅଙ୍କ_LIMIT;

ଗଣନା {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

ଧାଡ଼ିଭିତରେ UBool isInvalidArray(ଅଚଳ ଶୂନ୍ୟ *array, int32_t length) {
   ଫେରନ୍ତୁ (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::operator=(ଅଚଳ SimpleFormatter& other) U_LIFETIME_BOUND {
    ଯଦି (ଏହି == &other) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    compiledPattern = other.compiledPattern;
    ଫେରନ୍ତୁ *ଏହି;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        ଅଚଳ UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ଅଚଳ char16_t *patternBuffer = pattern.getBuffer();
    int32_t patternLength = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t textLength = 0;
    int32_t maxArg = -1;
    UBool inQuote = ମିଥ୍ୟା;
    ପାଇଁ (int32_t i = 0; i < patternLength;) {
        char16_t c = patternBuffer[i++];
        ଯଦି (c == APOS) {
            ଯଦି (i < patternLength && (c = patternBuffer[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } ଅନ୍ୟଥା ଯଦି (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = ମିଥ୍ୟା;
                ଜାରି;
            } ଅନ୍ୟଥା ଯଦି (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = ସତ୍ୟ;
            } ଅନ୍ୟଥା {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } ଅନ୍ୟଥା ଯଦି (!inQuote && c == OPEN_BRACE) {
            ଯଦି (textLength > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                          static_cast<char16_t>(ARG_ଅଙ୍କ_LIMIT + textLength));
                textLength = 0;
            }
            int32_t arg_ସଂଖ୍ୟା;
            ଯଦି ((i + 1) < patternLength &&
                    0 <= (arg_ସଂଖ୍ୟା = patternBuffer[i] - DIGIT_ZERO) && arg_ସଂଖ୍ୟା <= 9 &&
                    patternBuffer[i + 1] == CLOSE_BRACE) {
                i += 2;
            } ଅନ୍ୟଥା {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_ସଂଖ୍ୟା = -1;
                ଯଦି (i < patternLength && DIGIT_ONE <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                    arg_ସଂଖ୍ୟା = c - DIGIT_ZERO;
                    ଯେତେବେଳେ (i < patternLength &&
                            DIGIT_ZERO <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                        arg_ସଂଖ୍ୟା = arg_ସଂଖ୍ୟା * 10 + (c - DIGIT_ZERO);
                        ଯଦି (arg_ସଂଖ୍ୟା >= ARG_ଅଙ୍କ_LIMIT) {
                            ଭାଙ୍ଗ;
                        }
                    }
                }
                ଯଦି (arg_ସଂଖ୍ୟା < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
            }
            ଯଦି (arg_ସଂଖ୍ୟା > maxArg) {
                maxArg = arg_ସଂଖ୍ୟା;
            }
            compiledPattern.append(static_cast<char16_t>(arg_ସଂଖ୍ୟା));
            ଜାରି;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        ଯଦି (textLength == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_LENGTH_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        ଯଦି (++textLength == MAX_SEGMENT_LENGTH) {
            textLength = 0;
        }
    }
    ଯଦି (textLength > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                  static_cast<char16_t>(ARG_ଅଙ୍କ_LIMIT + textLength));
    }
    int32_t arg_ଗଣନା = maxArg + 1;
    ଯଦି (arg_ଗଣନା < min || max < arg_ଗଣନା) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(arg_ଗଣନା));
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

UnicodeString& SimpleFormatter::format(
        ଅଚଳ UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ଅଚଳ {
    ଅଚଳ UnicodeString *ମୂଲ୍ୟଗୁଡ଼ିକ[] = { &value0 };
    ଫେରନ୍ତୁ formatAndAppend(ମୂଲ୍ୟଗୁଡ଼ିକ, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ଅଚଳ UnicodeString& value0,
        ଅଚଳ UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ଅଚଳ {
    ଅଚଳ UnicodeString *ମୂଲ୍ୟଗୁଡ଼ିକ[] = { &value0, &value1 };
    ଫେରନ୍ତୁ formatAndAppend(ମୂଲ୍ୟଗୁଡ଼ିକ, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ଅଚଳ UnicodeString& value0,
        ଅଚଳ UnicodeString& value1,
        ଅଚଳ UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ଅଚଳ {
    ଅଚଳ UnicodeString *ମୂଲ୍ୟଗୁଡ଼ିକ[] = { &value0, &value1, &value2 };
    ଫେରନ୍ତୁ formatAndAppend(ମୂଲ୍ୟଗୁଡ଼ିକ, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ଅଚଳ UnicodeString* ଅଚଳ* ମୂଲ୍ୟଗୁଡ଼ିକ,
        int32_t ମୂଲ୍ୟଗୁଡ଼ିକ_Length,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ appendTo;
    }
    ଯଦି (isInvalidArray(ମୂଲ୍ୟଗୁଡ଼ିକ, ମୂଲ୍ୟଗୁଡ଼ିକ_Length) || isInvalidArray(offsets, offsetsLength) ||
            ମୂଲ୍ୟଗୁଡ଼ିକ_Length < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ appendTo;
    }
    ଫେରନ୍ତୁ format(compiledPattern.getBuffer(), compiledPattern.length(), ମୂଲ୍ୟଗୁଡ଼ିକ,
                  appendTo, nullptr, ସତ୍ୟ,
                  offsets, offsetsLength, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ଅଚଳ UnicodeString* ଅଚଳ* ମୂଲ୍ୟଗୁଡ଼ିକ,
        int32_t ମୂଲ୍ୟଗୁଡ଼ିକ_Length,
        UnicodeString& result U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ result;
    }
    ଯଦି (isInvalidArray(ମୂଲ୍ୟଗୁଡ଼ିକ, ମୂଲ୍ୟଗୁଡ଼ିକ_Length) || isInvalidArray(offsets, offsetsLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ result;
    }
    ଅଚଳ char16_t *cp = compiledPattern.getBuffer();
    int32_t cpLength = compiledPattern.length();
    ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ_Length < getArgumentLimit(cp, cpLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ result;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString resultCopy;
    ଯଦି (getArgumentLimit(cp, cpLength) > 0) {
        ପାଇଁ (int32_t i = 1; i < cpLength;) {
            int32_t n = cp[i++];
            ଯଦି (n < ARG_ଅଙ୍କ_LIMIT) {
                ଯଦି (ମୂଲ୍ୟଗୁଡ଼ିକ[n] == &result) {
                    ଯଦି (i == 2) {
                        firstArg = n;
                    } ଅନ୍ୟଥା ଯଦି (resultCopy.isEmpty() && !result.isEmpty()) {
                        resultCopy = result;
                    }
                }
            } ଅନ୍ୟଥା {
                i += n - ARG_ଅଙ୍କ_LIMIT;
            }
        }
    }
    ଯଦି (firstArg < 0) {
        result.remove();
    }
    ଫେରନ୍ତୁ format(cp, cpLength, ମୂଲ୍ୟଗୁଡ଼ିକ,
                  result, &resultCopy, ମିଥ୍ୟା,
                  offsets, offsetsLength, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        ଅଚଳ char16_t *compiledPattern,
        int32_t compiledPatternLength,
        int32_t* offsets,
        int32_t offsetsLength) {
    ପାଇଁ (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiledPatternLength - 1 -
            getArgumentLimit(compiledPattern, compiledPatternLength);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    ପାଇଁ (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        ଯଦି (n > ARG_ଅଙ୍କ_LIMIT) {
            n -= ARG_ଅଙ୍କ_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } ଅନ୍ୟଥା ଯଦି (n < offsetsLength) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    ଫେରନ୍ତୁ sb;
}

UnicodeString& SimpleFormatter::format(
        ଅଚଳ char16_t* compiledPattern,
        int32_t compiledPatternLength,
        ଅଚଳ UnicodeString* ଅଚଳ* ମୂଲ୍ୟଗୁଡ଼ିକ,
        UnicodeString& result U_LIFETIME_BOUND,
        ଅଚଳ UnicodeString* resultCopy,
        UBool forbid_Result_As_ମୂଲ୍ୟ,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ result;
    }
    ପାଇଁ (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    ପାଇଁ (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        ଯଦି (n < ARG_ଅଙ୍କ_LIMIT) {
            ଅଚଳ UnicodeString *value = ମୂଲ୍ୟଗୁଡ଼ିକ[n];
            ଯଦି (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ଫେରନ୍ତୁ result;
            }
            ଯଦି (value == &result) {
                ଯଦି (forbid_Result_As_ମୂଲ୍ୟ) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ଫେରନ୍ତୁ result;
                }
                ଯଦି (i == 2) {
                    // We are appending to result which is also the first value object.
                    ଯଦି (n < offsetsLength) {
                        offsets[n] = 0;
                    }
                } ଅନ୍ୟଥା {
                    ଯଦି (n < offsetsLength) {
                        offsets[n] = result.length();
                    }
                    result.append(*resultCopy);
                }
            } ଅନ୍ୟଥା {
                ଯଦି (n < offsetsLength) {
                    offsets[n] = result.length();
                }
                result.append(*value);
            }
        } ଅନ୍ୟଥା {
            int32_t length = n - ARG_ଅଙ୍କ_LIMIT;
            result.append(compiledPattern + i, length);
            i += length;
        }
    }
    ଫେରନ୍ତୁ result;
}

U_NAMESPACE_END
