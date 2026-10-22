// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *****************************************************************************
 * Copyright (C) 1996-2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 *****************************************************************************
 */

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_NORMALIZATION

#شامل "unicode/caniter.h"
#شامل "unicode/normalizer2.h"
#شامل "unicode/uchar.h"
#شامل "unicode/uniset.h"
#شامل "unicode/usetiter.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "hash.h"
#شامل "normalizer2impl.h"

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
CanonicalIterator::CanonicalIterator(ثابت UnicodeString &ماخذ_Str, UError_رمز &حالت_2) :
    pieces(خالی_اشاریہ),
    pieces_لمبائی(0),
    pieces_lengths(خالی_اشاریہ),
    current(خالی_اشاریہ),
    current_لمبائی(0),
    nfd(Normalizer2::getNFDInstance(حالت_2)),
    nfcImpl(Normalizer2Factory::getNFCImpl(حالت_2))
{
    اگر(U_SUCCESS(حالت_2) && nfcImpl->ensure_Canon_Iter_ڈیٹا(حالت_2)) {
      سیٹ_ماخذ(ماخذ_Str, حالت_2);
    }
}

CanonicalIterator::~CanonicalIterator() {
  cleanPieces();
}

خالی CanonicalIterator::cleanPieces() {
    int32_t i = 0;
    اگر(pieces != خالی_اشاریہ) {
        برائے(i = 0; i < pieces_لمبائی; i++) {
            اگر(pieces[i] != خالی_اشاریہ) {
                حذف[] pieces[i];
            }
        }
        uprv_free(pieces);
        pieces = خالی_اشاریہ;
        pieces_لمبائی = 0;
    }
    اگر(pieces_lengths != خالی_اشاریہ) {
        uprv_free(pieces_lengths);
        pieces_lengths = خالی_اشاریہ;
    }
    اگر(current != خالی_اشاریہ) {
        uprv_free(current);
        current = خالی_اشاریہ;
        current_لمبائی = 0;
    }
}

/**
 *@return gets the source: NOTE: it is the NFD form of source
 */
UnicodeString CanonicalIterator::get_ماخذ() {
  واپس ماخذ;
}

/**
 * Resets the iterator so that one can start again from the beginning.
 */
خالی CanonicalIterator::reset() {
    مکمل = جھوٹ;
    برائے (صحیح i = 0; i < current_لمبائی; ++i) {
        current[i] = 0;
    }
}

/**
 *@return the next string that is canonically equivalent. The value null is returned when
 * the iteration is done.
 */
UnicodeString CanonicalIterator::next() {
    int32_t i = 0;

    اگر (مکمل) {
      بفر.سیٹ_مقصد_Bogus();
      واپس بفر;
    }

    // delete old contents
    بفر.remove();

    // construct return value

    برائے (i = 0; i < pieces_لمبائی; ++i) {
        بفر.append(pieces[i][current[i]]);
    }
    //String result = buffer.toString(); // not needed

    // find next value for next time

    برائے (i = current_لمبائی - 1; ; --i) {
        اگر (i < 0) {
            مکمل = سچ;
            توڑو;
        }
        current[i]++;
        اگر (current[i] < pieces_lengths[i]) توڑو; // got sequence
        current[i] = 0;
    }
    واپس بفر;
}

/**
 *@param set the source string to iterate against. This allows the same iterator to be used
 * while changing the source string, saving object creation.
 */
خالی CanonicalIterator::سیٹ_ماخذ(ثابت UnicodeString &new_ماخذ, UError_رمز &حالت_2) {
    int32_t list_لمبائی_2 = 0;
    UChar32 cp = 0;
    int32_t آغاز = 0;
    int32_t i = 0;
    UnicodeString *list = خالی_اشاریہ;

    nfd->normalize(new_ماخذ, ماخذ, حالت_2);
    اگر(U_FAILURE(حالت_2)) {
      واپس;
    }
    مکمل = جھوٹ;

    cleanPieces();

    // catch degenerate case
    اگر (new_ماخذ.length() == 0) {
        pieces = static_cast<UnicodeString**>(uprv_malloc(حجم(UnicodeString*)));
        pieces_lengths = static_cast<int32_t*>(uprv_malloc(1 * حجم(int32_t)));
        pieces_لمبائی = 1;
        current = static_cast<int32_t*>(uprv_malloc(1 * حجم(int32_t)));
        current_لمبائی = 1;
        اگر (pieces == خالی_اشاریہ || pieces_lengths == خالی_اشاریہ || current == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            جاؤ CleanPartialInitialization;
        }
        current[0] = 0;
        pieces[0] = نیا UnicodeString[1];
        pieces_lengths[0] = 1;
        اگر (pieces[0] == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            جاؤ CleanPartialInitialization;
        }
        واپس;
    }


    list = نیا UnicodeString[ماخذ.length()];
    اگر (list == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        جاؤ CleanPartialInitialization;
    }

    // i should initially be the number of code units at the 
    // start of the string
    i = U16_لمبائی(ماخذ.char32At(0));
    // int32_t i = 1;
    // find the segments
    // This code iterates through the source string and 
    // extracts segments that end up on a codepoint that
    // doesn't start any decompositions. (Analysis is done
    // on the NFD form - see above).
    برائے (; i < ماخذ.length(); i += U16_لمبائی(cp)) {
        cp = ماخذ.char32At(i);
        اگر (nfcImpl->isCanonSegmentStarter(cp)) {
            ماخذ.extract(آغاز, i-آغاز, list[list_لمبائی_2++]); // add up to i
            آغاز = i;
        }
    }
    ماخذ.extract(آغاز, i-آغاز, list[list_لمبائی_2++]); // add last one


    // allocate the arrays, and find the strings that are CE to each segment
    pieces = static_cast<UnicodeString**>(uprv_malloc(list_لمبائی_2 * حجم(UnicodeString*)));
    pieces_لمبائی = list_لمبائی_2;
    pieces_lengths = static_cast<int32_t*>(uprv_malloc(list_لمبائی_2 * حجم(int32_t)));
    current = static_cast<int32_t*>(uprv_malloc(list_لمبائی_2 * حجم(int32_t)));
    current_لمبائی = list_لمبائی_2;
    اگر (pieces == خالی_اشاریہ || pieces_lengths == خالی_اشاریہ || current == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        جاؤ CleanPartialInitialization;
    }

    برائے (i = 0; i < current_لمبائی; i++) {
        current[i] = 0;
    }
    // for each segment, get all the combinations that can produce 
    // it after NFD normalization
    برائے (i = 0; i < pieces_لمبائی; ++i) {
        //if (PROGRESS) printf("SEGMENT\n");
        pieces[i] = getEquivalents(list[i], pieces_lengths[i], حالت_2);
    }

    حذف[] list;
    واپس;
// Common section to cleanup all local variables and reset object variables.
CleanPartialInitialization:
    حذف[] list;
    cleanPieces();
}

/**
 * Dumb recursive implementation of permutation.
 * TODO: optimize
 * @param source the string to find permutations for
 * @return the results in a set.
 */
خالی U_EXPORT2 CanonicalIterator::ترتیب_بدلو(UnicodeString &ماخذ, UBool skipZeros, Hashtable *نتیجہ, UError_رمز &حالت_2, int32_t گہرائی) {
    اگر(U_FAILURE(حالت_2)) {
        واپس;
    }
    // To avoid infinity loop caused by permute, we limit the depth of recursive
    // call to permute and return U_UNSUPPORTED_ERROR.
    // We know in some unit test we need at least 4. Set to 8 just in case some
    // unforseen use cases.
    constexpr int32_t k_ترتیب_بدلو_گہرائی_حد_بندی = 8;
    اگر (گہرائی > k_ترتیب_بدلو_گہرائی_حد_بندی) {
        حالت_2 = U_UNSUPPORTED_ERROR;
        واپس;
    }
    //if (PROGRESS) printf("Permute: %s\n", UToS(Tr(source)));
    int32_t i = 0;

    // optimization:
    // if zero or one character, just return a set with it
    // we check for length < 2 to keep from counting code points all the time
    اگر (ماخذ.length() <= 2 && ماخذ.گنتی_Char32() <= 1) {
        UnicodeString *مقصد_Put = نیا UnicodeString(ماخذ);
        /* test for nullptr */
        اگر (مقصد_Put == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس;
        }
        نتیجہ->put(ماخذ, مقصد_Put, حالت_2);
        واپس;
    }

    // otherwise iterate through the string, and recursively permute all the other characters
    UChar32 cp;
    Hashtable subpermute(حالت_2);
    اگر(U_FAILURE(حالت_2)) {
        واپس;
    }
    subpermute.سیٹ_قدر_Deleter(uprv_deleteUObject);

    برائے (i = 0; i < ماخذ.length(); i += U16_لمبائی(cp)) {
        cp = ماخذ.char32At(i);
        ثابت UHashElement *ne = خالی_اشاریہ;
        int32_t el = UHASH_پہلا;
        UnicodeString sub_ترتیب_بدلو_String = ماخذ;

        // optimization:
        // if the character is canonical combining class zero,
        // don't permute it
        اگر (skipZeros && i != 0 && u_getCombiningClass(cp) == 0) {
            //System.out.println("Skipping " + Utility.hex(UTF16.valueOf(source, i)));
            جاری;
        }

        subpermute.removeAll();

        // see what the permutations of the characters before and after this one are
        //Hashtable *subpermute = permute(source.substring(0,i) + source.substring(i + UTF16.getCharCount(cp)));
        ترتیب_بدلو(sub_ترتیب_بدلو_String.remove(i, U16_لمبائی(cp)), skipZeros, &subpermute, حالت_2, گہرائی+1);
        /* Test for buffer overflows */
        اگر(U_FAILURE(حالت_2)) {
            واپس;
        }
        // The upper remove is destructive. The question is do we have to make a copy, or we don't care about the contents 
        // of source at this point.

        // prefix this character to all of them
        ne = subpermute.اگلا_Element(el);
        جب (ne != خالی_اشاریہ) {
            UnicodeString* permRes = static_cast<UnicodeString*>(ne->value.اشارہ_گر);
            UnicodeString *chStr = نیا UnicodeString(cp);
            //test for nullptr
            اگر (chStr == خالی_اشاریہ) {
                حالت_2 = U_حافظہ_تخصیص_ERROR;
                واپس;
            }
            chStr->append(*permRes); //*((UnicodeString *)(ne->value.pointer));
            //if (PROGRESS) printf("  Piece: %s\n", UToS(*chStr));
            نتیجہ->put(*chStr, chStr, حالت_2);
            ne = subpermute.اگلا_Element(el);
        }
    }
    //return result;
}

// privates

