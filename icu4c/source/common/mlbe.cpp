// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "mlbe.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ubrkimpl.h"
#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "util.h"
#अन्तर्भाव "uvectr32.h"

U_NAMESPACE_BEGIN

गणना वर्ग ModelIndex { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ध्रुव UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             ध्रुव UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      fNegativeSum(0) {
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, ध्रुव UnicodeString &inString,
                                     ध्रुव LocalPointer<UVector32> &आदान_Map,
                                     UErrorCode &status) ध्रुव {
    यदि (U_FAILURE(status)) {
        प्रत्यागम 0;
    }
    यदि (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    UVector32 boundary(inString.मोजणी_Char32() + 1, status);
    यदि (U_FAILURE(status)) {
        प्रत्यागम 0;
    }
    int32_t अंक_Breaks = 0;
    int32_t codePointLength = inString.मोजणी_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t indexSize = codePointLength + 4;
    LocalMemory<int32_t> indexList(static_cast<int32_t*>(uprv_malloc(indexSize * परिमाण(int32_t))));
    यदि (indexList.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम 0;
    }
    int32_t अंक_Code_Units = initIndexList(inString, indexList.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    अंक_Breaks++;
    यदि (U_FAILURE(status)) प्रत्यागम 0;

    प्रति (int32_t idx = 0; idx + 1 < codePointLength && U_SUCCESS(status); idx++) {
        अंक_Breaks =
            evaluateBreakpoint(inString, indexList.getAlias(), idx, अंक_Code_Units, अंक_Breaks, boundary, status);
        यदि (idx + 4 < codePointLength) {
            indexList[idx + 6] = अंक_Code_Units;
            अंक_Code_Units += U16_LENGTH(inString.char32At(indexList[idx + 6]));
        }
    }

    यदि (U_FAILURE(status)) प्रत्यागम 0;

    // Add a break for the end if there is not one there already.
    यदि (boundary.lastElementi() != inString.मोजणी_Char32()) {
        boundary.addElement(inString.मोजणी_Char32(), status);
        अंक_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_अंक_Breaks = 0;
    प्रति (int32_t i = 0; i < अंक_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = आदान_Map.isValid() ? आदान_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        यदि (utextPos > prevUTextPos) {
            यदि (utextPos != rangeStart ||
                (utextPos > 0 &&
                 fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_अंक_Breaks++;
            }
        } अन्यथा {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --अंक_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (शून्य)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 nextChar = utext_char32At(inText, rangeEnd);
    यदि (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        यदि (!fDigitOrOpenPunctuationOrAlphabetSet.contains(nextChar)) {
            foundBreaks.popi();
            corrected_अंक_Breaks--;
        }
    }

    प्रत्यागम corrected_अंक_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ध्रुव UnicodeString &inString, int32_t *indexList,
                                          int32_t startIdx, int32_t अंक_Code_Units, int32_t अंक_Breaks,
                                          UVector32 &boundary, UErrorCode &status) ध्रुव {
    यदि (U_FAILURE(status)) {
        प्रत्यागम अंक_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = fNegativeSum;

    प्रति (पूर्णाङ्क i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        यदि (indexList[start] != -1) {
            end = (indexList[start + 1] != -1) ? indexList[start + 1] : अंक_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kUWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    प्रति (पूर्णाङ्क i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        यदि (indexList[start] != -1 && indexList[start + 1] != -1) {
            end = (indexList[start + 2] != -1) ? indexList[start + 2] : अंक_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kBWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    प्रति (पूर्णाङ्क i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        यदि (indexList[start] != -1 && indexList[start + 1] != -1 && indexList[start + 2] != -1) {
            end = (indexList[start + 3] != -1) ? indexList[start + 3] : अंक_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kTWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }

    यदि (score > 0) {
        boundary.addElement(startIdx + 1, status);
        अंक_Breaks++;
    }
    प्रत्यागम अंक_Breaks;
}

int32_t MlBreakEngine::initIndexList(ध्रुव UnicodeString &inString, int32_t *indexList,
                                     UErrorCode &status) ध्रुव {
    यदि (U_FAILURE(status)) {
        प्रत्यागम 0;
    }
    int32_t index = 0;
    int32_t length = inString.मोजणी_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(indexList, 0xff, (length + 4) * परिमाण(int32_t));
    यदि (length > 0) {
        indexList[2] = 0;
        index = U16_LENGTH(inString.char32At(0));
        यदि (length > 1) {
            indexList[3] = index;
            index += U16_LENGTH(inString.char32At(index));
            यदि (length > 2) {
                indexList[4] = index;
                index += U16_LENGTH(inString.char32At(index));
                यदि (length > 3) {
                    indexList[5] = index;
                    index += U16_LENGTH(inString.char32At(index));
                }
            }
        }
    }
    प्रत्यागम index;
}

शून्य MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    यदि (U_FAILURE(error)) प्रत्यागम;

    UnicodeString किल्ली;
    चळत_UResource_Bundle चळत_Temp_Bundle;
    Resource_Data_मूल्य model_किल्ली;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    यदि (U_FAILURE(error)) प्रत्यागम;

    int32_t index = 0;
    init_किल्ली_मूल्य(rb, "UW1Keys", "UW1Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "UW2Keys", "UW2Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "UW3Keys", "UW3Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "UW4Keys", "UW4Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "UW5Keys", "UW5Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "UW6Keys", "UW6Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "BW1Keys", "BW1Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "BW2Keys", "BW2Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "BW3Keys", "BW3Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "TW1Keys", "TW1Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "TW2Keys", "TW2Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "TW3Keys", "TW3Values", fModel[index++], error);
    init_किल्ली_मूल्य(rb, "TW4Keys", "TW4Values", fModel[index++], error);
    fNegativeSum /= 2;
}

शून्य MlBreakEngine::init_किल्ली_मूल्य(UResourceBundle *rb, ध्रुव वर्ण *किल्ली_Name, ध्रुव वर्ण *मूल्य_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t किल्ली_Size = 0;
    int32_t मूल्य_Size = 0;
    int32_t stringLength = 0;
    UnicodeString किल्ली;
    चळत_UResource_Bundle चळत_Temp_Bundle;
    Resource_Data_मूल्य model_किल्ली;

    // get modelValues
    LocalUResourceBundlePointer model_मूल्य(ures_get_By_किल्ली(rb, मूल्य_Name, nullptr, &error));
    ध्रुव int32_t *value = ures_getIntVector(model_मूल्य.getAlias(), &मूल्य_Size, &error);
    यदि (U_FAILURE(error)) प्रत्यागम;

    // get modelKeys
    ures_get_मूल्य_With_Fallback(rb, किल्ली_Name, चळत_Temp_Bundle.getAlias(), model_किल्ली, error);
    ResourceArray stringArray = model_किल्ली.getArray(error);
    किल्ली_Size = stringArray.getSize();
    यदि (U_FAILURE(error)) प्रत्यागम;

    प्रति (int32_t idx = 0; idx < किल्ली_Size; idx++) {
        stringArray.get_मूल्य(idx, model_किल्ली);
        किल्ली = UnicodeString(model_किल्ली.getString(stringLength, error));
        यदि (U_SUCCESS(error)) {
            U_ASSERT(idx < मूल्य_Size);
            fNegativeSum -= value[idx];
            model.puti(किल्ली, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
