// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "mlbe.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "ubrkimpl.h"
#অন্তর্ভাৱ "unicode/resbund.h"
#অন্তর্ভাৱ "unicode/udata.h"
#অন্তর্ভাৱ "unicode/utf16.h"
#অন্তর্ভাৱ "uresimp.h"
#অন্তর্ভাৱ "util.h"
#অন্তর্ভাৱ "uvectr32.h"

U_NAMESPACE_BEGIN

গণনা শ্ৰেণী ModelIndex { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ধ্রুৱ UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             ধ্রুৱ UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      fNegativeSum(0) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, ধ্রুৱ UnicodeString &inString,
                                     ধ্রুৱ LocalPointer<UVector32> &প্ৰবেশ_Map,
                                     UErrorCode &status) ধ্রুৱ {
    যদি (U_FAILURE(status)) {
        ঘূৰাই 0;
    }
    যদি (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    UVector32 boundary(inString.গুনতি_Char32() + 1, status);
    যদি (U_FAILURE(status)) {
        ঘূৰাই 0;
    }
    int32_t অঙ্ক_Breaks = 0;
    int32_t codePointLength = inString.গুনতি_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t indexSize = codePointLength + 4;
    LocalMemory<int32_t> indexList(static_cast<int32_t*>(uprv_malloc(indexSize * পরিমাণ(int32_t))));
    যদি (indexList.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই 0;
    }
    int32_t অঙ্ক_Code_Units = initIndexList(inString, indexList.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    অঙ্ক_Breaks++;
    যদি (U_FAILURE(status)) ঘূৰাই 0;

    বাবে (int32_t idx = 0; idx + 1 < codePointLength && U_SUCCESS(status); idx++) {
        অঙ্ক_Breaks =
            evaluateBreakpoint(inString, indexList.getAlias(), idx, অঙ্ক_Code_Units, অঙ্ক_Breaks, boundary, status);
        যদি (idx + 4 < codePointLength) {
            indexList[idx + 6] = অঙ্ক_Code_Units;
            অঙ্ক_Code_Units += U16_LENGTH(inString.char32At(indexList[idx + 6]));
        }
    }

    যদি (U_FAILURE(status)) ঘূৰাই 0;

    // Add a break for the end if there is not one there already.
    যদি (boundary.lastElementi() != inString.গুনতি_Char32()) {
        boundary.addElement(inString.গুনতি_Char32(), status);
        অঙ্ক_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_অঙ্ক_Breaks = 0;
    বাবে (int32_t i = 0; i < অঙ্ক_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = প্ৰবেশ_Map.isValid() ? প্ৰবেশ_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        যদি (utextPos > prevUTextPos) {
            যদি (utextPos != rangeStart ||
                (utextPos > 0 &&
                 fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_অঙ্ক_Breaks++;
            }
        } অন্যথা {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --অঙ্ক_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (শূন্য)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 nextChar = utext_char32At(inText, rangeEnd);
    যদি (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        যদি (!fDigitOrOpenPunctuationOrAlphabetSet.contains(nextChar)) {
            foundBreaks.popi();
            corrected_অঙ্ক_Breaks--;
        }
    }

    ঘূৰাই corrected_অঙ্ক_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ধ্রুৱ UnicodeString &inString, int32_t *indexList,
                                          int32_t startIdx, int32_t অঙ্ক_Code_Units, int32_t অঙ্ক_Breaks,
                                          UVector32 &boundary, UErrorCode &status) ধ্রুৱ {
    যদি (U_FAILURE(status)) {
        ঘূৰাই অঙ্ক_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = fNegativeSum;

    বাবে (পূর্ণাঙ্ক i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        যদি (indexList[start] != -1) {
            end = (indexList[start + 1] != -1) ? indexList[start + 1] : অঙ্ক_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kUWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    বাবে (পূর্ণাঙ্ক i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        যদি (indexList[start] != -1 && indexList[start + 1] != -1) {
            end = (indexList[start + 2] != -1) ? indexList[start + 2] : অঙ্ক_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kBWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    বাবে (পূর্ণাঙ্ক i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        যদি (indexList[start] != -1 && indexList[start + 1] != -1 && indexList[start + 2] != -1) {
            end = (indexList[start + 3] != -1) ? indexList[start + 3] : অঙ্ক_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kTWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }

    যদি (score > 0) {
        boundary.addElement(startIdx + 1, status);
        অঙ্ক_Breaks++;
    }
    ঘূৰাই অঙ্ক_Breaks;
}

int32_t MlBreakEngine::initIndexList(ধ্রুৱ UnicodeString &inString, int32_t *indexList,
                                     UErrorCode &status) ধ্রুৱ {
    যদি (U_FAILURE(status)) {
        ঘূৰাই 0;
    }
    int32_t index = 0;
    int32_t length = inString.গুনতি_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(indexList, 0xff, (length + 4) * পরিমাণ(int32_t));
    যদি (length > 0) {
        indexList[2] = 0;
        index = U16_LENGTH(inString.char32At(0));
        যদি (length > 1) {
            indexList[3] = index;
            index += U16_LENGTH(inString.char32At(index));
            যদি (length > 2) {
                indexList[4] = index;
                index += U16_LENGTH(inString.char32At(index));
                যদি (length > 3) {
                    indexList[5] = index;
                    index += U16_LENGTH(inString.char32At(index));
                }
            }
        }
    }
    ঘূৰাই index;
}

শূন্য MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    যদি (U_FAILURE(error)) ঘূৰাই;

    UnicodeString চাবি;
    স্তূপ_UResource_Bundle স্তূপ_Temp_Bundle;
    Resource_Data_মান model_চাবি;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    যদি (U_FAILURE(error)) ঘূৰাই;

    int32_t index = 0;
    init_চাবি_মান(rb, "UW1Keys", "UW1Values", fModel[index++], error);
    init_চাবি_মান(rb, "UW2Keys", "UW2Values", fModel[index++], error);
    init_চাবি_মান(rb, "UW3Keys", "UW3Values", fModel[index++], error);
    init_চাবি_মান(rb, "UW4Keys", "UW4Values", fModel[index++], error);
    init_চাবি_মান(rb, "UW5Keys", "UW5Values", fModel[index++], error);
    init_চাবি_মান(rb, "UW6Keys", "UW6Values", fModel[index++], error);
    init_চাবি_মান(rb, "BW1Keys", "BW1Values", fModel[index++], error);
    init_চাবি_মান(rb, "BW2Keys", "BW2Values", fModel[index++], error);
    init_চাবি_মান(rb, "BW3Keys", "BW3Values", fModel[index++], error);
    init_চাবি_মান(rb, "TW1Keys", "TW1Values", fModel[index++], error);
    init_চাবি_মান(rb, "TW2Keys", "TW2Values", fModel[index++], error);
    init_চাবি_মান(rb, "TW3Keys", "TW3Values", fModel[index++], error);
    init_চাবি_মান(rb, "TW4Keys", "TW4Values", fModel[index++], error);
    fNegativeSum /= 2;
}

শূন্য MlBreakEngine::init_চাবি_মান(UResourceBundle *rb, ধ্রুৱ ৱর্ণ *চাবি_Name, ধ্রুৱ ৱর্ণ *মান_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t চাবি_Size = 0;
    int32_t মান_Size = 0;
    int32_t stringLength = 0;
    UnicodeString চাবি;
    স্তূপ_UResource_Bundle স্তূপ_Temp_Bundle;
    Resource_Data_মান model_চাবি;

    // get modelValues
    LocalUResourceBundlePointer model_মান(ures_get_By_চাবি(rb, মান_Name, nullptr, &error));
    ধ্রুৱ int32_t *value = ures_getIntVector(model_মান.getAlias(), &মান_Size, &error);
    যদি (U_FAILURE(error)) ঘূৰাই;

    // get modelKeys
    ures_get_মান_With_Fallback(rb, চাবি_Name, স্তূপ_Temp_Bundle.getAlias(), model_চাবি, error);
    ResourceArray stringArray = model_চাবি.getArray(error);
    চাবি_Size = stringArray.getSize();
    যদি (U_FAILURE(error)) ঘূৰাই;

    বাবে (int32_t idx = 0; idx < চাবি_Size; idx++) {
        stringArray.get_মান(idx, model_চাবি);
        চাবি = UnicodeString(model_চাবি.getString(stringLength, error));
        যদি (U_SUCCESS(error)) {
            U_ASSERT(idx < মান_Size);
            fNegativeSum -= value[idx];
            model.puti(চাবি, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
