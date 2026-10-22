// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *****************************************************************************
 * Copyright (C) 1996-2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 *****************************************************************************
 */

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_NORMALIZATION

#অন্তর্ভাব "unicode/caniter.h"
#অন্তর্ভাব "unicode/normalizer2.h"
#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/usetiter.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "hash.h"
#অন্তর্ভাব "normalizer2impl.h"

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
CanonicalIterator::CanonicalIterator(ধ্রুবক UnicodeString &sourceStr, UErrorCode &status) :
    pieces(nullptr),
    pieces_দৈর্ঘ্য(0),
    pieces_lengths(nullptr),
    current(nullptr),
    current_দৈর্ঘ্য(0),
    nfd(Normalizer2::getNFDInstance(status)),
    nfcImpl(Normalizer2Factory::getNFCImpl(status))
{
    যদি(U_SUCCESS(status) && nfcImpl->ensure_Canon_Iter_তথ্য(status)) {
      setSource(sourceStr, status);
    }
}

CanonicalIterator::~CanonicalIterator() {
  cleanPieces();
}

শূন্য CanonicalIterator::cleanPieces() {
    int32_t i = 0;
    যদি(pieces != nullptr) {
        জন্য(i = 0; i < pieces_দৈর্ঘ্য; i++) {
            যদি(pieces[i] != nullptr) {
                delete[] pieces[i];
            }
        }
        uprv_free(pieces);
        pieces = nullptr;
        pieces_দৈর্ঘ্য = 0;
    }
    যদি(pieces_lengths != nullptr) {
        uprv_free(pieces_lengths);
        pieces_lengths = nullptr;
    }
    যদি(current != nullptr) {
        uprv_free(current);
        current = nullptr;
        current_দৈর্ঘ্য = 0;
    }
}

/**
 *@return gets the source: NOTE: it is the NFD form of source
 */
UnicodeString CanonicalIterator::getSource() {
  ফেরত source;
}

/**
 * Resets the iterator so that one can start again from the beginning.
 */
শূন্য CanonicalIterator::reset() {
    done = মিথ্যা;
    জন্য (পূর্ণসংখ্যা i = 0; i < current_দৈর্ঘ্য; ++i) {
        current[i] = 0;
    }
}

/**
 *@return the next string that is canonically equivalent. The value null is returned when
 * the iteration is done.
 */
UnicodeString CanonicalIterator::next() {
    int32_t i = 0;

    যদি (done) {
      বাফার.setToBogus();
      ফেরত বাফার;
    }

    // delete old contents
    বাফার.remove();

    // construct return value

    জন্য (i = 0; i < pieces_দৈর্ঘ্য; ++i) {
        বাফার.append(pieces[i][current[i]]);
    }
    //String result = buffer.toString(); // not needed

    // find next value for next time

    জন্য (i = current_দৈর্ঘ্য - 1; ; --i) {
        যদি (i < 0) {
            done = সত্য;
            বিরতি;
        }
        current[i]++;
        যদি (current[i] < pieces_lengths[i]) বিরতি; // got sequence
        current[i] = 0;
    }
    ফেরত বাফার;
}

/**
 *@param set the source string to iterate against. This allows the same iterator to be used
 * while changing the source string, saving object creation.
 */
শূন্য CanonicalIterator::setSource(ধ্রুবক UnicodeString &newSource, UErrorCode &status) {
    int32_t list_দৈর্ঘ্য_2 = 0;
    UChar32 cp = 0;
    int32_t start = 0;
    int32_t i = 0;
    UnicodeString *list = nullptr;

    nfd->normalize(newSource, source, status);
    যদি(U_FAILURE(status)) {
      ফেরত;
    }
    done = মিথ্যা;

    cleanPieces();

    // catch degenerate case
    যদি (newSource.length() == 0) {
        pieces = static_cast<UnicodeString**>(uprv_malloc(আকার(UnicodeString*)));
        pieces_lengths = static_cast<int32_t*>(uprv_malloc(1 * আকার(int32_t)));
        pieces_দৈর্ঘ্য = 1;
        current = static_cast<int32_t*>(uprv_malloc(1 * আকার(int32_t)));
        current_দৈর্ঘ্য = 1;
        যদি (pieces == nullptr || pieces_lengths == nullptr || current == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            যাও CleanPartialInitialization;
        }
        current[0] = 0;
        pieces[0] = নতুন UnicodeString[1];
        pieces_lengths[0] = 1;
        যদি (pieces[0] == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            যাও CleanPartialInitialization;
        }
        ফেরত;
    }


    list = নতুন UnicodeString[source.length()];
    যদি (list == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        যাও CleanPartialInitialization;
    }

    // i should initially be the number of code units at the 
    // start of the string
    i = U16_দৈর্ঘ্য(source.char32At(0));
    // int32_t i = 1;
    // find the segments
    // This code iterates through the source string and 
    // extracts segments that end up on a codepoint that
    // doesn't start any decompositions. (Analysis is done
    // on the NFD form - see above).
    জন্য (; i < source.length(); i += U16_দৈর্ঘ্য(cp)) {
        cp = source.char32At(i);
        যদি (nfcImpl->isCanonSegmentStarter(cp)) {
            source.extract(start, i-start, list[list_দৈর্ঘ্য_2++]); // add up to i
            start = i;
        }
    }
    source.extract(start, i-start, list[list_দৈর্ঘ্য_2++]); // add last one


    // allocate the arrays, and find the strings that are CE to each segment
    pieces = static_cast<UnicodeString**>(uprv_malloc(list_দৈর্ঘ্য_2 * আকার(UnicodeString*)));
    pieces_দৈর্ঘ্য = list_দৈর্ঘ্য_2;
    pieces_lengths = static_cast<int32_t*>(uprv_malloc(list_দৈর্ঘ্য_2 * আকার(int32_t)));
    current = static_cast<int32_t*>(uprv_malloc(list_দৈর্ঘ্য_2 * আকার(int32_t)));
    current_দৈর্ঘ্য = list_দৈর্ঘ্য_2;
    যদি (pieces == nullptr || pieces_lengths == nullptr || current == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        যাও CleanPartialInitialization;
    }

    জন্য (i = 0; i < current_দৈর্ঘ্য; i++) {
        current[i] = 0;
    }
    // for each segment, get all the combinations that can produce 
    // it after NFD normalization
    জন্য (i = 0; i < pieces_দৈর্ঘ্য; ++i) {
        //if (PROGRESS) printf("SEGMENT\n");
        pieces[i] = getEquivalents(list[i], pieces_lengths[i], status);
    }

    delete[] list;
    ফেরত;
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
শূন্য U_EXPORT2 CanonicalIterator::permute(UnicodeString &source, UBool skipZeros, Hashtable *ফলাফল, UErrorCode &status, int32_t depth) {
    যদি(U_FAILURE(status)) {
        ফেরত;
    }
    // To avoid infinity loop caused by permute, we limit the depth of recursive
    // call to permute and return U_UNSUPPORTED_ERROR.
    // We know in some unit test we need at least 4. Set to 8 just in case some
    // unforseen use cases.
    constexpr int32_t kPermuteDepthLimit = 8;
    যদি (depth > kPermuteDepthLimit) {
        status = U_UNSUPPORTED_ERROR;
        ফেরত;
    }
    //if (PROGRESS) printf("Permute: %s\n", UToS(Tr(source)));
    int32_t i = 0;

    // optimization:
    // if zero or one character, just return a set with it
    // we check for length < 2 to keep from counting code points all the time
    যদি (source.length() <= 2 && source.গুনতি_Char32() <= 1) {
        UnicodeString *toPut = নতুন UnicodeString(source);
        /* test for nullptr */
        যদি (toPut == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
        ফলাফল->put(source, toPut, status);
        ফেরত;
    }

    // otherwise iterate through the string, and recursively permute all the other characters
    UChar32 cp;
    Hashtable subpermute(status);
    যদি(U_FAILURE(status)) {
        ফেরত;
    }
    subpermute.set_মান_Deleter(uprv_deleteUObject);

    জন্য (i = 0; i < source.length(); i += U16_দৈর্ঘ্য(cp)) {
        cp = source.char32At(i);
        ধ্রুবক UHashElement *ne = nullptr;
        int32_t el = UHASH_FIRST;
        UnicodeString subPermuteString = source;

        // optimization:
        // if the character is canonical combining class zero,
        // don't permute it
        যদি (skipZeros && i != 0 && u_getCombiningClass(cp) == 0) {
            //System.out.println("Skipping " + Utility.hex(UTF16.valueOf(source, i)));
            চালু;
        }

        subpermute.removeAll();

        // see what the permutations of the characters before and after this one are
        //Hashtable *subpermute = permute(source.substring(0,i) + source.substring(i + UTF16.getCharCount(cp)));
        permute(subPermuteString.remove(i, U16_দৈর্ঘ্য(cp)), skipZeros, &subpermute, status, depth+1);
        /* Test for buffer overflows */
        যদি(U_FAILURE(status)) {
            ফেরত;
        }
        // The upper remove is destructive. The question is do we have to make a copy, or we don't care about the contents 
        // of source at this point.

        // prefix this character to all of them
        ne = subpermute.পরবর্তী_Element(el);
        যখন (ne != nullptr) {
            UnicodeString* permRes = static_cast<UnicodeString*>(ne->value.pointer);
            UnicodeString *chStr = নতুন UnicodeString(cp);
            //test for nullptr
            যদি (chStr == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
            chStr->append(*permRes); //*((UnicodeString *)(ne->value.pointer));
            //if (PROGRESS) printf("  Piece: %s\n", UToS(*chStr));
            ফলাফল->put(*chStr, chStr, status);
            ne = subpermute.পরবর্তী_Element(el);
        }
    }
    //return result;
}

// privates

// we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
UnicodeString* CanonicalIterator::getEquivalents(ধ্রুবক UnicodeString &segment, int32_t &ফলাফল_len, UErrorCode &status) {
    Hashtable ফলাফল(status);
    Hashtable permutations(status);
    Hashtable basic(status);
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }
    ফলাফল.set_মান_Deleter(uprv_deleteUObject);
    permutations.set_মান_Deleter(uprv_deleteUObject);
    basic.set_মান_Deleter(uprv_deleteUObject);

    char16_t USeg[256];
    int32_t segLen = segment.extract(USeg, 256, status);
    getEquivalents2(&basic, USeg, segLen, status);
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    // now get all the permutations
    // add only the ones that are canonically equivalent
    // TODO: optimize by not permuting any class zero.

    ধ্রুবক UHashElement *ne = nullptr;
    int32_t el = UHASH_FIRST;
    //Iterator it = basic.iterator();
    ne = basic.পরবর্তী_Element(el);
    //while (it.hasNext())
    যখন (ne != nullptr) {
        //String item = (String) it.next();
        UnicodeString item = *static_cast<UnicodeString*>(ne->value.pointer);

        permutations.removeAll();
        permute(item, CANITER_SKIP_ZEROES, &permutations, status);
        ধ্রুবক UHashElement *ne2 = nullptr;
        int32_t el2 = UHASH_FIRST;
        //Iterator it2 = permutations.iterator();
        ne2 = permutations.পরবর্তী_Element(el2);
        //while (it2.hasNext())
        যখন (ne2 != nullptr) {
            //String possible = (String) it2.next();
            //UnicodeString *possible = new UnicodeString(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString possible(*static_cast<UnicodeString*>(ne2->value.pointer));
            UnicodeString attempt;
            nfd->normalize(possible, attempt, status);

            // TODO: check if operator == is semanticaly the same as attempt.equals(segment)
            যদি (attempt==segment) {
                //if (PROGRESS) printf("Adding Permutation: %s\n", UToS(Tr(*possible)));
                // TODO: use the hashtable just to catch duplicates - store strings directly (somehow).
                ফলাফল.put(possible, নতুন UnicodeString(possible), status); //add(possible);
            } অন্যথা {
                //if (PROGRESS) printf("-Skipping Permutation: %s\n", UToS(Tr(*possible)));
            }

            ne2 = permutations.পরবর্তী_Element(el2);
        }
        ne = basic.পরবর্তী_Element(el);
    }

    /* Test for buffer overflows */
    যদি(U_FAILURE(status)) {
        ফেরত nullptr;
    }
    // convert into a String[] to clean up storage
    //String[] finalResult = new String[result.size()];
    UnicodeString *final_ফলাফল = nullptr;
    int32_t ফলাফল_গুনতি;
    যদি((ফলাফল_গুনতি = ফলাফল.count()) != 0) {
        final_ফলাফল = নতুন UnicodeString[ফলাফল_গুনতি];
        যদি (final_ফলাফল == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত nullptr;
        }
    }
    অন্যথা {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    //result.toArray(finalResult);
    ফলাফল_len = 0;
    el = UHASH_FIRST;
    ne = ফলাফল.পরবর্তী_Element(el);
    যখন(ne != nullptr) {
        final_ফলাফল[ফলাফল_len++] = *static_cast<UnicodeString*>(ne->value.pointer);
        ne = ফলাফল.পরবর্তী_Element(el);
    }


    ফেরত final_ফলাফল;
}

Hashtable* CanonicalIterator::getEquivalents2(Hashtable* fillin_ফলাফল U_LIFETIME_BOUND,
                                              ধ্রুবক char16_t* segment,
                                              int32_t segLen,
                                              UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(segment)));

    UnicodeString toPut(segment, segLen);

    fillin_ফলাফল->put(toPut, নতুন UnicodeString(toPut), status);

    UnicodeSet starts;

    // cycle through all the characters
    UChar32 cp;
    জন্য (int32_t i = 0; i < segLen; i += U16_দৈর্ঘ্য(cp)) {
        // see if any character is at the start of some decomposition
        U16_GET(segment, 0, i, segLen, cp);
        যদি (!nfcImpl->getCanonStartSet(cp, starts)) {
            চালু;
        }
        // if so, see which decompositions match
        UnicodeSetIterator iter(starts);
        যখন (iter.next()) {
            UChar32 cp2 = iter.getCodepoint();
            Hashtable remainder(status);
            remainder.set_মান_Deleter(uprv_deleteUObject);
            যদি (extract(&remainder, cp2, segment, segLen, i, status) == nullptr) {
                যদি (U_FAILURE(status)) {
                    ফেরত nullptr;
                }
                চালু;
            }

            // there were some matches, so add all the possibilities to the set.
            UnicodeString prefix(segment, i);
            prefix += cp2;

            int32_t el = UHASH_FIRST;
            ধ্রুবক UHashElement *ne = remainder.পরবর্তী_Element(el);
            যখন (ne != nullptr) {
                UnicodeString item = *static_cast<UnicodeString*>(ne->value.pointer);
                UnicodeString *toAdd = নতুন UnicodeString(prefix);
                /* test for nullptr */
                যদি (toAdd == nullptr) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    ফেরত nullptr;
                }
                *toAdd += item;
                fillin_ফলাফল->put(*toAdd, toAdd, status);

                //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(*toAdd)));

                ne = remainder.পরবর্তী_Element(el);
            }
            // ICU-22642 Guards against strings that have so many permutations
            // that they would otherwise hang the function.
            constexpr int32_t k_ফলাফল_Limit = 4096;
            যদি (fillin_ফলাফল->count() > k_ফলাফল_Limit) {
                status = U_UNSUPPORTED_ERROR;
                ফেরত nullptr;
            }
        }
    }

    /* Test for buffer overflows */
    যদি(U_FAILURE(status)) {
        ফেরত nullptr;
    }
    ফেরত fillin_ফলাফল;
}

/**
 * See if the decomposition of cp2 is at segment starting at segmentPos 
 * (with canonical rearrangement!)
 * If so, take the remainder, and return the equivalents 
 */
Hashtable* CanonicalIterator::extract(Hashtable* fillin_ফলাফল U_LIFETIME_BOUND,
                                      UChar32 comp,
                                      ধ্রুবক char16_t* segment,
                                      int32_t segLen,
                                      int32_t segmentPos,
                                      UErrorCode& status) {
//Hashtable *CanonicalIterator::extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
    //if (PROGRESS) printf(" extract: %s, ", UToS(Tr(UnicodeString(comp))));
    //if (PROGRESS) printf("%s, %i\n", UToS(Tr(segment)), segmentPos);

    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }

    UnicodeString অস্থায়ী(comp);
    int32_t প্রবেশ_Len=অস্থায়ী.length();
    UnicodeString decompString;
    nfd->normalize(অস্থায়ী, decompString, status);
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }
    যদি (decompString.isBogus()) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    ধ্রুবক char16_t *decomp=decompString.get_বাফার();
    int32_t decompLen=decompString.length();

    // See if it matches the start of segment (at segmentPos)
    UBool ok = মিথ্যা;
    UChar32 cp;
    int32_t decompPos = 0;
    UChar32 decompCp;
    U16_পরবর্তী(decomp, decompPos, decompLen, decompCp);

    int32_t i = segmentPos;
    যখন(i < segLen) {
        U16_পরবর্তী(segment, i, segLen, cp);

        যদি (cp == decompCp) { // if equal, eat another cp from decomp

            //if (PROGRESS) printf("  matches: %s\n", UToS(Tr(UnicodeString(cp))));

            যদি (decompPos == decompLen) { // done, have all decomp characters!
                অস্থায়ী.append(segment+i, segLen-i);
                ok = সত্য;
                বিরতি;
            }
            U16_পরবর্তী(decomp, decompPos, decompLen, decompCp);
        } অন্যথা {
            //if (PROGRESS) printf("  buffer: %s\n", UToS(Tr(UnicodeString(cp))));

            // brute force approach
            অস্থায়ী.append(cp);

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
    যদি (!ok)
        ফেরত nullptr; // we failed, characters left over

    //if (PROGRESS) printf("Matches\n");

    যদি (প্রবেশ_Len == অস্থায়ী.length()) {
        fillin_ফলাফল->put(UnicodeString(), নতুন UnicodeString(), status);
        ফেরত fillin_ফলাফল; // succeed, but no remainder
    }

    // brute force approach
    // check to make sure result is canonically equivalent
    UnicodeString trial;
    nfd->normalize(অস্থায়ী, trial, status);
    যদি(U_FAILURE(status) || trial.compare(segment+segmentPos, segLen - segmentPos) != 0) {
        ফেরত nullptr;
    }

    ফেরত getEquivalents2(fillin_ফলাফল, অস্থায়ী.get_বাফার()+প্রবেশ_Len, অস্থায়ী.length()-প্রবেশ_Len, status);
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_NORMALIZATION */
