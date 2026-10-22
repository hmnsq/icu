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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/char16ptr.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "unicode/uset.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openEmpty() {
    திரும்பு (USet*) புதிய UnicodeSet();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 start, UChar32 end) {
    திரும்பு (USet*) புதிய UnicodeSet(start, end);
}

U_CAPI வெறுமை U_EXPORT2
uset_close(USet* set) {
    delete (UnicodeSet*) set;
}

U_CAPI USet * U_EXPORT2
uset_clone(மாறிலி USet *set) {
    திரும்பு (USet*) (((UnicodeSet*) set)->UnicodeSet::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(மாறிலி USet *set) {
    திரும்பு ((UnicodeSet*) set)->UnicodeSet::isFrozen();
}

U_CAPI வெறுமை U_EXPORT2
uset_freeze(USet *set) {
    ((UnicodeSet*) set)->UnicodeSet::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(மாறிலி USet *set) {
    திரும்பு (USet*) (((UnicodeSet*) set)->UnicodeSet::cloneAsThawed());
}

U_CAPI வெறுமை U_EXPORT2
uset_set(USet* set,
     UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::set(start, end);
}

U_CAPI வெறுமை U_EXPORT2
uset_addAll(USet* set, மாறிலி USet *additionalSet) {
    ((UnicodeSet*) set)->UnicodeSet::addAll(*((மாறிலி UnicodeSet*)additionalSet));
}

U_CAPI வெறுமை U_EXPORT2
uset_add(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::add(c);
}

U_CAPI வெறுமை U_EXPORT2
uset_addRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::add(start, end);    
}

U_CAPI வெறுமை U_EXPORT2
uset_addString(USet* set, மாறிலி char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::add(s);
}

U_CAPI வெறுமை U_EXPORT2
uset_addAllCodePoints(USet* set, மாறிலி char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::addAll(s);
}

U_CAPI வெறுமை U_EXPORT2
uset_remove(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::remove(c);
}

U_CAPI வெறுமை U_EXPORT2
uset_removeRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::remove(start, end);
}

U_CAPI வெறுமை U_EXPORT2
uset_removeString(USet* set, மாறிலி char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::remove(s);
}

U_CAPI வெறுமை U_EXPORT2
uset_removeAllCodePoints(USet *set, மாறிலி char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::removeAll(s);
}

U_CAPI வெறுமை U_EXPORT2
uset_removeAll(USet* set, மாறிலி USet* remove) {
    ((UnicodeSet*) set)->UnicodeSet::removeAll(*(மாறிலி UnicodeSet*)remove);
}

U_CAPI வெறுமை U_EXPORT2
uset_retain(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::retain(start, end);
}

U_CAPI வெறுமை U_EXPORT2
uset_retainString(USet *set, மாறிலி char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retain(s);
}

U_CAPI வெறுமை U_EXPORT2
uset_retainAllCodePoints(USet *set, மாறிலி char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retainAll(s);
}

U_CAPI வெறுமை U_EXPORT2
uset_retainAll(USet* set, மாறிலி USet* retain) {
    ((UnicodeSet*) set)->UnicodeSet::retainAll(*(மாறிலி UnicodeSet*)retain);
}

U_CAPI வெறுமை U_EXPORT2
uset_compact(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::compact();
}

U_CAPI வெறுமை U_EXPORT2
uset_complement(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::complement();
}

U_CAPI வெறுமை U_EXPORT2
uset_complementRange(USet *set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::complement(start, end);
}

U_CAPI வெறுமை U_EXPORT2
uset_complementString(USet *set, மாறிலி char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complement(s);
}

U_CAPI வெறுமை U_EXPORT2
uset_complementAllCodePoints(USet *set, மாறிலி char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complementAll(s);
}

U_CAPI வெறுமை U_EXPORT2
uset_complementAll(USet* set, மாறிலி USet* complement) {
    ((UnicodeSet*) set)->UnicodeSet::complementAll(*(மாறிலி UnicodeSet*)complement);
}

U_CAPI வெறுமை U_EXPORT2
uset_clear(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::clear();
}

U_CAPI வெறுமை U_EXPORT2
uset_removeAllStrings(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::removeAllStrings();
}

U_CAPI UBool U_EXPORT2
uset_isEmpty(மாறிலி USet* set) {
    திரும்பு ((மாறிலி UnicodeSet*) set)->UnicodeSet::isEmpty();
}

U_CAPI UBool U_EXPORT2
uset_hasStrings(மாறிலி USet* set) {
    திரும்பு ((மாறிலி UnicodeSet*) set)->UnicodeSet::hasStrings();
}

U_CAPI UBool U_EXPORT2
uset_contains(மாறிலி USet* set, UChar32 c) {
    திரும்பு ((மாறிலி UnicodeSet*) set)->UnicodeSet::contains(c);
}

U_CAPI UBool U_EXPORT2
uset_containsRange(மாறிலி USet* set, UChar32 start, UChar32 end) {
    திரும்பு ((மாறிலி UnicodeSet*) set)->UnicodeSet::contains(start, end);
}

U_CAPI UBool U_EXPORT2
uset_containsString(மாறிலி USet* set, மாறிலி char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    திரும்பு ((மாறிலி UnicodeSet*) set)->UnicodeSet::contains(s);
}

U_CAPI UBool U_EXPORT2
uset_containsAll(மாறிலி USet* set1, மாறிலி USet* set2) {
    திரும்பு ((மாறிலி UnicodeSet*) set1)->UnicodeSet::containsAll(* (மாறிலி UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsAllCodePoints(மாறிலி USet* set, மாறிலி char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    திரும்பு ((மாறிலி UnicodeSet*) set)->UnicodeSet::containsAll(s);
}

U_CAPI UBool U_EXPORT2
uset_containsNone(மாறிலி USet* set1, மாறிலி USet* set2) {
    திரும்பு ((மாறிலி UnicodeSet*) set1)->UnicodeSet::containsNone(* (மாறிலி UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsSome(மாறிலி USet* set1, மாறிலி USet* set2) {
    திரும்பு ((மாறிலி UnicodeSet*) set1)->UnicodeSet::containsSome(* (மாறிலி UnicodeSet*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_span(மாறிலி USet *set, மாறிலி char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    திரும்பு ((UnicodeSet*) set)->UnicodeSet::span(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBack(மாறிலி USet *set, மாறிலி char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    திரும்பு ((UnicodeSet*) set)->UnicodeSet::spanBack(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanUTF8(மாறிலி USet *set, மாறிலி எழுத்து *s, int32_t length, USetSpanCondition spanCondition) {
    திரும்பு ((UnicodeSet*) set)->UnicodeSet::spanUTF8(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBackUTF8(மாறிலி USet *set, மாறிலி எழுத்து *s, int32_t length, USetSpanCondition spanCondition) {
    திரும்பு ((UnicodeSet*) set)->UnicodeSet::spanBackUTF8(s, length, spanCondition);
}

U_CAPI UBool U_EXPORT2
uset_equals(மாறிலி USet* set1, மாறிலி USet* set2) {
    திரும்பு *(மாறிலி UnicodeSet*)set1 == *(மாறிலி UnicodeSet*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_indexOf(மாறிலி USet* set, UChar32 c) {
    திரும்பு ((UnicodeSet*) set)->UnicodeSet::indexOf(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(மாறிலி USet* set, int32_t index) {
    திரும்பு ((UnicodeSet*) set)->UnicodeSet::charAt(index);
}

U_CAPI int32_t U_EXPORT2
uset_size(மாறிலி USet* set) {
    திரும்பு ((மாறிலி UnicodeSet*) set)->UnicodeSet::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
வகுப்பு USetAccess /* not : public UObject because all methods are static */ {
பொது:
    /* Try to have the compiler inline these*/
    உள்வரி நிலையான int32_t get_String_மொத்தம்(மாறிலி UnicodeSet& set) {
        திரும்பு set.stringsSize();
    }
    உள்வரி நிலையான மாறிலி UnicodeString* getString(மாறிலி UnicodeSet& set U_LIFETIME_BOUND,
                                                 int32_t i) {
        திரும்பு set.getString(i);
    }
தனிப்பட்ட:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_END

U_CAPI int32_t U_EXPORT2
uset_get_Range_மொத்தம்(மாறிலி USet *set) {
    திரும்பு ((மாறிலி UnicodeSet *)set)->UnicodeSet::get_Range_மொத்தம்();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_மொத்தம்(மாறிலி USet *uset) {
    மாறிலி UnicodeSet &set = *(மாறிலி UnicodeSet *)uset;
    திரும்பு USetAccess::get_String_மொத்தம்(set);
}

U_CAPI int32_t U_EXPORT2
uset_get_Item_மொத்தம்(மாறிலி USet* uset) {
    மாறிலி UnicodeSet& set = *(மாறிலி UnicodeSet*)uset;
    திரும்பு set.get_Range_மொத்தம்() + USetAccess::get_String_மொத்தம்(set);
}

U_CAPI மாறிலி UChar* U_EXPORT2
uset_getString(மாறிலி USet* uset U_LIFETIME_BOUND, int32_t index, int32_t* pLength) {
    எனில் (pLength == nullptr) { திரும்பு nullptr; }
    மாறிலி UnicodeSet &set = *(மாறிலி UnicodeSet *)uset;
    int32_t count = USetAccess::get_String_மொத்தம்(set);
    எனில் (index < 0 || count <= index) {
        *pLength = 0;
        திரும்பு nullptr;
    }
    மாறிலி UnicodeString *s = USetAccess::getString(set, index);
    *pLength = s->length();
    திரும்பு toUCharPtr(s->getBuffer());
}

U_CAPI int32_t U_EXPORT2
uset_getItem(மாறிலி USet* uset, int32_t itemIndex,
             UChar32* start, UChar32* end,
             char16_t* str, int32_t strCapacity,
             UErrorCode* ec) {
    எனில் (U_FAILURE(*ec)) திரும்பு 0;
    மாறிலி UnicodeSet& set = *(மாறிலி UnicodeSet*)uset;
    int32_t range_மொத்தம்;

    எனில் (itemIndex < 0) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு -1;
    } இல்லையெனில் எனில் (itemIndex < (range_மொத்தம் = set.get_Range_மொத்தம்())) {
        *start = set.getRangeStart(itemIndex);
        *end = set.getRangeEnd(itemIndex);
        திரும்பு 0;
    } இல்லையெனில் {
        itemIndex -= range_மொத்தம்;
        எனில் (itemIndex < USetAccess::get_String_மொத்தம்(set)) {
            மாறிலி UnicodeString* s = USetAccess::getString(set, itemIndex);
            திரும்பு s->extract(str, strCapacity, *ec);
        } இல்லையெனில் {
            *ec = U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு -1;
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
uset_serialize(மாறிலி USet* set, uint16_t* dest, int32_t destCapacity, UErrorCode* ec) {
    எனில் (ec==nullptr || U_FAILURE(*ec)) {
        திரும்பு 0;
    }

    திரும்பு ((மாறிலி UnicodeSet*) set)->UnicodeSet::serialize(dest, destCapacity,* ec);
}

U_CAPI UBool U_EXPORT2
uset_getSerializedSet(USerializedSet* fillSet, மாறிலி uint16_t* src, int32_t srcLength) {
    int32_t length;

    எனில்(fillSet==nullptr) {
        திரும்பு பொய்;
    }
    எனில்(src==nullptr || srcLength<=0) {
        fillSet->length=fillSet->bmpLength=0;
        திரும்பு பொய்;
    }

    length=*src++;
    எனில்(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        எனில்(srcLength<(2+length)) {
            fillSet->length=fillSet->bmpLength=0;
            திரும்பு பொய்;
        }
        fillSet->bmpLength=*src++;
    } இல்லையெனில் {
        /* only BMP values */
        எனில்(srcLength<(1+length)) {
            fillSet->length=fillSet->bmpLength=0;
            திரும்பு பொய்;
        }
        fillSet->bmpLength=length;
    }
    fillSet->array=src;
    fillSet->length=length;
    திரும்பு உண்மை;
}

U_CAPI வெறுமை U_EXPORT2
uset_setSerializedToOne(USerializedSet* fillSet, UChar32 c) {
    எனில்(fillSet==nullptr || (uint32_t)c>0x10ffff) {
        திரும்பு;
    }

    fillSet->array=fillSet->staticArray;
    எனில்(c<0xffff) {
        fillSet->bmpLength=fillSet->length=2;
        fillSet->staticArray[0]=(uint16_t)c;
        fillSet->staticArray[1]=(uint16_t)c+1;
    } இல்லையெனில் எனில்(c==0xffff) {
        fillSet->bmpLength=1;
        fillSet->length=3;
        fillSet->staticArray[0]=0xffff;
        fillSet->staticArray[1]=1;
        fillSet->staticArray[2]=0;
    } இல்லையெனில் எனில்(c<0x10ffff) {
        fillSet->bmpLength=0;
        fillSet->length=4;
        fillSet->staticArray[0]=(uint16_t)(c>>16);
        fillSet->staticArray[1]=(uint16_t)c;
        ++c;
        fillSet->staticArray[2]=(uint16_t)(c>>16);
        fillSet->staticArray[3]=(uint16_t)c;
    } இல்லையெனில் /* c==0x10ffff */ {
        fillSet->bmpLength=0;
        fillSet->length=2;
        fillSet->staticArray[0]=0x10;
        fillSet->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serializedContains(மாறிலி USerializedSet* set, UChar32 c) {
    மாறிலி uint16_t* array;

    எனில்(set==nullptr || (uint32_t)c>0x10ffff) {
        திரும்பு பொய்;
    }

    array=set->array;
    எனில்(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = set->bmpLength-1;
        எனில் (c < array[0]) {
            hi = 0;
        } இல்லையெனில் எனில் (c < array[hi]) {
            க்கு(;;) {
                int32_t i = (lo + hi) >> 1;
                எனில் (i == lo) {
                    நிறுத்து;  // Done!
                } இல்லையெனில் எனில் (c < array[i]) {
                    hi = i;
                } இல்லையெனில் {
                    lo = i;
                }
            }
        } இல்லையெனில் {
            hi += 1;
        }
        திரும்பு hi&1;
    } இல்லையெனில் {
        /* find c in the supplementary part */
        uint16_t உயர்வு=(uint16_t)(c>>16), கீழ்=(uint16_t)c;
        int32_t base = set->bmpLength;
        int32_t lo = 0;
        int32_t hi = set->length - 2 - base;
        எனில் (உயர்வு < array[base] || (உயர்வு==array[base] && கீழ்<array[base+1])) {
            hi = 0;
        } இல்லையெனில் எனில் (உயர்வு < array[base+hi] || (உயர்வு==array[base+hi] && கீழ்<array[base+hi+1])) {
            க்கு (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + base;
                எனில் (i == lo) {
                    நிறுத்து;  // Done!
                } இல்லையெனில் எனில் (உயர்வு < array[iabs] || (உயர்வு==array[iabs] && கீழ்<array[iabs+1])) {
                    hi = i;
                } இல்லையெனில் {
                    lo = i;
                }
            }
        } இல்லையெனில் {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        திரும்பு ((hi+(base<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_மொத்தம்(மாறிலி USerializedSet* set) {
    எனில்(set==nullptr) {
        திரும்பு 0;
    }

    திரும்பு (set->bmpLength+(set->length-set->bmpLength)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(மாறிலி USerializedSet* set, int32_t rangeIndex,
                        UChar32* pStart, UChar32* pEnd) {
    மாறிலி uint16_t* array;
    int32_t bmpLength, length;

    எனில்(set==nullptr || rangeIndex<0 || pStart==nullptr || pEnd==nullptr) {
        திரும்பு பொய்;
    }

    array=set->array;
    length=set->length;
    bmpLength=set->bmpLength;

    rangeIndex*=2; /* address start/limit pairs */
    எனில்(rangeIndex<bmpLength) {
        *pStart=array[rangeIndex++];
        எனில்(rangeIndex<bmpLength) {
            *pEnd=array[rangeIndex]-1;
        } இல்லையெனில் எனில்(rangeIndex<length) {
            *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
        } இல்லையெனில் {
            *pEnd=0x10ffff;
        }
        திரும்பு உண்மை;
    } இல்லையெனில் {
        rangeIndex-=bmpLength;
        rangeIndex*=2; /* address pairs of pairs of units */
        length-=bmpLength;
        எனில்(rangeIndex<length) {
            array+=bmpLength;
            *pStart=(((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1];
            rangeIndex+=2;
            எனில்(rangeIndex<length) {
                *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
            } இல்லையெனில் {
                *pEnd=0x10ffff;
            }
            திரும்பு உண்மை;
        } இல்லையெனில் {
            திரும்பு பொய்;
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
