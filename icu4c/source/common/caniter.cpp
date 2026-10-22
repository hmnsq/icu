// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *****************************************************************************
 * Copyright (C) 1996-2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 *****************************************************************************
 */

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_NORMALIZATION

#ضمن "unicode/caniter.h"
#ضمن "unicode/normalizer2.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/usetiter.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "hash.h"
#ضمن "normalizer2impl.h"

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

//عامه

U_NAMESPACE_BEGIN

// TODO: add boilerplate methods.

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(CanonicalIterator)


/**
 *@param source string to get results for
 */
CanonicalIterator::CanonicalIterator(ثابت UnicodeString &سرچینه_Str, UError_کوډ &حالت) :
    pieces(تش_اشاره),
    pieces_اوږدتیا(0),
    pieces_lengths(تش_اشاره),
    current(تش_اشاره),
    current_اوږدتیا(0),
    nfd(Normalizer2::getNFDInstance(حالت)),
    nfcImpl(Normalizer2Factory::getNFCImpl(حالت))
{
    إذا(U_SUCCESS(حالت) && nfcImpl->ensure_Canon_Iter_معلومات(حالت)) {
      سېټ_سرچینه(سرچینه_Str, حالت);
    }
}

CanonicalIterator::~CanonicalIterator() {
  cleanPieces();
}

فراغ CanonicalIterator::cleanPieces() {
    int32_t i = 0;
    إذا(pieces != تش_اشاره) {
        لكل(i = 0; i < pieces_اوږدتیا; i++) {
            إذا(pieces[i] != تش_اشاره) {
                ړنګول[] pieces[i];
            }
        }
        uprv_free(pieces);
        pieces = تش_اشاره;
        pieces_اوږدتیا = 0;
    }
    إذا(pieces_lengths != تش_اشاره) {
        uprv_free(pieces_lengths);
        pieces_lengths = تش_اشاره;
    }
    إذا(current != تش_اشاره) {
        uprv_free(current);
        current = تش_اشاره;
        current_اوږدتیا = 0;
    }
}

/**
 *@return gets the source: NOTE: it is the NFD form of source
 */
UnicodeString CanonicalIterator::get_سرچینه() {
  أرجع سرچینه;
}

/**
 * Resets the iterator so that one can start again from the beginning.
 */
فراغ CanonicalIterator::reset() {
    بشپړ = خطأ;
    لكل (صحيح i = 0; i < current_اوږدتیا; ++i) {
        current[i] = 0;
    }
}

/**
 *@return the next string that is canonically equivalent. The value null is returned when
 * the iteration is done.
 */
UnicodeString CanonicalIterator::next() {
    int32_t i = 0;

    إذا (بشپړ) {
      بفر.سېټ_منزل_Bogus();
      أرجع بفر;
    }

    // delete old contents
    بفر.remove();

    // construct return value

    لكل (i = 0; i < pieces_اوږدتیا; ++i) {
        بفر.append(pieces[i][current[i]]);
    }
    //String result = buffer.toString(); // not needed

    // find next value for next time

    لكل (i = current_اوږدتیا - 1; ; --i) {
        إذا (i < 0) {
            بشپړ = ریښتیا;
            اكسر;
        }
        current[i]++;
        إذا (current[i] < pieces_lengths[i]) اكسر; // got sequence
        current[i] = 0;
    }
    أرجع بفر;
}

/**
 *@param set the source string to iterate against. This allows the same iterator to be used
 * while changing the source string, saving object creation.
 */
فراغ CanonicalIterator::سېټ_سرچینه(ثابت UnicodeString &new_سرچینه, UError_کوډ &حالت) {
    int32_t list_اوږدتیا_2 = 0;
    UChar32 cp = 0;
    int32_t پیل = 0;
    int32_t i = 0;
    UnicodeString *list = تش_اشاره;

    nfd->normalize(new_سرچینه, سرچینه, حالت);
    إذا(U_FAILURE(حالت)) {
      أرجع;
    }
    بشپړ = خطأ;

    cleanPieces();

    // catch degenerate case
    إذا (new_سرچینه.length() == 0) {
        pieces = static_cast<UnicodeString**>(uprv_malloc(حجم(UnicodeString*)));
        pieces_lengths = static_cast<int32_t*>(uprv_malloc(1 * حجم(int32_t)));
        pieces_اوږدتیا = 1;
        current = static_cast<int32_t*>(uprv_malloc(1 * حجم(int32_t)));
        current_اوږدتیا = 1;
        إذا (pieces == تش_اشاره || pieces_lengths == تش_اشاره || current == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
            اذهب CleanPartialInitialization;
        }
        current[0] = 0;
        pieces[0] = جديد UnicodeString[1];
        pieces_lengths[0] = 1;
        إذا (pieces[0] == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
            اذهب CleanPartialInitialization;
        }
        أرجع;
    }


    list = جديد UnicodeString[سرچینه.length()];
    إذا (list == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        اذهب CleanPartialInitialization;
    }

    // i should initially be the number of code units at the 
    // start of the string
    i = U16_اوږدتیا(سرچینه.char32At(0));
    // int32_t i = 1;
    // find the segments
    // This code iterates through the source string and 
    // extracts segments that end up on a codepoint that
    // doesn't start any decompositions. (Analysis is done
    // on the NFD form - see above).
    لكل (; i < سرچینه.length(); i += U16_اوږدتیا(cp)) {
        cp = سرچینه.char32At(i);
        إذا (nfcImpl->isCanonSegmentStarter(cp)) {
            سرچینه.extract(پیل, i-پیل, list[list_اوږدتیا_2++]); // add up to i
            پیل = i;
        }
    }
    سرچینه.extract(پیل, i-پیل, list[list_اوږدتیا_2++]); // add last one


    // allocate the arrays, and find the strings that are CE to each segment
    pieces = static_cast<UnicodeString**>(uprv_malloc(list_اوږدتیا_2 * حجم(UnicodeString*)));
    pieces_اوږدتیا = list_اوږدتیا_2;
    pieces_lengths = static_cast<int32_t*>(uprv_malloc(list_اوږدتیا_2 * حجم(int32_t)));
    current = static_cast<int32_t*>(uprv_malloc(list_اوږدتیا_2 * حجم(int32_t)));
    current_اوږدتیا = list_اوږدتیا_2;
    إذا (pieces == تش_اشاره || pieces_lengths == تش_اشاره || current == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        اذهب CleanPartialInitialization;
    }

    لكل (i = 0; i < current_اوږدتیا; i++) {
        current[i] = 0;
    }
    // for each segment, get all the combinations that can produce 
    // it after NFD normalization
    لكل (i = 0; i < pieces_اوږدتیا; ++i) {
        //if (PROGRESS) printf("SEGMENT\n");
        pieces[i] = getEquivalents(list[i], pieces_lengths[i], حالت);
    }

    ړنګول[] list;
    أرجع;
// Common section to cleanup all local variables and reset object variables.
CleanPartialInitialization:
    ړنګول[] list;
    cleanPieces();
}

/**
 * Dumb recursive implementation of permutation.
 * TODO: optimize
 * @param source the string to find permutations for
 * @return the results in a set.
 */
