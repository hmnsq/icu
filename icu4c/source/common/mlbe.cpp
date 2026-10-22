// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#include "unicode/utypes.h"

#if !UCONFIG_NO_BREAK_ITERATION

#include "cmemory.h"
#include "mlbe.h"
#include "uassert.h"
#include "ubrkimpl.h"
#include "unicode/resbund.h"
#include "unicode/udata.h"
#include "unicode/utf16.h"
#include "uresimp.h"
#include "util.h"
#include "uvectr32.h"

U_NAMESPACE_BEGIN

enum class ModelIndex { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(const UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             const UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      fNegativeSum(0) {
    if (U_FAILURE(status)) {
        return;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, const UnicodeString &inString,
                                     const LocalPointer<UVector32> &पासोक_Map,
                                     UErrorCode &status) const {
    if (U_FAILURE(status)) {
        return 0;
    }
    if (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }

    UVector32 boundary(inString.बिलांग_Char32() + 1, status);
    if (U_FAILURE(status)) {
        return 0;
    }
    int32_t नुमेरो_Breaks = 0;
    int32_t codePointLength = inString.बिलांग_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t indexSize = codePointLength + 4;
    LocalMemory<int32_t> indexList(static_cast<int32_t*>(uprv_malloc(indexSize * sizeof(int32_t))));
    if (indexList.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return 0;
    }
    int32_t नुमेरो_Code_Units = initIndexList(inString, indexList.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    नुमेरो_Breaks++;
    if (U_FAILURE(status)) return 0;

    for (int32_t idx = 0; idx + 1 < codePointLength && U_SUCCESS(status); idx++) {
        नुमेरो_Breaks =
            evaluateBreakpoint(inString, indexList.getAlias(), idx, नुमेरो_Code_Units, नुमेरो_Breaks, boundary, status);
        if (idx + 4 < codePointLength) {
            indexList[idx + 6] = नुमेरो_Code_Units;
            नुमेरो_Code_Units += U16_LENGTH(inString.char32At(indexList[idx + 6]));
        }
    }

    if (U_FAILURE(status)) return 0;

    // Add a break for the end if there is not one there already.
    if (boundary.lastElementi() != inString.बिलांग_Char32()) {
        boundary.addElement(inString.बिलांग_Char32(), status);
        नुमेरो_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_नुमेरो_Breaks = 0;
    for (int32_t i = 0; i < नुमेरो_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = पासोक_Map.isValid() ? पासोक_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        if (utextPos > prevUTextPos) {
            if (utextPos != rangeStart ||
                (utextPos > 0 &&
                 fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_नुमेरो_Breaks++;
            }
        } else {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --नुमेरो_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (void)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 nextChar = utext_char32At(inText, rangeEnd);
    if (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        if (!fDigitOrOpenPunctuationOrAlphabetSet.contains(nextChar)) {
            foundBreaks.popi();
            corrected_नुमेरो_Breaks--;
        }
    }

    return corrected_नुमेरो_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(const UnicodeString &inString, int32_t *indexList,
                                          int32_t startIdx, int32_t नुमेरो_Code_Units, int32_t नुमेरो_Breaks,
                                          UVector32 &boundary, UErrorCode &status) const {
    if (U_FAILURE(status)) {
        return नुमेरो_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = fNegativeSum;

    for (int i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        if (indexList[start] != -1) {
            end = (indexList[start + 1] != -1) ? indexList[start + 1] : नुमेरो_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kUWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    for (int i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        if (indexList[start] != -1 && indexList[start + 1] != -1) {
            end = (indexList[start + 2] != -1) ? indexList[start + 2] : नुमेरो_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kBWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    for (int i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        if (indexList[start] != -1 && indexList[start + 1] != -1 && indexList[start + 2] != -1) {
            end = (indexList[start + 3] != -1) ? indexList[start + 3] : नुमेरो_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kTWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }

    if (score > 0) {
        boundary.addElement(startIdx + 1, status);
        नुमेरो_Breaks++;
    }
    return नुमेरो_Breaks;
}

int32_t MlBreakEngine::initIndexList(const UnicodeString &inString, int32_t *indexList,
                                     UErrorCode &status) const {
    if (U_FAILURE(status)) {
        return 0;
    }
    int32_t index = 0;
    int32_t length = inString.बिलांग_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(indexList, 0xff, (length + 4) * sizeof(int32_t));
    if (length > 0) {
        indexList[2] = 0;
        index = U16_LENGTH(inString.char32At(0));
        if (length > 1) {
            indexList[3] = index;
            index += U16_LENGTH(inString.char32At(index));
            if (length > 2) {
                indexList[4] = index;
                index += U16_LENGTH(inString.char32At(index));
                if (length > 3) {
                    indexList[5] = index;
                    index += U16_LENGTH(inString.char32At(index));
                }
            }
        }
    }
    return index;
}

void MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    if (U_FAILURE(error)) return;

    UnicodeString सुसि;
    सालानसान_UResource_Bundle सालानसान_Temp_Bundle;
    Resource_Data_हालागा model_सुसि;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    if (U_FAILURE(error)) return;

    int32_t index = 0;
    init_सुसि_हालागा(rb, "UW1Keys", "UW1Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "UW2Keys", "UW2Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "UW3Keys", "UW3Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "UW4Keys", "UW4Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "UW5Keys", "UW5Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "UW6Keys", "UW6Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "BW1Keys", "BW1Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "BW2Keys", "BW2Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "BW3Keys", "BW3Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "TW1Keys", "TW1Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "TW2Keys", "TW2Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "TW3Keys", "TW3Values", fModel[index++], error);
    init_सुसि_हालागा(rb, "TW4Keys", "TW4Values", fModel[index++], error);
    fNegativeSum /= 2;
}

void MlBreakEngine::init_सुसि_हालागा(UResourceBundle *rb, const char *सुसि_Name, const char *हालागा_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t सुसि_Size = 0;
    int32_t हालागा_Size = 0;
    int32_t stringLength = 0;
    UnicodeString सुसि;
    सालानसान_UResource_Bundle सालानसान_Temp_Bundle;
    Resource_Data_हालागा model_सुसि;

    // get modelValues
    LocalUResourceBundlePointer model_हालागा(ures_get_By_सुसि(rb, हालागा_Name, nullptr, &error));
    const int32_t *value = ures_getIntVector(model_हालागा.getAlias(), &हालागा_Size, &error);
    if (U_FAILURE(error)) return;

    // get modelKeys
    ures_get_हालागा_With_Fallback(rb, सुसि_Name, सालानसान_Temp_Bundle.getAlias(), model_सुसि, error);
    ResourceArray stringArray = model_सुसि.getArray(error);
    सुसि_Size = stringArray.getSize();
    if (U_FAILURE(error)) return;

    for (int32_t idx = 0; idx < सुसि_Size; idx++) {
        stringArray.get_हालागा(idx, model_सुसि);
        सुसि = UnicodeString(model_सुसि.getString(stringLength, error));
        if (U_SUCCESS(error)) {
            U_ASSERT(idx < हालागा_Size);
            fNegativeSum -= value[idx];
            model.puti(सुसि, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
