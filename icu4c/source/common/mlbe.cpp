// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "mlbe.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ubrkimpl.h"
#అన్తర్భావ "unicode/resbund.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "util.h"
#అన్తర్భావ "uvectr32.h"

U_NAMESPACE_BEGIN

గణన తరగతి Model_సూచీ { kUWStart = 0, kBWStart = 6, kTWStart = 9 };

MlBreakEngine::MlBreakEngine(స్థిరరాశి Unicode_సమితి &digit_Or_Open_Punctuation_Or_Alphabet_సమితి,
                             స్థిరరాశి Unicode_సమితి &close_Punctuation_సమితి, UError_సంకేతం &స్థితి_2)
    : f_Digit_Or_Open_Punctuation_Or_Alphabet_సమితి(digit_Or_Open_Punctuation_Or_Alphabet_సమితి),
      f_Close_Punctuation_సమితి(close_Punctuation_సమితి),
      f_Negative_కూడిక(0) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    ఎక్కించు_MLModel(స్థితి_2);
}

MlBreakEngine::~MlBreakEngine() {}

int32_t MlBreakEngine::divide_పైకి_Range(UText *లోపల_పాఠ్యం, int32_t range_ప్రారంభం, int32_t range_చివర,
                                     UVector32 &దొరికింది_Breaks, స్థిరరాశి UnicodeString &లోపల_String,
                                     స్థిరరాశి Local_సూచిక<UVector32> &ఆదానం_Map,
                                     UError_సంకేతం &స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి 0;
    }
    ఒకవేళ (range_ప్రారంభం >= range_చివర) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    UVector32 boundary(లోపల_String.లెక్క_Char32() + 1, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి 0;
    }
    int32_t అంకె_Breaks = 0;
    int32_t సంకేతం_బిందువు_నిడివి = లోపల_String.లెక్క_Char32();
    // The ML algorithm groups six char and evaluates whether the 4th char is a breakpoint.
    // In each iteration, it evaluates the 4th char and then moves forward one char like a sliding
    // window. Initially, the first six values in the indexList are [-1, -1, 0, 1, 2, 3]. After
    // moving forward, finally the last six values in the indexList are
    // [length-4, length-3, length-2, length-1, -1, -1]. The "+4" here means four extra "-1".
    int32_t సూచీ_కొలత = సంకేతం_బిందువు_నిడివి + 4;
    Local_జ్ఞాపకం<int32_t> సూచీ_List(static_cast<int32_t*>(uprv_malloc(సూచీ_కొలత * పరిమాణం(int32_t))));
    ఒకవేళ (సూచీ_List.isNull()) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి 0;
    }
    int32_t అంకె_సంకేతం_Units = init_సూచీ_List(లోపల_String, సూచీ_List.getAlias(), స్థితి_2);

    // Add a break for the start.
    boundary.కూడు_Element(0, స్థితి_2);
    అంకె_Breaks++;
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;

    కొరకు (int32_t idx = 0; idx + 1 < సంకేతం_బిందువు_నిడివి && U_SUCCESS(స్థితి_2); idx++) {
        అంకె_Breaks =
            evaluateBreakpoint(లోపల_String, సూచీ_List.getAlias(), idx, అంకె_సంకేతం_Units, అంకె_Breaks, boundary, స్థితి_2);
        ఒకవేళ (idx + 4 < సంకేతం_బిందువు_నిడివి) {
            సూచీ_List[idx + 6] = అంకె_సంకేతం_Units;
            అంకె_సంకేతం_Units += U16_నిడివి(లోపల_String.char32At(సూచీ_List[idx + 6]));
        }
    }

    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;

    // Add a break for the end if there is not one there already.
    ఒకవేళ (boundary.చివరి_Elementi() != లోపల_String.లెక్క_Char32()) {
        boundary.కూడు_Element(లోపల_String.లెక్క_Char32(), స్థితి_2);
        అంకె_Breaks++;
    }

    int32_t prevCPPos = -1;
    int32_t prevUTextPos = -1;
    int32_t corrected_అంకె_Breaks = 0;
    కొరకు (int32_t i = 0; i < అంకె_Breaks; i++) {
        int32_t cpPos = boundary.elementAti(i);
        int32_t utextPos = ఆదానం_Map.isValid() ? ఆదానం_Map->elementAti(cpPos) : cpPos + range_ప్రారంభం;
        U_ASSERT(cpPos > prevCPPos);
        U_ASSERT(utextPos >= prevUTextPos);

        ఒకవేళ (utextPos > prevUTextPos) {
            ఒకవేళ (utextPos != range_ప్రారంభం ||
                (utextPos > 0 &&
                 f_Close_Punctuation_సమితి.కలిగియుంది(utext_char32At(లోపల_పాఠ్యం, utextPos - 1)))) {
                దొరికింది_Breaks.push(utextPos, స్థితి_2);
                corrected_అంకె_Breaks++;
            }
        } లేకపోతే {
            // Normalization expanded the input text, the dictionary found a boundary
            // within the expansion, giving two boundaries with the same index in the
            // original text. Ignore the second. See ticket #12918.
            --అంకె_Breaks;
        }
        prevCPPos = cpPos;
        prevUTextPos = utextPos;
    }
    (శూన్య)prevCPPos;  // suppress compiler warnings about unused variable

    UChar32 తదుపరి_Char = utext_char32At(లోపల_పాఠ్యం, range_చివర);
    ఒకవేళ (!దొరికింది_Breaks.is_ఖాళీలు() && దొరికింది_Breaks.peeki() == range_చివర) {
        // In phrase breaking, there has to be a breakpoint between Cj character and
        // the number/open punctuation.
        // E.g. る文字「そうだ、京都」->る▁文字▁「そうだ、▁京都」-> breakpoint between 字 and「
        // E.g. 乗車率９０％程度だろうか -> 乗車▁率▁９０％▁程度だろうか -> breakpoint between 率 and ９
        // E.g. しかもロゴがＵｎｉｃｏｄｅ！ -> しかも▁ロゴが▁Ｕｎｉｃｏｄｅ！-> breakpoint between が and Ｕ
        ఒకవేళ (!f_Digit_Or_Open_Punctuation_Or_Alphabet_సమితి.కలిగియుంది(తదుపరి_Char)) {
            దొరికింది_Breaks.popi();
            corrected_అంకె_Breaks--;
        }
    }

    తిరిగి corrected_అంకె_Breaks;
}

