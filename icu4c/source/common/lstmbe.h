// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef LSTMBE_H
#define LSTMBE_H

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utext.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଅନ୍ତର୍ଭାଵ "brkeng.h"
#ଅନ୍ତର୍ଭାଵ "dictbe.h"
#ଅନ୍ତର୍ଭାଵ "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
ସଂରଚନା LSTMData;

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
class LSTMBreakEngine : ସର୍ବସାଧାରଣ DictionaryBreakEngine {
ସର୍ବସାଧାରଣ:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(ଅଚଳ LSTMData* data, ଅଚଳ UnicodeSet& set, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
    ଆଭାସୀ ~LSTMBreakEngine();

    ଆଭାସୀ ଅଚଳ char16_t* name() ଅଚଳ;

ସୁରକ୍ଷିତ:
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
     ଆଭାସୀ int32_t divideUpDictionaryRange(UText *text,
                                             int32_t rangeStart,
                                             int32_t rangeEnd,
                                             UVector32 &foundBreaks,
                                             UBool isPhraseBreaking,
                                             UErrorCode& status) ଅଚଳ override;
ବ୍ୟକ୍ତିଗତ:
    ଅଚଳ LSTMData* fData;
    ଅଚଳ Vectorizer* fVectorizer;
};

U_CAPI ଅଚଳ LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScriptCode script, ଅଚଳ LSTMData* data, UErrorCode& status);

U_CAPI ଅଚଳ LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UErrorCode& status);

U_CAPI ଅଚଳ LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScriptCode script, UErrorCode& status);

U_CAPI ଶୂନ୍ୟ U_EXPORT2 DeleteLSTMData(ଅଚଳ LSTMData* data);
U_CAPI ଅଚଳ char16_t* U_EXPORT2 LSTMDataName(ଅଚଳ LSTMData* data);

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif  /* LSTMBE_H */
