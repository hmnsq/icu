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
ধ্রুবক int32_t ARG_অঙ্ক_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ধ্রুবক char16_t SEGMENT_দৈর্ঘ্য_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ধ্রুবক int32_t MAX_SEGMENT_দৈর্ঘ্য = SEGMENT_দৈর্ঘ্য_PLACEHOLDER_CHAR - ARG_অঙ্ক_LIMIT;

গণনা {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

অন্তঃরেখ UBool isInvalidArray(ধ্রুবক শূন্য *array, int32_t length) {
   ফেরত (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::operator=(ধ্রুবক SimpleFormatter& other) U_LIFETIME_BOUND {
    যদি (এই == &other) {
        ফেরত *এই;
    }
    compiledPattern = other.compiledPattern;
    ফেরত *এই;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        ধ্রুবক UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) {
        ফেরত মিথ্যা;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ধ্রুবক char16_t *pattern_বাফার = pattern.get_বাফার();
    int32_t pattern_দৈর্ঘ্য = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t text_দৈর্ঘ্য = 0;
    int32_t maxArg = -1;
    UBool inQuote = মিথ্যা;
    জন্য (int32_t i = 0; i < pattern_দৈর্ঘ্য;) {
        char16_t c = pattern_বাফার[i++];
        যদি (c == APOS) {
            যদি (i < pattern_দৈর্ঘ্য && (c = pattern_বাফার[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } অন্যথা যদি (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = মিথ্যা;
                চালু;
            } অন্যথা যদি (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = সত্য;
            } অন্যথা {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } অন্যথা যদি (!inQuote && c == OPEN_BRACE) {
            যদি (text_দৈর্ঘ্য > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - text_দৈর্ঘ্য - 1,
                                          static_cast<char16_t>(ARG_অঙ্ক_LIMIT + text_দৈর্ঘ্য));
                text_দৈর্ঘ্য = 0;
            }
            int32_t arg_সংখ্যা;
            যদি ((i + 1) < pattern_দৈর্ঘ্য &&
                    0 <= (arg_সংখ্যা = pattern_বাফার[i] - DIGIT_ZERO) && arg_সংখ্যা <= 9 &&
                    pattern_বাফার[i + 1] == CLOSE_BRACE) {
                i += 2;
            } অন্যথা {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_সংখ্যা = -1;
                যদি (i < pattern_দৈর্ঘ্য && DIGIT_ONE <= (c = pattern_বাফার[i++]) && c <= DIGIT_NINE) {
                    arg_সংখ্যা = c - DIGIT_ZERO;
                    যখন (i < pattern_দৈর্ঘ্য &&
                            DIGIT_ZERO <= (c = pattern_বাফার[i++]) && c <= DIGIT_NINE) {
                        arg_সংখ্যা = arg_সংখ্যা * 10 + (c - DIGIT_ZERO);
                        যদি (arg_সংখ্যা >= ARG_অঙ্ক_LIMIT) {
                            বিরতি;
                        }
                    }
                }
                যদি (arg_সংখ্যা < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ফেরত মিথ্যা;
                }
            }
            যদি (arg_সংখ্যা > maxArg) {
                maxArg = arg_সংখ্যা;
            }
            compiledPattern.append(static_cast<char16_t>(arg_সংখ্যা));
            চালু;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        যদি (text_দৈর্ঘ্য == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_দৈর্ঘ্য_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        যদি (++text_দৈর্ঘ্য == MAX_SEGMENT_দৈর্ঘ্য) {
            text_দৈর্ঘ্য = 0;
        }
    }
    যদি (text_দৈর্ঘ্য > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - text_দৈর্ঘ্য - 1,
                                  static_cast<char16_t>(ARG_অঙ্ক_LIMIT + text_দৈর্ঘ্য));
    }
    int32_t arg_গুনতি = maxArg + 1;
    যদি (arg_গুনতি < min || max < arg_গুনতি) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত মিথ্যা;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(arg_গুনতি));
    ফেরত সত্য;
}

UnicodeString& SimpleFormatter::format(
        ধ্রুবক UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ধ্রুবক {
    ধ্রুবক UnicodeString *মানসমূহ[] = { &value0 };
    ফেরত formatAndAppend(মানসমূহ, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ধ্রুবক UnicodeString& value0,
        ধ্রুবক UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ধ্রুবক {
    ধ্রুবক UnicodeString *মানসমূহ[] = { &value0, &value1 };
    ফেরত formatAndAppend(মানসমূহ, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        ধ্রুবক UnicodeString& value0,
        ধ্রুবক UnicodeString& value1,
        ধ্রুবক UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) ধ্রুবক {
    ধ্রুবক UnicodeString *মানসমূহ[] = { &value0, &value1, &value2 };
    ফেরত formatAndAppend(মানসমূহ, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ধ্রুবক UnicodeString* ধ্রুবক* মানসমূহ,
        int32_t মানসমূহ_দৈর্ঘ্য,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_দৈর্ঘ্য,
        UErrorCode& errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) {
        ফেরত appendTo;
    }
    যদি (isInvalidArray(মানসমূহ, মানসমূহ_দৈর্ঘ্য) || isInvalidArray(offsets, offsets_দৈর্ঘ্য) ||
            মানসমূহ_দৈর্ঘ্য < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত appendTo;
    }
    ফেরত format(compiledPattern.get_বাফার(), compiledPattern.length(), মানসমূহ,
                  appendTo, nullptr, সত্য,
                  offsets, offsets_দৈর্ঘ্য, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ধ্রুবক UnicodeString* ধ্রুবক* মানসমূহ,
        int32_t মানসমূহ_দৈর্ঘ্য,
        UnicodeString& ফলাফল U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_দৈর্ঘ্য,
        UErrorCode& errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) {
        ফেরত ফলাফল;
    }
    যদি (isInvalidArray(মানসমূহ, মানসমূহ_দৈর্ঘ্য) || isInvalidArray(offsets, offsets_দৈর্ঘ্য)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত ফলাফল;
    }
    ধ্রুবক char16_t *cp = compiledPattern.get_বাফার();
    int32_t cp_দৈর্ঘ্য = compiledPattern.length();
    যদি (মানসমূহ_দৈর্ঘ্য < getArgumentLimit(cp, cp_দৈর্ঘ্য)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত ফলাফল;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString ফলাফল_Copy;
    যদি (getArgumentLimit(cp, cp_দৈর্ঘ্য) > 0) {
        জন্য (int32_t i = 1; i < cp_দৈর্ঘ্য;) {
            int32_t n = cp[i++];
            যদি (n < ARG_অঙ্ক_LIMIT) {
                যদি (মানসমূহ[n] == &ফলাফল) {
                    যদি (i == 2) {
                        firstArg = n;
                    } অন্যথা যদি (ফলাফল_Copy.isEmpty() && !ফলাফল.isEmpty()) {
                        ফলাফল_Copy = ফলাফল;
                    }
                }
            } অন্যথা {
                i += n - ARG_অঙ্ক_LIMIT;
            }
        }
    }
    যদি (firstArg < 0) {
        ফলাফল.remove();
    }
    ফেরত format(cp, cp_দৈর্ঘ্য, মানসমূহ,
                  ফলাফল, &ফলাফল_Copy, মিথ্যা,
                  offsets, offsets_দৈর্ঘ্য, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        ধ্রুবক char16_t *compiledPattern,
        int32_t compiled_Pattern_দৈর্ঘ্য,
        int32_t* offsets,
        int32_t offsets_দৈর্ঘ্য) {
    জন্য (int32_t i = 0; i < offsets_দৈর্ঘ্য; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiled_Pattern_দৈর্ঘ্য - 1 -
            getArgumentLimit(compiledPattern, compiled_Pattern_দৈর্ঘ্য);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    জন্য (int32_t i = 1; i < compiled_Pattern_দৈর্ঘ্য;) {
        int32_t n = compiledPattern[i++];
        যদি (n > ARG_অঙ্ক_LIMIT) {
            n -= ARG_অঙ্ক_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } অন্যথা যদি (n < offsets_দৈর্ঘ্য) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    ফেরত sb;
}

UnicodeString& SimpleFormatter::format(
        ধ্রুবক char16_t* compiledPattern,
        int32_t compiled_Pattern_দৈর্ঘ্য,
        ধ্রুবক UnicodeString* ধ্রুবক* মানসমূহ,
        UnicodeString& ফলাফল U_LIFETIME_BOUND,
        ধ্রুবক UnicodeString* ফলাফল_Copy,
        UBool forbid_ফলাফল_As_মান,
        int32_t* offsets,
        int32_t offsets_দৈর্ঘ্য,
        UErrorCode& errorCode) {
    যদি (U_FAILURE(errorCode)) {
        ফেরত ফলাফল;
    }
    জন্য (int32_t i = 0; i < offsets_দৈর্ঘ্য; i++) {
        offsets[i] = -1;
    }
    জন্য (int32_t i = 1; i < compiled_Pattern_দৈর্ঘ্য;) {
        int32_t n = compiledPattern[i++];
        যদি (n < ARG_অঙ্ক_LIMIT) {
            ধ্রুবক UnicodeString *value = মানসমূহ[n];
            যদি (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                ফেরত ফলাফল;
            }
            যদি (value == &ফলাফল) {
                যদি (forbid_ফলাফল_As_মান) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    ফেরত ফলাফল;
                }
                যদি (i == 2) {
                    // We are appending to result which is also the first value object.
                    যদি (n < offsets_দৈর্ঘ্য) {
                        offsets[n] = 0;
                    }
                } অন্যথা {
                    যদি (n < offsets_দৈর্ঘ্য) {
                        offsets[n] = ফলাফল.length();
                    }
                    ফলাফল.append(*ফলাফল_Copy);
                }
            } অন্যথা {
                যদি (n < offsets_দৈর্ঘ্য) {
                    offsets[n] = ফলাফল.length();
                }
                ফলাফল.append(*value);
            }
        } অন্যথা {
            int32_t length = n - ARG_অঙ্ক_LIMIT;
            ফলাফল.append(compiledPattern + i, length);
            i += length;
        }
    }
    ফেরত ফলাফল;
}

U_NAMESPACE_END
