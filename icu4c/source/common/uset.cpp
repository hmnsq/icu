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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/char16ptr.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/uset.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openEmpty() {
    প্রত্যাগম (USet*) নূতন UnicodeSet();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 start, UChar32 end) {
    প্রত্যাগম (USet*) নূতন UnicodeSet(start, end);
}

U_CAPI শূন্য U_EXPORT2
uset_close(USet* set) {
    delete (UnicodeSet*) set;
}

U_CAPI USet * U_EXPORT2
uset_clone(ধ্রুব USet *set) {
    প্রত্যাগম (USet*) (((UnicodeSet*) set)->UnicodeSet::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ধ্রুব USet *set) {
    প্রত্যাগম ((UnicodeSet*) set)->UnicodeSet::isFrozen();
}

U_CAPI শূন্য U_EXPORT2
uset_freeze(USet *set) {
    ((UnicodeSet*) set)->UnicodeSet::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ধ্রুব USet *set) {
    প্রত্যাগম (USet*) (((UnicodeSet*) set)->UnicodeSet::cloneAsThawed());
}

U_CAPI শূন্য U_EXPORT2
uset_set(USet* set,
     UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::set(start, end);
}

U_CAPI শূন্য U_EXPORT2
uset_addAll(USet* set, ধ্রুব USet *additionalSet) {
    ((UnicodeSet*) set)->UnicodeSet::addAll(*((ধ্রুব UnicodeSet*)additionalSet));
}

U_CAPI শূন্য U_EXPORT2
uset_add(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::add(c);
}

U_CAPI শূন্য U_EXPORT2
uset_addRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::add(start, end);    
}

U_CAPI শূন্য U_EXPORT2
uset_addString(USet* set, ধ্রুব char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::add(s);
}

U_CAPI শূন্য U_EXPORT2
uset_addAllCodePoints(USet* set, ধ্রুব char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::addAll(s);
}

U_CAPI শূন্য U_EXPORT2
uset_remove(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::remove(c);
}

U_CAPI শূন্য U_EXPORT2
uset_removeRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::remove(start, end);
}

U_CAPI শূন্য U_EXPORT2
uset_removeString(USet* set, ধ্রুব char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::remove(s);
}

U_CAPI শূন্য U_EXPORT2
uset_removeAllCodePoints(USet *set, ধ্রুব char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::removeAll(s);
}

U_CAPI শূন্য U_EXPORT2
uset_removeAll(USet* set, ধ্রুব USet* remove) {
    ((UnicodeSet*) set)->UnicodeSet::removeAll(*(ধ্রুব UnicodeSet*)remove);
}

U_CAPI শূন্য U_EXPORT2
uset_retain(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::retain(start, end);
}

U_CAPI শূন্য U_EXPORT2
uset_retainString(USet *set, ধ্রুব char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retain(s);
}

U_CAPI শূন্য U_EXPORT2
uset_retainAllCodePoints(USet *set, ধ্রুব char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retainAll(s);
}

U_CAPI শূন্য U_EXPORT2
uset_retainAll(USet* set, ধ্রুব USet* retain) {
    ((UnicodeSet*) set)->UnicodeSet::retainAll(*(ধ্রুব UnicodeSet*)retain);
}

U_CAPI শূন্য U_EXPORT2
uset_compact(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::compact();
}

U_CAPI শূন্য U_EXPORT2
uset_complement(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::complement();
}

U_CAPI শূন্য U_EXPORT2
uset_complementRange(USet *set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::complement(start, end);
}

U_CAPI শূন্য U_EXPORT2
uset_complementString(USet *set, ধ্রুব char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complement(s);
}

U_CAPI শূন্য U_EXPORT2
uset_complementAllCodePoints(USet *set, ধ্রুব char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complementAll(s);
}

U_CAPI শূন্য U_EXPORT2
uset_complementAll(USet* set, ধ্রুব USet* complement) {
    ((UnicodeSet*) set)->UnicodeSet::complementAll(*(ধ্রুব UnicodeSet*)complement);
}

U_CAPI শূন্য U_EXPORT2
uset_clear(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::clear();
}

U_CAPI শূন্য U_EXPORT2
uset_removeAllStrings(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::removeAllStrings();
}

