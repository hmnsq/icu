// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef LSTMBE_H
#define LSTMBE_H

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "unicode/utext.h"
#અન્તર્ભાવ "unicode/utypes.h"

#અન્તર્ભાવ "brkeng.h"
#અન્તર્ભાવ "dictbe.h"
#અન્તર્ભાવ "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
સંરચના LSTMData;

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
class LSTMBreakEngine : જાહેર DictionaryBreakEngine {
જાહેર:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(અચલ LSTMData* data, અચલ UnicodeSet& set, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
    આભાસી ~LSTMBreakEngine();

    આભાસી અચલ char16_t* name() અચલ;

સુરક્ષિત:
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
     આભાસી int32_t divideUpDictionaryRange(UText *text,
                                             int32_t rangeStart,
                                             int32_t rangeEnd,
                                             UVector32 &foundBreaks,
                                             UBool isPhraseBreaking,
                                             UErrorCode& status) અચલ override;
ખાનગી:
    અચલ LSTMData* fData;
    અચલ Vectorizer* fVectorizer;
};

U_CAPI અચલ LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScriptCode script, અચલ LSTMData* data, UErrorCode& status);

U_CAPI અચલ LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UErrorCode& status);

U_CAPI અચલ LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScriptCode script, UErrorCode& status);

U_CAPI શૂન્ય U_EXPORT2 DeleteLSTMData(અચલ LSTMData* data);
U_CAPI અચલ char16_t* U_EXPORT2 LSTMDataName(અચલ LSTMData* data);

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif  /* LSTMBE_H */
