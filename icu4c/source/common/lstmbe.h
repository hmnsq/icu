// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#اگر_غیر_تعریف_شدہ LSTMBE_H
#تعریف_کرو LSTMBE_H

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/uniset.h"
#شامل "unicode/ures.h"
#شامل "unicode/utext.h"
#شامل "unicode/utypes.h"

#شامل "brkeng.h"
#شامل "dictbe.h"
#شامل "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
ساخت LSTMData;

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
class LSTMBreakEngine : عوامی DictionaryBreakEngine {
عوامی:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(ثابت LSTMData* ڈیٹا, ثابت Unicode_سیٹ& سیٹ, UError_رمز &حالت_2);

    /**
     * <p>Virtual destructor.</p>
     */
    مجازی ~LSTMBreakEngine();

    مجازی ثابت char16_t* name() ثابت;

محفوظ:
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
     مجازی int32_t divide_اوپر_Dictionary_Range(UText *متن,
                                             int32_t range_آغاز,
                                             int32_t range_انتہا,
                                             UVector32 &ملا_Breaks,
                                             UBool isPhraseBreaking,
                                             UError_رمز& حالت_2) ثابت override;
نجی:
    ثابت LSTMData* f_ڈیٹا;
    ثابت Vectorizer* fVectorizer;
};

U_CAPI ثابت LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScript_رمز script, ثابت LSTMData* ڈیٹا, UError_رمز& حالت_2);

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UError_رمز& حالت_2);

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScript_رمز script, UError_رمز& حالت_2);

U_CAPI خالی U_EXPORT2 DeleteLSTMData(ثابت LSTMData* ڈیٹا);
U_CAPI ثابت char16_t* U_EXPORT2 LSTMDataName(ثابت LSTMData* ڈیٹا);

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */

#اگر_ختم  /* LSTMBE_H */
