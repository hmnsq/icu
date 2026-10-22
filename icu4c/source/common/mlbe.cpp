// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "mlbe.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ubrkimpl.h"
#অন্তর্ভাব "unicode/resbund.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "util.h"
#অন্তর্ভাব "uvectr32.h"

U_NAMESPACE_BEGIN

গণনা শ্রেণী Model_সূচক { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ধ্রুবক UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                             ধ্রুবক UnicodeSet &closePunctuationSet, UErrorCode &status)
    : fDigitOrOpenPunctuationOrAlphabetSet(digitOrOpenPunctuationOrAlphabetSet),
      fClosePunctuationSet(closePunctuationSet),
      f_Negative_যোগ(0) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    loadMLModel(status);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                                     UVector32 &foundBreaks, ধ্রুবক UnicodeString &inString,
                                     ধ্রুবক LocalPointer<UVector32> &প্রবেশ_Map,
                                     UErrorCode &status) ধ্রুবক {
    যদি (U_FAILURE(status)) {
        ফেরত 0;
    }
    যদি (rangeStart >= rangeEnd) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    UVector32 boundary(inString.গুনতি_Char32() + 1, status);
    যদি (U_FAILURE(status)) {
        ফেরত 0;
    }
    int32_t অঙ্ক_Breaks = 0;
    int32_t code_Point_দৈর্ঘ্য = inString.গুনতি_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t সূচক_আকার = code_Point_দৈর্ঘ্য + 4;
    LocalMemory<int32_t> সূচক_List(static_cast<int32_t*>(uprv_malloc(সূচক_আকার * আকার(int32_t))));
    যদি (সূচক_List.isNull()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত 0;
    }
    int32_t অঙ্ক_Code_Units = init_সূচক_List(inString, সূচক_List.getAlias(), status);

    // Add a break for the start.
    boundary.addElement(0, status);
    অঙ্ক_Breaks++;
    যদি (U_FAILURE(status)) ফেরত 0;

    জন্য (int32_t idx = 0; idx + 1 < code_Point_দৈর্ঘ্য && U_SUCCESS(status); idx++) {
        অঙ্ক_Breaks =
            evaluateBreakpoint(inString, সূচক_List.getAlias(), idx, অঙ্ক_Code_Units, অঙ্ক_Breaks, boundary, status);
        যদি (idx + 4 < code_Point_দৈর্ঘ্য) {
            সূচক_List[idx + 6] = অঙ্ক_Code_Units;
            অঙ্ক_Code_Units += U16_দৈর্ঘ্য(inString.char32At(সূচক_List[idx + 6]));
        }
    }

    যদি (U_FAILURE(status)) ফেরত 0;

    // Add a break for the end if there is not one there already.
    যদি (boundary.lastElementi() != inString.গুনতি_Char32()) {
        boundary.addElement(inString.গুনতি_Char32(), status);
        অঙ্ক_Breaks++;
    }

    int32_t পূর্ব_CPPos = -1;
    int32_t পূর্ব_UText_Pos = -1;
    int32_t corrected_অঙ্ক_Breaks = 0;
    জন্য (int32_t i = 0; i < অঙ্ক_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = প্রবেশ_Map.isValid() ? প্রবেশ_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > পূর্ব_CPPos);
        U_ASSERT(utextPos >= পূর্ব_UText_Pos);

        যদি (utextPos > পূর্ব_UText_Pos) {
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
        পূর্ব_CPPos = cpPos;
        পূর্ব_UText_Pos = utextPos;
    }
    (শূন্য)পূর্ব_CPPos;  // suppress compiler warnings about unused variable

    UChar32 পরবর্তী_Char = utext_char32At(inText, rangeEnd);
    যদি (!foundBreaks.isEmpty() && foundBreaks.peeki() == rangeEnd) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        যদি (!fDigitOrOpenPunctuationOrAlphabetSet.contains(পরবর্তী_Char)) {
            foundBreaks.popi();
            corrected_অঙ্ক_Breaks--;
        }
    }

    ফেরত corrected_অঙ্ক_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ধ্রুবক UnicodeString &inString, int32_t *সূচক_List,
                                          int32_t startIdx, int32_t অঙ্ক_Code_Units, int32_t অঙ্ক_Breaks,
                                          UVector32 &boundary, UErrorCode &status) ধ্রুবক {
    যদি (U_FAILURE(status)) {
        ফেরত অঙ্ক_Breaks;
    }
    int32_t start = 0, end = 0;
    int32_t score = f_Negative_যোগ;

    জন্য (পূর্ণসংখ্যা i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        যদি (সূচক_List[start] != -1) {
            end = (সূচক_List[start + 1] != -1) ? সূচক_List[start + 1] : অঙ্ক_Code_Units;
            score += fModel[static_cast<int32_t>(Model_সূচক::kUWStart) + i].geti(
                inString.অস্থায়ী_Sub_String(সূচক_List[start], end - সূচক_List[start]));
        }
    }
    জন্য (পূর্ণসংখ্যা i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        যদি (সূচক_List[start] != -1 && সূচক_List[start + 1] != -1) {
            end = (সূচক_List[start + 2] != -1) ? সূচক_List[start + 2] : অঙ্ক_Code_Units;
            score += fModel[static_cast<int32_t>(Model_সূচক::kBWStart) + i].geti(
                inString.অস্থায়ী_Sub_String(সূচক_List[start], end - সূচক_List[start]));
        }
    }
    জন্য (পূর্ণসংখ্যা i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        যদি (সূচক_List[start] != -1 && সূচক_List[start + 1] != -1 && সূচক_List[start + 2] != -1) {
            end = (সূচক_List[start + 3] != -1) ? সূচক_List[start + 3] : অঙ্ক_Code_Units;
            score += fModel[static_cast<int32_t>(Model_সূচক::kTWStart) + i].geti(
                inString.অস্থায়ী_Sub_String(সূচক_List[start], end - সূচক_List[start]));
        }
    }

    যদি (score > 0) {
        boundary.addElement(startIdx + 1, status);
        অঙ্ক_Breaks++;
    }
    ফেরত অঙ্ক_Breaks;
}

