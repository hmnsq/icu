// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *****************************************************************************
 * Copyright (C) 1996-2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 *****************************************************************************
 */

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_NORMALIZATION

#समावेश "unicode/caniter.h"
#समावेश "unicode/normalizer2.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/usetiter.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "hash.h"
#समावेश "normalizer2impl.h"

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

// public

U_NAMESPACE_BEGIN

// TODO: add boilerplate methods.

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(CanonicalIterator)


/**
 *@param source string to get results for
 */
CanonicalIterator::CanonicalIterator(स्थिर UnicodeString &sourceStr, UErrorCode &status) :
    pieces(nullptr),
    pieces_लंबाई(0),
    pieces_lengths(nullptr),
    current(nullptr),
    current_लंबाई(0),
    nfd(Normalizer2::getNFDInstance(status)),
    nfcImpl(Normalizer2Factory::getNFCImpl(status))
{
    अगर(U_SUCCESS(status) && nfcImpl->ensure_Canon_Iter_तथ्य(status)) {
      setSource(sourceStr, status);
    }
}

CanonicalIterator::~CanonicalIterator() {
  cleanPieces();
}

व्योम CanonicalIterator::cleanPieces() {
    int32_t i = 0;
    अगर(pieces != nullptr) {
        क्रम(i = 0; i < pieces_लंबाई; i++) {
            अगर(pieces[i] != nullptr) {
                मिटाओ[] pieces[i];
            }
        }
        uprv_free(pieces);
        pieces = nullptr;
        pieces_लंबाई = 0;
    }
    अगर(pieces_lengths != nullptr) {
        uprv_free(pieces_lengths);
        pieces_lengths = nullptr;
    }
    अगर(current != nullptr) {
        uprv_free(current);
        current = nullptr;
        current_लंबाई = 0;
    }
}

/**
 *@return gets the source: NOTE: it is the NFD form of source
 */
UnicodeString CanonicalIterator::getSource() {
  वापस source;
}

/**
 * Resets the iterator so that one can start again from the beginning.
 */
व्योम CanonicalIterator::reset() {
    done = false;
    क्रम (पूर्णांक i = 0; i < current_लंबाई; ++i) {
        current[i] = 0;
    }
}

/**
 *@return the next string that is canonically equivalent. The value null is returned when
 * the iteration is done.
 */
UnicodeString CanonicalIterator::next() {
    int32_t i = 0;

    अगर (done) {
      बफर.setToBogus();
      वापस बफर;
    }

    // delete old contents
    बफर.हटाओ();

    // construct return value

    क्रम (i = 0; i < pieces_लंबाई; ++i) {
        बफर.append(pieces[i][current[i]]);
    }
    //String result = buffer.toString(); // not needed

    // find next value for next time

    क्रम (i = current_लंबाई - 1; ; --i) {
        अगर (i < 0) {
            done = true;
            अवरोध;
        }
        current[i]++;
        अगर (current[i] < pieces_lengths[i]) अवरोध; // got sequence
        current[i] = 0;
    }
    वापस बफर;
}

/**
 *@param set the source string to iterate against. This allows the same iterator to be used
 * while changing the source string, saving object creation.
 */
व्योम CanonicalIterator::setSource(स्थिर UnicodeString &newSource, UErrorCode &status) {
    int32_t list_लंबाई_2 = 0;
    UChar32 cp = 0;
    int32_t start = 0;
    int32_t i = 0;
    UnicodeString *list = nullptr;

    nfd->normalize(newSource, source, status);
    अगर(U_FAILURE(status)) {
      वापस;
    }
    done = false;

    cleanPieces();

    // catch degenerate case
    अगर (newSource.length() == 0) {
        pieces = static_cast<UnicodeString**>(uprv_malloc(माप(UnicodeString*)));
        pieces_lengths = static_cast<int32_t*>(uprv_malloc(1 * माप(int32_t)));
        pieces_लंबाई = 1;
        current = static_cast<int32_t*>(uprv_malloc(1 * माप(int32_t)));
        current_लंबाई = 1;
        अगर (pieces == nullptr || pieces_lengths == nullptr || current == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            जाओ CleanPartialInitialization;
        }
        current[0] = 0;
        pieces[0] = नया UnicodeString[1];
        pieces_lengths[0] = 1;
        अगर (pieces[0] == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            जाओ CleanPartialInitialization;
        }
        वापस;
    }


    list = नया UnicodeString[source.length()];
    अगर (list == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        जाओ CleanPartialInitialization;
    }

    // i should initially be the number of code units at the 
    // start of the string
    i = U16_लंबाई(source.char32At(0));
    // int32_t i = 1;
    // find the segments
    // This code iterates through the source string and 
    // extracts segments that end up on a codepoint that
    // doesn't start any decompositions. (Analysis is done
    // on the NFD form - see above).
    क्रम (; i < source.length(); i += U16_लंबाई(cp)) {
        cp = source.char32At(i);
        अगर (nfcImpl->isCanonSegmentStarter(cp)) {
            source.extract(start, i-start, list[list_लंबाई_2++]); // add up to i
            start = i;
        }
    }
    source.extract(start, i-start, list[list_लंबाई_2++]); // add last one


    // allocate the arrays, and find the strings that are CE to each segment
    pieces = static_cast<UnicodeString**>(uprv_malloc(list_लंबाई_2 * माप(UnicodeString*)));
    pieces_लंबाई = list_लंबाई_2;
    pieces_lengths = static_cast<int32_t*>(uprv_malloc(list_लंबाई_2 * माप(int32_t)));
    current = static_cast<int32_t*>(uprv_malloc(list_लंबाई_2 * माप(int32_t)));
    current_लंबाई = list_लंबाई_2;
    अगर (pieces == nullptr || pieces_lengths == nullptr || current == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        जाओ CleanPartialInitialization;
    }

    क्रम (i = 0; i < current_लंबाई; i++) {
        current[i] = 0;
    }
    // for each segment, get all the combinations that can produce 
    // it after NFD normalization
    क्रम (i = 0; i < pieces_लंबाई; ++i) {
        //if (PROGRESS) printf("SEGMENT\n");
        pieces[i] = getEquivalents(list[i], pieces_lengths[i], status);
    }

    मिटाओ[] list;
    वापस;
// Common section to cleanup all local variables and reset object variables.
CleanPartialInitialization:
    मिटाओ[] list;
    cleanPieces();
}

/**
 * Dumb recursive implementation of permutation.
 * TODO: optimize
 * @param source the string to find permutations for
 * @return the results in a set.
 */
व्योम U_EXPORT2 CanonicalIterator::permute(UnicodeString &source, UBool skipZeros, Hashtable *परिणाम, UErrorCode &status, int32_t depth) {
    अगर(U_FAILURE(status)) {
        वापस;
    }
    // To avoid infinity loop caused by permute, we limit the depth of recursive
    // call to permute and return U_UNSUPPORTED_ERROR.
    // We know in some unit test we need at least 4. Set to 8 just in case some
    // unforseen use cases.
    constexpr int32_t kPermuteDepthLimit = 8;
    अगर (depth > kPermuteDepthLimit) {
        status = U_UNSUPPORTED_ERROR;
        वापस;
    }
    //if (PROGRESS) printf("Permute: %s\n", UToS(Tr(source)));
    int32_t i = 0;

    // optimization:
    // if zero or one character, just return a set with it
    // we check for length < 2 to keep from counting code points all the time
    अगर (source.length() <= 2 && source.गिनती_Char32() <= 1) {
        UnicodeString *toPut = नया UnicodeString(source);
        /* test for nullptr */
        अगर (toPut == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
        परिणाम->दो(source, toPut, status);
        वापस;
    }

    // otherwise iterate through the string, and recursively permute all the other characters
    UChar32 cp;
    Hashtable subpermute(status);
    अगर(U_FAILURE(status)) {
        वापस;
    }
    subpermute.set_मूल्य_Deleter(uprv_deleteUObject);

    क्रम (i = 0; i < source.length(); i += U16_लंबाई(cp)) {
        cp = source.char32At(i);
        स्थिर UHashElement *ne = nullptr;
        int32_t el = UHASH_FIRST;
        UnicodeString subPermuteString = source;

        // optimization:
        // if the character is canonical combining class zero,
        // don't permute it
        अगर (skipZeros && i != 0 && u_getCombiningClass(cp) == 0) {
            //System.out.println("Skipping " + Utility.hex(UTF16.valueOf(source, i)));
            जारी;
        }

        subpermute.removeAll();

        // see what the permutations of the characters before and after this one are
        //Hashtable *subpermute = permute(source.substring(0,i) + source.substring(i + UTF16.getCharCount(cp)));
        permute(subPermuteString.हटाओ(i, U16_लंबाई(cp)), skipZeros, &subpermute, status, depth+1);
        /* Test for buffer overflows */
        अगर(U_FAILURE(status)) {
            वापस;
        }
        // The upper remove is destructive. The question is do we have to make a copy, or we don't care about the contents 
        // of source at this point.

        // prefix this character to all of them
        ne = subpermute.अग्र_Element(el);
        जबतक (ne != nullptr) {
            UnicodeString* permRes = static_cast<UnicodeString*>(ne->value.pointer);
            UnicodeString *chStr = नया UnicodeString(cp);
            //test for nullptr
            अगर (chStr == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
            chStr->append(*permRes); //*((UnicodeString *)(ne->value.pointer));
            //if (PROGRESS) printf("  Piece: %s\n", UToS(*chStr));
            परिणाम->दो(*chStr, chStr, status);
            ne = subpermute.अग्र_Element(el);
        }
    }
    //return result;
}

// privates

// we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
UnicodeString* CanonicalIterator::getEquivalents(स्थिर UnicodeString &segment, int32_t &परिणाम_len, UErrorCode &status) {
    Hashtable परिणाम(status);
    Hashtable permutations(status);
    Hashtable basic(status);
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }
    परिणाम.set_मूल्य_Deleter(uprv_deleteUObject);
    permutations.set_मूल्य_Deleter(uprv_deleteUObject);
    basic.set_मूल्य_Deleter(uprv_deleteUObject);

    char16_t USeg[256];
    int32_t segLen = segment.extract(USeg, 256, status);
    getEquivalents2(&basic, USeg, segLen, status);
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    // now get all the permutations
    // add only the ones that are canonically equivalent
    // TODO: optimize by not permuting any class zero.

    स्थिर UHashElement *ne = nullptr;
    int32_t el = UHASH_FIRST;
    //Iterator it = basic.iterator();
    ne = basic.अग्र_Element(el);
    //while (it.hasNext())
    जबतक (ne != nullptr) {
        //String item = (String) it.next();
        UnicodeString item = *static_cast<UnicodeString*>(ne->value.pointer);

        permutations.removeAll();
        permute(item, CANITER_SKIP_ZEROES, &permutations, status);
        स्थिर UHashElement *ne2 = nullptr;
        int32_t el2 = UHASH_FIRST;
        //Iterator it2 = permutations.iterator();
        ne2 = permutations.अग्र_Element(el2);
        //while (it2.hasNext())
        जबतक (ne2 != nullptr) {
            //String possible = (String) it2.next();
            //UnicodeString *possible = new UnicodeString(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString possible(*static_cast<UnicodeString*>(ne2->value.pointer));
            UnicodeString attempt;
            nfd->normalize(possible, attempt, status);

            // TODO: check if operator == is semanticaly the same as attempt.equals(segment)
            अगर (attempt==segment) {
                //if (PROGRESS) printf("Adding Permutation: %s\n", UToS(Tr(*possible)));
                // TODO: use the hashtable just to catch duplicates - store strings directly (somehow).
                परिणाम.दो(possible, नया UnicodeString(possible), status); //add(possible);
            } अन्यथा {
                //if (PROGRESS) printf("-Skipping Permutation: %s\n", UToS(Tr(*possible)));
            }

            ne2 = permutations.अग्र_Element(el2);
        }
        ne = basic.अग्र_Element(el);
    }

    /* Test for buffer overflows */
    अगर(U_FAILURE(status)) {
        वापस nullptr;
    }
    // convert into a String[] to clean up storage
    //String[] finalResult = new String[result.size()];
    UnicodeString *final_परिणाम = nullptr;
    int32_t परिणाम_गिनती;
    अगर((परिणाम_गिनती = परिणाम.count()) != 0) {
        final_परिणाम = नया UnicodeString[परिणाम_गिनती];
        अगर (final_परिणाम == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            वापस nullptr;
        }
    }
    अन्यथा {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    //result.toArray(finalResult);
    परिणाम_len = 0;
    el = UHASH_FIRST;
    ne = परिणाम.अग्र_Element(el);
    जबतक(ne != nullptr) {
        final_परिणाम[परिणाम_len++] = *static_cast<UnicodeString*>(ne->value.pointer);
        ne = परिणाम.अग्र_Element(el);
    }


    वापस final_परिणाम;
}

Hashtable* CanonicalIterator::getEquivalents2(Hashtable* fillin_परिणाम U_LIFETIME_BOUND,
                                              स्थिर char16_t* segment,
                                              int32_t segLen,
                                              UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(segment)));

    UnicodeString toPut(segment, segLen);

    fillin_परिणाम->दो(toPut, नया UnicodeString(toPut), status);

    UnicodeSet starts;

    // cycle through all the characters
    UChar32 cp;
    क्रम (int32_t i = 0; i < segLen; i += U16_लंबाई(cp)) {
        // see if any character is at the start of some decomposition
        U16_GET(segment, 0, i, segLen, cp);
        अगर (!nfcImpl->getCanonStartSet(cp, starts)) {
            जारी;
        }
        // if so, see which decompositions match
        UnicodeSetIterator iter(starts);
        जबतक (iter.next()) {
            UChar32 cp2 = iter.getCodepoint();
            Hashtable remainder(status);
            remainder.set_मूल्य_Deleter(uprv_deleteUObject);
            अगर (extract(&remainder, cp2, segment, segLen, i, status) == nullptr) {
                अगर (U_FAILURE(status)) {
                    वापस nullptr;
                }
                जारी;
            }

            // there were some matches, so add all the possibilities to the set.
            UnicodeString prefix(segment, i);
            prefix += cp2;

            int32_t el = UHASH_FIRST;
            स्थिर UHashElement *ne = remainder.अग्र_Element(el);
            जबतक (ne != nullptr) {
                UnicodeString item = *static_cast<UnicodeString*>(ne->value.pointer);
                UnicodeString *toAdd = नया UnicodeString(prefix);
                /* test for nullptr */
                अगर (toAdd == nullptr) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    वापस nullptr;
                }
                *toAdd += item;
                fillin_परिणाम->दो(*toAdd, toAdd, status);

                //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(*toAdd)));

                ne = remainder.अग्र_Element(el);
            }
            // ICU-22642 Guards against strings that have so many permutations
            // that they would otherwise hang the function.
            constexpr int32_t k_परिणाम_Limit = 4096;
            अगर (fillin_परिणाम->count() > k_परिणाम_Limit) {
                status = U_UNSUPPORTED_ERROR;
                वापस nullptr;
            }
        }
    }

    /* Test for buffer overflows */
    अगर(U_FAILURE(status)) {
        वापस nullptr;
    }
    वापस fillin_परिणाम;
}

/**
 * See if the decomposition of cp2 is at segment starting at segmentPos 
 * (with canonical rearrangement!)
 * If so, take the remainder, and return the equivalents 
 */
Hashtable* CanonicalIterator::extract(Hashtable* fillin_परिणाम U_LIFETIME_BOUND,
                                      UChar32 comp,
                                      स्थिर char16_t* segment,
                                      int32_t segLen,
                                      int32_t segmentPos,
                                      UErrorCode& status) {
//Hashtable *CanonicalIterator::extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
    //if (PROGRESS) printf(" extract: %s, ", UToS(Tr(UnicodeString(comp))));
    //if (PROGRESS) printf("%s, %i\n", UToS(Tr(segment)), segmentPos);

    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }

    UnicodeString अस्थायी(comp);
    int32_t निवेश_Len=अस्थायी.length();
    UnicodeString decompString;
    nfd->normalize(अस्थायी, decompString, status);
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }
    अगर (decompString.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    स्थिर char16_t *decomp=decompString.get_बफर();
    int32_t decompLen=decompString.length();

    // See if it matches the start of segment (at segmentPos)
    UBool ok = false;
    UChar32 cp;
    int32_t decompPos = 0;
    UChar32 decompCp;
    U16_अग्र(decomp, decompPos, decompLen, decompCp);

    int32_t i = segmentPos;
    जबतक(i < segLen) {
        U16_अग्र(segment, i, segLen, cp);

        अगर (cp == decompCp) { // if equal, eat another cp from decomp

            //if (PROGRESS) printf("  matches: %s\n", UToS(Tr(UnicodeString(cp))));

            अगर (decompPos == decompLen) { // done, have all decomp characters!
                अस्थायी.append(segment+i, segLen-i);
                ok = true;
                अवरोध;
            }
            U16_अग्र(decomp, decompPos, decompLen, decompCp);
        } अन्यथा {
            //if (PROGRESS) printf("  buffer: %s\n", UToS(Tr(UnicodeString(cp))));

            // brute force approach
            अस्थायी.append(cp);

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
    अगर (!ok)
        वापस nullptr; // we failed, characters left over

    //if (PROGRESS) printf("Matches\n");

    अगर (निवेश_Len == अस्थायी.length()) {
        fillin_परिणाम->दो(UnicodeString(), नया UnicodeString(), status);
        वापस fillin_परिणाम; // succeed, but no remainder
    }

    // brute force approach
    // check to make sure result is canonically equivalent
    UnicodeString trial;
    nfd->normalize(अस्थायी, trial, status);
    अगर(U_FAILURE(status) || trial.compare(segment+segmentPos, segLen - segmentPos) != 0) {
        वापस nullptr;
    }

    वापस getEquivalents2(fillin_परिणाम, अस्थायी.get_बफर()+निवेश_Len, अस्थायी.length()-निवेश_Len, status);
}

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_NORMALIZATION */
