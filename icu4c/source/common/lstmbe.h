// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef LSTMBE_H
#define LSTMBE_H

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/utext.h"
#अन्तर्भाव "unicode/utypes.h"

#अन्तर्भाव "brkeng.h"
#अन्तर्भाव "dictbe.h"
#अन्तर्भाव "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
संरचना LSTMData;

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
class LSTMBreakEngine : सार्वजनिक DictionaryBreakEngine {
सार्वजनिक:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(स्थिरांक LSTMData* data, स्थिरांक UnicodeSet& set, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
    आभासी ~LSTMBreakEngine();

    आभासी स्थिरांक char16_t* name() स्थिरांक;

संरक्षित:
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
     आभासी int32_t divideUpDictionaryRange(UText *text,
                                             int32_t rangeStart,
                                             int32_t rangeEnd,
                                             UVector32 &foundBreaks,
                                             UBool isPhraseBreaking,
                                             UErrorCode& status) स्थिरांक override;
खाजगी:
    स्थिरांक LSTMData* fData;
    स्थिरांक Vectorizer* fVectorizer;
};

U_CAPI स्थिरांक LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScriptCode script, स्थिरांक LSTMData* data, UErrorCode& status);

U_CAPI स्थिरांक LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UErrorCode& status);

U_CAPI स्थिरांक LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScriptCode script, UErrorCode& status);

U_CAPI शून्य U_EXPORT2 DeleteLSTMData(स्थिरांक LSTMData* data);
U_CAPI स्थिरांक char16_t* U_EXPORT2 LSTMDataName(स्थिरांक LSTMData* data);

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif  /* LSTMBE_H */
