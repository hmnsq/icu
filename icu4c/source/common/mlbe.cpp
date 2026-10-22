// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

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

ಗಣನಾ ವರ್ಗ ModelIndex { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ಧ್ರುವ UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             ಧ್ರುವ UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      fNegativeSum(0) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, ಧ್ರುವ UnicodeString &inString,
                                     ಧ್ರುವ LocalPointer<UVector32> &ಒಳಹರಿವು_Map,
                                     UErrorCode &status) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    UVector32 boundary(inString.ಎಣಿಕೆ_Char32() + 1, status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t ಅಂಕೆ_Breaks = 0;
    int32_t codePointLength = inString.ಎಣಿಕೆ_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t indexSize = codePointLength + 4;
    LocalMemory<int32_t> indexList(static_cast<int32_t*>(uprv_malloc(indexSize * ಪರಿಮಾಣ(int32_t))));
    ಯದಿ (indexList.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t ಅಂಕೆ_Code_Units = initIndexList(inString, indexList.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    ಅಂಕೆ_Breaks++;
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತಿ (int32_t idx = 0; idx + 1 < codePointLength && U_SUCCESS(status); idx++) {
        ಅಂಕೆ_Breaks =
            evaluateBreakpoint(inString, indexList.getAlias(), idx, ಅಂಕೆ_Code_Units, ಅಂಕೆ_Breaks, boundary, status);
        ಯದಿ (idx + 4 < codePointLength) {
            indexList[idx + 6] = ಅಂಕೆ_Code_Units;
            ಅಂಕೆ_Code_Units += U16_LENGTH(inString.char32At(indexList[idx + 6]));
        }
    }

    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ 0;

    // Add a break for the end if there is not one there already.
    ಯದಿ (boundary.lastElementi() != inString.ಎಣಿಕೆ_Char32()) {
        boundary.addElement(inString.ಎಣಿಕೆ_Char32(), status);
        ಅಂಕೆ_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_ಅಂಕೆ_Breaks = 0;
    ಪ್ರತಿ (int32_t i = 0; i < ಅಂಕೆ_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = ಒಳಹರಿವು_Map.isValid() ? ಒಳಹರಿವು_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        ಯದಿ (utextPos > prevUTextPos) {
            ಯದಿ (utextPos != rangeStart ||
                (utextPos > 0 &&
                 fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_ಅಂಕೆ_Breaks++;
            }
        } ಅನ್ಯಥಾ {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --ಅಂಕೆ_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (ಶೂನ್ಯ)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 nextChar = utext_char32At(inText, rangeEnd);
    ಯದಿ (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        ಯದಿ (!fDigitOrOpenPunctuationOrAlphabetSet.contains(nextChar)) {
            foundBreaks.popi();
            corrected_ಅಂಕೆ_Breaks--;
        }
    }

    ಪ್ರತ್ಯಾಗಮ corrected_ಅಂಕೆ_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ಧ್ರುವ UnicodeString &inString, int32_t *indexList,
                                          int32_t startIdx, int32_t ಅಂಕೆ_Code_Units, int32_t ಅಂಕೆ_Breaks,
                                          UVector32 &boundary, UErrorCode &status) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = fNegativeSum;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        ಯದಿ (indexList[start] != -1) {
            end = (indexList[start + 1] != -1) ? indexList[start + 1] : ಅಂಕೆ_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kUWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        ಯದಿ (indexList[start] != -1 && indexList[start + 1] != -1) {
            end = (indexList[start + 2] != -1) ? indexList[start + 2] : ಅಂಕೆ_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kBWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        ಯದಿ (indexList[start] != -1 && indexList[start + 1] != -1 && indexList[start + 2] != -1) {
            end = (indexList[start + 3] != -1) ? indexList[start + 3] : ಅಂಕೆ_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kTWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }

    ಯದಿ (score > 0) {
        boundary.addElement(startIdx + 1, status);
        ಅಂಕೆ_Breaks++;
    }
    ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_Breaks;
}

int32_t MlBreakEngine::initIndexList(ಧ್ರುವ UnicodeString &inString, int32_t *indexList,
                                     UErrorCode &status) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t index = 0;
    int32_t length = inString.ಎಣಿಕೆ_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(indexList, 0xff, (length + 4) * ಪರಿಮಾಣ(int32_t));
    ಯದಿ (length > 0) {
        indexList[2] = 0;
        index = U16_LENGTH(inString.char32At(0));
        ಯದಿ (length > 1) {
            indexList[3] = index;
            index += U16_LENGTH(inString.char32At(index));
            ಯದಿ (length > 2) {
                indexList[4] = index;
                index += U16_LENGTH(inString.char32At(index));
                ಯದಿ (length > 3) {
                    indexList[5] = index;
                    index += U16_LENGTH(inString.char32At(index));
                }
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ index;
}

ಶೂನ್ಯ MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    ಯದಿ (U_FAILURE(error)) ಪ್ರತ್ಯಾಗಮ;

    UnicodeString ಕೀಲಿ;
    ರಾಶಿ_UResource_Bundle ರಾಶಿ_Temp_Bundle;
    Resource_Data_ಮೌಲ್ಯ model_ಕೀಲಿ;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    ಯದಿ (U_FAILURE(error)) ಪ್ರತ್ಯಾಗಮ;

    int32_t index = 0;
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW1Keys", "UW1Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW2Keys", "UW2Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW3Keys", "UW3Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW4Keys", "UW4Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW5Keys", "UW5Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "UW6Keys", "UW6Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "BW1Keys", "BW1Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "BW2Keys", "BW2Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "BW3Keys", "BW3Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "TW1Keys", "TW1Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "TW2Keys", "TW2Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "TW3Keys", "TW3Values", fModel[index++], error);
    init_ಕೀಲಿ_ಮೌಲ್ಯ(rb, "TW4Keys", "TW4Values", fModel[index++], error);
    fNegativeSum /= 2;
}

ಶೂನ್ಯ MlBreakEngine::init_ಕೀಲಿ_ಮೌಲ್ಯ(UResourceBundle *rb, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_Name, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t ಕೀಲಿ_Size = 0;
    int32_t ಮೌಲ್ಯ_Size = 0;
    int32_t stringLength = 0;
    UnicodeString ಕೀಲಿ;
    ರಾಶಿ_UResource_Bundle ರಾಶಿ_Temp_Bundle;
    Resource_Data_ಮೌಲ್ಯ model_ಕೀಲಿ;

    // get modelValues
    LocalUResourceBundlePointer model_ಮೌಲ್ಯ(ures_get_By_ಕೀಲಿ(rb, ಮೌಲ್ಯ_Name, nullptr, &error));
    ಧ್ರುವ int32_t *value = ures_getIntVector(model_ಮೌಲ್ಯ.getAlias(), &ಮೌಲ್ಯ_Size, &error);
    ಯದಿ (U_FAILURE(error)) ಪ್ರತ್ಯಾಗಮ;

    // get modelKeys
    ures_get_ಮೌಲ್ಯ_With_Fallback(rb, ಕೀಲಿ_Name, ರಾಶಿ_Temp_Bundle.getAlias(), model_ಕೀಲಿ, error);
    ResourceArray stringArray = model_ಕೀಲಿ.getArray(error);
    ಕೀಲಿ_Size = stringArray.getSize();
    ಯದಿ (U_FAILURE(error)) ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (int32_t idx = 0; idx < ಕೀಲಿ_Size; idx++) {
        stringArray.get_ಮೌಲ್ಯ(idx, model_ಕೀಲಿ);
        ಕೀಲಿ = UnicodeString(model_ಕೀಲಿ.getString(stringLength, error));
        ಯದಿ (U_SUCCESS(error)) {
            U_ASSERT(idx < ಮೌಲ್ಯ_Size);
            fNegativeSum -= value[idx];
            model.puti(ಕೀಲಿ, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
