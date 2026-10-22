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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/char16ptr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openEmpty() {
    ଫେରନ୍ତୁ (USet*) ନୂତନ UnicodeSet();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 start, UChar32 end) {
    ଫେରନ୍ତୁ (USet*) ନୂତନ UnicodeSet(start, end);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_close(USet* set) {
    delete (UnicodeSet*) set;
}

U_CAPI USet * U_EXPORT2
uset_clone(ଅଚଳ USet *set) {
    ଫେରନ୍ତୁ (USet*) (((UnicodeSet*) set)->UnicodeSet::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ଅଚଳ USet *set) {
    ଫେରନ୍ତୁ ((UnicodeSet*) set)->UnicodeSet::isFrozen();
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_freeze(USet *set) {
    ((UnicodeSet*) set)->UnicodeSet::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ଅଚଳ USet *set) {
    ଫେରନ୍ତୁ (USet*) (((UnicodeSet*) set)->UnicodeSet::cloneAsThawed());
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_set(USet* set,
     UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::set(start, end);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_addAll(USet* set, ଅଚଳ USet *additionalSet) {
    ((UnicodeSet*) set)->UnicodeSet::addAll(*((ଅଚଳ UnicodeSet*)additionalSet));
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_add(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::add(c);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_addRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::add(start, end);    
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_addString(USet* set, ଅଚଳ char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::add(s);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_addAllCodePoints(USet* set, ଅଚଳ char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::addAll(s);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_remove(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::remove(c);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_removeRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::remove(start, end);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_removeString(USet* set, ଅଚଳ char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::remove(s);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_removeAllCodePoints(USet *set, ଅଚଳ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::removeAll(s);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_removeAll(USet* set, ଅଚଳ USet* remove) {
    ((UnicodeSet*) set)->UnicodeSet::removeAll(*(ଅଚଳ UnicodeSet*)remove);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_retain(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::retain(start, end);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_retainString(USet *set, ଅଚଳ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retain(s);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_retainAllCodePoints(USet *set, ଅଚଳ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retainAll(s);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_retainAll(USet* set, ଅଚଳ USet* retain) {
    ((UnicodeSet*) set)->UnicodeSet::retainAll(*(ଅଚଳ UnicodeSet*)retain);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_compact(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::compact();
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_complement(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::complement();
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_complementRange(USet *set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::complement(start, end);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_complementString(USet *set, ଅଚଳ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complement(s);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_complementAllCodePoints(USet *set, ଅଚଳ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complementAll(s);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_complementAll(USet* set, ଅଚଳ USet* complement) {
    ((UnicodeSet*) set)->UnicodeSet::complementAll(*(ଅଚଳ UnicodeSet*)complement);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_clear(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::clear();
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_removeAllStrings(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::removeAllStrings();
}

U_CAPI UBool U_EXPORT2
uset_isEmpty(ଅଚଳ USet* set) {
    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet*) set)->UnicodeSet::isEmpty();
}

U_CAPI UBool U_EXPORT2
uset_hasStrings(ଅଚଳ USet* set) {
    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet*) set)->UnicodeSet::hasStrings();
}

U_CAPI UBool U_EXPORT2
uset_contains(ଅଚଳ USet* set, UChar32 c) {
    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet*) set)->UnicodeSet::contains(c);
}

U_CAPI UBool U_EXPORT2
uset_containsRange(ଅଚଳ USet* set, UChar32 start, UChar32 end) {
    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet*) set)->UnicodeSet::contains(start, end);
}

U_CAPI UBool U_EXPORT2
uset_containsString(ଅଚଳ USet* set, ଅଚଳ char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet*) set)->UnicodeSet::contains(s);
}

U_CAPI UBool U_EXPORT2
uset_containsAll(ଅଚଳ USet* set1, ଅଚଳ USet* set2) {
    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet*) set1)->UnicodeSet::containsAll(* (ଅଚଳ UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsAllCodePoints(ଅଚଳ USet* set, ଅଚଳ char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet*) set)->UnicodeSet::containsAll(s);
}

U_CAPI UBool U_EXPORT2
uset_containsNone(ଅଚଳ USet* set1, ଅଚଳ USet* set2) {
    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet*) set1)->UnicodeSet::containsNone(* (ଅଚଳ UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsSome(ଅଚଳ USet* set1, ଅଚଳ USet* set2) {
    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet*) set1)->UnicodeSet::containsSome(* (ଅଚଳ UnicodeSet*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_span(ଅଚଳ USet *set, ଅଚଳ char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    ଫେରନ୍ତୁ ((UnicodeSet*) set)->UnicodeSet::span(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBack(ଅଚଳ USet *set, ଅଚଳ char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    ଫେରନ୍ତୁ ((UnicodeSet*) set)->UnicodeSet::spanBack(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanUTF8(ଅଚଳ USet *set, ଅଚଳ ଅକ୍ଷର *s, int32_t length, USetSpanCondition spanCondition) {
    ଫେରନ୍ତୁ ((UnicodeSet*) set)->UnicodeSet::spanUTF8(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBackUTF8(ଅଚଳ USet *set, ଅଚଳ ଅକ୍ଷର *s, int32_t length, USetSpanCondition spanCondition) {
    ଫେରନ୍ତୁ ((UnicodeSet*) set)->UnicodeSet::spanBackUTF8(s, length, spanCondition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ଅଚଳ USet* set1, ଅଚଳ USet* set2) {
    ଫେରନ୍ତୁ *(ଅଚଳ UnicodeSet*)set1 == *(ଅଚଳ UnicodeSet*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_indexOf(ଅଚଳ USet* set, UChar32 c) {
    ଫେରନ୍ତୁ ((UnicodeSet*) set)->UnicodeSet::indexOf(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ଅଚଳ USet* set, int32_t index) {
    ଫେରନ୍ତୁ ((UnicodeSet*) set)->UnicodeSet::charAt(index);
}

U_CAPI int32_t U_EXPORT2
uset_size(ଅଚଳ USet* set) {
    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet*) set)->UnicodeSet::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
ଶ୍ରେଣୀ USetAccess /* not : public UObject because all methods are static */ {
ସର୍ବସାଧାରଣ:
    /* Try to have the compiler inline these*/
    ଧାଡ଼ିଭିତରେ ସ୍ଥିର int32_t get_String_ଗଣନା(ଅଚଳ UnicodeSet& set) {
        ଫେରନ୍ତୁ set.stringsSize();
    }
    ଧାଡ଼ିଭିତରେ ସ୍ଥିର ଅଚଳ UnicodeString* getString(ଅଚଳ UnicodeSet& set U_LIFETIME_BOUND,
                                                 int32_t i) {
        ଫେରନ୍ତୁ set.getString(i);
    }
ବ୍ୟକ୍ତିଗତ:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_END

U_CAPI int32_t U_EXPORT2
uset_get_Range_ଗଣନା(ଅଚଳ USet *set) {
    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet *)set)->UnicodeSet::get_Range_ଗଣନା();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_ଗଣନା(ଅଚଳ USet *uset) {
    ଅଚଳ UnicodeSet &set = *(ଅଚଳ UnicodeSet *)uset;
    ଫେରନ୍ତୁ USetAccess::get_String_ଗଣନା(set);
}

U_CAPI int32_t U_EXPORT2
uset_get_Item_ଗଣନା(ଅଚଳ USet* uset) {
    ଅଚଳ UnicodeSet& set = *(ଅଚଳ UnicodeSet*)uset;
    ଫେରନ୍ତୁ set.get_Range_ଗଣନା() + USetAccess::get_String_ଗଣନା(set);
}

U_CAPI ଅଚଳ UChar* U_EXPORT2
uset_getString(ଅଚଳ USet* uset U_LIFETIME_BOUND, int32_t index, int32_t* pLength) {
    ଯଦି (pLength == nullptr) { ଫେରନ୍ତୁ nullptr; }
    ଅଚଳ UnicodeSet &set = *(ଅଚଳ UnicodeSet *)uset;
    int32_t count = USetAccess::get_String_ଗଣନା(set);
    ଯଦି (index < 0 || count <= index) {
        *pLength = 0;
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ UnicodeString *s = USetAccess::getString(set, index);
    *pLength = s->length();
    ଫେରନ୍ତୁ toUCharPtr(s->getBuffer());
}

U_CAPI int32_t U_EXPORT2
uset_getItem(ଅଚଳ USet* uset, int32_t itemIndex,
             UChar32* start, UChar32* end,
             char16_t* str, int32_t strCapacity,
             UErrorCode* ec) {
    ଯଦି (U_FAILURE(*ec)) ଫେରନ୍ତୁ 0;
    ଅଚଳ UnicodeSet& set = *(ଅଚଳ UnicodeSet*)uset;
    int32_t range_ଗଣନା;

    ଯଦି (itemIndex < 0) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ -1;
    } ଅନ୍ୟଥା ଯଦି (itemIndex < (range_ଗଣନା = set.get_Range_ଗଣନା())) {
        *start = set.getRangeStart(itemIndex);
        *end = set.getRangeEnd(itemIndex);
        ଫେରନ୍ତୁ 0;
    } ଅନ୍ୟଥା {
        itemIndex -= range_ଗଣନା;
        ଯଦି (itemIndex < USetAccess::get_String_ଗଣନା(set)) {
            ଅଚଳ UnicodeString* s = USetAccess::getString(set, itemIndex);
            ଫେରନ୍ତୁ s->extract(str, strCapacity, *ec);
        } ଅନ୍ୟଥା {
            *ec = U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ -1;
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
uset_serialize(ଅଚଳ USet* set, uint16_t* dest, int32_t destCapacity, UErrorCode* ec) {
    ଯଦି (ec==nullptr || U_FAILURE(*ec)) {
        ଫେରନ୍ତୁ 0;
    }

    ଫେରନ୍ତୁ ((ଅଚଳ UnicodeSet*) set)->UnicodeSet::serialize(dest, destCapacity,* ec);
}

U_CAPI UBool U_EXPORT2
uset_getSerializedSet(USerializedSet* fillSet, ଅଚଳ uint16_t* src, int32_t srcLength) {
    int32_t length;

    ଯଦି(fillSet==nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି(src==nullptr || srcLength<=0) {
        fillSet->length=fillSet->bmpLength=0;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    length=*src++;
    ଯଦି(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        ଯଦି(srcLength<(2+length)) {
            fillSet->length=fillSet->bmpLength=0;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        fillSet->bmpLength=*src++;
    } ଅନ୍ୟଥା {
        /* only BMP values */
        ଯଦି(srcLength<(1+length)) {
            fillSet->length=fillSet->bmpLength=0;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        fillSet->bmpLength=length;
    }
    fillSet->array=src;
    fillSet->length=length;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
uset_setSerializedToOne(USerializedSet* fillSet, UChar32 c) {
    ଯଦି(fillSet==nullptr || (uint32_t)c>0x10ffff) {
        ଫେରନ୍ତୁ;
    }

    fillSet->array=fillSet->staticArray;
    ଯଦି(c<0xffff) {
        fillSet->bmpLength=fillSet->length=2;
        fillSet->staticArray[0]=(uint16_t)c;
        fillSet->staticArray[1]=(uint16_t)c+1;
    } ଅନ୍ୟଥା ଯଦି(c==0xffff) {
        fillSet->bmpLength=1;
        fillSet->length=3;
        fillSet->staticArray[0]=0xffff;
        fillSet->staticArray[1]=1;
        fillSet->staticArray[2]=0;
    } ଅନ୍ୟଥା ଯଦି(c<0x10ffff) {
        fillSet->bmpLength=0;
        fillSet->length=4;
        fillSet->staticArray[0]=(uint16_t)(c>>16);
        fillSet->staticArray[1]=(uint16_t)c;
        ++c;
        fillSet->staticArray[2]=(uint16_t)(c>>16);
        fillSet->staticArray[3]=(uint16_t)c;
    } ଅନ୍ୟଥା /* c==0x10ffff */ {
        fillSet->bmpLength=0;
        fillSet->length=2;
        fillSet->staticArray[0]=0x10;
        fillSet->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serializedContains(ଅଚଳ USerializedSet* set, UChar32 c) {
    ଅଚଳ uint16_t* array;

    ଯଦି(set==nullptr || (uint32_t)c>0x10ffff) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    array=set->array;
    ଯଦି(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = set->bmpLength-1;
        ଯଦି (c < array[0]) {
            hi = 0;
        } ଅନ୍ୟଥା ଯଦି (c < array[hi]) {
            ପାଇଁ(;;) {
                int32_t i = (lo + hi) >> 1;
                ଯଦି (i == lo) {
                    ଭାଙ୍ଗ;  // Done!
                } ଅନ୍ୟଥା ଯଦି (c < array[i]) {
                    hi = i;
                } ଅନ୍ୟଥା {
                    lo = i;
                }
            }
        } ଅନ୍ୟଥା {
            hi += 1;
        }
        ଫେରନ୍ତୁ hi&1;
    } ଅନ୍ୟଥା {
        /* find c in the supplementary part */
        uint16_t ଉପର=(uint16_t)(c>>16), ତଳ=(uint16_t)c;
        int32_t base = set->bmpLength;
        int32_t lo = 0;
        int32_t hi = set->length - 2 - base;
        ଯଦି (ଉପର < array[base] || (ଉପର==array[base] && ତଳ<array[base+1])) {
            hi = 0;
        } ଅନ୍ୟଥା ଯଦି (ଉପର < array[base+hi] || (ଉପର==array[base+hi] && ତଳ<array[base+hi+1])) {
            ପାଇଁ (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + base;
                ଯଦି (i == lo) {
                    ଭାଙ୍ଗ;  // Done!
                } ଅନ୍ୟଥା ଯଦି (ଉପର < array[iabs] || (ଉପର==array[iabs] && ତଳ<array[iabs+1])) {
                    hi = i;
                } ଅନ୍ୟଥା {
                    lo = i;
                }
            }
        } ଅନ୍ୟଥା {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        ଫେରନ୍ତୁ ((hi+(base<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_ଗଣନା(ଅଚଳ USerializedSet* set) {
    ଯଦି(set==nullptr) {
        ଫେରନ୍ତୁ 0;
    }

    ଫେରନ୍ତୁ (set->bmpLength+(set->length-set->bmpLength)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ଅଚଳ USerializedSet* set, int32_t rangeIndex,
                        UChar32* pStart, UChar32* pEnd) {
    ଅଚଳ uint16_t* array;
    int32_t bmpLength, length;

    ଯଦି(set==nullptr || rangeIndex<0 || pStart==nullptr || pEnd==nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    array=set->array;
    length=set->length;
    bmpLength=set->bmpLength;

    rangeIndex*=2; /* address start/limit pairs */
    ଯଦି(rangeIndex<bmpLength) {
        *pStart=array[rangeIndex++];
        ଯଦି(rangeIndex<bmpLength) {
            *pEnd=array[rangeIndex]-1;
        } ଅନ୍ୟଥା ଯଦି(rangeIndex<length) {
            *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
        } ଅନ୍ୟଥା {
            *pEnd=0x10ffff;
        }
        ଫେରନ୍ତୁ ସତ୍ୟ;
    } ଅନ୍ୟଥା {
        rangeIndex-=bmpLength;
        rangeIndex*=2; /* address pairs of pairs of units */
        length-=bmpLength;
        ଯଦି(rangeIndex<length) {
            array+=bmpLength;
            *pStart=(((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1];
            rangeIndex+=2;
            ଯଦି(rangeIndex<length) {
                *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
            } ଅନ୍ୟଥା {
                *pEnd=0x10ffff;
            }
            ଫେରନ୍ତୁ ସତ୍ୟ;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
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
