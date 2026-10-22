// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "cmemory.h"
#شامل "mlbe.h"
#شامل "uassert.h"
#شامل "ubrkimpl.h"
#شامل "unicode/resbund.h"
#شامل "unicode/udata.h"
#شامل "unicode/utf16.h"
#شامل "uresimp.h"
#شامل "util.h"
#شامل "uvectr32.h"

U_NAMESPACE_BEGIN

شمارش رده ModelIndex { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ثابت UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             ثابت UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      fNegativeSum(0) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, ثابت UnicodeString &inString,
                                     ثابت LocalPointer<UVector32> &درون_داد_Map,
                                     UErrorCode &status) ثابت {
    اگر (U_FAILURE(status)) {
        بازگشت 0;
    }
    اگر (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    UVector32 boundary(inString.شمار_Char32() + 1, status);
    اگر (U_FAILURE(status)) {
        بازگشت 0;
    }
    int32_t رقم_Breaks = 0;
    int32_t codePointLength = inString.شمار_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t indexSize = codePointLength + 4;
    LocalMemory<int32_t> indexList(static_cast<int32_t*>(uprv_malloc(indexSize * اندازه(int32_t))));
    اگر (indexList.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت 0;
    }
    int32_t رقم_Code_Units = initIndexList(inString, indexList.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    رقم_Breaks++;
    اگر (U_FAILURE(status)) بازگشت 0;

    برای (int32_t idx = 0; idx + 1 < codePointLength && U_SUCCESS(status); idx++) {
        رقم_Breaks =
            evaluateBreakpoint(inString, indexList.getAlias(), idx, رقم_Code_Units, رقم_Breaks, boundary, status);
        اگر (idx + 4 < codePointLength) {
            indexList[idx + 6] = رقم_Code_Units;
            رقم_Code_Units += U16_LENGTH(inString.char32At(indexList[idx + 6]));
        }
    }

    اگر (U_FAILURE(status)) بازگشت 0;

    // Add a break for the end if there is not one there already.
    اگر (boundary.lastElementi() != inString.شمار_Char32()) {
        boundary.addElement(inString.شمار_Char32(), status);
        رقم_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_رقم_Breaks = 0;
    برای (int32_t i = 0; i < رقم_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = درون_داد_Map.isValid() ? درون_داد_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        اگر (utextPos > prevUTextPos) {
            اگر (utextPos != rangeStart ||
                (utextPos > 0 &&
                 fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_رقم_Breaks++;
            }
        } وگرنه {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --رقم_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (تهی)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 nextChar = utext_char32At(inText, rangeEnd);
    اگر (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        اگر (!fDigitOrOpenPunctuationOrAlphabetSet.contains(nextChar)) {
            foundBreaks.popi();
            corrected_رقم_Breaks--;
        }
    }

    بازگشت corrected_رقم_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ثابت UnicodeString &inString, int32_t *indexList,
                                          int32_t startIdx, int32_t رقم_Code_Units, int32_t رقم_Breaks,
                                          UVector32 &boundary, UErrorCode &status) ثابت {
    اگر (U_FAILURE(status)) {
        بازگشت رقم_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = fNegativeSum;

    برای (صحیح i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        اگر (indexList[start] != -1) {
            end = (indexList[start + 1] != -1) ? indexList[start + 1] : رقم_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kUWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    برای (صحیح i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        اگر (indexList[start] != -1 && indexList[start + 1] != -1) {
            end = (indexList[start + 2] != -1) ? indexList[start + 2] : رقم_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kBWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    برای (صحیح i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        اگر (indexList[start] != -1 && indexList[start + 1] != -1 && indexList[start + 2] != -1) {
            end = (indexList[start + 3] != -1) ? indexList[start + 3] : رقم_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kTWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }

    اگر (score > 0) {
        boundary.addElement(startIdx + 1, status);
        رقم_Breaks++;
    }
    بازگشت رقم_Breaks;
}

int32_t MlBreakEngine::initIndexList(ثابت UnicodeString &inString, int32_t *indexList,
                                     UErrorCode &status) ثابت {
    اگر (U_FAILURE(status)) {
        بازگشت 0;
    }
    int32_t index = 0;
    int32_t length = inString.شمار_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(indexList, 0xff, (length + 4) * اندازه(int32_t));
    اگر (length > 0) {
        indexList[2] = 0;
        index = U16_LENGTH(inString.char32At(0));
        اگر (length > 1) {
            indexList[3] = index;
            index += U16_LENGTH(inString.char32At(index));
            اگر (length > 2) {
                indexList[4] = index;
                index += U16_LENGTH(inString.char32At(index));
                اگر (length > 3) {
                    indexList[5] = index;
                    index += U16_LENGTH(inString.char32At(index));
                }
            }
        }
    }
    بازگشت index;
}

تهی MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    اگر (U_FAILURE(error)) بازگشت;

    UnicodeString کلید;
    پشته_UResource_Bundle پشته_Temp_Bundle;
    Resource_Data_مقدار model_کلید;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    اگر (U_FAILURE(error)) بازگشت;

    int32_t index = 0;
    init_کلید_مقدار(rb, "UW1Keys", "UW1Values", fModel[index++], error);
    init_کلید_مقدار(rb, "UW2Keys", "UW2Values", fModel[index++], error);
    init_کلید_مقدار(rb, "UW3Keys", "UW3Values", fModel[index++], error);
    init_کلید_مقدار(rb, "UW4Keys", "UW4Values", fModel[index++], error);
    init_کلید_مقدار(rb, "UW5Keys", "UW5Values", fModel[index++], error);
    init_کلید_مقدار(rb, "UW6Keys", "UW6Values", fModel[index++], error);
    init_کلید_مقدار(rb, "BW1Keys", "BW1Values", fModel[index++], error);
    init_کلید_مقدار(rb, "BW2Keys", "BW2Values", fModel[index++], error);
    init_کلید_مقدار(rb, "BW3Keys", "BW3Values", fModel[index++], error);
    init_کلید_مقدار(rb, "TW1Keys", "TW1Values", fModel[index++], error);
    init_کلید_مقدار(rb, "TW2Keys", "TW2Values", fModel[index++], error);
    init_کلید_مقدار(rb, "TW3Keys", "TW3Values", fModel[index++], error);
    init_کلید_مقدار(rb, "TW4Keys", "TW4Values", fModel[index++], error);
    fNegativeSum /= 2;
}

تهی MlBreakEngine::init_کلید_مقدار(UResourceBundle *rb, ثابت نویسه *کلید_Name, ثابت نویسه *مقدار_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t کلید_Size = 0;
    int32_t مقدار_Size = 0;
    int32_t stringLength = 0;
    UnicodeString کلید;
    پشته_UResource_Bundle پشته_Temp_Bundle;
    Resource_Data_مقدار model_کلید;

    // get modelValues
    LocalUResourceBundlePointer model_مقدار(ures_get_By_کلید(rb, مقدار_Name, nullptr, &error));
    ثابت int32_t *value = ures_getIntVector(model_مقدار.getAlias(), &مقدار_Size, &error);
    اگر (U_FAILURE(error)) بازگشت;

    // get modelKeys
    ures_get_مقدار_With_Fallback(rb, کلید_Name, پشته_Temp_Bundle.getAlias(), model_کلید, error);
    ResourceArray stringArray = model_کلید.getArray(error);
    کلید_Size = stringArray.getSize();
    اگر (U_FAILURE(error)) بازگشت;

    برای (int32_t idx = 0; idx < کلید_Size; idx++) {
        stringArray.get_مقدار(idx, model_کلید);
        کلید = UnicodeString(model_کلید.getString(stringLength, error));
        اگر (U_SUCCESS(error)) {
            U_ASSERT(idx < مقدار_Size);
            fNegativeSum -= value[idx];
            model.puti(کلید, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
