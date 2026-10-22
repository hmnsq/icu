// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/simpleformatter.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "uassert.h"

U_NAMESPACE_BEGIN

নামাকাশ {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
ধ্রুব int32_t ARG_অঙ্ক_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ধ্রুব char16_t SEGMENT_LENGTH_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ধ্রুব int32_t MAX_SEGMENT_LENGTH = SEGMENT_LENGTH_PLACEHOLDER_CHAR - ARG_অঙ্ক_LIMIT;

গণনা {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

অন্তঃস্থ UBool isInvalidArray(ধ্রুব শূন্য *array, int32_t length) {
   প্রত্যাগম (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::operator=(ধ্রুব SimpleFormatter& other) U_LIFETIME_BOUND {
    যদি (এতত্ == &other) {
        প্রত্যাগম *এতত্;
    }
    compiledPattern = other.compiledPattern;
    প্রত্যাগম *এতত্;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        ধ্রুব UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) {
        প্রত্যাগম অসত্য;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ধ্রুব char16_t *patternBuffer = pattern.getBuffer();
    int32_t patternLength = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t textLength = 0;
    int32_t maxArg = -1;
    UBool inQuote = অসত্য;
    প্রতি (int32_t i = 0; i < patternLength;) {
        char16_t c = patternBuffer[i++];
        যদি (c == APOS) {
            যদি (i < patternLength && (c = patternBuffer[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } অন্যথা যদি (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = অসত্য;
                অনুবর্ত;
            } অন্যথা যদি (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = সত্য;
            } অন্যথা {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } অন্যথা যদি (!inQuote && c == OPEN_BRACE) {
            যদি (textLength > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                          static_cast<char16_t>(ARG_অঙ্ক_LIMIT + textLength));
                textLength = 0;
            }
            int32_t arg_সংখ্যা;
            যদি ((i + 1) < patternLength &&
                    0 <= (arg_সংখ্যা = patternBuffer[i] - DIGIT_ZERO) && arg_সংখ্যা <= 9 &&
                    patternBuffer[i + 1] == CLOSE_BRACE) {
                i += 2;
            } অন্যথা {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_সংখ্যা = -1;
                যদি (i < patternLength && DIGIT_ONE <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                    arg_সংখ্যা = c - DIGIT_ZERO;
                    যাবত্ (i < patternLength &&
                            DIGIT_ZERO <= (c = patternBuffer[i++]) && c <= DIGIT_NINE) {
                        arg_সংখ্যা = arg_সংখ্যা * 10 + (c - DIGIT_ZERO);
                        যদি (arg_সংখ্যা >= ARG_অঙ্ক_LIMIT) {
                            ভঙ্গ;
                        }
                    }
                }
                যদি (arg_সংখ্যা < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    প্রত্যাগম অসত্য;
                }
            }
            যদি (arg_সংখ্যা > maxArg) {
                maxArg = arg_সংখ্যা;
            }
            compiledPattern.append(static_cast<char16_t>(arg_সংখ্যা));
            অনুবর্ত;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        যদি (textLength == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_LENGTH_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        যদি (++textLength == MAX_SEGMENT_LENGTH) {
            textLength = 0;
        }
    }
    যদি (textLength > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - textLength - 1,
                                  static_cast<char16_t>(ARG_অঙ্ক_LIMIT + textLength));
    }
    int32_t arg_গুনতি = maxArg + 1;
    যদি (arg_গুনতি < min || max < arg_গুনতি) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম অসত্য;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(arg_গুনতি));
    প্রত্যাগম সত্য;
}

UnicodeString& SimpleFormatter::format(
        ধ্রুব UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ধ্রুব {
    ধ্রুব UnicodeString *মানসমূহ[] = { &value0 };
    প্রত্যাগম formatAndAppend(মানসমূহ, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ধ্রুব UnicodeString& value0,
        ধ্রুব UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ধ্রুব {
    ধ্রুব UnicodeString *মানসমূহ[] = { &value0, &value1 };
    প্রত্যাগম formatAndAppend(মানসমূহ, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ধ্রুব UnicodeString& value0,
        ধ্রুব UnicodeString& value1,
        ধ্রুব UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ধ্রুব {
    ধ্রুব UnicodeString *মানসমূহ[] = { &value0, &value1, &value2 };
    প্রত্যাগম formatAndAppend(মানসমূহ, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ধ্রুব UnicodeString* ধ্রুব* মানসমূহ,
        int32_t মানসমূহ_Length,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) {
        প্রত্যাগম appendTo;
    }
    যদি (isInvalidArray(মানসমূহ, মানসমূহ_Length) || isInvalidArray(offsets, offsetsLength) ||
            মানসমূহ_Length < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম appendTo;
    }
    প্রত্যাগম format(compiledPattern.getBuffer(), compiledPattern.length(), মানসমূহ,
                  appendTo, nullptr, সত্য,
                  offsets, offsetsLength, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ধ্রুব UnicodeString* ধ্রুব* মানসমূহ,
        int32_t মানসমূহ_Length,
        UnicodeString& result U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) {
        প্রত্যাগম result;
    }
    যদি (isInvalidArray(মানসমূহ, মানসমূহ_Length) || isInvalidArray(offsets, offsetsLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম result;
    }
    ধ্রুব char16_t *cp = compiledPattern.getBuffer();
    int32_t cpLength = compiledPattern.length();
    যদি (মানসমূহ_Length < getArgumentLimit(cp, cpLength)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম result;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString resultCopy;
    যদি (getArgumentLimit(cp, cpLength) > 0) {
        প্রতি (int32_t i = 1; i < cpLength;) {
            int32_t n = cp[i++];
            যদি (n < ARG_অঙ্ক_LIMIT) {
                যদি (মানসমূহ[n] == &result) {
                    যদি (i == 2) {
                        firstArg = n;
                    } অন্যথা যদি (resultCopy.isEmpty() && !result.isEmpty()) {
                        resultCopy = result;
                    }
                }
            } অন্যথা {
                i += n - ARG_অঙ্ক_LIMIT;
            }
        }
    }
    যদি (firstArg < 0) {
        result.remove();
    }
    প্রত্যাগম format(cp, cpLength, মানসমূহ,
                  result, &resultCopy, অসত্য,
                  offsets, offsetsLength, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        ধ্রুব char16_t *compiledPattern,
        int32_t compiledPatternLength,
        int32_t* offsets,
        int32_t offsetsLength) {
    প্রতি (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiledPatternLength - 1 -
            getArgumentLimit(compiledPattern, compiledPatternLength);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    প্রতি (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        যদি (n > ARG_অঙ্ক_LIMIT) {
            n -= ARG_অঙ্ক_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } অন্যথা যদি (n < offsetsLength) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    প্রত্যাগম sb;
}

UnicodeString& SimpleFormatter::format(
        ধ্রুব char16_t* compiledPattern,
        int32_t compiledPatternLength,
        ধ্রুব UnicodeString* ধ্রুব* মানসমূহ,
        UnicodeString& result U_LIFETIME_BOUND,
        ধ্রুব UnicodeString* resultCopy,
        UBool forbid_Result_As_মান,
        int32_t* offsets,
        int32_t offsetsLength,
        UErrorCode& errorCode) {
    যদি (U_FAILURE(errorCode)) {
        প্রত্যাগম result;
    }
    প্রতি (int32_t i = 0; i < offsetsLength; i++) {
        offsets[i] = -1;
    }
    প্রতি (int32_t i = 1; i < compiledPatternLength;) {
        int32_t n = compiledPattern[i++];
        যদি (n < ARG_অঙ্ক_LIMIT) {
            ধ্রুব UnicodeString *value = মানসমূহ[n];
            যদি (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                প্রত্যাগম result;
            }
            যদি (value == &result) {
                যদি (forbid_Result_As_মান) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    প্রত্যাগম result;
                }
                যদি (i == 2) {
                    // We are appending to result which is also the first value object.
                    যদি (n < offsetsLength) {
                        offsets[n] = 0;
                    }
                } অন্যথা {
                    যদি (n < offsetsLength) {
                        offsets[n] = result.length();
                    }
                    result.append(*resultCopy);
                }
            } অন্যথা {
                যদি (n < offsetsLength) {
                    offsets[n] = result.length();
                }
                result.append(*value);
            }
        } অন্যথা {
            int32_t length = n - ARG_অঙ্ক_LIMIT;
            result.append(compiledPattern + i, length);
            i += length;
        }
    }
    প্রত্যাগম result;
}

U_NAMESPACE_END