int32_t MlBreakEngine::evaluateBreakpoint(స్థిరరాశి UnicodeString &లోపల_String, int32_t *సూచీ_List,
                                          int32_t ప్రారంభం_Idx, int32_t అంకె_సంకేతం_Units, int32_t అంకె_Breaks,
                                          UVector32 &boundary, UError_సంకేతం &స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి అంకె_Breaks;
    }
    int32_t ప్రారంభం = 0, చివర = 0;
    int32_t score = f_Negative_కూడిక;

    కొరకు (పూర్ణాంకం i = 0; i < 6; i++) {
        // UW1 ~ UW6
        ప్రారంభం = ప్రారంభం_Idx + i;
        ఒకవేళ (సూచీ_List[ప్రారంభం] != -1) {
            చివర = (సూచీ_List[ప్రారంభం + 1] != -1) ? సూచీ_List[ప్రారంభం + 1] : అంకె_సంకేతం_Units;
            score += fModel[static_cast<int32_t>(Model_సూచీ::kUWStart) + i].geti(
                లోపల_String.తాత్కాలిక_Sub_String(సూచీ_List[ప్రారంభం], చివర - సూచీ_List[ప్రారంభం]));
        }
    }
    కొరకు (పూర్ణాంకం i = 0; i < 3; i++) {
        // BW1 ~ BW3
        ప్రారంభం = ప్రారంభం_Idx + i + 1;
        ఒకవేళ (సూచీ_List[ప్రారంభం] != -1 && సూచీ_List[ప్రారంభం + 1] != -1) {
            చివర = (సూచీ_List[ప్రారంభం + 2] != -1) ? సూచీ_List[ప్రారంభం + 2] : అంకె_సంకేతం_Units;
            score += fModel[static_cast<int32_t>(Model_సూచీ::kBWStart) + i].geti(
                లోపల_String.తాత్కాలిక_Sub_String(సూచీ_List[ప్రారంభం], చివర - సూచీ_List[ప్రారంభం]));
        }
    }
    కొరకు (పూర్ణాంకం i = 0; i < 4; i++) {
        // TW1 ~ TW4
        ప్రారంభం = ప్రారంభం_Idx + i;
        ఒకవేళ (సూచీ_List[ప్రారంభం] != -1 && సూచీ_List[ప్రారంభం + 1] != -1 && సూచీ_List[ప్రారంభం + 2] != -1) {
            చివర = (సూచీ_List[ప్రారంభం + 3] != -1) ? సూచీ_List[ప్రారంభం + 3] : అంకె_సంకేతం_Units;
            score += fModel[static_cast<int32_t>(Model_సూచీ::kTWStart) + i].geti(
                లోపల_String.తాత్కాలిక_Sub_String(సూచీ_List[ప్రారంభం], చివర - సూచీ_List[ప్రారంభం]));
        }
    }

    ఒకవేళ (score > 0) {
        boundary.కూడు_Element(ప్రారంభం_Idx + 1, స్థితి_2);
        అంకె_Breaks++;
    }
    తిరిగి అంకె_Breaks;
}

