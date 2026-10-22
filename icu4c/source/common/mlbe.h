// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#جيڪڏهن_وصف_نه_ٿيل MLBREAKENGINE_H
#وصف_ڪريو MLBREAKENGINE_H

#ضمن "hash.h"
#ضمن "unicode/resbund.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/utext.h"
#ضمن "uvectr32.h"

U_NAMESPACE_BEGIN

#إذا !UCONFIG_NO_BREAK_ITERATION

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
    MlBreakEngine(ثابت Unicode_سيٽ &digit_Or_Open_Punctuation_Or_Alphabet_سيٽ,
                  ثابت Unicode_سيٽ &close_Punctuation_سيٽ, UError_ڪوڊ &حالت);

    /**
     * Virtual destructor.
     */
    مجازي ~MlBreakEngine();

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
    int32_t divide_مٿي_Range(UText *اندريون_متن, int32_t range_شروعات, int32_t range_پڇاڙي,
                          UVector32 &مليو_Breaks, ثابت UnicodeString &اندريون_String,
                          ثابت Local_اشارو<UVector32> &داخلا_Map, UError_ڪوڊ &حالت) ثابت;

   خاص:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    فراغ ڀريو_MLModel(UError_ڪوڊ &error);

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
    فراغ init_ڪنجي_قدر(UResourceBundle *rb, ثابت حرف *ڪنجي_Name, ثابت حرف *قدر_Name,
                      Hashtable &model, UError_ڪوڊ &error);

    /**
     * Initialize the index list from the input string.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of inString.
     * @param status Information on any errors encountered.
     * @return The number of code units of the first four characters in inString.
     */
    int32_t init_اشاريو_List(ثابت UnicodeString &اندريون_String, int32_t *اشاريو_List,
                          UError_ڪوڊ &حالت) ثابت;

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
    int32_t evaluateBreakpoint(ثابت UnicodeString &اندريون_String, int32_t *اشاريو_List, int32_t شروعات_Idx,
                               int32_t انگ_ڪوڊ_Units, int32_t انگ_Breaks, UVector32 &boundary,
                               UError_ڪوڊ &حالت) ثابت;

    فراغ printUnicodeString(ثابت UnicodeString &s) ثابت;

    Unicode_سيٽ f_Digit_Or_Open_Punctuation_Or_Alphabet_سيٽ;
    Unicode_سيٽ f_Close_Punctuation_سيٽ;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t f_Negative_جوڙ;
};

#جيڪڏهن_پورو

U_NAMESPACE_پڇاڙي

/* MLBREAKENGINE_H */
#جيڪڏهن_پورو