// we have a segment, in NFD. Find all the strings that are canonically equivalent to it.
UnicodeString* CanonicalIterator::getEquivalents(ثابت UnicodeString &segment, int32_t &نتیجہ_len, UError_رمز &حالت_2) {
    Hashtable نتیجہ(حالت_2);
    Hashtable permutations(حالت_2);
    Hashtable basic(حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    نتیجہ.سیٹ_قدر_Deleter(uprv_deleteUObject);
    permutations.سیٹ_قدر_Deleter(uprv_deleteUObject);
    basic.سیٹ_قدر_Deleter(uprv_deleteUObject);

    char16_t USeg[256];
    int32_t segLen = segment.extract(USeg, 256, حالت_2);
    getEquivalents2(&basic, USeg, segLen, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    // now get all the permutations
    // add only the ones that are canonically equivalent
    // TODO: optimize by not permuting any class zero.

    ثابت UHashElement *ne = خالی_اشاریہ;
    int32_t el = UHASH_پہلا;
    //Iterator it = basic.iterator();
    ne = basic.اگلا_Element(el);
    //while (it.hasNext())
    جب (ne != خالی_اشاریہ) {
        //String item = (String) it.next();
        UnicodeString شے = *static_cast<UnicodeString*>(ne->value.اشارہ_گر);

        permutations.removeAll();
        ترتیب_بدلو(شے, CANITER_SKIP_ZEROES, &permutations, حالت_2);
        ثابت UHashElement *ne2 = خالی_اشاریہ;
        int32_t el2 = UHASH_پہلا;
        //Iterator it2 = permutations.iterator();
        ne2 = permutations.اگلا_Element(el2);
        //while (it2.hasNext())
        جب (ne2 != خالی_اشاریہ) {
            //String possible = (String) it2.next();
            //UnicodeString *possible = new UnicodeString(*((UnicodeString *)(ne2->value.pointer)));
            UnicodeString possible(*static_cast<UnicodeString*>(ne2->value.اشارہ_گر));
            UnicodeString attempt;
            nfd->normalize(possible, attempt, حالت_2);

            // TODO: check if operator == is semanticaly the same as attempt.equals(segment)
            اگر (attempt==segment) {
                //if (PROGRESS) printf("Adding Permutation: %s\n", UToS(Tr(*possible)));
                // TODO: use the hashtable just to catch duplicates - store strings directly (somehow).
                نتیجہ.put(possible, نیا UnicodeString(possible), حالت_2); //add(possible);
            } ورنہ {
                //if (PROGRESS) printf("-Skipping Permutation: %s\n", UToS(Tr(*possible)));
            }

            ne2 = permutations.اگلا_Element(el2);
        }
        ne = basic.اگلا_Element(el);
    }

    /* Test for buffer overflows */
    اگر(U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    // convert into a String[] to clean up storage
    //String[] finalResult = new String[result.size()];
    UnicodeString *final_نتیجہ = خالی_اشاریہ;
    int32_t نتیجہ_گنتی;
    اگر((نتیجہ_گنتی = نتیجہ.count()) != 0) {
        final_نتیجہ = نیا UnicodeString[نتیجہ_گنتی];
        اگر (final_نتیجہ == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
            واپس خالی_اشاریہ;
        }
    }
    ورنہ {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    //result.toArray(finalResult);
    نتیجہ_len = 0;
    el = UHASH_پہلا;
    ne = نتیجہ.اگلا_Element(el);
    جب(ne != خالی_اشاریہ) {
        final_نتیجہ[نتیجہ_len++] = *static_cast<UnicodeString*>(ne->value.اشارہ_گر);
        ne = نتیجہ.اگلا_Element(el);
    }


    واپس final_نتیجہ;
}

Hashtable* CanonicalIterator::getEquivalents2(Hashtable* fillin_نتیجہ U_LIFETIME_BOUND,
                                              ثابت char16_t* segment,
                                              int32_t segLen,
                                              UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(segment)));

    UnicodeString مقصد_Put(segment, segLen);

    fillin_نتیجہ->put(مقصد_Put, نیا UnicodeString(مقصد_Put), حالت_2);

    Unicode_سیٹ starts;

    // cycle through all the characters
    UChar32 cp;
    برائے (int32_t i = 0; i < segLen; i += U16_لمبائی(cp)) {
        // see if any character is at the start of some decomposition
        U16_GET(segment, 0, i, segLen, cp);
        اگر (!nfcImpl->get_Canon_آغاز_سیٹ(cp, starts)) {
            جاری;
        }
        // if so, see which decompositions match
        Unicode_سیٹ_Iterator iter(starts);
        جب (iter.next()) {
            UChar32 cp2 = iter.getCodepoint();
            Hashtable remainder(حالت_2);
            remainder.سیٹ_قدر_Deleter(uprv_deleteUObject);
            اگر (extract(&remainder, cp2, segment, segLen, i, حالت_2) == خالی_اشاریہ) {
                اگر (U_FAILURE(حالت_2)) {
                    واپس خالی_اشاریہ;
                }
                جاری;
            }

            // there were some matches, so add all the possibilities to the set.
            UnicodeString سابقہ(segment, i);
            سابقہ += cp2;

            int32_t el = UHASH_پہلا;
            ثابت UHashElement *ne = remainder.اگلا_Element(el);
            جب (ne != خالی_اشاریہ) {
                UnicodeString شے = *static_cast<UnicodeString*>(ne->value.اشارہ_گر);
                UnicodeString *مقصد_جوڑو = نیا UnicodeString(سابقہ);
                /* test for nullptr */
                اگر (مقصد_جوڑو == خالی_اشاریہ) {
                    حالت_2 = U_حافظہ_تخصیص_ERROR;
                    واپس خالی_اشاریہ;
                }
                *مقصد_جوڑو += شے;
                fillin_نتیجہ->put(*مقصد_جوڑو, مقصد_جوڑو, حالت_2);

                //if (PROGRESS) printf("Adding: %s\n", UToS(Tr(*toAdd)));

                ne = remainder.اگلا_Element(el);
            }
            // ICU-22642 Guards against strings that have so many permutations
            // that they would otherwise hang the function.
            constexpr int32_t k_نتیجہ_حد_بندی = 4096;
            اگر (fillin_نتیجہ->count() > k_نتیجہ_حد_بندی) {
                حالت_2 = U_UNSUPPORTED_ERROR;
                واپس خالی_اشاریہ;
            }
        }
    }

    /* Test for buffer overflows */
    اگر(U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    واپس fillin_نتیجہ;
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
                                      UError_رمز& حالت_2) {
//Hashtable *CanonicalIterator::extract(UChar32 comp, const UnicodeString &segment, int32_t segLen, int32_t segmentPos, UErrorCode &status) {
    //if (PROGRESS) printf(" extract: %s, ", UToS(Tr(UnicodeString(comp))));
    //if (PROGRESS) printf("%s, %i\n", UToS(Tr(segment)), segmentPos);

    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    UnicodeString عارضی(comp);
    int32_t داخلہ_Len=عارضی.length();
    UnicodeString decompString;
    nfd->normalize(عارضی, decompString, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر (decompString.isBogus()) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    ثابت char16_t *decomp=decompString.get_بفر();
    int32_t decompLen=decompString.length();

    // See if it matches the start of segment (at segmentPos)
    UBool ok = جھوٹ;
    UChar32 cp;
    int32_t decompPos = 0;
    UChar32 decompCp;
    U16_اگلا(decomp, decompPos, decompLen, decompCp);

    int32_t i = segmentPos;
    جب(i < segLen) {
        U16_اگلا(segment, i, segLen, cp);

        اگر (cp == decompCp) { // if equal, eat another cp from decomp

            //if (PROGRESS) printf("  matches: %s\n", UToS(Tr(UnicodeString(cp))));

            اگر (decompPos == decompLen) { // done, have all decomp characters!
                عارضی.append(segment+i, segLen-i);
                ok = سچ;
                توڑو;
            }
            U16_اگلا(decomp, decompPos, decompLen, decompCp);
        } ورنہ {
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
    اگر (!ok)
        واپس خالی_اشاریہ; // we failed, characters left over

    //if (PROGRESS) printf("Matches\n");

    اگر (داخلہ_Len == عارضی.length()) {
        fillin_نتیجہ->put(UnicodeString(), نیا UnicodeString(), حالت_2);
        واپس fillin_نتیجہ; // succeed, but no remainder
    }

    // brute force approach
    // check to make sure result is canonically equivalent
    UnicodeString trial;
    nfd->normalize(عارضی, trial, حالت_2);
    اگر(U_FAILURE(حالت_2) || trial.compare(segment+segmentPos, segLen - segmentPos) != 0) {
        واپس خالی_اشاریہ;
    }

    واپس getEquivalents2(fillin_نتیجہ, عارضی.get_بفر()+داخلہ_Len, عارضی.length()-داخلہ_Len, حالت_2);
}

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_NORMALIZATION */