int32_t MlBreakEngine::init_సూచీ_List(స్థిరరాశి UnicodeString &లోపల_String, int32_t *సూచీ_List,
                                     UError_సంకేతం &స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి 0;
    }
    int32_t సూచీ = 0;
    int32_t length = లోపల_String.లెక్క_Char32();
    // Set all (lenght+4) items inside indexLength to -1 presuming -1 is 4 bytes of 0xff.
    uprv_memset(సూచీ_List, 0xff, (length + 4) * పరిమాణం(int32_t));
    ఒకవేళ (length > 0) {
        సూచీ_List[2] = 0;
        సూచీ = U16_నిడివి(లోపల_String.char32At(0));
        ఒకవేళ (length > 1) {
            సూచీ_List[3] = సూచీ;
            సూచీ += U16_నిడివి(లోపల_String.char32At(సూచీ));
            ఒకవేళ (length > 2) {
                సూచీ_List[4] = సూచీ;
                సూచీ += U16_నిడివి(లోపల_String.char32At(సూచీ));
                ఒకవేళ (length > 3) {
                    సూచీ_List[5] = సూచీ;
                    సూచీ += U16_నిడివి(లోపల_String.char32At(సూచీ));
                }
            }
        }
    }
    తిరిగి సూచీ;
}

శూన్య MlBreakEngine::ఎక్కించు_MLModel(UError_సంకేతం &error) {
    // BudouX's model consists of thirteen categories, each of which is make up of pairs of the
    // feature and its score. As integrating it into jaml.txt, we define thirteen kinds of key and
    // value to represent the feature and the corresponding score respectively.

    ఒకవేళ (U_FAILURE(error)) తిరిగి;

    UnicodeString కీలకం;
    దొంతర_UResource_Bundle దొంతర_తాత్కాలిక_Bundle;
    Resource_దత్తాంశం_విలువ model_కీలకం;

    Local_UResource_Bundle_సూచిక rbp(ures_open_ప్రత్యక్ష(U_ICUDATA_BRKITR, "jaml", &error));
    UResourceBundle *rb = rbp.getAlias();
    ఒకవేళ (U_FAILURE(error)) తిరిగి;

    int32_t సూచీ = 0;
    init_కీలకం_విలువ(rb, "UW1Keys", "UW1Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "UW2Keys", "UW2Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "UW3Keys", "UW3Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "UW4Keys", "UW4Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "UW5Keys", "UW5Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "UW6Keys", "UW6Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "BW1Keys", "BW1Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "BW2Keys", "BW2Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "BW3Keys", "BW3Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "TW1Keys", "TW1Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "TW2Keys", "TW2Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "TW3Keys", "TW3Values", fModel[సూచీ++], error);
    init_కీలకం_విలువ(rb, "TW4Keys", "TW4Values", fModel[సూచీ++], error);
    f_Negative_కూడిక /= 2;
}

శూన్య MlBreakEngine::init_కీలకం_విలువ(UResourceBundle *rb, స్థిరరాశి అక్షరం *కీలకం_Name, స్థిరరాశి అక్షరం *విలువ_Name,
                                 Hashtable &model, UError_సంకేతం &error) {
    int32_t కీలకం_కొలత = 0;
    int32_t విలువ_కొలత = 0;
    int32_t string_నిడివి = 0;
    UnicodeString కీలకం;
    దొంతర_UResource_Bundle దొంతర_తాత్కాలిక_Bundle;
    Resource_దత్తాంశం_విలువ model_కీలకం;

    // get modelValues
    Local_UResource_Bundle_సూచిక model_విలువ(ures_get_By_కీలకం(rb, విలువ_Name, శూన్యసూచిక, &error));
    స్థిరరాశి int32_t *value = ures_get_Int_సదిశ(model_విలువ.getAlias(), &విలువ_కొలత, &error);
    ఒకవేళ (U_FAILURE(error)) తిరిగి;

    // get modelKeys
    ures_get_విలువ_With_Fallback(rb, కీలకం_Name, దొంతర_తాత్కాలిక_Bundle.getAlias(), model_కీలకం, error);
    ResourceArray stringArray = model_కీలకం.getArray(error);
    కీలకం_కొలత = stringArray.get_కొలత();
    ఒకవేళ (U_FAILURE(error)) తిరిగి;

    కొరకు (int32_t idx = 0; idx < కీలకం_కొలత; idx++) {
        stringArray.get_విలువ(idx, model_కీలకం);
        కీలకం = UnicodeString(model_కీలకం.getString(string_నిడివి, error));
        ఒకవేళ (U_SUCCESS(error)) {
            U_ASSERT(idx < విలువ_కొలత);
            f_Negative_కూడిక -= value[idx];
            model.puti(కీలకం, value[idx], error);
        }
    }
}

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */
