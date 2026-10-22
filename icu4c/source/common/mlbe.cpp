// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "mlbe.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ubrkimpl.h"
#அந்தர்பாவ "unicode/resbund.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "util.h"
#அந்தர்பாவ "uvectr32.h"

U_NAMESPACE_BEGIN

எண்ணிக்கை வகுப்பு ModelIndex { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(மாறிலி UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             மாறிலி UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      fNegativeSum(0) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, மாறிலி UnicodeString &inString,
                                     மாறிலி LocalPointer<UVector32> &உள்ளீடு_Map,
                                     UErrorCode &status) மாறிலி {
    எனில் (U_FAILURE(status)) {
        திரும்பு 0;
    }
    எனில் (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    UVector32 boundary(inString.மொத்தம்_Char32() + 1, status);
    எனில் (U_FAILURE(status)) {
        திரும்பு 0;
    }
    int32_t இலக்கம்_Breaks = 0;
    int32_t codePointLength = inString.மொத்தம்_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t indexSize = codePointLength + 4;
    LocalMemory<int32_t> indexList(static_cast<int32_t*>(uprv_malloc(indexSize * அளவு(int32_t))));
    எனில் (indexList.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு 0;
    }
    int32_t இலக்கம்_Code_Units = initIndexList(inString, indexList.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    இலக்கம்_Breaks++;
    எனில் (U_FAILURE(status)) திரும்பு 0;

    க்கு (int32_t idx = 0; idx + 1 < codePointLength && U_SUCCESS(status); idx++) {
        இலக்கம்_Breaks =
            evaluateBreakpoint(inString, indexList.getAlias(), idx, இலக்கம்_Code_Units, இலக்கம்_Breaks, boundary, status);
        எனில் (idx + 4 < codePointLength) {
            indexList[idx + 6] = இலக்கம்_Code_Units;
            இலக்கம்_Code_Units += U16_LENGTH(inString.char32At(indexList[idx + 6]));
        }
    }

    எனில் (U_FAILURE(status)) திரும்பு 0;

    // Add a break for the end if there is not one there already.
    எனில் (boundary.lastElementi() != inString.மொத்தம்_Char32()) {
        boundary.addElement(inString.மொத்தம்_Char32(), status);
        இலக்கம்_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_இலக்கம்_Breaks = 0;
    க்கு (int32_t i = 0; i < இலக்கம்_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = உள்ளீடு_Map.isValid() ? உள்ளீடு_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        எனில் (utextPos > prevUTextPos) {
            எனில் (utextPos != rangeStart ||
                (utextPos > 0 &&
                 fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_இலக்கம்_Breaks++;
            }
        } இல்லையெனில் {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --இலக்கம்_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (வெறுமை)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 nextChar = utext_char32At(inText, rangeEnd);
    எனில் (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        எனில் (!fDigitOrOpenPunctuationOrAlphabetSet.contains(nextChar)) {
            foundBreaks.popi();
            corrected_இலக்கம்_Breaks--;
        }
    }

    திரும்பு corrected_இலக்கம்_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(மாறிலி UnicodeString &inString, int32_t *indexList,
                                          int32_t startIdx, int32_t இலக்கம்_Code_Units, int32_t இலக்கம்_Breaks,
                                          UVector32 &boundary, UErrorCode &status) மாறிலி {
    எனில் (U_FAILURE(status)) {
        திரும்பு இலக்கம்_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = fNegativeSum;

    க்கு (முழு i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        எனில் (indexList[start] != -1) {
            end = (indexList[start + 1] != -1) ? indexList[start + 1] : இலக்கம்_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kUWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    க்கு (முழு i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        எனில் (indexList[start] != -1 && indexList[start + 1] != -1) {
            end = (indexList[start + 2] != -1) ? indexList[start + 2] : இலக்கம்_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kBWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    க்கு (முழு i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        எனில் (indexList[start] != -1 && indexList[start + 1] != -1 && indexList[start + 2] != -1) {
            end = (indexList[start + 3] != -1) ? indexList[start + 3] : இலக்கம்_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kTWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }

    எனில் (score > 0) {
        boundary.addElement(startIdx + 1, status);
        இலக்கம்_Breaks++;
    }
    திரும்பு இலக்கம்_Breaks;
}

int32_t MlBreakEngine::initIndexList(மாறிலி UnicodeString &inString, int32_t *indexList,
                                     UErrorCode &status) மாறிலி {
    எனில் (U_FAILURE(status)) {
        திரும்பு 0;
    }
    int32_t index = 0;
    int32_t length = inString.மொத்தம்_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(indexList, 0xff, (length + 4) * அளவு(int32_t));
    எனில் (length > 0) {
        indexList[2] = 0;
        index = U16_LENGTH(inString.char32At(0));
        எனில் (length > 1) {
            indexList[3] = index;
            index += U16_LENGTH(inString.char32At(index));
            எனில் (length > 2) {
                indexList[4] = index;
                index += U16_LENGTH(inString.char32At(index));
                எனில் (length > 3) {
                    indexList[5] = index;
                    index += U16_LENGTH(inString.char32At(index));
                }
            }
        }
    }
    திரும்பு index;
}

வெறுமை MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    எனில் (U_FAILURE(error)) திரும்பு;

    UnicodeString சாவி;
    அடுக்கு_UResource_Bundle அடுக்கு_Temp_Bundle;
    Resource_Data_மதிப்பு model_சாவி;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    எனில் (U_FAILURE(error)) திரும்பு;

    int32_t index = 0;
    init_சாவி_மதிப்பு(rb, "UW1Keys", "UW1Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "UW2Keys", "UW2Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "UW3Keys", "UW3Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "UW4Keys", "UW4Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "UW5Keys", "UW5Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "UW6Keys", "UW6Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "BW1Keys", "BW1Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "BW2Keys", "BW2Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "BW3Keys", "BW3Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "TW1Keys", "TW1Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "TW2Keys", "TW2Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "TW3Keys", "TW3Values", fModel[index++], error);
    init_சாவி_மதிப்பு(rb, "TW4Keys", "TW4Values", fModel[index++], error);
    fNegativeSum /= 2;
}

வெறுமை MlBreakEngine::init_சாவி_மதிப்பு(UResourceBundle *rb, மாறிலி எழுத்து *சாவி_Name, மாறிலி எழுத்து *மதிப்பு_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t சாவி_Size = 0;
    int32_t மதிப்பு_Size = 0;
    int32_t stringLength = 0;
    UnicodeString சாவி;
    அடுக்கு_UResource_Bundle அடுக்கு_Temp_Bundle;
    Resource_Data_மதிப்பு model_சாவி;

    // get modelValues
    LocalUResourceBundlePointer model_மதிப்பு(ures_get_By_சாவி(rb, மதிப்பு_Name, nullptr, &error));
    மாறிலி int32_t *value = ures_getIntVector(model_மதிப்பு.getAlias(), &மதிப்பு_Size, &error);
    எனில் (U_FAILURE(error)) திரும்பு;

    // get modelKeys
    ures_get_மதிப்பு_With_Fallback(rb, சாவி_Name, அடுக்கு_Temp_Bundle.getAlias(), model_சாவி, error);
    ResourceArray stringArray = model_சாவி.getArray(error);
    சாவி_Size = stringArray.getSize();
    எனில் (U_FAILURE(error)) திரும்பு;

    க்கு (int32_t idx = 0; idx < சாவி_Size; idx++) {
        stringArray.get_மதிப்பு(idx, model_சாவி);
        சாவி = UnicodeString(model_சாவி.getString(stringLength, error));
        எனில் (U_SUCCESS(error)) {
            U_ASSERT(idx < மதிப்பு_Size);
            fNegativeSum -= value[idx];
            model.puti(சாவி, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
