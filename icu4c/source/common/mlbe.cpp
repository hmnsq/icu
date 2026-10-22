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

تعداد صنف Model_اشاريو { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ثابت Unicode_سيٽ &digit_Or_Open_Punctuation_Or_Alphabet_سيٽ,
                             ثابت Unicode_سيٽ &close_Punctuation_سيٽ, UError_ڪوڊ &حالت)
    : f_Digit_Or_Open_Punctuation_Or_Alphabet_سيٽ(digit_Or_Open_Punctuation_Or_Alphabet_سيٽ),
      f_Close_Punctuation_سيٽ(close_Punctuation_سيٽ),
      f_Negative_جوڙ(0) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    ڀريو_MLModel(حالت);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divide_مٿي_Range(UText *اندريون_متن, int32_t range_شروعات, int32_t range_پڇاڙي,
                                     UVector32 &مليو_Breaks, ثابت UnicodeString &اندريون_String,
                                     ثابت Local_اشارو<UVector32> &داخلا_Map,
                                     UError_ڪوڊ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    إذا (range_شروعات >= range_پڇاڙي) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    UVector32 boundary(اندريون_String.ڳڻپ_Char32() + 1, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    int32_t انگ_Breaks = 0;
    int32_t ڪوڊ_نقطو_ڊيگهائي = اندريون_String.ڳڻپ_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t اشاريو_سائيز = ڪوڊ_نقطو_ڊيگهائي + 4;
    Local_يادگيري<int32_t> اشاريو_List(static_cast<int32_t*>(uprv_malloc(اشاريو_سائيز * حجم(int32_t))));
    إذا (اشاريو_List.isNull()) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع 0;
    }
    int32_t انگ_ڪوڊ_Units = init_اشاريو_List(اندريون_String, اشاريو_List.getAlias(), حالت);

    // Add a break for the start.
    boundary.جوڙيو_Element(0, حالت);
    انگ_Breaks++;
    إذا (U_FAILURE(حالت)) أرجع 0;

    لكل (int32_t idx = 0; idx + 1 < ڪوڊ_نقطو_ڊيگهائي && U_SUCCESS(حالت); idx++) {
        انگ_Breaks =
            evaluateBreakpoint(اندريون_String, اشاريو_List.getAlias(), idx, انگ_ڪوڊ_Units, انگ_Breaks, boundary, حالت);
        إذا (idx + 4 < ڪوڊ_نقطو_ڊيگهائي) {
            اشاريو_List[idx + 6] = انگ_ڪوڊ_Units;
            انگ_ڪوڊ_Units += U16_ڊيگهائي(اندريون_String.char32At(اشاريو_List[idx + 6]));
        }
    }

    إذا (U_FAILURE(حالت)) أرجع 0;

    // Add a break for the end if there is not one there already.
    إذا (boundary.آخري_Elementi() != اندريون_String.ڳڻپ_Char32()) {
        boundary.جوڙيو_Element(اندريون_String.ڳڻپ_Char32(), حالت);
        انگ_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_انگ_Breaks = 0;
    لكل (int32_t i = 0; i < انگ_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = داخلا_Map.isValid() ? داخلا_Map->elementAti(cpPos) : cpPos + range_شروعات;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        إذا (utextPos > prevUTextPos) {
            إذا (utextPos != range_شروعات ||
                (utextPos > 0 &&
                 f_Close_Punctuation_سيٽ.شامل_آهي(utext_char32At(اندريون_متن, utextPos - 1)))) {
                مليو_Breaks.push(utextPos, حالت);
                corrected_انگ_Breaks++;
            }
        } وإلا {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --انگ_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (فراغ)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 ايندڙ_Char = utext_char32At(اندريون_متن, range_پڇاڙي);
    إذا (!مليو_Breaks.is_خالي_جايون() && مليو_Breaks.peeki() == range_پڇاڙي) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        إذا (!f_Digit_Or_Open_Punctuation_Or_Alphabet_سيٽ.شامل_آهي(ايندڙ_Char)) {
            مليو_Breaks.popi();
            corrected_انگ_Breaks--;
        }
    }

    أرجع corrected_انگ_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ثابت UnicodeString &اندريون_String, int32_t *اشاريو_List,
                                          int32_t شروعات_Idx, int32_t انگ_ڪوڊ_Units, int32_t انگ_Breaks,
                                          UVector32 &boundary, UError_ڪوڊ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع انگ_Breaks;
    }
    int32_t شروعات = 0, پڇاڙي = 0;
    int32_t score = f_Negative_جوڙ;

    لكل (صحيح i = 0; i < 6; i++) {
        // UW1 ~ UW6
        شروعات = شروعات_Idx + i;
        إذا (اشاريو_List[شروعات] != -1) {
            پڇاڙي = (اشاريو_List[شروعات + 1] != -1) ? اشاريو_List[شروعات + 1] : انگ_ڪوڊ_Units;
            score += fModel[static_cast<int32_t>(Model_اشاريو::kUWStart) + i].geti(
                اندريون_String.عارضي_Sub_String(اشاريو_List[شروعات], پڇاڙي - اشاريو_List[شروعات]));
        }
    }
    لكل (صحيح i = 0; i < 3; i++) {
        // BW1 ~ BW3
        شروعات = شروعات_Idx + i + 1;
        إذا (اشاريو_List[شروعات] != -1 && اشاريو_List[شروعات + 1] != -1) {
            پڇاڙي = (اشاريو_List[شروعات + 2] != -1) ? اشاريو_List[شروعات + 2] : انگ_ڪوڊ_Units;
            score += fModel[static_cast<int32_t>(Model_اشاريو::kBWStart) + i].geti(
                اندريون_String.عارضي_Sub_String(اشاريو_List[شروعات], پڇاڙي - اشاريو_List[شروعات]));
        }
    }
    لكل (صحيح i = 0; i < 4; i++) {
        // TW1 ~ TW4
        شروعات = شروعات_Idx + i;
        إذا (اشاريو_List[شروعات] != -1 && اشاريو_List[شروعات + 1] != -1 && اشاريو_List[شروعات + 2] != -1) {
            پڇاڙي = (اشاريو_List[شروعات + 3] != -1) ? اشاريو_List[شروعات + 3] : انگ_ڪوڊ_Units;
            score += fModel[static_cast<int32_t>(Model_اشاريو::kTWStart) + i].geti(
                اندريون_String.عارضي_Sub_String(اشاريو_List[شروعات], پڇاڙي - اشاريو_List[شروعات]));
        }
    }

    إذا (score > 0) {
        boundary.جوڙيو_Element(شروعات_Idx + 1, حالت);
        انگ_Breaks++;
    }
    أرجع انگ_Breaks;
}

int32_t MlBreakEngine::init_اشاريو_List(ثابت UnicodeString &اندريون_String, int32_t *اشاريو_List,
                                     UError_ڪوڊ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع 0;
    }
    int32_t اشاريو = 0;
    int32_t length = اندريون_String.ڳڻپ_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(اشاريو_List, 0xff, (length + 4) * حجم(int32_t));
    إذا (length > 0) {
        اشاريو_List[2] = 0;
        اشاريو = U16_ڊيگهائي(اندريون_String.char32At(0));
        إذا (length > 1) {
            اشاريو_List[3] = اشاريو;
            اشاريو += U16_ڊيگهائي(اندريون_String.char32At(اشاريو));
            إذا (length > 2) {
                اشاريو_List[4] = اشاريو;
                اشاريو += U16_ڊيگهائي(اندريون_String.char32At(اشاريو));
                إذا (length > 3) {
                    اشاريو_List[5] = اشاريو;
                    اشاريو += U16_ڊيگهائي(اندريون_String.char32At(اشاريو));
                }
            }
        }
    }
    أرجع اشاريو;
}

