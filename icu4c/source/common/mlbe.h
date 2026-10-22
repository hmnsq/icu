// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#که_نه_تعریف_شوی MLBREAKENGINE_H
#تعریف_کړه MLBREAKENGINE_H

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
    MlBreakEngine(ثابت Unicode_سېټ &digit_Or_Open_Punctuation_Or_Alphabet_سېټ,
                  ثابت Unicode_سېټ &close_Punctuation_سېټ, UError_کوډ &حالت);

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
    int32_t divide_پورته_Range(UText *دننه_متن, int32_t range_پیل, int32_t range_پای_ټکی,
                          UVector32 &وموندل_Breaks, ثابت UnicodeString &دننه_String,
                          ثابت Local_اشاره_ګر<UVector32> &ننوت_Map, UError_کوډ &حالت) ثابت;

   خاص:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    فراغ بارول_MLModel(UError_کوډ &error);

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
    فراغ init_کلی_ارزښت(UResourceBundle *rb, ثابت حرف *کلی_Name, ثابت حرف *ارزښت_Name,
                      Hashtable &model, UError_کوډ &error);

    /**
     * Initialize the index list from the input string.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of inString.
     * @param status Information on any errors encountered.
     * @return The number of code units of the first four characters in inString.
     */
    int32_t init_شاخص_List(ثابت UnicodeString &دننه_String, int32_t *شاخص_List,
                          UError_کوډ &حالت) ثابت;

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
    int32_t evaluateBreakpoint(ثابت UnicodeString &دننه_String, int32_t *شاخص_List, int32_t پیل_Idx,
                               int32_t عدد_کوډ_Units, int32_t عدد_Breaks, UVector32 &boundary,
                               UError_کوډ &حالت) ثابت;

    فراغ printUnicodeString(ثابت UnicodeString &s) ثابت;

    Unicode_سېټ f_Digit_Or_Open_Punctuation_Or_Alphabet_سېټ;
    Unicode_سېټ f_Close_Punctuation_سېټ;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t f_Negative_مجموعه;
};

#د_که_پای

U_NAMESPACE_پای_ټکی

/* MLBREAKENGINE_H */
#د_که_پای
