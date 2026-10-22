// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *****************************************************************************
 * Copyright (C) 1996-2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 *****************************************************************************
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_NORMALIZATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/caniter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/normalizer2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/usetiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "normalizer2impl.h"

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

//ਜਨਤਕ

U_NAMESPACE_BEGIN

// TODO: add boilerplate methods.

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(CanonicalIterator)


/**
 *@param source string to get results for
 */
CanonicalIterator::CanonicalIterator(ਅਚਲ UnicodeString &ਸਰੋਤ_Str, UError_ਕੋਡ &ਸਥਿਤੀ) :
    pieces(ਖਾਲੀ_ਸੂਚਕ),
    pieces_ਲੰਮਾਈ(0),
    pieces_lengths(ਖਾਲੀ_ਸੂਚਕ),
    current(ਖਾਲੀ_ਸੂਚਕ),
    current_ਲੰਮਾਈ(0),
    nfd(Normalizer2::getNFDInstance(ਸਥਿਤੀ)),
    nfcImpl(Normalizer2Factory::getNFCImpl(ਸਥਿਤੀ))
{
    ਜੇ(U_SUCCESS(ਸਥਿਤੀ) && nfcImpl->ensure_Canon_Iter_ਅੰਕੜੇ(ਸਥਿਤੀ)) {
      ਸੈੱਟ_ਸਰੋਤ(ਸਰੋਤ_Str, ਸਥਿਤੀ);
    }
}

CanonicalIterator::~CanonicalIterator() {
  cleanPieces();
}

