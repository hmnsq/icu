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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/char16ptr.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openEmpty() {
    परत (USet*) नवीन UnicodeSet();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 start, UChar32 end) {
    परत (USet*) नवीन UnicodeSet(start, end);
}

U_CAPI शून्य U_EXPORT2
uset_close(USet* set) {
    delete (UnicodeSet*) set;
}

U_CAPI USet * U_EXPORT2
uset_clone(स्थिरांक USet *set) {
    परत (USet*) (((UnicodeSet*) set)->UnicodeSet::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(स्थिरांक USet *set) {
    परत ((UnicodeSet*) set)->UnicodeSet::isFrozen();
}

U_CAPI शून्य U_EXPORT2
uset_freeze(USet *set) {
    ((UnicodeSet*) set)->UnicodeSet::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(स्थिरांक USet *set) {
    परत (USet*) (((UnicodeSet*) set)->UnicodeSet::cloneAsThawed());
}

U_CAPI शून्य U_EXPORT2
uset_set(USet* set,
     UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::set(start, end);
}

U_CAPI शून्य U_EXPORT2
uset_addAll(USet* set, स्थिरांक USet *additionalSet) {
    ((UnicodeSet*) set)->UnicodeSet::addAll(*((स्थिरांक UnicodeSet*)additionalSet));
}

U_CAPI शून्य U_EXPORT2
uset_add(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::add(c);
}

U_CAPI शून्य U_EXPORT2
uset_addRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::add(start, end);    
}

U_CAPI शून्य U_EXPORT2
uset_addString(USet* set, स्थिरांक char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::add(s);
}

U_CAPI शून्य U_EXPORT2
uset_addAllCodePoints(USet* set, स्थिरांक char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::addAll(s);
}

U_CAPI शून्य U_EXPORT2
uset_remove(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::हटाओ(c);
}

U_CAPI शून्य U_EXPORT2
uset_removeRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::हटाओ(start, end);
}

U_CAPI शून्य U_EXPORT2
uset_removeString(USet* set, स्थिरांक char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::हटाओ(s);
}

U_CAPI शून्य U_EXPORT2
uset_removeAllCodePoints(USet *set, स्थिरांक char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::removeAll(s);
}

U_CAPI शून्य U_EXPORT2
uset_removeAll(USet* set, स्थिरांक USet* हटाओ) {
    ((UnicodeSet*) set)->UnicodeSet::removeAll(*(स्थिरांक UnicodeSet*)हटाओ);
}

U_CAPI शून्य U_EXPORT2
uset_retain(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::retain(start, end);
}

U_CAPI शून्य U_EXPORT2
uset_retainString(USet *set, स्थिरांक char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retain(s);
}

U_CAPI शून्य U_EXPORT2
uset_retainAllCodePoints(USet *set, स्थिरांक char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retainAll(s);
}

U_CAPI शून्य U_EXPORT2
uset_retainAll(USet* set, स्थिरांक USet* retain) {
    ((UnicodeSet*) set)->UnicodeSet::retainAll(*(स्थिरांक UnicodeSet*)retain);
}

U_CAPI शून्य U_EXPORT2
uset_compact(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::compact();
}

U_CAPI शून्य U_EXPORT2
uset_complement(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::complement();
}

U_CAPI शून्य U_EXPORT2
uset_complementRange(USet *set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::complement(start, end);
}

U_CAPI शून्य U_EXPORT2
uset_complementString(USet *set, स्थिरांक char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complement(s);
}

U_CAPI शून्य U_EXPORT2
uset_complementAllCodePoints(USet *set, स्थिरांक char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complementAll(s);
}

U_CAPI शून्य U_EXPORT2
uset_complementAll(USet* set, स्थिरांक USet* complement) {
    ((UnicodeSet*) set)->UnicodeSet::complementAll(*(स्थिरांक UnicodeSet*)complement);
}

U_CAPI शून्य U_EXPORT2
uset_clear(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::clear();
}

U_CAPI शून्य U_EXPORT2
uset_removeAllStrings(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::removeAllStrings();
}

U_CAPI UBool U_EXPORT2
uset_isEmpty(स्थिरांक USet* set) {
    परत ((स्थिरांक UnicodeSet*) set)->UnicodeSet::isEmpty();
}

U_CAPI UBool U_EXPORT2
uset_hasStrings(स्थिरांक USet* set) {
    परत ((स्थिरांक UnicodeSet*) set)->UnicodeSet::hasStrings();
}

U_CAPI UBool U_EXPORT2
uset_contains(स्थिरांक USet* set, UChar32 c) {
    परत ((स्थिरांक UnicodeSet*) set)->UnicodeSet::contains(c);
}

U_CAPI UBool U_EXPORT2
uset_containsRange(स्थिरांक USet* set, UChar32 start, UChar32 end) {
    परत ((स्थिरांक UnicodeSet*) set)->UnicodeSet::contains(start, end);
}

U_CAPI UBool U_EXPORT2
uset_containsString(स्थिरांक USet* set, स्थिरांक char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    परत ((स्थिरांक UnicodeSet*) set)->UnicodeSet::contains(s);
}

U_CAPI UBool U_EXPORT2
uset_containsAll(स्थिरांक USet* set1, स्थिरांक USet* set2) {
    परत ((स्थिरांक UnicodeSet*) set1)->UnicodeSet::containsAll(* (स्थिरांक UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsAllCodePoints(स्थिरांक USet* set, स्थिरांक char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    परत ((स्थिरांक UnicodeSet*) set)->UnicodeSet::containsAll(s);
}

U_CAPI UBool U_EXPORT2
uset_containsNone(स्थिरांक USet* set1, स्थिरांक USet* set2) {
    परत ((स्थिरांक UnicodeSet*) set1)->UnicodeSet::containsNone(* (स्थिरांक UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsSome(स्थिरांक USet* set1, स्थिरांक USet* set2) {
    परत ((स्थिरांक UnicodeSet*) set1)->UnicodeSet::containsSome(* (स्थिरांक UnicodeSet*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_span(स्थिरांक USet *set, स्थिरांक char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    परत ((UnicodeSet*) set)->UnicodeSet::span(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBack(स्थिरांक USet *set, स्थिरांक char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    परत ((UnicodeSet*) set)->UnicodeSet::spanBack(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanUTF8(स्थिरांक USet *set, स्थिरांक अक्षर *s, int32_t length, USetSpanCondition spanCondition) {
    परत ((UnicodeSet*) set)->UnicodeSet::spanUTF8(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBackUTF8(स्थिरांक USet *set, स्थिरांक अक्षर *s, int32_t length, USetSpanCondition spanCondition) {
    परत ((UnicodeSet*) set)->UnicodeSet::spanBackUTF8(s, length, spanCondition);
}

U_CAPI UBool U_EXPORT2
uset_equals(स्थिरांक USet* set1, स्थिरांक USet* set2) {
    परत *(स्थिरांक UnicodeSet*)set1 == *(स्थिरांक UnicodeSet*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_indexOf(स्थिरांक USet* set, UChar32 c) {
    परत ((UnicodeSet*) set)->UnicodeSet::indexOf(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(स्थिरांक USet* set, int32_t index) {
    परत ((UnicodeSet*) set)->UnicodeSet::charAt(index);
}

U_CAPI int32_t U_EXPORT2
uset_size(स्थिरांक USet* set) {
    परत ((स्थिरांक UnicodeSet*) set)->UnicodeSet::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
वर्ग USetAccess /* not : public UObject because all methods are static */ {
सार्वजनिक:
    /* Try to have the compiler inline these*/
    ओळीत स्थिर int32_t get_String_मोजणी(स्थिरांक UnicodeSet& set) {
        परत set.stringsSize();
    }
    ओळीत स्थिर स्थिरांक UnicodeString* getString(स्थिरांक UnicodeSet& set U_LIFETIME_BOUND,
                                                 int32_t i) {
        परत set.getString(i);
    }
खाजगी:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_END

U_CAPI int32_t U_EXPORT2
uset_get_Range_मोजणी(स्थिरांक USet *set) {
    परत ((स्थिरांक UnicodeSet *)set)->UnicodeSet::get_Range_मोजणी();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_मोजणी(स्थिरांक USet *uset) {
    स्थिरांक UnicodeSet &set = *(स्थिरांक UnicodeSet *)uset;
    परत USetAccess::get_String_मोजणी(set);
}

U_CAPI int32_t U_EXPORT2
uset_get_Item_मोजणी(स्थिरांक USet* uset) {
    स्थिरांक UnicodeSet& set = *(स्थिरांक UnicodeSet*)uset;
    परत set.get_Range_मोजणी() + USetAccess::get_String_मोजणी(set);
}

U_CAPI स्थिरांक UChar* U_EXPORT2
uset_getString(स्थिरांक USet* uset U_LIFETIME_BOUND, int32_t index, int32_t* pLength) {
    जर (pLength == nullptr) { परत nullptr; }
    स्थिरांक UnicodeSet &set = *(स्थिरांक UnicodeSet *)uset;
    int32_t count = USetAccess::get_String_मोजणी(set);
    जर (index < 0 || count <= index) {
        *pLength = 0;
        परत nullptr;
    }
    स्थिरांक UnicodeString *s = USetAccess::getString(set, index);
    *pLength = s->length();
    परत toUCharPtr(s->getBuffer());
}

U_CAPI int32_t U_EXPORT2
uset_getItem(स्थिरांक USet* uset, int32_t itemIndex,
             UChar32* start, UChar32* end,
             char16_t* str, int32_t strCapacity,
             UErrorCode* ec) {
    जर (U_FAILURE(*ec)) परत 0;
    स्थिरांक UnicodeSet& set = *(स्थिरांक UnicodeSet*)uset;
    int32_t range_मोजणी;

    जर (itemIndex < 0) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        परत -1;
    } अन्यथा जर (itemIndex < (range_मोजणी = set.get_Range_मोजणी())) {
        *start = set.getRangeStart(itemIndex);
        *end = set.getRangeEnd(itemIndex);
        परत 0;
    } अन्यथा {
        itemIndex -= range_मोजणी;
        जर (itemIndex < USetAccess::get_String_मोजणी(set)) {
            स्थिरांक UnicodeString* s = USetAccess::getString(set, itemIndex);
            परत s->extract(str, strCapacity, *ec);
        } अन्यथा {
            *ec = U_INDEX_OUTOFBOUNDS_ERROR;
            परत -1;
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
uset_serialize(स्थिरांक USet* set, uint16_t* dest, int32_t destCapacity, UErrorCode* ec) {
    जर (ec==nullptr || U_FAILURE(*ec)) {
        परत 0;
    }

    परत ((स्थिरांक UnicodeSet*) set)->UnicodeSet::serialize(dest, destCapacity,* ec);
}

U_CAPI UBool U_EXPORT2
uset_getSerializedSet(USerializedSet* fillSet, स्थिरांक uint16_t* src, int32_t srcLength) {
    int32_t length;

    जर(fillSet==nullptr) {
        परत असत्य;
    }
    जर(src==nullptr || srcLength<=0) {
        fillSet->length=fillSet->bmpLength=0;
        परत असत्य;
    }

    length=*src++;
    जर(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        जर(srcLength<(2+length)) {
            fillSet->length=fillSet->bmpLength=0;
            परत असत्य;
        }
        fillSet->bmpLength=*src++;
    } अन्यथा {
        /* only BMP values */
        जर(srcLength<(1+length)) {
            fillSet->length=fillSet->bmpLength=0;
            परत असत्य;
        }
        fillSet->bmpLength=length;
    }
    fillSet->array=src;
    fillSet->length=length;
    परत सत्य;
}

U_CAPI शून्य U_EXPORT2
uset_setSerializedToOne(USerializedSet* fillSet, UChar32 c) {
    जर(fillSet==nullptr || (uint32_t)c>0x10ffff) {
        परत;
    }

    fillSet->array=fillSet->staticArray;
    जर(c<0xffff) {
        fillSet->bmpLength=fillSet->length=2;
        fillSet->staticArray[0]=(uint16_t)c;
        fillSet->staticArray[1]=(uint16_t)c+1;
    } अन्यथा जर(c==0xffff) {
        fillSet->bmpLength=1;
        fillSet->length=3;
        fillSet->staticArray[0]=0xffff;
        fillSet->staticArray[1]=1;
        fillSet->staticArray[2]=0;
    } अन्यथा जर(c<0x10ffff) {
        fillSet->bmpLength=0;
        fillSet->length=4;
        fillSet->staticArray[0]=(uint16_t)(c>>16);
        fillSet->staticArray[1]=(uint16_t)c;
        ++c;
        fillSet->staticArray[2]=(uint16_t)(c>>16);
        fillSet->staticArray[3]=(uint16_t)c;
    } अन्यथा /* c==0x10ffff */ {
        fillSet->bmpLength=0;
        fillSet->length=2;
        fillSet->staticArray[0]=0x10;
        fillSet->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serializedContains(स्थिरांक USerializedSet* set, UChar32 c) {
    स्थिरांक uint16_t* array;

    जर(set==nullptr || (uint32_t)c>0x10ffff) {
        परत असत्य;
    }

    array=set->array;
    जर(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = set->bmpLength-1;
        जर (c < array[0]) {
            hi = 0;
        } अन्यथा जर (c < array[hi]) {
            साठी(;;) {
                int32_t i = (lo + hi) >> 1;
                जर (i == lo) {
                    थांबा;  // Done!
                } अन्यथा जर (c < array[i]) {
                    hi = i;
                } अन्यथा {
                    lo = i;
                }
            }
        } अन्यथा {
            hi += 1;
        }
        परत hi&1;
    } अन्यथा {
        /* find c in the supplementary part */
        uint16_t वरचा=(uint16_t)(c>>16), खालचा=(uint16_t)c;
        int32_t base = set->bmpLength;
        int32_t lo = 0;
        int32_t hi = set->length - 2 - base;
        जर (वरचा < array[base] || (वरचा==array[base] && खालचा<array[base+1])) {
            hi = 0;
        } अन्यथा जर (वरचा < array[base+hi] || (वरचा==array[base+hi] && खालचा<array[base+hi+1])) {
            साठी (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + base;
                जर (i == lo) {
                    थांबा;  // Done!
                } अन्यथा जर (वरचा < array[iabs] || (वरचा==array[iabs] && खालचा<array[iabs+1])) {
                    hi = i;
                } अन्यथा {
                    lo = i;
                }
            }
        } अन्यथा {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        परत ((hi+(base<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_मोजणी(स्थिरांक USerializedSet* set) {
    जर(set==nullptr) {
        परत 0;
    }

    परत (set->bmpLength+(set->length-set->bmpLength)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(स्थिरांक USerializedSet* set, int32_t rangeIndex,
                        UChar32* pStart, UChar32* pEnd) {
    स्थिरांक uint16_t* array;
    int32_t bmpLength, length;

    जर(set==nullptr || rangeIndex<0 || pStart==nullptr || pEnd==nullptr) {
        परत असत्य;
    }

    array=set->array;
    length=set->length;
    bmpLength=set->bmpLength;

    rangeIndex*=2; /* address start/limit pairs */
    जर(rangeIndex<bmpLength) {
        *pStart=array[rangeIndex++];
        जर(rangeIndex<bmpLength) {
            *pEnd=array[rangeIndex]-1;
        } अन्यथा जर(rangeIndex<length) {
            *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
        } अन्यथा {
            *pEnd=0x10ffff;
        }
        परत सत्य;
    } अन्यथा {
        rangeIndex-=bmpLength;
        rangeIndex*=2; /* address pairs of pairs of units */
        length-=bmpLength;
        जर(rangeIndex<length) {
            array+=bmpLength;
            *pStart=(((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1];
            rangeIndex+=2;
            जर(rangeIndex<length) {
                *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
            } अन्यथा {
                *pEnd=0x10ffff;
            }
            परत सत्य;
        } अन्यथा {
            परत असत्य;
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
