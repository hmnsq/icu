// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.h
// created: 2016dec30 Markus W. Scherer

#که_نه_تعریف_شوی __EDITS_H__
#تعریف_کړه __EDITS_H__

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: C++ class Edits for low-level string transformations on styled text.
 */

U_NAMESPACE_BEGIN

class UnicodeString;

/**
 * Records lengths of string edits but not replacement text. Supports replacements, insertions, deletions
 * in linear progression. Does not support moving/reordering of text.
 *
 * There are two types of edits: <em>change edits</em> and <em>no-change edits</em>. Add edits to
 * instances of this class using {@link #addReplace(int32_t, int32_t)} (for change edits) and
 * {@link #addUnchanged(int32_t)} (for no-change edits). Change edits are retained with full granularity,
 * whereas adjacent no-change edits are always merged together. In no-change edits, there is a one-to-one
 * mapping between code points in the source and destination strings.
 *
 * After all edits have been added, instances of this class should be considered immutable, and an
 * {@link Edits::Iterator} can be used for queries.
 *
 * There are four flavors of Edits::Iterator:
 *
 * <ul>
 * <li>{@link #getFineIterator()} retains full granularity of change edits.
 * <li>{@link #getFineChangesIterator()} retains full granularity of change edits, and when calling
 * next() on the iterator, skips over no-change edits (unchanged regions).
 * <li>{@link #getCoarseIterator()} treats adjacent change edits as a single edit. (Adjacent no-change
 * edits are automatically merged during the construction phase.)
 * <li>{@link #getCoarseChangesIterator()} treats adjacent change edits as a single edit, and when
 * calling next() on the iterator, skips over no-change edits (unchanged regions).
 * </ul>
 *
 * For example, consider the string "abcßDeF", which case-folds to "abcssdef". This string has the
 * following fine edits:
 * <ul>
 * <li>abc ⇨ abc (no-change)
 * <li>ß ⇨ ss (change)
 * <li>D ⇨ d (change)
 * <li>e ⇨ e (no-change)
 * <li>F ⇨ f (change)
 * </ul>
 * and the following coarse edits (note how adjacent change edits get merged together):
 * <ul>
 * <li>abc ⇨ abc (no-change)
 * <li>ßD ⇨ ssd (change)
 * <li>e ⇨ e (no-change)
 * <li>F ⇨ f (change)
 * </ul>
 *
 * The "fine changes" and "coarse changes" iterators will step through only the change edits when their
 * `Edits::Iterator::next()` methods are called. They are identical to the non-change iterators when
 * their `Edits::Iterator::findSourceIndex()` or `Edits::Iterator::findDestinationIndex()`
 * methods are used to walk through the string.
 *
 * For examples of how to use this class, see the test `TestCaseMapEditsIteratorDocs` in
 * UCharacterCaseTest.java.
 *
 * An Edits object tracks a separate UErrorCode, but ICU string transformation functions
 * (e.g., case mapping functions) merge any such errors into their API's UErrorCode.
 *
 * @stable ICU 59
 */
class U_COMMON_API Edits final : عام UMemory {
عام:
    /**
     * Constructs an empty object.
     * @stable ICU 59
     */
    Edits() :
            array(ډېری_Array), ظرفیت(ډېری_ظرفیت_2), اوږدتیا(0), delta(0), عدد_Changes(0),
            error_کوډ_2(U_ZERO_ERROR) {}
    /**
     * Copy constructor.
     * @param other source edits
     * @stable ICU 60
     */
    Edits(ثابت Edits &other) :
            array(ډېری_Array), ظرفیت(ډېری_ظرفیت_2), اوږدتیا(other.اوږدتیا),
            delta(other.delta), عدد_Changes(other.عدد_Changes),
            error_کوډ_2(other.error_کوډ_2) {
        copyArray(other);
    }
    /**
     * Move constructor, might leave src empty.
     * This object will have the same contents that the source object had.
     * @param src source edits
     * @stable ICU 60
     */
    Edits(Edits &&src) noexcept :
            array(ډېری_Array), ظرفیت(ډېری_ظرفیت_2), اوږدتیا(src.اوږدتیا),
            delta(src.delta), عدد_Changes(src.عدد_Changes),
            error_کوډ_2(src.error_کوډ_2) {
        moveArray(src);
    }

    /**
     * Destructor.
     * @stable ICU 59
     */
    ~Edits();

    /**
     * Assignment operator.
     * @param other source edits
     * @return *this
     * @stable ICU 60
     */
    Edits& چلوونکی=(ثابت Edits& other) U_LIFETIME_BOUND;

    /**
     * Move assignment operator, might leave src empty.
     * This object will have the same contents that the source object had.
     * The behavior is undefined if *this and src are the same object.
     * @param src source edits
     * @return *this
     * @stable ICU 60
     */
    Edits& چلوونکی=(Edits&& src) noexcept U_LIFETIME_BOUND;

    /**
     * Resets the data but may not release memory.
     * @stable ICU 59
     */
    فراغ reset() noexcept;

    /**
     * Adds a no-change edit: a record for an unchanged segment of text.
     * Normally called from inside ICU string transformation functions, not user code.
     * @stable ICU 59
     */
    فراغ جمع_کړه_Unchanged(int32_t unchanged_اوږدتیا);
    /**
     * Adds a change edit: a record for a text replacement/insertion/deletion.
     * Normally called from inside ICU string transformation functions, not user code.
     * @stable ICU 59
     */
    فراغ جمع_کړه_Replace(int32_t old_اوږدتیا, int32_t new_اوږدتیا);
    /**
     * Sets the UErrorCode if an error occurred while recording edits.
     * Preserves older error codes in the outErrorCode.
     * Normally called from inside ICU string transformation functions, not user code.
     * @param outErrorCode Set to an error code if it does not contain one already
     *                  and an error occurred while recording edits.
     *                  Otherwise unchanged.
     * @return true if U_FAILURE(outErrorCode)
     * @stable ICU 59
     */
    UBool copy_Error_منزل(UError_کوډ &بهر_Error_کوډ) ثابت;

    /**
     * How much longer is the new text compared with the old text?
     * @return new length minus old length
     * @stable ICU 59
     */
    int32_t اوږدتیا_Delta() ثابت { أرجع delta; }
    /**
     * @return true if there are any change edits
     * @stable ICU 59
     */
    UBool hasChanges() ثابت { أرجع عدد_Changes != 0; }

    /**
     * @return the number of change edits
     * @stable ICU 60
     */
    int32_t شمېره_د_Changes() ثابت { أرجع عدد_Changes; }

    /**
     * Access to the list of edits.
     *
     * At any moment in time, an instance of this class points to a single edit: a "window" into a span
     * of the source string and the corresponding span of the destination string. The source string span
     * starts at {@link #sourceIndex()} and runs for {@link #oldLength()} chars; the destination string
     * span starts at {@link #destinationIndex()} and runs for {@link #newLength()} chars.
     *
     * The iterator can be moved between edits using the `next()`, `findSourceIndex(int32_t, UErrorCode &)`,
     * and `findDestinationIndex(int32_t, UErrorCode &)` methods.
     * Calling any of these methods mutates the iterator to make it point to the corresponding edit.
     *
     * For more information, see the documentation for {@link Edits}.
     *
     * @see getCoarseIterator
     * @see getFineIterator
     * @stable ICU 59
     */
    بنية U_COMMON_API Iterator final : عام UMemory {
        /**
         * Default constructor, empty iterator.
         * @stable ICU 60
         */
        Iterator() :
                array(nullptr), شاخص(0), اوږدتیا(0),
                پاتې_شوی(0), onlyChanges_(false), coarse(false),
                dir(0), بدل_شوی(false), old_اوږدتیا_2(0), new_اوږدتیا_2(0),
                src_شاخص(0), repl_شاخص(0), dest_شاخص(0) {}
        /**
         * Copy constructor.
         * @stable ICU 59
         */
        Iterator(ثابت Iterator &other) = افتراضي;
        /**
         * Assignment operator.
         * @stable ICU 59
         */
        Iterator& چلوونکی=(ثابت Iterator& other) U_LIFETIME_BOUND = افتراضي;

        /**
         * Advances the iterator to the next edit.
         * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
         *                  or else the function returns immediately. Check for U_FAILURE()
         *                  on output or use with function chaining. (See User Guide for details.)
         * @return true if there is another edit
         * @stable ICU 59
         */
        UBool بل(UError_کوډ &error_کوډ) { أرجع بل(onlyChanges_, error_کوډ); }

        /**
         * Moves the iterator to the edit that contains the source index.
         * The source index may be found in a no-change edit
         * even if normal iteration would skip no-change edits.
         * Normal iteration can continue from a found edit.
         *
         * The iterator state before this search logically does not matter.
         * (It may affect the performance of the search.)
         *
         * The iterator state after this search is undefined
         * if the source index is out of bounds for the source string.
         *
         * @param i source index
         * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
         *                  or else the function returns immediately. Check for U_FAILURE()
         *                  on output or use with function chaining. (See User Guide for details.)
         * @return true if the edit for the source index was found
         * @stable ICU 59
         */
        UBool پیدا_کړه_سرچینه_شاخص(int32_t i, UError_کوډ &error_کوډ) {
            أرجع پیدا_کړه_شاخص(i, true, error_کوډ) == 0;
        }

        /**
         * Moves the iterator to the edit that contains the destination index.
         * The destination index may be found in a no-change edit
         * even if normal iteration would skip no-change edits.
         * Normal iteration can continue from a found edit.
         *
         * The iterator state before this search logically does not matter.
         * (It may affect the performance of the search.)
         *
         * The iterator state after this search is undefined
         * if the source index is out of bounds for the source string.
         *
         * @param i destination index
         * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
         *                  or else the function returns immediately. Check for U_FAILURE()
         *                  on output or use with function chaining. (See User Guide for details.)
         * @return true if the edit for the destination index was found
         * @stable ICU 60
         */
        UBool پیدا_کړه_Destination_شاخص(int32_t i, UError_کوډ &error_کوډ) {
            أرجع پیدا_کړه_شاخص(i, false, error_کوډ) == 0;
        }

        /**
         * Computes the destination index corresponding to the given source index.
         * If the source index is inside a change edit (not at its start),
         * then the destination index at the end of that edit is returned,
         * since there is no information about index mapping inside a change edit.
         *
         * (This means that indexes to the start and middle of an edit,
         * for example around a grapheme cluster, are mapped to indexes
         * encompassing the entire edit.
         * The alternative, mapping an interior index to the start,
         * would map such an interval to an empty one.)
         *
         * This operation will usually but not always modify this object.
         * The iterator state after this search is undefined.
         *
         * @param i source index
         * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
         *                  or else the function returns immediately. Check for U_FAILURE()
         *                  on output or use with function chaining. (See User Guide for details.)
         * @return destination index; undefined if i is not 0..string length
         * @stable ICU 60
         */
        int32_t destination_شاخص_From_سرچینه_شاخص(int32_t i, UError_کوډ &error_کوډ);

        /**
         * Computes the source index corresponding to the given destination index.
         * If the destination index is inside a change edit (not at its start),
         * then the source index at the end of that edit is returned,
         * since there is no information about index mapping inside a change edit.
         *
         * (This means that indexes to the start and middle of an edit,
         * for example around a grapheme cluster, are mapped to indexes
         * encompassing the entire edit.
         * The alternative, mapping an interior index to the start,
         * would map such an interval to an empty one.)
         *
         * This operation will usually but not always modify this object.
         * The iterator state after this search is undefined.
         *
         * @param i destination index
         * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
         *                  or else the function returns immediately. Check for U_FAILURE()
         *                  on output or use with function chaining. (See User Guide for details.)
         * @return source index; undefined if i is not 0..string length
         * @stable ICU 60
         */
        int32_t سرچینه_شاخص_From_Destination_شاخص(int32_t i, UError_کوډ &error_کوډ);

        /**
         * Returns whether the edit currently represented by the iterator is a change edit.
         *
         * @return true if this edit replaces oldLength() units with newLength() different ones.
         *         false if oldLength units remain unchanged.
         * @stable ICU 59
         */
        UBool hasChange() ثابت { أرجع بدل_شوی; }

        /**
         * The length of the current span in the source string, which starts at {@link #sourceIndex}.
         *
         * @return the number of units in the original string which are replaced or remain unchanged.
         * @stable ICU 59
         */
        int32_t old_اوږدتیا() ثابت { أرجع old_اوږدتیا_2; }

        /**
         * The length of the current span in the destination string, which starts at
         * {@link #destinationIndex}, or in the replacement string, which starts at
         * {@link #replacementIndex}.
         *
         * @return the number of units in the modified string, if hasChange() is true.
         *         Same as oldLength if hasChange() is false.
         * @stable ICU 59
         */
        int32_t new_اوږدتیا() ثابت { أرجع new_اوږدتیا_2; }

        /**
         * The start index of the current span in the source string; the span has length
         * {@link #oldLength}.
         *
         * @return the current index into the source string
         * @stable ICU 59
         */
        int32_t سرچینه_شاخص() ثابت { أرجع src_شاخص; }

        /**
         * The start index of the current span in the replacement string; the span has length
         * {@link #newLength}. Well-defined only if the current edit is a change edit.
         *
         * The *replacement string* is the concatenation of all substrings of the destination
         * string corresponding to change edits.
         *
         * This method is intended to be used together with operations that write only replacement
         * characters (e.g. operations specifying the \ref U_OMIT_UNCHANGED_TEXT option).
         * The source string can then be modified in-place.
         *
         * @return the current index into the replacement-characters-only string,
         *         not counting unchanged spans
         * @stable ICU 59
         */
        int32_t replacement_شاخص() ثابت {
            // TODO: Throw an exception if we aren't in a change edit?
            أرجع repl_شاخص;
        }

        /**
         * The start index of the current span in the destination string; the span has length
         * {@link #newLength}.
         *
         * @return the current index into the full destination string
         * @stable ICU 59
         */
        int32_t destination_شاخص() ثابت { أرجع dest_شاخص; }

#که_نه_تعریف_شوی U_HIDE_دننی_API
        /**
         * A string representation of the current edit represented by the iterator for debugging. You
         * should not depend on the contents of the return string.
         * @internal
         */
        UnicodeString& منزل_String(UnicodeString& append_منزل U_LIFETIME_BOUND) ثابت;
#د_که_پای  // U_HIDE_INTERNAL_API

    خاص:
        ملګری class Edits;

        Iterator(ثابت uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs);

        int32_t read_اوږدتیا(int32_t سر);
        فراغ update_بل_Indexes();
        فراغ update_پخوانی_Indexes();
        UBool no_بل();
        UBool بل(UBool onlyChanges, UError_کوډ &error_کوډ);
        UBool پخوانی(UError_کوډ &error_کوډ);
        /** @return -1: error or i<0; 0: found; 1: i>=string length */
        int32_t پیدا_کړه_شاخص(int32_t i, UBool پیدا_کړه_سرچینه, UError_کوډ &error_کوډ);

        ثابت uint16_t *array;
        int32_t شاخص, اوږدتیا;
        // 0 if we are not within compressed equal-length changes.
        // Otherwise the number of remaining changes, including the current one.
        int32_t پاتې_شوی;
        UBool onlyChanges_, coarse;

        int8_t dir;  // iteration direction: back(<0), initial(0), forward(>0)
        UBool بدل_شوی;
        int32_t old_اوږدتیا_2, new_اوږدتیا_2;
        int32_t src_شاخص, repl_شاخص, dest_شاخص;
    };

    /**
     * Returns an Iterator for coarse-grained change edits
     * (adjacent change edits are treated as one).
     * Can be used to perform simple string updates.
     * Skips no-change edits.
     * @return an Iterator that merges adjacent changes.
     * @stable ICU 59
     */
    Iterator getCoarseChangesIterator() ثابت U_LIFETIME_BOUND {
        أرجع Iterator(array, اوږدتیا, true, true);
    }

    /**
     * Returns an Iterator for coarse-grained change and no-change edits
     * (adjacent change edits are treated as one).
     * Can be used to perform simple string updates.
     * Adjacent change edits are treated as one edit.
     * @return an Iterator that merges adjacent changes.
     * @stable ICU 59
     */
    Iterator getCoarseIterator() ثابت U_LIFETIME_BOUND {
        أرجع Iterator(array, اوږدتیا, false, true);
    }

    /**
     * Returns an Iterator for fine-grained change edits
     * (full granularity of change edits is retained).
     * Can be used for modifying styled text.
     * Skips no-change edits.
     * @return an Iterator that separates adjacent changes.
     * @stable ICU 59
     */
    Iterator getFineChangesIterator() ثابت U_LIFETIME_BOUND {
        أرجع Iterator(array, اوږدتیا, true, false);
    }

    /**
     * Returns an Iterator for fine-grained change and no-change edits
     * (full granularity of change edits is retained).
     * Can be used for modifying styled text.
     * @return an Iterator that separates adjacent changes.
     * @stable ICU 59
     */
    Iterator getFineIterator() ثابت U_LIFETIME_BOUND {
        أرجع Iterator(array, اوږدتیا, false, false);
    }

    /**
     * Merges the two input Edits and appends the result to this object.
     *
     * Consider two string transformations (for example, normalization and case mapping)
     * where each records Edits in addition to writing an output string.<br>
     * Edits ab reflect how substrings of input string a
     * map to substrings of intermediate string b.<br>
     * Edits bc reflect how substrings of intermediate string b
     * map to substrings of output string c.<br>
     * This function merges ab and bc such that the additional edits
     * recorded in this object reflect how substrings of input string a
     * map to substrings of output string c.
     *
     * If unrelated Edits are passed in where the output string of the first
     * has a different length than the input string of the second,
     * then a U_ILLEGAL_ARGUMENT_ERROR is reported.
     *
     * @param ab reflects how substrings of input string a
     *     map to substrings of intermediate string b.
     * @param bc reflects how substrings of intermediate string b
     *     map to substrings of output string c.
     * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
     *                  or else the function returns immediately. Check for U_FAILURE()
     *                  on output or use with function chaining. (See User Guide for details.)
     * @return *this, with the merged edits appended
     * @stable ICU 60
     */
    Edits& یوځای_And_Append(ثابت Edits& ab, ثابت Edits& bc, UError_کوډ& error_کوډ) U_LIFETIME_BOUND;

خاص:
    فراغ releaseArray() noexcept;
    Edits& copyArray(ثابت Edits& other) U_LIFETIME_BOUND;
    Edits& moveArray(Edits& src) noexcept U_LIFETIME_BOUND;

    فراغ سېټ_وروستی_Unit(int32_t وروستی) { array[اوږدتیا - 1] = static_cast<uint16_t>(وروستی); }
    int32_t وروستی_Unit() ثابت { أرجع اوږدتیا > 0 ? array[اوږدتیا - 1] : 0xffff; }

    فراغ append(int32_t r);
    UBool growArray();

    ساكن ثابت int32_t ډېری_ظرفیت_2 = 100;
    uint16_t *array;
    int32_t ظرفیت;
    int32_t اوږدتیا;
    int32_t delta;
    int32_t عدد_Changes;
    UError_کوډ error_کوډ_2;
    uint16_t ډېری_Array[ډېری_ظرفیت_2];
};

U_NAMESPACE_پای_ټکی

#د_که_پای /* U_SHOW_CPLUSPLUS_API */

#د_که_پای  // __EDITS_H__
