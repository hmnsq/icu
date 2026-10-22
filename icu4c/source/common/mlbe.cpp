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

गणना वर्ग Model_सूचकांक { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(ध्रुव Unicode_समुच्चयः &digit_Or_Open_Punctuation_Or_Alphabet_समुच्चयः,
                             ध्रुव Unicode_समुच्चयः &close_Punctuation_समुच्चयः, UError_संहिता &स्थितिः)
    : f_Digit_Or_Open_Punctuation_Or_Alphabet_समुच्चयः(digit_Or_Open_Punctuation_Or_Alphabet_समुच्चयः),
      f_Close_Punctuation_समुच्चयः(close_Punctuation_समुच्चयः),
      f_Negative_योग(0) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    आरोपय_MLModel(स्थितिः);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divide_ऊर्ध्वम्_Range(UText *अन्तर्_Text, int32_t rangeStart, int32_t range_अन्तः,
                                     UVector32 &प्राप्तम्_Breaks, ध्रुव UnicodeString &अन्तर्_String,
                                     ध्रुव LocalPointer<UVector32> &निवेशः_Map,
                                     UError_संहिता &स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम 0;
    }
    यदि (rangeStart >= range_अन्तः) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    UVector32 boundary(अन्तर्_String.गणनम्_Char32() + 1, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम 0;
    }
    int32_t अङ्कः_Breaks = 0;
    int32_t संहिता_Point_दीर्घता = अन्तर्_String.गणनम्_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t सूचकांक_आकार = संहिता_Point_दीर्घता + 4;
    Local_स्मृतिः<int32_t> सूचकांक_List(static_cast<int32_t*>(uprv_malloc(सूचकांक_आकार * परिमाण(int32_t))));
    यदि (सूचकांक_List.isNull()) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम 0;
    }
    int32_t अङ्कः_संहिता_Units = init_सूचकांक_List(अन्तर्_String, सूचकांक_List.getAlias(), स्थितिः);

    // Add a break for the start.
    boundary.सङ्कलय_Element(0, स्थितिः);
    अङ्कः_Breaks++;
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;

    प्रति (int32_t idx = 0; idx + 1 < संहिता_Point_दीर्घता && U_SUCCESS(स्थितिः); idx++) {
        अङ्कः_Breaks =
            evaluateBreakpoint(अन्तर्_String, सूचकांक_List.getAlias(), idx, अङ्कः_संहिता_Units, अङ्कः_Breaks, boundary, स्थितिः);
        यदि (idx + 4 < संहिता_Point_दीर्घता) {
            सूचकांक_List[idx + 6] = अङ्कः_संहिता_Units;
            अङ्कः_संहिता_Units += U16_दीर्घता(अन्तर्_String.char32At(सूचकांक_List[idx + 6]));
        }
    }

    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;

    // Add a break for the end if there is not one there already.
    यदि (boundary.चरमः_Elementi() != अन्तर्_String.गणनम्_Char32()) {
        boundary.सङ्कलय_Element(अन्तर्_String.गणनम्_Char32(), स्थितिः);
        अङ्कः_Breaks++;
    }

    int32_t पूर्व_CPPos = -1;
    int32_t पूर्व_UText_Pos = -1;
    int32_t corrected_अङ्कः_Breaks = 0;
    प्रति (int32_t i = 0; i < अङ्कः_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = निवेशः_Map.isValid() ? निवेशः_Map->elementAti(cpPos) : cpPos + rangeStart;
        U_ASSERT(cpPos > पूर्व_CPPos);
        U_ASSERT(utextPos >= पूर्व_UText_Pos);

        यदि (utextPos > पूर्व_UText_Pos) {
            यदि (utextPos != rangeStart ||
                (utextPos > 0 &&
                 f_Close_Punctuation_समुच्चयः.अन्तर्भवति(utext_char32At(अन्तर्_Text, utextPos - 1)))) {
                प्राप्तम्_Breaks.push(utextPos, स्थितिः);
                corrected_अङ्कः_Breaks++;
            }
        } अन्यथा {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --अङ्कः_Breaks;
        }
        पूर्व_CPPos = cpPos;
        पूर्व_UText_Pos = utextPos;
    }
    (शून्य)पूर्व_CPPos;  // suppress compiler warnings about unused variable

    UChar32 अग्र_Char = utext_char32At(अन्तर्_Text, range_अन्तः);
    यदि (!प्राप्तम्_Breaks.is_रिक्तानि() && प्राप्तम्_Breaks.peeki() == range_अन्तः) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        यदि (!f_Digit_Or_Open_Punctuation_Or_Alphabet_समुच्चयः.अन्तर्भवति(अग्र_Char)) {
            प्राप्तम्_Breaks.popi();
            corrected_अङ्कः_Breaks--;
        }
    }

    प्रत्यागम corrected_अङ्कः_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(ध्रुव UnicodeString &अन्तर्_String, int32_t *सूचकांक_List,
                                          int32_t startIdx, int32_t अङ्कः_संहिता_Units, int32_t अङ्कः_Breaks,
                                          UVector32 &boundary, UError_संहिता &स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम अङ्कः_Breaks;
    }
    int32_t start = 0, अन्तः = 0;
    int32_t score = f_Negative_योग;

    प्रति (पूर्णाङ्क i = 0; i < 6; i++) {
        // UW1 ~ UW6
        start = startIdx + i;
        यदि (सूचकांक_List[start] != -1) {
            अन्तः = (सूचकांक_List[start + 1] != -1) ? सूचकांक_List[start + 1] : अङ्कः_संहिता_Units;
            score += fModel[static_cast<int32_t>(Model_सूचकांक::kUWStart) + i].geti(
                अन्तर्_String.अस्थायी_Sub_String(सूचकांक_List[start], अन्तः - सूचकांक_List[start]));
        }
    }
    प्रति (पूर्णाङ्क i = 0; i < 3; i++) {
        // BW1 ~ BW3
        start = startIdx + i + 1;
        यदि (सूचकांक_List[start] != -1 && सूचकांक_List[start + 1] != -1) {
            अन्तः = (सूचकांक_List[start + 2] != -1) ? सूचकांक_List[start + 2] : अङ्कः_संहिता_Units;
            score += fModel[static_cast<int32_t>(Model_सूचकांक::kBWStart) + i].geti(
                अन्तर्_String.अस्थायी_Sub_String(सूचकांक_List[start], अन्तः - सूचकांक_List[start]));
        }
    }
    प्रति (पूर्णाङ्क i = 0; i < 4; i++) {
        // TW1 ~ TW4
        start = startIdx + i;
        यदि (सूचकांक_List[start] != -1 && सूचकांक_List[start + 1] != -1 && सूचकांक_List[start + 2] != -1) {
            अन्तः = (सूचकांक_List[start + 3] != -1) ? सूचकांक_List[start + 3] : अङ्कः_संहिता_Units;
            score += fModel[static_cast<int32_t>(Model_सूचकांक::kTWStart) + i].geti(
                अन्तर्_String.अस्थायी_Sub_String(सूचकांक_List[start], अन्तः - सूचकांक_List[start]));
        }
    }

    यदि (score > 0) {
        boundary.सङ्कलय_Element(startIdx + 1, स्थितिः);
        अङ्कः_Breaks++;
    }
    प्रत्यागम अङ्कः_Breaks;
}

