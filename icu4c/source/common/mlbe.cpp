// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "mlbe.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ubrkimpl.h"
#അന്തര്ഭാവ "unicode/resbund.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "util.h"
#അന്തര്ഭാവ "uvectr32.h"

U_NAMESPACE_BEGIN

എണ്ണൽ വർഗ്ഗം ModelIndex { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(സ്ഥിരം UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             സ്ഥിരം UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      fNegativeSum(0) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, സ്ഥിരം UnicodeString &inString,
                                     സ്ഥിരം LocalPointer<UVector32> &നിവേശം_Map,
                                     UErrorCode &status) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ 0;
    }
    എങ്കിൽ (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    UVector32 boundary(inString.എണ്ണം_Char32() + 1, status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ 0;
    }
    int32_t അക്കം_Breaks = 0;
    int32_t codePointLength = inString.എണ്ണം_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t indexSize = codePointLength + 4;
    LocalMemory<int32_t> indexList(static_cast<int32_t*>(uprv_malloc(indexSize * വലുപ്പം(int32_t))));
    എങ്കിൽ (indexList.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ 0;
    }
    int32_t അക്കം_Code_Units = initIndexList(inString, indexList.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    അക്കം_Breaks++;
    എങ്കിൽ (U_FAILURE(status)) തിരികെ 0;

    വേണ്ടി (int32_t idx = 0; idx + 1 < codePointLength && U_SUCCESS(status); idx++) {
        അക്കം_Breaks =
            evaluateBreakpoint(inString, indexList.getAlias(), idx, അക്കം_Code_Units, അക്കം_Breaks, boundary, status);
        എങ്കിൽ (idx + 4 < codePointLength) {
            indexList[idx + 6] = അക്കം_Code_Units;
            അക്കം_Code_Units += U16_LENGTH(inString.char32At(indexList[idx + 6]));
        }
    }

    എങ്കിൽ (U_FAILURE(status)) തിരികെ 0;

    // Add a break for the end if there is not one there already.
    എങ്കിൽ (boundary.lastElementi() != inString.എണ്ണം_Char32()) {
        boundary.addElement(inString.എണ്ണം_Char32(), status);
        അക്കം_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_അക്കം_Breaks = 0;
    വേണ്ടി (int32_t i = 0; i < അക്കം_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = നിവേശം_Map.isValid() ? നിവേശം_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        എങ്കിൽ (utextPos > prevUTextPos) {
            എങ്കിൽ (utextPos != rangeStart ||
                (utextPos > 0 &&
                 fClosePunctuationSet.contains(utext_char32At(inText, utextPos - 1)))) {
                foundBreaks.push(utextPos, status);
                corrected_അക്കം_Breaks++;
            }
        } അല്ലെങ്കിൽ {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --അക്കം_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (ശൂന്യം)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 nextChar = utext_char32At(inText, rangeEnd);
    എങ്കിൽ (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        എങ്കിൽ (!fDigitOrOpenPunctuationOrAlphabetSet.contains(nextChar)) {
            foundBreaks.popi();
            corrected_അക്കം_Breaks--;
        }
    }

    തിരികെ corrected_അക്കം_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(സ്ഥിരം UnicodeString &inString, int32_t *indexList,
                                          int32_t startIdx, int32_t അക്കം_Code_Units, int32_t അക്കം_Breaks,
                                          UVector32 &boundary, UErrorCode &status) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ അക്കം_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = fNegativeSum;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        എങ്കിൽ (indexList[start] != -1) {
            end = (indexList[start + 1] != -1) ? indexList[start + 1] : അക്കം_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kUWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        എങ്കിൽ (indexList[start] != -1 && indexList[start + 1] != -1) {
            end = (indexList[start + 2] != -1) ? indexList[start + 2] : അക്കം_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kBWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        എങ്കിൽ (indexList[start] != -1 && indexList[start + 1] != -1 && indexList[start + 2] != -1) {
            end = (indexList[start + 3] != -1) ? indexList[start + 3] : അക്കം_Code_Units;
            score += fModel[static_cast<int32_t>(ModelIndex::kTWStart) + i].geti(
                inString.tempSubString(indexList[start], end - indexList[start]));
        }
    }

    എങ്കിൽ (score > 0) {
        boundary.addElement(startIdx + 1, status);
        അക്കം_Breaks++;
    }
    തിരികെ അക്കം_Breaks;
}

int32_t MlBreakEngine::initIndexList(സ്ഥിരം UnicodeString &inString, int32_t *indexList,
                                     UErrorCode &status) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ 0;
    }
    int32_t index = 0;
    int32_t length = inString.എണ്ണം_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(indexList, 0xff, (length + 4) * വലുപ്പം(int32_t));
    എങ്കിൽ (length > 0) {
        indexList[2] = 0;
        index = U16_LENGTH(inString.char32At(0));
        എങ്കിൽ (length > 1) {
            indexList[3] = index;
            index += U16_LENGTH(inString.char32At(index));
            എങ്കിൽ (length > 2) {
                indexList[4] = index;
                index += U16_LENGTH(inString.char32At(index));
                എങ്കിൽ (length > 3) {
                    indexList[5] = index;
                    index += U16_LENGTH(inString.char32At(index));
                }
            }
        }
    }
    തിരികെ index;
}

ശൂന്യം MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    എങ്കിൽ (U_FAILURE(error)) തിരികെ;

    UnicodeString താക്കോൽ;
    അട്ടി_UResource_Bundle അട്ടി_Temp_Bundle;
    Resource_Data_മൂല്യം model_താക്കോൽ;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    എങ്കിൽ (U_FAILURE(error)) തിരികെ;

    int32_t index = 0;
    init_താക്കോൽ_മൂല്യം(rb, "UW1Keys", "UW1Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "UW2Keys", "UW2Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "UW3Keys", "UW3Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "UW4Keys", "UW4Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "UW5Keys", "UW5Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "UW6Keys", "UW6Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "BW1Keys", "BW1Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "BW2Keys", "BW2Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "BW3Keys", "BW3Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "TW1Keys", "TW1Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "TW2Keys", "TW2Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "TW3Keys", "TW3Values", fModel[index++], error);
    init_താക്കോൽ_മൂല്യം(rb, "TW4Keys", "TW4Values", fModel[index++], error);
    fNegativeSum /= 2;
}

ശൂന്യം MlBreakEngine::init_താക്കോൽ_മൂല്യം(UResourceBundle *rb, സ്ഥിരം അക്ഷരം *താക്കോൽ_Name, സ്ഥിരം അക്ഷരം *മൂല്യം_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t താക്കോൽ_Size = 0;
    int32_t മൂല്യം_Size = 0;
    int32_t stringLength = 0;
    UnicodeString താക്കോൽ;
    അട്ടി_UResource_Bundle അട്ടി_Temp_Bundle;
    Resource_Data_മൂല്യം model_താക്കോൽ;

    // get modelValues
    LocalUResourceBundlePointer model_മൂല്യം(ures_get_By_താക്കോൽ(rb, മൂല്യം_Name, nullptr, &error));
    സ്ഥിരം int32_t *value = ures_getIntVector(model_മൂല്യം.getAlias(), &മൂല്യം_Size, &error);
    എങ്കിൽ (U_FAILURE(error)) തിരികെ;

    // get modelKeys
    ures_get_മൂല്യം_With_Fallback(rb, താക്കോൽ_Name, അട്ടി_Temp_Bundle.getAlias(), model_താക്കോൽ, error);
    ResourceArray stringArray = model_താക്കോൽ.getArray(error);
    താക്കോൽ_Size = stringArray.getSize();
    എങ്കിൽ (U_FAILURE(error)) തിരികെ;

    വേണ്ടി (int32_t idx = 0; idx < താക്കോൽ_Size; idx++) {
        stringArray.get_മൂല്യം(idx, model_താക്കോൽ);
        താക്കോൽ = UnicodeString(model_താക്കോൽ.getString(stringLength, error));
        എങ്കിൽ (U_SUCCESS(error)) {
            U_ASSERT(idx < മൂല്യം_Size);
            fNegativeSum -= value[idx];
            model.puti(താക്കോൽ, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
