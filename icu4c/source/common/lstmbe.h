// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अगर_अघोषित LSTMBE_H
#घोषणा LSTMBE_H

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/uniset.h"
#समावेश "unicode/ures.h"
#समावेश "unicode/utext.h"
#समावेश "unicode/utypes.h"

#समावेश "brkeng.h"
#समावेश "dictbe.h"
#समावेश "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
काष्ठा LSTMData;

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
class LSTMBreakEngine : खुला DictionaryBreakEngine {
खुला:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(स्थिर LSTMData* तथ्य, स्थिर UnicodeSet& set, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
    भव ~LSTMBreakEngine();

    भव स्थिर char16_t* name() स्थिर;

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
     भव int32_t divideUpDictionaryRange(UText *text,
                                             int32_t rangeStart,
                                             int32_t rangeEnd,
                                             UVector32 &foundBreaks,
                                             UBool isPhraseBreaking,
                                             UErrorCode& status) स्थिर override;
निजी:
    स्थिर LSTMData* f_तथ्य;
    स्थिर Vectorizer* fVectorizer;
};

U_CAPI स्थिर LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScriptCode script, स्थिर LSTMData* तथ्य, UErrorCode& status);

U_CAPI स्थिर LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UErrorCode& status);

U_CAPI स्थिर LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScriptCode script, UErrorCode& status);

U_CAPI व्योम U_EXPORT2 DeleteLSTMData(स्थिर LSTMData* तथ्य);
U_CAPI स्थिर char16_t* U_EXPORT2 LSTMDataName(स्थिर LSTMData* तथ्य);

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */

#पूर्ण_अगर  /* LSTMBE_H */
