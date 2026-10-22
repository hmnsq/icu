// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *****************************************************************************
 * Copyright (C) 1996-2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 *****************************************************************************
 */

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_NORMALIZATION

#അന്തര്ഭാവ "unicode/caniter.h"
#അന്തര്ഭാവ "unicode/normalizer2.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/usetiter.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "hash.h"
#അന്തര്ഭാവ "normalizer2impl.h"

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
CanonicalIterator::CanonicalIterator(സ്ഥിരം UnicodeString &sourceStr, UErrorCode &status) :
    pieces(nullptr),
    pieces_length(0),
    pieces_lengths(nullptr),
    current(nullptr),
    current_length(0),
    nfd(Normalizer2::getNFDInstance(status)),
    nfcImpl(Normalizer2Factory::getNFCImpl(status))
{
    എങ്കിൽ(U_SUCCESS(status) && nfcImpl->ensureCanonIterData(status)) {
      setSource(sourceStr, status);
    }
}

CanonicalIterator::~CanonicalIterator() {
  cleanPieces();
}

ശൂന്യം CanonicalIterator::cleanPieces() {
    int32_t i = 0;
    എങ്കിൽ(pieces != nullptr) {
        വേണ്ടി(i = 0; i < pieces_length; i++) {
            എങ്കിൽ(pieces[i] != nullptr) {
                delete[] pieces[i];
            }
        }
        uprv_free(pieces);
        pieces = nullptr;
        pieces_length = 0;
    }
    എങ്കിൽ(pieces_lengths != nullptr) {
        uprv_free(pieces_lengths);
        pieces_lengths = nullptr;
    }
    എങ്കിൽ(current != nullptr) {
        uprv_free(current);
        current = nullptr;
        current_length = 0;
    }
}

/**
 *@return gets the source: NOTE: it is the NFD form of source
 */
UnicodeString CanonicalIterator::getSource() {
  തിരികെ source;
}

/**
 * Resets the iterator so that one can start again from the beginning.
 */
ശൂന്യം CanonicalIterator::reset() {
    done = അസത്യം;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < current_length; ++i) {
        current[i] = 0;
    }
}

/**
 *@return the next string that is canonically equivalent. The value null is returned when
 * the iteration is done.
 */
UnicodeString CanonicalIterator::next() {
    int32_t i = 0;

    എങ്കിൽ (done) {
      buffer.setToBogus();
      തിരികെ buffer;
    }

    // delete old contents
    buffer.remove();

    // construct return value

    വേണ്ടി (i = 0; i < pieces_length; ++i) {
        buffer.append(pieces[i][current[i]]);
    }
    //String result = buffer.toString(); // not needed

    // find next value for next time

    വേണ്ടി (i = current_length - 1; ; --i) {
        എങ്കിൽ (i < 0) {
            done = സത്യം;
            നിർത്തുക;
        }
        current[i]++;
        എങ്കിൽ (current[i] < pieces_lengths[i]) നിർത്തുക; // got sequence
        current[i] = 0;
    }
    തിരികെ buffer;
}

/**
 *@param set the source string to iterate against. This allows the same iterator to be used
 * while changing the source string, saving object creation.
 */
ശൂന്യം CanonicalIterator::setSource(സ്ഥിരം UnicodeString &newSource, UErrorCode &status) {
    int32_t list_length = 0;
    UChar32 cp = 0;
    int32_t start = 0;
    int32_t i = 0;
    UnicodeString *list = nullptr;

    nfd->normalize(newSource, source, status);
    എങ്കിൽ(U_FAILURE(status)) {
      തിരികെ;
    }
    done = അസത്യം;

    cleanPieces();

    // catch degenerate case
    എങ്കിൽ (newSource.length() == 0) {
        pieces = static_cast<UnicodeString**>(uprv_malloc(വലുപ്പം(UnicodeString*)));
        pieces_lengths = static_cast<int32_t*>(uprv_malloc(1 * വലുപ്പം(int32_t)));
        pieces_length = 1;
        current = static_cast<int32_t*>(uprv_malloc(1 * വലുപ്പം(int32_t)));
        current_length = 1;
        എങ്കിൽ (pieces == nullptr || pieces_lengths == nullptr || current == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            പോകുക CleanPartialInitialization;
        }
        current[0] = 0;
        pieces[0] = പുതിയ UnicodeString[1];
        pieces_lengths[0] = 1;
        എങ്കിൽ (pieces[0] == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            പോകുക CleanPartialInitialization;
        }
        തിരികെ;
    }


    list = പുതിയ UnicodeString[source.length()];
    എങ്കിൽ (list == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        പോകുക CleanPartialInitialization;
    }

    // i should initially be the number of code units at the 
    // start of the string
    i = U16_LENGTH(source.char32At(0));
    // int32_t i = 1;
    // find the segments
    // This code iterates through the source string and 
    // extracts segments that end up on a codepoint that
    // doesn't start any decompositions. (Analysis is done
    // on the NFD form - see above).
    വേണ്ടി (; i < source.length(); i += U16_LENGTH(cp)) {
        cp = source.char32At(i);
        എങ്കിൽ (nfcImpl->isCanonSegmentStarter(cp)) {
            source.extract(start, i-start, list[list_length++]); // add up to i
            start = i;
        }
    }
    source.extract(start, i-start, list[list_length++]); // add last one


    // allocate the arrays, and find the strings that are CE to each segment
    pieces = static_cast<UnicodeString**>(uprv_malloc(list_length * വലുപ്പം(UnicodeString*)));
    pieces_length = list_length;
    pieces_lengths = static_cast<int32_t*>(uprv_malloc(list_length * വലുപ്പം(int32_t)));
    current = static_cast<int32_t*>(uprv_malloc(list_length * വലുപ്പം(int32_t)));
    current_length = list_length;
    എങ്കിൽ (pieces == nullptr || pieces_lengths == nullptr || current == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        പോകുക CleanPartialInitialization;
    }

    വേണ്ടി (i = 0; i < current_length; i++) {
        current[i] = 0;
    }
    // for each segment, get all the combinations that can produce 
    // it after NFD normalization
    വേണ്ടി (i = 0; i < pieces_length; ++i) {
        //if (PROGRESS) printf("SEGMENT\n");
        pieces[i] = getEquivalents(list[i], pieces_lengths[i], status);
    }

    delete[] list;
    തിരികെ;
// Common section to cleanup all local variables and reset object variables.
CleanPartialInitialization:
    delete[] list;
    cleanPieces();
}

/**
 * Dumb recursive implementation of permutation.
 * TODO: optimize
 * @param source the string to find permutations for
 * @return the results in a set.
 */
ശൂന്യം U_EXPORT2 CanonicalIterator::permute(UnicodeString &source, UBool skipZeros, Hashtable *result, UErrorCode &status, int32_t depth) {
    എങ്കിൽ(U_FAILURE(status)) {
        തിരികെ;
    }
    // To avoid infinity loop caused by permute, we limit the depth of recursive
    // call to permute and return U_UNSUPPORTED_ERROR.
    // We know in some unit test we need at least 4. Set to 8 just in case some
    // unforseen use cases.
    constexpr int32_t kPermuteDepthLimit = 8;
    എങ്കിൽ (depth > kPermuteDepthLimit) {
        status = U_UNSUPPORTED_ERROR;
        തിരികെ;
    }
    //if (PROGRESS) printf("Permute: %s\n", UToS(Tr(source)));
    int32_t i = 0;

    // optimization:
    // if zero or one character, just return a set with it
    // we check for length < 2 to keep from counting code points all the time
    എങ്കിൽ (source.length() <= 2 && source.എണ്ണം_Char32() <= 1) {
        UnicodeString *toPut = പുതിയ UnicodeString(source);
        /* test for nullptr */
        എങ്കിൽ (toPut == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
        result->put(source, toPut, status);
        തിരികെ;
    }

    // otherwise iterate through the string, and recursively permute all the other characters
    UChar32 cp;
    Hashtable subpermute(status);
    എങ്കിൽ(U_FAILURE(status)) {
        തിരികെ;
    }
    subpermute.set_മൂല്യം_Deleter(uprv_deleteUObject);

    വേണ്ടി (i = 0; i < source.length(); i += U16_LENGTH(cp)) {
        cp = source.char32At(i);
        സ്ഥിരം UHashElement *ne = nullptr;
        int32_t el = UHASH_FIRST;
        UnicodeString subPermuteString = source;

        // optimization:
        // if the character is canonical combining class zero,
        // don't permute it
        എങ്കിൽ (skipZeros && i != 0 && u_getCombiningClass(cp) == 0) {
            //System.out.println("Skipping " + Utility.hex(UTF16.valueOf(source, i)));
            തുടരുക;
        }

        subpermute.removeAll();

        // see what the permutations of the characters before and after this one are
        //Hashtable *subpermute = permute(source.substring(0,i) + source.substring(i + UTF16.getCharCount(cp)));
        permute(subPermuteString.remove(i, U16_LENGTH(cp)), skipZeros, &subpermute, status, depth+1);
        /* Test for buffer overflows */
        എങ്കിൽ(U_FAILURE(status)) {
            തിരികെ;
        }
        // The upper remove is destructive. The question is do we have to make a copy, or we don't care about the contents 
        // of source at this point.

        // prefix this character to all of them
        ne = subpermute.nextElement(el);
        ആകുമ്പോൾ (ne != nullptr) {
            UnicodeString* permRes = static_cast<UnicodeString*>(ne->value.pointer);
            UnicodeString *chStr = പുതിയ UnicodeString(cp);
            //test for nullptr
            എങ്കിൽ (chStr == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ;
            }
            chStr->append(*permRes); //*((UnicodeString *)(ne->value.pointer));
            //if (PROGRESS) printf("  Piece: %s\n", UToS(*chStr));
            result->put(*chStr, chStr, status);
            ne = subpermute.nextElement(el);
        }
    }
    //return result;
}

// privates

// we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
UnicodeString* CanonicalIterator::getEquivalents(സ്ഥിരം UnicodeString &segment, int32_t &result_len, UErrorCode &status) {
    Hashtable result(status);
    Hashtable permutations(status);
    Hashtable basic(status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    result.set_മൂല്യം_Deleter(uprv_deleteUObject);
    permutations.set_മൂല്യം_Deleter(uprv_deleteUObject);
    basic.set_മൂല്യം_Deleter(uprv_deleteUObject);

    char16_t USeg[256];
    int32_t segLen = segment.extract(USeg, 256, status);
    getEquivalents2(&basic, USeg, segLen, status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    // now get all the permutations
    // add only the ones that are canonically equivalent
    // TODO: optimize by not permuting any class zero.

    സ്ഥിരം UHashElement *ne = nullptr;
    int32_t el = UHASH_FIRST;
    //Iterator it = basic.iterator();
    ne = basic.nextElement(el);
    //while (it.hasNext())
    ആകുമ്പോൾ (ne != nullptr) {
        //String item = (String) it.next();
        UnicodeString item = *static_cast<UnicodeString*>(ne->value.pointer);

        permutations.removeAll();
        permute(item, CANITER_SKIP_ZEROES, &permutations, status);
        സ്ഥിരം UHashElement *ne2 = nullptr;
        int32_t el2 = UHASH_FIRST;
        //Iterator it2 = permutations.iterator();
        ne2 = permutations.nextElement(el2);
        //while (it2.hasNext())
        ആകുമ്പോൾ (ne2 != nullptr) {
            //String possible = (String) it2.next();
            //UnicodeString *possible = new UnicodeString(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString possible(*static_cast<UnicodeString*>(ne2->value.pointer));
            UnicodeString attempt;
            nfd->normalize(possible, attempt, status);

            // TODO: check if operator == is semanticaly the same as attempt.equals(segment)
            എങ്കിൽ (attempt==segment) {
                //if (PROGRESS) printf("Adding Permutation: %s\n", UToS(Tr(*possible)));
                // TODO: use the hashtable just to catch duplicates - store strings directly (somehow).
                result.put(possible, പുതിയ UnicodeString(possible), status); //add(possible);
            } അല്ലെങ്കിൽ {
                //if (PROGRESS) printf("-Skipping Permutation: %s\n", UToS(Tr(*possible)));
            }

            ne2 = permutations.nextElement(el2);
        }
        ne = basic.nextElement(el);
    }

    /* Test for buffer overflows */
    എങ്കിൽ(U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    // convert into a String[] to clean up storage
    //String[] finalResult = new String[result.size()];
    UnicodeString *finalResult = nullptr;
    int32_t result_എണ്ണം;
    എങ്കിൽ((result_എണ്ണം = result.count()) != 0) {
        finalResult = പുതിയ UnicodeString[result_എണ്ണം];
        എങ്കിൽ (finalResult == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ nullptr;
        }
    }
    അല്ലെങ്കിൽ {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    //result.toArray(finalResult);
    result_len = 0;
    el = UHASH_FIRST;
    ne = result.nextElement(el);
    ആകുമ്പോൾ(ne != nullptr) {
        finalResult[result_len++] = *static_cast<UnicodeString*>(ne->value.pointer);
        ne = result.nextElement(el);
    }


    തിരികെ finalResult;
}

Hashtable* CanonicalIterator::getEquivalents2(Hashtable* fillinResult U_LIFETIME_BOUND,
                                              സ്ഥിരം char16_t* segment,
                                              int32_t segLen,
                                              UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(segment)));

    UnicodeString toPut(segment, segLen);

    fillinResult->put(toPut, പുതിയ UnicodeString(toPut), status);

    UnicodeSet starts;

    // cycle through all the characters
    UChar32 cp;
    വേണ്ടി (int32_t i = 0; i < segLen; i += U16_LENGTH(cp)) {
        // see if any character is at the start of some decomposition
        U16_GET(segment, 0, i, segLen, cp);
        എങ്കിൽ (!nfcImpl->getCanonStartSet(cp, starts)) {
            തുടരുക;
        }
        // if so, see which decompositions match
        UnicodeSetIterator iter(starts);
        ആകുമ്പോൾ (iter.next()) {
            UChar32 cp2 = iter.getCodepoint();
            Hashtable remainder(status);
            remainder.set_മൂല്യം_Deleter(uprv_deleteUObject);
            എങ്കിൽ (extract(&remainder, cp2, segment, segLen, i, status) == nullptr) {
                എങ്കിൽ (U_FAILURE(status)) {
                    തിരികെ nullptr;
                }
                തുടരുക;
            }

            // there were some matches, so add all the possibilities to the set.
            UnicodeString prefix(segment, i);
            prefix += cp2;

            int32_t el = UHASH_FIRST;
            സ്ഥിരം UHashElement *ne = remainder.nextElement(el);
            ആകുമ്പോൾ (ne != nullptr) {
                UnicodeString item = *static_cast<UnicodeString*>(ne->value.pointer);
                UnicodeString *toAdd = പുതിയ UnicodeString(prefix);
                /* test for nullptr */
                എങ്കിൽ (toAdd == nullptr) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    തിരികെ nullptr;
                }
                *toAdd += item;
                fillinResult->put(*toAdd, toAdd, status);

                //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(*toAdd)));

                ne = remainder.nextElement(el);
            }
            // ICU-22642 Guards against strings that have so many permutations
            // that they would otherwise hang the function.
            constexpr int32_t kResultLimit = 4096;
            എങ്കിൽ (fillinResult->count() > kResultLimit) {
                status = U_UNSUPPORTED_ERROR;
                തിരികെ nullptr;
            }
        }
    }

    /* Test for buffer overflows */
    എങ്കിൽ(U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    തിരികെ fillinResult;
}

/**
 * See if the decomposition of cp2 is at segment starting at segmentPos 
 * (with canonical rearrangement!)
 * If so, take the remainder, and return the equivalents 
 */
Hashtable* CanonicalIterator::extract(Hashtable* fillinResult U_LIFETIME_BOUND,
                                      UChar32 comp,
                                      സ്ഥിരം char16_t* segment,
                                      int32_t segLen,
                                      int32_t segmentPos,
                                      UErrorCode& status) {
//Hashtable *CanonicalIterator::extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
    //if (PROGRESS) printf(" extract: %s, ", UToS(Tr(UnicodeString(comp))));
    //if (PROGRESS) printf("%s, %i\n", UToS(Tr(segment)), segmentPos);

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    UnicodeString temp(comp);
    int32_t നിവേശം_Len=temp.length();
    UnicodeString decompString;
    nfd->normalize(temp, decompString, status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ (decompString.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    സ്ഥിരം char16_t *decomp=decompString.getBuffer();
    int32_t decompLen=decompString.length();

    // See if it matches the start of segment (at segmentPos)
    UBool ok = അസത്യം;
    UChar32 cp;
    int32_t decompPos = 0;
    UChar32 decompCp;
    U16_NEXT(decomp, decompPos, decompLen, decompCp);

    int32_t i = segmentPos;
    ആകുമ്പോൾ(i < segLen) {
        U16_NEXT(segment, i, segLen, cp);

        എങ്കിൽ (cp == decompCp) { // if equal, eat another cp from decomp

            //if (PROGRESS) printf("  matches: %s\n", UToS(Tr(UnicodeString(cp))));

            എങ്കിൽ (decompPos == decompLen) { // done, have all decomp characters!
                temp.append(segment+i, segLen-i);
                ok = സത്യം;
                നിർത്തുക;
            }
            U16_NEXT(decomp, decompPos, decompLen, decompCp);
        } അല്ലെങ്കിൽ {
            //if (PROGRESS) printf("  buffer: %s\n", UToS(Tr(UnicodeString(cp))));

            // brute force approach
            temp.append(cp);

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
    എങ്കിൽ (!ok)
        തിരികെ nullptr; // we failed, characters left over

    //if (PROGRESS) printf("Matches\n");

    എങ്കിൽ (നിവേശം_Len == temp.length()) {
        fillinResult->put(UnicodeString(), പുതിയ UnicodeString(), status);
        തിരികെ fillinResult; // succeed, but no remainder
    }

    // brute force approach
    // check to make sure result is canonically equivalent
    UnicodeString trial;
    nfd->normalize(temp, trial, status);
    എങ്കിൽ(U_FAILURE(status) || trial.compare(segment+segmentPos, segLen - segmentPos) != 0) {
        തിരികെ nullptr;
    }

    തിരികെ getEquivalents2(fillinResult, temp.getBuffer()+നിവേശം_Len, temp.length()-നിവേശം_Len, status);
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */
