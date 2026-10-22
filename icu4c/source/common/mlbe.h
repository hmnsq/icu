// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#यदि_अपरिभाषितम् MLBREAKENGINE_H
#परिभाषय MLBREAKENGINE_H

#अन्तर्भाव "hash.h"
#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/utext.h"
#अन्तर्भाव "uvectr32.h"

U_NAMESPACE_BEGIN

#यदि !UCONFIG_NO_BREAK_ITERATION

/**
 * A machine learning break engine for the phrase breaking in Japanese.
 */
class MlBreakEngine : सार्वजनिक UMemory {
   सार्वजनिक:
    /**
     * Constructor.
     *
     * @param digitOrOpenPunctuationOrAlphabetSet An UnicodeSet with the digit, open punctuation and
     * alphabet.
     * @param closePunctuationSet An UnicodeSet with close punctuation.
     * @param status Information on any errors encountered.
     */
    MlBreakEngine(ध्रुव Unicode_समुच्चयः &digit_Or_Open_Punctuation_Or_Alphabet_समुच्चयः,
                  ध्रुव Unicode_समुच्चयः &close_Punctuation_समुच्चयः, UError_संहिता &स्थितिः);

    /**
     * Virtual destructor.
     */
    आभास ~MlBreakEngine();

   सार्वजनिक:
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
    int32_t divide_ऊर्ध्वम्_Range(UText *अन्तर्_Text, int32_t rangeStart, int32_t range_अन्तः,
                          UVector32 &प्राप्तम्_Breaks, ध्रुव UnicodeString &अन्तर्_String,
                          ध्रुव LocalPointer<UVector32> &निवेशः_Map, UError_संहिता &स्थितिः) ध्रुव;

   गुप्त:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    शून्य आरोपय_MLModel(UError_संहिता &error);

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
    शून्य init_कुञ्चिका_मानम्(UResourceBundle *rb, ध्रुव वर्ण *कुञ्चिका_Name, ध्रुव वर्ण *मानम्_Name,
                      Hashtable &model, UError_संहिता &error);

    /**
     * Initialize the index list from the input string.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of inString.
     * @param status Information on any errors encountered.
     * @return The number of code units of the first four characters in inString.
     */
    int32_t init_सूचकांक_List(ध्रुव UnicodeString &अन्तर्_String, int32_t *सूचकांक_List,
                          UError_संहिता &स्थितिः) ध्रुव;

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
    int32_t evaluateBreakpoint(ध्रुव UnicodeString &अन्तर्_String, int32_t *सूचकांक_List, int32_t startIdx,
                               int32_t अङ्कः_संहिता_Units, int32_t अङ्कः_Breaks, UVector32 &boundary,
                               UError_संहिता &स्थितिः) ध्रुव;

    शून्य printUnicodeString(ध्रुव UnicodeString &s) ध्रुव;

    Unicode_समुच्चयः f_Digit_Or_Open_Punctuation_Or_Alphabet_समुच्चयः;
    Unicode_समुच्चयः f_Close_Punctuation_समुच्चयः;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t f_Negative_योग;
};

#यदि_अन्तः

U_NAMESPACE_अन्तः

/* MLBREAKENGINE_H */
#यदि_अन्तः
