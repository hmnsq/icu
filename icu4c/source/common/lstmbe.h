// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef LSTMBE_H
#define LSTMBE_H

#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_BREAK_ITERATION

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
    LSTMBreakEngine(ध्रुव LSTMData* data, ध्रुव UnicodeSet& set, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
    आभास ~LSTMBreakEngine();

    आभास ध्रुव char16_t* name() ध्रुव;

रक्षित:
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
     आभास int32_t divideUpDictionaryRange(UText *text,
                                             int32_t rangeStart,
                                             int32_t rangeEnd,
                                             UVector32 &foundBreaks,
                                             UBool isPhraseBreaking,
                                             UErrorCode& status) ध्रुव override;
गुप्त:
    ध्रुव LSTMData* fData;
    ध्रुव Vectorizer* fVectorizer;
};

U_CAPI ध्रुव LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScriptCode script, ध्रुव LSTMData* data, UErrorCode& status);

U_CAPI ध्रुव LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UErrorCode& status);

U_CAPI ध्रुव LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScriptCode script, UErrorCode& status);

U_CAPI शून्य U_EXPORT2 DeleteLSTMData(ध्रुव LSTMData* data);
U_CAPI ध्रुव char16_t* U_EXPORT2 LSTMDataName(ध्रुव LSTMData* data);

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif  /* LSTMBE_H */
