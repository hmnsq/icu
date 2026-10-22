// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/simpleformatter.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN

नामाकाश {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
स्थिरांक int32_t ARG_अंक_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
स्थिरांक char16_t SEGMENT_LENGTH_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
स्थिरांक int32_t MAX_SEGMENT_LENGTH = SEGMENT_LENGTH_PLACEHOLDER_CHAR - ARG_अंक_LIMIT;

गणना {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

ओळीत UBool isInvalidArray(स्थिरांक शून्य *array, int32_t length) {
   परत (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::चालक=(स्थिरांक SimpleFormatter& other) U_LIFETIME_BOUND {
    जर (हे == &other) {
        परत *हे;
    }
    compiledPattern = other.compiledPattern;
    परत *हे;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        स्थिरांक UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) {
        परत असत्य;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    स्थिरांक char16_t *patternBuffer = pattern.getBuffer();
    int32_t patternLength = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t textLength = 0;
    int32_t maxArg = -1;
    UBool inQuote = असत्य;
    साठी (int32_t i = 0; i < patternLength;) {
        char16_t c = patternBuffer[i++];
        जर (c == APOS) {
            जर (i < patternLength && (c = patternBuffer[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } अन्यथा जर (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = असत्य;
                सुरूठेवा;
            } अन्यथा जर (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = सत्य;
            } अन्यथा {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } अन्यथा जर (!inQuote && c == OPEN_BRACE) {
            जर (textLength > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                          static_cast<char16_t>(ARG_अंक_LIMIT + textLength));
                textLength = 0;
            }
            int32_t arg_संख्या;
            जर ((i + 1) < patternLength &&
                    0 <= (arg_संख्या = patternBuffer[i] - DIGIT_ZERO) && arg_संख्या <= 9 &&
                    patternBuffer[i + 1] == CLOSE_BRACE) {
                i += 2;
            } अन्यथा {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_संख्या = -1;
                जर (i < patternLength && DIGIT_ONE <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                    arg_संख्या = c - DIGIT_ZERO;
                    पर्यंत (i < patternLength &&
                            DIGIT_ZERO <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                        arg_संख्या = arg_संख्या * 10 + (c - DIGIT_ZERO);
                        जर (arg_संख्या >= ARG_अंक_LIMIT) {
                            थांबा;
                        }
                    }
                }
                जर (arg_संख्या < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    परत असत्य;
                }
            }
            जर (arg_संख्या > maxArg) {
                maxArg = arg_संख्या;
            }
            compiledPattern.append(static_cast<char16_t>(arg_संख्या));
            सुरूठेवा;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        जर (textLength == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_LENGTH_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        जर (++textLength == MAX_SEGMENT_LENGTH) {
            textLength = 0;
        }
    }
    जर (textLength > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                  static_cast<char16_t>(ARG_अंक_LIMIT + textLength));
    }
    int32_t arg_मोजणी = maxArg + 1;
    जर (arg_मोजणी < min || max < arg_मोजणी) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत असत्य;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(arg_मोजणी));
    परत सत्य;
}

UnicodeString& SimpleFormatter::format(
        स्थिरांक UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) स्थिरांक {
    स्थिरांक UnicodeString *मूल्ये[] = { &value0 };
    परत formatAndAppend(मूल्ये, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        स्थिरांक UnicodeString& value0,
        स्थिरांक UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) स्थिरांक {
    स्थिरांक UnicodeString *मूल्ये[] = { &value0, &value1 };
    परत formatAndAppend(मूल्ये, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        स्थिरांक UnicodeString& value0,
        स्थिरांक UnicodeString& value1,
        स्थिरांक UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) स्थिरांक {
    स्थिरांक UnicodeString *मूल्ये[] = { &value0, &value1, &value2 };
    परत formatAndAppend(मूल्ये, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        स्थिरांक UnicodeString* स्थिरांक* मूल्ये,
        int32_t मूल्ये_Length,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) {
        परत appendTo;
    }
    जर (isInvalidArray(मूल्ये, मूल्ये_Length) || isInvalidArray(offsets, offsetsLength) ||
            मूल्ये_Length < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत appendTo;
    }
    परत format(compiledPattern.getBuffer(), compiledPattern.length(), मूल्ये,
                  appendTo, nullptr, सत्य,
                  offsets, offsetsLength, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        स्थिरांक UnicodeString* स्थिरांक* मूल्ये,
        int32_t मूल्ये_Length,
        UnicodeString& result U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) {
        परत result;
    }
    जर (isInvalidArray(मूल्ये, मूल्ये_Length) || isInvalidArray(offsets, offsetsLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत result;
    }
    स्थिरांक char16_t *cp = compiledPattern.getBuffer();
    int32_t cpLength = compiledPattern.length();
    जर (मूल्ये_Length < getArgumentLimit(cp, cpLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत result;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString resultCopy;
    जर (getArgumentLimit(cp, cpLength) > 0) {
        साठी (int32_t i = 1; i < cpLength;) {
            int32_t n = cp[i++];
            जर (n < ARG_अंक_LIMIT) {
                जर (मूल्ये[n] == &result) {
                    जर (i == 2) {
                        firstArg = n;
                    } अन्यथा जर (resultCopy.isEmpty() && !result.isEmpty()) {
                        resultCopy = result;
                    }
                }
            } अन्यथा {
                i += n - ARG_अंक_LIMIT;
            }
        }
    }
    जर (firstArg < 0) {
        result.हटाओ();
    }
    परत format(cp, cpLength, मूल्ये,
                  result, &resultCopy, असत्य,
                  offsets, offsetsLength, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        स्थिरांक char16_t *compiledPattern,
        int32_t compiledPatternLength,
        int32_t* offsets,
        int32_t offsetsLength) {
    साठी (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiledPatternLength - 1 -
            getArgumentLimit(compiledPattern, compiledPatternLength);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    साठी (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        जर (n > ARG_अंक_LIMIT) {
            n -= ARG_अंक_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } अन्यथा जर (n < offsetsLength) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    परत sb;
}

UnicodeString& SimpleFormatter::format(
        स्थिरांक char16_t* compiledPattern,
        int32_t compiledPatternLength,
        स्थिरांक UnicodeString* स्थिरांक* मूल्ये,
        UnicodeString& result U_LIFETIME_BOUND,
        स्थिरांक UnicodeString* resultCopy,
        UBool forbid_Result_As_मूल्य,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) {
    जर (U_FAILURE(errorCode)) {
        परत result;
    }
    साठी (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    साठी (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        जर (n < ARG_अंक_LIMIT) {
            स्थिरांक UnicodeString *value = मूल्ये[n];
            जर (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                परत result;
            }
            जर (value == &result) {
                जर (forbid_Result_As_मूल्य) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    परत result;
                }
                जर (i == 2) {
                    // We are appending to result which is also the first value object.
                    जर (n < offsetsLength) {
                        offsets[n] = 0;
                    }
                } अन्यथा {
                    जर (n < offsetsLength) {
                        offsets[n] = result.length();
                    }
                    result.append(*resultCopy);
                }
            } अन्यथा {
                जर (n < offsetsLength) {
                    offsets[n] = result.length();
                }
                result.append(*value);
            }
        } अन्यथा {
            int32_t length = n - ARG_अंक_LIMIT;
            result.append(compiledPattern + i, length);
            i += length;
        }
    }
    परत result;
}

U_NAMESPACE_END
