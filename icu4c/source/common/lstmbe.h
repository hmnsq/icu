// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#اگر_تعریف_نشده LSTMBE_H
#تعریف_کن LSTMBE_H

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/uniset.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/utext.h"
#ضمن "unicode/utypes.h"

#ضمن "brkeng.h"
#ضمن "dictbe.h"
#ضمن "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
بنية LSTMData;

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
class LSTMBreakEngine : عام DictionaryBreakEngine {
عام:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(ثابت LSTMData* داده, ثابت Unicode_مجموعه& مجموعه, UError_کد &وضعیت);

    /**
     * <p>Virtual destructor.</p>
     */
    مجازی ~LSTMBreakEngine();

    مجازی ثابت char16_t* name() ثابت;

محمي:
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
     مجازی int32_t divide_به_بالا_Dictionary_Range(UText *متن,
                                             int32_t range_شروع,
                                             int32_t range_انتها,
                                             UVector32 &یافت_Breaks,
                                             UBool isPhraseBreaking,
                                             UError_کد& وضعیت) ثابت override;
خاص:
    ثابت LSTMData* f_داده;
    ثابت Vectorizer* fVectorizer;
};

U_CAPI ثابت LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScript_کد script, ثابت LSTMData* داده, UError_کد& وضعیت);

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UError_کد& وضعیت);

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScript_کد script, UError_کد& وضعیت);

U_CAPI فراغ U_EXPORT2 DeleteLSTMData(ثابت LSTMData* داده);
U_CAPI ثابت char16_t* U_EXPORT2 LSTMDataName(ثابت LSTMData* داده);

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */

#پایان_اگر  /* LSTMBE_H */
