// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LSTMBE_H
#ವ್ಯಾಖ್ಯಾನಿಸು LSTMBE_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/utext.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಅನ್ತರ್ಭಾವ "brkeng.h"
#ಅನ್ತರ್ಭಾವ "dictbe.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
ರಚನೆ LSTMData;

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
    LSTMBreakEngine(ಸ್ಥಿರಾಂಕ LSTMData* ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& ಗಣ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    /**
     * <p>Virtual destructor.</p>
     */
    ವಾಸ್ತವಿಕ ~LSTMBreakEngine();

    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ char16_t* name() ಸ್ಥಿರಾಂಕ;

ಸಂರಕ್ಷಿತ:
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
     ವಾಸ್ತವಿಕ int32_t divide_ಮೇಲೆ_Dictionary_Range(UText *ಪಠ್ಯ,
                                             int32_t range_ಆರಂಭ,
                                             int32_t range_ಕೊನೆ,
                                             UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                             UBool isPhraseBreaking,
                                             UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;
ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ LSTMData* f_ದತ್ತಾಂಶ;
    ಸ್ಥಿರಾಂಕ Vectorizer* fVectorizer;
};

U_CAPI ಸ್ಥಿರಾಂಕ LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScript_ಸಂಕೇತ script, ಸ್ಥಿರಾಂಕ LSTMData* ದತ್ತಾಂಶ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_CAPI ಸ್ಥಿರಾಂಕ LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_CAPI ಸ್ಥಿರಾಂಕ LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScript_ಸಂಕೇತ script, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

U_CAPI ಶೂನ್ಯ U_EXPORT2 DeleteLSTMData(ಸ್ಥಿರಾಂಕ LSTMData* ದತ್ತಾಂಶ);
U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2 LSTMDataName(ಸ್ಥಿರಾಂಕ LSTMData* ದತ್ತಾಂಶ);

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */

#ಆದರೆ_ಅಂತ್ಯ  /* LSTMBE_H */
