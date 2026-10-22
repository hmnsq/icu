// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *****************************************************************************
 * Copyright (C) 1996-2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 *****************************************************************************
 */

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/caniter.h"
#अन्तर्भाव "unicode/normalizer2.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/usetiter.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "hash.h"
#अन्तर्भाव "normalizer2impl.h"

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

//लौकिकम्

U_NAMESPACE_BEGIN

// TODO: add boilerplate methods.

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(CanonicalIterator)


/**
 *@param source string to get results for
 */
CanonicalIterator::CanonicalIterator(ध्रुव UnicodeString &sourceStr, UError_संहिता &स्थितिः) :
    pieces(शून्यसूचकः),
    pieces_दीर्घता(0),
    pieces_lengths(शून्यसूचकः),
    current(शून्यसूचकः),
    current_दीर्घता(0),
    nfd(Normalizer2::getNFDInstance(स्थितिः)),
    nfcImpl(Normalizer2Factory::getNFCImpl(स्थितिः))
{
    यदि(U_SUCCESS(स्थितिः) && nfcImpl->ensure_Canon_Iter_दत्त(स्थितिः)) {
      समुच्चयः_Source(sourceStr, स्थितिः);
    }
}

CanonicalIterator::~CanonicalIterator() {
  cleanPieces();
}

शून्य CanonicalIterator::cleanPieces() {
    int32_t i = 0;
    यदि(pieces != शून्यसूचकः) {
        प्रति(i = 0; i < pieces_दीर्घता; i++) {
            यदि(pieces[i] != शून्यसूचकः) {
                विलोपय[] pieces[i];
            }
        }
        uprv_free(pieces);
        pieces = शून्यसूचकः;
        pieces_दीर्घता = 0;
    }
    यदि(pieces_lengths != शून्यसूचकः) {
        uprv_free(pieces_lengths);
        pieces_lengths = शून्यसूचकः;
    }
    यदि(current != शून्यसूचकः) {
        uprv_free(current);
        current = शून्यसूचकः;
        current_दीर्घता = 0;
    }
}

/**
 *@return gets the source: NOTE: it is the NFD form of source
 */
UnicodeString CanonicalIterator::getSource() {
  प्रत्यागम source;
}

/**
 * Resets the iterator so that one can start again from the beginning.
 */
शून्य CanonicalIterator::reset() {
    समाप्तम् = असत्य;
    प्रति (पूर्णाङ्क i = 0; i < current_दीर्घता; ++i) {
        current[i] = 0;
    }
}

/**
 *@return the next string that is canonically equivalent. The value null is returned when
 * the iteration is done.
 */
UnicodeString CanonicalIterator::next() {
    int32_t i = 0;

    यदि (समाप्तम्) {
      बफर.समुच्चयः_गन्तव्यम्_Bogus();
      प्रत्यागम बफर;
    }

    // delete old contents
    बफर.remove();

    // construct return value

    प्रति (i = 0; i < pieces_दीर्घता; ++i) {
        बफर.append(pieces[i][current[i]]);
    }
    //String result = buffer.toString(); // not needed

    // find next value for next time

    प्रति (i = current_दीर्घता - 1; ; --i) {
        यदि (i < 0) {
            समाप्तम् = सत्य;
            भङ्ग;
        }
        current[i]++;
        यदि (current[i] < pieces_lengths[i]) भङ्ग; // got sequence
        current[i] = 0;
    }
    प्रत्यागम बफर;
}

/**
 *@param set the source string to iterate against. This allows the same iterator to be used
 * while changing the source string, saving object creation.
 */
