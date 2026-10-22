// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef LSTMBE_H
#define LSTMBE_H

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
ساختار LSTMData;

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
class LSTMBreakEngine : عمومی DictionaryBreakEngine {
عمومی:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(ثابت LSTMData* data, ثابت UnicodeSet& set, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
    مجازی ~LSTMBreakEngine();

    مجازی ثابت char16_t* name() ثابت;

محافظتشده:
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
     مجازی int32_t divideUpDictionaryRange(UText *text,
                                             int32_t rangeStart,
                                             int32_t rangeEnd,
                                             UVector32 &foundBreaks,
                                             UBool isPhraseBreaking,
                                             UErrorCode& status) ثابت override;
خصوصی:
    ثابت LSTMData* fData;
    ثابت Vectorizer* fVectorizer;
};

U_CAPI ثابت LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScriptCode script, ثابت LSTMData* data, UErrorCode& status);

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UErrorCode& status);

U_CAPI ثابت LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScriptCode script, UErrorCode& status);

U_CAPI تهی U_EXPORT2 DeleteLSTMData(ثابت LSTMData* data);
U_CAPI ثابت char16_t* U_EXPORT2 LSTMDataName(ثابت LSTMData* data);

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif  /* LSTMBE_H */
