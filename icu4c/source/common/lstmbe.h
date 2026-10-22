// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#جيڪڏهن_وصف_نه_ٿيل LSTMBE_H
#وصف_ڪريو LSTMBE_H

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
    LSTMBreakEngine(ثابت LSTMData* معلومات, ثابت Unicode_سيٽ& سيٽ, UError_ڪوڊ &حالت);

    /**
     * <p>Virtual destructor.</p>
     */
    مجازي ~LSTMBreakEngine();

    مجازي ثابت char16_t* name() ثابت;

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
     مجازي int32_t divide_مٿي_Dictionary_Range(UText *متن,
                                             int32_t range_شروعات,
                                             int32_t range_پڇاڙي,
                                             UVector32 &مليو_Breaks,
                                             UBool isPhraseBreaking,
                                             UError_ڪوڊ& حالت) ثابت override;
خاص:
    ثابت LSTMData* f_معلومات;
    ثابت Vectorizer* fVectorizer;
};

U_CAPI ثابت LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScript_ڪوڊ script, ثابت LSTMData* معلومات, UError_ڪوڊ& حالت);

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UError_ڪوڊ& حالت);

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScript_ڪوڊ script, UError_ڪوڊ& حالت);

U_CAPI فراغ U_EXPORT2 DeleteLSTMData(ثابت LSTMData* معلومات);
U_CAPI ثابت char16_t* U_EXPORT2 LSTMDataName(ثابت LSTMData* معلومات);

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */

#جيڪڏهن_پورو  /* LSTMBE_H */
