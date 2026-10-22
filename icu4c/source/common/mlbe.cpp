// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "cmemory.h"
#شامل "mlbe.h"
#شامل "uassert.h"
#شامل "ubrkimpl.h"
#شامل "unicode/resbund.h"
#شامل "unicode/udata.h"
#شامل "unicode/utf16.h"
#شامل "uresimp.h"
#شامل "util.h"
#شامل "uvectr32.h"

U_NAMESPACE_BEGIN

شمار جماعت Model_اشاریہ { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ثابت Unicode_سیٹ &digit_Or_Open_Punctuation_Or_Alphabet_سیٹ,
                             ثابت Unicode_سیٹ &close_Punctuation_سیٹ, UError_رمز &حالت_2)
    : f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ(digit_Or_Open_Punctuation_Or_Alphabet_سیٹ),
      f_Close_Punctuation_سیٹ(close_Punctuation_سیٹ),
      f_Negative_مجموع(0) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    بھرو_MLModel(حالت_2);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divide_اوپر_Range(UText *اندر_متن, int32_t range_آغاز, int32_t range_انتہا,
                                     UVector32 &ملا_Breaks, ثابت UnicodeString &اندر_String,
                                     ثابت Local_اشارہ_گر<UVector32> &داخلہ_Map,
                                     UError_رمز &حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) {
        واپس 0;
    }
    اگر (range_آغاز >= range_انتہا) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    UVector32 boundary(اندر_String.گنتی_Char32() + 1, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس 0;
    }
    int32_t عدد_Breaks = 0;
    int32_t رمز_نقطہ_لمبائی = اندر_String.گنتی_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t اشاریہ_سائز = رمز_نقطہ_لمبائی + 4;
    Local_حافظہ<int32_t> اشاریہ_List(static_cast<int32_t*>(uprv_malloc(اشاریہ_سائز * حجم(int32_t))));
    اگر (اشاریہ_List.isNull()) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس 0;
    }
    int32_t عدد_رمز_Units = init_اشاریہ_List(اندر_String, اشاریہ_List.getAlias(), حالت_2);

    // Add a break for the start.
    boundary.جوڑو_Element(0, حالت_2);
    عدد_Breaks++;
    اگر (U_FAILURE(حالت_2)) واپس 0;

    برائے (int32_t idx = 0; idx + 1 < رمز_نقطہ_لمبائی && U_SUCCESS(حالت_2); idx++) {
        عدد_Breaks =
            evaluateBreakpoint(اندر_String, اشاریہ_List.getAlias(), idx, عدد_رمز_Units, عدد_Breaks, boundary, حالت_2);
        اگر (idx + 4 < رمز_نقطہ_لمبائی) {
            اشاریہ_List[idx + 6] = عدد_رمز_Units;
            عدد_رمز_Units += U16_لمبائی(اندر_String.char32At(اشاریہ_List[idx + 6]));
        }
    }

    اگر (U_FAILURE(حالت_2)) واپس 0;

    // Add a break for the end if there is not one there already.
    اگر (boundary.آخری_Elementi() != اندر_String.گنتی_Char32()) {
        boundary.جوڑو_Element(اندر_String.گنتی_Char32(), حالت_2);
        عدد_Breaks++;
    }

    int32_t پچھلا_CPPos = -1;
    int32_t پچھلا_UText_Pos = -1;
    int32_t corrected_عدد_Breaks = 0;
    برائے (int32_t i = 0; i < عدد_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = داخلہ_Map.isValid() ? داخلہ_Map->elementAti(cpPos) : cpPos + range_آغاز;
        U_ASSERT(cpPos > پچھلا_CPPos);
        U_ASSERT(utextPos >= پچھلا_UText_Pos);

        اگر (utextPos > پچھلا_UText_Pos) {
            اگر (utextPos != range_آغاز ||
                (utextPos > 0 &&
                 f_Close_Punctuation_سیٹ.شامل_ہے(utext_char32At(اندر_متن, utextPos - 1)))) {
                ملا_Breaks.push(utextPos, حالت_2);
                corrected_عدد_Breaks++;
            }
        } ورنہ {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --عدد_Breaks;
        }
        پچھلا_CPPos = cpPos;
        پچھلا_UText_Pos = utextPos;
    }
    (خالی)پچھلا_CPPos;  // suppress compiler warnings about unused variable

    UChar32 اگلا_Char = utext_char32At(اندر_متن, range_انتہا);
    اگر (!ملا_Breaks.is_خالی_خانے() && ملا_Breaks.peeki() == range_انتہا) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        اگر (!f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ.شامل_ہے(اگلا_Char)) {
            ملا_Breaks.popi();
            corrected_عدد_Breaks--;
        }
    }

    واپس corrected_عدد_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ثابت UnicodeString &اندر_String, int32_t *اشاریہ_List,
                                          int32_t آغاز_Idx, int32_t عدد_رمز_Units, int32_t عدد_Breaks,
                                          UVector32 &boundary, UError_رمز &حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) {
        واپس عدد_Breaks;
    }
    int32_t آغاز = 0, انتہا = 0;
    int32_t score = f_Negative_مجموع;

    برائے (صحیح i = 0; i < 6; i++) {
        // UW1 ~ UW6
        آغاز = آغاز_Idx + i;
        اگر (اشاریہ_List[آغاز] != -1) {
            انتہا = (اشاریہ_List[آغاز + 1] != -1) ? اشاریہ_List[آغاز + 1] : عدد_رمز_Units;
            score += fModel[static_cast<int32_t>(Model_اشاریہ::kUWStart) + i].geti(
                اندر_String.عارضی_Sub_String(اشاریہ_List[آغاز], انتہا - اشاریہ_List[آغاز]));
        }
    }
    برائے (صحیح i = 0; i < 3; i++) {
        // BW1 ~ BW3
        آغاز = آغاز_Idx + i + 1;
        اگر (اشاریہ_List[آغاز] != -1 && اشاریہ_List[آغاز + 1] != -1) {
            انتہا = (اشاریہ_List[آغاز + 2] != -1) ? اشاریہ_List[آغاز + 2] : عدد_رمز_Units;
            score += fModel[static_cast<int32_t>(Model_اشاریہ::kBWStart) + i].geti(
                اندر_String.عارضی_Sub_String(اشاریہ_List[آغاز], انتہا - اشاریہ_List[آغاز]));
        }
    }
    برائے (صحیح i = 0; i < 4; i++) {
        // TW1 ~ TW4
        آغاز = آغاز_Idx + i;
        اگر (اشاریہ_List[آغاز] != -1 && اشاریہ_List[آغاز + 1] != -1 && اشاریہ_List[آغاز + 2] != -1) {
            انتہا = (اشاریہ_List[آغاز + 3] != -1) ? اشاریہ_List[آغاز + 3] : عدد_رمز_Units;
            score += fModel[static_cast<int32_t>(Model_اشاریہ::kTWStart) + i].geti(
                اندر_String.عارضی_Sub_String(اشاریہ_List[آغاز], انتہا - اشاریہ_List[آغاز]));
        }
    }

    اگر (score > 0) {
        boundary.جوڑو_Element(آغاز_Idx + 1, حالت_2);
        عدد_Breaks++;
    }
    واپس عدد_Breaks;
}

int32_t MlBreakEngine::init_اشاریہ_List(ثابت UnicodeString &اندر_String, int32_t *اشاریہ_List,
                                     UError_رمز &حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) {
        واپس 0;
    }
    int32_t اشاریہ = 0;
    int32_t length = اندر_String.گنتی_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(اشاریہ_List, 0xff, (length + 4) * حجم(int32_t));
    اگر (length > 0) {
        اشاریہ_List[2] = 0;
        اشاریہ = U16_لمبائی(اندر_String.char32At(0));
        اگر (length > 1) {
            اشاریہ_List[3] = اشاریہ;
            اشاریہ += U16_لمبائی(اندر_String.char32At(اشاریہ));
            اگر (length > 2) {
                اشاریہ_List[4] = اشاریہ;
                اشاریہ += U16_لمبائی(اندر_String.char32At(اشاریہ));
                اگر (length > 3) {
                    اشاریہ_List[5] = اشاریہ;
                    اشاریہ += U16_لمبائی(اندر_String.char32At(اشاریہ));
                }
            }
        }
    }
    واپس اشاریہ;
}

خالی MlBreakEngine::بھرو_MLModel(UError_رمز &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    اگر (U_FAILURE(error)) واپس;

    UnicodeString کلید;
    ڈھیر_UResource_Bundle ڈھیر_عارضی_Bundle;
    Resource_ڈیٹا_قدر model_کلید;

    Local_UResource_Bundle_اشارہ_گر rbp(ures_open_براہ_راست(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    اگر (U_FAILURE(error)) واپس;

    int32_t اشاریہ = 0;
    init_کلید_قدر(rb, "UW1Keys", "UW1Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "UW2Keys", "UW2Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "UW3Keys", "UW3Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "UW4Keys", "UW4Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "UW5Keys", "UW5Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "UW6Keys", "UW6Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "BW1Keys", "BW1Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "BW2Keys", "BW2Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "BW3Keys", "BW3Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "TW1Keys", "TW1Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "TW2Keys", "TW2Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "TW3Keys", "TW3Values", fModel[اشاریہ++], error);
    init_کلید_قدر(rb, "TW4Keys", "TW4Values", fModel[اشاریہ++], error);
    f_Negative_مجموع /= 2;
}

خالی MlBreakEngine::init_کلید_قدر(UResourceBundle *rb, ثابت حرف *کلید_Name, ثابت حرف *قدر_Name,
                                 Hashtable &model, UError_رمز &error) {
    int32_t کلید_سائز = 0;
    int32_t قدر_سائز = 0;
    int32_t string_لمبائی = 0;
    UnicodeString کلید;
    ڈھیر_UResource_Bundle ڈھیر_عارضی_Bundle;
    Resource_ڈیٹا_قدر model_کلید;

    // get modelValues
    Local_UResource_Bundle_اشارہ_گر model_قدر(ures_get_By_کلید(rb, قدر_Name, خالی_اشاریہ, &error));
    ثابت int32_t *value = ures_get_Int_سمتیہ(model_قدر.getAlias(), &قدر_سائز, &error);
    اگر (U_FAILURE(error)) واپس;

    // get modelKeys
    ures_get_قدر_With_Fallback(rb, کلید_Name, ڈھیر_عارضی_Bundle.getAlias(), model_کلید, error);
    ResourceArray stringArray = model_کلید.getArray(error);
    کلید_سائز = stringArray.get_سائز();
    اگر (U_FAILURE(error)) واپس;

    برائے (int32_t idx = 0; idx < کلید_سائز; idx++) {
        stringArray.get_قدر(idx, model_کلید);
        کلید = UnicodeString(model_کلید.getString(string_لمبائی, error));
        اگر (U_SUCCESS(error)) {
            U_ASSERT(idx < قدر_سائز);
            f_Negative_مجموع -= value[idx];
            model.puti(کلید, value[idx], error);
        }
    }
}

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */
