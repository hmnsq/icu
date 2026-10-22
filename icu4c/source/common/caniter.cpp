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

//عوامی

U_NAMESPACE_BEGIN

// TODO: add boilerplate methods.

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(CanonicalIterator)


/**
 *@param source string to get results for
 */
CanonicalIterator::CanonicalIterator(ثابت UnicodeString &سروت_Str, UError_کوڈ &حالت) :
    pieces(خالی_سوچک),
    pieces_لمائی(0),
    pieces_lengths(خالی_سوچک),
    current(خالی_سوچک),
    current_لمائی(0),
    nfd(Normalizer2::getNFDInstance(حالت)),
    nfcImpl(Normalizer2Factory::getNFCImpl(حالت))
{
    إذا(U_SUCCESS(حالت) && nfcImpl->ensure_Canon_Iter_انکڑے(حالت)) {
      سیٹ_سروت(سروت_Str, حالت);
    }
}

CanonicalIterator::~CanonicalIterator() {
  cleanPieces();
}

فراغ CanonicalIterator::cleanPieces() {
    int32_t i = 0;
    إذا(pieces != خالی_سوچک) {
        لكل(i = 0; i < pieces_لمائی; i++) {
            إذا(pieces[i] != خالی_سوچک) {
                مٹاؤ[] pieces[i];
            }
        }
        uprv_free(pieces);
        pieces = خالی_سوچک;
        pieces_لمائی = 0;
    }
    إذا(pieces_lengths != خالی_سوچک) {
        uprv_free(pieces_lengths);
        pieces_lengths = خالی_سوچک;
    }
    إذا(current != خالی_سوچک) {
        uprv_free(current);
        current = خالی_سوچک;
        current_لمائی = 0;
    }
}

/**
 *@return gets the source: NOTE: it is the NFD form of source
 */
UnicodeString CanonicalIterator::get_سروت() {
  أرجع سروت;
}

/**
 * Resets the iterator so that one can start again from the beginning.
 */
فراغ CanonicalIterator::reset() {
    پورا = خطأ;
    لكل (صحيح i = 0; i < current_لمائی; ++i) {
        current[i] = 0;
    }
}

/**
 *@return the next string that is canonically equivalent. The value null is returned when
 * the iteration is done.
 */
