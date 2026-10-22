// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* simpleformatter.cpp
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/simpleformatter.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "uassert.h"

U_NAMESPACE_BEGIN

నామాకాశ {

/**
 * Argument numbers must be smaller than this limit.
 * Text segment lengths are offset by this much.
 * This is currently the only unused char value in compiled patterns,
 * except it is the maximum value of the first unit (max arg +1).
 */
స్థిరరాశి int32_t ARG_అంకె_హద్దు = 0x100;
/**
 * Initial and maximum char/char16_t value set for a text segment.
 * Segment length char values are from ARG_NUM_LIMIT+1 to this value here.
 * Normally 0xffff, but can be as small as ARG_NUM_LIMIT+1 for testing.
 */
స్థిరరాశి char16_t SEGMENT_నిడివి_PLACEHOLDER_CHAR = 0xffff;
/**
 * Maximum length of a text segment. Longer segments are split into shorter ones.
 */
స్థిరరాశి int32_t MAX_SEGMENT_నిడివి = SEGMENT_నిడివి_PLACEHOLDER_CHAR - ARG_అంకె_హద్దు;

గణన {
    APOS = 0x27,
    DIGIT_ZERO = 0x30,
    DIGIT_ONE = 0x31,
    DIGIT_NINE = 0x39,
    OPEN_BRACE = 0x7b,
    CLOSE_BRACE = 0x7d
};

అంతఃవరుస UBool isInvalidArray(స్థిరరాశి శూన్య *array, int32_t length) {
   తిరిగి (length < 0 || (array == శూన్యసూచిక && length != 0));
}

}  // namespace

SimpleFormatter& SimpleFormatter::క్రియాకారకం=(స్థిరరాశి SimpleFormatter& other) U_LIFETIME_BOUND {
    ఒకవేళ (ఇది == &other) {
        తిరిగి *ఇది;
    }
    compiled_నమూనా = other.compiled_నమూనా;
    తిరిగి *ఇది;
}

SimpleFormatter::~SimpleFormatter() {}

UBool SimpleFormatter::apply_నమూనా_Min_Max_Arguments(
        స్థిరరాశి UnicodeString &నమూనా,
        int32_t min, int32_t max,
        UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    // Parse consistent with MessagePattern, but
    // - support only simple numbered arguments
    // - build a simple binary structure into the result string
    స్థిరరాశి char16_t *నమూనా_బఫరు = నమూనా.get_బఫరు();
    int32_t నమూనా_నిడివి = నమూనా.length();
    // Reserve the first char for the number of arguments.
    compiled_నమూనా.సమితి_గమ్యం(static_cast<char16_t>(0));
    int32_t పాఠ్యం_నిడివి = 0;
    int32_t maxArg = -1;
    UBool లోపల_Quote = అబద్ధం;
    కొరకు (int32_t i = 0; i < నమూనా_నిడివి;) {
        char16_t c = నమూనా_బఫరు[i++];
        ఒకవేళ (c == APOS) {
            ఒకవేళ (i < నమూనా_నిడివి && (c = నమూనా_బఫరు[i]) == APOS) {
                // double apostrophe, skip the second one
                ++i;
            } లేకపోతే ఒకవేళ (లోపల_Quote) {
                // skip the quote-ending apostrophe
                లోపల_Quote = అబద్ధం;
                కొనసాగు;
            } లేకపోతే ఒకవేళ (c == OPEN_BRACE || c == CLOSE_BRACE) {
                // Skip the quote-starting apostrophe, find the end of the quoted literal text.
                ++i;
                లోపల_Quote = నిజం;
            } లేకపోతే {
                // The apostrophe is part of literal text.
                c = APOS;
            }
        } లేకపోతే ఒకవేళ (!లోపల_Quote && c == OPEN_BRACE) {
            ఒకవేళ (పాఠ్యం_నిడివి > 0) {
                compiled_నమూనా.సమితి_Char_At(compiled_నమూనా.length() - పాఠ్యం_నిడివి - 1,
                                          static_cast<char16_t>(ARG_అంకె_హద్దు + పాఠ్యం_నిడివి));
                పాఠ్యం_నిడివి = 0;
            }
            int32_t arg_సంఖ్య;
            ఒకవేళ ((i + 1) < నమూనా_నిడివి &&
                    0 <= (arg_సంఖ్య = నమూనా_బఫరు[i] - DIGIT_ZERO) && arg_సంఖ్య <= 9 &&
                    నమూనా_బఫరు[i + 1] == CLOSE_BRACE) {
                i += 2;
            } లేకపోతే {
                // Multi-digit argument number (no leading zero) or syntax error.
                // MessagePattern permits PatternProps.skipWhiteSpace(pattern, index)
                // around the number, but this class does not.
                arg_సంఖ్య = -1;
                ఒకవేళ (i < నమూనా_నిడివి && DIGIT_ONE <= (c = నమూనా_బఫరు[i++]) && c <= DIGIT_NINE) {
                    arg_సంఖ్య = c - DIGIT_ZERO;
                    అయితే (i < నమూనా_నిడివి &&
                            DIGIT_ZERO <= (c = నమూనా_బఫరు[i++]) && c <= DIGIT_NINE) {
                        arg_సంఖ్య = arg_సంఖ్య * 10 + (c - DIGIT_ZERO);
                        ఒకవేళ (arg_సంఖ్య >= ARG_అంకె_హద్దు) {
                            ఆపు;
                        }
                    }
                }
                ఒకవేళ (arg_సంఖ్య < 0 || c != CLOSE_BRACE) {
                    error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                    తిరిగి అబద్ధం;
                }
            }
            ఒకవేళ (arg_సంఖ్య > maxArg) {
                maxArg = arg_సంఖ్య;
            }
            compiled_నమూనా.append(static_cast<char16_t>(arg_సంఖ్య));
            కొనసాగు;
        }  // else: c is part of literal text
        // Append c and track the literal-text segment length.
        ఒకవేళ (పాఠ్యం_నిడివి == 0) {
            // Reserve a char for the length of a new text segment, preset the maximum length.
            compiled_నమూనా.append(SEGMENT_నిడివి_PLACEHOLDER_CHAR);
        }
        compiled_నమూనా.append(c);
        ఒకవేళ (++పాఠ్యం_నిడివి == MAX_SEGMENT_నిడివి) {
            పాఠ్యం_నిడివి = 0;
        }
    }
    ఒకవేళ (పాఠ్యం_నిడివి > 0) {
        compiled_నమూనా.సమితి_Char_At(compiled_నమూనా.length() - పాఠ్యం_నిడివి - 1,
                                  static_cast<char16_t>(ARG_అంకె_హద్దు + పాఠ్యం_నిడివి));
    }
    int32_t arg_లెక్క = maxArg + 1;
    ఒకవేళ (arg_లెక్క < min || max < arg_లెక్క) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి అబద్ధం;
    }
    compiled_నమూనా.సమితి_Char_At(0, static_cast<char16_t>(arg_లెక్క));
    తిరిగి నిజం;
}

UnicodeString& SimpleFormatter::format(
        స్థిరరాశి UnicodeString& value0,
        UnicodeString& append_గమ్యం U_LIFETIME_BOUND,
        UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    స్థిరరాశి UnicodeString *విలువలు[] = { &value0 };
    తిరిగి formatAndAppend(విలువలు, 1, append_గమ్యం, శూన్యసూచిక, 0, error_సంకేతం);
}

UnicodeString& SimpleFormatter::format(
        స్థిరరాశి UnicodeString& value0,
        స్థిరరాశి UnicodeString& value1,
        UnicodeString& append_గమ్యం U_LIFETIME_BOUND,
        UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    స్థిరరాశి UnicodeString *విలువలు[] = { &value0, &value1 };
    తిరిగి formatAndAppend(విలువలు, 2, append_గమ్యం, శూన్యసూచిక, 0, error_సంకేతం);
}

UnicodeString& SimpleFormatter::format(
        స్థిరరాశి UnicodeString& value0,
        స్థిరరాశి UnicodeString& value1,
        స్థిరరాశి UnicodeString& value2,
        UnicodeString& append_గమ్యం U_LIFETIME_BOUND,
        UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    స్థిరరాశి UnicodeString *విలువలు[] = { &value0, &value1, &value2 };
    తిరిగి formatAndAppend(విలువలు, 3, append_గమ్యం, శూన్యసూచిక, 0, error_సంకేతం);
}

UnicodeString& SimpleFormatter::formatAndAppend(
        స్థిరరాశి UnicodeString* స్థిరరాశి* విలువలు,
        int32_t విలువలు_నిడివి,
        UnicodeString& append_గమ్యం U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_నిడివి,
        UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి append_గమ్యం;
    }
    ఒకవేళ (isInvalidArray(విలువలు, విలువలు_నిడివి) || isInvalidArray(offsets, offsets_నిడివి) ||
            విలువలు_నిడివి < get_వాదం_హద్దు()) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి append_గమ్యం;
    }
    తిరిగి format(compiled_నమూనా.get_బఫరు(), compiled_నమూనా.length(), విలువలు,
                  append_గమ్యం, శూన్యసూచిక, నిజం,
                  offsets, offsets_నిడివి, error_సంకేతం);
}

UnicodeString& SimpleFormatter::formatAndReplace(
        స్థిరరాశి UnicodeString* స్థిరరాశి* విలువలు,
        int32_t విలువలు_నిడివి,
        UnicodeString& ఫలితం U_LIFETIME_BOUND,
        int32_t* offsets,
        int32_t offsets_నిడివి,
        UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి ఫలితం;
    }
    ఒకవేళ (isInvalidArray(విలువలు, విలువలు_నిడివి) || isInvalidArray(offsets, offsets_నిడివి)) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి ఫలితం;
    }
    స్థిరరాశి char16_t *cp = compiled_నమూనా.get_బఫరు();
    int32_t cp_నిడివి = compiled_నమూనా.length();
    ఒకవేళ (విలువలు_నిడివి < get_వాదం_హద్దు(cp, cp_నిడివి)) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి ఫలితం;
    }

    // If the pattern starts with an argument whose value is the same object
    // as the result, then we keep the result contents and append to it.
    // Otherwise we replace its contents.
    int32_t మొదటి_Arg = -1;
    // If any non-initial argument value is the same object as the result,
    // then we first copy its contents and use that instead while formatting.
    UnicodeString ఫలితం_Copy;
    ఒకవేళ (get_వాదం_హద్దు(cp, cp_నిడివి) > 0) {
        కొరకు (int32_t i = 1; i < cp_నిడివి;) {
            int32_t n = cp[i++];
            ఒకవేళ (n < ARG_అంకె_హద్దు) {
                ఒకవేళ (విలువలు[n] == &ఫలితం) {
                    ఒకవేళ (i == 2) {
                        మొదటి_Arg = n;
                    } లేకపోతే ఒకవేళ (ఫలితం_Copy.is_ఖాళీలు() && !ఫలితం.is_ఖాళీలు()) {
                        ఫలితం_Copy = ఫలితం;
                    }
                }
            } లేకపోతే {
                i += n - ARG_అంకె_హద్దు;
            }
        }
    }
    ఒకవేళ (మొదటి_Arg < 0) {
        ఫలితం.remove();
    }
    తిరిగి format(cp, cp_నిడివి, విలువలు,
                  ఫలితం, &ఫలితం_Copy, అబద్ధం,
                  offsets, offsets_నిడివి, error_సంకేతం);
}

UnicodeString SimpleFormatter::get_పాఠ్యం_With_No_Arguments(
        స్థిరరాశి char16_t *compiled_నమూనా,
        int32_t compiled_నమూనా_నిడివి,
        int32_t* offsets,
        int32_t offsets_నిడివి) {
    కొరకు (int32_t i = 0; i < offsets_నిడివి; i++) {
        offsets[i] = -1;
    }
    int32_t capacity = compiled_నమూనా_నిడివి - 1 -
            get_వాదం_హద్దు(compiled_నమూనా, compiled_నమూనా_నిడివి);
    UnicodeString sb(capacity, 0, 0);  // Java: StringBuilder
    కొరకు (int32_t i = 1; i < compiled_నమూనా_నిడివి;) {
        int32_t n = compiled_నమూనా[i++];
        ఒకవేళ (n > ARG_అంకె_హద్దు) {
            n -= ARG_అంకె_హద్దు;
            sb.append(compiled_నమూనా + i, n);
            i += n;
        } లేకపోతే ఒకవేళ (n < offsets_నిడివి) {
            // TODO(ICU-20406): This does not distinguish between "{0}{1}" and "{1}{0}".
            // Consider removing this function and replacing it with an iterator interface.
            offsets[n] = sb.length();
        }
    }
    తిరిగి sb;
}

UnicodeString& SimpleFormatter::format(
        స్థిరరాశి char16_t* compiled_నమూనా,
        int32_t compiled_నమూనా_నిడివి,
        స్థిరరాశి UnicodeString* స్థిరరాశి* విలువలు,
        UnicodeString& ఫలితం U_LIFETIME_BOUND,
        స్థిరరాశి UnicodeString* ఫలితం_Copy,
        UBool forbid_ఫలితం_As_విలువ,
        int32_t* offsets,
        int32_t offsets_నిడివి,
        UError_సంకేతం& error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి ఫలితం;
    }
    కొరకు (int32_t i = 0; i < offsets_నిడివి; i++) {
        offsets[i] = -1;
    }
    కొరకు (int32_t i = 1; i < compiled_నమూనా_నిడివి;) {
        int32_t n = compiled_నమూనా[i++];
        ఒకవేళ (n < ARG_అంకె_హద్దు) {
            స్థిరరాశి UnicodeString *value = విలువలు[n];
            ఒకవేళ (value == శూన్యసూచిక) {
                error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                తిరిగి ఫలితం;
            }
            ఒకవేళ (value == &ఫలితం) {
                ఒకవేళ (forbid_ఫలితం_As_విలువ) {
                    error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
                    తిరిగి ఫలితం;
                }
                ఒకవేళ (i == 2) {
                    // We are appending to result which is also the first value object.
                    ఒకవేళ (n < offsets_నిడివి) {
                        offsets[n] = 0;
                    }
                } లేకపోతే {
                    ఒకవేళ (n < offsets_నిడివి) {
                        offsets[n] = ఫలితం.length();
                    }
                    ఫలితం.append(*ఫలితం_Copy);
                }
            } లేకపోతే {
                ఒకవేళ (n < offsets_నిడివి) {
                    offsets[n] = ఫలితం.length();
                }
                ఫలితం.append(*value);
            }
        } లేకపోతే {
            int32_t length = n - ARG_అంకె_హద్దు;
            ఫలితం.append(compiled_నమూనా + i, length);
            i += length;
        }
    }
    తిరిగి ఫలితం;
}

U_NAMESPACE_చివర
