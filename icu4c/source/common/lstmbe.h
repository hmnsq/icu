// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#నిర్వచితం_కాకపోతే LSTMBE_H
#నిర్వచించు LSTMBE_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/utext.h"
#అన్తర్భావ "unicode/utypes.h"

#అన్తర్భావ "brkeng.h"
#అన్తర్భావ "dictbe.h"
#అన్తర్భావ "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
నిర్మాణం LSTMData;

/*******************************************************************
 * LSTMBreakEngine
 */

/**
 * <p>LSTMBreakEngine is a kind of DictionaryBreakEngine that uses a
 * LSTM to determine language-specific breaks.</p>
 *
 * <p>After it is constructed a LSTMBreakEngine may be shared between
 * threads without synchronization.</p>
 */
class LSTMBreakEngine : బహిరంగ DictionaryBreakEngine {
బహిరంగ:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(స్థిరరాశి LSTMData* దత్తాంశం, స్థిరరాశి Unicode_సమితి& సమితి, UError_సంకేతం &స్థితి_2);

    /**
     * <p>Virtual destructor.</p>
     */
    మాయా ~LSTMBreakEngine();

    మాయా స్థిరరాశి char16_t* name() స్థిరరాశి;

రక్షిత:
    /**
     * <p>Divide up a range of known dictionary characters handled by this break engine.</p>
     *
     * @param text A UText representing the text
     * @param rangeStart The start of the range of dictionary characters
     * @param rangeEnd The end of the range of dictionary characters
     * @param foundBreaks Output of C array of int32_t break positions, or 0
     * @param status Information on any errors encountered.
     * @return The number of breaks found
     */
     మాయా int32_t divide_పైకి_Dictionary_Range(UText *పాఠ్యం,
                                             int32_t range_ప్రారంభం,
                                             int32_t range_చివర,
                                             UVector32 &దొరికింది_Breaks,
                                             UBool isPhraseBreaking,
                                             UError_సంకేతం& స్థితి_2) స్థిరరాశి override;
అంతరంగ:
    స్థిరరాశి LSTMData* f_దత్తాంశం;
    స్థిరరాశి Vectorizer* fVectorizer;
};

U_CAPI స్థిరరాశి LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScript_సంకేతం script, స్థిరరాశి LSTMData* దత్తాంశం, UError_సంకేతం& స్థితి_2);

U_CAPI స్థిరరాశి LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UError_సంకేతం& స్థితి_2);

U_CAPI స్థిరరాశి LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScript_సంకేతం script, UError_సంకేతం& స్థితి_2);

U_CAPI శూన్య U_EXPORT2 DeleteLSTMData(స్థిరరాశి LSTMData* దత్తాంశం);
U_CAPI స్థిరరాశి char16_t* U_EXPORT2 LSTMDataName(స్థిరరాశి LSTMData* దత్తాంశం);

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */

#అయితే_ముగింపు  /* LSTMBE_H */