فراغ U_EXPORT2 CanonicalIterator::ترتیب_بدلول(UnicodeString &سرچینه, UBool skipZeros, Hashtable *پایله, UError_کوډ &حالت, int32_t ژوروالی) {
    إذا(U_FAILURE(حالت)) {
        أرجع;
    }
    // To avoid infinity loop caused by permute, we limit the depth of recursive
    // call to permute and return U_UNSUPPORTED_ERROR.
    // We know in some unit test we need at least 4. Set to 8 just in case some
    // unforseen use cases.
    constexpr int32_t k_ترتیب_بدلول_ژوروالی_حد = 8;
    إذا (ژوروالی > k_ترتیب_بدلول_ژوروالی_حد) {
        حالت = U_UNSUPPORTED_ERROR;
        أرجع;
    }
    //if (PROGRESS) printf("Permute: %s\n", UToS(Tr(source)));
    int32_t i = 0;

    // optimization:
    // if zero or one character, just return a set with it
    // we check for length < 2 to keep from counting code points all the time
    إذا (سرچینه.length() <= 2 && سرچینه.شمېر_Char32() <= 1) {
        UnicodeString *منزل_Put = جديد UnicodeString(سرچینه);
        /* test for nullptr */
        إذا (منزل_Put == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
            أرجع;
        }
        پایله->put(سرچینه, منزل_Put, حالت);
        أرجع;
    }

    // otherwise iterate through the string, and recursively permute all the other characters
    UChar32 cp;
    Hashtable subpermute(حالت);
    إذا(U_FAILURE(حالت)) {
        أرجع;
    }
    subpermute.سېټ_ارزښت_Deleter(uprv_deleteUObject);

    لكل (i = 0; i < سرچینه.length(); i += U16_اوږدتیا(cp)) {
        cp = سرچینه.char32At(i);
        ثابت UHashElement *ne = تش_اشاره;
        int32_t el = UHASH_لومړی;
        UnicodeString sub_ترتیب_بدلول_String = سرچینه;

        // optimization:
        // if the character is canonical combining class zero,
        // don't permute it
        إذا (skipZeros && i != 0 && u_getCombiningClass(cp) == 0) {
            //System.out.println("Skipping " + Utility.hex(UTF16.valueOf(source, i)));
            استمر;
        }

        subpermute.removeAll();

        // see what the permutations of the characters before and after this one are
        //Hashtable *subpermute = permute(source.substring(0,i) + source.substring(i + UTF16.getCharCount(cp)));
        ترتیب_بدلول(sub_ترتیب_بدلول_String.remove(i, U16_اوږدتیا(cp)), skipZeros, &subpermute, حالت, ژوروالی+1);
        /* Test for buffer overflows */
        إذا(U_FAILURE(حالت)) {
            أرجع;
        }
        // The upper remove is destructive. The question is do we have to make a copy, or we don't care about the contents 
        // of source at this point.

        // prefix this character to all of them
        ne = subpermute.بل_Element(el);
        طالما (ne != تش_اشاره) {
            UnicodeString* permRes = static_cast<UnicodeString*>(ne->value.اشاره_ګر);
            UnicodeString *chStr = جديد UnicodeString(cp);
            //test for nullptr
            إذا (chStr == تش_اشاره) {
                حالت = U_حافظه_ویش_ERROR;
                أرجع;
            }
            chStr->append(*permRes); //*((UnicodeString *)(ne->value.pointer));
            //if (PROGRESS) printf("  Piece: %s\n", UToS(*chStr));
            پایله->put(*chStr, chStr, حالت);
            ne = subpermute.بل_Element(el);
        }
    }
    //return result;
}

// privates

// we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
UnicodeString* CanonicalIterator::getEquivalents(ثابت UnicodeString &segment, int32_t &پایله_len, UError_کوډ &حالت) {
    Hashtable پایله(حالت);
    Hashtable permutations(حالت);
    Hashtable basic(حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    پایله.سېټ_ارزښت_Deleter(uprv_deleteUObject);
    permutations.سېټ_ارزښت_Deleter(uprv_deleteUObject);
    basic.سېټ_ارزښت_Deleter(uprv_deleteUObject);

    char16_t USeg[256];
    int32_t segLen = segment.extract(USeg, 256, حالت);
    getEquivalents2(&basic, USeg, segLen, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    // now get all the permutations
    // add only the ones that are canonically equivalent
    // TODO: optimize by not permuting any class zero.

    ثابت UHashElement *ne = تش_اشاره;
    int32_t el = UHASH_لومړی;
    //Iterator it = basic.iterator();
    ne = basic.بل_Element(el);
    //while (it.hasNext())
    طالما (ne != تش_اشاره) {
        //String item = (String) it.next();
        UnicodeString توکی = *static_cast<UnicodeString*>(ne->value.اشاره_ګر);

        permutations.removeAll();
        ترتیب_بدلول(توکی, CANITER_SKIP_ZEROES, &permutations, حالت);
        ثابت UHashElement *ne2 = تش_اشاره;
        int32_t el2 = UHASH_لومړی;
        //Iterator it2 = permutations.iterator();
        ne2 = permutations.بل_Element(el2);
        //while (it2.hasNext())
        طالما (ne2 != تش_اشاره) {
            //String possible = (String) it2.next();
            //UnicodeString *possible = new UnicodeString(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString possible(*static_cast<UnicodeString*>(ne2->value.اشاره_ګر));
            UnicodeString attempt;
            nfd->normalize(possible, attempt, حالت);

            // TODO: check if operator == is semanticaly the same as attempt.equals(segment)
            إذا (attempt==segment) {
                //if (PROGRESS) printf("Adding Permutation: %s\n", UToS(Tr(*possible)));
                // TODO: use the hashtable just to catch duplicates - store strings directly (somehow).
                پایله.put(possible, جديد UnicodeString(possible), حالت); //add(possible);
            } وإلا {
                //if (PROGRESS) printf("-Skipping Permutation: %s\n", UToS(Tr(*possible)));
            }

            ne2 = permutations.بل_Element(el2);
        }
        ne = basic.بل_Element(el);
    }

    /* Test for buffer overflows */
    إذا(U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    // convert into a String[] to clean up storage
    //String[] finalResult = new String[result.size()];
    UnicodeString *final_پایله = تش_اشاره;
    int32_t پایله_شمېر;
    إذا((پایله_شمېر = پایله.count()) != 0) {
        final_پایله = جديد UnicodeString[پایله_شمېر];
        إذا (final_پایله == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
            أرجع تش_اشاره;
        }
    }
    وإلا {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    //result.toArray(finalResult);
    پایله_len = 0;
    el = UHASH_لومړی;
    ne = پایله.بل_Element(el);
    طالما(ne != تش_اشاره) {
        final_پایله[پایله_len++] = *static_cast<UnicodeString*>(ne->value.اشاره_ګر);
        ne = پایله.بل_Element(el);
    }


    أرجع final_پایله;
}

Hashtable* CanonicalIterator::getEquivalents2(Hashtable* fillin_پایله U_LIFETIME_BOUND,
                                              ثابت char16_t* segment,
                                              int32_t segLen,
                                              UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(segment)));

    UnicodeString منزل_Put(segment, segLen);

    fillin_پایله->put(منزل_Put, جديد UnicodeString(منزل_Put), حالت);

    Unicode_سېټ starts;

    // cycle through all the characters
    UChar32 cp;
    لكل (int32_t i = 0; i < segLen; i += U16_اوږدتیا(cp)) {
        // see if any character is at the start of some decomposition
        U16_GET(segment, 0, i, segLen, cp);
        إذا (!nfcImpl->get_Canon_پیل_سېټ(cp, starts)) {
            استمر;
        }
        // if so, see which decompositions match
        Unicode_سېټ_Iterator iter(starts);
        طالما (iter.next()) {
            UChar32 cp2 = iter.getCodepoint();
            Hashtable remainder(حالت);
            remainder.سېټ_ارزښت_Deleter(uprv_deleteUObject);
            إذا (extract(&remainder, cp2, segment, segLen, i, حالت) == تش_اشاره) {
                إذا (U_FAILURE(حالت)) {
                    أرجع تش_اشاره;
                }
                استمر;
            }

            // there were some matches, so add all the possibilities to the set.
            UnicodeString مختاړی(segment, i);
            مختاړی += cp2;

            int32_t el = UHASH_لومړی;
            ثابت UHashElement *ne = remainder.بل_Element(el);
            طالما (ne != تش_اشاره) {
                UnicodeString توکی = *static_cast<UnicodeString*>(ne->value.اشاره_ګر);
                UnicodeString *منزل_جمع_کړه = جديد UnicodeString(مختاړی);
                /* test for nullptr */
                إذا (منزل_جمع_کړه == تش_اشاره) {
                    حالت = U_حافظه_ویش_ERROR;
                    أرجع تش_اشاره;
                }
                *منزل_جمع_کړه += توکی;
                fillin_پایله->put(*منزل_جمع_کړه, منزل_جمع_کړه, حالت);

                //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(*toAdd)));

                ne = remainder.بل_Element(el);
            }
            // ICU-22642 Guards against strings that have so many permutations
            // that they would otherwise hang the function.
            constexpr int32_t k_پایله_حد = 4096;
            إذا (fillin_پایله->count() > k_پایله_حد) {
                حالت = U_UNSUPPORTED_ERROR;
                أرجع تش_اشاره;
            }
        }
    }

    /* Test for buffer overflows */
    إذا(U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    أرجع fillin_پایله;
}

/**
 * See if the decomposition of cp2 is at segment starting at segmentPos 
 * (with canonical rearrangement!)
 * If so, take the remainder, and return the equivalents 
 */
Hashtable* CanonicalIterator::extract(Hashtable* fillin_پایله U_LIFETIME_BOUND,
                                      UChar32 comp,
                                      ثابت char16_t* segment,
                                      int32_t segLen,
                                      int32_t segmentPos,
                                      UError_کوډ& حالت) {
//Hashtable *CanonicalIterator::extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
    //if (PROGRESS) printf(" extract: %s, ", UToS(Tr(UnicodeString(comp))));
    //if (PROGRESS) printf("%s, %i\n", UToS(Tr(segment)), segmentPos);

    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    UnicodeString لنډمهاله(comp);
    int32_t ننوت_Len=لنډمهاله.length();
    UnicodeString decompString;
    nfd->normalize(لنډمهاله, decompString, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    إذا (decompString.isBogus()) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    ثابت char16_t *decomp=decompString.get_بفر();
    int32_t decompLen=decompString.length();

    // See if it matches the start of segment (at segmentPos)
    UBool ok = خطأ;
    UChar32 cp;
    int32_t decompPos = 0;
    UChar32 decompCp;
    U16_بل(decomp, decompPos, decompLen, decompCp);

    int32_t i = segmentPos;
    طالما(i < segLen) {
        U16_بل(segment, i, segLen, cp);

        إذا (cp == decompCp) { // if equal, eat another cp from decomp

            //if (PROGRESS) printf("  matches: %s\n", UToS(Tr(UnicodeString(cp))));

            إذا (decompPos == decompLen) { // done, have all decomp characters!
                لنډمهاله.append(segment+i, segLen-i);
                ok = ریښتیا;
                اكسر;
            }
            U16_بل(decomp, decompPos, decompLen, decompCp);
        } وإلا {
            //if (PROGRESS) printf("  buffer: %s\n", UToS(Tr(UnicodeString(cp))));

            // brute force approach
            لنډمهاله.append(cp);

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
    إذا (!ok)
        أرجع تش_اشاره; // we failed, characters left over

    //if (PROGRESS) printf("Matches\n");

    إذا (ننوت_Len == لنډمهاله.length()) {
        fillin_پایله->put(UnicodeString(), جديد UnicodeString(), حالت);
        أرجع fillin_پایله; // succeed, but no remainder
    }

    // brute force approach
    // check to make sure result is canonically equivalent
    UnicodeString trial;
    nfd->normalize(لنډمهاله, trial, حالت);
    إذا(U_FAILURE(حالت) || trial.compare(segment+segmentPos, segLen - segmentPos) != 0) {
        أرجع تش_اشاره;
    }

    أرجع getEquivalents2(fillin_پایله, لنډمهاله.get_بفر()+ننوت_Len, لنډمهاله.length()-ننوت_Len, حالت);
}

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_NORMALIZATION */
