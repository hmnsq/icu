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

MlBreakEngine::MlBreakEngine(ثابت Unicode_مجموعه &digit_Or_Open_Punctuation_Or_Alphabet_مجموعه,
                             ثابت Unicode_مجموعه &close_Punctuation_مجموعه, UError_کد &وضعیت)
    : f_Digit_Or_Open_Punctuation_Or_Alphabet_مجموعه(digit_Or_Open_Punctuation_Or_Alphabet_مجموعه),
      f_Close_Punctuation_مجموعه(close_Punctuation_مجموعه),
      f_Negative_جمع(0) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    بارگیری_MLModel(وضعیت);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divide_به_بالا_Range(UText *درون_متن, int32_t range_شروع, int32_t range_انتها,
                                     UVector32 &یافت_Breaks, ثابت UnicodeString &درون_String,
                                     ثابت Local_اشاره_گر<UVector32> &درون_داد_Map,
                                     UError_کد &وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) {
        أرجع 0;
    }
    إذا (range_شروع >= range_انتها) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    UVector32 boundary(درون_String.شمار_Char32() + 1, وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع 0;
    }
    int32_t رقم_Breaks = 0;
    int32_t کد_نقطه_درازی = درون_String.شمار_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t شاخص_اندازه = کد_نقطه_درازی + 4;
    Local_حافظه<int32_t> شاخص_List(static_cast<int32_t*>(uprv_malloc(شاخص_اندازه * حجم(int32_t))));
    إذا (شاخص_List.isNull()) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع 0;
    }
    int32_t رقم_کد_Units = init_شاخص_List(درون_String, شاخص_List.getAlias(), وضعیت);

    // Add a break for the start.
    boundary.جمع_کن_Element(0, وضعیت);
    رقم_Breaks++;
    إذا (U_FAILURE(وضعیت)) أرجع 0;

    لكل (int32_t idx = 0; idx + 1 < کد_نقطه_درازی && U_SUCCESS(وضعیت); idx++) {
        رقم_Breaks =
            evaluateBreakpoint(درون_String, شاخص_List.getAlias(), idx, رقم_کد_Units, رقم_Breaks, boundary, وضعیت);
        إذا (idx + 4 < کد_نقطه_درازی) {
            شاخص_List[idx + 6] = رقم_کد_Units;
            رقم_کد_Units += U16_درازی(درون_String.char32At(شاخص_List[idx + 6]));
        }
    }

    إذا (U_FAILURE(وضعیت)) أرجع 0;

    // Add a break for the end if there is not one there already.
    إذا (boundary.آخرین_Elementi() != درون_String.شمار_Char32()) {
        boundary.جمع_کن_Element(درون_String.شمار_Char32(), وضعیت);
        رقم_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_رقم_Breaks = 0;
    لكل (int32_t i = 0; i < رقم_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = درون_داد_Map.isValid() ? درون_داد_Map->elementAti(cpPos) : cpPos + range_شروع;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        إذا (utextPos > prevUTextPos) {
            إذا (utextPos != range_شروع ||
                (utextPos > 0 &&
                 f_Close_Punctuation_مجموعه.شامل_است(utext_char32At(درون_متن, utextPos - 1)))) {
                یافت_Breaks.push(utextPos, وضعیت);
                corrected_رقم_Breaks++;
            }
        } وإلا {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --رقم_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (فراغ)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 بعدی_Char = utext_char32At(درون_متن, range_انتها);
    إذا (!یافت_Breaks.is_خالی() && یافت_Breaks.peeki() == range_انتها) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        إذا (!f_Digit_Or_Open_Punctuation_Or_Alphabet_مجموعه.شامل_است(بعدی_Char)) {
            یافت_Breaks.popi();
            corrected_رقم_Breaks--;
        }
    }

    أرجع corrected_رقم_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ثابت UnicodeString &درون_String, int32_t *شاخص_List,
                                          int32_t شروع_Idx, int32_t رقم_کد_Units, int32_t رقم_Breaks,
                                          UVector32 &boundary, UError_کد &وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) {
        أرجع رقم_Breaks;
    }
    int32_t شروع = 0, انتها = 0;
    int32_t score = f_Negative_جمع;

    لكل (صحيح i = 0; i < 6; i++) {
        // UW1 ~ UW6
        شروع = شروع_Idx + i;
        إذا (شاخص_List[شروع] != -1) {
            انتها = (شاخص_List[شروع + 1] != -1) ? شاخص_List[شروع + 1] : رقم_کد_Units;
            score += fModel[static_cast<int32_t>(Model_شاخص::kUWStart) + i].geti(
                درون_String.موقت_Sub_String(شاخص_List[شروع], انتها - شاخص_List[شروع]));
        }
    }
    لكل (صحيح i = 0; i < 3; i++) {
        // BW1 ~ BW3
        شروع = شروع_Idx + i + 1;
        إذا (شاخص_List[شروع] != -1 && شاخص_List[شروع + 1] != -1) {
            انتها = (شاخص_List[شروع + 2] != -1) ? شاخص_List[شروع + 2] : رقم_کد_Units;
            score += fModel[static_cast<int32_t>(Model_شاخص::kBWStart) + i].geti(
                درون_String.موقت_Sub_String(شاخص_List[شروع], انتها - شاخص_List[شروع]));
        }
    }
    لكل (صحيح i = 0; i < 4; i++) {
        // TW1 ~ TW4
        شروع = شروع_Idx + i;
        إذا (شاخص_List[شروع] != -1 && شاخص_List[شروع + 1] != -1 && شاخص_List[شروع + 2] != -1) {
            انتها = (شاخص_List[شروع + 3] != -1) ? شاخص_List[شروع + 3] : رقم_کد_Units;
            score += fModel[static_cast<int32_t>(Model_شاخص::kTWStart) + i].geti(
                درون_String.موقت_Sub_String(شاخص_List[شروع], انتها - شاخص_List[شروع]));
        }
    }

    إذا (score > 0) {
        boundary.جمع_کن_Element(شروع_Idx + 1, وضعیت);
        رقم_Breaks++;
    }
    أرجع رقم_Breaks;
}

