// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "mlbe.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ubrkimpl.h"
#અન્તર્ભાવ "unicode/resbund.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "util.h"
#અન્તર્ભાવ "uvectr32.h"

U_NAMESPACE_BEGIN

ગણના વર્ગ ModelIndex { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(અચલ UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             અચલ UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      fNegativeSum(0) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, અચલ UnicodeString &inString,
                                     અચલ LocalPointer<UVector32> &આગત_Map,
                                     UErrorCode &status) અચલ {
    જો (U_FAILURE(status)) {
        પાછા 0;
    }
    જો (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    UVector32 boundary(inString.ગણતરી_Char32() + 1, status);
    જો (U_FAILURE(status)) {
        પાછા 0;
    }
    int32_t અંક_Breaks = 0;
    int32_t codePointLength = inString.ગણતરી_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t indexSize = codePointLength + 4;
    LocalMemory<int32_t> indexList(static_cast<int32_t*>(uprv_malloc(indexSize * કદ(int32_t))));
    જો (indexList.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા 0;
    }
    int32_t અંક_Code_Units = initIndexList(inString, indexList.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    અંક_Breaks++;
    જો (U_FAILURE(status)) પાછા 0;

    માટે (int32_t idx = 0; idx + 1 < codePointLength && U_SUCCESS(status); idx++) {
        અંક_Breaks =
            evaluateBreakpoint(inString, indexList.getAlias(), idx, અંક_Code_Units, અંક_Breaks, boundary, status);
        જો (idx + 4 < codePointLength) {
            indexList[idx + 6] = અંક_Code_Units;
            અંક_Code_Units += U16_LENGTH(inString.char32At(indexList[idx + 6]));
        }
    }

    જો (U_FAILURE(status)) પાછા 0;

    // Add a break for the end if there is not one there already.
    જો (boundary.lastElementi() != inString.ગણતરી_Char32()) {
        boundary.addElement(inString.ગણતરી_Char32(), status);
        અંક_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_અંક_Breaks = 0;
    માટે (int32_t i = 0; i < અંક_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = આગત_Map.isValid() ? આગત_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        જો (utextPos > prevUTextPos) {
            જો (utextPos != rangeStart ||
                (utextPos > 0 &&
                 fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_અંક_Breaks++;
            }
        } નહીંતર {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --અંક_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (શૂન્ય)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 nextChar = utext_char32At(inText, rangeEnd);
    જો (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        જો (!fDigitOrOpenPunctuationOrAlphabetSet.contains(nextChar)) {
            foundBreaks.popi();
            corrected_અંક_Breaks--;
        }
    }

    પાછા corrected_અંક_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(અચલ UnicodeString &inString, int32_t *indexList,
                                          int32_t startIdx, int32_t અંક_Code_Units, int32_t અંક_Breaks,
                                          UVector32 &boundary, UErrorCode &status) અચલ {
    જો (U_FAILURE(status)) {
        પાછા અંક_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = fNegativeSum;

    માટે (પૂર્ણાંક i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        જો (indexList[start] != -1) {
            end = (indexList[start + 1] != -1) ? indexList[start + 1] : અંક_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kUWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    માટે (પૂર્ણાંક i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        જો (indexList[start] != -1 && indexList[start + 1] != -1) {
            end = (indexList[start + 2] != -1) ? indexList[start + 2] : અંક_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kBWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    માટે (પૂર્ણાંક i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        જો (indexList[start] != -1 && indexList[start + 1] != -1 && indexList[start + 2] != -1) {
            end = (indexList[start + 3] != -1) ? indexList[start + 3] : અંક_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kTWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }

    જો (score > 0) {
        boundary.addElement(startIdx + 1, status);
        અંક_Breaks++;
    }
    પાછા અંક_Breaks;
}

int32_t MlBreakEngine::initIndexList(અચલ UnicodeString &inString, int32_t *indexList,
                                     UErrorCode &status) અચલ {
    જો (U_FAILURE(status)) {
        પાછા 0;
    }
    int32_t index = 0;
    int32_t length = inString.ગણતરી_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(indexList, 0xff, (length + 4) * કદ(int32_t));
    જો (length > 0) {
        indexList[2] = 0;
        index = U16_LENGTH(inString.char32At(0));
        જો (length > 1) {
            indexList[3] = index;
            index += U16_LENGTH(inString.char32At(index));
            જો (length > 2) {
                indexList[4] = index;
                index += U16_LENGTH(inString.char32At(index));
                જો (length > 3) {
                    indexList[5] = index;
                    index += U16_LENGTH(inString.char32At(index));
                }
            }
        }
    }
    પાછા index;
}

શૂન્ય MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    જો (U_FAILURE(error)) પાછા;

    UnicodeString ચાવી;
    ઢગલો_UResource_Bundle ઢગલો_Temp_Bundle;
    Resource_Data_કિંમત model_ચાવી;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    જો (U_FAILURE(error)) પાછા;

    int32_t index = 0;
    init_ચાવી_કિંમત(rb, "UW1Keys", "UW1Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "UW2Keys", "UW2Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "UW3Keys", "UW3Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "UW4Keys", "UW4Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "UW5Keys", "UW5Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "UW6Keys", "UW6Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "BW1Keys", "BW1Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "BW2Keys", "BW2Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "BW3Keys", "BW3Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "TW1Keys", "TW1Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "TW2Keys", "TW2Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "TW3Keys", "TW3Values", fModel[index++], error);
    init_ચાવી_કિંમત(rb, "TW4Keys", "TW4Values", fModel[index++], error);
    fNegativeSum /= 2;
}

શૂન્ય MlBreakEngine::init_ચાવી_કિંમત(UResourceBundle *rb, અચલ અક્ષર *ચાવી_Name, અચલ અક્ષર *કિંમત_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t ચાવી_Size = 0;
    int32_t કિંમત_Size = 0;
    int32_t stringLength = 0;
    UnicodeString ચાવી;
    ઢગલો_UResource_Bundle ઢગલો_Temp_Bundle;
    Resource_Data_કિંમત model_ચાવી;

    // get modelValues
    LocalUResourceBundlePointer model_કિંમત(ures_get_By_ચાવી(rb, કિંમત_Name, nullptr, &error));
    અચલ int32_t *value = ures_getIntVector(model_કિંમત.getAlias(), &કિંમત_Size, &error);
    જો (U_FAILURE(error)) પાછા;

    // get modelKeys
    ures_get_કિંમત_With_Fallback(rb, ચાવી_Name, ઢગલો_Temp_Bundle.getAlias(), model_ચાવી, error);
    ResourceArray stringArray = model_ચાવી.getArray(error);
    ચાવી_Size = stringArray.getSize();
    જો (U_FAILURE(error)) પાછા;

    માટે (int32_t idx = 0; idx < ચાવી_Size; idx++) {
        stringArray.get_કિંમત(idx, model_ચાવી);
        ચાવી = UnicodeString(model_ચાવી.getString(stringLength, error));
        જો (U_SUCCESS(error)) {
            U_ASSERT(idx < કિંમત_Size);
            fNegativeSum -= value[idx];
            model.puti(ચાવી, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