int32_t MlBreakEngine::init_সূচক_List(ধ্রুবক UnicodeString &inString, int32_t *সূচক_List,
                                     UErrorCode &status) ধ্রুবক {
    যদি (U_FAILURE(status)) {
        ফেরত 0;
    }
    int32_t সূচক = 0;
    int32_t length = inString.গুনতি_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(সূচক_List, 0xff, (length + 4) * আকার(int32_t));
    যদি (length > 0) {
        সূচক_List[2] = 0;
        সূচক = U16_দৈর্ঘ্য(inString.char32At(0));
        যদি (length > 1) {
            সূচক_List[3] = সূচক;
            সূচক += U16_দৈর্ঘ্য(inString.char32At(সূচক));
            যদি (length > 2) {
                সূচক_List[4] = সূচক;
                সূচক += U16_দৈর্ঘ্য(inString.char32At(সূচক));
                যদি (length > 3) {
                    সূচক_List[5] = সূচক;
                    সূচক += U16_দৈর্ঘ্য(inString.char32At(সূচক));
                }
            }
        }
    }
    ফেরত সূচক;
}

শূন্য MlBreakEngine::loadMLModel(UErrorCode &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    যদি (U_FAILURE(error)) ফেরত;

    UnicodeString চাবি;
    স্তূপ_UResource_Bundle স্তূপ_অস্থায়ী_Bundle;
    Resource_তথ্য_মান model_চাবি;

    LocalUResourceBundlePointer rbp(ures_openDirect(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    যদি (U_FAILURE(error)) ফেরত;

    int32_t সূচক = 0;
    init_চাবি_মান(rb, "UW1Keys", "UW1Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "UW2Keys", "UW2Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "UW3Keys", "UW3Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "UW4Keys", "UW4Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "UW5Keys", "UW5Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "UW6Keys", "UW6Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "BW1Keys", "BW1Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "BW2Keys", "BW2Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "BW3Keys", "BW3Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "TW1Keys", "TW1Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "TW2Keys", "TW2Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "TW3Keys", "TW3Values", fModel[সূচক++], error);
    init_চাবি_মান(rb, "TW4Keys", "TW4Values", fModel[সূচক++], error);
    f_Negative_যোগ /= 2;
}

শূন্য MlBreakEngine::init_চাবি_মান(UResourceBundle *rb, ধ্রুবক অক্ষর *চাবি_Name, ধ্রুবক অক্ষর *মান_Name,
                                 Hashtable &model, UErrorCode &error) {
    int32_t চাবি_আকার = 0;
    int32_t মান_আকার = 0;
    int32_t string_দৈর্ঘ্য = 0;
    UnicodeString চাবি;
    স্তূপ_UResource_Bundle স্তূপ_অস্থায়ী_Bundle;
    Resource_তথ্য_মান model_চাবি;

    // get modelValues
    LocalUResourceBundlePointer model_মান(ures_get_By_চাবি(rb, মান_Name, nullptr, &error));
    ধ্রুবক int32_t *value = ures_getIntVector(model_মান.getAlias(), &মান_আকার, &error);
    যদি (U_FAILURE(error)) ফেরত;

    // get modelKeys
    ures_get_মান_With_Fallback(rb, চাবি_Name, স্তূপ_অস্থায়ী_Bundle.getAlias(), model_চাবি, error);
    ResourceArray stringArray = model_চাবি.getArray(error);
    চাবি_আকার = stringArray.get_আকার();
    যদি (U_FAILURE(error)) ফেরত;

    জন্য (int32_t idx = 0; idx < চাবি_আকার; idx++) {
        stringArray.get_মান(idx, model_চাবি);
        চাবি = UnicodeString(model_চাবি.getString(string_দৈর্ঘ্য, error));
        যদি (U_SUCCESS(error)) {
            U_ASSERT(idx < মান_আকার);
            f_Negative_যোগ -= value[idx];
            model.puti(চাবি, value[idx], error);
        }
    }
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
