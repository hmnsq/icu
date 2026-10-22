// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "cmemory.h"
#समावेश "mlbe.h"
#समावेश "uassert.h"
#समावेश "ubrkimpl.h"
#समावेश "unicode/resbund.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/utf16.h"
#समावेश "uresimp.h"
#समावेश "util.h"
#समावेश "uvectr32.h"

U_NAMESPACE_BEGIN

क्रमागत श्रेणी Model_सूचकांक { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(स्थिर UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             स्थिर UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      f_Negative_योग(0) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, स्थिर UnicodeString &inString,
                                     स्थिर LocalPointer<UVector32> &निवेश_Map,
                                     UErrorCode &status) स्थिर {
    अगर (U_FAILURE(status)) {
        वापस 0;
    }
    अगर (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    UVector32 boundary(inString.गिनती_Char32() + 1, status);
    अगर (U_FAILURE(status)) {
        वापस 0;
    }
    int32_t अंक_Breaks = 0;
    int32_t code_Point_लंबाई = inString.गिनती_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t सूचकांक_आकार = code_Point_लंबाई + 4;
    LocalMemory<int32_t> सूचकांक_List(static_cast<int32_t*>(uprv_malloc(सूचकांक_आकार * माप(int32_t))));
    अगर (सूचकांक_List.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस 0;
    }
    int32_t अंक_Code_Units = init_सूचकांक_List(inString, सूचकांक_List.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    अंक_Breaks++;
    अगर (U_FAILURE(status)) वापस 0;

    क्रम (int32_t idx = 0; idx + 1 < code_Point_लंबाई && U_SUCCESS(status); idx++) {
        अंक_Breaks =
            evaluateBreakpoint(inString, सूचकांक_List.getAlias(), idx, अंक_Code_Units, अंक_Breaks, boundary, status);
        अगर (idx + 4 < code_Point_लंबाई) {
            सूचकांक_List[idx + 6] = अंक_Code_Units;
            अंक_Code_Units += U16_लंबाई(inString.char32At(सूचकांक_List[idx + 6]));
        }
    }

    अगर (U_FAILURE(status)) वापस 0;

    // Add a break for the end if there is not one there already.
    अगर (boundary.lastElementi() != inString.गिनती_Char32()) {
        boundary.addElement(inString.गिनती_Char32(), status);
        अंक_Breaks++;
    }

    int32_t पूर्व_CPPos = -1;
    int32_t पूर्व_UText_Pos = -1;
    int32_t corrected_अंक_Breaks = 0;
    क्रम (int32_t i = 0; i < अंक_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = निवेश_Map.isValid() ? निवेश_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > पूर्व_CPPos);
        U_ASSERT(utextPos >= पूर्व_UText_Pos);

        अगर (utextPos > पूर्व_UText_Pos) {
            अगर (utextPos != rangeStart ||
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
        पूर्व_CPPos = cpPos;
        पूर्व_UText_Pos = utextPos;
    }
    (व्योम)पूर्व_CPPos;  // suppress compiler warnings about unused variable

    UChar32 अग्र_Char = utext_char32At(inText, rangeEnd);
    अगर (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        अगर (!fDigitOrOpenPunctuationOrAlphabetSet.contains(अग्र_Char)) {
            foundBreaks.popi();
            corrected_अंक_Breaks--;
        }
    }

    वापस corrected_अंक_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(स्थिर UnicodeString &inString, int32_t *सूचकांक_List,
                                          int32_t startIdx, int32_t अंक_Code_Units, int32_t अंक_Breaks,
                                          UVector32 &boundary, UErrorCode &status) स्थिर {
    अगर (U_FAILURE(status)) {
        वापस अंक_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = f_Negative_योग;

    क्रम (पूर्णांक i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        अगर (सूचकांक_List[start] != -1) {
            end = (सूचकांक_List[start + 1] != -1) ? सूचकांक_List[start + 1] : अंक_Code_Units;
            score += fModel[static_cast<int32_t>(Model_सूचकांक::kUWStart) + i].geti(
                inString.अस्थायी_Sub_String(सूचकांक_List[start], end - सूचकांक_List[start]));
        }
    }
    क्रम (पूर्णांक i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        अगर (सूचकांक_List[start] != -1 && सूचकांक_List[start + 1] != -1) {
            end = (सूचकांक_List[start + 2] != -1) ? सूचकांक_List[start + 2] : अंक_Code_Units;
            score += fModel[static_cast<int32_t>(Model_सूचकांक::kBWStart) + i].geti(
                inString.अस्थायी_Sub_String(सूचकांक_List[start], end - सूचकांक_List[start]));
        }
    }
    क्रम (पूर्णांक i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        अगर (सूचकांक_List[start] != -1 && सूचकांक_List[start + 1] != -1 && सूचकांक_List[start + 2] != -1) {
            end = (सूचकांक_List[start + 3] != -1) ? सूचकांक_List[start + 3] : अंक_Code_Units;
            score += fModel[static_cast<int32_t>(Model_सूचकांक::kTWStart) + i].geti(
                inString.अस्थायी_Sub_String(सूचकांक_List[start], end - सूचकांक_List[start]));
        }
    }

    अगर (score > 0) {
        boundary.addElement(startIdx + 1, status);
        अंक_Breaks++;
    }
    वापस अंक_Breaks;
}

int32_t MlBreakEngine::init_सूचकांक_List(स्थिर UnicodeString &inString, int32_t *सूचकांक_List,
                                     UErrorCode &status) स्थिर {
    अगर (U_FAILURE(status)) {
        वापस 0;
    }
    int32_t सूचकांक = 0;
    int32_t length = inString.गिनती_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(सूचकांक_List, 0xff, (length + 4) * माप(int32_t));
    अगर (length > 0) {
        सूचकांक_List[2] = 0;
        सूचकांक = U16_लंबाई(inString.char32At(0));
        अगर (length > 1) {
            सूचकांक_List[3] = सूचकांक;
            सूचकांक += U16_लंबाई(inString.char32At(सूचकांक));
            अगर (length > 2) {
                सूचकांक_List[4] = सूचकांक;
                सूचकांक += U16_लंबाई(inString.char32At(सूचकांक));
                अगर (length > 3) {
                    सूचकांक_List[5] = सूचकांक;
                    सूचकांक += U16_लंबाई(inString.char32At(सूचकांक));
                }
            }
        }
    }
    वापस सूचकांक;
}

व्योम MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    अगर (U_FAILURE(error)) वापस;

    UnicodeString कुंजी;
    ढेर_UResource_Bundle ढेर_अस्थायी_Bundle;
    Resource_तथ्य_मूल्य model_कुंजी;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    अगर (U_FAILURE(error)) वापस;

    int32_t सूचकांक = 0;
    init_कुंजी_मूल्य(rb, "UW1Keys", "UW1Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "UW2Keys", "UW2Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "UW3Keys", "UW3Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "UW4Keys", "UW4Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "UW5Keys", "UW5Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "UW6Keys", "UW6Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "BW1Keys", "BW1Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "BW2Keys", "BW2Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "BW3Keys", "BW3Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "TW1Keys", "TW1Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "TW2Keys", "TW2Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "TW3Keys", "TW3Values", fModel[सूचकांक++], error);
    init_कुंजी_मूल्य(rb, "TW4Keys", "TW4Values", fModel[सूचकांक++], error);
    f_Negative_योग /= 2;
}

व्योम MlBreakEngine::init_कुंजी_मूल्य(UResourceBundle *rb, स्थिर अक्षर *कुंजी_Name, स्थिर अक्षर *मूल्य_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t कुंजी_आकार = 0;
    int32_t मूल्य_आकार = 0;
    int32_t string_लंबाई = 0;
    UnicodeString कुंजी;
    ढेर_UResource_Bundle ढेर_अस्थायी_Bundle;
    Resource_तथ्य_मूल्य model_कुंजी;

    // get modelValues
    LocalUResourceBundlePointer model_मूल्य(ures_get_By_कुंजी(rb, मूल्य_Name, nullptr, &error));
    स्थिर int32_t *value = ures_getIntVector(model_मूल्य.getAlias(), &मूल्य_आकार, &error);
    अगर (U_FAILURE(error)) वापस;

    // get modelKeys
    ures_get_मूल्य_With_Fallback(rb, कुंजी_Name, ढेर_अस्थायी_Bundle.getAlias(), model_कुंजी, error);
    ResourceArray stringArray = model_कुंजी.getArray(error);
    कुंजी_आकार = stringArray.get_आकार();
    अगर (U_FAILURE(error)) वापस;

    क्रम (int32_t idx = 0; idx < कुंजी_आकार; idx++) {
        stringArray.get_मूल्य(idx, model_कुंजी);
        कुंजी = UnicodeString(model_कुंजी.getString(string_लंबाई, error));
        अगर (U_SUCCESS(error)) {
            U_ASSERT(idx < मूल्य_आकार);
            f_Negative_योग -= value[idx];
            model.puti(कुंजी, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */
