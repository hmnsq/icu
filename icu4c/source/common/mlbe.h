// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#నిర్వచితం_కాకపోతే MLBREAKENGINE_H
#నిర్వచించు MLBREAKENGINE_H

#అన్తర్భావ "hash.h"
#అన్తర్భావ "unicode/resbund.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/utext.h"
#అన్తర్భావ "uvectr32.h"

U_NAMESPACE_BEGIN

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

/**
 * A machine learning break engine for the phrase breaking in Japanese.
 */
class MlBreakEngine : బహిరంగ UMemory {
   బహిరంగ:
    /**
     * Constructor.
     *
     * @param digitOrOpenPunctuationOrAlphabetSet An UnicodeSet with the digit, open punctuation and
     * alphabet.
     * @param closePunctuationSet An UnicodeSet with close punctuation.
     * @param status Information on any errors encountered.
     */
    MlBreakEngine(స్థిరరాశి Unicode_సమితి &digit_Or_Open_Punctuation_Or_Alphabet_సమితి,
                  స్థిరరాశి Unicode_సమితి &close_Punctuation_సమితి, UError_సంకేతం &స్థితి_2);

    /**
     * Virtual destructor.
     */
    మాయా ~MlBreakEngine();

   బహిరంగ:
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
    int32_t divide_పైకి_Range(UText *లోపల_పాఠ్యం, int32_t range_ప్రారంభం, int32_t range_చివర,
                          UVector32 &దొరికింది_Breaks, స్థిరరాశి UnicodeString &లోపల_String,
                          స్థిరరాశి Local_సూచిక<UVector32> &ఆదానం_Map, UError_సంకేతం &స్థితి_2) స్థిరరాశి;

   అంతరంగ:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    శూన్య ఎక్కించు_MLModel(UError_సంకేతం &error);

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
    శూన్య init_కీలకం_విలువ(UResourceBundle *rb, స్థిరరాశి అక్షరం *కీలకం_Name, స్థిరరాశి అక్షరం *విలువ_Name,
                      Hashtable &model, UError_సంకేతం &error);

    /**
     * Initialize the index list from the input string.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of inString.
     * @param status Information on any errors encountered.
     * @return The number of code units of the first four characters in inString.
     */
    int32_t init_సూచీ_List(స్థిరరాశి UnicodeString &లోపల_String, int32_t *సూచీ_List,
                          UError_సంకేతం &స్థితి_2) స్థిరరాశి;

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
    int32_t evaluateBreakpoint(స్థిరరాశి UnicodeString &లోపల_String, int32_t *సూచీ_List, int32_t ప్రారంభం_Idx,
                               int32_t అంకె_సంకేతం_Units, int32_t అంకె_Breaks, UVector32 &boundary,
                               UError_సంకేతం &స్థితి_2) స్థిరరాశి;

    శూన్య printUnicodeString(స్థిరరాశి UnicodeString &s) స్థిరరాశి;

    Unicode_సమితి f_Digit_Or_Open_Punctuation_Or_Alphabet_సమితి;
    Unicode_సమితి f_Close_Punctuation_సమితి;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t f_Negative_కూడిక;
};

#అయితే_ముగింపు

U_NAMESPACE_చివర

/* MLBREAKENGINE_H */
#అయితే_ముగింపు