ਖਾਲੀ CanonicalIterator::cleanPieces() {
    int32_t i = 0;
    ਜੇ(pieces != ਖਾਲੀ_ਸੂਚਕ) {
        ਲਈ(i = 0; i < pieces_ਲੰਮਾਈ; i++) {
            ਜੇ(pieces[i] != ਖਾਲੀ_ਸੂਚਕ) {
                ਮਿਟਾਓ[] pieces[i];
            }
        }
        uprv_free(pieces);
        pieces = ਖਾਲੀ_ਸੂਚਕ;
        pieces_ਲੰਮਾਈ = 0;
    }
    ਜੇ(pieces_lengths != ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(pieces_lengths);
        pieces_lengths = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(current != ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(current);
        current = ਖਾਲੀ_ਸੂਚਕ;
        current_ਲੰਮਾਈ = 0;
    }
}

/**
 *@return gets the source: NOTE: it is the NFD form of source
 */
UnicodeString CanonicalIterator::get_ਸਰੋਤ() {
  ਵਾਪਸ ਸਰੋਤ;
}

/**
 * Resets the iterator so that one can start again from the beginning.
 */
ਖਾਲੀ CanonicalIterator::reset() {
    ਪੂਰਾ = ਝੂਠ;
    ਲਈ (ਪੂਰਨ i = 0; i < current_ਲੰਮਾਈ; ++i) {
        current[i] = 0;
    }
}

/**
 *@return the next string that is canonically equivalent. The value null is returned when
 * the iteration is done.
 */
UnicodeString CanonicalIterator::next() {
    int32_t i = 0;

    ਜੇ (ਪੂਰਾ) {
      ਬਫਰ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ ਬਫਰ;
    }

    // delete old contents
    ਬਫਰ.remove();

    // construct return value

    ਲਈ (i = 0; i < pieces_ਲੰਮਾਈ; ++i) {
        ਬਫਰ.append(pieces[i][current[i]]);
    }
    //String result = buffer.toString(); // not needed

    // find next value for next time

    ਲਈ (i = current_ਲੰਮਾਈ - 1; ; --i) {
        ਜੇ (i < 0) {
            ਪੂਰਾ = ਸੱਚ;
            ਰੋਕੋ;
        }
        current[i]++;
        ਜੇ (current[i] < pieces_lengths[i]) ਰੋਕੋ; // got sequence
        current[i] = 0;
    }
    ਵਾਪਸ ਬਫਰ;
}

/**
 *@param set the source string to iterate against. This allows the same iterator to be used
 * while changing the source string, saving object creation.
 */
ਖਾਲੀ CanonicalIterator::ਸੈੱਟ_ਸਰੋਤ(ਅਚਲ UnicodeString &new_ਸਰੋਤ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    int32_t list_ਲੰਮਾਈ_2 = 0;
    UChar32 cp = 0;
    int32_t ਸ਼ੁਰੂ = 0;
    int32_t i = 0;
    UnicodeString *list = ਖਾਲੀ_ਸੂਚਕ;

    nfd->normalize(new_ਸਰੋਤ, ਸਰੋਤ, ਸਥਿਤੀ);
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
      ਵਾਪਸ;
    }
    ਪੂਰਾ = ਝੂਠ;

    cleanPieces();

    // catch degenerate case
    ਜੇ (new_ਸਰੋਤ.length() == 0) {
        pieces = static_cast<UnicodeString**>(uprv_malloc(ਆਕਾਰ(UnicodeString*)));
        pieces_lengths = static_cast<int32_t*>(uprv_malloc(1 * ਆਕਾਰ(int32_t)));
        pieces_ਲੰਮਾਈ = 1;
        current = static_cast<int32_t*>(uprv_malloc(1 * ਆਕਾਰ(int32_t)));
        current_ਲੰਮਾਈ = 1;
        ਜੇ (pieces == ਖਾਲੀ_ਸੂਚਕ || pieces_lengths == ਖਾਲੀ_ਸੂਚਕ || current == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਜਾਓ CleanPartialInitialization;
        }
        current[0] = 0;
        pieces[0] = ਨਵਾਂ UnicodeString[1];
        pieces_lengths[0] = 1;
        ਜੇ (pieces[0] == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਜਾਓ CleanPartialInitialization;
        }
        ਵਾਪਸ;
    }


    list = ਨਵਾਂ UnicodeString[ਸਰੋਤ.length()];
    ਜੇ (list == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਜਾਓ CleanPartialInitialization;
    }

    // i should initially be the number of code units at the 
    // start of the string
    i = U16_ਲੰਮਾਈ(ਸਰੋਤ.char32At(0));
    // int32_t i = 1;
    // find the segments
    // This code iterates through the source string and 
    // extracts segments that end up on a codepoint that
    // doesn't start any decompositions. (Analysis is done
    // on the NFD form - see above).
    ਲਈ (; i < ਸਰੋਤ.length(); i += U16_ਲੰਮਾਈ(cp)) {
        cp = ਸਰੋਤ.char32At(i);
        ਜੇ (nfcImpl->isCanonSegmentStarter(cp)) {
            ਸਰੋਤ.extract(ਸ਼ੁਰੂ, i-ਸ਼ੁਰੂ, list[list_ਲੰਮਾਈ_2++]); // add up to i
            ਸ਼ੁਰੂ = i;
        }
    }
    ਸਰੋਤ.extract(ਸ਼ੁਰੂ, i-ਸ਼ੁਰੂ, list[list_ਲੰਮਾਈ_2++]); // add last one


    // allocate the arrays, and find the strings that are CE to each segment
    pieces = static_cast<UnicodeString**>(uprv_malloc(list_ਲੰਮਾਈ_2 * ਆਕਾਰ(UnicodeString*)));
    pieces_ਲੰਮਾਈ = list_ਲੰਮਾਈ_2;
    pieces_lengths = static_cast<int32_t*>(uprv_malloc(list_ਲੰਮਾਈ_2 * ਆਕਾਰ(int32_t)));
    current = static_cast<int32_t*>(uprv_malloc(list_ਲੰਮਾਈ_2 * ਆਕਾਰ(int32_t)));
    current_ਲੰਮਾਈ = list_ਲੰਮਾਈ_2;
    ਜੇ (pieces == ਖਾਲੀ_ਸੂਚਕ || pieces_lengths == ਖਾਲੀ_ਸੂਚਕ || current == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਜਾਓ CleanPartialInitialization;
    }

    ਲਈ (i = 0; i < current_ਲੰਮਾਈ; i++) {
        current[i] = 0;
    }
    // for each segment, get all the combinations that can produce 
    // it after NFD normalization
    ਲਈ (i = 0; i < pieces_ਲੰਮਾਈ; ++i) {
        //if (PROGRESS) printf("SEGMENT\n");
        pieces[i] = getEquivalents(list[i], pieces_lengths[i], ਸਥਿਤੀ);
    }

    ਮਿਟਾਓ[] list;
    ਵਾਪਸ;
// Common section to cleanup all local variables and reset object variables.
CleanPartialInitialization:
    ਮਿਟਾਓ[] list;
    cleanPieces();
}

/**
 * Dumb recursive implementation of permutation.
 * TODO: optimize
 * @param source the string to find permutations for
 * @return the results in a set.
 */
ਖਾਲੀ U_EXPORT2 CanonicalIterator::ਕ੍ਰਮ_ਬਦਲੋ(UnicodeString &ਸਰੋਤ, UBool skipZeros, Hashtable *ਨਤੀਜਾ, UError_ਕੋਡ &ਸਥਿਤੀ, int32_t ਡੂੰਘਾਈ) {
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    // To avoid infinity loop caused by permute, we limit the depth of recursive
    // call to permute and return U_UNSUPPORTED_ERROR.
    // We know in some unit test we need at least 4. Set to 8 just in case some
    // unforseen use cases.
    constexpr int32_t k_ਕ੍ਰਮ_ਬਦਲੋ_ਡੂੰਘਾਈ_ਹੱਦਬੰਦੀ = 8;
    ਜੇ (ਡੂੰਘਾਈ > k_ਕ੍ਰਮ_ਬਦਲੋ_ਡੂੰਘਾਈ_ਹੱਦਬੰਦੀ) {
        ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
        ਵਾਪਸ;
    }
    //if (PROGRESS) printf("Permute: %s\n", UToS(Tr(source)));
    int32_t i = 0;

    // optimization:
    // if zero or one character, just return a set with it
    // we check for length < 2 to keep from counting code points all the time
    ਜੇ (ਸਰੋਤ.length() <= 2 && ਸਰੋਤ.ਗਿਣਤੀ_Char32() <= 1) {
        UnicodeString *ਮੰਜ਼ਿਲ_Put = ਨਵਾਂ UnicodeString(ਸਰੋਤ);
        /* test for nullptr */
        ਜੇ (ਮੰਜ਼ਿਲ_Put == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
        ਨਤੀਜਾ->put(ਸਰੋਤ, ਮੰਜ਼ਿਲ_Put, ਸਥਿਤੀ);
        ਵਾਪਸ;
    }

    // otherwise iterate through the string, and recursively permute all the other characters
    UChar32 cp;
    Hashtable subpermute(ਸਥਿਤੀ);
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    subpermute.ਸੈੱਟ_ਮੁੱਲ_Deleter(uprv_deleteUObject);

    ਲਈ (i = 0; i < ਸਰੋਤ.length(); i += U16_ਲੰਮਾਈ(cp)) {
        cp = ਸਰੋਤ.char32At(i);
        ਅਚਲ UHashElement *ne = ਖਾਲੀ_ਸੂਚਕ;
        int32_t el = UHASH_ਪਹਿਲਾ;
        UnicodeString sub_ਕ੍ਰਮ_ਬਦਲੋ_String = ਸਰੋਤ;

        // optimization:
        // if the character is canonical combining class zero,
        // don't permute it
        ਜੇ (skipZeros && i != 0 && u_getCombiningClass(cp) == 0) {
            //System.out.println("Skipping " + Utility.hex(UTF16.valueOf(source, i)));
            ਜਾਰੀ;
        }

        subpermute.removeAll();

        // see what the permutations of the characters before and after this one are
        //Hashtable *subpermute = permute(source.substring(0,i) + source.substring(i + UTF16.getCharCount(cp)));
        ਕ੍ਰਮ_ਬਦਲੋ(sub_ਕ੍ਰਮ_ਬਦਲੋ_String.remove(i, U16_ਲੰਮਾਈ(cp)), skipZeros, &subpermute, ਸਥਿਤੀ, ਡੂੰਘਾਈ+1);
        /* Test for buffer overflows */
        ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ;
        }
        // The upper remove is destructive. The question is do we have to make a copy, or we don't care about the contents 
        // of source at this point.

        // prefix this character to all of them
        ne = subpermute.ਅਗਲਾ_Element(el);
        ਜਦੋਂ (ne != ਖਾਲੀ_ਸੂਚਕ) {
            UnicodeString* permRes = static_cast<UnicodeString*>(ne->value.ਸੂਚਕ);
            UnicodeString *chStr = ਨਵਾਂ UnicodeString(cp);
            //test for nullptr
            ਜੇ (chStr == ਖਾਲੀ_ਸੂਚਕ) {
                ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
            chStr->append(*permRes); //*((UnicodeString *)(ne->value.pointer));
            //if (PROGRESS) printf("  Piece: %s\n", UToS(*chStr));
            ਨਤੀਜਾ->put(*chStr, chStr, ਸਥਿਤੀ);
            ne = subpermute.ਅਗਲਾ_Element(el);
        }
    }
    //return result;
}

// privates

// we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
UnicodeString* CanonicalIterator::getEquivalents(ਅਚਲ UnicodeString &segment, int32_t &ਨਤੀਜਾ_len, UError_ਕੋਡ &ਸਥਿਤੀ) {
    Hashtable ਨਤੀਜਾ(ਸਥਿਤੀ);
    Hashtable permutations(ਸਥਿਤੀ);
    Hashtable basic(ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਨਤੀਜਾ.ਸੈੱਟ_ਮੁੱਲ_Deleter(uprv_deleteUObject);
    permutations.ਸੈੱਟ_ਮੁੱਲ_Deleter(uprv_deleteUObject);
    basic.ਸੈੱਟ_ਮੁੱਲ_Deleter(uprv_deleteUObject);

    char16_t USeg[256];
    int32_t segLen = segment.extract(USeg, 256, ਸਥਿਤੀ);
    getEquivalents2(&basic, USeg, segLen, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    // now get all the permutations
    // add only the ones that are canonically equivalent
    // TODO: optimize by not permuting any class zero.

    ਅਚਲ UHashElement *ne = ਖਾਲੀ_ਸੂਚਕ;
    int32_t el = UHASH_ਪਹਿਲਾ;
    //Iterator it = basic.iterator();
    ne = basic.ਅਗਲਾ_Element(el);
    //while (it.hasNext())
    ਜਦੋਂ (ne != ਖਾਲੀ_ਸੂਚਕ) {
        //String item = (String) it.next();
        UnicodeString ਵਸਤੂ = *static_cast<UnicodeString*>(ne->value.ਸੂਚਕ);

        permutations.removeAll();
        ਕ੍ਰਮ_ਬਦਲੋ(ਵਸਤੂ, CANITER_SKIP_ZEROES, &permutations, ਸਥਿਤੀ);
        ਅਚਲ UHashElement *ne2 = ਖਾਲੀ_ਸੂਚਕ;
        int32_t el2 = UHASH_ਪਹਿਲਾ;
        //Iterator it2 = permutations.iterator();
        ne2 = permutations.ਅਗਲਾ_Element(el2);
        //while (it2.hasNext())
        ਜਦੋਂ (ne2 != ਖਾਲੀ_ਸੂਚਕ) {
            //String possible = (String) it2.next();
            //UnicodeString *possible = new UnicodeString(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString possible(*static_cast<UnicodeString*>(ne2->value.ਸੂਚਕ));
            UnicodeString attempt;
            nfd->normalize(possible, attempt, ਸਥਿਤੀ);

            // TODO: check if operator == is semanticaly the same as attempt.equals(segment)
            ਜੇ (attempt==segment) {
                //if (PROGRESS) printf("Adding Permutation: %s\n", UToS(Tr(*possible)));
                // TODO: use the hashtable just to catch duplicates - store strings directly (somehow).
                ਨਤੀਜਾ.put(possible, ਨਵਾਂ UnicodeString(possible), ਸਥਿਤੀ); //add(possible);
            } ਨਹੀਂਤਾਂ {
                //if (PROGRESS) printf("-Skipping Permutation: %s\n", UToS(Tr(*possible)));
            }

            ne2 = permutations.ਅਗਲਾ_Element(el2);
        }
        ne = basic.ਅਗਲਾ_Element(el);
    }

    /* Test for buffer overflows */
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    // convert into a String[] to clean up storage
    //String[] finalResult = new String[result.size()];
    UnicodeString *final_ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    int32_t ਨਤੀਜਾ_ਗਿਣਤੀ;
    ਜੇ((ਨਤੀਜਾ_ਗਿਣਤੀ = ਨਤੀਜਾ.count()) != 0) {
        final_ਨਤੀਜਾ = ਨਵਾਂ UnicodeString[ਨਤੀਜਾ_ਗਿਣਤੀ];
        ਜੇ (final_ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
            ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }
    ਨਹੀਂਤਾਂ {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    //result.toArray(finalResult);
    ਨਤੀਜਾ_len = 0;
    el = UHASH_ਪਹਿਲਾ;
    ne = ਨਤੀਜਾ.ਅਗਲਾ_Element(el);
    ਜਦੋਂ(ne != ਖਾਲੀ_ਸੂਚਕ) {
        final_ਨਤੀਜਾ[ਨਤੀਜਾ_len++] = *static_cast<UnicodeString*>(ne->value.ਸੂਚਕ);
        ne = ਨਤੀਜਾ.ਅਗਲਾ_Element(el);
    }


    ਵਾਪਸ final_ਨਤੀਜਾ;
}

Hashtable* CanonicalIterator::getEquivalents2(Hashtable* fillin_ਨਤੀਜਾ U_LIFETIME_BOUND,
                                              ਅਚਲ char16_t* segment,
                                              int32_t segLen,
                                              UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(segment)));

    UnicodeString ਮੰਜ਼ਿਲ_Put(segment, segLen);

    fillin_ਨਤੀਜਾ->put(ਮੰਜ਼ਿਲ_Put, ਨਵਾਂ UnicodeString(ਮੰਜ਼ਿਲ_Put), ਸਥਿਤੀ);

    Unicode_ਸੈੱਟ starts;

    // cycle through all the characters
    UChar32 cp;
    ਲਈ (int32_t i = 0; i < segLen; i += U16_ਲੰਮਾਈ(cp)) {
        // see if any character is at the start of some decomposition
        U16_GET(segment, 0, i, segLen, cp);
        ਜੇ (!nfcImpl->get_Canon_ਸ਼ੁਰੂ_ਸੈੱਟ(cp, starts)) {
            ਜਾਰੀ;
        }
        // if so, see which decompositions match
        Unicode_ਸੈੱਟ_Iterator iter(starts);
        ਜਦੋਂ (iter.next()) {
            UChar32 cp2 = iter.getCodepoint();
            Hashtable remainder(ਸਥਿਤੀ);
            remainder.ਸੈੱਟ_ਮੁੱਲ_Deleter(uprv_deleteUObject);
            ਜੇ (extract(&remainder, cp2, segment, segLen, i, ਸਥਿਤੀ) == ਖਾਲੀ_ਸੂਚਕ) {
                ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }
                ਜਾਰੀ;
            }

            // there were some matches, so add all the possibilities to the set.
            UnicodeString ਅਗੇਤਰ(segment, i);
            ਅਗੇਤਰ += cp2;

            int32_t el = UHASH_ਪਹਿਲਾ;
            ਅਚਲ UHashElement *ne = remainder.ਅਗਲਾ_Element(el);
            ਜਦੋਂ (ne != ਖਾਲੀ_ਸੂਚਕ) {
                UnicodeString ਵਸਤੂ = *static_cast<UnicodeString*>(ne->value.ਸੂਚਕ);
                UnicodeString *ਮੰਜ਼ਿਲ_ਜੋੜੋ = ਨਵਾਂ UnicodeString(ਅਗੇਤਰ);
                /* test for nullptr */
                ਜੇ (ਮੰਜ਼ਿਲ_ਜੋੜੋ == ਖਾਲੀ_ਸੂਚਕ) {
                    ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }
                *ਮੰਜ਼ਿਲ_ਜੋੜੋ += ਵਸਤੂ;
                fillin_ਨਤੀਜਾ->put(*ਮੰਜ਼ਿਲ_ਜੋੜੋ, ਮੰਜ਼ਿਲ_ਜੋੜੋ, ਸਥਿਤੀ);

                //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(*toAdd)));

                ne = remainder.ਅਗਲਾ_Element(el);
            }
            // ICU-22642 Guards against strings that have so many permutations
            // that they would otherwise hang the function.
            constexpr int32_t k_ਨਤੀਜਾ_ਹੱਦਬੰਦੀ = 4096;
            ਜੇ (fillin_ਨਤੀਜਾ->count() > k_ਨਤੀਜਾ_ਹੱਦਬੰਦੀ) {
                ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
        }
    }

    /* Test for buffer overflows */
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ fillin_ਨਤੀਜਾ;
}

/**
 * See if the decomposition of cp2 is at segment starting at segmentPos 
 * (with canonical rearrangement!)
 * If so, take the remainder, and return the equivalents 
 */
Hashtable* CanonicalIterator::extract(Hashtable* fillin_ਨਤੀਜਾ U_LIFETIME_BOUND,
                                      UChar32 comp,
                                      ਅਚਲ char16_t* segment,
                                      int32_t segLen,
                                      int32_t segmentPos,
                                      UError_ਕੋਡ& ਸਥਿਤੀ) {
//Hashtable *CanonicalIterator::extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
    //if (PROGRESS) printf(" extract: %s, ", UToS(Tr(UnicodeString(comp))));
    //if (PROGRESS) printf("%s, %i\n", UToS(Tr(segment)), segmentPos);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    UnicodeString ਆਰਜ਼ੀ(comp);
    int32_t ਨਿਵੇਸ਼_Len=ਆਰਜ਼ੀ.length();
    UnicodeString decompString;
    nfd->normalize(ਆਰਜ਼ੀ, decompString, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (decompString.isBogus()) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ char16_t *decomp=decompString.get_ਬਫਰ();
    int32_t decompLen=decompString.length();

    // See if it matches the start of segment (at segmentPos)
    UBool ok = ਝੂਠ;
    UChar32 cp;
    int32_t decompPos = 0;
    UChar32 decompCp;
    U16_ਅਗਲਾ(decomp, decompPos, decompLen, decompCp);

    int32_t i = segmentPos;
    ਜਦੋਂ(i < segLen) {
        U16_ਅਗਲਾ(segment, i, segLen, cp);

        ਜੇ (cp == decompCp) { // if equal, eat another cp from decomp

            //if (PROGRESS) printf("  matches: %s\n", UToS(Tr(UnicodeString(cp))));

            ਜੇ (decompPos == decompLen) { // done, have all decomp characters!
                ਆਰਜ਼ੀ.append(segment+i, segLen-i);
                ok = ਸੱਚ;
                ਰੋਕੋ;
            }
            U16_ਅਗਲਾ(decomp, decompPos, decompLen, decompCp);
        } ਨਹੀਂਤਾਂ {
            //if (PROGRESS) printf("  buffer: %s\n", UToS(Tr(UnicodeString(cp))));

            // brute force approach
            ਆਰਜ਼ੀ.append(cp);

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
    ਜੇ (!ok)
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; // we failed, characters left over

    //if (PROGRESS) printf("Matches\n");

    ਜੇ (ਨਿਵੇਸ਼_Len == ਆਰਜ਼ੀ.length()) {
        fillin_ਨਤੀਜਾ->put(UnicodeString(), ਨਵਾਂ UnicodeString(), ਸਥਿਤੀ);
        ਵਾਪਸ fillin_ਨਤੀਜਾ; // succeed, but no remainder
    }

    // brute force approach
    // check to make sure result is canonically equivalent
    UnicodeString trial;
    nfd->normalize(ਆਰਜ਼ੀ, trial, ਸਥਿਤੀ);
    ਜੇ(U_FAILURE(ਸਥਿਤੀ) || trial.compare(segment+segmentPos, segLen - segmentPos) != 0) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਵਾਪਸ getEquivalents2(fillin_ਨਤੀਜਾ, ਆਰਜ਼ੀ.get_ਬਫਰ()+ਨਿਵੇਸ਼_Len, ਆਰਜ਼ੀ.length()-ਨਿਵੇਸ਼_Len, ਸਥਿਤੀ);
}

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_NORMALIZATION */