فراغ MlBreakEngine::ڀريو_MLModel(UError_ڪوڊ &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    إذا (U_FAILURE(error)) أرجع;

    UnicodeString ڪنجي;
    ڍير_UResource_Bundle ڍير_عارضي_Bundle;
    Resource_معلومات_قدر model_ڪنجي;

    Local_UResource_Bundle_اشارو rbp(ures_open_سڌو(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    إذا (U_FAILURE(error)) أرجع;

    int32_t اشاريو = 0;
    init_ڪنجي_قدر(rb, "UW1Keys", "UW1Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "UW2Keys", "UW2Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "UW3Keys", "UW3Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "UW4Keys", "UW4Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "UW5Keys", "UW5Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "UW6Keys", "UW6Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "BW1Keys", "BW1Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "BW2Keys", "BW2Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "BW3Keys", "BW3Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "TW1Keys", "TW1Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "TW2Keys", "TW2Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "TW3Keys", "TW3Values", fModel[اشاريو++], error);
    init_ڪنجي_قدر(rb, "TW4Keys", "TW4Values", fModel[اشاريو++], error);
    f_Negative_جوڙ /= 2;
}

فراغ MlBreakEngine::init_ڪنجي_قدر(UResourceBundle *rb, ثابت حرف *ڪنجي_Name, ثابت حرف *قدر_Name,
                                 Hashtable &model, UError_ڪوڊ &error) {
    int32_t ڪنجي_سائيز = 0;
    int32_t قدر_سائيز = 0;
    int32_t string_ڊيگهائي = 0;
    UnicodeString ڪنجي;
    ڍير_UResource_Bundle ڍير_عارضي_Bundle;
    Resource_معلومات_قدر model_ڪنجي;

    // get modelValues
    Local_UResource_Bundle_اشارو model_قدر(ures_get_By_ڪنجي(rb, قدر_Name, خالي_اشارو, &error));
    ثابت int32_t *value = ures_get_Int_ويڪٽر(model_قدر.getAlias(), &قدر_سائيز, &error);
    إذا (U_FAILURE(error)) أرجع;

    // get modelKeys
    ures_get_قدر_With_Fallback(rb, ڪنجي_Name, ڍير_عارضي_Bundle.getAlias(), model_ڪنجي, error);
    ResourceArray stringArray = model_ڪنجي.getArray(error);
    ڪنجي_سائيز = stringArray.get_سائيز();
    إذا (U_FAILURE(error)) أرجع;

    لكل (int32_t idx = 0; idx < ڪنجي_سائيز; idx++) {
        stringArray.get_قدر(idx, model_ڪنجي);
        ڪنجي = UnicodeString(model_ڪنجي.getString(string_ڊيگهائي, error));
        إذا (U_SUCCESS(error)) {
            U_ASSERT(idx < قدر_سائيز);
            f_Negative_جوڙ -= value[idx];
            model.puti(ڪنجي, value[idx], error);
        }
    }
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */
