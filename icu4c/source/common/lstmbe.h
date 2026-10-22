// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef LSTMBE_H
#define LSTMBE_H

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

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
    LSTMBreakEngine(अचर LSTMData* data, अचर UnicodeSet& set, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
    आभास ~LSTMBreakEngine();

    आभास अचर char16_t* name() अचर;

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
                                             UErrorCode& status) अचर override;
गुप्त:
    अचर LSTMData* fData;
    अचर Vectorizer* fVectorizer;
};

U_CAPI अचर LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScriptCode script, अचर LSTMData* data, UErrorCode& status);

U_CAPI अचर LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UErrorCode& status);

U_CAPI अचर LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScriptCode script, UErrorCode& status);

U_CAPI शून्य U_EXPORT2 DeleteLSTMData(अचर LSTMData* data);
U_CAPI अचर char16_t* U_EXPORT2 LSTMDataName(अचर LSTMData* data);

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif  /* LSTMBE_H */
