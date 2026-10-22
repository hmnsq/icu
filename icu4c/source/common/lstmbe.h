// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef LSTMBE_H
#define LSTMBE_H

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "unicode/utext.h"
#அந்தர்பாவ "unicode/utypes.h"

#அந்தர்பாவ "brkeng.h"
#அந்தர்பாவ "dictbe.h"
#அந்தர்பாவ "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
கட்டமைப்பு LSTMData;

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
class LSTMBreakEngine : பொது DictionaryBreakEngine {
பொது:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(மாறிலி LSTMData* data, மாறிலி UnicodeSet& set, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
    மெய்நிகர் ~LSTMBreakEngine();

    மெய்நிகர் மாறிலி char16_t* name() மாறிலி;

பாதுகாக்கப்பட்ட:
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
     மெய்நிகர் int32_t divideUpDictionaryRange(UText *text,
                                             int32_t rangeStart,
                                             int32_t rangeEnd,
                                             UVector32 &foundBreaks,
                                             UBool isPhraseBreaking,
                                             UErrorCode& status) மாறிலி override;
தனிப்பட்ட:
    மாறிலி LSTMData* fData;
    மாறிலி Vectorizer* fVectorizer;
};

U_CAPI மாறிலி LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScriptCode script, மாறிலி LSTMData* data, UErrorCode& status);

U_CAPI மாறிலி LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UErrorCode& status);

U_CAPI மாறிலி LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScriptCode script, UErrorCode& status);

U_CAPI வெறுமை U_EXPORT2 DeleteLSTMData(மாறிலி LSTMData* data);
U_CAPI மாறிலி char16_t* U_EXPORT2 LSTMDataName(மாறிலி LSTMData* data);

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif  /* LSTMBE_H */