U_CAPI UBool U_EXPORT2
uset_isEmpty(ধ্রুব USet* set) {
    প্রত্যাগম ((ধ্রুব UnicodeSet*) set)->UnicodeSet::isEmpty();
}

U_CAPI UBool U_EXPORT2
uset_hasStrings(ধ্রুব USet* set) {
    প্রত্যাগম ((ধ্রুব UnicodeSet*) set)->UnicodeSet::hasStrings();
}

U_CAPI UBool U_EXPORT2
uset_contains(ধ্রুব USet* set, UChar32 c) {
    প্রত্যাগম ((ধ্রুব UnicodeSet*) set)->UnicodeSet::contains(c);
}

U_CAPI UBool U_EXPORT2
uset_containsRange(ধ্রুব USet* set, UChar32 start, UChar32 end) {
    প্রত্যাগম ((ধ্রুব UnicodeSet*) set)->UnicodeSet::contains(start, end);
}

U_CAPI UBool U_EXPORT2
uset_containsString(ধ্রুব USet* set, ধ্রুব char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    প্রত্যাগম ((ধ্রুব UnicodeSet*) set)->UnicodeSet::contains(s);
}

U_CAPI UBool U_EXPORT2
uset_containsAll(ধ্রুব USet* set1, ধ্রুব USet* set2) {
    প্রত্যাগম ((ধ্রুব UnicodeSet*) set1)->UnicodeSet::containsAll(* (ধ্রুব UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsAllCodePoints(ধ্রুব USet* set, ধ্রুব char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    প্রত্যাগম ((ধ্রুব UnicodeSet*) set)->UnicodeSet::containsAll(s);
}

U_CAPI UBool U_EXPORT2
uset_containsNone(ধ্রুব USet* set1, ধ্রুব USet* set2) {
    প্রত্যাগম ((ধ্রুব UnicodeSet*) set1)->UnicodeSet::containsNone(* (ধ্রুব UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsSome(ধ্রুব USet* set1, ধ্রুব USet* set2) {
    প্রত্যাগম ((ধ্রুব UnicodeSet*) set1)->UnicodeSet::containsSome(* (ধ্রুব UnicodeSet*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_span(ধ্রুব USet *set, ধ্রুব char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    প্রত্যাগম ((UnicodeSet*) set)->UnicodeSet::span(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBack(ধ্রুব USet *set, ধ্রুব char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    প্রত্যাগম ((UnicodeSet*) set)->UnicodeSet::spanBack(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanUTF8(ধ্রুব USet *set, ধ্রুব বর্ণ *s, int32_t length, USetSpanCondition spanCondition) {
    প্রত্যাগম ((UnicodeSet*) set)->UnicodeSet::spanUTF8(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBackUTF8(ধ্রুব USet *set, ধ্রুব বর্ণ *s, int32_t length, USetSpanCondition spanCondition) {
    প্রত্যাগম ((UnicodeSet*) set)->UnicodeSet::spanBackUTF8(s, length, spanCondition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ধ্রুব USet* set1, ধ্রুব USet* set2) {
    প্রত্যাগম *(ধ্রুব UnicodeSet*)set1 == *(ধ্রুব UnicodeSet*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_indexOf(ধ্রুব USet* set, UChar32 c) {
    প্রত্যাগম ((UnicodeSet*) set)->UnicodeSet::indexOf(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ধ্রুব USet* set, int32_t index) {
    প্রত্যাগম ((UnicodeSet*) set)->UnicodeSet::charAt(index);
}

U_CAPI int32_t U_EXPORT2
uset_size(ধ্রুব USet* set) {
    প্রত্যাগম ((ধ্রুব UnicodeSet*) set)->UnicodeSet::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
বর্গ USetAccess /* not : public UObject because all methods are static */ {
সার্বজনিক:
    /* Try to have the compiler inline these*/
    অন্তঃস্থ স্থির int32_t get_String_গুনতি(ধ্রুব UnicodeSet& set) {
        প্রত্যাগম set.stringsSize();
    }
    অন্তঃস্থ স্থির ধ্রুব UnicodeString* getString(ধ্রুব UnicodeSet& set U_LIFETIME_BOUND,
                                                 int32_t i) {
        প্রত্যাগম set.getString(i);
    }
গুপ্ত:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_END

U_CAPI int32_t U_EXPORT2
uset_get_Range_গুনতি(ধ্রুব USet *set) {
    প্রত্যাগম ((ধ্রুব UnicodeSet *)set)->UnicodeSet::get_Range_গুনতি();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_গুনতি(ধ্রুব USet *uset) {
    ধ্রুব UnicodeSet &set = *(ধ্রুব UnicodeSet *)uset;
    প্রত্যাগম USetAccess::get_String_গুনতি(set);
}

U_CAPI int32_t U_EXPORT2
uset_get_Item_গুনতি(ধ্রুব USet* uset) {
    ধ্রুব UnicodeSet& set = *(ধ্রুব UnicodeSet*)uset;
    প্রত্যাগম set.get_Range_গুনতি() + USetAccess::get_String_গুনতি(set);
}

U_CAPI ধ্রুব UChar* U_EXPORT2
uset_getString(ধ্রুব USet* uset U_LIFETIME_BOUND, int32_t index, int32_t* pLength) {
    যদি (pLength == nullptr) { প্রত্যাগম nullptr; }
    ধ্রুব UnicodeSet &set = *(ধ্রুব UnicodeSet *)uset;
    int32_t count = USetAccess::get_String_গুনতি(set);
    যদি (index < 0 || count <= index) {
        *pLength = 0;
        প্রত্যাগম nullptr;
    }
    ধ্রুব UnicodeString *s = USetAccess::getString(set, index);
    *pLength = s->length();
    প্রত্যাগম toUCharPtr(s->getBuffer());
}

U_CAPI int32_t U_EXPORT2
uset_getItem(ধ্রুব USet* uset, int32_t itemIndex,
             UChar32* start, UChar32* end,
             char16_t* str, int32_t strCapacity,
             UErrorCode* ec) {
    যদি (U_FAILURE(*ec)) প্রত্যাগম 0;
    ধ্রুব UnicodeSet& set = *(ধ্রুব UnicodeSet*)uset;
    int32_t range_গুনতি;

    যদি (itemIndex < 0) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম -1;
    } অন্যথা যদি (itemIndex < (range_গুনতি = set.get_Range_গুনতি())) {
        *start = set.getRangeStart(itemIndex);
        *end = set.getRangeEnd(itemIndex);
        প্রত্যাগম 0;
    } অন্যথা {
        itemIndex -= range_গুনতি;
        যদি (itemIndex < USetAccess::get_String_গুনতি(set)) {
            ধ্রুব UnicodeString* s = USetAccess::getString(set, itemIndex);
            প্রত্যাগম s->extract(str, strCapacity, *ec);
        } অন্যথা {
            *ec = U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম -1;
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
uset_serialize(ধ্রুব USet* set, uint16_t* dest, int32_t destCapacity, UErrorCode* ec) {
    যদি (ec==nullptr || U_FAILURE(*ec)) {
        প্রত্যাগম 0;
    }

    প্রত্যাগম ((ধ্রুব UnicodeSet*) set)->UnicodeSet::serialize(dest, destCapacity,* ec);
}

U_CAPI UBool U_EXPORT2
uset_getSerializedSet(USerializedSet* fillSet, ধ্রুব uint16_t* src, int32_t srcLength) {
    int32_t length;

    যদি(fillSet==nullptr) {
        প্রত্যাগম অসত্য;
    }
    যদি(src==nullptr || srcLength<=0) {
        fillSet->length=fillSet->bmpLength=0;
        প্রত্যাগম অসত্য;
    }

    length=*src++;
    যদি(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        যদি(srcLength<(2+length)) {
            fillSet->length=fillSet->bmpLength=0;
            প্রত্যাগম অসত্য;
        }
        fillSet->bmpLength=*src++;
    } অন্যথা {
        /* only BMP values */
        যদি(srcLength<(1+length)) {
            fillSet->length=fillSet->bmpLength=0;
            প্রত্যাগম অসত্য;
        }
        fillSet->bmpLength=length;
    }
    fillSet->array=src;
    fillSet->length=length;
    প্রত্যাগম সত্য;
}

U_CAPI শূন্য U_EXPORT2
uset_setSerializedToOne(USerializedSet* fillSet, UChar32 c) {
    যদি(fillSet==nullptr || (uint32_t)c>0x10ffff) {
        প্রত্যাগম;
    }

    fillSet->array=fillSet->staticArray;
    যদি(c<0xffff) {
        fillSet->bmpLength=fillSet->length=2;
        fillSet->staticArray[0]=(uint16_t)c;
        fillSet->staticArray[1]=(uint16_t)c+1;
    } অন্যথা যদি(c==0xffff) {
        fillSet->bmpLength=1;
        fillSet->length=3;
        fillSet->staticArray[0]=0xffff;
        fillSet->staticArray[1]=1;
        fillSet->staticArray[2]=0;
    } অন্যথা যদি(c<0x10ffff) {
        fillSet->bmpLength=0;
        fillSet->length=4;
        fillSet->staticArray[0]=(uint16_t)(c>>16);
        fillSet->staticArray[1]=(uint16_t)c;
        ++c;
        fillSet->staticArray[2]=(uint16_t)(c>>16);
        fillSet->staticArray[3]=(uint16_t)c;
    } অন্যথা /* c==0x10ffff */ {
        fillSet->bmpLength=0;
        fillSet->length=2;
        fillSet->staticArray[0]=0x10;
        fillSet->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serializedContains(ধ্রুব USerializedSet* set, UChar32 c) {
    ধ্রুব uint16_t* array;

    যদি(set==nullptr || (uint32_t)c>0x10ffff) {
        প্রত্যাগম অসত্য;
    }

    array=set->array;
    যদি(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = set->bmpLength-1;
        যদি (c < array[0]) {
            hi = 0;
        } অন্যথা যদি (c < array[hi]) {
            প্রতি(;;) {
                int32_t i = (lo + hi) >> 1;
                যদি (i == lo) {
                    ভঙ্গ;  // Done!
                } অন্যথা যদি (c < array[i]) {
                    hi = i;
                } অন্যথা {
                    lo = i;
                }
            }
        } অন্যথা {
            hi += 1;
        }
        প্রত্যাগম hi&1;
    } অন্যথা {
        /* find c in the supplementary part */
        uint16_t উপরের=(uint16_t)(c>>16), নিচের=(uint16_t)c;
        int32_t base = set->bmpLength;
        int32_t lo = 0;
        int32_t hi = set->length - 2 - base;
        যদি (উপরের < array[base] || (উপরের==array[base] && নিচের<array[base+1])) {
            hi = 0;
        } অন্যথা যদি (উপরের < array[base+hi] || (উপরের==array[base+hi] && নিচের<array[base+hi+1])) {
            প্রতি (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + base;
                যদি (i == lo) {
                    ভঙ্গ;  // Done!
                } অন্যথা যদি (উপরের < array[iabs] || (উপরের==array[iabs] && নিচের<array[iabs+1])) {
                    hi = i;
                } অন্যথা {
                    lo = i;
                }
            }
        } অন্যথা {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        প্রত্যাগম ((hi+(base<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_গুনতি(ধ্রুব USerializedSet* set) {
    যদি(set==nullptr) {
        প্রত্যাগম 0;
    }

    প্রত্যাগম (set->bmpLength+(set->length-set->bmpLength)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ধ্রুব USerializedSet* set, int32_t rangeIndex,
                        UChar32* pStart, UChar32* pEnd) {
    ধ্রুব uint16_t* array;
    int32_t bmpLength, length;

    যদি(set==nullptr || rangeIndex<0 || pStart==nullptr || pEnd==nullptr) {
        প্রত্যাগম অসত্য;
    }

    array=set->array;
    length=set->length;
    bmpLength=set->bmpLength;

    rangeIndex*=2; /* address start/limit pairs */
    যদি(rangeIndex<bmpLength) {
        *pStart=array[rangeIndex++];
        যদি(rangeIndex<bmpLength) {
            *pEnd=array[rangeIndex]-1;
        } অন্যথা যদি(rangeIndex<length) {
            *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
        } অন্যথা {
            *pEnd=0x10ffff;
        }
        প্রত্যাগম সত্য;
    } অন্যথা {
        rangeIndex-=bmpLength;
        rangeIndex*=2; /* address pairs of pairs of units */
        length-=bmpLength;
        যদি(rangeIndex<length) {
            array+=bmpLength;
            *pStart=(((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1];
            rangeIndex+=2;
            যদি(rangeIndex<length) {
                *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
            } অন্যথা {
                *pEnd=0x10ffff;
            }
            প্রত্যাগম সত্য;
        } অন্যথা {
            প্রত্যাগম অসত্য;
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
