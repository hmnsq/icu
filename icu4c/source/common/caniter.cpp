// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *****************************************************************************
 * Copyright (C) 1996-2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 *****************************************************************************
 */

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_NORMALIZATION

#అన్తర్భావ "unicode/caniter.h"
#అన్తర్భావ "unicode/normalizer2.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/usetiter.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "hash.h"
#అన్తర్భావ "normalizer2impl.h"

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

//ప్రజా

U_NAMESPACE_BEGIN

// TODO: add boilerplate methods.

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(CanonicalIterator)


/**
 *@param source string to get results for
 */
CanonicalIterator::CanonicalIterator(స్థిరరాశి UnicodeString &ఆధారం_Str, UError_సంకేతం &స్థితి_2) :
    pieces(శూన్యసూచిక),
    pieces_నిడివి(0),
    pieces_lengths(శూన్యసూచిక),
    current(శూన్యసూచిక),
    current_నిడివి(0),
    nfd(Normalizer2::getNFDInstance(స్థితి_2)),
    nfcImpl(Normalizer2Factory::getNFCImpl(స్థితి_2))
{
    ఒకవేళ(U_SUCCESS(స్థితి_2) && nfcImpl->ensure_Canon_Iter_దత్తాంశం(స్థితి_2)) {
      సమితి_ఆధారం(ఆధారం_Str, స్థితి_2);
    }
}

CanonicalIterator::~CanonicalIterator() {
  cleanPieces();
}

శూన్య CanonicalIterator::cleanPieces() {
    int32_t i = 0;
    ఒకవేళ(pieces != శూన్యసూచిక) {
        కొరకు(i = 0; i < pieces_నిడివి; i++) {
            ఒకవేళ(pieces[i] != శూన్యసూచిక) {
                తొలగించు[] pieces[i];
            }
        }
        uprv_free(pieces);
        pieces = శూన్యసూచిక;
        pieces_నిడివి = 0;
    }
    ఒకవేళ(pieces_lengths != శూన్యసూచిక) {
        uprv_free(pieces_lengths);
        pieces_lengths = శూన్యసూచిక;
    }
    ఒకవేళ(current != శూన్యసూచిక) {
        uprv_free(current);
        current = శూన్యసూచిక;
        current_నిడివి = 0;
    }
}

/**
 *@return gets the source: NOTE: it is the NFD form of source
 */
UnicodeString CanonicalIterator::get_ఆధారం() {
  తిరిగి ఆధారం;
}

/**
 * Resets the iterator so that one can start again from the beginning.
 */
శూన్య CanonicalIterator::reset() {
    పూర్తి = అబద్ధం;
    కొరకు (పూర్ణాంకం i = 0; i < current_నిడివి; ++i) {
        current[i] = 0;
    }
}

/**
 *@return the next string that is canonically equivalent. The value null is returned when
 * the iteration is done.
 */
UnicodeString CanonicalIterator::next() {
    int32_t i = 0;

    ఒకవేళ (పూర్తి) {
      బఫరు.సమితి_గమ్యం_Bogus();
      తిరిగి బఫరు;
    }

    // delete old contents
    బఫరు.remove();

    // construct return value

    కొరకు (i = 0; i < pieces_నిడివి; ++i) {
        బఫరు.append(pieces[i][current[i]]);
    }
    //String result = buffer.toString(); // not needed

    // find next value for next time

    కొరకు (i = current_నిడివి - 1; ; --i) {
        ఒకవేళ (i < 0) {
            పూర్తి = నిజం;
            ఆపు;
        }
        current[i]++;
        ఒకవేళ (current[i] < pieces_lengths[i]) ఆపు; // got sequence
        current[i] = 0;
    }
    తిరిగి బఫరు;
}

/**
 *@param set the source string to iterate against. This allows the same iterator to be used
 * while changing the source string, saving object creation.
 */
శూన్య CanonicalIterator::సమితి_ఆధారం(స్థిరరాశి UnicodeString &new_ఆధారం, UError_సంకేతం &స్థితి_2) {
    int32_t list_నిడివి_2 = 0;
    UChar32 cp = 0;
    int32_t ప్రారంభం = 0;
    int32_t i = 0;
    UnicodeString *list = శూన్యసూచిక;

    nfd->normalize(new_ఆధారం, ఆధారం, స్థితి_2);
    ఒకవేళ(U_FAILURE(స్థితి_2)) {
      తిరిగి;
    }
    పూర్తి = అబద్ధం;

    cleanPieces();

    // catch degenerate case
    ఒకవేళ (new_ఆధారం.length() == 0) {
        pieces = static_cast<UnicodeString**>(uprv_malloc(పరిమాణం(UnicodeString*)));
        pieces_lengths = static_cast<int32_t*>(uprv_malloc(1 * పరిమాణం(int32_t)));
        pieces_నిడివి = 1;
        current = static_cast<int32_t*>(uprv_malloc(1 * పరిమాణం(int32_t)));
        current_నిడివి = 1;
        ఒకవేళ (pieces == శూన్యసూచిక || pieces_lengths == శూన్యసూచిక || current == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            వెళ్ళు CleanPartialInitialization;
        }
        current[0] = 0;
        pieces[0] = కొత్త UnicodeString[1];
        pieces_lengths[0] = 1;
        ఒకవేళ (pieces[0] == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            వెళ్ళు CleanPartialInitialization;
        }
        తిరిగి;
    }


    list = కొత్త UnicodeString[ఆధారం.length()];
    ఒకవేళ (list == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        వెళ్ళు CleanPartialInitialization;
    }

    // i should initially be the number of code units at the 
    // start of the string
    i = U16_నిడివి(ఆధారం.char32At(0));
    // int32_t i = 1;
    // find the segments
    // This code iterates through the source string and 
    // extracts segments that end up on a codepoint that
    // doesn't start any decompositions. (Analysis is done
    // on the NFD form - see above).
    కొరకు (; i < ఆధారం.length(); i += U16_నిడివి(cp)) {
        cp = ఆధారం.char32At(i);
        ఒకవేళ (nfcImpl->isCanonSegmentStarter(cp)) {
            ఆధారం.extract(ప్రారంభం, i-ప్రారంభం, list[list_నిడివి_2++]); // add up to i
            ప్రారంభం = i;
        }
    }
    ఆధారం.extract(ప్రారంభం, i-ప్రారంభం, list[list_నిడివి_2++]); // add last one


    // allocate the arrays, and find the strings that are CE to each segment
    pieces = static_cast<UnicodeString**>(uprv_malloc(list_నిడివి_2 * పరిమాణం(UnicodeString*)));
    pieces_నిడివి = list_నిడివి_2;
    pieces_lengths = static_cast<int32_t*>(uprv_malloc(list_నిడివి_2 * పరిమాణం(int32_t)));
    current = static_cast<int32_t*>(uprv_malloc(list_నిడివి_2 * పరిమాణం(int32_t)));
    current_నిడివి = list_నిడివి_2;
    ఒకవేళ (pieces == శూన్యసూచిక || pieces_lengths == శూన్యసూచిక || current == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        వెళ్ళు CleanPartialInitialization;
    }

    కొరకు (i = 0; i < current_నిడివి; i++) {
        current[i] = 0;
    }
    // for each segment, get all the combinations that can produce 
    // it after NFD normalization
    కొరకు (i = 0; i < pieces_నిడివి; ++i) {
        //if (PROGRESS) printf("SEGMENT\n");
        pieces[i] = getEquivalents(list[i], pieces_lengths[i], స్థితి_2);
    }

    తొలగించు[] list;
    తిరిగి;
// Common section to cleanup all local variables and reset object variables.
CleanPartialInitialization:
    తొలగించు[] list;
    cleanPieces();
}

/**
 * Dumb recursive implementation of permutation.
 * TODO: optimize
 * @param source the string to find permutations for
 * @return the results in a set.
 */
శూన్య U_EXPORT2 CanonicalIterator::క్రమమార్పు(UnicodeString &ఆధారం, UBool skipZeros, Hashtable *ఫలితం, UError_సంకేతం &స్థితి_2, int32_t లోతు) {
    ఒకవేళ(U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    // To avoid infinity loop caused by permute, we limit the depth of recursive
    // call to permute and return U_UNSUPPORTED_ERROR.
    // We know in some unit test we need at least 4. Set to 8 just in case some
    // unforseen use cases.
    constexpr int32_t k_క్రమమార్పు_లోతు_హద్దు = 8;
    ఒకవేళ (లోతు > k_క్రమమార్పు_లోతు_హద్దు) {
        స్థితి_2 = U_UNSUPPORTED_ERROR;
        తిరిగి;
    }
    //if (PROGRESS) printf("Permute: %s\n", UToS(Tr(source)));
    int32_t i = 0;

    // optimization:
    // if zero or one character, just return a set with it
    // we check for length < 2 to keep from counting code points all the time
    ఒకవేళ (ఆధారం.length() <= 2 && ఆధారం.లెక్క_Char32() <= 1) {
        UnicodeString *గమ్యం_Put = కొత్త UnicodeString(ఆధారం);
        /* test for nullptr */
        ఒకవేళ (గమ్యం_Put == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
        ఫలితం->put(ఆధారం, గమ్యం_Put, స్థితి_2);
        తిరిగి;
    }

    // otherwise iterate through the string, and recursively permute all the other characters
    UChar32 cp;
    Hashtable subpermute(స్థితి_2);
    ఒకవేళ(U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    subpermute.సమితి_విలువ_Deleter(uprv_deleteUObject);

    కొరకు (i = 0; i < ఆధారం.length(); i += U16_నిడివి(cp)) {
        cp = ఆధారం.char32At(i);
        స్థిరరాశి UHashElement *ne = శూన్యసూచిక;
        int32_t el = UHASH_మొదటి;
        UnicodeString sub_క్రమమార్పు_String = ఆధారం;

        // optimization:
        // if the character is canonical combining class zero,
        // don't permute it
        ఒకవేళ (skipZeros && i != 0 && u_getCombiningClass(cp) == 0) {
            //System.out.println("Skipping " + Utility.hex(UTF16.valueOf(source, i)));
            కొనసాగు;
        }

        subpermute.removeAll();

        // see what the permutations of the characters before and after this one are
        //Hashtable *subpermute = permute(source.substring(0,i) + source.substring(i + UTF16.getCharCount(cp)));
        క్రమమార్పు(sub_క్రమమార్పు_String.remove(i, U16_నిడివి(cp)), skipZeros, &subpermute, స్థితి_2, లోతు+1);
        /* Test for buffer overflows */
        ఒకవేళ(U_FAILURE(స్థితి_2)) {
            తిరిగి;
        }
        // The upper remove is destructive. The question is do we have to make a copy, or we don't care about the contents 
        // of source at this point.

        // prefix this character to all of them
        ne = subpermute.తదుపరి_Element(el);
        అయితే (ne != శూన్యసూచిక) {
            UnicodeString* permRes = static_cast<UnicodeString*>(ne->value.సూచిక);
            UnicodeString *chStr = కొత్త UnicodeString(cp);
            //test for nullptr
            ఒకవేళ (chStr == శూన్యసూచిక) {
                స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
            chStr->append(*permRes); //*((UnicodeString *)(ne->value.pointer));
            //if (PROGRESS) printf("  Piece: %s\n", UToS(*chStr));
            ఫలితం->put(*chStr, chStr, స్థితి_2);
            ne = subpermute.తదుపరి_Element(el);
        }
    }
    //return result;
}

// privates

// we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
UnicodeString* CanonicalIterator::getEquivalents(స్థిరరాశి UnicodeString &segment, int32_t &ఫలితం_len, UError_సంకేతం &స్థితి_2) {
    Hashtable ఫలితం(స్థితి_2);
    Hashtable permutations(స్థితి_2);
    Hashtable basic(స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఫలితం.సమితి_విలువ_Deleter(uprv_deleteUObject);
    permutations.సమితి_విలువ_Deleter(uprv_deleteUObject);
    basic.సమితి_విలువ_Deleter(uprv_deleteUObject);

    char16_t USeg[256];
    int32_t segLen = segment.extract(USeg, 256, స్థితి_2);
    getEquivalents2(&basic, USeg, segLen, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    // now get all the permutations
    // add only the ones that are canonically equivalent
    // TODO: optimize by not permuting any class zero.

    స్థిరరాశి UHashElement *ne = శూన్యసూచిక;
    int32_t el = UHASH_మొదటి;
    //Iterator it = basic.iterator();
    ne = basic.తదుపరి_Element(el);
    //while (it.hasNext())
    అయితే (ne != శూన్యసూచిక) {
        //String item = (String) it.next();
        UnicodeString అంశం = *static_cast<UnicodeString*>(ne->value.సూచిక);

        permutations.removeAll();
        క్రమమార్పు(అంశం, CANITER_SKIP_ZEROES, &permutations, స్థితి_2);
        స్థిరరాశి UHashElement *ne2 = శూన్యసూచిక;
        int32_t el2 = UHASH_మొదటి;
        //Iterator it2 = permutations.iterator();
        ne2 = permutations.తదుపరి_Element(el2);
        //while (it2.hasNext())
        అయితే (ne2 != శూన్యసూచిక) {
            //String possible = (String) it2.next();
            //UnicodeString *possible = new UnicodeString(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString possible(*static_cast<UnicodeString*>(ne2->value.సూచిక));
            UnicodeString attempt;
            nfd->normalize(possible, attempt, స్థితి_2);

            // TODO: check if operator == is semanticaly the same as attempt.equals(segment)
            ఒకవేళ (attempt==segment) {
                //if (PROGRESS) printf("Adding Permutation: %s\n", UToS(Tr(*possible)));
                // TODO: use the hashtable just to catch duplicates - store strings directly (somehow).
                ఫలితం.put(possible, కొత్త UnicodeString(possible), స్థితి_2); //add(possible);
            } లేకపోతే {
                //if (PROGRESS) printf("-Skipping Permutation: %s\n", UToS(Tr(*possible)));
            }

            ne2 = permutations.తదుపరి_Element(el2);
        }
        ne = basic.తదుపరి_Element(el);
    }

    /* Test for buffer overflows */
    ఒకవేళ(U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    // convert into a String[] to clean up storage
    //String[] finalResult = new String[result.size()];
    UnicodeString *final_ఫలితం = శూన్యసూచిక;
    int32_t ఫలితం_లెక్క;
    ఒకవేళ((ఫలితం_లెక్క = ఫలితం.count()) != 0) {
        final_ఫలితం = కొత్త UnicodeString[ఫలితం_లెక్క];
        ఒకవేళ (final_ఫలితం == శూన్యసూచిక) {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి శూన్యసూచిక;
        }
    }
    లేకపోతే {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    //result.toArray(finalResult);
    ఫలితం_len = 0;
    el = UHASH_మొదటి;
    ne = ఫలితం.తదుపరి_Element(el);
    అయితే(ne != శూన్యసూచిక) {
        final_ఫలితం[ఫలితం_len++] = *static_cast<UnicodeString*>(ne->value.సూచిక);
        ne = ఫలితం.తదుపరి_Element(el);
    }


    తిరిగి final_ఫలితం;
}

Hashtable* CanonicalIterator::getEquivalents2(Hashtable* fillin_ఫలితం U_LIFETIME_BOUND,
                                              స్థిరరాశి char16_t* segment,
                                              int32_t segLen,
                                              UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(segment)));

    UnicodeString గమ్యం_Put(segment, segLen);

    fillin_ఫలితం->put(గమ్యం_Put, కొత్త UnicodeString(గమ్యం_Put), స్థితి_2);

    Unicode_సమితి starts;

    // cycle through all the characters
    UChar32 cp;
    కొరకు (int32_t i = 0; i < segLen; i += U16_నిడివి(cp)) {
        // see if any character is at the start of some decomposition
        U16_GET(segment, 0, i, segLen, cp);
        ఒకవేళ (!nfcImpl->get_Canon_ప్రారంభం_సమితి(cp, starts)) {
            కొనసాగు;
        }
        // if so, see which decompositions match
        Unicode_సమితి_Iterator iter(starts);
        అయితే (iter.next()) {
            UChar32 cp2 = iter.getCodepoint();
            Hashtable remainder(స్థితి_2);
            remainder.సమితి_విలువ_Deleter(uprv_deleteUObject);
            ఒకవేళ (extract(&remainder, cp2, segment, segLen, i, స్థితి_2) == శూన్యసూచిక) {
                ఒకవేళ (U_FAILURE(స్థితి_2)) {
                    తిరిగి శూన్యసూచిక;
                }
                కొనసాగు;
            }

            // there were some matches, so add all the possibilities to the set.
            UnicodeString ఉపసర్గ(segment, i);
            ఉపసర్గ += cp2;

            int32_t el = UHASH_మొదటి;
            స్థిరరాశి UHashElement *ne = remainder.తదుపరి_Element(el);
            అయితే (ne != శూన్యసూచిక) {
                UnicodeString అంశం = *static_cast<UnicodeString*>(ne->value.సూచిక);
                UnicodeString *గమ్యం_కూడు = కొత్త UnicodeString(ఉపసర్గ);
                /* test for nullptr */
                ఒకవేళ (గమ్యం_కూడు == శూన్యసూచిక) {
                    స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    తిరిగి శూన్యసూచిక;
                }
                *గమ్యం_కూడు += అంశం;
                fillin_ఫలితం->put(*గమ్యం_కూడు, గమ్యం_కూడు, స్థితి_2);

                //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(*toAdd)));

                ne = remainder.తదుపరి_Element(el);
            }
            // ICU-22642 Guards against strings that have so many permutations
            // that they would otherwise hang the function.
            constexpr int32_t k_ఫలితం_హద్దు = 4096;
            ఒకవేళ (fillin_ఫలితం->count() > k_ఫలితం_హద్దు) {
                స్థితి_2 = U_UNSUPPORTED_ERROR;
                తిరిగి శూన్యసూచిక;
            }
        }
    }

    /* Test for buffer overflows */
    ఒకవేళ(U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి fillin_ఫలితం;
}

/**
 * See if the decomposition of cp2 is at segment starting at segmentPos 
 * (with canonical rearrangement!)
 * If so, take the remainder, and return the equivalents 
 */
Hashtable* CanonicalIterator::extract(Hashtable* fillin_ఫలితం U_LIFETIME_BOUND,
                                      UChar32 comp,
                                      స్థిరరాశి char16_t* segment,
                                      int32_t segLen,
                                      int32_t segmentPos,
                                      UError_సంకేతం& స్థితి_2) {
//Hashtable *CanonicalIterator::extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
    //if (PROGRESS) printf(" extract: %s, ", UToS(Tr(UnicodeString(comp))));
    //if (PROGRESS) printf("%s, %i\n", UToS(Tr(segment)), segmentPos);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    UnicodeString తాత్కాలిక(comp);
    int32_t ఆదానం_Len=తాత్కాలిక.length();
    UnicodeString decompString;
    nfd->normalize(తాత్కాలిక, decompString, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (decompString.isBogus()) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి char16_t *decomp=decompString.get_బఫరు();
    int32_t decompLen=decompString.length();

    // See if it matches the start of segment (at segmentPos)
    UBool ok = అబద్ధం;
    UChar32 cp;
    int32_t decompPos = 0;
    UChar32 decompCp;
    U16_తదుపరి(decomp, decompPos, decompLen, decompCp);

    int32_t i = segmentPos;
    అయితే(i < segLen) {
        U16_తదుపరి(segment, i, segLen, cp);

        ఒకవేళ (cp == decompCp) { // if equal, eat another cp from decomp

            //if (PROGRESS) printf("  matches: %s\n", UToS(Tr(UnicodeString(cp))));

            ఒకవేళ (decompPos == decompLen) { // done, have all decomp characters!
                తాత్కాలిక.append(segment+i, segLen-i);
                ok = నిజం;
                ఆపు;
            }
            U16_తదుపరి(decomp, decompPos, decompLen, decompCp);
        } లేకపోతే {
            //if (PROGRESS) printf("  buffer: %s\n", UToS(Tr(UnicodeString(cp))));

            // brute force approach
            తాత్కాలిక.append(cp);

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
    ఒకవేళ (!ok)
        తిరిగి శూన్యసూచిక; // we failed, characters left over

    //if (PROGRESS) printf("Matches\n");

    ఒకవేళ (ఆదానం_Len == తాత్కాలిక.length()) {
        fillin_ఫలితం->put(UnicodeString(), కొత్త UnicodeString(), స్థితి_2);
        తిరిగి fillin_ఫలితం; // succeed, but no remainder
    }

    // brute force approach
    // check to make sure result is canonically equivalent
    UnicodeString trial;
    nfd->normalize(తాత్కాలిక, trial, స్థితి_2);
    ఒకవేళ(U_FAILURE(స్థితి_2) || trial.compare(segment+segmentPos, segLen - segmentPos) != 0) {
        తిరిగి శూన్యసూచిక;
    }

    తిరిగి getEquivalents2(fillin_ఫలితం, తాత్కాలిక.get_బఫరు()+ఆదానం_Len, తాత్కాలిక.length()-ఆదానం_Len, స్థితి_2);
}

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_NORMALIZATION */
