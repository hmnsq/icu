// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef LSTMBE_H
#define LSTMBE_H

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাৱ "unicode/uniset.h"
#অন্তর্ভাৱ "unicode/ures.h"
#অন্তর্ভাৱ "unicode/utext.h"
#অন্তর্ভাৱ "unicode/utypes.h"

#অন্তর্ভাৱ "brkeng.h"
#অন্তর্ভাৱ "dictbe.h"
#অন্তর্ভাৱ "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
সংরচনা LSTMData;

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
class LSTMBreakEngine : সার্ৱজনিক DictionaryBreakEngine {
সার্ৱজনিক:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(ধ্রুৱ LSTMData* data, ধ্রুৱ UnicodeSet& set, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
    আভাস ~LSTMBreakEngine();

    আভাস ধ্রুৱ char16_t* name() ধ্রুৱ;

রক্ষিত:
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
     আভাস int32_t divideUpDictionaryRange(UText *text,
                                             int32_t rangeStart,
                                             int32_t rangeEnd,
                                             UVector32 &foundBreaks,
                                             UBool isPhraseBreaking,
                                             UErrorCode& status) ধ্রুৱ override;
গুপ্ত:
    ধ্রুৱ LSTMData* fData;
    ধ্রুৱ Vectorizer* fVectorizer;
};

U_CAPI ধ্রুৱ LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScriptCode script, ধ্রুৱ LSTMData* data, UErrorCode& status);

U_CAPI ধ্রুৱ LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UErrorCode& status);

U_CAPI ধ্রুৱ LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScriptCode script, UErrorCode& status);

U_CAPI শূন্য U_EXPORT2 DeleteLSTMData(ধ্রুৱ LSTMData* data);
U_CAPI ধ্রুৱ char16_t* U_EXPORT2 LSTMDataName(ধ্রুৱ LSTMData* data);

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif  /* LSTMBE_H */
