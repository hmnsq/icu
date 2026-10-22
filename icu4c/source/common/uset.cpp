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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/char16ptr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openEmpty() {
    ಪ್ರತ್ಯಾಗಮ (USet*) ನೂತನ UnicodeSet();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 start, UChar32 end) {
    ಪ್ರತ್ಯಾಗಮ (USet*) ನೂತನ UnicodeSet(start, end);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_close(USet* set) {
    delete (UnicodeSet*) set;
}

U_CAPI USet * U_EXPORT2
uset_clone(ಧ್ರುವ USet *set) {
    ಪ್ರತ್ಯಾಗಮ (USet*) (((UnicodeSet*) set)->UnicodeSet::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ಧ್ರುವ USet *set) {
    ಪ್ರತ್ಯಾಗಮ ((UnicodeSet*) set)->UnicodeSet::isFrozen();
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_freeze(USet *set) {
    ((UnicodeSet*) set)->UnicodeSet::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ಧ್ರುವ USet *set) {
    ಪ್ರತ್ಯಾಗಮ (USet*) (((UnicodeSet*) set)->UnicodeSet::cloneAsThawed());
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_set(USet* set,
     UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::set(start, end);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_addAll(USet* set, ಧ್ರುವ USet *additionalSet) {
    ((UnicodeSet*) set)->UnicodeSet::addAll(*((ಧ್ರುವ UnicodeSet*)additionalSet));
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_add(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::add(c);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_addRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::add(start, end);    
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_addString(USet* set, ಧ್ರುವ char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::add(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_addAllCodePoints(USet* set, ಧ್ರುವ char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::addAll(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_remove(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::remove(c);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_removeRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::remove(start, end);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_removeString(USet* set, ಧ್ರುವ char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::remove(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_removeAllCodePoints(USet *set, ಧ್ರುವ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::removeAll(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_removeAll(USet* set, ಧ್ರುವ USet* remove) {
    ((UnicodeSet*) set)->UnicodeSet::removeAll(*(ಧ್ರುವ UnicodeSet*)remove);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_retain(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::retain(start, end);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_retainString(USet *set, ಧ್ರುವ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retain(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_retainAllCodePoints(USet *set, ಧ್ರುವ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retainAll(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_retainAll(USet* set, ಧ್ರುವ USet* retain) {
    ((UnicodeSet*) set)->UnicodeSet::retainAll(*(ಧ್ರುವ UnicodeSet*)retain);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_compact(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::compact();
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_complement(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::complement();
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_complementRange(USet *set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::complement(start, end);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_complementString(USet *set, ಧ್ರುವ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complement(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_complementAllCodePoints(USet *set, ಧ್ರುವ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complementAll(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_complementAll(USet* set, ಧ್ರುವ USet* complement) {
    ((UnicodeSet*) set)->UnicodeSet::complementAll(*(ಧ್ರುವ UnicodeSet*)complement);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_clear(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::clear();
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_removeAllStrings(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::removeAllStrings();
}

U_CAPI UBool U_EXPORT2
uset_isEmpty(ಧ್ರುವ USet* set) {
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet*) set)->UnicodeSet::isEmpty();
}

U_CAPI UBool U_EXPORT2
uset_hasStrings(ಧ್ರುವ USet* set) {
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet*) set)->UnicodeSet::hasStrings();
}

U_CAPI UBool U_EXPORT2
uset_contains(ಧ್ರುವ USet* set, UChar32 c) {
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet*) set)->UnicodeSet::contains(c);
}

U_CAPI UBool U_EXPORT2
uset_containsRange(ಧ್ರುವ USet* set, UChar32 start, UChar32 end) {
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet*) set)->UnicodeSet::contains(start, end);
}

U_CAPI UBool U_EXPORT2
uset_containsString(ಧ್ರುವ USet* set, ಧ್ರುವ char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet*) set)->UnicodeSet::contains(s);
}

U_CAPI UBool U_EXPORT2
uset_containsAll(ಧ್ರುವ USet* set1, ಧ್ರುವ USet* set2) {
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet*) set1)->UnicodeSet::containsAll(* (ಧ್ರುವ UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsAllCodePoints(ಧ್ರುವ USet* set, ಧ್ರುವ char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet*) set)->UnicodeSet::containsAll(s);
}

U_CAPI UBool U_EXPORT2
uset_containsNone(ಧ್ರುವ USet* set1, ಧ್ರುವ USet* set2) {
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet*) set1)->UnicodeSet::containsNone(* (ಧ್ರುವ UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsSome(ಧ್ರುವ USet* set1, ಧ್ರುವ USet* set2) {
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet*) set1)->UnicodeSet::containsSome(* (ಧ್ರುವ UnicodeSet*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_span(ಧ್ರುವ USet *set, ಧ್ರುವ char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    ಪ್ರತ್ಯಾಗಮ ((UnicodeSet*) set)->UnicodeSet::span(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBack(ಧ್ರುವ USet *set, ಧ್ರುವ char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    ಪ್ರತ್ಯಾಗಮ ((UnicodeSet*) set)->UnicodeSet::spanBack(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanUTF8(ಧ್ರುವ USet *set, ಧ್ರುವ ವರ್ಣ *s, int32_t length, USetSpanCondition spanCondition) {
    ಪ್ರತ್ಯಾಗಮ ((UnicodeSet*) set)->UnicodeSet::spanUTF8(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBackUTF8(ಧ್ರುವ USet *set, ಧ್ರುವ ವರ್ಣ *s, int32_t length, USetSpanCondition spanCondition) {
    ಪ್ರತ್ಯಾಗಮ ((UnicodeSet*) set)->UnicodeSet::spanBackUTF8(s, length, spanCondition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ಧ್ರುವ USet* set1, ಧ್ರುವ USet* set2) {
    ಪ್ರತ್ಯಾಗಮ *(ಧ್ರುವ UnicodeSet*)set1 == *(ಧ್ರುವ UnicodeSet*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_indexOf(ಧ್ರುವ USet* set, UChar32 c) {
    ಪ್ರತ್ಯಾಗಮ ((UnicodeSet*) set)->UnicodeSet::indexOf(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ಧ್ರುವ USet* set, int32_t index) {
    ಪ್ರತ್ಯಾಗಮ ((UnicodeSet*) set)->UnicodeSet::charAt(index);
}

U_CAPI int32_t U_EXPORT2
uset_size(ಧ್ರುವ USet* set) {
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet*) set)->UnicodeSet::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
ವರ್ಗ USetAccess /* not : public UObject because all methods are static */ {
ಸಾರ್ವಜನಿಕ:
    /* Try to have the compiler inline these*/
    ಅನ್ತಃಸ್ಥ ಸ್ಥಿರ int32_t get_String_ಎಣಿಕೆ(ಧ್ರುವ UnicodeSet& set) {
        ಪ್ರತ್ಯಾಗಮ set.stringsSize();
    }
    ಅನ್ತಃಸ್ಥ ಸ್ಥಿರ ಧ್ರುವ UnicodeString* getString(ಧ್ರುವ UnicodeSet& set U_LIFETIME_BOUND,
                                                 int32_t i) {
        ಪ್ರತ್ಯಾಗಮ set.getString(i);
    }
ಗುಪ್ತ:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_END

U_CAPI int32_t U_EXPORT2
uset_get_Range_ಎಣಿಕೆ(ಧ್ರುವ USet *set) {
    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet *)set)->UnicodeSet::get_Range_ಎಣಿಕೆ();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_ಎಣಿಕೆ(ಧ್ರುವ USet *uset) {
    ಧ್ರುವ UnicodeSet &set = *(ಧ್ರುವ UnicodeSet *)uset;
    ಪ್ರತ್ಯಾಗಮ USetAccess::get_String_ಎಣಿಕೆ(set);
}

U_CAPI int32_t U_EXPORT2
uset_get_Item_ಎಣಿಕೆ(ಧ್ರುವ USet* uset) {
    ಧ್ರುವ UnicodeSet& set = *(ಧ್ರುವ UnicodeSet*)uset;
    ಪ್ರತ್ಯಾಗಮ set.get_Range_ಎಣಿಕೆ() + USetAccess::get_String_ಎಣಿಕೆ(set);
}

U_CAPI ಧ್ರುವ UChar* U_EXPORT2
uset_getString(ಧ್ರುವ USet* uset U_LIFETIME_BOUND, int32_t index, int32_t* pLength) {
    ಯದಿ (pLength == nullptr) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    ಧ್ರುವ UnicodeSet &set = *(ಧ್ರುವ UnicodeSet *)uset;
    int32_t count = USetAccess::get_String_ಎಣಿಕೆ(set);
    ಯದಿ (index < 0 || count <= index) {
        *pLength = 0;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಧ್ರುವ UnicodeString *s = USetAccess::getString(set, index);
    *pLength = s->length();
    ಪ್ರತ್ಯಾಗಮ toUCharPtr(s->getBuffer());
}

U_CAPI int32_t U_EXPORT2
uset_getItem(ಧ್ರುವ USet* uset, int32_t itemIndex,
             UChar32* start, UChar32* end,
             char16_t* str, int32_t strCapacity,
             UErrorCode* ec) {
    ಯದಿ (U_FAILURE(*ec)) ಪ್ರತ್ಯಾಗಮ 0;
    ಧ್ರುವ UnicodeSet& set = *(ಧ್ರುವ UnicodeSet*)uset;
    int32_t range_ಎಣಿಕೆ;

    ಯದಿ (itemIndex < 0) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ -1;
    } ಅನ್ಯಥಾ ಯದಿ (itemIndex < (range_ಎಣಿಕೆ = set.get_Range_ಎಣಿಕೆ())) {
        *start = set.getRangeStart(itemIndex);
        *end = set.getRangeEnd(itemIndex);
        ಪ್ರತ್ಯಾಗಮ 0;
    } ಅನ್ಯಥಾ {
        itemIndex -= range_ಎಣಿಕೆ;
        ಯದಿ (itemIndex < USetAccess::get_String_ಎಣಿಕೆ(set)) {
            ಧ್ರುವ UnicodeString* s = USetAccess::getString(set, itemIndex);
            ಪ್ರತ್ಯಾಗಮ s->extract(str, strCapacity, *ec);
        } ಅನ್ಯಥಾ {
            *ec = U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ -1;
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
uset_serialize(ಧ್ರುವ USet* set, uint16_t* dest, int32_t destCapacity, UErrorCode* ec) {
    ಯದಿ (ec==nullptr || U_FAILURE(*ec)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ UnicodeSet*) set)->UnicodeSet::serialize(dest, destCapacity,* ec);
}

U_CAPI UBool U_EXPORT2
uset_getSerializedSet(USerializedSet* fillSet, ಧ್ರುವ uint16_t* src, int32_t srcLength) {
    int32_t length;

    ಯದಿ(fillSet==nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ(src==nullptr || srcLength<=0) {
        fillSet->length=fillSet->bmpLength=0;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    length=*src++;
    ಯದಿ(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        ಯದಿ(srcLength<(2+length)) {
            fillSet->length=fillSet->bmpLength=0;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        fillSet->bmpLength=*src++;
    } ಅನ್ಯಥಾ {
        /* only BMP values */
        ಯದಿ(srcLength<(1+length)) {
            fillSet->length=fillSet->bmpLength=0;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        fillSet->bmpLength=length;
    }
    fillSet->array=src;
    fillSet->length=length;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_setSerializedToOne(USerializedSet* fillSet, UChar32 c) {
    ಯದಿ(fillSet==nullptr || (uint32_t)c>0x10ffff) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    fillSet->array=fillSet->staticArray;
    ಯದಿ(c<0xffff) {
        fillSet->bmpLength=fillSet->length=2;
        fillSet->staticArray[0]=(uint16_t)c;
        fillSet->staticArray[1]=(uint16_t)c+1;
    } ಅನ್ಯಥಾ ಯದಿ(c==0xffff) {
        fillSet->bmpLength=1;
        fillSet->length=3;
        fillSet->staticArray[0]=0xffff;
        fillSet->staticArray[1]=1;
        fillSet->staticArray[2]=0;
    } ಅನ್ಯಥಾ ಯದಿ(c<0x10ffff) {
        fillSet->bmpLength=0;
        fillSet->length=4;
        fillSet->staticArray[0]=(uint16_t)(c>>16);
        fillSet->staticArray[1]=(uint16_t)c;
        ++c;
        fillSet->staticArray[2]=(uint16_t)(c>>16);
        fillSet->staticArray[3]=(uint16_t)c;
    } ಅನ್ಯಥಾ /* c==0x10ffff */ {
        fillSet->bmpLength=0;
        fillSet->length=2;
        fillSet->staticArray[0]=0x10;
        fillSet->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serializedContains(ಧ್ರುವ USerializedSet* set, UChar32 c) {
    ಧ್ರುವ uint16_t* array;

    ಯದಿ(set==nullptr || (uint32_t)c>0x10ffff) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    array=set->array;
    ಯದಿ(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = set->bmpLength-1;
        ಯದಿ (c < array[0]) {
            hi = 0;
        } ಅನ್ಯಥಾ ಯದಿ (c < array[hi]) {
            ಪ್ರತಿ(;;) {
                int32_t i = (lo + hi) >> 1;
                ಯದಿ (i == lo) {
                    ಭಙ್ಗ;  // Done!
                } ಅನ್ಯಥಾ ಯದಿ (c < array[i]) {
                    hi = i;
                } ಅನ್ಯಥಾ {
                    lo = i;
                }
            }
        } ಅನ್ಯಥಾ {
            hi += 1;
        }
        ಪ್ರತ್ಯಾಗಮ hi&1;
    } ಅನ್ಯಥಾ {
        /* find c in the supplementary part */
        uint16_t ಮೇಲಿನ=(uint16_t)(c>>16), ಕೆಳಗಿನ=(uint16_t)c;
        int32_t base = set->bmpLength;
        int32_t lo = 0;
        int32_t hi = set->length - 2 - base;
        ಯದಿ (ಮೇಲಿನ < array[base] || (ಮೇಲಿನ==array[base] && ಕೆಳಗಿನ<array[base+1])) {
            hi = 0;
        } ಅನ್ಯಥಾ ಯದಿ (ಮೇಲಿನ < array[base+hi] || (ಮೇಲಿನ==array[base+hi] && ಕೆಳಗಿನ<array[base+hi+1])) {
            ಪ್ರತಿ (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + base;
                ಯದಿ (i == lo) {
                    ಭಙ್ಗ;  // Done!
                } ಅನ್ಯಥಾ ಯದಿ (ಮೇಲಿನ < array[iabs] || (ಮೇಲಿನ==array[iabs] && ಕೆಳಗಿನ<array[iabs+1])) {
                    hi = i;
                } ಅನ್ಯಥಾ {
                    lo = i;
                }
            }
        } ಅನ್ಯಥಾ {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        ಪ್ರತ್ಯಾಗಮ ((hi+(base<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_ಎಣಿಕೆ(ಧ್ರುವ USerializedSet* set) {
    ಯದಿ(set==nullptr) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪ್ರತ್ಯಾಗಮ (set->bmpLength+(set->length-set->bmpLength)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ಧ್ರುವ USerializedSet* set, int32_t rangeIndex,
                        UChar32* pStart, UChar32* pEnd) {
    ಧ್ರುವ uint16_t* array;
    int32_t bmpLength, length;

    ಯದಿ(set==nullptr || rangeIndex<0 || pStart==nullptr || pEnd==nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    array=set->array;
    length=set->length;
    bmpLength=set->bmpLength;

    rangeIndex*=2; /* address start/limit pairs */
    ಯದಿ(rangeIndex<bmpLength) {
        *pStart=array[rangeIndex++];
        ಯದಿ(rangeIndex<bmpLength) {
            *pEnd=array[rangeIndex]-1;
        } ಅನ್ಯಥಾ ಯದಿ(rangeIndex<length) {
            *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
        } ಅನ್ಯಥಾ {
            *pEnd=0x10ffff;
        }
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    } ಅನ್ಯಥಾ {
        rangeIndex-=bmpLength;
        rangeIndex*=2; /* address pairs of pairs of units */
        length-=bmpLength;
        ಯದಿ(rangeIndex<length) {
            array+=bmpLength;
            *pStart=(((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1];
            rangeIndex+=2;
            ಯದಿ(rangeIndex<length) {
                *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
            } ಅನ್ಯಥಾ {
                *pEnd=0x10ffff;
            }
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
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
