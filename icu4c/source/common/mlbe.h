// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef MLBREAKENGINE_H
#define MLBREAKENGINE_H

#شامل "hash.h"
#شامل "unicode/resbund.h"
#شامل "unicode/uniset.h"
#شامل "unicode/utext.h"
#شامل "uvectr32.h"

U_NAMESPACE_BEGIN

#اگر !UCONFIG_NO_BREAK_ITERATION

/**
 * A machine learning break engine for the phrase breaking in Japanese.
 */
class MlBreakEngine : عمومی UMemory {
   عمومی:
    /**
     * Constructor.
     *
     * @param digitOrOpenPunctuationOrAlphabetSet An UnicodeSet with the digit, open punctuation and
     * alphabet.
     * @param closePunctuationSet An UnicodeSet with close punctuation.
     * @param status Information on any errors encountered.
     */
    MlBreakEngine(ثابت UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                  ثابت UnicodeSet &closePunctuationSet, UErrorCode &status);

    /**
     * Virtual destructor.
     */
    مجازی ~MlBreakEngine();

   عمومی:
    /**
     * Divide up a range of characters handled by this break engine.
     *
     * @param inText A UText representing the text
     * @param rangeStart The start of the range of the characters
     * @param rangeEnd The end of the range of the characters
     * @param foundBreaks Output of C array of int32_t break positions, or 0
     * @param inString The normalized string of text ranging from rangeStart to rangeEnd
     * @param inputMap The vector storing the native index of inText
     * @param status Information on any errors encountered.
     * @return The number of breaks found
     */
    int32_t divideUpRange(UText *inText, int32_t rangeStart, int32_t rangeEnd,
                          UVector32 &foundBreaks, ثابت UnicodeString &inString,
                          ثابت LocalPointer<UVector32> &درون_داد_Map, UErrorCode &status) ثابت;

   خصوصی:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    تهی loadMLModel(UErrorCode &error);

    /**
     * In the machine learning's model file, specify the name of the key and value to load the
     * corresponding feature and its score.
     *
     * @param rb A ResouceBundle corresponding to the model file.
     * @param keyName The kay name in the model file.
     * @param valueName The value name in the model file.
     * @param model A hashtable to store the pairs of the feature and its score.
     * @param error Information on any errors encountered.
     */
    تهی init_کلید_مقدار(UResourceBundle *rb, ثابت نویسه *کلید_Name, ثابت نویسه *مقدار_Name,
                      Hashtable &model, UErrorCode &error);

    /**
     * Initialize the index list from the input string.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of inString.
     * @param status Information on any errors encountered.
     * @return The number of code units of the first four characters in inString.
     */
    int32_t initIndexList(ثابت UnicodeString &inString, int32_t *indexList,
                          UErrorCode &status) ثابت;

    /**
     * Evaluate whether the index is a potential breakpoint.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of the inString.
     * @param startIdx The start index of the indexList.
     * @param numCodeUnits  The current code unit boundary of the indexList.
     * @param numBreaks The accumulated number of breakpoints.
     * @param boundary A vector including the index of the breakpoint.
     * @param status Information on any errors encountered.
     * @return The number of breakpoints
     */
    int32_t evaluateBreakpoint(ثابت UnicodeString &inString, int32_t *indexList, int32_t startIdx,
                               int32_t رقم_Code_Units, int32_t رقم_Breaks, UVector32 &boundary,
                               UErrorCode &status) ثابت;

    تهی printUnicodeString(ثابت UnicodeString &s) ثابت;

    UnicodeSet fDigitOrOpenPunctuationOrAlphabetSet;
    UnicodeSet fClosePunctuationSet;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t fNegativeSum;
};

#endif

U_NAMESPACE_END

/* MLBREAKENGINE_H */
#endif
