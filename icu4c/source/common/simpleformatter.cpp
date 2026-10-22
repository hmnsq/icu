// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/simpleformatter.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "uassert.h"

U_NAMESPACE_BEGIN

നാമാകാശ {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
സ്ഥിരം int32_t ARG_അക്കം_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
സ്ഥിരം char16_t SEGMENT_LENGTH_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
സ്ഥിരം int32_t MAX_SEGMENT_LENGTH = SEGMENT_LENGTH_PLACEHOLDER_CHAR - ARG_അക്കം_LIMIT;

എണ്ണൽ {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

വരിയിൽ UBool isInvalidArray(സ്ഥിരം ശൂന്യം *array, int32_t length) {
   തിരികെ (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::operator=(സ്ഥിരം SimpleFormatter& other) U_LIFETIME_BOUND {
    എങ്കിൽ (ഇത് == &other) {
        തിരികെ *ഇത്;
    }
    compiledPattern = other.compiledPattern;
    തിരികെ *ഇത്;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        സ്ഥിരം UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ അസത്യം;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    സ്ഥിരം char16_t *patternBuffer = pattern.getBuffer();
    int32_t patternLength = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t textLength = 0;
    int32_t maxArg = -1;
    UBool inQuote = അസത്യം;
    വേണ്ടി (int32_t i = 0; i < patternLength;) {
        char16_t c = patternBuffer[i++];
        എങ്കിൽ (c == APOS) {
            എങ്കിൽ (i < patternLength && (c = patternBuffer[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } അല്ലെങ്കിൽ എങ്കിൽ (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = അസത്യം;
                തുടരുക;
            } അല്ലെങ്കിൽ എങ്കിൽ (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = സത്യം;
            } അല്ലെങ്കിൽ {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ (!inQuote && c == OPEN_BRACE) {
            എങ്കിൽ (textLength > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                          static_cast<char16_t>(ARG_അക്കം_LIMIT + textLength));
                textLength = 0;
            }
            int32_t arg_സംഖ്യ;
            എങ്കിൽ ((i + 1) < patternLength &&
                    0 <= (arg_സംഖ്യ = patternBuffer[i] - DIGIT_ZERO) && arg_സംഖ്യ <= 9 &&
                    patternBuffer[i + 1] == CLOSE_BRACE) {
                i += 2;
            } അല്ലെങ്കിൽ {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_സംഖ്യ = -1;
                എങ്കിൽ (i < patternLength && DIGIT_ONE <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                    arg_സംഖ്യ = c - DIGIT_ZERO;
                    ആകുമ്പോൾ (i < patternLength &&
                            DIGIT_ZERO <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                        arg_സംഖ്യ = arg_സംഖ്യ * 10 + (c - DIGIT_ZERO);
                        എങ്കിൽ (arg_സംഖ്യ >= ARG_അക്കം_LIMIT) {
                            നിർത്തുക;
                        }
                    }
                }
                എങ്കിൽ (arg_സംഖ്യ < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    തിരികെ അസത്യം;
                }
            }
            എങ്കിൽ (arg_സംഖ്യ > maxArg) {
                maxArg = arg_സംഖ്യ;
            }
            compiledPattern.append(static_cast<char16_t>(arg_സംഖ്യ));
            തുടരുക;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        എങ്കിൽ (textLength == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_LENGTH_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        എങ്കിൽ (++textLength == MAX_SEGMENT_LENGTH) {
            textLength = 0;
        }
    }
    എങ്കിൽ (textLength > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                  static_cast<char16_t>(ARG_അക്കം_LIMIT + textLength));
    }
    int32_t arg_എണ്ണം = maxArg + 1;
    എങ്കിൽ (arg_എണ്ണം < min || max < arg_എണ്ണം) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ അസത്യം;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(arg_എണ്ണം));
    തിരികെ സത്യം;
}

UnicodeString& SimpleFormatter::format(
        സ്ഥിരം UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) സ്ഥിരം {
    സ്ഥിരം UnicodeString *മൂല്യങ്ങൾ[] = { &value0 };
    തിരികെ formatAndAppend(മൂല്യങ്ങൾ, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        സ്ഥിരം UnicodeString& value0,
        സ്ഥിരം UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) സ്ഥിരം {
    സ്ഥിരം UnicodeString *മൂല്യങ്ങൾ[] = { &value0, &value1 };
    തിരികെ formatAndAppend(മൂല്യങ്ങൾ, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        സ്ഥിരം UnicodeString& value0,
        സ്ഥിരം UnicodeString& value1,
        സ്ഥിരം UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) സ്ഥിരം {
    സ്ഥിരം UnicodeString *മൂല്യങ്ങൾ[] = { &value0, &value1, &value2 };
    തിരികെ formatAndAppend(മൂല്യങ്ങൾ, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        സ്ഥിരം UnicodeString* സ്ഥിരം* മൂല്യങ്ങൾ,
        int32_t മൂല്യങ്ങൾ_Length,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ appendTo;
    }
    എങ്കിൽ (isInvalidArray(മൂല്യങ്ങൾ, മൂല്യങ്ങൾ_Length) || isInvalidArray(offsets, offsetsLength) ||
            മൂല്യങ്ങൾ_Length < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ appendTo;
    }
    തിരികെ format(compiledPattern.getBuffer(), compiledPattern.length(), മൂല്യങ്ങൾ,
                  appendTo, nullptr, സത്യം,
                  offsets, offsetsLength, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        സ്ഥിരം UnicodeString* സ്ഥിരം* മൂല്യങ്ങൾ,
        int32_t മൂല്യങ്ങൾ_Length,
        UnicodeString& result U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ result;
    }
    എങ്കിൽ (isInvalidArray(മൂല്യങ്ങൾ, മൂല്യങ്ങൾ_Length) || isInvalidArray(offsets, offsetsLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ result;
    }
    സ്ഥിരം char16_t *cp = compiledPattern.getBuffer();
    int32_t cpLength = compiledPattern.length();
    എങ്കിൽ (മൂല്യങ്ങൾ_Length < getArgumentLimit(cp, cpLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ result;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString resultCopy;
    എങ്കിൽ (getArgumentLimit(cp, cpLength) > 0) {
        വേണ്ടി (int32_t i = 1; i < cpLength;) {
            int32_t n = cp[i++];
            എങ്കിൽ (n < ARG_അക്കം_LIMIT) {
                എങ്കിൽ (മൂല്യങ്ങൾ[n] == &result) {
                    എങ്കിൽ (i == 2) {
                        firstArg = n;
                    } അല്ലെങ്കിൽ എങ്കിൽ (resultCopy.isEmpty() && !result.isEmpty()) {
                        resultCopy = result;
                    }
                }
            } അല്ലെങ്കിൽ {
                i += n - ARG_അക്കം_LIMIT;
            }
        }
    }
    എങ്കിൽ (firstArg < 0) {
        result.remove();
    }
    തിരികെ format(cp, cpLength, മൂല്യങ്ങൾ,
                  result, &resultCopy, അസത്യം,
                  offsets, offsetsLength, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        സ്ഥിരം char16_t *compiledPattern,
        int32_t compiledPatternLength,
        int32_t* offsets,
        int32_t offsetsLength) {
    വേണ്ടി (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiledPatternLength - 1 -
            getArgumentLimit(compiledPattern, compiledPatternLength);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    വേണ്ടി (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        എങ്കിൽ (n > ARG_അക്കം_LIMIT) {
            n -= ARG_അക്കം_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } അല്ലെങ്കിൽ എങ്കിൽ (n < offsetsLength) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    തിരികെ sb;
}

UnicodeString& SimpleFormatter::format(
        സ്ഥിരം char16_t* compiledPattern,
        int32_t compiledPatternLength,
        സ്ഥിരം UnicodeString* സ്ഥിരം* മൂല്യങ്ങൾ,
        UnicodeString& result U_LIFETIME_BOUND,
        സ്ഥിരം UnicodeString* resultCopy,
        UBool forbid_Result_As_മൂല്യം,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ result;
    }
    വേണ്ടി (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    വേണ്ടി (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        എങ്കിൽ (n < ARG_അക്കം_LIMIT) {
            സ്ഥിരം UnicodeString *value = മൂല്യങ്ങൾ[n];
            എങ്കിൽ (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                തിരികെ result;
            }
            എങ്കിൽ (value == &result) {
                എങ്കിൽ (forbid_Result_As_മൂല്യം) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    തിരികെ result;
                }
                എങ്കിൽ (i == 2) {
                    // We are appending to result which is also the first value object.
                    എങ്കിൽ (n < offsetsLength) {
                        offsets[n] = 0;
                    }
                } അല്ലെങ്കിൽ {
                    എങ്കിൽ (n < offsetsLength) {
                        offsets[n] = result.length();
                    }
                    result.append(*resultCopy);
                }
            } അല്ലെങ്കിൽ {
                എങ്കിൽ (n < offsetsLength) {
                    offsets[n] = result.length();
                }
                result.append(*value);
            }
        } അല്ലെങ്കിൽ {
            int32_t length = n - ARG_അക്കം_LIMIT;
            result.append(compiledPattern + i, length);
            i += length;
        }
    }
    തിരികെ result;
}

U_NAMESPACE_END
