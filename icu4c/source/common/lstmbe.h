// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LSTMBE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LSTMBE_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "brkeng.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dictbe.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
ਬਣਤਰ LSTMData;

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
class LSTMBreakEngine : ਜਨਤਕ DictionaryBreakEngine {
ਜਨਤਕ:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(ਅਚਲ LSTMData* ਅੰਕੜੇ, ਅਚਲ Unicode_ਸੈੱਟ& ਸੈੱਟ, UError_ਕੋਡ &ਸਥਿਤੀ);

    /**
     * <p>Virtual destructor.</p>
     */
    ਆਭਾਸੀ ~LSTMBreakEngine();

    ਆਭਾਸੀ ਅਚਲ char16_t* name() ਅਚਲ;

ਸੁਰੱਖਿਅਤ:
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
     ਆਭਾਸੀ int32_t divide_ਉੱਪਰ_Dictionary_Range(UText *ਪਾਠ,
                                             int32_t range_ਸ਼ੁਰੂ,
                                             int32_t range_ਸਿਰਾ,
                                             UVector32 &ਮਿਲਿਆ_Breaks,
                                             UBool isPhraseBreaking,
                                             UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;
ਨਿਜੀ:
    ਅਚਲ LSTMData* f_ਅੰਕੜੇ;
    ਅਚਲ Vectorizer* fVectorizer;
};

U_CAPI ਅਚਲ LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScript_ਕੋਡ script, ਅਚਲ LSTMData* ਅੰਕੜੇ, UError_ਕੋਡ& ਸਥਿਤੀ);

U_CAPI ਅਚਲ LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UError_ਕੋਡ& ਸਥਿਤੀ);

U_CAPI ਅਚਲ LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScript_ਕੋਡ script, UError_ਕੋਡ& ਸਥਿਤੀ);

U_CAPI ਖਾਲੀ U_EXPORT2 DeleteLSTMData(ਅਚਲ LSTMData* ਅੰਕੜੇ);
U_CAPI ਅਚਲ char16_t* U_EXPORT2 LSTMDataName(ਅਚਲ LSTMData* ਅੰਕੜੇ);

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */

#ਜੇ_ਅੰਤ  /* LSTMBE_H */
