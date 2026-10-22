// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef MLBREAKENGINE_H
#define MLBREAKENGINE_H

#ضمن "hash.h"
#ضمن "unicode/resbund.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/utext.h"
#ضمن "uvectr32.h"

U_NAMESPACE_BEGIN

#𐤀𐤌 !UCONFIG_NO_BREAK_ITERATION

/**
 * A machine learning break engine for the phrase breaking in Japanese.
 */
class MlBreakEngine : عام UMemory {
   عام:
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
    virtual ~MlBreakEngine();

   عام:
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
                          ثابت LocalPointer<UVector32> &inputMap, UErrorCode &status) ثابت;

   خاص:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    فراغ loadMLModel(UErrorCode &error);

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
    فراغ initKeyValue(UResourceBundle *rb, ثابت حرف *keyName, ثابت حرف *valueName,
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
                               int32_t numCodeUnits, int32_t numBreaks, UVector32 &boundary,
                               UErrorCode &status) ثابت;

    فراغ printUnicodeString(ثابت UnicodeString &s) ثابت;

    UnicodeSet fDigitOrOpenPunctuationOrAlphabetSet;
    UnicodeSet fClosePunctuationSet;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t fNegativeSum;
};

#endif

U_NAMESPACE_END

/* MLBREAKENGINE_H */
#endif
