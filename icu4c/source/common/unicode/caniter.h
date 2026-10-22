// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 * Copyright (C) 1996-2014, International Business Machines Corporation and
 * others. All Rights Reserved.
 *******************************************************************************
 */

#अगर_अघोषित CANITER_H
#घोषणा CANITER_H

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

#अगर !UCONFIG_NO_NORMALIZATION

#समावेश "unicode/uobject.h"
#समावेश "unicode/unistr.h"

/**
 * \file
 * \brief C++ API: Canonical Iterator
 */
 
/** Should permutation skip characters with combining class zero
 *  Should be either true or false. This is a compile time option
 *  @stable ICU 2.4
 */
#अगर_अघोषित CANITER_SKIP_ZEROES
#घोषणा CANITER_SKIP_ZEROES true
#पूर्ण_अगर

U_NAMESPACE_BEGIN

class Hashtable;
class Normalizer2;
class Normalizer2Impl;

/**
 * This class allows one to iterate through all the strings that are canonically equivalent to a given
 * string. For example, here are some sample results:
Results for: {LATIN CAPITAL LETTER A WITH RING ABOVE}{LATIN SMALL LETTER D}{COMBINING DOT ABOVE}{COMBINING CEDILLA}
1: \\u0041\\u030A\\u0064\\u0307\\u0327
 = {LATIN CAPITAL LETTER A}{COMBINING RING ABOVE}{LATIN SMALL LETTER D}{COMBINING DOT ABOVE}{COMBINING CEDILLA}
2: \\u0041\\u030A\\u0064\\u0327\\u0307
 = {LATIN CAPITAL LETTER A}{COMBINING RING ABOVE}{LATIN SMALL LETTER D}{COMBINING CEDILLA}{COMBINING DOT ABOVE}
3: \\u0041\\u030A\\u1E0B\\u0327
 = {LATIN CAPITAL LETTER A}{COMBINING RING ABOVE}{LATIN SMALL LETTER D WITH DOT ABOVE}{COMBINING CEDILLA}
4: \\u0041\\u030A\\u1E11\\u0307
 = {LATIN CAPITAL LETTER A}{COMBINING RING ABOVE}{LATIN SMALL LETTER D WITH CEDILLA}{COMBINING DOT ABOVE}
5: \\u00C5\\u0064\\u0307\\u0327
 = {LATIN CAPITAL LETTER A WITH RING ABOVE}{LATIN SMALL LETTER D}{COMBINING DOT ABOVE}{COMBINING CEDILLA}
6: \\u00C5\\u0064\\u0327\\u0307
 = {LATIN CAPITAL LETTER A WITH RING ABOVE}{LATIN SMALL LETTER D}{COMBINING CEDILLA}{COMBINING DOT ABOVE}
7: \\u00C5\\u1E0B\\u0327
 = {LATIN CAPITAL LETTER A WITH RING ABOVE}{LATIN SMALL LETTER D WITH DOT ABOVE}{COMBINING CEDILLA}
8: \\u00C5\\u1E11\\u0307
 = {LATIN CAPITAL LETTER A WITH RING ABOVE}{LATIN SMALL LETTER D WITH CEDILLA}{COMBINING DOT ABOVE}
9: \\u212B\\u0064\\u0307\\u0327
 = {ANGSTROM SIGN}{LATIN SMALL LETTER D}{COMBINING DOT ABOVE}{COMBINING CEDILLA}
10: \\u212B\\u0064\\u0327\\u0307
 = {ANGSTROM SIGN}{LATIN SMALL LETTER D}{COMBINING CEDILLA}{COMBINING DOT ABOVE}
11: \\u212B\\u1E0B\\u0327
 = {ANGSTROM SIGN}{LATIN SMALL LETTER D WITH DOT ABOVE}{COMBINING CEDILLA}
12: \\u212B\\u1E11\\u0307
 = {ANGSTROM SIGN}{LATIN SMALL LETTER D WITH CEDILLA}{COMBINING DOT ABOVE}
 *<br>Note: the code is intended for use with small strings, and is not suitable for larger ones,
 * since it has not been optimized for that situation.
 * Note, CanonicalIterator is not intended to be subclassed.
 * @author M. Davis
 * @author C++ port by V. Weinstein
 * @stable ICU 2.4
 */
class U_COMMON_API CanonicalIterator final : खुला UObject {
खुला:
    /**
     * Construct a CanonicalIterator object
     * @param source    string to get results for
     * @param status    Fill-in parameter which receives the status of this operation.
     * @stable ICU 2.4
     */
    CanonicalIterator(स्थिर UnicodeString &source, UErrorCode &status);

    /** Destructor
     *  Cleans pieces
     * @stable ICU 2.4
     */
    भव ~CanonicalIterator();

    /**
     * Gets the NFD form of the current source we are iterating over.
     * @return gets the source: NOTE: it is the NFD form of source
     * @stable ICU 2.4
     */
    UnicodeString getSource();

    /**
     * Resets the iterator so that one can start again from the beginning.
     * @stable ICU 2.4
     */
    व्योम reset();

    /**
     * Get the next canonically equivalent string.
     * <br><b>Warning: The strings are not guaranteed to be in any particular order.</b>
     * @return the next string that is canonically equivalent. A bogus string is returned when
     * the iteration is done.
     * @stable ICU 2.4
     */
    UnicodeString अग्र();

    /**
     * Set a new source for this iterator. Allows object reuse.
     * @param newSource     the source string to iterate against. This allows the same iterator to be used
     *                     while changing the source string, saving object creation.
     * @param status        Fill-in parameter which receives the status of this operation.
     * @stable ICU 2.4
     */
    व्योम setSource(स्थिर UnicodeString &newSource, UErrorCode &status);

#अगर_अघोषित U_HIDE_INTERNAL_API
    /**
     * Dumb recursive implementation of permutation.
     * TODO: optimize
     * @param source     the string to find permutations for
     * @param skipZeros  determine if skip zeros
     * @param result     the results in a set.
     * @param status       Fill-in parameter which receives the status of this operation.
     * @param depth     depth of the call.
     * @internal
     */
    अटल व्योम U_EXPORT2 permute(UnicodeString &source, UBool skipZeros, Hashtable *परिणाम, UErrorCode &status, int32_t depth=0);
#पूर्ण_अगर  /* U_HIDE_INTERNAL_API */

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    अटल UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.2
     */
    भव UClassID getDynamicClassID() स्थिर override;

निजी:
    // ===================== PRIVATES ==============================
    // private default constructor
    CanonicalIterator() = delete;


    /**
     * Copy constructor. Private for now.
     */
    CanonicalIterator(स्थिर CanonicalIterator& other) = delete;

    /**
     * Assignment operator. Private for now.
     */
    CanonicalIterator& चालक=(स्थिर CanonicalIterator& other) = delete;

    // fields
    UnicodeString source;
    UBool done;

    // 2 dimensional array holds the pieces of the string with
    // their different canonically equivalent representations
    UnicodeString **pieces;
    int32_t pieces_लंबाई;
    int32_t *pieces_lengths;

    // current is used in iterating to combine pieces
    int32_t *current;
    int32_t current_लंबाई;

    // transient fields
    UnicodeString बफर;

    स्थिर Normalizer2 *nfd;
    स्थिर Normalizer2Impl *nfcImpl;

    // we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
    UnicodeString *getEquivalents(स्थिर UnicodeString &segment, int32_t &परिणाम_len, UErrorCode &status); //private String[] getEquivalents(String segment)

    //Set getEquivalents2(String segment);
    Hashtable* getEquivalents2(Hashtable* fillin_परिणाम U_LIFETIME_BOUND,
                               स्थिर char16_t* segment,
                               int32_t segLen,
                               UErrorCode& status);
    //Hashtable *getEquivalents2(const UnicodeString &segment, int32_t segLen, UErrorCode &status);

    /**
     * See if the decomposition of cp2 is at segment starting at segmentPos
     * (with canonical rearrangement!)
     * If so, take the remainder, and return the equivalents
     */
    //Set extract(int comp, String segment, int segmentPos, StringBuffer buffer);
    Hashtable* extract(Hashtable* fillin_परिणाम U_LIFETIME_BOUND,
                       UChar32 comp,
                       स्थिर char16_t* segment,
                       int32_t segLen,
                       int32_t segmentPos,
                       UErrorCode& status);
    //Hashtable *extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status);

    व्योम cleanPieces();

};

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_NORMALIZATION */

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */

#पूर्ण_अगर
