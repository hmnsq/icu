// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// casemap.h
// created: 2017jan12 Markus W. Scherer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __CASEMAP_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __CASEMAP_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Low-level C++ case mapping functions.
 */

U_NAMESPACE_BEGIN

class BreakIterator;
class ByteSink;
class Edits;

/**
 * Low-level C++ case mapping functions.
 *
 * @stable ICU 59
 */
class U_COMMON_API CaseMap final : ਜਨਤਕ UMemory {
ਜਨਤਕ:
    /**
     * Lowercases a UTF-16 string and optionally records edits.
     * Casing is locale-dependent and context-sensitive.
     * The result may be longer or shorter than the original.
     * The source string and the destination buffer must not overlap.
     *
     * @param locale    The locale ID. ("" = root locale, nullptr = default locale.)
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT and U_EDITS_NO_RESET.
     * @param src       The original string.
     * @param srcLength The length of the original string. If -1, then src must be NUL-terminated.
     * @param dest      A buffer for the result string. The result will be NUL-terminated if
     *                  the buffer is large enough.
     *                  The contents is undefined in case of failure.
     * @param destCapacity The size of the buffer (number of char16_ts). If it is 0, then
     *                  dest may be nullptr and the function will only return the length of the result
     *                  without writing any of the result string.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     * @return The length of the result string, if successful.
     *         When the result would be longer than destCapacity,
     *         the full length is returned and a U_BUFFER_OVERFLOW_ERROR is set.
     *
     * @see u_strToLower
     * @stable ICU 59
     */
     ਸਥਿਰ int32_t ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(
            ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
            ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
            char16_t *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Uppercases a UTF-16 string and optionally records edits.
     * Casing is locale-dependent and context-sensitive.
     * The result may be longer or shorter than the original.
     * The source string and the destination buffer must not overlap.
     *
     * @param locale    The locale ID. ("" = root locale, nullptr = default locale.)
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT and U_EDITS_NO_RESET.
     * @param src       The original string.
     * @param srcLength The length of the original string. If -1, then src must be NUL-terminated.
     * @param dest      A buffer for the result string. The result will be NUL-terminated if
     *                  the buffer is large enough.
     *                  The contents is undefined in case of failure.
     * @param destCapacity The size of the buffer (number of char16_ts). If it is 0, then
     *                  dest may be nullptr and the function will only return the length of the result
     *                  without writing any of the result string.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     * @return The length of the result string, if successful.
     *         When the result would be longer than destCapacity,
     *         the full length is returned and a U_BUFFER_OVERFLOW_ERROR is set.
     *
     * @see u_strToUpper
     * @stable ICU 59
     */
    ਸਥਿਰ int32_t ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(
            ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
            ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
            char16_t *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

    /**
     * Titlecases a UTF-16 string and optionally records edits.
     * Casing is locale-dependent and context-sensitive.
     * The result may be longer or shorter than the original.
     * The source string and the destination buffer must not overlap.
     *
     * Titlecasing uses a break iterator to find the first characters of words
     * that are to be titlecased. It titlecases those characters and lowercases
     * all others. (This can be modified with options bits.)
     *
     * @param locale    The locale ID. ("" = root locale, nullptr = default locale.)
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT, U_EDITS_NO_RESET,
     *                  U_TITLECASE_NO_LOWERCASE,
     *                  U_TITLECASE_NO_BREAK_ADJUSTMENT, U_TITLECASE_ADJUST_TO_CASED,
     *                  U_TITLECASE_WHOLE_STRING, U_TITLECASE_SENTENCES.
     * @param iter      A break iterator to find the first characters of words that are to be titlecased.
     *                  It is set to the source string (setText())
     *                  and used one or more times for iteration (first() and next()).
     *                  If nullptr, then a word break iterator for the locale is used
     *                  (or something equivalent).
     * @param src       The original string.
     * @param srcLength The length of the original string. If -1, then src must be NUL-terminated.
     * @param dest      A buffer for the result string. The result will be NUL-terminated if
     *                  the buffer is large enough.
     *                  The contents is undefined in case of failure.
     * @param destCapacity The size of the buffer (number of char16_ts). If it is 0, then
     *                  dest may be nullptr and the function will only return the length of the result
     *                  without writing any of the result string.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     * @return The length of the result string, if successful.
     *         When the result would be longer than destCapacity,
     *         the full length is returned and a U_BUFFER_OVERFLOW_ERROR is set.
     *
     * @see u_strToTitle
     * @see ucasemap_toTitle
     * @stable ICU 59
     */
    ਸਥਿਰ int32_t ਮੰਜ਼ਿਲ_Title(
            ਅਚਲ ਅੱਖਰ *locale, uint32_t options, BreakIterator *iter,
            ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
            char16_t *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

#ਜੇ_ਅੰਤ  // UCONFIG_NO_BREAK_ITERATION

    /**
     * Case-folds a UTF-16 string and optionally records edits.
     *
     * Case folding is locale-independent and not context-sensitive,
     * but there is an option for whether to include or exclude mappings for dotted I
     * and dotless i that are marked with 'T' in CaseFolding.txt.
     *
     * The result may be longer or shorter than the original.
     * The source string and the destination buffer must not overlap.
     *
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT, U_EDITS_NO_RESET,
     *                  U_FOLD_CASE_DEFAULT, U_FOLD_CASE_EXCLUDE_SPECIAL_I.
     * @param src       The original string.
     * @param srcLength The length of the original string. If -1, then src must be NUL-terminated.
     * @param dest      A buffer for the result string. The result will be NUL-terminated if
     *                  the buffer is large enough.
     *                  The contents is undefined in case of failure.
     * @param destCapacity The size of the buffer (number of char16_ts). If it is 0, then
     *                  dest may be nullptr and the function will only return the length of the result
     *                  without writing any of the result string.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     * @return The length of the result string, if successful.
     *         When the result would be longer than destCapacity,
     *         the full length is returned and a U_BUFFER_OVERFLOW_ERROR is set.
     *
     * @see u_strFoldCase
     * @stable ICU 59
     */
    ਸਥਿਰ int32_t fold(
            uint32_t options,
            ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
            char16_t *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Lowercases a UTF-8 string and optionally records edits.
     * Casing is locale-dependent and context-sensitive.
     * The result may be longer or shorter than the original.
     *
     * @param locale    The locale ID. ("" = root locale, nullptr = default locale.)
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT and U_EDITS_NO_RESET.
     * @param src       The original string.
     * @param sink      A ByteSink to which the result string is written.
     *                  sink.Flush() is called at the end.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     *
     * @see ucasemap_utf8ToLower
     * @stable ICU 60
     */
    ਸਥਿਰ ਖਾਲੀ utf8_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(
            ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
            StringPiece src, ByteSink &sink, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Uppercases a UTF-8 string and optionally records edits.
     * Casing is locale-dependent and context-sensitive.
     * The result may be longer or shorter than the original.
     *
     * @param locale    The locale ID. ("" = root locale, nullptr = default locale.)
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT and U_EDITS_NO_RESET.
     * @param src       The original string.
     * @param sink      A ByteSink to which the result string is written.
     *                  sink.Flush() is called at the end.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     *
     * @see ucasemap_utf8ToUpper
     * @stable ICU 60
     */
    ਸਥਿਰ ਖਾਲੀ utf8_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(
            ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
            StringPiece src, ByteSink &sink, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

    /**
     * Titlecases a UTF-8 string and optionally records edits.
     * Casing is locale-dependent and context-sensitive.
     * The result may be longer or shorter than the original.
     *
     * Titlecasing uses a break iterator to find the first characters of words
     * that are to be titlecased. It titlecases those characters and lowercases
     * all others. (This can be modified with options bits.)
     *
     * @param locale    The locale ID. ("" = root locale, nullptr = default locale.)
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT, U_EDITS_NO_RESET,
     *                  U_TITLECASE_NO_LOWERCASE,
     *                  U_TITLECASE_NO_BREAK_ADJUSTMENT, U_TITLECASE_ADJUST_TO_CASED,
     *                  U_TITLECASE_WHOLE_STRING, U_TITLECASE_SENTENCES.
     * @param iter      A break iterator to find the first characters of words that are to be titlecased.
     *                  It is set to the source string (setUText())
     *                  and used one or more times for iteration (first() and next()).
     *                  If nullptr, then a word break iterator for the locale is used
     *                  (or something equivalent).
     * @param src       The original string.
     * @param sink      A ByteSink to which the result string is written.
     *                  sink.Flush() is called at the end.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     *
     * @see ucasemap_utf8ToTitle
     * @stable ICU 60
     */
    ਸਥਿਰ ਖਾਲੀ utf8_ਮੰਜ਼ਿਲ_Title(
            ਅਚਲ ਅੱਖਰ *locale, uint32_t options, BreakIterator *iter,
            StringPiece src, ByteSink &sink, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

#ਜੇ_ਅੰਤ  // UCONFIG_NO_BREAK_ITERATION

    /**
     * Case-folds a UTF-8 string and optionally records edits.
     *
     * Case folding is locale-independent and not context-sensitive,
     * but there is an option for whether to include or exclude mappings for dotted I
     * and dotless i that are marked with 'T' in CaseFolding.txt.
     *
     * The result may be longer or shorter than the original.
     *
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT and U_EDITS_NO_RESET.
     * @param src       The original string.
     * @param sink      A ByteSink to which the result string is written.
     *                  sink.Flush() is called at the end.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     *
     * @see ucasemap_utf8FoldCase
     * @stable ICU 60
     */
    ਸਥਿਰ ਖਾਲੀ utf8Fold(
            uint32_t options,
            StringPiece src, ByteSink &sink, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Lowercases a UTF-8 string and optionally records edits.
     * Casing is locale-dependent and context-sensitive.
     * The result may be longer or shorter than the original.
     * The source string and the destination buffer must not overlap.
     *
     * @param locale    The locale ID. ("" = root locale, nullptr = default locale.)
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT and U_EDITS_NO_RESET.
     * @param src       The original string.
     * @param srcLength The length of the original string. If -1, then src must be NUL-terminated.
     * @param dest      A buffer for the result string. The result will be NUL-terminated if
     *                  the buffer is large enough.
     *                  The contents is undefined in case of failure.
     * @param destCapacity The size of the buffer (number of bytes). If it is 0, then
     *                  dest may be nullptr and the function will only return the length of the result
     *                  without writing any of the result string.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     * @return The length of the result string, if successful.
     *         When the result would be longer than destCapacity,
     *         the full length is returned and a U_BUFFER_OVERFLOW_ERROR is set.
     *
     * @see ucasemap_utf8ToLower
     * @stable ICU 59
     */
    ਸਥਿਰ int32_t utf8_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(
            ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
            ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
            ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

    /**
     * Uppercases a UTF-8 string and optionally records edits.
     * Casing is locale-dependent and context-sensitive.
     * The result may be longer or shorter than the original.
     * The source string and the destination buffer must not overlap.
     *
     * @param locale    The locale ID. ("" = root locale, nullptr = default locale.)
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT and U_EDITS_NO_RESET.
     * @param src       The original string.
     * @param srcLength The length of the original string. If -1, then src must be NUL-terminated.
     * @param dest      A buffer for the result string. The result will be NUL-terminated if
     *                  the buffer is large enough.
     *                  The contents is undefined in case of failure.
     * @param destCapacity The size of the buffer (number of bytes). If it is 0, then
     *                  dest may be nullptr and the function will only return the length of the result
     *                  without writing any of the result string.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     * @return The length of the result string, if successful.
     *         When the result would be longer than destCapacity,
     *         the full length is returned and a U_BUFFER_OVERFLOW_ERROR is set.
     *
     * @see ucasemap_utf8ToUpper
     * @stable ICU 59
     */
    ਸਥਿਰ int32_t utf8_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(
            ਅਚਲ ਅੱਖਰ *locale, uint32_t options,
            ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
            ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

    /**
     * Titlecases a UTF-8 string and optionally records edits.
     * Casing is locale-dependent and context-sensitive.
     * The result may be longer or shorter than the original.
     * The source string and the destination buffer must not overlap.
     *
     * Titlecasing uses a break iterator to find the first characters of words
     * that are to be titlecased. It titlecases those characters and lowercases
     * all others. (This can be modified with options bits.)
     *
     * @param locale    The locale ID. ("" = root locale, nullptr = default locale.)
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT, U_EDITS_NO_RESET,
     *                  U_TITLECASE_NO_LOWERCASE,
     *                  U_TITLECASE_NO_BREAK_ADJUSTMENT, U_TITLECASE_ADJUST_TO_CASED,
     *                  U_TITLECASE_WHOLE_STRING, U_TITLECASE_SENTENCES.
     * @param iter      A break iterator to find the first characters of words that are to be titlecased.
     *                  It is set to the source string (setUText())
     *                  and used one or more times for iteration (first() and next()).
     *                  If nullptr, then a word break iterator for the locale is used
     *                  (or something equivalent).
     * @param src       The original string.
     * @param srcLength The length of the original string. If -1, then src must be NUL-terminated.
     * @param dest      A buffer for the result string. The result will be NUL-terminated if
     *                  the buffer is large enough.
     *                  The contents is undefined in case of failure.
     * @param destCapacity The size of the buffer (number of bytes). If it is 0, then
     *                  dest may be nullptr and the function will only return the length of the result
     *                  without writing any of the result string.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     * @return The length of the result string, if successful.
     *         When the result would be longer than destCapacity,
     *         the full length is returned and a U_BUFFER_OVERFLOW_ERROR is set.
     *
     * @see ucasemap_utf8ToTitle
     * @stable ICU 59
     */
    ਸਥਿਰ int32_t utf8_ਮੰਜ਼ਿਲ_Title(
            ਅਚਲ ਅੱਖਰ *locale, uint32_t options, BreakIterator *iter,
            ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
            ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

#ਜੇ_ਅੰਤ  // UCONFIG_NO_BREAK_ITERATION

    /**
     * Case-folds a UTF-8 string and optionally records edits.
     *
     * Case folding is locale-independent and not context-sensitive,
     * but there is an option for whether to include or exclude mappings for dotted I
     * and dotless i that are marked with 'T' in CaseFolding.txt.
     *
     * The result may be longer or shorter than the original.
     * The source string and the destination buffer must not overlap.
     *
     * @param options   Options bit set, usually 0. See U_OMIT_UNCHANGED_TEXT, U_EDITS_NO_RESET,
     *                  U_FOLD_CASE_DEFAULT, U_FOLD_CASE_EXCLUDE_SPECIAL_I.
     * @param src       The original string.
     * @param srcLength The length of the original string. If -1, then src must be NUL-terminated.
     * @param dest      A buffer for the result string. The result will be NUL-terminated if
     *                  the buffer is large enough.
     *                  The contents is undefined in case of failure.
     * @param destCapacity The size of the buffer (number of bytes). If it is 0, then
     *                  dest may be nullptr and the function will only return the length of the result
     *                  without writing any of the result string.
     * @param edits     Records edits for index mapping, working with styled text,
     *                  and getting only changes (if any).
     *                  The Edits contents is undefined if any error occurs.
     *                  This function calls edits->reset() first unless
     *                  options includes U_EDITS_NO_RESET. edits can be nullptr.
     * @param errorCode Reference to an in/out error code value
     *                  which must not indicate a failure before the function call.
     * @return The length of the result string, if successful.
     *         When the result would be longer than destCapacity,
     *         the full length is returned and a U_BUFFER_OVERFLOW_ERROR is set.
     *
     * @see ucasemap_utf8FoldCase
     * @stable ICU 59
     */
    ਸਥਿਰ int32_t utf8Fold(
            uint32_t options,
            ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
            ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ, Edits *edits,
            UError_ਕੋਡ &error_ਕੋਡ);

ਨਿਜੀ:
    CaseMap() = delete;
    CaseMap(ਅਚਲ CaseMap &other) = delete;
    CaseMap &ਸੰਚਾਲਕ=(ਅਚਲ CaseMap &other) = delete;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* U_SHOW_CPLUSPLUS_API */

#ਜੇ_ਅੰਤ  // __CASEMAP_H__
