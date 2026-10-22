// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef MLBREAKENGINE_H
#define MLBREAKENGINE_H

#அந்தர்பாவ "hash.h"
#அந்தர்பாவ "unicode/resbund.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/utext.h"
#அந்தர்பாவ "uvectr32.h"

U_NAMESPACE_BEGIN

#எனில் !UCONFIG_NO_BREAK_ITERATION

/**
 * A machine learning break engine for the phrase breaking in Japanese.
 */
class MlBreakEngine : பொது UMemory {
   பொது:
    /**
     * Constructor.
     *
     * @param digitOrOpenPunctuationOrAlphabetSet An UnicodeSet with the digit, open punctuation and
     * alphabet.
     * @param closePunctuationSet An UnicodeSet with close punctuation.
     * @param status Information on any errors encountered.
     */
    MlBreakEngine(மாறிலி UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                  மாறிலி UnicodeSet &closePunctuationSet, UErrorCode &status);

    /**
     * Virtual destructor.
     */
    மெய்நிகர் ~MlBreakEngine();

   பொது:
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
                          UVector32 &foundBreaks, மாறிலி UnicodeString &inString,
                          மாறிலி LocalPointer<UVector32> &உள்ளீடு_Map, UErrorCode &status) மாறிலி;

   தனிப்பட்ட:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    வெறுமை loadMLModel(UErrorCode &error);

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
    வெறுமை init_சாவி_மதிப்பு(UResourceBundle *rb, மாறிலி எழுத்து *சாவி_Name, மாறிலி எழுத்து *மதிப்பு_Name,
                      Hashtable &model, UErrorCode &error);

    /**
     * Initialize the index list from the input string.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of inString.
     * @param status Information on any errors encountered.
     * @return The number of code units of the first four characters in inString.
     */
    int32_t initIndexList(மாறிலி UnicodeString &inString, int32_t *indexList,
                          UErrorCode &status) மாறிலி;

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
    int32_t evaluateBreakpoint(மாறிலி UnicodeString &inString, int32_t *indexList, int32_t startIdx,
                               int32_t இலக்கம்_Code_Units, int32_t இலக்கம்_Breaks, UVector32 &boundary,
                               UErrorCode &status) மாறிலி;

    வெறுமை printUnicodeString(மாறிலி UnicodeString &s) மாறிலி;

    UnicodeSet fDigitOrOpenPunctuationOrAlphabetSet;
    UnicodeSet fClosePunctuationSet;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t fNegativeSum;
};

#endif

U_NAMESPACE_END

/* MLBREAKENGINE_H */
#endif