शून्य CanonicalIterator::समुच्चयः_Source(ध्रुव UnicodeString &newSource, UError_संहिता &स्थितिः) {
    int32_t list_दीर्घता_2 = 0;
    UChar32 cp = 0;
    int32_t start = 0;
    int32_t i = 0;
    UnicodeString *list = शून्यसूचकः;

    nfd->normalize(newSource, source, स्थितिः);
    यदि(U_FAILURE(स्थितिः)) {
      प्रत्यागम;
    }
    समाप्तम् = असत्य;

    cleanPieces();

    // catch degenerate case
    यदि (newSource.length() == 0) {
        pieces = static_cast<UnicodeString**>(uprv_malloc(परिमाण(UnicodeString*)));
        pieces_lengths = static_cast<int32_t*>(uprv_malloc(1 * परिमाण(int32_t)));
        pieces_दीर्घता = 1;
        current = static_cast<int32_t*>(uprv_malloc(1 * परिमाण(int32_t)));
        current_दीर्घता = 1;
        यदि (pieces == शून्यसूचकः || pieces_lengths == शून्यसूचकः || current == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            गच्छ CleanPartialInitialization;
        }
        current[0] = 0;
        pieces[0] = नूतन UnicodeString[1];
        pieces_lengths[0] = 1;
        यदि (pieces[0] == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            गच्छ CleanPartialInitialization;
        }
        प्रत्यागम;
    }


    list = नूतन UnicodeString[source.length()];
    यदि (list == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        गच्छ CleanPartialInitialization;
    }

    // i should initially be the number of code units at the 
    // start of the string
    i = U16_दीर्घता(source.char32At(0));
    // int32_t i = 1;
    // find the segments
    // This code iterates through the source string and 
    // extracts segments that end up on a codepoint that
    // doesn't start any decompositions. (Analysis is done
    // on the NFD form - see above).
    प्रति (; i < source.length(); i += U16_दीर्घता(cp)) {
        cp = source.char32At(i);
        यदि (nfcImpl->isCanonSegmentStarter(cp)) {
            source.extract(start, i-start, list[list_दीर्घता_2++]); // add up to i
            start = i;
        }
    }
    source.extract(start, i-start, list[list_दीर्घता_2++]); // add last one


    // allocate the arrays, and find the strings that are CE to each segment
    pieces = static_cast<UnicodeString**>(uprv_malloc(list_दीर्घता_2 * परिमाण(UnicodeString*)));
    pieces_दीर्घता = list_दीर्घता_2;
    pieces_lengths = static_cast<int32_t*>(uprv_malloc(list_दीर्घता_2 * परिमाण(int32_t)));
    current = static_cast<int32_t*>(uprv_malloc(list_दीर्घता_2 * परिमाण(int32_t)));
    current_दीर्घता = list_दीर्घता_2;
    यदि (pieces == शून्यसूचकः || pieces_lengths == शून्यसूचकः || current == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        गच्छ CleanPartialInitialization;
    }

    प्रति (i = 0; i < current_दीर्घता; i++) {
        current[i] = 0;
    }
    // for each segment, get all the combinations that can produce 
    // it after NFD normalization
    प्रति (i = 0; i < pieces_दीर्घता; ++i) {
        //if (PROGRESS) printf("SEGMENT\n");
        pieces[i] = getEquivalents(list[i], pieces_lengths[i], स्थितिः);
    }

    विलोपय[] list;
    प्रत्यागम;
// Common section to cleanup all local variables and reset object variables.
CleanPartialInitialization:
    विलोपय[] list;
    cleanPieces();
}

/**
 * Dumb recursive implementation of permutation.
 * TODO: optimize
 * @param source the string to find permutations for
 * @return the results in a set.
 */
शून्य U_EXPORT2 CanonicalIterator::permute(UnicodeString &source, UBool skipZeros, Hashtable *परिणाम, UError_संहिता &स्थितिः, int32_t गभीरता) {
    यदि(U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    // To avoid infinity loop caused by permute, we limit the depth of recursive
    // call to permute and return U_UNSUPPORTED_ERROR.
    // We know in some unit test we need at least 4. Set to 8 just in case some
    // unforseen use cases.
    constexpr int32_t k_Permute_गभीरता_सीमा = 8;
    यदि (गभीरता > k_Permute_गभीरता_सीमा) {
        स्थितिः = U_UNSUPPORTED_ERROR;
        प्रत्यागम;
    }
    //if (PROGRESS) printf("Permute: %s\n", UToS(Tr(source)));
    int32_t i = 0;

    // optimization:
    // if zero or one character, just return a set with it
    // we check for length < 2 to keep from counting code points all the time
    यदि (source.length() <= 2 && source.गणनम्_Char32() <= 1) {
        UnicodeString *गन्तव्यम्_Put = नूतन UnicodeString(source);
        /* test for nullptr */
        यदि (गन्तव्यम्_Put == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
        परिणाम->put(source, गन्तव्यम्_Put, स्थितिः);
        प्रत्यागम;
    }

    // otherwise iterate through the string, and recursively permute all the other characters
    UChar32 cp;
    Hashtable subpermute(स्थितिः);
    यदि(U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    subpermute.समुच्चयः_मानम्_Deleter(uprv_deleteUObject);

    प्रति (i = 0; i < source.length(); i += U16_दीर्घता(cp)) {
        cp = source.char32At(i);
        ध्रुव UHashElement *ne = शून्यसूचकः;
        int32_t el = UHASH_FIRST;
        UnicodeString subPermuteString = source;

        // optimization:
        // if the character is canonical combining class zero,
        // don't permute it
        यदि (skipZeros && i != 0 && u_getCombiningClass(cp) == 0) {
            //System.out.println("Skipping " + Utility.hex(UTF16.valueOf(source, i)));
            अनुवर्त;
        }

        subpermute.removeAll();

        // see what the permutations of the characters before and after this one are
        //Hashtable *subpermute = permute(source.substring(0,i) + source.substring(i + UTF16.getCharCount(cp)));
        permute(subPermuteString.remove(i, U16_दीर्घता(cp)), skipZeros, &subpermute, स्थितिः, गभीरता+1);
        /* Test for buffer overflows */
        यदि(U_FAILURE(स्थितिः)) {
            प्रत्यागम;
        }
        // The upper remove is destructive. The question is do we have to make a copy, or we don't care about the contents 
        // of source at this point.

        // prefix this character to all of them
        ne = subpermute.अग्र_Element(el);
        यावत् (ne != शून्यसूचकः) {
            UnicodeString* permRes = static_cast<UnicodeString*>(ne->value.pointer);
            UnicodeString *chStr = नूतन UnicodeString(cp);
            //test for nullptr
            यदि (chStr == शून्यसूचकः) {
                स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
            chStr->append(*permRes); //*((UnicodeString *)(ne->value.pointer));
            //if (PROGRESS) printf("  Piece: %s\n", UToS(*chStr));
            परिणाम->put(*chStr, chStr, स्थितिः);
            ne = subpermute.अग्र_Element(el);
        }
    }
    //return result;
}

// privates

// we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
UnicodeString* CanonicalIterator::getEquivalents(ध्रुव UnicodeString &segment, int32_t &परिणाम_len, UError_संहिता &स्थितिः) {
    Hashtable परिणाम(स्थितिः);
    Hashtable permutations(स्थितिः);
    Hashtable basic(स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    परिणाम.समुच्चयः_मानम्_Deleter(uprv_deleteUObject);
    permutations.समुच्चयः_मानम्_Deleter(uprv_deleteUObject);
    basic.समुच्चयः_मानम्_Deleter(uprv_deleteUObject);

    char16_t USeg[256];
    int32_t segLen = segment.extract(USeg, 256, स्थितिः);
    getEquivalents2(&basic, USeg, segLen, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    // now get all the permutations
    // add only the ones that are canonically equivalent
    // TODO: optimize by not permuting any class zero.

    ध्रुव UHashElement *ne = शून्यसूचकः;
    int32_t el = UHASH_FIRST;
    //Iterator it = basic.iterator();
    ne = basic.अग्र_Element(el);
    //while (it.hasNext())
    यावत् (ne != शून्यसूचकः) {
        //String item = (String) it.next();
        UnicodeString item = *static_cast<UnicodeString*>(ne->value.pointer);

        permutations.removeAll();
        permute(item, CANITER_SKIP_ZEROES, &permutations, स्थितिः);
        ध्रुव UHashElement *ne2 = शून्यसूचकः;
        int32_t el2 = UHASH_FIRST;
        //Iterator it2 = permutations.iterator();
        ne2 = permutations.अग्र_Element(el2);
        //while (it2.hasNext())
        यावत् (ne2 != शून्यसूचकः) {
            //String possible = (String) it2.next();
            //UnicodeString *possible = new UnicodeString(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString possible(*static_cast<UnicodeString*>(ne2->value.pointer));
            UnicodeString attempt;
            nfd->normalize(possible, attempt, स्थितिः);

            // TODO: check if operator == is semanticaly the same as attempt.equals(segment)
            यदि (attempt==segment) {
                //if (PROGRESS) printf("Adding Permutation: %s\n", UToS(Tr(*possible)));
                // TODO: use the hashtable just to catch duplicates - store strings directly (somehow).
                परिणाम.put(possible, नूतन UnicodeString(possible), स्थितिः); //add(possible);
            } अन्यथा {
                //if (PROGRESS) printf("-Skipping Permutation: %s\n", UToS(Tr(*possible)));
            }

            ne2 = permutations.अग्र_Element(el2);
        }
        ne = basic.अग्र_Element(el);
    }

    /* Test for buffer overflows */
    यदि(U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    // convert into a String[] to clean up storage
    //String[] finalResult = new String[result.size()];
    UnicodeString *final_परिणाम = शून्यसूचकः;
    int32_t परिणाम_गणनम्;
    यदि((परिणाम_गणनम् = परिणाम.count()) != 0) {
        final_परिणाम = नूतन UnicodeString[परिणाम_गणनम्];
        यदि (final_परिणाम == शून्यसूचकः) {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
    }
    अन्यथा {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    //result.toArray(finalResult);
    परिणाम_len = 0;
    el = UHASH_FIRST;
    ne = परिणाम.अग्र_Element(el);
    यावत्(ne != शून्यसूचकः) {
        final_परिणाम[परिणाम_len++] = *static_cast<UnicodeString*>(ne->value.pointer);
        ne = परिणाम.अग्र_Element(el);
    }


    प्रत्यागम final_परिणाम;
}

Hashtable* CanonicalIterator::getEquivalents2(Hashtable* fillin_परिणाम U_LIFETIME_BOUND,
                                              ध्रुव char16_t* segment,
                                              int32_t segLen,
                                              UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(segment)));

    UnicodeString गन्तव्यम्_Put(segment, segLen);

    fillin_परिणाम->put(गन्तव्यम्_Put, नूतन UnicodeString(गन्तव्यम्_Put), स्थितिः);

    Unicode_समुच्चयः starts;

    // cycle through all the characters
    UChar32 cp;
    प्रति (int32_t i = 0; i < segLen; i += U16_दीर्घता(cp)) {
        // see if any character is at the start of some decomposition
        U16_GET(segment, 0, i, segLen, cp);
        यदि (!nfcImpl->get_Canon_Start_समुच्चयः(cp, starts)) {
            अनुवर्त;
        }
        // if so, see which decompositions match
        Unicode_समुच्चयः_Iterator iter(starts);
        यावत् (iter.next()) {
            UChar32 cp2 = iter.getCodepoint();
            Hashtable remainder(स्थितिः);
            remainder.समुच्चयः_मानम्_Deleter(uprv_deleteUObject);
            यदि (extract(&remainder, cp2, segment, segLen, i, स्थितिः) == शून्यसूचकः) {
                यदि (U_FAILURE(स्थितिः)) {
                    प्रत्यागम शून्यसूचकः;
                }
                अनुवर्त;
            }

            // there were some matches, so add all the possibilities to the set.
            UnicodeString उपसर्गः(segment, i);
            उपसर्गः += cp2;

            int32_t el = UHASH_FIRST;
            ध्रुव UHashElement *ne = remainder.अग्र_Element(el);
            यावत् (ne != शून्यसूचकः) {
                UnicodeString item = *static_cast<UnicodeString*>(ne->value.pointer);
                UnicodeString *गन्तव्यम्_सङ्कलय = नूतन UnicodeString(उपसर्गः);
                /* test for nullptr */
                यदि (गन्तव्यम्_सङ्कलय == शून्यसूचकः) {
                    स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                    प्रत्यागम शून्यसूचकः;
                }
                *गन्तव्यम्_सङ्कलय += item;
                fillin_परिणाम->put(*गन्तव्यम्_सङ्कलय, गन्तव्यम्_सङ्कलय, स्थितिः);

                //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(*toAdd)));

                ne = remainder.अग्र_Element(el);
            }
            // ICU-22642 Guards against strings that have so many permutations
            // that they would otherwise hang the function.
            constexpr int32_t k_परिणाम_सीमा = 4096;
            यदि (fillin_परिणाम->count() > k_परिणाम_सीमा) {
                स्थितिः = U_UNSUPPORTED_ERROR;
                प्रत्यागम शून्यसूचकः;
            }
        }
    }

    /* Test for buffer overflows */
    यदि(U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम fillin_परिणाम;
}

/**
 * See if the decomposition of cp2 is at segment starting at segmentPos 
 * (with canonical rearrangement!)
 * If so, take the remainder, and return the equivalents 
 */
Hashtable* CanonicalIterator::extract(Hashtable* fillin_परिणाम U_LIFETIME_BOUND,
                                      UChar32 comp,
                                      ध्रुव char16_t* segment,
                                      int32_t segLen,
                                      int32_t segmentPos,
                                      UError_संहिता& स्थितिः) {
//Hashtable *CanonicalIterator::extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
    //if (PROGRESS) printf(" extract: %s, ", UToS(Tr(UnicodeString(comp))));
    //if (PROGRESS) printf("%s, %i\n", UToS(Tr(segment)), segmentPos);

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    UnicodeString अस्थायी(comp);
    int32_t निवेशः_Len=अस्थायी.length();
    UnicodeString decompString;
    nfd->normalize(अस्थायी, decompString, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (decompString.isBogus()) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव char16_t *decomp=decompString.get_बफर();
    int32_t decompLen=decompString.length();

    // See if it matches the start of segment (at segmentPos)
    UBool ok = असत्य;
    UChar32 cp;
    int32_t decompPos = 0;
    UChar32 decompCp;
    U16_अग्र(decomp, decompPos, decompLen, decompCp);

    int32_t i = segmentPos;
    यावत्(i < segLen) {
        U16_अग्र(segment, i, segLen, cp);

        यदि (cp == decompCp) { // if equal, eat another cp from decomp

            //if (PROGRESS) printf("  matches: %s\n", UToS(Tr(UnicodeString(cp))));

            यदि (decompPos == decompLen) { // done, have all decomp characters!
                अस्थायी.append(segment+i, segLen-i);
                ok = सत्य;
                भङ्ग;
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
    यदि (!ok)
        प्रत्यागम शून्यसूचकः; // we failed, characters left over

    //if (PROGRESS) printf("Matches\n");

    यदि (निवेशः_Len == अस्थायी.length()) {
        fillin_परिणाम->put(UnicodeString(), नूतन UnicodeString(), स्थितिः);
        प्रत्यागम fillin_परिणाम; // succeed, but no remainder
    }

    // brute force approach
    // check to make sure result is canonically equivalent
    UnicodeString trial;
    nfd->normalize(अस्थायी, trial, स्थितिः);
    यदि(U_FAILURE(स्थितिः) || trial.compare(segment+segmentPos, segLen - segmentPos) != 0) {
        प्रत्यागम शून्यसूचकः;
    }

    प्रत्यागम getEquivalents2(fillin_परिणाम, अस्थायी.get_बफर()+निवेशः_Len, अस्थायी.length()-निवेशः_Len, स्थितिः);
}

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_NORMALIZATION */
