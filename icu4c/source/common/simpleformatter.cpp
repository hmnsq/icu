// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/simpleformatter.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "uassert.h"

U_NAMESPACE_BEGIN

નામાકાશ {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
અચલ int32_t ARG_અંક_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
અચલ char16_t SEGMENT_LENGTH_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
અચલ int32_t MAX_SEGMENT_LENGTH = SEGMENT_LENGTH_PLACEHOLDER_CHAR - ARG_અંક_LIMIT;

ગણના {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

અંતઃરેખ UBool isInvalidArray(અચલ શૂન્ય *array, int32_t length) {
   પાછા (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::operator=(અચલ SimpleFormatter& other) U_LIFETIME_BOUND {
    જો (આ == &other) {
        પાછા *આ;
    }
    compiledPattern = other.compiledPattern;
    પાછા *આ;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        અચલ UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) {
        પાછા ખોટું;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    અચલ char16_t *patternBuffer = pattern.getBuffer();
    int32_t patternLength = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t textLength = 0;
    int32_t maxArg = -1;
    UBool inQuote = ખોટું;
    માટે (int32_t i = 0; i < patternLength;) {
        char16_t c = patternBuffer[i++];
        જો (c == APOS) {
            જો (i < patternLength && (c = patternBuffer[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } નહીંતર જો (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = ખોટું;
                ચાલુ;
            } નહીંતર જો (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = સાચું;
            } નહીંતર {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } નહીંતર જો (!inQuote && c == OPEN_BRACE) {
            જો (textLength > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                          static_cast<char16_t>(ARG_અંક_LIMIT + textLength));
                textLength = 0;
            }
            int32_t arg_સંખ્યા;
            જો ((i + 1) < patternLength &&
                    0 <= (arg_સંખ્યા = patternBuffer[i] - DIGIT_ZERO) && arg_સંખ્યા <= 9 &&
                    patternBuffer[i + 1] == CLOSE_BRACE) {
                i += 2;
            } નહીંતર {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_સંખ્યા = -1;
                જો (i < patternLength && DIGIT_ONE <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                    arg_સંખ્યા = c - DIGIT_ZERO;
                    જ્યારે (i < patternLength &&
                            DIGIT_ZERO <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                        arg_સંખ્યા = arg_સંખ્યા * 10 + (c - DIGIT_ZERO);
                        જો (arg_સંખ્યા >= ARG_અંક_LIMIT) {
                            તોડો;
                        }
                    }
                }
                જો (arg_સંખ્યા < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    પાછા ખોટું;
                }
            }
            જો (arg_સંખ્યા > maxArg) {
                maxArg = arg_સંખ્યા;
            }
            compiledPattern.append(static_cast<char16_t>(arg_સંખ્યા));
            ચાલુ;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        જો (textLength == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_LENGTH_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        જો (++textLength == MAX_SEGMENT_LENGTH) {
            textLength = 0;
        }
    }
    જો (textLength > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                  static_cast<char16_t>(ARG_અંક_LIMIT + textLength));
    }
    int32_t arg_ગણતરી = maxArg + 1;
    જો (arg_ગણતરી < min || max < arg_ગણતરી) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા ખોટું;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(arg_ગણતરી));
    પાછા સાચું;
}

UnicodeString& SimpleFormatter::format(
        અચલ UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) અચલ {
    અચલ UnicodeString *કિંમતો[] = { &value0 };
    પાછા formatAndAppend(કિંમતો, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        અચલ UnicodeString& value0,
        અચલ UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) અચલ {
    અચલ UnicodeString *કિંમતો[] = { &value0, &value1 };
    પાછા formatAndAppend(કિંમતો, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        અચલ UnicodeString& value0,
        અચલ UnicodeString& value1,
        અચલ UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) અચલ {
    અચલ UnicodeString *કિંમતો[] = { &value0, &value1, &value2 };
    પાછા formatAndAppend(કિંમતો, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        અચલ UnicodeString* અચલ* કિંમતો,
        int32_t કિંમતો_Length,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) અચલ {
    જો (U_FAILURE(errorCode)) {
        પાછા appendTo;
    }
    જો (isInvalidArray(કિંમતો, કિંમતો_Length) || isInvalidArray(offsets, offsetsLength) ||
            કિંમતો_Length < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા appendTo;
    }
    પાછા format(compiledPattern.getBuffer(), compiledPattern.length(), કિંમતો,
                  appendTo, nullptr, સાચું,
                  offsets, offsetsLength, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        અચલ UnicodeString* અચલ* કિંમતો,
        int32_t કિંમતો_Length,
        UnicodeString& result U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) અચલ {
    જો (U_FAILURE(errorCode)) {
        પાછા result;
    }
    જો (isInvalidArray(કિંમતો, કિંમતો_Length) || isInvalidArray(offsets, offsetsLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા result;
    }
    અચલ char16_t *cp = compiledPattern.getBuffer();
    int32_t cpLength = compiledPattern.length();
    જો (કિંમતો_Length < getArgumentLimit(cp, cpLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા result;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString resultCopy;
    જો (getArgumentLimit(cp, cpLength) > 0) {
        માટે (int32_t i = 1; i < cpLength;) {
            int32_t n = cp[i++];
            જો (n < ARG_અંક_LIMIT) {
                જો (કિંમતો[n] == &result) {
                    જો (i == 2) {
                        firstArg = n;
                    } નહીંતર જો (resultCopy.isEmpty() && !result.isEmpty()) {
                        resultCopy = result;
                    }
                }
            } નહીંતર {
                i += n - ARG_અંક_LIMIT;
            }
        }
    }
    જો (firstArg < 0) {
        result.remove();
    }
    પાછા format(cp, cpLength, કિંમતો,
                  result, &resultCopy, ખોટું,
                  offsets, offsetsLength, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        અચલ char16_t *compiledPattern,
        int32_t compiledPatternLength,
        int32_t* offsets,
        int32_t offsetsLength) {
    માટે (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiledPatternLength - 1 -
            getArgumentLimit(compiledPattern, compiledPatternLength);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    માટે (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        જો (n > ARG_અંક_LIMIT) {
            n -= ARG_અંક_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } નહીંતર જો (n < offsetsLength) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    પાછા sb;
}

UnicodeString& SimpleFormatter::format(
        અચલ char16_t* compiledPattern,
        int32_t compiledPatternLength,
        અચલ UnicodeString* અચલ* કિંમતો,
        UnicodeString& result U_LIFETIME_BOUND,
        અચલ UnicodeString* resultCopy,
        UBool forbid_Result_As_કિંમત,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) {
    જો (U_FAILURE(errorCode)) {
        પાછા result;
    }
    માટે (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    માટે (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        જો (n < ARG_અંક_LIMIT) {
            અચલ UnicodeString *value = કિંમતો[n];
            જો (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                પાછા result;
            }
            જો (value == &result) {
                જો (forbid_Result_As_કિંમત) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    પાછા result;
                }
                જો (i == 2) {
                    // We are appending to result which is also the first value object.
                    જો (n < offsetsLength) {
                        offsets[n] = 0;
                    }
                } નહીંતર {
                    જો (n < offsetsLength) {
                        offsets[n] = result.length();
                    }
                    result.append(*resultCopy);
                }
            } નહીંતર {
                જો (n < offsetsLength) {
                    offsets[n] = result.length();
                }
                result.append(*value);
            }
        } નહીંતર {
            int32_t length = n - ARG_અંક_LIMIT;
            result.append(compiledPattern + i, length);
            i += length;
        }
    }
    પાછા result;
}

U_NAMESPACE_END
