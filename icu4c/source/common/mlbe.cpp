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

تعداد صنف ModelIndex { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ثابت UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             ثابت UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      fNegativeSum(0) {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, ثابت UnicodeString &inString,
                                     ثابت LocalPointer<UVector32> &ماسوقن_Map,
                                     UErrorCode &status) ثابت {
    إذا (U_FAILURE(status)) {
        أرجع 0;
    }
    إذا (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    UVector32 boundary(inString.کيراءن_Char32() + 1, status);
    إذا (U_FAILURE(status)) {
        أرجع 0;
    }
    int32_t اڠک_Breaks = 0;
    int32_t codePointLength = inString.کيراءن_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t indexSize = codePointLength + 4;
    LocalMemory<int32_t> indexList(static_cast<int32_t*>(uprv_malloc(indexSize * حجم(int32_t))));
    إذا (indexList.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع 0;
    }
    int32_t اڠک_Code_Units = initIndexList(inString, indexList.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    اڠک_Breaks++;
    إذا (U_FAILURE(status)) أرجع 0;

    لكل (int32_t idx = 0; idx + 1 < codePointLength && U_SUCCESS(status); idx++) {
        اڠک_Breaks =
            evaluateBreakpoint(inString, indexList.getAlias(), idx, اڠک_Code_Units, اڠک_Breaks, boundary, status);
        إذا (idx + 4 < codePointLength) {
            indexList[idx + 6] = اڠک_Code_Units;
            اڠک_Code_Units += U16_LENGTH(inString.char32At(indexList[idx + 6]));
        }
    }

    إذا (U_FAILURE(status)) أرجع 0;

    // Add a break for the end if there is not one there already.
    إذا (boundary.lastElementi() != inString.کيراءن_Char32()) {
        boundary.addElement(inString.کيراءن_Char32(), status);
        اڠک_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_اڠک_Breaks = 0;
    لكل (int32_t i = 0; i < اڠک_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = ماسوقن_Map.isValid() ? ماسوقن_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        إذا (utextPos > prevUTextPos) {
            إذا (utextPos != rangeStart ||
                (utextPos > 0 &&
                 fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_اڠک_Breaks++;
            }
        } وإلا {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --اڠک_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (فراغ)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 nextChar = utext_char32At(inText, rangeEnd);
    إذا (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        إذا (!fDigitOrOpenPunctuationOrAlphabetSet.contains(nextChar)) {
            foundBreaks.popi();
            corrected_اڠک_Breaks--;
        }
    }

    أرجع corrected_اڠک_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ثابت UnicodeString &inString, int32_t *indexList,
                                          int32_t startIdx, int32_t اڠک_Code_Units, int32_t اڠک_Breaks,
                                          UVector32 &boundary, UErrorCode &status) ثابت {
    إذا (U_FAILURE(status)) {
        أرجع اڠک_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = fNegativeSum;

    لكل (صحيح i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        إذا (indexList[start] != -1) {
            end = (indexList[start + 1] != -1) ? indexList[start + 1] : اڠک_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kUWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    لكل (صحيح i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        إذا (indexList[start] != -1 && indexList[start + 1] != -1) {
            end = (indexList[start + 2] != -1) ? indexList[start + 2] : اڠک_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kBWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    لكل (صحيح i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        إذا (indexList[start] != -1 && indexList[start + 1] != -1 && indexList[start + 2] != -1) {
            end = (indexList[start + 3] != -1) ? indexList[start + 3] : اڠک_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kTWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }

    إذا (score > 0) {
        boundary.addElement(startIdx + 1, status);
        اڠک_Breaks++;
    }
    أرجع اڠک_Breaks;
}

int32_t MlBreakEngine::initIndexList(ثابت UnicodeString &inString, int32_t *indexList,
                                     UErrorCode &status) ثابت {
    إذا (U_FAILURE(status)) {
        أرجع 0;
    }
    int32_t index = 0;
    int32_t length = inString.کيراءن_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(indexList, 0xff, (length + 4) * حجم(int32_t));
    إذا (length > 0) {
        indexList[2] = 0;
        index = U16_LENGTH(inString.char32At(0));
        إذا (length > 1) {
            indexList[3] = index;
            index += U16_LENGTH(inString.char32At(index));
            إذا (length > 2) {
                indexList[4] = index;
                index += U16_LENGTH(inString.char32At(index));
                إذا (length > 3) {
                    indexList[5] = index;
                    index += U16_LENGTH(inString.char32At(index));
                }
            }
        }
    }
    أرجع index;
}

فراغ MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    إذا (U_FAILURE(error)) أرجع;

    UnicodeString کونچي;
    تيندنن_UResource_Bundle تيندنن_Temp_Bundle;
    Resource_Data_نيلاي model_کونچي;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    إذا (U_FAILURE(error)) أرجع;

    int32_t index = 0;
    init_کونچي_نيلاي(rb, "UW1Keys", "UW1Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "UW2Keys", "UW2Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "UW3Keys", "UW3Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "UW4Keys", "UW4Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "UW5Keys", "UW5Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "UW6Keys", "UW6Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "BW1Keys", "BW1Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "BW2Keys", "BW2Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "BW3Keys", "BW3Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "TW1Keys", "TW1Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "TW2Keys", "TW2Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "TW3Keys", "TW3Values", fModel[index++], error);
    init_کونچي_نيلاي(rb, "TW4Keys", "TW4Values", fModel[index++], error);
    fNegativeSum /= 2;
}

فراغ MlBreakEngine::init_کونچي_نيلاي(UResourceBundle *rb, ثابت حرف *کونچي_Name, ثابت حرف *نيلاي_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t کونچي_Size = 0;
    int32_t نيلاي_Size = 0;
    int32_t stringLength = 0;
    UnicodeString کونچي;
    تيندنن_UResource_Bundle تيندنن_Temp_Bundle;
    Resource_Data_نيلاي model_کونچي;

    // get modelValues
    LocalUResourceBundlePointer model_نيلاي(ures_get_By_کونچي(rb, نيلاي_Name, nullptr, &error));
    ثابت int32_t *value = ures_getIntVector(model_نيلاي.getAlias(), &نيلاي_Size, &error);
    إذا (U_FAILURE(error)) أرجع;

    // get modelKeys
    ures_get_نيلاي_With_Fallback(rb, کونچي_Name, تيندنن_Temp_Bundle.getAlias(), model_کونچي, error);
    ResourceArray stringArray = model_کونچي.getArray(error);
    کونچي_Size = stringArray.getSize();
    إذا (U_FAILURE(error)) أرجع;

    لكل (int32_t idx = 0; idx < کونچي_Size; idx++) {
        stringArray.get_نيلاي(idx, model_کونچي);
        کونچي = UnicodeString(model_کونچي.getString(stringLength, error));
        إذا (U_SUCCESS(error)) {
            U_ASSERT(idx < نيلاي_Size);
            fNegativeSum -= value[idx];
            model.puti(کونچي, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
