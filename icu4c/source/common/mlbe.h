// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अगर_अघोषित MLBREAKENGINE_H
#घोषणा MLBREAKENGINE_H

#समावेश "hash.h"
#समावेश "unicode/resbund.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/utext.h"
#समावेश "uvectr32.h"

U_NAMESPACE_BEGIN

#अगर !UCONFIG_NO_BREAK_ITERATION

/**
 * A machine learning break engine for the phrase breaking in Japanese.
 */
class MlBreakEngine : खुला UMemory {
   खुला:
    /**
     * Constructor.
     *
     * @param digitOrOpenPunctuationOrAlphabetSet An UnicodeSet with the digit, open punctuation and
     * alphabet.
     * @param closePunctuationSet An UnicodeSet with close punctuation.
     * @param status Information on any errors encountered.
     */
    MlBreakEngine(स्थिर UnicodeSet &digitOrOpenPunctuationOrAlphabetSet,
                  स्थिर UnicodeSet &closePunctuationSet, UErrorCode &status);

    /**
     * Virtual destructor.
     */
    भव ~MlBreakEngine();

   खुला:
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
                          UVector32 &foundBreaks, स्थिर UnicodeString &inString,
                          स्थिर LocalPointer<UVector32> &निवेश_Map, UErrorCode &status) स्थिर;

   निजी:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    व्योम loadMLModel(UErrorCode &error);

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
    व्योम init_कुंजी_मूल्य(UResourceBundle *rb, स्थिर अक्षर *कुंजी_Name, स्थिर अक्षर *मूल्य_Name,
                      Hashtable &model, UErrorCode &error);

    /**
     * Initialize the index list from the input string.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of inString.
     * @param status Information on any errors encountered.
     * @return The number of code units of the first four characters in inString.
     */
    int32_t init_सूचकांक_List(स्थिर UnicodeString &inString, int32_t *सूचकांक_List,
                          UErrorCode &status) स्थिर;

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
    int32_t evaluateBreakpoint(स्थिर UnicodeString &inString, int32_t *सूचकांक_List, int32_t startIdx,
                               int32_t अंक_Code_Units, int32_t अंक_Breaks, UVector32 &boundary,
                               UErrorCode &status) स्थिर;

    व्योम printUnicodeString(स्थिर UnicodeString &s) स्थिर;

    UnicodeSet fDigitOrOpenPunctuationOrAlphabetSet;
    UnicodeSet fClosePunctuationSet;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t f_Negative_योग;
};

#पूर्ण_अगर

U_NAMESPACE_END

/* MLBREAKENGINE_H */
#पूर्ण_अगर