UnicodeString CanonicalIterator::next() {
    int32_t i = 0;

    إذا (پورا) {
      بفر.سیٹ_منزل_Bogus();
      أرجع بفر;
    }

    // delete old contents
    بفر.remove();

    // construct return value

    لكل (i = 0; i < pieces_لمائی; ++i) {
        بفر.append(pieces[i][current[i]]);
    }
    //String result = buffer.toString(); // not needed

    // find next value for next time

    لكل (i = current_لمائی - 1; ; --i) {
        إذا (i < 0) {
            پورا = سچ;
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
فراغ CanonicalIterator::سیٹ_سروت(ثابت UnicodeString &new_سروت, UError_کوڈ &حالت) {
    int32_t list_لمائی_2 = 0;
    UChar32 cp = 0;
    int32_t شروع = 0;
    int32_t i = 0;
    UnicodeString *list = خالی_سوچک;

    nfd->normalize(new_سروت, سروت, حالت);
    إذا(U_FAILURE(حالت)) {
      أرجع;
    }
    پورا = خطأ;

    cleanPieces();

    // catch degenerate case
    إذا (new_سروت.length() == 0) {
        pieces = static_cast<UnicodeString**>(uprv_malloc(حجم(UnicodeString*)));
        pieces_lengths = static_cast<int32_t*>(uprv_malloc(1 * حجم(int32_t)));
        pieces_لمائی = 1;
        current = static_cast<int32_t*>(uprv_malloc(1 * حجم(int32_t)));
        current_لمائی = 1;
        إذا (pieces == خالی_سوچک || pieces_lengths == خالی_سوچک || current == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
            اذهب CleanPartialInitialization;
        }
        current[0] = 0;
        pieces[0] = جديد UnicodeString[1];
        pieces_lengths[0] = 1;
        إذا (pieces[0] == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
            اذهب CleanPartialInitialization;
        }
        أرجع;
    }


    list = جديد UnicodeString[سروت.length()];
    إذا (list == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        اذهب CleanPartialInitialization;
    }

    // i should initially be the number of code units at the 
    // start of the string
    i = U16_لمائی(سروت.char32At(0));
    // int32_t i = 1;
    // find the segments
    // This code iterates through the source string and 
    // extracts segments that end up on a codepoint that
    // doesn't start any decompositions. (Analysis is done
    // on the NFD form - see above).
    لكل (; i < سروت.length(); i += U16_لمائی(cp)) {
        cp = سروت.char32At(i);
        إذا (nfcImpl->isCanonSegmentStarter(cp)) {
            سروت.extract(شروع, i-شروع, list[list_لمائی_2++]); // add up to i
            شروع = i;
        }
    }
    سروت.extract(شروع, i-شروع, list[list_لمائی_2++]); // add last one


    // allocate the arrays, and find the strings that are CE to each segment
    pieces = static_cast<UnicodeString**>(uprv_malloc(list_لمائی_2 * حجم(UnicodeString*)));
    pieces_لمائی = list_لمائی_2;
    pieces_lengths = static_cast<int32_t*>(uprv_malloc(list_لمائی_2 * حجم(int32_t)));
    current = static_cast<int32_t*>(uprv_malloc(list_لمائی_2 * حجم(int32_t)));
    current_لمائی = list_لمائی_2;
    إذا (pieces == خالی_سوچک || pieces_lengths == خالی_سوچک || current == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        اذهب CleanPartialInitialization;
    }

    لكل (i = 0; i < current_لمائی; i++) {
        current[i] = 0;
    }
    // for each segment, get all the combinations that can produce 
    // it after NFD normalization
    لكل (i = 0; i < pieces_لمائی; ++i) {
        //if (PROGRESS) printf("SEGMENT\n");
        pieces[i] = getEquivalents(list[i], pieces_lengths[i], حالت);
    }

    مٹاؤ[] list;
    أرجع;
// Common section to cleanup all local variables and reset object variables.
CleanPartialInitialization:
    مٹاؤ[] list;
    cleanPieces();
}

/**
 * Dumb recursive implementation of permutation.
 * TODO: optimize
 * @param source the string to find permutations for
 * @return the results in a set.
 */
فراغ U_EXPORT2 CanonicalIterator::ترتیب_بدلو(UnicodeString &سروت, UBool skipZeros, Hashtable *نتیجہ, UError_کوڈ &حالت, int32_t ڈونگھائی) {
    إذا(U_FAILURE(حالت)) {
        أرجع;
    }
    // To avoid infinity loop caused by permute, we limit the depth of recursive
    // call to permute and return U_UNSUPPORTED_ERROR.
    // We know in some unit test we need at least 4. Set to 8 just in case some
    // unforseen use cases.
    constexpr int32_t k_ترتیب_بدلو_ڈونگھائی_حد_بندی = 8;
    إذا (ڈونگھائی > k_ترتیب_بدلو_ڈونگھائی_حد_بندی) {
        حالت = U_UNSUPPORTED_ERROR;
        أرجع;
    }
    //if (PROGRESS) printf("Permute: %s\n", UToS(Tr(source)));
    int32_t i = 0;

    // optimization:
    // if zero or one character, just return a set with it
    // we check for length < 2 to keep from counting code points all the time
    إذا (سروت.length() <= 2 && سروت.گنتی_Char32() <= 1) {
        UnicodeString *منزل_Put = جديد UnicodeString(سروت);
        /* test for nullptr */
        إذا (منزل_Put == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
        نتیجہ->put(سروت, منزل_Put, حالت);
        أرجع;
    }

    // otherwise iterate through the string, and recursively permute all the other characters
    UChar32 cp;
    Hashtable subpermute(حالت);
    إذا(U_FAILURE(حالت)) {
        أرجع;
    }
    subpermute.سیٹ_قدر_Deleter(uprv_deleteUObject);

    لكل (i = 0; i < سروت.length(); i += U16_لمائی(cp)) {
        cp = سروت.char32At(i);
        ثابت UHashElement *ne = خالی_سوچک;
        int32_t el = UHASH_پہلا;
        UnicodeString sub_ترتیب_بدلو_String = سروت;

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
        ترتیب_بدلو(sub_ترتیب_بدلو_String.remove(i, U16_لمائی(cp)), skipZeros, &subpermute, حالت, ڈونگھائی+1);
        /* Test for buffer overflows */
        إذا(U_FAILURE(حالت)) {
            أرجع;
        }
        // The upper remove is destructive. The question is do we have to make a copy, or we don't care about the contents 
        // of source at this point.

        // prefix this character to all of them
        ne = subpermute.اگلا_Element(el);
        طالما (ne != خالی_سوچک) {
            UnicodeString* permRes = static_cast<UnicodeString*>(ne->value.اشارہ_گر);
            UnicodeString *chStr = جديد UnicodeString(cp);
            //test for nullptr
            إذا (chStr == خالی_سوچک) {
                حالت = U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
            chStr->append(*permRes); //*((UnicodeString *)(ne->value.pointer));
            //if (PROGRESS) printf("  Piece: %s\n", UToS(*chStr));
            نتیجہ->put(*chStr, chStr, حالت);
            ne = subpermute.اگلا_Element(el);
        }
    }
    //return result;
}

// privates

// we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
UnicodeString* CanonicalIterator::getEquivalents(ثابت UnicodeString &segment, int32_t &نتیجہ_len, UError_کوڈ &حالت) {
    Hashtable نتیجہ(حالت);
    Hashtable permutations(حالت);
    Hashtable basic(حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    نتیجہ.سیٹ_قدر_Deleter(uprv_deleteUObject);
    permutations.سیٹ_قدر_Deleter(uprv_deleteUObject);
    basic.سیٹ_قدر_Deleter(uprv_deleteUObject);

    char16_t USeg[256];
    int32_t segLen = segment.extract(USeg, 256, حالت);
    getEquivalents2(&basic, USeg, segLen, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    // now get all the permutations
    // add only the ones that are canonically equivalent
    // TODO: optimize by not permuting any class zero.

    ثابت UHashElement *ne = خالی_سوچک;
    int32_t el = UHASH_پہلا;
    //Iterator it = basic.iterator();
    ne = basic.اگلا_Element(el);
    //while (it.hasNext())
    طالما (ne != خالی_سوچک) {
        //String item = (String) it.next();
        UnicodeString شے = *static_cast<UnicodeString*>(ne->value.اشارہ_گر);

        permutations.removeAll();
        ترتیب_بدلو(شے, CANITER_SKIP_ZEROES, &permutations, حالت);
        ثابت UHashElement *ne2 = خالی_سوچک;
        int32_t el2 = UHASH_پہلا;
        //Iterator it2 = permutations.iterator();
        ne2 = permutations.اگلا_Element(el2);
        //while (it2.hasNext())
        طالما (ne2 != خالی_سوچک) {
            //String possible = (String) it2.next();
            //UnicodeString *possible = new UnicodeString(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString possible(*static_cast<UnicodeString*>(ne2->value.اشارہ_گر));
            UnicodeString attempt;
            nfd->normalize(possible, attempt, حالت);

            // TODO: check if operator == is semanticaly the same as attempt.equals(segment)
            إذا (attempt==segment) {
                //if (PROGRESS) printf("Adding Permutation: %s\n", UToS(Tr(*possible)));
                // TODO: use the hashtable just to catch duplicates - store strings directly (somehow).
                نتیجہ.put(possible, جديد UnicodeString(possible), حالت); //add(possible);
            } وإلا {
                //if (PROGRESS) printf("-Skipping Permutation: %s\n", UToS(Tr(*possible)));
            }

            ne2 = permutations.اگلا_Element(el2);
        }
        ne = basic.اگلا_Element(el);
    }

    /* Test for buffer overflows */
    إذا(U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    // convert into a String[] to clean up storage
    //String[] finalResult = new String[result.size()];
    UnicodeString *final_نتیجہ = خالی_سوچک;
    int32_t نتیجہ_گنتی;
    إذا((نتیجہ_گنتی = نتیجہ.count()) != 0) {
        final_نتیجہ = جديد UnicodeString[نتیجہ_گنتی];
        إذا (final_نتیجہ == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
            أرجع خالی_سوچک;
        }
    }
    وإلا {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    //result.toArray(finalResult);
    نتیجہ_len = 0;
    el = UHASH_پہلا;
    ne = نتیجہ.اگلا_Element(el);
    طالما(ne != خالی_سوچک) {
        final_نتیجہ[نتیجہ_len++] = *static_cast<UnicodeString*>(ne->value.اشارہ_گر);
        ne = نتیجہ.اگلا_Element(el);
    }


    أرجع final_نتیجہ;
}

Hashtable* CanonicalIterator::getEquivalents2(Hashtable* fillin_نتیجہ U_LIFETIME_BOUND,
                                              ثابت char16_t* segment,
                                              int32_t segLen,
                                              UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(segment)));

    UnicodeString منزل_Put(segment, segLen);

    fillin_نتیجہ->put(منزل_Put, جديد UnicodeString(منزل_Put), حالت);

    Unicode_سیٹ starts;

    // cycle through all the characters
    UChar32 cp;
    لكل (int32_t i = 0; i < segLen; i += U16_لمائی(cp)) {
        // see if any character is at the start of some decomposition
        U16_GET(segment, 0, i, segLen, cp);
        إذا (!nfcImpl->get_Canon_شروع_سیٹ(cp, starts)) {
            استمر;
        }
        // if so, see which decompositions match
        Unicode_سیٹ_Iterator iter(starts);
        طالما (iter.next()) {
            UChar32 cp2 = iter.getCodepoint();
            Hashtable remainder(حالت);
            remainder.سیٹ_قدر_Deleter(uprv_deleteUObject);
            إذا (extract(&remainder, cp2, segment, segLen, i, حالت) == خالی_سوچک) {
                إذا (U_FAILURE(حالت)) {
                    أرجع خالی_سوچک;
                }
                استمر;
            }

            // there were some matches, so add all the possibilities to the set.
            UnicodeString اگیتر(segment, i);
            اگیتر += cp2;

            int32_t el = UHASH_پہلا;
            ثابت UHashElement *ne = remainder.اگلا_Element(el);
            طالما (ne != خالی_سوچک) {
                UnicodeString شے = *static_cast<UnicodeString*>(ne->value.اشارہ_گر);
                UnicodeString *منزل_جوڑو = جديد UnicodeString(اگیتر);
                /* test for nullptr */
                إذا (منزل_جوڑو == خالی_سوچک) {
                    حالت = U_حافظہ_ونڈ_ERROR;
                    أرجع خالی_سوچک;
                }
                *منزل_جوڑو += شے;
                fillin_نتیجہ->put(*منزل_جوڑو, منزل_جوڑو, حالت);

                //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(*toAdd)));

                ne = remainder.اگلا_Element(el);
            }
            // ICU-22642 Guards against strings that have so many permutations
            // that they would otherwise hang the function.
            constexpr int32_t k_نتیجہ_حد_بندی = 4096;
            إذا (fillin_نتیجہ->count() > k_نتیجہ_حد_بندی) {
                حالت = U_UNSUPPORTED_ERROR;
                أرجع خالی_سوچک;
            }
        }
    }

    /* Test for buffer overflows */
    إذا(U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    أرجع fillin_نتیجہ;
}

/**
 * See if the decomposition of cp2 is at segment starting at segmentPos 
 * (with canonical rearrangement!)
 * If so, take the remainder, and return the equivalents 
 */
Hashtable* CanonicalIterator::extract(Hashtable* fillin_نتیجہ U_LIFETIME_BOUND,
                                      UChar32 comp,
                                      ثابت char16_t* segment,
                                      int32_t segLen,
                                      int32_t segmentPos,
                                      UError_کوڈ& حالت) {
//Hashtable *CanonicalIterator::extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
    //if (PROGRESS) printf(" extract: %s, ", UToS(Tr(UnicodeString(comp))));
    //if (PROGRESS) printf("%s, %i\n", UToS(Tr(segment)), segmentPos);

    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    UnicodeString عارضی(comp);
    int32_t داخلہ_Len=عارضی.length();
    UnicodeString decompString;
    nfd->normalize(عارضی, decompString, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    إذا (decompString.isBogus()) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    ثابت char16_t *decomp=decompString.get_بفر();
    int32_t decompLen=decompString.length();

    // See if it matches the start of segment (at segmentPos)
    UBool ok = خطأ;
    UChar32 cp;
    int32_t decompPos = 0;
    UChar32 decompCp;
    U16_اگلا(decomp, decompPos, decompLen, decompCp);

    int32_t i = segmentPos;
    طالما(i < segLen) {
        U16_اگلا(segment, i, segLen, cp);

        إذا (cp == decompCp) { // if equal, eat another cp from decomp

            //if (PROGRESS) printf("  matches: %s\n", UToS(Tr(UnicodeString(cp))));

            إذا (decompPos == decompLen) { // done, have all decomp characters!
                عارضی.append(segment+i, segLen-i);
                ok = سچ;
                اكسر;
            }
            U16_اگلا(decomp, decompPos, decompLen, decompCp);
        } وإلا {
            //if (PROGRESS) printf("  buffer: %s\n", UToS(Tr(UnicodeString(cp))));

            // brute force approach
            عارضی.append(cp);

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
        أرجع خالی_سوچک; // we failed, characters left over

    //if (PROGRESS) printf("Matches\n");

    إذا (داخلہ_Len == عارضی.length()) {
        fillin_نتیجہ->put(UnicodeString(), جديد UnicodeString(), حالت);
        أرجع fillin_نتیجہ; // succeed, but no remainder
    }

    // brute force approach
    // check to make sure result is canonically equivalent
    UnicodeString trial;
    nfd->normalize(عارضی, trial, حالت);
    إذا(U_FAILURE(حالت) || trial.compare(segment+segmentPos, segLen - segmentPos) != 0) {
        أرجع خالی_سوچک;
    }

    أرجع getEquivalents2(fillin_نتیجہ, عارضی.get_بفر()+داخلہ_Len, عارضی.length()-داخلہ_Len, حالت);
}

U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_NORMALIZATION */
