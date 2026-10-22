// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#جے_بنا_تعریف MLBREAKENGINE_H
#تعریف_کرو MLBREAKENGINE_H

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
    MlBreakEngine(ثابت Unicode_سیٹ &digit_Or_Open_Punctuation_Or_Alphabet_سیٹ,
                  ثابت Unicode_سیٹ &close_Punctuation_سیٹ, UError_کوڈ &حالت);

    /**
     * Virtual destructor.
     */
    مجازی ~MlBreakEngine();

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
    int32_t divide_اتے_Range(UText *وچ_متن, int32_t range_شروع, int32_t range_سرا,
                          UVector32 &لبھیا_Breaks, ثابت UnicodeString &وچ_String,
                          ثابت Local_اشارہ_گر<UVector32> &داخلہ_Map, UError_کوڈ &حالت) ثابت;

   خاص:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    فراغ لدو_MLModel(UError_کوڈ &error);

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
    فراغ init_کلید_قدر(UResourceBundle *rb, ثابت حرف *کلید_Name, ثابت حرف *قدر_Name,
                      Hashtable &model, UError_کوڈ &error);

    /**
     * Initialize the index list from the input string.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of inString.
     * @param status Information on any errors encountered.
     * @return The number of code units of the first four characters in inString.
     */
    int32_t init_اشاریہ_List(ثابت UnicodeString &وچ_String, int32_t *اشاریہ_List,
                          UError_کوڈ &حالت) ثابت;

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
    int32_t evaluateBreakpoint(ثابت UnicodeString &وچ_String, int32_t *اشاریہ_List, int32_t شروع_Idx,
                               int32_t عدد_کوڈ_Units, int32_t عدد_Breaks, UVector32 &boundary,
                               UError_کوڈ &حالت) ثابت;

    فراغ printUnicodeString(ثابت UnicodeString &s) ثابت;

    Unicode_سیٹ f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ;
    Unicode_سیٹ f_Close_Punctuation_سیٹ;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t f_Negative_جوڑ;
};

#جے_انت

U_NAMESPACE_سرا

/* MLBREAKENGINE_H */
#جے_انت
