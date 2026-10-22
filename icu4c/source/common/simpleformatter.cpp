// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#समावेश "unicode/utypes.h"
#समावेश "unicode/simpleformatter.h"
#समावेश "unicode/unistr.h"
#समावेश "uassert.h"

U_NAMESPACE_BEGIN

namespace {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
स्थिर int32_t ARG_अंक_LIMIT = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
स्थिर char16_t SEGMENT_लंबाई_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
स्थिर int32_t MAX_SEGMENT_लंबाई = SEGMENT_लंबाई_PLACEHOLDER_CHAR - ARG_अंक_LIMIT;

क्रमागत {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

अंतरभूत UBool isInvalidArray(स्थिर व्योम *array, int32_t length) {
   वापस (length < 0 || (array == nullptr && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::चालक=(स्थिर SimpleFormatter& other) U_LIFETIME_BOUND {
    अगर (यह == &other) {
        वापस *यह;
    }
    compiledPattern = other.compiledPattern;
    वापस *यह;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::applyPatternMinMaxArguments(
        स्थिर UnicodeString &pattern,
        int32_t min, int32_t max,
        UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) {
        वापस false;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    स्थिर char16_t *pattern_बफर = pattern.get_बफर();
    int32_t pattern_लंबाई = pattern.length();
    // Reserve the first char for the number of arguments.
    compiledPattern.setTo(static_cast<char16_t>(0));
    int32_t text_लंबाई = 0;
    int32_t maxArg = -1;
    UBool inQuote = false;
    क्रम (int32_t i = 0; i < pattern_लंबाई;) {
        char16_t c = pattern_बफर[i++];
        अगर (c == APOS) {
            अगर (i < pattern_लंबाई && (c = pattern_बफर[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } अन्यथा अगर (inQuote) {
                // skip the quote-ending apostrophe
                inQuote = false;
                जारी;
            } अन्यथा अगर (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                inQuote = true;
            } अन्यथा {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } अन्यथा अगर (!inQuote && c == OPEN_BRACE) {
            अगर (text_लंबाई > 0) {
                compiledPattern.setCharAt(compiledPattern.length() - text_लंबाई - 1,
                                          static_cast<char16_t>(ARG_अंक_LIMIT + text_लंबाई));
                text_लंबाई = 0;
            }
            int32_t arg_संख्या;
            अगर ((i + 1) < pattern_लंबाई &&
                    0 <= (arg_संख्या = pattern_बफर[i] - DIGIT_ZERO) && arg_संख्या <= 9 &&
                    pattern_बफर[i + 1] == CLOSE_BRACE) {
                i += 2;
            } अन्यथा {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_संख्या = -1;
                अगर (i < pattern_लंबाई && DIGIT_ONE <= (c = pattern_बफर[i++]) && c <= DIGIT_NINE) {
                    arg_संख्या = c - DIGIT_ZERO;
                    जबतक (i < pattern_लंबाई &&
                            DIGIT_ZERO <= (c = pattern_बफर[i++]) && c <= DIGIT_NINE) {
                        arg_संख्या = arg_संख्या * 10 + (c - DIGIT_ZERO);
                        अगर (arg_संख्या >= ARG_अंक_LIMIT) {
                            अवरोध;
                        }
                    }
                }
                अगर (arg_संख्या < 0 || c != CLOSE_BRACE) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    वापस false;
                }
            }
            अगर (arg_संख्या > maxArg) {
                maxArg = arg_संख्या;
            }
            compiledPattern.append(static_cast<char16_t>(arg_संख्या));
            जारी;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        अगर (text_लंबाई == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiledPattern.append(SEGMENT_लंबाई_PLACEHOLDER_CHAR);
        }
        compiledPattern.append(c);
        अगर (++text_लंबाई == MAX_SEGMENT_लंबाई) {
            text_लंबाई = 0;
        }
    }
    अगर (text_लंबाई > 0) {
        compiledPattern.setCharAt(compiledPattern.length() - text_लंबाई - 1,
                                  static_cast<char16_t>(ARG_अंक_LIMIT + text_लंबाई));
    }
    int32_t arg_गिनती = maxArg + 1;
    अगर (arg_गिनती < min || max < arg_गिनती) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }
    compiledPattern.setCharAt(0, static_cast<char16_t>(arg_गिनती));
    वापस true;
}

UnicodeString& SimpleFormatter::format(
        स्थिर UnicodeString& value0,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) स्थिर {
    स्थिर UnicodeString *मूल्यावली[] = { &value0 };
    वापस formatAndAppend(मूल्यावली, 1, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        स्थिर UnicodeString& value0,
        स्थिर UnicodeString& value1,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) स्थिर {
    स्थिर UnicodeString *मूल्यावली[] = { &value0, &value1 };
    वापस formatAndAppend(मूल्यावली, 2, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::format(
        स्थिर UnicodeString& value0,
        स्थिर UnicodeString& value1,
        स्थिर UnicodeString& value2,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        UErrorCode& errorCode) स्थिर {
    स्थिर UnicodeString *मूल्यावली[] = { &value0, &value1, &value2 };
    वापस formatAndAppend(मूल्यावली, 3, appendTo, nullptr, 0, errorCode);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        स्थिर UnicodeString* स्थिर* मूल्यावली,
        int32_t मूल्यावली_लंबाई,
        UnicodeString& appendTo U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_लंबाई,
        UErrorCode& errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) {
        वापस appendTo;
    }
    अगर (isInvalidArray(मूल्यावली, मूल्यावली_लंबाई) || isInvalidArray(offsets, offsets_लंबाई) ||
            मूल्यावली_लंबाई < getArgumentLimit()) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस appendTo;
    }
    वापस format(compiledPattern.get_बफर(), compiledPattern.length(), मूल्यावली,
                  appendTo, nullptr, true,
                  offsets, offsets_लंबाई, errorCode);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        स्थिर UnicodeString* स्थिर* मूल्यावली,
        int32_t मूल्यावली_लंबाई,
        UnicodeString& परिणाम U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_लंबाई,
        UErrorCode& errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) {
        वापस परिणाम;
    }
    अगर (isInvalidArray(मूल्यावली, मूल्यावली_लंबाई) || isInvalidArray(offsets, offsets_लंबाई)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस परिणाम;
    }
    स्थिर char16_t *cp = compiledPattern.get_बफर();
    int32_t cp_लंबाई = compiledPattern.length();
    अगर (मूल्यावली_लंबाई < getArgumentLimit(cp, cp_लंबाई)) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस परिणाम;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString परिणाम_Copy;
    अगर (getArgumentLimit(cp, cp_लंबाई) > 0) {
        क्रम (int32_t i = 1; i < cp_लंबाई;) {
            int32_t n = cp[i++];
            अगर (n < ARG_अंक_LIMIT) {
                अगर (मूल्यावली[n] == &परिणाम) {
                    अगर (i == 2) {
                        firstArg = n;
                    } अन्यथा अगर (परिणाम_Copy.isEmpty() && !परिणाम.isEmpty()) {
                        परिणाम_Copy = परिणाम;
                    }
                }
            } अन्यथा {
                i += n - ARG_अंक_LIMIT;
            }
        }
    }
    अगर (firstArg < 0) {
        परिणाम.हटाओ();
    }
    वापस format(cp, cp_लंबाई, मूल्यावली,
                  परिणाम, &परिणाम_Copy, false,
                  offsets, offsets_लंबाई, errorCode);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        स्थिर char16_t *compiledPattern,
        int32_t compiled_Pattern_लंबाई,
        int32_t* offsets,
        int32_t offsets_लंबाई) {
    क्रम (int32_t i = 0; i < offsets_लंबाई; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiled_Pattern_लंबाई - 1 -
            getArgumentLimit(compiledPattern, compiled_Pattern_लंबाई);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    क्रम (int32_t i = 1; i < compiled_Pattern_लंबाई;) {
        int32_t n = compiledPattern[i++];
        अगर (n > ARG_अंक_LIMIT) {
            n -= ARG_अंक_LIMIT;
            sb.append(compiledPattern + i, n);
            i += n;
        } अन्यथा अगर (n < offsets_लंबाई) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    वापस sb;
}

UnicodeString& SimpleFormatter::format(
        स्थिर char16_t* compiledPattern,
        int32_t compiled_Pattern_लंबाई,
        स्थिर UnicodeString* स्थिर* मूल्यावली,
        UnicodeString& परिणाम U_LIFETIME_BOUND,
        स्थिर UnicodeString* परिणाम_Copy,
        UBool forbid_परिणाम_As_मूल्य,
        int32_t* offsets,
        int32_t offsets_लंबाई,
        UErrorCode& errorCode) {
    अगर (U_FAILURE(errorCode)) {
        वापस परिणाम;
    }
    क्रम (int32_t i = 0; i < offsets_लंबाई; i++) {
        offsets[i] = -1;
    }
    क्रम (int32_t i = 1; i < compiled_Pattern_लंबाई;) {
        int32_t n = compiledPattern[i++];
        अगर (n < ARG_अंक_LIMIT) {
            स्थिर UnicodeString *value = मूल्यावली[n];
            अगर (value == nullptr) {
                errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                वापस परिणाम;
            }
            अगर (value == &परिणाम) {
                अगर (forbid_परिणाम_As_मूल्य) {
                    errorCode = U_ILLEGAL_ARGUMENT_ERROR;
                    वापस परिणाम;
                }
                अगर (i == 2) {
                    // We are appending to result which is also the first value object.
                    अगर (n < offsets_लंबाई) {
                        offsets[n] = 0;
                    }
                } अन्यथा {
                    अगर (n < offsets_लंबाई) {
                        offsets[n] = परिणाम.length();
                    }
                    परिणाम.append(*परिणाम_Copy);
                }
            } अन्यथा {
                अगर (n < offsets_लंबाई) {
                    offsets[n] = परिणाम.length();
                }
                परिणाम.append(*value);
            }
        } अन्यथा {
            int32_t length = n - ARG_अंक_LIMIT;
            परिणाम.append(compiledPattern + i, length);
            i += length;
        }
    }
    वापस परिणाम;
}

U_NAMESPACE_END
