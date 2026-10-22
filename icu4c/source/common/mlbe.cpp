// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "mlbe.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ubrkimpl.h"
#ଅନ୍ତର୍ଭାଵ "unicode/resbund.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "util.h"
#ଅନ୍ତର୍ଭାଵ "uvectr32.h"

U_NAMESPACE_BEGIN

ଗଣନା ଶ୍ରେଣୀ ModelIndex { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ଅଚଳ UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             ଅଚଳ UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      fNegativeSum(0) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, ଅଚଳ UnicodeString &inString,
                                     ଅଚଳ LocalPointer<UVector32> &ନିବେଶ_Map,
                                     UErrorCode &status) ଅଚଳ {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    UVector32 boundary(inString.ଗଣନା_Char32() + 1, status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ 0;
    }
    int32_t ଅଙ୍କ_Breaks = 0;
    int32_t codePointLength = inString.ଗଣନା_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t indexSize = codePointLength + 4;
    LocalMemory<int32_t> indexList(static_cast<int32_t*>(uprv_malloc(indexSize * ଆକାର(int32_t))));
    ଯଦି (indexList.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    int32_t ଅଙ୍କ_Code_Units = initIndexList(inString, indexList.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    ଅଙ୍କ_Breaks++;
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ 0;

    ପାଇଁ (int32_t idx = 0; idx + 1 < codePointLength && U_SUCCESS(status); idx++) {
        ଅଙ୍କ_Breaks =
            evaluateBreakpoint(inString, indexList.getAlias(), idx, ଅଙ୍କ_Code_Units, ଅଙ୍କ_Breaks, boundary, status);
        ଯଦି (idx + 4 < codePointLength) {
            indexList[idx + 6] = ଅଙ୍କ_Code_Units;
            ଅଙ୍କ_Code_Units += U16_LENGTH(inString.char32At(indexList[idx + 6]));
        }
    }

    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ 0;

    // Add a break for the end if there is not one there already.
    ଯଦି (boundary.lastElementi() != inString.ଗଣନା_Char32()) {
        boundary.addElement(inString.ଗଣନା_Char32(), status);
        ଅଙ୍କ_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_ଅଙ୍କ_Breaks = 0;
    ପାଇଁ (int32_t i = 0; i < ଅଙ୍କ_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = ନିବେଶ_Map.isValid() ? ନିବେଶ_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        ଯଦି (utextPos > prevUTextPos) {
            ଯଦି (utextPos != rangeStart ||
                (utextPos > 0 &&
                 fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_ଅଙ୍କ_Breaks++;
            }
        } ଅନ୍ୟଥା {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --ଅଙ୍କ_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (ଶୂନ୍ୟ)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 nextChar = utext_char32At(inText, rangeEnd);
    ଯଦି (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        ଯଦି (!fDigitOrOpenPunctuationOrAlphabetSet.contains(nextChar)) {
            foundBreaks.popi();
            corrected_ଅଙ୍କ_Breaks--;
        }
    }

    ଫେରନ୍ତୁ corrected_ଅଙ୍କ_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ଅଚଳ UnicodeString &inString, int32_t *indexList,
                                          int32_t startIdx, int32_t ଅଙ୍କ_Code_Units, int32_t ଅଙ୍କ_Breaks,
                                          UVector32 &boundary, UErrorCode &status) ଅଚଳ {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ ଅଙ୍କ_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = fNegativeSum;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        ଯଦି (indexList[start] != -1) {
            end = (indexList[start + 1] != -1) ? indexList[start + 1] : ଅଙ୍କ_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kUWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        ଯଦି (indexList[start] != -1 && indexList[start + 1] != -1) {
            end = (indexList[start + 2] != -1) ? indexList[start + 2] : ଅଙ୍କ_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kBWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        ଯଦି (indexList[start] != -1 && indexList[start + 1] != -1 && indexList[start + 2] != -1) {
            end = (indexList[start + 3] != -1) ? indexList[start + 3] : ଅଙ୍କ_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kTWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }

    ଯଦି (score > 0) {
        boundary.addElement(startIdx + 1, status);
        ଅଙ୍କ_Breaks++;
    }
    ଫେରନ୍ତୁ ଅଙ୍କ_Breaks;
}

int32_t MlBreakEngine::initIndexList(ଅଚଳ UnicodeString &inString, int32_t *indexList,
                                     UErrorCode &status) ଅଚଳ {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ 0;
    }
    int32_t index = 0;
    int32_t length = inString.ଗଣନା_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(indexList, 0xff, (length + 4) * ଆକାର(int32_t));
    ଯଦି (length > 0) {
        indexList[2] = 0;
        index = U16_LENGTH(inString.char32At(0));
        ଯଦି (length > 1) {
            indexList[3] = index;
            index += U16_LENGTH(inString.char32At(index));
            ଯଦି (length > 2) {
                indexList[4] = index;
                index += U16_LENGTH(inString.char32At(index));
                ଯଦି (length > 3) {
                    indexList[5] = index;
                    index += U16_LENGTH(inString.char32At(index));
                }
            }
        }
    }
    ଫେରନ୍ତୁ index;
}

ଶୂନ୍ୟ MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    ଯଦି (U_FAILURE(error)) ଫେରନ୍ତୁ;

    UnicodeString ଚାବି;
    ଗଦା_UResource_Bundle ଗଦା_Temp_Bundle;
    Resource_Data_ମୂଲ୍ୟ model_ଚାବି;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    ଯଦି (U_FAILURE(error)) ଫେରନ୍ତୁ;

    int32_t index = 0;
    init_ଚାବି_ମୂଲ୍ୟ(rb, "UW1Keys", "UW1Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "UW2Keys", "UW2Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "UW3Keys", "UW3Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "UW4Keys", "UW4Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "UW5Keys", "UW5Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "UW6Keys", "UW6Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "BW1Keys", "BW1Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "BW2Keys", "BW2Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "BW3Keys", "BW3Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "TW1Keys", "TW1Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "TW2Keys", "TW2Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "TW3Keys", "TW3Values", fModel[index++], error);
    init_ଚାବି_ମୂଲ୍ୟ(rb, "TW4Keys", "TW4Values", fModel[index++], error);
    fNegativeSum /= 2;
}

ଶୂନ୍ୟ MlBreakEngine::init_ଚାବି_ମୂଲ୍ୟ(UResourceBundle *rb, ଅଚଳ ଅକ୍ଷର *ଚାବି_Name, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t ଚାବି_Size = 0;
    int32_t ମୂଲ୍ୟ_Size = 0;
    int32_t stringLength = 0;
    UnicodeString ଚାବି;
    ଗଦା_UResource_Bundle ଗଦା_Temp_Bundle;
    Resource_Data_ମୂଲ୍ୟ model_ଚାବି;

    // get modelValues
    LocalUResourceBundlePointer model_ମୂଲ୍ୟ(ures_get_By_ଚାବି(rb, ମୂଲ୍ୟ_Name, nullptr, &error));
    ଅଚଳ int32_t *value = ures_getIntVector(model_ମୂଲ୍ୟ.getAlias(), &ମୂଲ୍ୟ_Size, &error);
    ଯଦି (U_FAILURE(error)) ଫେରନ୍ତୁ;

    // get modelKeys
    ures_get_ମୂଲ୍ୟ_With_Fallback(rb, ଚାବି_Name, ଗଦା_Temp_Bundle.getAlias(), model_ଚାବି, error);
    ResourceArray stringArray = model_ଚାବି.getArray(error);
    ଚାବି_Size = stringArray.getSize();
    ଯଦି (U_FAILURE(error)) ଫେରନ୍ତୁ;

    ପାଇଁ (int32_t idx = 0; idx < ଚାବି_Size; idx++) {
        stringArray.get_ମୂଲ୍ୟ(idx, model_ଚାବି);
        ଚାବି = UnicodeString(model_ଚାବି.getString(stringLength, error));
        ଯଦି (U_SUCCESS(error)) {
            U_ASSERT(idx < ମୂଲ୍ୟ_Size);
            fNegativeSum -= value[idx];
            model.puti(ଚାବି, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