int32_t MlBreakEngine::init_شاخص_List(ثابت UnicodeString &درون_String, int32_t *شاخص_List,
                                     UError_کد &وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) {
        أرجع 0;
    }
    int32_t شاخص = 0;
    int32_t length = درون_String.شمار_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(شاخص_List, 0xff, (length + 4) * حجم(int32_t));
    إذا (length > 0) {
        شاخص_List[2] = 0;
        شاخص = U16_درازی(درون_String.char32At(0));
        إذا (length > 1) {
            شاخص_List[3] = شاخص;
            شاخص += U16_درازی(درون_String.char32At(شاخص));
            إذا (length > 2) {
                شاخص_List[4] = شاخص;
                شاخص += U16_درازی(درون_String.char32At(شاخص));
                إذا (length > 3) {
                    شاخص_List[5] = شاخص;
                    شاخص += U16_درازی(درون_String.char32At(شاخص));
                }
            }
        }
    }
    أرجع شاخص;
}

فراغ MlBreakEngine::بارگیری_MLModel(UError_کد &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    إذا (U_FAILURE(error)) أرجع;

    UnicodeString کلید;
    پشته_UResource_Bundle پشته_موقت_Bundle;
    Resource_داده_مقدار model_کلید;

    Local_UResource_Bundle_اشاره_گر rbp(ures_open_مستقیم(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    إذا (U_FAILURE(error)) أرجع;

    int32_t شاخص = 0;
    init_کلید_مقدار(rb, "UW1Keys", "UW1Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "UW2Keys", "UW2Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "UW3Keys", "UW3Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "UW4Keys", "UW4Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "UW5Keys", "UW5Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "UW6Keys", "UW6Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "BW1Keys", "BW1Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "BW2Keys", "BW2Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "BW3Keys", "BW3Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "TW1Keys", "TW1Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "TW2Keys", "TW2Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "TW3Keys", "TW3Values", fModel[شاخص++], error);
    init_کلید_مقدار(rb, "TW4Keys", "TW4Values", fModel[شاخص++], error);
    f_Negative_جمع /= 2;
}

فراغ MlBreakEngine::init_کلید_مقدار(UResourceBundle *rb, ثابت حرف *کلید_Name, ثابت حرف *مقدار_Name,
                                 Hashtable &model, UError_کد &error) {
    int32_t کلید_اندازه = 0;
    int32_t مقدار_اندازه = 0;
    int32_t string_درازی = 0;
    UnicodeString کلید;
    پشته_UResource_Bundle پشته_موقت_Bundle;
    Resource_داده_مقدار model_کلید;

    // get modelValues
    Local_UResource_Bundle_اشاره_گر model_مقدار(ures_get_By_کلید(rb, مقدار_Name, اشاره_تهی, &error));
    ثابت int32_t *value = ures_get_Int_وکتور(model_مقدار.getAlias(), &مقدار_اندازه, &error);
    إذا (U_FAILURE(error)) أرجع;

    // get modelKeys
    ures_get_مقدار_With_Fallback(rb, کلید_Name, پشته_موقت_Bundle.getAlias(), model_کلید, error);
    ResourceArray stringArray = model_کلید.getArray(error);
    کلید_اندازه = stringArray.get_اندازه();
    إذا (U_FAILURE(error)) أرجع;

    لكل (int32_t idx = 0; idx < کلید_اندازه; idx++) {
        stringArray.get_مقدار(idx, model_کلید);
        کلید = UnicodeString(model_کلید.getString(string_درازی, error));
        إذا (U_SUCCESS(error)) {
            U_ASSERT(idx < مقدار_اندازه);
            f_Negative_جمع -= value[idx];
            model.puti(کلید, value[idx], error);
        }
    }
}

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */
