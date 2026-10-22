// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "mlbe.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ubrkimpl.h"
#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "util.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"

U_NAMESPACE_BEGIN

ಎಣಿಕೆ ವರ್ಗ Model_ಸೂಚ್ಯಂಕ { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &digit_Or_Open_Punctuation_Or_Alphabet_ಗಣ,
                             ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &close_Punctuation_ಗಣ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
    : f_Digit_Or_Open_Punctuation_Or_Alphabet_ಗಣ(digit_Or_Open_Punctuation_Or_Alphabet_ಗಣ),
      f_Close_Punctuation_ಗಣ(close_Punctuation_ಗಣ),
      f_Negative_ಮೊತ್ತ(0) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ತುಂಬು_MLModel(ಸ್ಥಿತಿ_2);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divide_ಮೇಲೆ_Range(UText *ಒಳ_ಪಠ್ಯ, int32_t range_ಆರಂಭ, int32_t range_ಕೊನೆ,
                                     UVector32 &ಸಿಕ್ಕಿತು_Breaks, ಸ್ಥಿರಾಂಕ UnicodeString &ಒಳ_String,
                                     ಸ್ಥಿರಾಂಕ Local_ಸೂಚಕ<UVector32> &ಒಳಹರಿವು_Map,
                                     UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (range_ಆರಂಭ >= range_ಕೊನೆ) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    UVector32 boundary(ಒಳ_String.ಎಣಿಕೆ_Char32() + 1, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t ಅಂಕೆ_Breaks = 0;
    int32_t ಸಂಕೇತ_ಬಿಂದು_ಉದ್ದಳತೆ = ಒಳ_String.ಎಣಿಕೆ_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t ಸೂಚ್ಯಂಕ_ಗಾತ್ರ = ಸಂಕೇತ_ಬಿಂದು_ಉದ್ದಳತೆ + 4;
    Local_ಸ್ಮರಣೆ<int32_t> ಸೂಚ್ಯಂಕ_List(static_cast<int32_t*>(uprv_malloc(ಸೂಚ್ಯಂಕ_ಗಾತ್ರ * ಗಾತ್ರ(int32_t))));
    ಆದರೆ (ಸೂಚ್ಯಂಕ_List.isNull()) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t ಅಂಕೆ_ಸಂಕೇತ_Units = init_ಸೂಚ್ಯಂಕ_List(ಒಳ_String, ಸೂಚ್ಯಂಕ_List.getAlias(), ಸ್ಥಿತಿ_2);

    // Add a break for the start.
    boundary.ಕೂಡಿಸು_Element(0, ಸ್ಥಿತಿ_2);
    ಅಂಕೆ_Breaks++;
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;

    ಗಾಗಿ (int32_t idx = 0; idx + 1 < ಸಂಕೇತ_ಬಿಂದು_ಉದ್ದಳತೆ && U_SUCCESS(ಸ್ಥಿತಿ_2); idx++) {
        ಅಂಕೆ_Breaks =
            evaluateBreakpoint(ಒಳ_String, ಸೂಚ್ಯಂಕ_List.getAlias(), idx, ಅಂಕೆ_ಸಂಕೇತ_Units, ಅಂಕೆ_Breaks, boundary, ಸ್ಥಿತಿ_2);
        ಆದರೆ (idx + 4 < ಸಂಕೇತ_ಬಿಂದು_ಉದ್ದಳತೆ) {
            ಸೂಚ್ಯಂಕ_List[idx + 6] = ಅಂಕೆ_ಸಂಕೇತ_Units;
            ಅಂಕೆ_ಸಂಕೇತ_Units += U16_ಉದ್ದಳತೆ(ಒಳ_String.char32At(ಸೂಚ್ಯಂಕ_List[idx + 6]));
        }
    }

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ 0;

    // Add a break for the end if there is not one there already.
    ಆದರೆ (boundary.ಕೊನೆಯ_Elementi() != ಒಳ_String.ಎಣಿಕೆ_Char32()) {
        boundary.ಕೂಡಿಸು_Element(ಒಳ_String.ಎಣಿಕೆ_Char32(), ಸ್ಥಿತಿ_2);
        ಅಂಕೆ_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_ಅಂಕೆ_Breaks = 0;
    ಗಾಗಿ (int32_t i = 0; i < ಅಂಕೆ_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = ಒಳಹರಿವು_Map.isValid() ? ಒಳಹರಿವು_Map->elementAti(cpPos) : cpPos + range_ಆರಂಭ;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        ಆದರೆ (utextPos > prevUTextPos) {
            ಆದರೆ (utextPos != range_ಆರಂಭ ||
                (utextPos > 0 &&
                 f_Close_Punctuation_ಗಣ.ಒಳಗೊಂಡಿದೆ(utext_char32At(ಒಳ_ಪಠ್ಯ, utextPos - 1)))) {
                ಸಿಕ್ಕಿತು_Breaks.push(utextPos, ಸ್ಥಿತಿ_2);
                corrected_ಅಂಕೆ_Breaks++;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --ಅಂಕೆ_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (ಶೂನ್ಯ)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 ಮುಂದಿನ_Char = utext_char32At(ಒಳ_ಪಠ್ಯ, range_ಕೊನೆ);
    ಆದರೆ (!ಸಿಕ್ಕಿತು_Breaks.is_ಖಾಲಿಗಳು() && ಸಿಕ್ಕಿತು_Breaks.peeki() == range_ಕೊನೆ) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        ಆದರೆ (!f_Digit_Or_Open_Punctuation_Or_Alphabet_ಗಣ.ಒಳಗೊಂಡಿದೆ(ಮುಂದಿನ_Char)) {
            ಸಿಕ್ಕಿತು_Breaks.popi();
            corrected_ಅಂಕೆ_Breaks--;
        }
    }

    ಹಿಂತಿರುಗಿ corrected_ಅಂಕೆ_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ಸ್ಥಿರಾಂಕ UnicodeString &ಒಳ_String, int32_t *ಸೂಚ್ಯಂಕ_List,
                                          int32_t ಆರಂಭ_Idx, int32_t ಅಂಕೆ_ಸಂಕೇತ_Units, int32_t ಅಂಕೆ_Breaks,
                                          UVector32 &boundary, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಅಂಕೆ_Breaks;
    }
    int32_t ಆರಂಭ = 0, ಕೊನೆ = 0;
    int32_t score = f_Negative_ಮೊತ್ತ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 6; i++) {
        // UW1 ~ UW6
        ಆರಂಭ = ಆರಂಭ_Idx + i;
        ಆದರೆ (ಸೂಚ್ಯಂಕ_List[ಆರಂಭ] != -1) {
            ಕೊನೆ = (ಸೂಚ್ಯಂಕ_List[ಆರಂಭ + 1] != -1) ? ಸೂಚ್ಯಂಕ_List[ಆರಂಭ + 1] : ಅಂಕೆ_ಸಂಕೇತ_Units;
            score += fModel[static_cast<int32_t>(Model_ಸೂಚ್ಯಂಕ::kUWStart) + i].geti(
                ಒಳ_String.ತಾತ್ಕಾಲಿಕ_Sub_String(ಸೂಚ್ಯಂಕ_List[ಆರಂಭ], ಕೊನೆ - ಸೂಚ್ಯಂಕ_List[ಆರಂಭ]));
        }
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++) {
        // BW1 ~ BW3
        ಆರಂಭ = ಆರಂಭ_Idx + i + 1;
        ಆದರೆ (ಸೂಚ್ಯಂಕ_List[ಆರಂಭ] != -1 && ಸೂಚ್ಯಂಕ_List[ಆರಂಭ + 1] != -1) {
            ಕೊನೆ = (ಸೂಚ್ಯಂಕ_List[ಆರಂಭ + 2] != -1) ? ಸೂಚ್ಯಂಕ_List[ಆರಂಭ + 2] : ಅಂಕೆ_ಸಂಕೇತ_Units;
            score += fModel[static_cast<int32_t>(Model_ಸೂಚ್ಯಂಕ::kBWStart) + i].geti(
                ಒಳ_String.ತಾತ್ಕಾಲಿಕ_Sub_String(ಸೂಚ್ಯಂಕ_List[ಆರಂಭ], ಕೊನೆ - ಸೂಚ್ಯಂಕ_List[ಆರಂಭ]));
        }
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 4; i++) {
        // TW1 ~ TW4
        ಆರಂಭ = ಆರಂಭ_Idx + i;
        ಆದರೆ (ಸೂಚ್ಯಂಕ_List[ಆರಂಭ] != -1 && ಸೂಚ್ಯಂಕ_List[ಆರಂಭ + 1] != -1 && ಸೂಚ್ಯಂಕ_List[ಆರಂಭ + 2] != -1) {
            ಕೊನೆ = (ಸೂಚ್ಯಂಕ_List[ಆರಂಭ + 3] != -1) ? ಸೂಚ್ಯಂಕ_List[ಆರಂಭ + 3] : ಅಂಕೆ_ಸಂಕೇತ_Units;
            score += fModel[static_cast<int32_t>(Model_ಸೂಚ್ಯಂಕ::kTWStart) + i].geti(
                ಒಳ_String.ತಾತ್ಕಾಲಿಕ_Sub_String(ಸೂಚ್ಯಂಕ_List[ಆರಂಭ], ಕೊನೆ - ಸೂಚ್ಯಂಕ_List[ಆರಂಭ]));
        }
    }

    ಆದರೆ (score > 0) {
        boundary.ಕೂಡಿಸು_Element(ಆರಂಭ_Idx + 1, ಸ್ಥಿತಿ_2);
        ಅಂಕೆ_Breaks++;
    }
    ಹಿಂತಿರುಗಿ ಅಂಕೆ_Breaks;
}

int32_t MlBreakEngine::init_ಸೂಚ್ಯಂಕ_List(ಸ್ಥಿರಾಂಕ UnicodeString &ಒಳ_String, int32_t *ಸೂಚ್ಯಂಕ_List,
                                     UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t ಸೂಚ್ಯಂಕ = 0;
    int32_t length = ಒಳ_String.ಎಣಿಕೆ_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(ಸೂಚ್ಯಂಕ_List, 0xff, (length + 4) * ಗಾತ್ರ(int32_t));
    ಆದರೆ (length > 0) {
        ಸೂಚ್ಯಂಕ_List[2] = 0;
        ಸೂಚ್ಯಂಕ = U16_ಉದ್ದಳತೆ(ಒಳ_String.char32At(0));
        ಆದರೆ (length > 1) {
            ಸೂಚ್ಯಂಕ_List[3] = ಸೂಚ್ಯಂಕ;
            ಸೂಚ್ಯಂಕ += U16_ಉದ್ದಳತೆ(ಒಳ_String.char32At(ಸೂಚ್ಯಂಕ));
            ಆದರೆ (length > 2) {
                ಸೂಚ್ಯಂಕ_List[4] = ಸೂಚ್ಯಂಕ;
                ಸೂಚ್ಯಂಕ += U16_ಉದ್ದಳತೆ(ಒಳ_String.char32At(ಸೂಚ್ಯಂಕ));
                ಆದರೆ (length > 3) {
                    ಸೂಚ್ಯಂಕ_List[5] = ಸೂಚ್ಯಂಕ;
                    ಸೂಚ್ಯಂಕ += U16_ಉದ್ದಳತೆ(ಒಳ_String.char32At(ಸೂಚ್ಯಂಕ));
                }
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
}

ಶೂನ್ಯ MlBreakEngine::ತುಂಬು_MLModel(UError_ಸಂಕೇತ &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    ಆದರೆ (U_FAILURE(error)) ಹಿಂತಿರುಗಿ;

    UnicodeString ಕೀಲಿ;
    ರಾಶಿ_UResource_Bundle ರಾಶಿ_ತಾತ್ಕಾಲಿಕ_Bundle;
    Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ model_ಕೀಲಿ;

    Local_UResource_Bundle_ಸೂಚಕ rbp(ures_open_ನೇರ(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    ಆದರೆ (U_FAILURE(error)) ಹಿಂತಿರುಗಿ;

    int32_t ಸೂಚ್ಯಂಕ = 0;
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW1Keys", "UW1Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW2Keys", "UW2Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW3Keys", "UW3Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW4Keys", "UW4Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW5Keys", "UW5Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW6Keys", "UW6Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "BW1Keys", "BW1Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "BW2Keys", "BW2Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "BW3Keys", "BW3Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "TW1Keys", "TW1Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "TW2Keys", "TW2Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "TW3Keys", "TW3Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "TW4Keys", "TW4Values", fModel[ಸೂಚ್ಯಂಕ++], error);
    f_Negative_ಮೊತ್ತ /= 2;
}

ಶೂನ್ಯ MlBreakEngine::init_ಕೀಲಿ_ಮೌಲ್ಯ(UResourceBundle *rb, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_Name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ_Name,
                                 Hashtable &model, UError_ಸಂಕೇತ &error) {
    int32_t ಕೀಲಿ_ಗಾತ್ರ = 0;
    int32_t ಮೌಲ್ಯ_ಗಾತ್ರ = 0;
    int32_t string_ಉದ್ದಳತೆ = 0;
    UnicodeString ಕೀಲಿ;
    ರಾಶಿ_UResource_Bundle ರಾಶಿ_ತಾತ್ಕಾಲಿಕ_Bundle;
    Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ model_ಕೀಲಿ;

    // get modelValues
    Local_UResource_Bundle_ಸೂಚಕ model_ಮೌಲ್ಯ(ures_get_By_ಕೀಲಿ(rb, ಮೌಲ್ಯ_Name, ಶೂನ್ಯಸೂಚಕ, &error));
    ಸ್ಥಿರಾಂಕ int32_t *value = ures_get_Int_ಸದಿಶ(model_ಮೌಲ್ಯ.getAlias(), &ಮೌಲ್ಯ_ಗಾತ್ರ, &error);
    ಆದರೆ (U_FAILURE(error)) ಹಿಂತಿರುಗಿ;

    // get modelKeys
    ures_get_ಮೌಲ್ಯ_With_Fallback(rb, ಕೀಲಿ_Name, ರಾಶಿ_ತಾತ್ಕಾಲಿಕ_Bundle.getAlias(), model_ಕೀಲಿ, error);
    ResourceArray stringArray = model_ಕೀಲಿ.getArray(error);
    ಕೀಲಿ_ಗಾತ್ರ = stringArray.get_ಗಾತ್ರ();
    ಆದರೆ (U_FAILURE(error)) ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (int32_t idx = 0; idx < ಕೀಲಿ_ಗಾತ್ರ; idx++) {
        stringArray.get_ಮೌಲ್ಯ(idx, model_ಕೀಲಿ);
        ಕೀಲಿ = UnicodeString(model_ಕೀಲಿ.getString(string_ಉದ್ದಳತೆ, error));
        ಆದರೆ (U_SUCCESS(error)) {
            U_ASSERT(idx < ಮೌಲ್ಯ_ಗಾತ್ರ);
            f_Negative_ಮೊತ್ತ -= value[idx];
            model.puti(ಕೀಲಿ, value[idx], error);
        }
    }
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */
