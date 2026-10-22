// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mlbe.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ubrkimpl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/resbund.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"

U_NAMESPACE_BEGIN

ਗਿਣਤੀ ਵਰਗ Model_ਸੂਚਕਾਂਕ { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ਅਚਲ Unicode_ਸੈੱਟ &digit_Or_Open_Punctuation_Or_Alphabet_ਸੈੱਟ,
                             ਅਚਲ Unicode_ਸੈੱਟ &close_Punctuation_ਸੈੱਟ, UError_ਕੋਡ &ਸਥਿਤੀ)
    : f_Digit_Or_Open_Punctuation_Or_Alphabet_ਸੈੱਟ(digit_Or_Open_Punctuation_Or_Alphabet_ਸੈੱਟ),
      f_Close_Punctuation_ਸੈੱਟ(close_Punctuation_ਸੈੱਟ),
      f_Negative_ਜੋੜ(0) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਲੱਦੋ_MLModel(ਸਥਿਤੀ);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divide_ਉੱਪਰ_Range(UText *ਵਿੱਚ_ਪਾਠ, int32_t range_ਸ਼ੁਰੂ, int32_t range_ਸਿਰਾ,
                                     UVector32 &ਮਿਲਿਆ_Breaks, ਅਚਲ UnicodeString &ਵਿੱਚ_String,
                                     ਅਚਲ Local_ਸੂਚਕ<UVector32> &ਨਿਵੇਸ਼_Map,
                                     UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ (range_ਸ਼ੁਰੂ >= range_ਸਿਰਾ) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    UVector32 boundary(ਵਿੱਚ_String.ਗਿਣਤੀ_Char32() + 1, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    int32_t ਅੰਕ_Breaks = 0;
    int32_t ਕੋਡ_ਬਿੰਦੂ_ਲੰਮਾਈ = ਵਿੱਚ_String.ਗਿਣਤੀ_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t ਸੂਚਕਾਂਕ_ਮਾਪ = ਕੋਡ_ਬਿੰਦੂ_ਲੰਮਾਈ + 4;
    Local_ਸਮ੍ਰਿਤੀ<int32_t> ਸੂਚਕਾਂਕ_List(static_cast<int32_t*>(uprv_malloc(ਸੂਚਕਾਂਕ_ਮਾਪ * ਆਕਾਰ(int32_t))));
    ਜੇ (ਸੂਚਕਾਂਕ_List.isNull()) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ 0;
    }
    int32_t ਅੰਕ_ਕੋਡ_Units = init_ਸੂਚਕਾਂਕ_List(ਵਿੱਚ_String, ਸੂਚਕਾਂਕ_List.getAlias(), ਸਥਿਤੀ);

    // Add a break for the start.
    boundary.ਜੋੜੋ_Element(0, ਸਥਿਤੀ);
    ਅੰਕ_Breaks++;
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;

    ਲਈ (int32_t idx = 0; idx + 1 < ਕੋਡ_ਬਿੰਦੂ_ਲੰਮਾਈ && U_SUCCESS(ਸਥਿਤੀ); idx++) {
        ਅੰਕ_Breaks =
            evaluateBreakpoint(ਵਿੱਚ_String, ਸੂਚਕਾਂਕ_List.getAlias(), idx, ਅੰਕ_ਕੋਡ_Units, ਅੰਕ_Breaks, boundary, ਸਥਿਤੀ);
        ਜੇ (idx + 4 < ਕੋਡ_ਬਿੰਦੂ_ਲੰਮਾਈ) {
            ਸੂਚਕਾਂਕ_List[idx + 6] = ਅੰਕ_ਕੋਡ_Units;
            ਅੰਕ_ਕੋਡ_Units += U16_ਲੰਮਾਈ(ਵਿੱਚ_String.char32At(ਸੂਚਕਾਂਕ_List[idx + 6]));
        }
    }

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) ਵਾਪਸ 0;

    // Add a break for the end if there is not one there already.
    ਜੇ (boundary.ਆਖਰੀ_Elementi() != ਵਿੱਚ_String.ਗਿਣਤੀ_Char32()) {
        boundary.ਜੋੜੋ_Element(ਵਿੱਚ_String.ਗਿਣਤੀ_Char32(), ਸਥਿਤੀ);
        ਅੰਕ_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_ਅੰਕ_Breaks = 0;
    ਲਈ (int32_t i = 0; i < ਅੰਕ_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = ਨਿਵੇਸ਼_Map.isValid() ? ਨਿਵੇਸ਼_Map->elementAti(cpPos) : cpPos + range_ਸ਼ੁਰੂ;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        ਜੇ (utextPos > prevUTextPos) {
            ਜੇ (utextPos != range_ਸ਼ੁਰੂ ||
                (utextPos > 0 &&
                 f_Close_Punctuation_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(utext_char32At(ਵਿੱਚ_ਪਾਠ, utextPos - 1)))) {
                ਮਿਲਿਆ_Breaks.push(utextPos, ਸਥਿਤੀ);
                corrected_ਅੰਕ_Breaks++;
            }
        } ਨਹੀਂਤਾਂ {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --ਅੰਕ_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (ਖਾਲੀ)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 ਅਗਲਾ_Char = utext_char32At(ਵਿੱਚ_ਪਾਠ, range_ਸਿਰਾ);
    ਜੇ (!ਮਿਲਿਆ_Breaks.is_ਖਾਲੀ_ਥਾਵਾਂ() && ਮਿਲਿਆ_Breaks.peeki() == range_ਸਿਰਾ) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        ਜੇ (!f_Digit_Or_Open_Punctuation_Or_Alphabet_ਸੈੱਟ.ਸ਼ਾਮਲ_ਹੈ(ਅਗਲਾ_Char)) {
            ਮਿਲਿਆ_Breaks.popi();
            corrected_ਅੰਕ_Breaks--;
        }
    }

    ਵਾਪਸ corrected_ਅੰਕ_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ਅਚਲ UnicodeString &ਵਿੱਚ_String, int32_t *ਸੂਚਕਾਂਕ_List,
                                          int32_t ਸ਼ੁਰੂ_Idx, int32_t ਅੰਕ_ਕੋਡ_Units, int32_t ਅੰਕ_Breaks,
                                          UVector32 &boundary, UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਅੰਕ_Breaks;
    }
    int32_t ਸ਼ੁਰੂ = 0, ਸਿਰਾ = 0;
    int32_t score = f_Negative_ਜੋੜ;

    ਲਈ (ਪੂਰਨ i = 0; i < 6; i++) {
        // UW1 ~ UW6
        ਸ਼ੁਰੂ = ਸ਼ੁਰੂ_Idx + i;
        ਜੇ (ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ] != -1) {
            ਸਿਰਾ = (ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ + 1] != -1) ? ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ + 1] : ਅੰਕ_ਕੋਡ_Units;
            score += fModel[static_cast<int32_t>(Model_ਸੂਚਕਾਂਕ::kUWStart) + i].geti(
                ਵਿੱਚ_String.ਆਰਜ਼ੀ_Sub_String(ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ], ਸਿਰਾ - ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ]));
        }
    }
    ਲਈ (ਪੂਰਨ i = 0; i < 3; i++) {
        // BW1 ~ BW3
        ਸ਼ੁਰੂ = ਸ਼ੁਰੂ_Idx + i + 1;
        ਜੇ (ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ] != -1 && ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ + 1] != -1) {
            ਸਿਰਾ = (ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ + 2] != -1) ? ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ + 2] : ਅੰਕ_ਕੋਡ_Units;
            score += fModel[static_cast<int32_t>(Model_ਸੂਚਕਾਂਕ::kBWStart) + i].geti(
                ਵਿੱਚ_String.ਆਰਜ਼ੀ_Sub_String(ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ], ਸਿਰਾ - ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ]));
        }
    }
    ਲਈ (ਪੂਰਨ i = 0; i < 4; i++) {
        // TW1 ~ TW4
        ਸ਼ੁਰੂ = ਸ਼ੁਰੂ_Idx + i;
        ਜੇ (ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ] != -1 && ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ + 1] != -1 && ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ + 2] != -1) {
            ਸਿਰਾ = (ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ + 3] != -1) ? ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ + 3] : ਅੰਕ_ਕੋਡ_Units;
            score += fModel[static_cast<int32_t>(Model_ਸੂਚਕਾਂਕ::kTWStart) + i].geti(
                ਵਿੱਚ_String.ਆਰਜ਼ੀ_Sub_String(ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ], ਸਿਰਾ - ਸੂਚਕਾਂਕ_List[ਸ਼ੁਰੂ]));
        }
    }

    ਜੇ (score > 0) {
        boundary.ਜੋੜੋ_Element(ਸ਼ੁਰੂ_Idx + 1, ਸਥਿਤੀ);
        ਅੰਕ_Breaks++;
    }
    ਵਾਪਸ ਅੰਕ_Breaks;
}

int32_t MlBreakEngine::init_ਸੂਚਕਾਂਕ_List(ਅਚਲ UnicodeString &ਵਿੱਚ_String, int32_t *ਸੂਚਕਾਂਕ_List,
                                     UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ 0;
    }
    int32_t ਸੂਚਕਾਂਕ = 0;
    int32_t length = ਵਿੱਚ_String.ਗਿਣਤੀ_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(ਸੂਚਕਾਂਕ_List, 0xff, (length + 4) * ਆਕਾਰ(int32_t));
    ਜੇ (length > 0) {
        ਸੂਚਕਾਂਕ_List[2] = 0;
        ਸੂਚਕਾਂਕ = U16_ਲੰਮਾਈ(ਵਿੱਚ_String.char32At(0));
        ਜੇ (length > 1) {
            ਸੂਚਕਾਂਕ_List[3] = ਸੂਚਕਾਂਕ;
            ਸੂਚਕਾਂਕ += U16_ਲੰਮਾਈ(ਵਿੱਚ_String.char32At(ਸੂਚਕਾਂਕ));
            ਜੇ (length > 2) {
                ਸੂਚਕਾਂਕ_List[4] = ਸੂਚਕਾਂਕ;
                ਸੂਚਕਾਂਕ += U16_ਲੰਮਾਈ(ਵਿੱਚ_String.char32At(ਸੂਚਕਾਂਕ));
                ਜੇ (length > 3) {
                    ਸੂਚਕਾਂਕ_List[5] = ਸੂਚਕਾਂਕ;
                    ਸੂਚਕਾਂਕ += U16_ਲੰਮਾਈ(ਵਿੱਚ_String.char32At(ਸੂਚਕਾਂਕ));
                }
            }
        }
    }
    ਵਾਪਸ ਸੂਚਕਾਂਕ;
}

ਖਾਲੀ MlBreakEngine::ਲੱਦੋ_MLModel(UError_ਕੋਡ &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    ਜੇ (U_FAILURE(error)) ਵਾਪਸ;

    UnicodeString ਕੁੰਜੀ;
    ਢੇਰ_UResource_Bundle ਢੇਰ_ਆਰਜ਼ੀ_Bundle;
    Resource_ਅੰਕੜੇ_ਮੁੱਲ model_ਕੁੰਜੀ;

    Local_UResource_Bundle_ਸੂਚਕ rbp(ures_open_ਸਿੱਧਾ(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    ਜੇ (U_FAILURE(error)) ਵਾਪਸ;

    int32_t ਸੂਚਕਾਂਕ = 0;
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "UW1Keys", "UW1Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "UW2Keys", "UW2Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "UW3Keys", "UW3Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "UW4Keys", "UW4Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "UW5Keys", "UW5Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "UW6Keys", "UW6Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "BW1Keys", "BW1Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "BW2Keys", "BW2Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "BW3Keys", "BW3Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "TW1Keys", "TW1Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "TW2Keys", "TW2Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "TW3Keys", "TW3Values", fModel[ਸੂਚਕਾਂਕ++], error);
    init_ਕੁੰਜੀ_ਮੁੱਲ(rb, "TW4Keys", "TW4Values", fModel[ਸੂਚਕਾਂਕ++], error);
    f_Negative_ਜੋੜ /= 2;
}

ਖਾਲੀ MlBreakEngine::init_ਕੁੰਜੀ_ਮੁੱਲ(UResourceBundle *rb, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_Name, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ_Name,
                                 Hashtable &model, UError_ਕੋਡ &error) {
    int32_t ਕੁੰਜੀ_ਮਾਪ = 0;
    int32_t ਮੁੱਲ_ਮਾਪ = 0;
    int32_t string_ਲੰਮਾਈ = 0;
    UnicodeString ਕੁੰਜੀ;
    ਢੇਰ_UResource_Bundle ਢੇਰ_ਆਰਜ਼ੀ_Bundle;
    Resource_ਅੰਕੜੇ_ਮੁੱਲ model_ਕੁੰਜੀ;

    // get modelValues
    Local_UResource_Bundle_ਸੂਚਕ model_ਮੁੱਲ(ures_get_By_ਕੁੰਜੀ(rb, ਮੁੱਲ_Name, ਖਾਲੀ_ਸੂਚਕ, &error));
    ਅਚਲ int32_t *value = ures_get_Int_ਸਦਿਸ਼(model_ਮੁੱਲ.getAlias(), &ਮੁੱਲ_ਮਾਪ, &error);
    ਜੇ (U_FAILURE(error)) ਵਾਪਸ;

    // get modelKeys
    ures_get_ਮੁੱਲ_With_Fallback(rb, ਕੁੰਜੀ_Name, ਢੇਰ_ਆਰਜ਼ੀ_Bundle.getAlias(), model_ਕੁੰਜੀ, error);
    ResourceArray stringArray = model_ਕੁੰਜੀ.getArray(error);
    ਕੁੰਜੀ_ਮਾਪ = stringArray.get_ਮਾਪ();
    ਜੇ (U_FAILURE(error)) ਵਾਪਸ;

    ਲਈ (int32_t idx = 0; idx < ਕੁੰਜੀ_ਮਾਪ; idx++) {
        stringArray.get_ਮੁੱਲ(idx, model_ਕੁੰਜੀ);
        ਕੁੰਜੀ = UnicodeString(model_ਕੁੰਜੀ.getString(string_ਲੰਮਾਈ, error));
        ਜੇ (U_SUCCESS(error)) {
            U_ASSERT(idx < ਮੁੱਲ_ਮਾਪ);
            f_Negative_ਜੋੜ -= value[idx];
            model.puti(ਕੁੰਜੀ, value[idx], error);
        }
    }
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */
