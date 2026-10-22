// © 2022 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MLBREAKENGINE_H
#ವ್ಯಾಖ್ಯಾನಿಸು MLBREAKENGINE_H

#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "unicode/resbund.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/utext.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"

U_NAMESPACE_BEGIN

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

/**
 * A machine learning break engine for the phrase breaking in Japanese.
 */
class MlBreakEngine : ಸಾರ್ವಜನಿಕ UMemory {
   ಸಾರ್ವಜನಿಕ:
    /**
     * Constructor.
     *
     * @param digitOrOpenPunctuationOrAlphabetSet An UnicodeSet with the digit, open punctuation and
     * alphabet.
     * @param closePunctuationSet An UnicodeSet with close punctuation.
     * @param status Information on any errors encountered.
     */
    MlBreakEngine(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &digit_Or_Open_Punctuation_Or_Alphabet_ಗಣ,
                  ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &close_Punctuation_ಗಣ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    /**
     * Virtual destructor.
     */
    ವಾಸ್ತವಿಕ ~MlBreakEngine();

   ಸಾರ್ವಜನಿಕ:
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
    int32_t divide_ಮೇಲೆ_Range(UText *ಒಳ_ಪಠ್ಯ, int32_t range_ಆರಂಭ, int32_t range_ಕೊನೆ,
                          UVector32 &ಸಿಕ್ಕಿತು_Breaks, ಸ್ಥಿರಾಂಕ UnicodeString &ಒಳ_String,
                          ಸ್ಥಿರಾಂಕ Local_ಸೂಚಕ<UVector32> &ಒಳಹರಿವು_Map, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

   ಖಾಸಗಿ:
    /**
     * Load the machine learning's model file.
     *
     * @param error Information on any errors encountered.
     */
    ಶೂನ್ಯ ತುಂಬು_MLModel(UError_ಸಂಕೇತ &error);

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
    ಶೂನ್ಯ init_ಕೀಲಿ_ಮೌಲ್ಯ(UResourceBundle *rb, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_Name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ_Name,
                      Hashtable &model, UError_ಸಂಕೇತ &error);

    /**
     * Initialize the index list from the input string.
     *
     * @param inString A input string to be segmented.
     * @param indexList A code unit index list of inString.
     * @param status Information on any errors encountered.
     * @return The number of code units of the first four characters in inString.
     */
    int32_t init_ಸೂಚ್ಯಂಕ_List(ಸ್ಥಿರಾಂಕ UnicodeString &ಒಳ_String, int32_t *ಸೂಚ್ಯಂಕ_List,
                          UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

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
    int32_t evaluateBreakpoint(ಸ್ಥಿರಾಂಕ UnicodeString &ಒಳ_String, int32_t *ಸೂಚ್ಯಂಕ_List, int32_t ಆರಂಭ_Idx,
                               int32_t ಅಂಕೆ_ಸಂಕೇತ_Units, int32_t ಅಂಕೆ_Breaks, UVector32 &boundary,
                               UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

    ಶೂನ್ಯ printUnicodeString(ಸ್ಥಿರಾಂಕ UnicodeString &s) ಸ್ಥಿರಾಂಕ;

    Unicode_ಗಣ f_Digit_Or_Open_Punctuation_Or_Alphabet_ಗಣ;
    Unicode_ಗಣ f_Close_Punctuation_ಗಣ;
    Hashtable fModel[13];  // {UW1, UW2, ... UW6, BW1, ... BW3, TW1, TW2, ... TW4} 6+3+4= 13
    int32_t f_Negative_ಮೊತ್ತ;
};

#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_ಕೊನೆ

/* MLBREAKENGINE_H */
#ಆದರೆ_ಅಂತ್ಯ