int32_t MlBreakEngine::init_सूचकांक_List(ध्रुव UnicodeString &अन्तर्_String, int32_t *सूचकांक_List,
                                     UError_संहिता &स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम 0;
    }
    int32_t सूचकांक = 0;
    int32_t length = अन्तर्_String.गणनम्_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(सूचकांक_List, 0xff, (length + 4) * परिमाण(int32_t));
    यदि (length > 0) {
        सूचकांक_List[2] = 0;
        सूचकांक = U16_दीर्घता(अन्तर्_String.char32At(0));
        यदि (length > 1) {
            सूचकांक_List[3] = सूचकांक;
            सूचकांक += U16_दीर्घता(अन्तर्_String.char32At(सूचकांक));
            यदि (length > 2) {
                सूचकांक_List[4] = सूचकांक;
                सूचकांक += U16_दीर्घता(अन्तर्_String.char32At(सूचकांक));
                यदि (length > 3) {
                    सूचकांक_List[5] = सूचकांक;
                    सूचकांक += U16_दीर्घता(अन्तर्_String.char32At(सूचकांक));
                }
            }
        }
    }
    प्रत्यागम सूचकांक;
}

शून्य MlBreakEngine::आरोपय_MLModel(UError_संहिता &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    यदि (U_FAILURE(error)) प्रत्यागम;

    UnicodeString कुञ्चिका;
    राशिः_UResource_Bundle राशिः_अस्थायी_Bundle;
    Resource_दत्त_मानम् model_कुञ्चिका;

    LocalUResourceBundlePointer rbp(ures_open_प्रत्यक्षम्(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    यदि (U_FAILURE(error)) प्रत्यागम;

    int32_t सूचकांक = 0;
    init_कुञ्चिका_मानम्(rb, "UW1Keys", "UW1Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "UW2Keys", "UW2Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "UW3Keys", "UW3Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "UW4Keys", "UW4Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "UW5Keys", "UW5Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "UW6Keys", "UW6Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "BW1Keys", "BW1Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "BW2Keys", "BW2Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "BW3Keys", "BW3Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "TW1Keys", "TW1Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "TW2Keys", "TW2Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "TW3Keys", "TW3Values", fModel[सूचकांक++], error);
    init_कुञ्चिका_मानम्(rb, "TW4Keys", "TW4Values", fModel[सूचकांक++], error);
    f_Negative_योग /= 2;
}

शून्य MlBreakEngine::init_कुञ्चिका_मानम्(UResourceBundle *rb, ध्रुव वर्ण *कुञ्चिका_Name, ध्रुव वर्ण *मानम्_Name,
                                 Hashtable &model, UError_संहिता &error) {
    int32_t कुञ्चिका_आकार = 0;
    int32_t मानम्_आकार = 0;
    int32_t string_दीर्घता = 0;
    UnicodeString कुञ्चिका;
    राशिः_UResource_Bundle राशिः_अस्थायी_Bundle;
    Resource_दत्त_मानम् model_कुञ्चिका;

    // get modelValues
    LocalUResourceBundlePointer model_मानम्(ures_get_By_कुञ्चिका(rb, मानम्_Name, शून्यसूचकः, &error));
    ध्रुव int32_t *value = ures_get_Int_सदिशम्(model_मानम्.getAlias(), &मानम्_आकार, &error);
    यदि (U_FAILURE(error)) प्रत्यागम;

    // get modelKeys
    ures_get_मानम्_With_Fallback(rb, कुञ्चिका_Name, राशिः_अस्थायी_Bundle.getAlias(), model_कुञ्चिका, error);
    ResourceArray stringArray = model_कुञ्चिका.getArray(error);
    कुञ्चिका_आकार = stringArray.get_आकार();
    यदि (U_FAILURE(error)) प्रत्यागम;

    प्रति (int32_t idx = 0; idx < कुञ्चिका_आकार; idx++) {
        stringArray.get_मानम्(idx, model_कुञ्चिका);
        कुञ्चिका = UnicodeString(model_कुञ्चिका.getString(string_दीर्घता, error));
        यदि (U_SUCCESS(error)) {
            U_ASSERT(idx < मानम्_आकार);
            f_Negative_योग -= value[idx];
            model.puti(कुञ्चिका, value[idx], error);
        }
    }
}

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */
