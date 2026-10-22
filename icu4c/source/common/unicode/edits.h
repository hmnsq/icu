// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// edits.h
// created: 2016dec30 Markus W. Scherer

#جيڪڏهن_وصف_نه_ٿيل __EDITS_H__
#وصف_ڪريو __EDITS_H__

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
            array(ڍير_Array), صلاحيت(ڍير_صلاحيت_2), ڊيگهائي(0), delta(0), انگ_Changes(0),
            error_ڪوڊ_2(U_ZERO_ERROR) {}
    /**
     * Copy constructor.
     * @param other source edits
     * @stable ICU 60
     */
    Edits(ثابت Edits &other) :
            array(ڍير_Array), صلاحيت(ڍير_صلاحيت_2), ڊيگهائي(other.ڊيگهائي),
            delta(other.delta), انگ_Changes(other.انگ_Changes),
            error_ڪوڊ_2(other.error_ڪوڊ_2) {
        copyArray(other);
    }
    /**
     * Move constructor, might leave src empty.
     * This object will have the same contents that the source object had.
     * @param src source edits
     * @stable ICU 60
     */
    Edits(Edits &&src) noexcept :
            array(ڍير_Array), صلاحيت(ڍير_صلاحيت_2), ڊيگهائي(src.ڊيگهائي),
            delta(src.delta), انگ_Changes(src.انگ_Changes),
            error_ڪوڊ_2(src.error_ڪوڊ_2) {
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
    Edits& عامل=(ثابت Edits& other) U_LIFETIME_BOUND;

    /**
     * Move assignment operator, might leave src empty.
     * This object will have the same contents that the source object had.
     * The behavior is undefined if *this and src are the same object.
     * @param src source edits
     * @return *this
     * @stable ICU 60
     */
    Edits& عامل=(Edits&& src) noexcept U_LIFETIME_BOUND;

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
    فراغ جوڙيو_Unchanged(int32_t unchanged_ڊيگهائي);
    /**
     * Adds a change edit: a record for a text replacement/insertion/deletion.
     * Normally called from inside ICU string transformation functions, not user code.
     * @stable ICU 59
     */
    فراغ جوڙيو_Replace(int32_t old_ڊيگهائي, int32_t new_ڊيگهائي);
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
    UBool copy_Error_منزل(UError_ڪوڊ &ٻاهريون_Error_ڪوڊ) ثابت;

    /**
     * How much longer is the new text compared with the old text?
     * @return new length minus old length
     * @stable ICU 59
     */
    int32_t ڊيگهائي_Delta() ثابت { أرجع delta; }
    /**
     * @return true if there are any change edits
     * @stable ICU 59
     */
    UBool hasChanges() ثابت { أرجع انگ_Changes != 0; }

    /**
     * @return the number of change edits
     * @stable ICU 60
     */
    int32_t عدد_جو_Changes() ثابت { أرجع انگ_Changes; }

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
                array(nullptr), اشاريو(0), ڊيگهائي(0),
                بچيل(0), onlyChanges_(false), coarse(false),
                dir(0), بدليو(false), old_ڊيگهائي_2(0), new_ڊيگهائي_2(0),
                src_اشاريو(0), repl_اشاريو(0), dest_اشاريو(0) {}
        /**
         * Copy constructor.
         * @stable ICU 59
         */
        Iterator(ثابت Iterator &other) = افتراضي;
        /**
         * Assignment operator.
         * @stable ICU 59
         */
        Iterator& عامل=(ثابت Iterator& other) U_LIFETIME_BOUND = افتراضي;

        /**
         * Advances the iterator to the next edit.
         * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
         *                  or else the function returns immediately. Check for U_FAILURE()
         *                  on output or use with function chaining. (See User Guide for details.)
         * @return true if there is another edit
         * @stable ICU 59
         */
        UBool ايندڙ(UError_ڪوڊ &error_ڪوڊ) { أرجع ايندڙ(onlyChanges_, error_ڪوڊ); }

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
        UBool ڳوليو_ذريعو_اشاريو(int32_t i, UError_ڪوڊ &error_ڪوڊ) {
            أرجع ڳوليو_اشاريو(i, true, error_ڪوڊ) == 0;
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
        UBool ڳوليو_Destination_اشاريو(int32_t i, UError_ڪوڊ &error_ڪوڊ) {
            أرجع ڳوليو_اشاريو(i, false, error_ڪوڊ) == 0;
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
        int32_t destination_اشاريو_From_ذريعو_اشاريو(int32_t i, UError_ڪوڊ &error_ڪوڊ);

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
        int32_t ذريعو_اشاريو_From_Destination_اشاريو(int32_t i, UError_ڪوڊ &error_ڪوڊ);

        /**
         * Returns whether the edit currently represented by the iterator is a change edit.
         *
         * @return true if this edit replaces oldLength() units with newLength() different ones.
         *         false if oldLength units remain unchanged.
         * @stable ICU 59
         */
        UBool hasChange() ثابت { أرجع بدليو; }

        /**
         * The length of the current span in the source string, which starts at {@link #sourceIndex}.
         *
         * @return the number of units in the original string which are replaced or remain unchanged.
         * @stable ICU 59
         */
        int32_t old_ڊيگهائي() ثابت { أرجع old_ڊيگهائي_2; }

        /**
         * The length of the current span in the destination string, which starts at
         * {@link #destinationIndex}, or in the replacement string, which starts at
         * {@link #replacementIndex}.
         *
         * @return the number of units in the modified string, if hasChange() is true.
         *         Same as oldLength if hasChange() is false.
         * @stable ICU 59
         */
        int32_t new_ڊيگهائي() ثابت { أرجع new_ڊيگهائي_2; }

        /**
         * The start index of the current span in the source string; the span has length
         * {@link #oldLength}.
         *
         * @return the current index into the source string
         * @stable ICU 59
         */
        int32_t ذريعو_اشاريو() ثابت { أرجع src_اشاريو; }

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
        int32_t replacement_اشاريو() ثابت {
            // TODO: Throw an exception if we aren't in a change edit?
            أرجع repl_اشاريو;
        }

        /**
         * The start index of the current span in the destination string; the span has length
         * {@link #newLength}.
         *
         * @return the current index into the full destination string
         * @stable ICU 59
         */
        int32_t destination_اشاريو() ثابت { أرجع dest_اشاريو; }

#جيڪڏهن_وصف_نه_ٿيل U_HIDE_اندروني_API
        /**
         * A string representation of the current edit represented by the iterator for debugging. You
         * should not depend on the contents of the return string.
         * @internal
         */
        UnicodeString& منزل_String(UnicodeString& append_منزل U_LIFETIME_BOUND) ثابت;
#جيڪڏهن_پورو  // U_HIDE_INTERNAL_API

    خاص:
        دوست class Edits;

        Iterator(ثابت uint16_t* a U_LIFETIME_BOUND, int32_t len, UBool oc, UBool crs);

        int32_t read_ڊيگهائي(int32_t سر);
        فراغ update_ايندڙ_Indexes();
        فراغ update_اڳيون_Indexes();
        UBool no_ايندڙ();
        UBool ايندڙ(UBool onlyChanges, UError_ڪوڊ &error_ڪوڊ);
        UBool اڳيون(UError_ڪوڊ &error_ڪوڊ);
        /** @return -1: error or i<0; 0: found; 1: i>=string length */
        int32_t ڳوليو_اشاريو(int32_t i, UBool ڳوليو_ذريعو, UError_ڪوڊ &error_ڪوڊ);

        ثابت uint16_t *array;
        int32_t اشاريو, ڊيگهائي;
        // 0 if we are not within compressed equal-length changes.
        // Otherwise the number of remaining changes, including the current one.
        int32_t بچيل;
        UBool onlyChanges_, coarse;

        int8_t dir;  // iteration direction: back(<0), initial(0), forward(>0)
        UBool بدليو;
        int32_t old_ڊيگهائي_2, new_ڊيگهائي_2;
        int32_t src_اشاريو, repl_اشاريو, dest_اشاريو;
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
        أرجع Iterator(array, ڊيگهائي, true, true);
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
        أرجع Iterator(array, ڊيگهائي, false, true);
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
        أرجع Iterator(array, ڊيگهائي, true, false);
    }

    /**
     * Returns an Iterator for fine-grained change and no-change edits
     * (full granularity of change edits is retained).
     * Can be used for modifying styled text.
     * @return an Iterator that separates adjacent changes.
     * @stable ICU 59
     */
    Iterator getFineIterator() ثابت U_LIFETIME_BOUND {
        أرجع Iterator(array, ڊيگهائي, false, false);
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
    Edits& ملايو_And_Append(ثابت Edits& ab, ثابت Edits& bc, UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND;

خاص:
    فراغ releaseArray() noexcept;
    Edits& copyArray(ثابت Edits& other) U_LIFETIME_BOUND;
    Edits& moveArray(Edits& src) noexcept U_LIFETIME_BOUND;

    فراغ سيٽ_آخري_Unit(int32_t آخري) { array[ڊيگهائي - 1] = static_cast<uint16_t>(آخري); }
    int32_t آخري_Unit() ثابت { أرجع ڊيگهائي > 0 ? array[ڊيگهائي - 1] : 0xffff; }

    فراغ append(int32_t r);
    UBool growArray();

    ساكن ثابت int32_t ڍير_صلاحيت_2 = 100;
    uint16_t *array;
    int32_t صلاحيت;
    int32_t ڊيگهائي;
    int32_t delta;
    int32_t انگ_Changes;
    UError_ڪوڊ error_ڪوڊ_2;
    uint16_t ڍير_Array[ڍير_صلاحيت_2];
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* U_SHOW_CPLUSPLUS_API */

#جيڪڏهن_پورو  // __EDITS_H__
