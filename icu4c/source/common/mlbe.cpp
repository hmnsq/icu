// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "cmemory.h"
#ضمن "mlbe.h"
#ضمن "uassert.h"
#ضمن "ubrkimpl.h"
#ضمن "unicode/resbund.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/utf16.h"
#ضمن "uresimp.h"
#ضمن "util.h"
#ضمن "uvectr32.h"

U_NAMESPACE_BEGIN

تعداد صنف Model_اشاریہ { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ثابت Unicode_سیٹ &digit_Or_Open_Punctuation_Or_Alphabet_سیٹ,
                             ثابت Unicode_سیٹ &close_Punctuation_سیٹ, UError_کوڈ &حالت)
    : f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ(digit_Or_Open_Punctuation_Or_Alphabet_سیٹ),
      f_Close_Punctuation_سیٹ(close_Punctuation_سیٹ),
      f_Negative_جوڑ(0) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    لدو_MLModel(حالت);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divide_اتے_Range(UText *وچ_متن, int32_t range_شروع, int32_t range_سرا,
                                     UVector32 &لبھیا_Breaks, ثابت UnicodeString &وچ_String,
                                     ثابت Local_اشارہ_گر<UVector32> &داخلہ_Map,
                                     UError_کوڈ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    إذا (range_شروع >= range_سرا) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    UVector32 boundary(وچ_String.گنتی_Char32() + 1, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    int32_t عدد_Breaks = 0;
    int32_t کوڈ_نقطہ_لمائی = وچ_String.گنتی_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t اشاریہ_ناپ = کوڈ_نقطہ_لمائی + 4;
    Local_حافظہ<int32_t> اشاریہ_List(static_cast<int32_t*>(uprv_malloc(اشاریہ_ناپ * حجم(int32_t))));
    إذا (اشاریہ_List.isNull()) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع 0;
    }
    int32_t عدد_کوڈ_Units = init_اشاریہ_List(وچ_String, اشاریہ_List.getAlias(), حالت);

    // Add a break for the start.
    boundary.جوڑو_Element(0, حالت);
    عدد_Breaks++;
    إذا (U_FAILURE(حالت)) أرجع 0;

    لكل (int32_t idx = 0; idx + 1 < کوڈ_نقطہ_لمائی && U_SUCCESS(حالت); idx++) {
        عدد_Breaks =
            evaluateBreakpoint(وچ_String, اشاریہ_List.getAlias(), idx, عدد_کوڈ_Units, عدد_Breaks, boundary, حالت);
        إذا (idx + 4 < کوڈ_نقطہ_لمائی) {
            اشاریہ_List[idx + 6] = عدد_کوڈ_Units;
            عدد_کوڈ_Units += U16_لمائی(وچ_String.char32At(اشاریہ_List[idx + 6]));
        }
    }

    إذا (U_FAILURE(حالت)) أرجع 0;

    // Add a break for the end if there is not one there already.
    إذا (boundary.آخری_Elementi() != وچ_String.گنتی_Char32()) {
        boundary.جوڑو_Element(وچ_String.گنتی_Char32(), حالت);
        عدد_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_عدد_Breaks = 0;
    لكل (int32_t i = 0; i < عدد_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = داخلہ_Map.isValid() ? داخلہ_Map->elementAti(cpPos) : cpPos + range_شروع;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        إذا (utextPos > prevUTextPos) {
            إذا (utextPos != range_شروع ||
                (utextPos > 0 &&
                 f_Close_Punctuation_سیٹ.شامل_اے(utext_char32At(وچ_متن, utextPos - 1)))) {
                لبھیا_Breaks.push(utextPos, حالت);
                corrected_عدد_Breaks++;
            }
        } وإلا {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --عدد_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (فراغ)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 اگلا_Char = utext_char32At(وچ_متن, range_سرا);
    إذا (!لبھیا_Breaks.is_خالی_تھاواں() && لبھیا_Breaks.peeki() == range_سرا) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        إذا (!f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ.شامل_اے(اگلا_Char)) {
            لبھیا_Breaks.popi();
            corrected_عدد_Breaks--;
        }
    }

    أرجع corrected_عدد_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ثابت UnicodeString &وچ_String, int32_t *اشاریہ_List,
                                          int32_t شروع_Idx, int32_t عدد_کوڈ_Units, int32_t عدد_Breaks,
                                          UVector32 &boundary, UError_کوڈ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع عدد_Breaks;
    }
    int32_t شروع = 0, سرا = 0;
    int32_t score = f_Negative_جوڑ;

    لكل (صحيح i = 0; i < 6; i++) {
        // UW1 ~ UW6
        شروع = شروع_Idx + i;
        إذا (اشاریہ_List[شروع] != -1) {
            سرا = (اشاریہ_List[شروع + 1] != -1) ? اشاریہ_List[شروع + 1] : عدد_کوڈ_Units;
            score += fModel[static_cast<int32_t>(Model_اشاریہ::kUWStart) + i].geti(
                وچ_String.عارضی_Sub_String(اشاریہ_List[شروع], سرا - اشاریہ_List[شروع]));
        }
    }
    لكل (صحيح i = 0; i < 3; i++) {
        // BW1 ~ BW3
        شروع = شروع_Idx + i + 1;
        إذا (اشاریہ_List[شروع] != -1 && اشاریہ_List[شروع + 1] != -1) {
            سرا = (اشاریہ_List[شروع + 2] != -1) ? اشاریہ_List[شروع + 2] : عدد_کوڈ_Units;
            score += fModel[static_cast<int32_t>(Model_اشاریہ::kBWStart) + i].geti(
                وچ_String.عارضی_Sub_String(اشاریہ_List[شروع], سرا - اشاریہ_List[شروع]));
        }
    }
    لكل (صحيح i = 0; i < 4; i++) {
        // TW1 ~ TW4
        شروع = شروع_Idx + i;
        إذا (اشاریہ_List[شروع] != -1 && اشاریہ_List[شروع + 1] != -1 && اشاریہ_List[شروع + 2] != -1) {
            سرا = (اشاریہ_List[شروع + 3] != -1) ? اشاریہ_List[شروع + 3] : عدد_کوڈ_Units;
            score += fModel[static_cast<int32_t>(Model_اشاریہ::kTWStart) + i].geti(
                وچ_String.عارضی_Sub_String(اشاریہ_List[شروع], سرا - اشاریہ_List[شروع]));
        }
    }

    إذا (score > 0) {
        boundary.جوڑو_Element(شروع_Idx + 1, حالت);
        عدد_Breaks++;
    }
    أرجع عدد_Breaks;
}

int32_t MlBreakEngine::init_اشاریہ_List(ثابت UnicodeString &وچ_String, int32_t *اشاریہ_List,
                                     UError_کوڈ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    int32_t اشاریہ = 0;
    int32_t length = وچ_String.گنتی_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(اشاریہ_List, 0xff, (length + 4) * حجم(int32_t));
    إذا (length > 0) {
        اشاریہ_List[2] = 0;
        اشاریہ = U16_لمائی(وچ_String.char32At(0));
        إذا (length > 1) {
            اشاریہ_List[3] = اشاریہ;
            اشاریہ += U16_لمائی(وچ_String.char32At(اشاریہ));
            إذا (length > 2) {
                اشاریہ_List[4] = اشاریہ;
                اشاریہ += U16_لمائی(وچ_String.char32At(اشاریہ));
                إذا (length > 3) {
                    اشاریہ_List[5] = اشاریہ;
                    اشاریہ += U16_لمائی(وچ_String.char32At(اشاریہ));
                }
            }
        }
    }
    أرجع اشاریہ;
}

فراغ MlBreakEngine::لدو_MLModel(UError_کوڈ &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    إذا (U_FAILURE(error)) أرجع;

    UnicodeString کلید;
    ڈھیر_UResource_Bundle ڈھیر_عارضی_Bundle;
    Resource_انکڑے_قدر model_کلید;

    Local_UResource_Bundle_اشارہ_گر rbp(ures_open_سدھا(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    إذا (U_FAILURE(error)) أرجع;

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
    f_Negative_جوڑ /= 2;
}

فراغ MlBreakEngine::init_کلید_قدر(UResourceBundle *rb, ثابت حرف *کلید_Name, ثابت حرف *قدر_Name,
                                 Hashtable &model, UError_کوڈ &error) {
    int32_t کلید_ناپ = 0;
    int32_t قدر_ناپ = 0;
    int32_t string_لمائی = 0;
    UnicodeString کلید;
    ڈھیر_UResource_Bundle ڈھیر_عارضی_Bundle;
    Resource_انکڑے_قدر model_کلید;

    // get modelValues
    Local_UResource_Bundle_اشارہ_گر model_قدر(ures_get_By_کلید(rb, قدر_Name, خالی_سوچک, &error));
    ثابت int32_t *value = ures_get_Int_سمتیہ(model_قدر.getAlias(), &قدر_ناپ, &error);
    إذا (U_FAILURE(error)) أرجع;

    // get modelKeys
    ures_get_قدر_With_Fallback(rb, کلید_Name, ڈھیر_عارضی_Bundle.getAlias(), model_کلید, error);
    ResourceArray stringArray = model_کلید.getArray(error);
    کلید_ناپ = stringArray.get_ناپ();
    إذا (U_FAILURE(error)) أرجع;

    لكل (int32_t idx = 0; idx < کلید_ناپ; idx++) {
        stringArray.get_قدر(idx, model_کلید);
        کلید = UnicodeString(model_کلید.getString(string_لمائی, error));
        إذا (U_SUCCESS(error)) {
            U_ASSERT(idx < قدر_ناپ);
            f_Negative_جوڑ -= value[idx];
            model.puti(کلید, value[idx], error);
        }
    }
}

U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */
