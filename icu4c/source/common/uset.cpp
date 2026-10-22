// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uset.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002mar07
*   created by: Markus W. Scherer
*
*   There are functions to efficiently serialize a USet into an array of uint16_t
*   and functions to use such a serialized form efficiently without
*   instantiating a new USet.
*/

#समावेश "unicode/utypes.h"
#समावेश "unicode/char16ptr.h"
#समावेश "unicode/uobject.h"
#समावेश "unicode/uset.h"
#समावेश "unicode/uniset.h"
#समावेश "cmemory.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openEmpty() {
    वापस (USet*) नया UnicodeSet();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 start, UChar32 end) {
    वापस (USet*) नया UnicodeSet(start, end);
}

U_CAPI व्योम U_EXPORT2
uset_close(USet* set) {
    मिटाओ (UnicodeSet*) set;
}

U_CAPI USet * U_EXPORT2
uset_clone(स्थिर USet *set) {
    वापस (USet*) (((UnicodeSet*) set)->UnicodeSet::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(स्थिर USet *set) {
    वापस ((UnicodeSet*) set)->UnicodeSet::isFrozen();
}

U_CAPI व्योम U_EXPORT2
uset_freeze(USet *set) {
    ((UnicodeSet*) set)->UnicodeSet::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(स्थिर USet *set) {
    वापस (USet*) (((UnicodeSet*) set)->UnicodeSet::cloneAsThawed());
}

U_CAPI व्योम U_EXPORT2
uset_set(USet* set,
     UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::set(start, end);
}

U_CAPI व्योम U_EXPORT2
uset_addAll(USet* set, स्थिर USet *additionalSet) {
    ((UnicodeSet*) set)->UnicodeSet::addAll(*((स्थिर UnicodeSet*)additionalSet));
}

U_CAPI व्योम U_EXPORT2
uset_add(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::add(c);
}

U_CAPI व्योम U_EXPORT2
uset_addRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::add(start, end);    
}

U_CAPI व्योम U_EXPORT2
uset_addString(USet* set, स्थिर char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::add(s);
}

U_CAPI व्योम U_EXPORT2
uset_addAllCodePoints(USet* set, स्थिर char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::addAll(s);
}

U_CAPI व्योम U_EXPORT2
uset_remove(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::हटाओ(c);
}

U_CAPI व्योम U_EXPORT2
uset_removeRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::हटाओ(start, end);
}

U_CAPI व्योम U_EXPORT2
uset_removeString(USet* set, स्थिर char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::हटाओ(s);
}

U_CAPI व्योम U_EXPORT2
uset_removeAllCodePoints(USet *set, स्थिर char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::removeAll(s);
}

U_CAPI व्योम U_EXPORT2
uset_removeAll(USet* set, स्थिर USet* हटाओ) {
    ((UnicodeSet*) set)->UnicodeSet::removeAll(*(स्थिर UnicodeSet*)हटाओ);
}

U_CAPI व्योम U_EXPORT2
uset_retain(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::retain(start, end);
}

U_CAPI व्योम U_EXPORT2
uset_retainString(USet *set, स्थिर char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retain(s);
}

U_CAPI व्योम U_EXPORT2
uset_retainAllCodePoints(USet *set, स्थिर char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retainAll(s);
}

U_CAPI व्योम U_EXPORT2
uset_retainAll(USet* set, स्थिर USet* retain) {
    ((UnicodeSet*) set)->UnicodeSet::retainAll(*(स्थिर UnicodeSet*)retain);
}

U_CAPI व्योम U_EXPORT2
uset_compact(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::compact();
}

U_CAPI व्योम U_EXPORT2
uset_complement(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::complement();
}

U_CAPI व्योम U_EXPORT2
uset_complementRange(USet *set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::complement(start, end);
}

U_CAPI व्योम U_EXPORT2
uset_complementString(USet *set, स्थिर char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complement(s);
}

U_CAPI व्योम U_EXPORT2
uset_complementAllCodePoints(USet *set, स्थिर char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complementAll(s);
}

U_CAPI व्योम U_EXPORT2
uset_complementAll(USet* set, स्थिर USet* complement) {
    ((UnicodeSet*) set)->UnicodeSet::complementAll(*(स्थिर UnicodeSet*)complement);
}

U_CAPI व्योम U_EXPORT2
uset_clear(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::सफा();
}

U_CAPI व्योम U_EXPORT2
uset_removeAllStrings(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::removeAllStrings();
}

U_CAPI UBool U_EXPORT2
uset_isEmpty(स्थिर USet* set) {
    वापस ((स्थिर UnicodeSet*) set)->UnicodeSet::isEmpty();
}

U_CAPI UBool U_EXPORT2
uset_hasStrings(स्थिर USet* set) {
    वापस ((स्थिर UnicodeSet*) set)->UnicodeSet::hasStrings();
}

U_CAPI UBool U_EXPORT2
uset_contains(स्थिर USet* set, UChar32 c) {
    वापस ((स्थिर UnicodeSet*) set)->UnicodeSet::contains(c);
}

U_CAPI UBool U_EXPORT2
uset_containsRange(स्थिर USet* set, UChar32 start, UChar32 end) {
    वापस ((स्थिर UnicodeSet*) set)->UnicodeSet::contains(start, end);
}

U_CAPI UBool U_EXPORT2
uset_containsString(स्थिर USet* set, स्थिर char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    वापस ((स्थिर UnicodeSet*) set)->UnicodeSet::contains(s);
}

U_CAPI UBool U_EXPORT2
uset_containsAll(स्थिर USet* set1, स्थिर USet* set2) {
    वापस ((स्थिर UnicodeSet*) set1)->UnicodeSet::containsAll(* (स्थिर UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsAllCodePoints(स्थिर USet* set, स्थिर char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    वापस ((स्थिर UnicodeSet*) set)->UnicodeSet::containsAll(s);
}

U_CAPI UBool U_EXPORT2
uset_containsNone(स्थिर USet* set1, स्थिर USet* set2) {
    वापस ((स्थिर UnicodeSet*) set1)->UnicodeSet::containsNone(* (स्थिर UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsSome(स्थिर USet* set1, स्थिर USet* set2) {
    वापस ((स्थिर UnicodeSet*) set1)->UnicodeSet::containsSome(* (स्थिर UnicodeSet*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_span(स्थिर USet *set, स्थिर char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    वापस ((UnicodeSet*) set)->UnicodeSet::span(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBack(स्थिर USet *set, स्थिर char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    वापस ((UnicodeSet*) set)->UnicodeSet::spanBack(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanUTF8(स्थिर USet *set, स्थिर अक्षर *s, int32_t length, USetSpanCondition spanCondition) {
    वापस ((UnicodeSet*) set)->UnicodeSet::spanUTF8(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBackUTF8(स्थिर USet *set, स्थिर अक्षर *s, int32_t length, USetSpanCondition spanCondition) {
    वापस ((UnicodeSet*) set)->UnicodeSet::spanBackUTF8(s, length, spanCondition);
}

U_CAPI UBool U_EXPORT2
uset_equals(स्थिर USet* set1, स्थिर USet* set2) {
    वापस *(स्थिर UnicodeSet*)set1 == *(स्थिर UnicodeSet*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_सूचकांक_Of(स्थिर USet* set, UChar32 c) {
    वापस ((UnicodeSet*) set)->UnicodeSet::सूचकांक_Of(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(स्थिर USet* set, int32_t सूचकांक) {
    वापस ((UnicodeSet*) set)->UnicodeSet::charAt(सूचकांक);
}

U_CAPI int32_t U_EXPORT2
uset_आकार(स्थिर USet* set) {
    वापस ((स्थिर UnicodeSet*) set)->UnicodeSet::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
श्रेणी USetAccess /* not : public UObject because all methods are static */ {
खुला:
    /* Try to have the compiler inline these*/
    अंतरभूत अटल int32_t get_String_गिनती(स्थिर UnicodeSet& set) {
        वापस set.strings_आकार();
    }
    अंतरभूत अटल स्थिर UnicodeString* getString(स्थिर UnicodeSet& set U_LIFETIME_BOUND,
                                                 int32_t i) {
        वापस set.getString(i);
    }
निजी:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_END

U_CAPI int32_t U_EXPORT2
uset_get_Range_गिनती(स्थिर USet *set) {
    वापस ((स्थिर UnicodeSet *)set)->UnicodeSet::get_Range_गिनती();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_गिनती(स्थिर USet *uset) {
    स्थिर UnicodeSet &set = *(स्थिर UnicodeSet *)uset;
    वापस USetAccess::get_String_गिनती(set);
}

U_CAPI int32_t U_EXPORT2
uset_get_Item_गिनती(स्थिर USet* uset) {
    स्थिर UnicodeSet& set = *(स्थिर UnicodeSet*)uset;
    वापस set.get_Range_गिनती() + USetAccess::get_String_गिनती(set);
}

U_CAPI स्थिर UChar* U_EXPORT2
uset_getString(स्थिर USet* uset U_LIFETIME_BOUND, int32_t सूचकांक, int32_t* p_लंबाई) {
    अगर (p_लंबाई == nullptr) { वापस nullptr; }
    स्थिर UnicodeSet &set = *(स्थिर UnicodeSet *)uset;
    int32_t count = USetAccess::get_String_गिनती(set);
    अगर (सूचकांक < 0 || count <= सूचकांक) {
        *p_लंबाई = 0;
        वापस nullptr;
    }
    स्थिर UnicodeString *s = USetAccess::getString(set, सूचकांक);
    *p_लंबाई = s->length();
    वापस toUCharPtr(s->get_बफर());
}

U_CAPI int32_t U_EXPORT2
uset_getItem(स्थिर USet* uset, int32_t item_सूचकांक,
             UChar32* start, UChar32* end,
             char16_t* str, int32_t strCapacity,
             UErrorCode* ec) {
    अगर (U_FAILURE(*ec)) वापस 0;
    स्थिर UnicodeSet& set = *(स्थिर UnicodeSet*)uset;
    int32_t range_गिनती;

    अगर (item_सूचकांक < 0) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        वापस -1;
    } अन्यथा अगर (item_सूचकांक < (range_गिनती = set.get_Range_गिनती())) {
        *start = set.getRangeStart(item_सूचकांक);
        *end = set.getRangeEnd(item_सूचकांक);
        वापस 0;
    } अन्यथा {
        item_सूचकांक -= range_गिनती;
        अगर (item_सूचकांक < USetAccess::get_String_गिनती(set)) {
            स्थिर UnicodeString* s = USetAccess::getString(set, item_सूचकांक);
            वापस s->extract(str, strCapacity, *ec);
        } अन्यथा {
            *ec = U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस -1;
        }
    }
}

//U_CAPI UBool U_EXPORT2
//uset_getRange(const USet* set, int32_t rangeIndex,
//              UChar32* pStart, UChar32* pEnd) {
//    if ((uint32_t) rangeIndex >= (uint32_t) uset_getRangeCount(set)) {
//        return false;
//    }
//    const UnicodeSet* us = (const UnicodeSet*) set;
//    *pStart = us->getRangeStart(rangeIndex);
//    *pEnd = us->getRangeEnd(rangeIndex);
//    return true;
//}

/*
 * Serialize a USet into 16-bit units.
 * Store BMP code points as themselves with one 16-bit unit each.
 *
 * Important: the code points in the array are in ascending order,
 * therefore all BMP code points precede all supplementary code points.
 *
 * Store each supplementary code point in 2 16-bit units,
 * simply with higher-then-lower 16-bit halves.
 *
 * Precede the entire list with the length.
 * If there are supplementary code points, then set bit 15 in the length
 * and add the bmpLength between it and the array.
 *
 * In other words:
 * - all BMP:            (length=bmpLength) BMP, .., BMP
 * - some supplementary: (length|0x8000) (bmpLength<length) BMP, .., BMP, supp-high, supp-low, ..
 */
U_CAPI int32_t U_EXPORT2
uset_serialize(स्थिर USet* set, uint16_t* dest, int32_t destCapacity, UErrorCode* ec) {
    अगर (ec==nullptr || U_FAILURE(*ec)) {
        वापस 0;
    }

    वापस ((स्थिर UnicodeSet*) set)->UnicodeSet::serialize(dest, destCapacity,* ec);
}

U_CAPI UBool U_EXPORT2
uset_getSerializedSet(USerializedSet* fillSet, स्थिर uint16_t* src, int32_t src_लंबाई) {
    int32_t length;

    अगर(fillSet==nullptr) {
        वापस false;
    }
    अगर(src==nullptr || src_लंबाई<=0) {
        fillSet->length=fillSet->bmp_लंबाई=0;
        वापस false;
    }

    length=*src++;
    अगर(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        अगर(src_लंबाई<(2+length)) {
            fillSet->length=fillSet->bmp_लंबाई=0;
            वापस false;
        }
        fillSet->bmp_लंबाई=*src++;
    } अन्यथा {
        /* only BMP values */
        अगर(src_लंबाई<(1+length)) {
            fillSet->length=fillSet->bmp_लंबाई=0;
            वापस false;
        }
        fillSet->bmp_लंबाई=length;
    }
    fillSet->array=src;
    fillSet->length=length;
    वापस true;
}

U_CAPI व्योम U_EXPORT2
uset_setSerializedToOne(USerializedSet* fillSet, UChar32 c) {
    अगर(fillSet==nullptr || (uint32_t)c>0x10ffff) {
        वापस;
    }

    fillSet->array=fillSet->staticArray;
    अगर(c<0xffff) {
        fillSet->bmp_लंबाई=fillSet->length=2;
        fillSet->staticArray[0]=(uint16_t)c;
        fillSet->staticArray[1]=(uint16_t)c+1;
    } अन्यथा अगर(c==0xffff) {
        fillSet->bmp_लंबाई=1;
        fillSet->length=3;
        fillSet->staticArray[0]=0xffff;
        fillSet->staticArray[1]=1;
        fillSet->staticArray[2]=0;
    } अन्यथा अगर(c<0x10ffff) {
        fillSet->bmp_लंबाई=0;
        fillSet->length=4;
        fillSet->staticArray[0]=(uint16_t)(c>>16);
        fillSet->staticArray[1]=(uint16_t)c;
        ++c;
        fillSet->staticArray[2]=(uint16_t)(c>>16);
        fillSet->staticArray[3]=(uint16_t)c;
    } अन्यथा /* c==0x10ffff */ {
        fillSet->bmp_लंबाई=0;
        fillSet->length=2;
        fillSet->staticArray[0]=0x10;
        fillSet->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serializedContains(स्थिर USerializedSet* set, UChar32 c) {
    स्थिर uint16_t* array;

    अगर(set==nullptr || (uint32_t)c>0x10ffff) {
        वापस false;
    }

    array=set->array;
    अगर(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = set->bmp_लंबाई-1;
        अगर (c < array[0]) {
            hi = 0;
        } अन्यथा अगर (c < array[hi]) {
            क्रम(;;) {
                int32_t i = (lo + hi) >> 1;
                अगर (i == lo) {
                    अवरोध;  // Done!
                } अन्यथा अगर (c < array[i]) {
                    hi = i;
                } अन्यथा {
                    lo = i;
                }
            }
        } अन्यथा {
            hi += 1;
        }
        वापस hi&1;
    } अन्यथा {
        /* find c in the supplementary part */
        uint16_t ऊपरी=(uint16_t)(c>>16), निचला=(uint16_t)c;
        int32_t base = set->bmp_लंबाई;
        int32_t lo = 0;
        int32_t hi = set->length - 2 - base;
        अगर (ऊपरी < array[base] || (ऊपरी==array[base] && निचला<array[base+1])) {
            hi = 0;
        } अन्यथा अगर (ऊपरी < array[base+hi] || (ऊपरी==array[base+hi] && निचला<array[base+hi+1])) {
            क्रम (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + base;
                अगर (i == lo) {
                    अवरोध;  // Done!
                } अन्यथा अगर (ऊपरी < array[iabs] || (ऊपरी==array[iabs] && निचला<array[iabs+1])) {
                    hi = i;
                } अन्यथा {
                    lo = i;
                }
            }
        } अन्यथा {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        वापस ((hi+(base<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_गिनती(स्थिर USerializedSet* set) {
    अगर(set==nullptr) {
        वापस 0;
    }

    वापस (set->bmp_लंबाई+(set->length-set->bmp_लंबाई)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(स्थिर USerializedSet* set, int32_t range_सूचकांक,
                        UChar32* pStart, UChar32* pEnd) {
    स्थिर uint16_t* array;
    int32_t bmp_लंबाई, length;

    अगर(set==nullptr || range_सूचकांक<0 || pStart==nullptr || pEnd==nullptr) {
        वापस false;
    }

    array=set->array;
    length=set->length;
    bmp_लंबाई=set->bmp_लंबाई;

    range_सूचकांक*=2; /* address start/limit pairs */
    अगर(range_सूचकांक<bmp_लंबाई) {
        *pStart=array[range_सूचकांक++];
        अगर(range_सूचकांक<bmp_लंबाई) {
            *pEnd=array[range_सूचकांक]-1;
        } अन्यथा अगर(range_सूचकांक<length) {
            *pEnd=((((int32_t)array[range_सूचकांक])<<16)|array[range_सूचकांक+1])-1;
        } अन्यथा {
            *pEnd=0x10ffff;
        }
        वापस true;
    } अन्यथा {
        range_सूचकांक-=bmp_लंबाई;
        range_सूचकांक*=2; /* address pairs of pairs of units */
        length-=bmp_लंबाई;
        अगर(range_सूचकांक<length) {
            array+=bmp_लंबाई;
            *pStart=(((int32_t)array[range_सूचकांक])<<16)|array[range_सूचकांक+1];
            range_सूचकांक+=2;
            अगर(range_सूचकांक<length) {
                *pEnd=((((int32_t)array[range_सूचकांक])<<16)|array[range_सूचकांक+1])-1;
            } अन्यथा {
                *pEnd=0x10ffff;
            }
            वापस true;
        } अन्यथा {
            वापस false;
        }
    }
}

// TODO The old, internal uset.c had an efficient uset_containsOne function.
// Returned the one and only code point, or else -1 or something.
// Consider adding such a function to both C and C++ UnicodeSet/uset.
// See tools/gennorm/store.c for usage, now usetContainsOne there.

// TODO Investigate incorporating this code into UnicodeSet to improve
// efficiency.
// ---
// #define USET_GROW_DELTA 20
// 
// static int32_t
// findChar(const UChar32* array, int32_t length, UChar32 c) {
//     int32_t i;
// 
//     /* check the last range limit first for more efficient appending */
//     if(length>0) {
//         if(c>=array[length-1]) {
//             return length;
//         }
// 
//         /* do not check the last range limit again in the loop below */
//         --length;
//     }
// 
//     for(i=0; i<length && c>=array[i]; ++i) {}
//     return i;
// }
// 
// static UBool
// addRemove(USet* set, UChar32 c, int32_t doRemove) {
//     int32_t i, length, more;
// 
//     if(set==nullptr || (uint32_t)c>0x10ffff) {
//         return false;
//     }
// 
//     length=set->length;
//     i=findChar(set->array, length, c);
//     if((i&1)^doRemove) {
//         /* c is already in the set */
//         return true;
//     }
// 
//     /* how many more array items do we need? */
//     if(i<length && (c+1)==set->array[i]) {
//         /* c is just before the following range, extend that in-place by one */
//         set->array[i]=c;
//         if(i>0) {
//             --i;
//             if(c==set->array[i]) {
//                 /* the previous range collapsed, remove it */
//                 set->length=length-=2;
//                 if(i<length) {
//                     uprv_memmove(set->array+i, set->array+i+2, (length-i)*4);
//                 }
//             }
//         }
//         return true;
//     } else if(i>0 && c==set->array[i-1]) {
//         /* c is just after the previous range, extend that in-place by one */
//         if(++c<=0x10ffff) {
//             set->array[i-1]=c;
//             if(i<length && c==set->array[i]) {
//                 /* the following range collapsed, remove it */
//                 --i;
//                 set->length=length-=2;
//                 if(i<length) {
//                     uprv_memmove(set->array+i, set->array+i+2, (length-i)*4);
//                 }
//             }
//         } else {
//             /* extend the previous range (had limit 0x10ffff) to the end of Unicode */
//             set->length=i-1;
//         }
//         return true;
//     } else if(i==length && c==0x10ffff) {
//         /* insert one range limit c */
//         more=1;
//     } else {
//         /* insert two range limits c, c+1 */
//         more=2;
//     }
// 
//     /* insert <more> range limits */
//     if(length+more>set->capacity) {
//         /* reallocate */
//         int32_t newCapacity=set->capacity+set->capacity/2+USET_GROW_DELTA;
//         UChar32* newArray=(UChar32* )uprv_malloc(newCapacity*4);
//         if(newArray==nullptr) {
//             return false;
//         }
//         set->capacity=newCapacity;
//         uprv_memcpy(newArray, set->array, length*4);
// 
//         if(set->array!=set->staticBuffer) {
//             uprv_free(set->array);
//         }
//         set->array=newArray;
//     }
// 
//     if(i<length) {
//         uprv_memmove(set->array+i+more, set->array+i, (length-i)*4);
//     }
//     set->array[i]=c;
//     if(more==2) {
//         set->array[i+1]=c+1;
//     }
//     set->length+=more;
// 
//     return true;
// }
// 
// U_CAPI UBool U_EXPORT2
// uset_add(USet* set, UChar32 c) {
//     return addRemove(set, c, 0);
// }
// 
// U_CAPI void U_EXPORT2
// uset_remove(USet* set, UChar32 c) {
//     addRemove(set, c, 1);
// }
