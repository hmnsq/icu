// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef MLBREAKENGINE_H
#define MLBREAKENGINE_H

#অন্তর্ভাৱ "hash.h"
#অন্তর্ভাৱ "unicode/resbund.h"
#অন্তর্ভাৱ "unicode/uniset.h"
#অন্তর্ভাৱ "unicode/utext.h"
#অন্তর্ভাৱ "uvectr32.h"

U_NAMESPACE_BEGIN

#যদি !UCONFIG_NO_BREAK_ITERATION

/**
 * A machine learning break engine for the phrase breaking in Japanese.
 */
class MlBreakEngine : সার্ৱজনিক UMemory {
   সার্ৱজনিক:
    /**
     * Constructor.
     *
     * @param digitOrOpenPunctuationOrAlphabetSet An UnicodeSet with the digit, open punctuation and
     * alphabet.
     * @param closePunctuationSet An UnicodeSet with close punctuation.
     * @param status Information on any errors encountered.
     */
    MlBreakEngine(ধ্রুৱ UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                  ধ্রুৱ UnicodeSet &closePunctuationSet, UErrorCode &status);

    /**
     * Virtual destructor.
     */
    আভাস ~MlBreakEngine();

   সার্ৱজনিক:
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
                          UVector32 &foundBreaks, ধ্রুৱ UnicodeString &inString,
                          ধ্রুৱ LocalPointer<UVector32> &প্ৰবেশ_Map, UErrorCode &status) ধ্রুৱ;

   গুপ্ত:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    শূন্য loadMLModel(UErrorCode &error);

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
    শূন্য init_চাবি_মান(UResourceBundle *rb, ধ্রুৱ ৱর্ণ *চাবি_Name, ধ্রুৱ ৱর্ণ *মান_Name,
                      Hashtable &model, UErrorCode &error);

    /**
     * Initialize the index list from the input string.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of inString.
     * @param status Information on any errors encountered.
     * @return The number of code units of the first four characters in inString.
     */
    int32_t initIndexList(ধ্রুৱ UnicodeString &inString, int32_t *indexList,
                          UErrorCode &status) ধ্রুৱ;

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
    int32_t evaluateBreakpoint(ধ্রুৱ UnicodeString &inString, int32_t *indexList, int32_t startIdx,
                               int32_t অঙ্ক_Code_Units, int32_t অঙ্ক_Breaks, UVector32 &boundary,
                               UErrorCode &status) ধ্রুৱ;

    শূন্য printUnicodeString(ধ্রুৱ UnicodeString &s) ধ্রুৱ;

    UnicodeSet fDigitOrOpenPunctuationOrAlphabetSet;
    UnicodeSet fClosePunctuationSet;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t fNegativeSum;
};

#endif

U_NAMESPACE_END

/* MLBREAKENGINE_H */
#endif
