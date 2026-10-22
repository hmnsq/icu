// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef LSTMBE_H
#define LSTMBE_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/utext.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಅನ್ತರ್ಭಾವ "brkeng.h"
#ಅನ್ತರ್ಭಾವ "dictbe.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
ಸಂರಚನಾ LSTMData;

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
class LSTMBreakEngine : ಸಾರ್ವಜನಿಕ DictionaryBreakEngine {
ಸಾರ್ವಜನಿಕ:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(ಧ್ರುವ LSTMData* data, ಧ್ರುವ UnicodeSet& set, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
    ಆಭಾಸ ~LSTMBreakEngine();

    ಆಭಾಸ ಧ್ರುವ char16_t* name() ಧ್ರುವ;

ರಕ್ಷಿತ:
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
     ಆಭಾಸ int32_t divideUpDictionaryRange(UText *text,
                                             int32_t rangeStart,
                                             int32_t rangeEnd,
                                             UVector32 &foundBreaks,
                                             UBool isPhraseBreaking,
                                             UErrorCode& status) ಧ್ರುವ override;
ಗುಪ್ತ:
    ಧ್ರುವ LSTMData* fData;
    ಧ್ರುವ Vectorizer* fVectorizer;
};

U_CAPI ಧ್ರುವ LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScriptCode script, ಧ್ರುವ LSTMData* data, UErrorCode& status);

U_CAPI ಧ್ರುವ LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UErrorCode& status);

U_CAPI ಧ್ರುವ LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScriptCode script, UErrorCode& status);

U_CAPI ಶೂನ್ಯ U_EXPORT2 DeleteLSTMData(ಧ್ರುವ LSTMData* data);
U_CAPI ಧ್ರುವ char16_t* U_EXPORT2 LSTMDataName(ಧ್ರುವ LSTMData* data);

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif  /* LSTMBE_H */
