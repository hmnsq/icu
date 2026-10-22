// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef LSTMBE_H
#define LSTMBE_H

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "unicode/utext.h"
#অন্তর্ভাব "unicode/utypes.h"

#অন্তর্ভাব "brkeng.h"
#অন্তর্ভাব "dictbe.h"
#অন্তর্ভাব "uvectr32.h"

U_NAMESPACE_BEGIN

class Vectorizer;
কাঠামো LSTMData;

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
class LSTMBreakEngine : সর্বজনীন DictionaryBreakEngine {
সর্বজনীন:
    /**
     * <p>Constructor.</p>
     */
    LSTMBreakEngine(ধ্রুবক LSTMData* তথ্য, ধ্রুবক UnicodeSet& set, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
    আভাসী ~LSTMBreakEngine();

    আভাসী ধ্রুবক char16_t* name() ধ্রুবক;

সুরক্ষিত:
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
     আভাসী int32_t divideUpDictionaryRange(UText *text,
                                             int32_t rangeStart,
                                             int32_t rangeEnd,
                                             UVector32 &foundBreaks,
                                             UBool isPhraseBreaking,
                                             UErrorCode& status) ধ্রুবক override;
ব্যক্তিগত:
    ধ্রুবক LSTMData* f_তথ্য;
    ধ্রুবক Vectorizer* fVectorizer;
};

U_CAPI ধ্রুবক LanguageBreakEngine* U_EXPORT2 CreateLSTMBreakEngine(
    UScriptCode script, ধ্রুবক LSTMData* তথ্য, UErrorCode& status);

U_CAPI ধ্রুবক LSTMData* U_EXPORT2 CreateLSTMData(
    UResourceBundle* rb, UErrorCode& status);

U_CAPI ধ্রুবক LSTMData* U_EXPORT2 CreateLSTMDataForScript(
    UScriptCode script, UErrorCode& status);

U_CAPI শূন্য U_EXPORT2 DeleteLSTMData(ধ্রুবক LSTMData* তথ্য);
U_CAPI ধ্রুবক char16_t* U_EXPORT2 LSTMDataName(ধ্রুবক LSTMData* তথ্য);

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif  /* LSTMBE_H */
