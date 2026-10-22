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

تعداد صنف Model_شاخص { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ثابت Unicode_سېټ &digit_Or_Open_Punctuation_Or_Alphabet_سېټ,
                             ثابت Unicode_سېټ &close_Punctuation_سېټ, UError_کوډ &حالت)
    : f_Digit_Or_Open_Punctuation_Or_Alphabet_سېټ(digit_Or_Open_Punctuation_Or_Alphabet_سېټ),
      f_Close_Punctuation_سېټ(close_Punctuation_سېټ),
      f_Negative_مجموعه(0) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    بارول_MLModel(حالت);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divide_پورته_Range(UText *دننه_متن, int32_t range_پیل, int32_t range_پای_ټکی,
                                     UVector32 &وموندل_Breaks, ثابت UnicodeString &دننه_String,
                                     ثابت Local_اشاره_ګر<UVector32> &ننوت_Map,
                                     UError_کوډ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    إذا (range_پیل >= range_پای_ټکی) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    UVector32 boundary(دننه_String.شمېر_Char32() + 1, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    int32_t عدد_Breaks = 0;
    int32_t کوډ_ټکی_اوږدتیا = دننه_String.شمېر_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t شاخص_اندازه = کوډ_ټکی_اوږدتیا + 4;
    Local_حافظه<int32_t> شاخص_List(static_cast<int32_t*>(uprv_malloc(شاخص_اندازه * حجم(int32_t))));
    إذا (شاخص_List.isNull()) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع 0;
    }
    int32_t عدد_کوډ_Units = init_شاخص_List(دننه_String, شاخص_List.getAlias(), حالت);

    // Add a break for the start.
    boundary.جمع_کړه_Element(0, حالت);
    عدد_Breaks++;
    إذا (U_FAILURE(حالت)) أرجع 0;

    لكل (int32_t idx = 0; idx + 1 < کوډ_ټکی_اوږدتیا && U_SUCCESS(حالت); idx++) {
        عدد_Breaks =
            evaluateBreakpoint(دننه_String, شاخص_List.getAlias(), idx, عدد_کوډ_Units, عدد_Breaks, boundary, حالت);
        إذا (idx + 4 < کوډ_ټکی_اوږدتیا) {
            شاخص_List[idx + 6] = عدد_کوډ_Units;
            عدد_کوډ_Units += U16_اوږدتیا(دننه_String.char32At(شاخص_List[idx + 6]));
        }
    }

    إذا (U_FAILURE(حالت)) أرجع 0;

    // Add a break for the end if there is not one there already.
    إذا (boundary.وروستی_Elementi() != دننه_String.شمېر_Char32()) {
        boundary.جمع_کړه_Element(دننه_String.شمېر_Char32(), حالت);
        عدد_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_عدد_Breaks = 0;
    لكل (int32_t i = 0; i < عدد_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = ننوت_Map.isValid() ? ننوت_Map->elementAti(cpPos) : cpPos + range_پیل;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        إذا (utextPos > prevUTextPos) {
            إذا (utextPos != range_پیل ||
                (utextPos > 0 &&
                 f_Close_Punctuation_سېټ.لري(utext_char32At(دننه_متن, utextPos - 1)))) {
                وموندل_Breaks.push(utextPos, حالت);
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

    UChar32 بل_Char = utext_char32At(دننه_متن, range_پای_ټکی);
    إذا (!وموندل_Breaks.is_تشه() && وموندل_Breaks.peeki() == range_پای_ټکی) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        إذا (!f_Digit_Or_Open_Punctuation_Or_Alphabet_سېټ.لري(بل_Char)) {
            وموندل_Breaks.popi();
            corrected_عدد_Breaks--;
        }
    }

    أرجع corrected_عدد_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ثابت UnicodeString &دننه_String, int32_t *شاخص_List,
                                          int32_t پیل_Idx, int32_t عدد_کوډ_Units, int32_t عدد_Breaks,
                                          UVector32 &boundary, UError_کوډ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع عدد_Breaks;
    }
    int32_t پیل = 0, پای_ټکی = 0;
    int32_t score = f_Negative_مجموعه;

    لكل (صحيح i = 0; i < 6; i++) {
        // UW1 ~ UW6
        پیل = پیل_Idx + i;
        إذا (شاخص_List[پیل] != -1) {
            پای_ټکی = (شاخص_List[پیل + 1] != -1) ? شاخص_List[پیل + 1] : عدد_کوډ_Units;
            score += fModel[static_cast<int32_t>(Model_شاخص::kUWStart) + i].geti(
                دننه_String.لنډمهاله_Sub_String(شاخص_List[پیل], پای_ټکی - شاخص_List[پیل]));
        }
    }
    لكل (صحيح i = 0; i < 3; i++) {
        // BW1 ~ BW3
        پیل = پیل_Idx + i + 1;
        إذا (شاخص_List[پیل] != -1 && شاخص_List[پیل + 1] != -1) {
            پای_ټکی = (شاخص_List[پیل + 2] != -1) ? شاخص_List[پیل + 2] : عدد_کوډ_Units;
            score += fModel[static_cast<int32_t>(Model_شاخص::kBWStart) + i].geti(
                دننه_String.لنډمهاله_Sub_String(شاخص_List[پیل], پای_ټکی - شاخص_List[پیل]));
        }
    }
    لكل (صحيح i = 0; i < 4; i++) {
        // TW1 ~ TW4
        پیل = پیل_Idx + i;
        إذا (شاخص_List[پیل] != -1 && شاخص_List[پیل + 1] != -1 && شاخص_List[پیل + 2] != -1) {
            پای_ټکی = (شاخص_List[پیل + 3] != -1) ? شاخص_List[پیل + 3] : عدد_کوډ_Units;
            score += fModel[static_cast<int32_t>(Model_شاخص::kTWStart) + i].geti(
                دننه_String.لنډمهاله_Sub_String(شاخص_List[پیل], پای_ټکی - شاخص_List[پیل]));
        }
    }

    إذا (score > 0) {
        boundary.جمع_کړه_Element(پیل_Idx + 1, حالت);
        عدد_Breaks++;
    }
    أرجع عدد_Breaks;
}

int32_t MlBreakEngine::init_شاخص_List(ثابت UnicodeString &دننه_String, int32_t *شاخص_List,
                                     UError_کوډ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    int32_t شاخص = 0;
    int32_t length = دننه_String.شمېر_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(شاخص_List, 0xff, (length + 4) * حجم(int32_t));
    إذا (length > 0) {
        شاخص_List[2] = 0;
        شاخص = U16_اوږدتیا(دننه_String.char32At(0));
        إذا (length > 1) {
            شاخص_List[3] = شاخص;
            شاخص += U16_اوږدتیا(دننه_String.char32At(شاخص));
            إذا (length > 2) {
                شاخص_List[4] = شاخص;
                شاخص += U16_اوږدتیا(دننه_String.char32At(شاخص));
                إذا (length > 3) {
                    شاخص_List[5] = شاخص;
                    شاخص += U16_اوږدتیا(دننه_String.char32At(شاخص));
                }
            }
        }
    }
    أرجع شاخص;
}

فراغ MlBreakEngine::بارول_MLModel(UError_کوډ &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    إذا (U_FAILURE(error)) أرجع;

    UnicodeString کلی;
    ډېری_UResource_Bundle ډېری_لنډمهاله_Bundle;
    Resource_معلومات_ارزښت model_کلی;

    Local_UResource_Bundle_اشاره_ګر rbp(ures_open_مستقیم(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    إذا (U_FAILURE(error)) أرجع;

    int32_t شاخص = 0;
    init_کلی_ارزښت(rb, "UW1Keys", "UW1Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "UW2Keys", "UW2Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "UW3Keys", "UW3Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "UW4Keys", "UW4Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "UW5Keys", "UW5Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "UW6Keys", "UW6Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "BW1Keys", "BW1Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "BW2Keys", "BW2Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "BW3Keys", "BW3Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "TW1Keys", "TW1Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "TW2Keys", "TW2Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "TW3Keys", "TW3Values", fModel[شاخص++], error);
    init_کلی_ارزښت(rb, "TW4Keys", "TW4Values", fModel[شاخص++], error);
    f_Negative_مجموعه /= 2;
}

فراغ MlBreakEngine::init_کلی_ارزښت(UResourceBundle *rb, ثابت حرف *کلی_Name, ثابت حرف *ارزښت_Name,
                                 Hashtable &model, UError_کوډ &error) {
    int32_t کلی_اندازه = 0;
    int32_t ارزښت_اندازه = 0;
    int32_t string_اوږدتیا = 0;
    UnicodeString کلی;
    ډېری_UResource_Bundle ډېری_لنډمهاله_Bundle;
    Resource_معلومات_ارزښت model_کلی;

    // get modelValues
    Local_UResource_Bundle_اشاره_ګر model_ارزښت(ures_get_By_کلی(rb, ارزښت_Name, تش_اشاره, &error));
    ثابت int32_t *value = ures_get_Int_ویکټور(model_ارزښت.getAlias(), &ارزښت_اندازه, &error);
    إذا (U_FAILURE(error)) أرجع;

    // get modelKeys
    ures_get_ارزښت_With_Fallback(rb, کلی_Name, ډېری_لنډمهاله_Bundle.getAlias(), model_کلی, error);
    ResourceArray stringArray = model_کلی.getArray(error);
    کلی_اندازه = stringArray.get_اندازه();
    إذا (U_FAILURE(error)) أرجع;

    لكل (int32_t idx = 0; idx < کلی_اندازه; idx++) {
        stringArray.get_ارزښت(idx, model_کلی);
        کلی = UnicodeString(model_کلی.getString(string_اوږدتیا, error));
        إذا (U_SUCCESS(error)) {
            U_ASSERT(idx < ارزښت_اندازه);
            f_Negative_مجموعه -= value[idx];
            model.puti(کلی, value[idx], error);
        }
    }
}

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */
