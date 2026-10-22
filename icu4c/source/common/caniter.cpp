// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *****************************************************************************
 * Copyright (C) 1996-2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 *****************************************************************************
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_NORMALIZATION

#ಅನ್ತರ್ಭಾವ "unicode/caniter.h"
#ಅನ್ತರ್ಭಾವ "unicode/normalizer2.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/usetiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "normalizer2impl.h"

/**
 * This class allows one to iterate through all the strings that are canonically equivalent to a given
 * string. For example, here are some sample results:
Results for: {LATIN CAPITAL LETTER A WITH RING ABOVE}{LATIN SMALL LETTER D}{COMBINING DOT ABOVE}{COMBINING CEDILLA}
1: \u0041\u030A\u0064\u0307\u0327
 = {LATIN CAPITAL LETTER A}{COMBINING RING ABOVE}{LATIN SMALL LETTER D}{COMBINING DOT ABOVE}{COMBINING CEDILLA}
2: \u0041\u030A\u0064\u0327\u0307
 = {LATIN CAPITAL LETTER A}{COMBINING RING ABOVE}{LATIN SMALL LETTER D}{COMBINING CEDILLA}{COMBINING DOT ABOVE}
3: \u0041\u030A\u1E0B\u0327
 = {LATIN CAPITAL LETTER A}{COMBINING RING ABOVE}{LATIN SMALL LETTER D WITH DOT ABOVE}{COMBINING CEDILLA}
4: \u0041\u030A\u1E11\u0307
 = {LATIN CAPITAL LETTER A}{COMBINING RING ABOVE}{LATIN SMALL LETTER D WITH CEDILLA}{COMBINING DOT ABOVE}
5: \u00C5\u0064\u0307\u0327
 = {LATIN CAPITAL LETTER A WITH RING ABOVE}{LATIN SMALL LETTER D}{COMBINING DOT ABOVE}{COMBINING CEDILLA}
6: \u00C5\u0064\u0327\u0307
 = {LATIN CAPITAL LETTER A WITH RING ABOVE}{LATIN SMALL LETTER D}{COMBINING CEDILLA}{COMBINING DOT ABOVE}
7: \u00C5\u1E0B\u0327
 = {LATIN CAPITAL LETTER A WITH RING ABOVE}{LATIN SMALL LETTER D WITH DOT ABOVE}{COMBINING CEDILLA}
8: \u00C5\u1E11\u0307
 = {LATIN CAPITAL LETTER A WITH RING ABOVE}{LATIN SMALL LETTER D WITH CEDILLA}{COMBINING DOT ABOVE}
9: \u212B\u0064\u0307\u0327
 = {ANGSTROM SIGN}{LATIN SMALL LETTER D}{COMBINING DOT ABOVE}{COMBINING CEDILLA}
10: \u212B\u0064\u0327\u0307
 = {ANGSTROM SIGN}{LATIN SMALL LETTER D}{COMBINING CEDILLA}{COMBINING DOT ABOVE}
11: \u212B\u1E0B\u0327
 = {ANGSTROM SIGN}{LATIN SMALL LETTER D WITH DOT ABOVE}{COMBINING CEDILLA}
12: \u212B\u1E11\u0307
 = {ANGSTROM SIGN}{LATIN SMALL LETTER D WITH CEDILLA}{COMBINING DOT ABOVE}
 *<br>Note: the code is intended for use with small strings, and is not suitable for larger ones,
 * since it has not been optimized for that situation.
 *@author M. Davis
 *@draft
 */

//ಬಹಿರಂಗ

U_NAMESPACE_BEGIN

// TODO: add boilerplate methods.

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(CanonicalIterator)


/**
 *@param source string to get results for
 */
CanonicalIterator::CanonicalIterator(ಸ್ಥಿರಾಂಕ UnicodeString &ಮೂಲ_Str, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
    pieces(ಶೂನ್ಯಸೂಚಕ),
    pieces_ಉದ್ದಳತೆ(0),
    pieces_lengths(ಶೂನ್ಯಸೂಚಕ),
    current(ಶೂನ್ಯಸೂಚಕ),
    current_ಉದ್ದಳತೆ(0),
    nfd(Normalizer2::getNFDInstance(ಸ್ಥಿತಿ_2)),
    nfcImpl(Normalizer2Factory::getNFCImpl(ಸ್ಥಿತಿ_2))
{
    ಆದರೆ(U_SUCCESS(ಸ್ಥಿತಿ_2) && nfcImpl->ensure_Canon_Iter_ದತ್ತಾಂಶ(ಸ್ಥಿತಿ_2)) {
      ಗಣ_ಮೂಲ(ಮೂಲ_Str, ಸ್ಥಿತಿ_2);
    }
}

CanonicalIterator::~CanonicalIterator() {
  cleanPieces();
}

ಶೂನ್ಯ CanonicalIterator::cleanPieces() {
    int32_t i = 0;
    ಆದರೆ(pieces != ಶೂನ್ಯಸೂಚಕ) {
        ಗಾಗಿ(i = 0; i < pieces_ಉದ್ದಳತೆ; i++) {
            ಆದರೆ(pieces[i] != ಶೂನ್ಯಸೂಚಕ) {
                ಅಳಿಸು[] pieces[i];
            }
        }
        uprv_free(pieces);
        pieces = ಶೂನ್ಯಸೂಚಕ;
        pieces_ಉದ್ದಳತೆ = 0;
    }
    ಆದರೆ(pieces_lengths != ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(pieces_lengths);
        pieces_lengths = ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(current != ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(current);
        current = ಶೂನ್ಯಸೂಚಕ;
        current_ಉದ್ದಳತೆ = 0;
    }
}

/**
 *@return gets the source: NOTE: it is the NFD form of source
 */
UnicodeString CanonicalIterator::get_ಮೂಲ() {
  ಹಿಂತಿರುಗಿ ಮೂಲ;
}

/**
 * Resets the iterator so that one can start again from the beginning.
 */
ಶೂನ್ಯ CanonicalIterator::reset() {
    ಮುಗಿದ = ಸುಳ್ಳು;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < current_ಉದ್ದಳತೆ; ++i) {
        current[i] = 0;
    }
}

/**
 *@return the next string that is canonically equivalent. The value null is returned when
 * the iteration is done.
 */
UnicodeString CanonicalIterator::next() {
    int32_t i = 0;

    ಆದರೆ (ಮುಗಿದ) {
      ಬಫರ್.ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ ಬಫರ್;
    }

    // delete old contents
    ಬಫರ್.remove();

    // construct return value

    ಗಾಗಿ (i = 0; i < pieces_ಉದ್ದಳತೆ; ++i) {
        ಬಫರ್.append(pieces[i][current[i]]);
    }
    //String result = buffer.toString(); // not needed

    // find next value for next time

    ಗಾಗಿ (i = current_ಉದ್ದಳತೆ - 1; ; --i) {
        ಆದರೆ (i < 0) {
            ಮುಗಿದ = ನಿಜ;
            ನಿಲ್ಲಿಸು;
        }
        current[i]++;
        ಆದರೆ (current[i] < pieces_lengths[i]) ನಿಲ್ಲಿಸು; // got sequence
        current[i] = 0;
    }
    ಹಿಂತಿರುಗಿ ಬಫರ್;
}

/**
 *@param set the source string to iterate against. This allows the same iterator to be used
 * while changing the source string, saving object creation.
 */
ಶೂನ್ಯ CanonicalIterator::ಗಣ_ಮೂಲ(ಸ್ಥಿರಾಂಕ UnicodeString &new_ಮೂಲ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    int32_t list_ಉದ್ದಳತೆ_2 = 0;
    UChar32 cp = 0;
    int32_t ಆರಂಭ = 0;
    int32_t i = 0;
    UnicodeString *list = ಶೂನ್ಯಸೂಚಕ;

    nfd->normalize(new_ಮೂಲ, ಮೂಲ, ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
      ಹಿಂತಿರುಗಿ;
    }
    ಮುಗಿದ = ಸುಳ್ಳು;

    cleanPieces();

    // catch degenerate case
    ಆದರೆ (new_ಮೂಲ.length() == 0) {
        pieces = static_cast<UnicodeString**>(uprv_malloc(ಗಾತ್ರ(UnicodeString*)));
        pieces_lengths = static_cast<int32_t*>(uprv_malloc(1 * ಗಾತ್ರ(int32_t)));
        pieces_ಉದ್ದಳತೆ = 1;
        current = static_cast<int32_t*>(uprv_malloc(1 * ಗಾತ್ರ(int32_t)));
        current_ಉದ್ದಳತೆ = 1;
        ಆದರೆ (pieces == ಶೂನ್ಯಸೂಚಕ || pieces_lengths == ಶೂನ್ಯಸೂಚಕ || current == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹೋಗು CleanPartialInitialization;
        }
        current[0] = 0;
        pieces[0] = ಹೊಸ UnicodeString[1];
        pieces_lengths[0] = 1;
        ಆದರೆ (pieces[0] == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹೋಗು CleanPartialInitialization;
        }
        ಹಿಂತಿರುಗಿ;
    }


    list = ಹೊಸ UnicodeString[ಮೂಲ.length()];
    ಆದರೆ (list == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹೋಗು CleanPartialInitialization;
    }

    // i should initially be the number of code units at the 
    // start of the string
    i = U16_ಉದ್ದಳತೆ(ಮೂಲ.char32At(0));
    // int32_t i = 1;
    // find the segments
    // This code iterates through the source string and 
    // extracts segments that end up on a codepoint that
    // doesn't start any decompositions. (Analysis is done
    // on the NFD form - see above).
    ಗಾಗಿ (; i < ಮೂಲ.length(); i += U16_ಉದ್ದಳತೆ(cp)) {
        cp = ಮೂಲ.char32At(i);
        ಆದರೆ (nfcImpl->isCanonSegmentStarter(cp)) {
            ಮೂಲ.extract(ಆರಂಭ, i-ಆರಂಭ, list[list_ಉದ್ದಳತೆ_2++]); // add up to i
            ಆರಂಭ = i;
        }
    }
    ಮೂಲ.extract(ಆರಂಭ, i-ಆರಂಭ, list[list_ಉದ್ದಳತೆ_2++]); // add last one


    // allocate the arrays, and find the strings that are CE to each segment
    pieces = static_cast<UnicodeString**>(uprv_malloc(list_ಉದ್ದಳತೆ_2 * ಗಾತ್ರ(UnicodeString*)));
    pieces_ಉದ್ದಳತೆ = list_ಉದ್ದಳತೆ_2;
    pieces_lengths = static_cast<int32_t*>(uprv_malloc(list_ಉದ್ದಳತೆ_2 * ಗಾತ್ರ(int32_t)));
    current = static_cast<int32_t*>(uprv_malloc(list_ಉದ್ದಳತೆ_2 * ಗಾತ್ರ(int32_t)));
    current_ಉದ್ದಳತೆ = list_ಉದ್ದಳತೆ_2;
    ಆದರೆ (pieces == ಶೂನ್ಯಸೂಚಕ || pieces_lengths == ಶೂನ್ಯಸೂಚಕ || current == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹೋಗು CleanPartialInitialization;
    }

    ಗಾಗಿ (i = 0; i < current_ಉದ್ದಳತೆ; i++) {
        current[i] = 0;
    }
    // for each segment, get all the combinations that can produce 
    // it after NFD normalization
    ಗಾಗಿ (i = 0; i < pieces_ಉದ್ದಳತೆ; ++i) {
        //if (PROGRESS) printf("SEGMENT\n");
        pieces[i] = getEquivalents(list[i], pieces_lengths[i], ಸ್ಥಿತಿ_2);
    }

    ಅಳಿಸು[] list;
    ಹಿಂತಿರುಗಿ;
// Common section to cleanup all local variables and reset object variables.
CleanPartialInitialization:
    ಅಳಿಸು[] list;
    cleanPieces();
}

/**
 * Dumb recursive implementation of permutation.
 * TODO: optimize
 * @param source the string to find permutations for
 * @return the results in a set.
 */
ಶೂನ್ಯ U_EXPORT2 CanonicalIterator::ಕ್ರಮಬದಲಾವಣೆ(UnicodeString &ಮೂಲ, UBool skipZeros, Hashtable *ಫಲಿತಾಂಶ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2, int32_t ಆಳ) {
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    // To avoid infinity loop caused by permute, we limit the depth of recursive
    // call to permute and return U_UNSUPPORTED_ERROR.
    // We know in some unit test we need at least 4. Set to 8 just in case some
    // unforseen use cases.
    constexpr int32_t k_ಕ್ರಮಬದಲಾವಣೆ_ಆಳ_ಎಲ್ಲೆ = 8;
    ಆದರೆ (ಆಳ > k_ಕ್ರಮಬದಲಾವಣೆ_ಆಳ_ಎಲ್ಲೆ) {
        ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    //if (PROGRESS) printf("Permute: %s\n", UToS(Tr(source)));
    int32_t i = 0;

    // optimization:
    // if zero or one character, just return a set with it
    // we check for length < 2 to keep from counting code points all the time
    ಆದರೆ (ಮೂಲ.length() <= 2 && ಮೂಲ.ಎಣಿಕೆ_Char32() <= 1) {
        UnicodeString *ಗಮ್ಯ_Put = ಹೊಸ UnicodeString(ಮೂಲ);
        /* test for nullptr */
        ಆದರೆ (ಗಮ್ಯ_Put == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        ಫಲಿತಾಂಶ->put(ಮೂಲ, ಗಮ್ಯ_Put, ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ;
    }

    // otherwise iterate through the string, and recursively permute all the other characters
    UChar32 cp;
    Hashtable subpermute(ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    subpermute.ಗಣ_ಮೌಲ್ಯ_Deleter(uprv_deleteUObject);

    ಗಾಗಿ (i = 0; i < ಮೂಲ.length(); i += U16_ಉದ್ದಳತೆ(cp)) {
        cp = ಮೂಲ.char32At(i);
        ಸ್ಥಿರಾಂಕ UHashElement *ne = ಶೂನ್ಯಸೂಚಕ;
        int32_t el = UHASH_ಮೊದಲ;
        UnicodeString sub_ಕ್ರಮಬದಲಾವಣೆ_String = ಮೂಲ;

        // optimization:
        // if the character is canonical combining class zero,
        // don't permute it
        ಆದರೆ (skipZeros && i != 0 && u_getCombiningClass(cp) == 0) {
            //System.out.println("Skipping " + Utility.hex(UTF16.valueOf(source, i)));
            ಮುಂದುವರಿ;
        }

        subpermute.removeAll();

        // see what the permutations of the characters before and after this one are
        //Hashtable *subpermute = permute(source.substring(0,i) + source.substring(i + UTF16.getCharCount(cp)));
        ಕ್ರಮಬದಲಾವಣೆ(sub_ಕ್ರಮಬದಲಾವಣೆ_String.remove(i, U16_ಉದ್ದಳತೆ(cp)), skipZeros, &subpermute, ಸ್ಥಿತಿ_2, ಆಳ+1);
        /* Test for buffer overflows */
        ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ;
        }
        // The upper remove is destructive. The question is do we have to make a copy, or we don't care about the contents 
        // of source at this point.

        // prefix this character to all of them
        ne = subpermute.ಮುಂದಿನ_Element(el);
        ಆಗ (ne != ಶೂನ್ಯಸೂಚಕ) {
            UnicodeString* permRes = static_cast<UnicodeString*>(ne->value.ಸೂಚಕ);
            UnicodeString *chStr = ಹೊಸ UnicodeString(cp);
            //test for nullptr
            ಆದರೆ (chStr == ಶೂನ್ಯಸೂಚಕ) {
                ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            chStr->append(*permRes); //*((UnicodeString *)(ne->value.pointer));
            //if (PROGRESS) printf("  Piece: %s\n", UToS(*chStr));
            ಫಲಿತಾಂಶ->put(*chStr, chStr, ಸ್ಥಿತಿ_2);
            ne = subpermute.ಮುಂದಿನ_Element(el);
        }
    }
    //return result;
}

// privates

// we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
UnicodeString* CanonicalIterator::getEquivalents(ಸ್ಥಿರಾಂಕ UnicodeString &segment, int32_t &ಫಲಿತಾಂಶ_len, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    Hashtable ಫಲಿತಾಂಶ(ಸ್ಥಿತಿ_2);
    Hashtable permutations(ಸ್ಥಿತಿ_2);
    Hashtable basic(ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಫಲಿತಾಂಶ.ಗಣ_ಮೌಲ್ಯ_Deleter(uprv_deleteUObject);
    permutations.ಗಣ_ಮೌಲ್ಯ_Deleter(uprv_deleteUObject);
    basic.ಗಣ_ಮೌಲ್ಯ_Deleter(uprv_deleteUObject);

    char16_t USeg[256];
    int32_t segLen = segment.extract(USeg, 256, ಸ್ಥಿತಿ_2);
    getEquivalents2(&basic, USeg, segLen, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    // now get all the permutations
    // add only the ones that are canonically equivalent
    // TODO: optimize by not permuting any class zero.

    ಸ್ಥಿರಾಂಕ UHashElement *ne = ಶೂನ್ಯಸೂಚಕ;
    int32_t el = UHASH_ಮೊದಲ;
    //Iterator it = basic.iterator();
    ne = basic.ಮುಂದಿನ_Element(el);
    //while (it.hasNext())
    ಆಗ (ne != ಶೂನ್ಯಸೂಚಕ) {
        //String item = (String) it.next();
        UnicodeString ಅಂಶ = *static_cast<UnicodeString*>(ne->value.ಸೂಚಕ);

        permutations.removeAll();
        ಕ್ರಮಬದಲಾವಣೆ(ಅಂಶ, CANITER_SKIP_ZEROES, &permutations, ಸ್ಥಿತಿ_2);
        ಸ್ಥಿರಾಂಕ UHashElement *ne2 = ಶೂನ್ಯಸೂಚಕ;
        int32_t el2 = UHASH_ಮೊದಲ;
        //Iterator it2 = permutations.iterator();
        ne2 = permutations.ಮುಂದಿನ_Element(el2);
        //while (it2.hasNext())
        ಆಗ (ne2 != ಶೂನ್ಯಸೂಚಕ) {
            //String possible = (String) it2.next();
            //UnicodeString *possible = new UnicodeString(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString possible(*static_cast<UnicodeString*>(ne2->value.ಸೂಚಕ));
            UnicodeString attempt;
            nfd->normalize(possible, attempt, ಸ್ಥಿತಿ_2);

            // TODO: check if operator == is semanticaly the same as attempt.equals(segment)
            ಆದರೆ (attempt==segment) {
                //if (PROGRESS) printf("Adding Permutation: %s\n", UToS(Tr(*possible)));
                // TODO: use the hashtable just to catch duplicates - store strings directly (somehow).
                ಫಲಿತಾಂಶ.put(possible, ಹೊಸ UnicodeString(possible), ಸ್ಥಿತಿ_2); //add(possible);
            } ಇಲ್ಲದಿದ್ದರೆ {
                //if (PROGRESS) printf("-Skipping Permutation: %s\n", UToS(Tr(*possible)));
            }

            ne2 = permutations.ಮುಂದಿನ_Element(el2);
        }
        ne = basic.ಮುಂದಿನ_Element(el);
    }

    /* Test for buffer overflows */
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    // convert into a String[] to clean up storage
    //String[] finalResult = new String[result.size()];
    UnicodeString *final_ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    int32_t ಫಲಿತಾಂಶ_ಎಣಿಕೆ;
    ಆದರೆ((ಫಲಿತಾಂಶ_ಎಣಿಕೆ = ಫಲಿತಾಂಶ.count()) != 0) {
        final_ಫಲಿತಾಂಶ = ಹೊಸ UnicodeString[ಫಲಿತಾಂಶ_ಎಣಿಕೆ];
        ಆದರೆ (final_ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    //result.toArray(finalResult);
    ಫಲಿತಾಂಶ_len = 0;
    el = UHASH_ಮೊದಲ;
    ne = ಫಲಿತಾಂಶ.ಮುಂದಿನ_Element(el);
    ಆಗ(ne != ಶೂನ್ಯಸೂಚಕ) {
        final_ಫಲಿತಾಂಶ[ಫಲಿತಾಂಶ_len++] = *static_cast<UnicodeString*>(ne->value.ಸೂಚಕ);
        ne = ಫಲಿತಾಂಶ.ಮುಂದಿನ_Element(el);
    }


    ಹಿಂತಿರುಗಿ final_ಫಲಿತಾಂಶ;
}

Hashtable* CanonicalIterator::getEquivalents2(Hashtable* fillin_ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                              ಸ್ಥಿರಾಂಕ char16_t* segment,
                                              int32_t segLen,
                                              UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(segment)));

    UnicodeString ಗಮ್ಯ_Put(segment, segLen);

    fillin_ಫಲಿತಾಂಶ->put(ಗಮ್ಯ_Put, ಹೊಸ UnicodeString(ಗಮ್ಯ_Put), ಸ್ಥಿತಿ_2);

    Unicode_ಗಣ starts;

    // cycle through all the characters
    UChar32 cp;
    ಗಾಗಿ (int32_t i = 0; i < segLen; i += U16_ಉದ್ದಳತೆ(cp)) {
        // see if any character is at the start of some decomposition
        U16_GET(segment, 0, i, segLen, cp);
        ಆದರೆ (!nfcImpl->get_Canon_ಆರಂಭ_ಗಣ(cp, starts)) {
            ಮುಂದುವರಿ;
        }
        // if so, see which decompositions match
        Unicode_ಗಣ_Iterator iter(starts);
        ಆಗ (iter.next()) {
            UChar32 cp2 = iter.getCodepoint();
            Hashtable remainder(ಸ್ಥಿತಿ_2);
            remainder.ಗಣ_ಮೌಲ್ಯ_Deleter(uprv_deleteUObject);
            ಆದರೆ (extract(&remainder, cp2, segment, segLen, i, ಸ್ಥಿತಿ_2) == ಶೂನ್ಯಸೂಚಕ) {
                ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }
                ಮುಂದುವರಿ;
            }

            // there were some matches, so add all the possibilities to the set.
            UnicodeString ಪೂರ್ವಪ್ರತ್ಯಯ(segment, i);
            ಪೂರ್ವಪ್ರತ್ಯಯ += cp2;

            int32_t el = UHASH_ಮೊದಲ;
            ಸ್ಥಿರಾಂಕ UHashElement *ne = remainder.ಮುಂದಿನ_Element(el);
            ಆಗ (ne != ಶೂನ್ಯಸೂಚಕ) {
                UnicodeString ಅಂಶ = *static_cast<UnicodeString*>(ne->value.ಸೂಚಕ);
                UnicodeString *ಗಮ್ಯ_ಕೂಡಿಸು = ಹೊಸ UnicodeString(ಪೂರ್ವಪ್ರತ್ಯಯ);
                /* test for nullptr */
                ಆದರೆ (ಗಮ್ಯ_ಕೂಡಿಸು == ಶೂನ್ಯಸೂಚಕ) {
                    ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }
                *ಗಮ್ಯ_ಕೂಡಿಸು += ಅಂಶ;
                fillin_ಫಲಿತಾಂಶ->put(*ಗಮ್ಯ_ಕೂಡಿಸು, ಗಮ್ಯ_ಕೂಡಿಸು, ಸ್ಥಿತಿ_2);

                //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(*toAdd)));

                ne = remainder.ಮುಂದಿನ_Element(el);
            }
            // ICU-22642 Guards against strings that have so many permutations
            // that they would otherwise hang the function.
            constexpr int32_t k_ಫಲಿತಾಂಶ_ಎಲ್ಲೆ = 4096;
            ಆದರೆ (fillin_ಫಲಿತಾಂಶ->count() > k_ಫಲಿತಾಂಶ_ಎಲ್ಲೆ) {
                ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
        }
    }

    /* Test for buffer overflows */
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ fillin_ಫಲಿತಾಂಶ;
}

/**
 * See if the decomposition of cp2 is at segment starting at segmentPos 
 * (with canonical rearrangement!)
 * If so, take the remainder, and return the equivalents 
 */
Hashtable* CanonicalIterator::extract(Hashtable* fillin_ಫಲಿತಾಂಶ U_LIFETIME_BOUND,
                                      UChar32 comp,
                                      ಸ್ಥಿರಾಂಕ char16_t* segment,
                                      int32_t segLen,
                                      int32_t segmentPos,
                                      UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
//Hashtable *CanonicalIterator::extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
    //if (PROGRESS) printf(" extract: %s, ", UToS(Tr(UnicodeString(comp))));
    //if (PROGRESS) printf("%s, %i\n", UToS(Tr(segment)), segmentPos);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    UnicodeString ತಾತ್ಕಾಲಿಕ(comp);
    int32_t ಒಳಹರಿವು_Len=ತಾತ್ಕಾಲಿಕ.length();
    UnicodeString decompString;
    nfd->normalize(ತಾತ್ಕಾಲಿಕ, decompString, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (decompString.isBogus()) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ char16_t *decomp=decompString.get_ಬಫರ್();
    int32_t decompLen=decompString.length();

    // See if it matches the start of segment (at segmentPos)
    UBool ok = ಸುಳ್ಳು;
    UChar32 cp;
    int32_t decompPos = 0;
    UChar32 decompCp;
    U16_ಮುಂದಿನ(decomp, decompPos, decompLen, decompCp);

    int32_t i = segmentPos;
    ಆಗ(i < segLen) {
        U16_ಮುಂದಿನ(segment, i, segLen, cp);

        ಆದರೆ (cp == decompCp) { // if equal, eat another cp from decomp

            //if (PROGRESS) printf("  matches: %s\n", UToS(Tr(UnicodeString(cp))));

            ಆದರೆ (decompPos == decompLen) { // done, have all decomp characters!
                ತಾತ್ಕಾಲಿಕ.append(segment+i, segLen-i);
                ok = ನಿಜ;
                ನಿಲ್ಲಿಸು;
            }
            U16_ಮುಂದಿನ(decomp, decompPos, decompLen, decompCp);
        } ಇಲ್ಲದಿದ್ದರೆ {
            //if (PROGRESS) printf("  buffer: %s\n", UToS(Tr(UnicodeString(cp))));

            // brute force approach
            ತಾತ್ಕಾಲಿಕ.append(cp);

            /* TODO: optimize
            // since we know that the classes are monotonically increasing, after zero
            // e.g. 0 5 7 9 0 3
            // we can do an optimization
            // there are only a few cases that work: zero, less, same, greater
            // if both classes are the same, we fail
            // if the decomp class < the segment class, we fail

            segClass = getClass(cp);
            if (decompClass <= segClass) return null;
            */
        }
    }
    ಆದರೆ (!ok)
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; // we failed, characters left over

    //if (PROGRESS) printf("Matches\n");

    ಆದರೆ (ಒಳಹರಿವು_Len == ತಾತ್ಕಾಲಿಕ.length()) {
        fillin_ಫಲಿತಾಂಶ->put(UnicodeString(), ಹೊಸ UnicodeString(), ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ fillin_ಫಲಿತಾಂಶ; // succeed, but no remainder
    }

    // brute force approach
    // check to make sure result is canonically equivalent
    UnicodeString trial;
    nfd->normalize(ತಾತ್ಕಾಲಿಕ, trial, ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2) || trial.compare(segment+segmentPos, segLen - segmentPos) != 0) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಹಿಂತಿರುಗಿ getEquivalents2(fillin_ಫಲಿತಾಂಶ, ತಾತ್ಕಾಲಿಕ.get_ಬಫರ್()+ಒಳಹರಿವು_Len, ತಾತ್ಕಾಲಿಕ.length()-ಒಳಹರಿವು_Len, ಸ್ಥಿತಿ_2);
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_NORMALIZATION */
