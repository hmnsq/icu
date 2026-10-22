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
ध्रुव int32_t ARG_अङ्कः_सीमा = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
ध्रुव char16_t SEGMENT_दीर्घता_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
ध्रुव int32_t MAX_SEGMENT_दीर्घता = SEGMENT_दीर्घता_PLACEHOLDER_CHAR - ARG_अङ्कः_सीमा;

गणना {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

अन्तःस्थ UBool isInvalidArray(ध्रुव शून्य *array, int32_t length) {
   प्रत्यागम (length < 0 || (array == शून्यसूचकः && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::संक्रियकः=(ध्रुव SimpleFormatter& other) U_LIFETIME_BOUND {
    यदि (एतत् == &other) {
        प्रत्यागम *एतत्;
    }
    compiled_आदर्शः = other.compiled_आदर्शः;
    प्रत्यागम *एतत्;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::apply_आदर्शः_Min_Max_Arguments(
        ध्रुव UnicodeString &आदर्शः,
        int32_t min, int32_t max,
        UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम असत्य;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    ध्रुव char16_t *आदर्शः_बफर = आदर्शः.get_बफर();
    int32_t आदर्शः_दीर्घता = आदर्शः.length();
    // Reserve the first char for the number of arguments.
    compiled_आदर्शः.समुच्चयः_गन्तव्यम्(static_cast<char16_t>(0));
    int32_t text_दीर्घता = 0;
    int32_t maxArg = -1;
    UBool अन्तर्_Quote = असत्य;
    प्रति (int32_t i = 0; i < आदर्शः_दीर्घता;) {
        char16_t c = आदर्शः_बफर[i++];
        यदि (c == APOS) {
            यदि (i < आदर्शः_दीर्घता && (c = आदर्शः_बफर[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } अन्यथा यदि (अन्तर्_Quote) {
                // skip the quote-ending apostrophe
                अन्तर्_Quote = असत्य;
                अनुवर्त;
            } अन्यथा यदि (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                अन्तर्_Quote = सत्य;
            } अन्यथा {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } अन्यथा यदि (!अन्तर्_Quote && c == OPEN_BRACE) {
            यदि (text_दीर्घता > 0) {
                compiled_आदर्शः.समुच्चयः_Char_At(compiled_आदर्शः.length() - text_दीर्घता - 1,
                                          static_cast<char16_t>(ARG_अङ्कः_सीमा + text_दीर्घता));
                text_दीर्घता = 0;
            }
            int32_t arg_सङ्ख्या;
            यदि ((i + 1) < आदर्शः_दीर्घता &&
                    0 <= (arg_सङ्ख्या = आदर्शः_बफर[i] - DIGIT_ZERO) && arg_सङ्ख्या <= 9 &&
                    आदर्शः_बफर[i + 1] == CLOSE_BRACE) {
                i += 2;
            } अन्यथा {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_सङ्ख्या = -1;
                यदि (i < आदर्शः_दीर्घता && DIGIT_ONE <= (c = आदर्शः_बफर[i++]) && c <= DIGIT_NINE) {
                    arg_सङ्ख्या = c - DIGIT_ZERO;
                    यावत् (i < आदर्शः_दीर्घता &&
                            DIGIT_ZERO <= (c = आदर्शः_बफर[i++]) && c <= DIGIT_NINE) {
                        arg_सङ्ख्या = arg_सङ्ख्या * 10 + (c - DIGIT_ZERO);
                        यदि (arg_सङ्ख्या >= ARG_अङ्कः_सीमा) {
                            भङ्ग;
                        }
                    }
                }
                यदि (arg_सङ्ख्या < 0 || c != CLOSE_BRACE) {
                    error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                    प्रत्यागम असत्य;
                }
            }
            यदि (arg_सङ्ख्या > maxArg) {
                maxArg = arg_सङ्ख्या;
            }
            compiled_आदर्शः.append(static_cast<char16_t>(arg_सङ्ख्या));
            अनुवर्त;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        यदि (text_दीर्घता == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiled_आदर्शः.append(SEGMENT_दीर्घता_PLACEHOLDER_CHAR);
        }
        compiled_आदर्शः.append(c);
        यदि (++text_दीर्घता == MAX_SEGMENT_दीर्घता) {
            text_दीर्घता = 0;
        }
    }
    यदि (text_दीर्घता > 0) {
        compiled_आदर्शः.समुच्चयः_Char_At(compiled_आदर्शः.length() - text_दीर्घता - 1,
                                  static_cast<char16_t>(ARG_अङ्कः_सीमा + text_दीर्घता));
    }
    int32_t arg_गणनम् = maxArg + 1;
    यदि (arg_गणनम् < min || max < arg_गणनम्) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम असत्य;
    }
    compiled_आदर्शः.समुच्चयः_Char_At(0, static_cast<char16_t>(arg_गणनम्));
    प्रत्यागम सत्य;
}

UnicodeString& SimpleFormatter::format(
        ध्रुव UnicodeString& value0,
        UnicodeString& append_गन्तव्यम् U_LIFETIME_BOUND,
        UError_संहिता& error_संहिता) ध्रुव {
    ध्रुव UnicodeString *मानानि[] = { &value0 };
    प्रत्यागम formatAndAppend(मानानि, 1, append_गन्तव्यम्, शून्यसूचकः, 0, error_संहिता);
}

UnicodeString& SimpleFormatter::format(
        ध्रुव UnicodeString& value0,
        ध्रुव UnicodeString& value1,
        UnicodeString& append_गन्तव्यम् U_LIFETIME_BOUND,
        UError_संहिता& error_संहिता) ध्रुव {
    ध्रुव UnicodeString *मानानि[] = { &value0, &value1 };
    प्रत्यागम formatAndAppend(मानानि, 2, append_गन्तव्यम्, शून्यसूचकः, 0, error_संहिता);
}

UnicodeString& SimpleFormatter::format(
        ध्रुव UnicodeString& value0,
        ध्रुव UnicodeString& value1,
        ध्रुव UnicodeString& value2,
        UnicodeString& append_गन्तव्यम् U_LIFETIME_BOUND,
        UError_संहिता& error_संहिता) ध्रुव {
    ध्रुव UnicodeString *मानानि[] = { &value0, &value1, &value2 };
    प्रत्यागम formatAndAppend(मानानि, 3, append_गन्तव्यम्, शून्यसूचकः, 0, error_संहिता);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        ध्रुव UnicodeString* ध्रुव* मानानि,
        int32_t मानानि_दीर्घता,
        UnicodeString& append_गन्तव्यम् U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_दीर्घता,
        UError_संहिता& error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम append_गन्तव्यम्;
    }
    यदि (isInvalidArray(मानानि, मानानि_दीर्घता) || isInvalidArray(offsets, offsets_दीर्घता) ||
            मानानि_दीर्घता < get_तर्कः_सीमा()) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम append_गन्तव्यम्;
    }
    प्रत्यागम format(compiled_आदर्शः.get_बफर(), compiled_आदर्शः.length(), मानानि,
                  append_गन्तव्यम्, शून्यसूचकः, सत्य,
                  offsets, offsets_दीर्घता, error_संहिता);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        ध्रुव UnicodeString* ध्रुव* मानानि,
        int32_t मानानि_दीर्घता,
        UnicodeString& परिणाम U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_दीर्घता,
        UError_संहिता& error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम परिणाम;
    }
    यदि (isInvalidArray(मानानि, मानानि_दीर्घता) || isInvalidArray(offsets, offsets_दीर्घता)) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम परिणाम;
    }
    ध्रुव char16_t *cp = compiled_आदर्शः.get_बफर();
    int32_t cp_दीर्घता = compiled_आदर्शः.length();
    यदि (मानानि_दीर्घता < get_तर्कः_सीमा(cp, cp_दीर्घता)) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम परिणाम;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t firstArg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString परिणाम_Copy;
    यदि (get_तर्कः_सीमा(cp, cp_दीर्घता) > 0) {
        प्रति (int32_t i = 1; i < cp_दीर्घता;) {
            int32_t n = cp[i++];
            यदि (n < ARG_अङ्कः_सीमा) {
                यदि (मानानि[n] == &परिणाम) {
                    यदि (i == 2) {
                        firstArg = n;
                    } अन्यथा यदि (परिणाम_Copy.is_रिक्तानि() && !परिणाम.is_रिक्तानि()) {
                        परिणाम_Copy = परिणाम;
                    }
                }
            } अन्यथा {
                i += n - ARG_अङ्कः_सीमा;
            }
        }
    }
    यदि (firstArg < 0) {
        परिणाम.remove();
    }
    प्रत्यागम format(cp, cp_दीर्घता, मानानि,
                  परिणाम, &परिणाम_Copy, असत्य,
                  offsets, offsets_दीर्घता, error_संहिता);
}

UnicodeString SimpleFormatter::getTextWithNoArguments(
        ध्रुव char16_t *compiled_आदर्शः,
        int32_t compiled_आदर्शः_दीर्घता,
        int32_t* offsets,
        int32_t offsets_दीर्घता) {
    प्रति (int32_t i = 0; i < offsets_दीर्घता; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiled_आदर्शः_दीर्घता - 1 -
            get_तर्कः_सीमा(compiled_आदर्शः, compiled_आदर्शः_दीर्घता);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    प्रति (int32_t i = 1; i < compiled_आदर्शः_दीर्घता;) {
        int32_t n = compiled_आदर्शः[i++];
        यदि (n > ARG_अङ्कः_सीमा) {
            n -= ARG_अङ्कः_सीमा;
            sb.append(compiled_आदर्शः + i, n);
            i += n;
        } अन्यथा यदि (n < offsets_दीर्घता) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    प्रत्यागम sb;
}

UnicodeString& SimpleFormatter::format(
        ध्रुव char16_t* compiled_आदर्शः,
        int32_t compiled_आदर्शः_दीर्घता,
        ध्रुव UnicodeString* ध्रुव* मानानि,
        UnicodeString& परिणाम U_LIFETIME_BOUND,
        ध्रुव UnicodeString* परिणाम_Copy,
        UBool forbid_परिणाम_As_मानम्,
        int32_t* offsets,
        int32_t offsets_दीर्घता,
        UError_संहिता& error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम परिणाम;
    }
    प्रति (int32_t i = 0; i < offsets_दीर्घता; i++) {
        offsets[i] = -1;
    }
    प्रति (int32_t i = 1; i < compiled_आदर्शः_दीर्घता;) {
        int32_t n = compiled_आदर्शः[i++];
        यदि (n < ARG_अङ्कः_सीमा) {
            ध्रुव UnicodeString *value = मानानि[n];
            यदि (value == शून्यसूचकः) {
                error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                प्रत्यागम परिणाम;
            }
            यदि (value == &परिणाम) {
                यदि (forbid_परिणाम_As_मानम्) {
                    error_संहिता = U_ILLEGAL_तर्कः_ERROR;
                    प्रत्यागम परिणाम;
                }
                यदि (i == 2) {
                    // We are appending to result which is also the first value object.
                    यदि (n < offsets_दीर्घता) {
                        offsets[n] = 0;
                    }
                } अन्यथा {
                    यदि (n < offsets_दीर्घता) {
                        offsets[n] = परिणाम.length();
                    }
                    परिणाम.append(*परिणाम_Copy);
                }
            } अन्यथा {
                यदि (n < offsets_दीर्घता) {
                    offsets[n] = परिणाम.length();
                }
                परिणाम.append(*value);
            }
        } अन्यथा {
            int32_t length = n - ARG_अङ्कः_सीमा;
            परिणाम.append(compiled_आदर्शः + i, length);
            i += length;
        }
    }
    प्रत्यागम परिणाम;
}

U_NAMESPACE_अन्तः
