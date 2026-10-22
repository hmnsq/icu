// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MLBREAKENGINE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MLBREAKENGINE_H

#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/resbund.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"

U_NAMESPACE_BEGIN

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

/**
 * A machine learning break engine for the phrase breaking in Japanese.
 */
class MlBreakEngine : ਜਨਤਕ UMemory {
   ਜਨਤਕ:
    /**
     * Constructor.
     *
     * @param digitOrOpenPunctuationOrAlphabetSet An UnicodeSet with the digit, open punctuation and
     * alphabet.
     * @param closePunctuationSet An UnicodeSet with close punctuation.
     * @param status Information on any errors encountered.
     */
    MlBreakEngine(ਅਚਲ Unicode_ਸੈੱਟ &digit_Or_Open_Punctuation_Or_Alphabet_ਸੈੱਟ,
                  ਅਚਲ Unicode_ਸੈੱਟ &close_Punctuation_ਸੈੱਟ, UError_ਕੋਡ &ਸਥਿਤੀ);

    /**
     * Virtual destructor.
     */
    ਆਭਾਸੀ ~MlBreakEngine();

   ਜਨਤਕ:
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
    int32_t divide_ਉੱਪਰ_Range(UText *ਵਿੱਚ_ਪਾਠ, int32_t range_ਸ਼ੁਰੂ, int32_t range_ਸਿਰਾ,
                          UVector32 &ਮਿਲਿਆ_Breaks, ਅਚਲ UnicodeString &ਵਿੱਚ_String,
                          ਅਚਲ Local_ਸੂਚਕ<UVector32> &ਨਿਵੇਸ਼_Map, UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ;

   ਨਿਜੀ:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    ਖਾਲੀ ਲੱਦੋ_MLModel(UError_ਕੋਡ &error);

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
    ਖਾਲੀ init_ਕੁੰਜੀ_ਮੁੱਲ(UResourceBundle *rb, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_Name, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ_Name,
                      Hashtable &model, UError_ਕੋਡ &error);

    /**
     * Initialize the index list from the input string.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of inString.
     * @param status Information on any errors encountered.
     * @return The number of code units of the first four characters in inString.
     */
    int32_t init_ਸੂਚਕਾਂਕ_List(ਅਚਲ UnicodeString &ਵਿੱਚ_String, int32_t *ਸੂਚਕਾਂਕ_List,
                          UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ;

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
    int32_t evaluateBreakpoint(ਅਚਲ UnicodeString &ਵਿੱਚ_String, int32_t *ਸੂਚਕਾਂਕ_List, int32_t ਸ਼ੁਰੂ_Idx,
                               int32_t ਅੰਕ_ਕੋਡ_Units, int32_t ਅੰਕ_Breaks, UVector32 &boundary,
                               UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ;

    ਖਾਲੀ printUnicodeString(ਅਚਲ UnicodeString &s) ਅਚਲ;

    Unicode_ਸੈੱਟ f_Digit_Or_Open_Punctuation_Or_Alphabet_ਸੈੱਟ;
    Unicode_ਸੈੱਟ f_Close_Punctuation_ਸੈੱਟ;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t f_Negative_ਜੋੜ;
};

#ਜੇ_ਅੰਤ

U_NAMESPACE_ਸਿਰਾ

/* MLBREAKENGINE_H */
#ਜੇ_ਅੰਤ
