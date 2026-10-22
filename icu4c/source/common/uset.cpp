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
    ফেরত (USet*) নতুন UnicodeSet();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 start, UChar32 end) {
    ফেরত (USet*) নতুন UnicodeSet(start, end);
}

U_CAPI শূন্য U_EXPORT2
uset_close(USet* set) {
    delete (UnicodeSet*) set;
}

U_CAPI USet * U_EXPORT2
uset_clone(ধ্রুবক USet *set) {
    ফেরত (USet*) (((UnicodeSet*) set)->UnicodeSet::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ধ্রুবক USet *set) {
    ফেরত ((UnicodeSet*) set)->UnicodeSet::isFrozen();
}

U_CAPI শূন্য U_EXPORT2
uset_freeze(USet *set) {
    ((UnicodeSet*) set)->UnicodeSet::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ধ্রুবক USet *set) {
    ফেরত (USet*) (((UnicodeSet*) set)->UnicodeSet::cloneAsThawed());
}

U_CAPI শূন্য U_EXPORT2
uset_set(USet* set,
     UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::set(start, end);
}

U_CAPI শূন্য U_EXPORT2
uset_addAll(USet* set, ধ্রুবক USet *additionalSet) {
    ((UnicodeSet*) set)->UnicodeSet::addAll(*((ধ্রুবক UnicodeSet*)additionalSet));
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
uset_addString(USet* set, ধ্রুবক char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::add(s);
}

U_CAPI শূন্য U_EXPORT2
uset_addAllCodePoints(USet* set, ধ্রুবক char16_t *str, int32_t strLen) {
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
uset_removeString(USet* set, ধ্রুবক char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::remove(s);
}

U_CAPI শূন্য U_EXPORT2
uset_removeAllCodePoints(USet *set, ধ্রুবক char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::removeAll(s);
}

U_CAPI শূন্য U_EXPORT2
uset_removeAll(USet* set, ধ্রুবক USet* remove) {
    ((UnicodeSet*) set)->UnicodeSet::removeAll(*(ধ্রুবক UnicodeSet*)remove);
}

U_CAPI শূন্য U_EXPORT2
uset_retain(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::retain(start, end);
}

U_CAPI শূন্য U_EXPORT2
uset_retainString(USet *set, ধ্রুবক char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retain(s);
}

U_CAPI শূন্য U_EXPORT2
uset_retainAllCodePoints(USet *set, ধ্রুবক char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retainAll(s);
}

U_CAPI শূন্য U_EXPORT2
uset_retainAll(USet* set, ধ্রুবক USet* retain) {
    ((UnicodeSet*) set)->UnicodeSet::retainAll(*(ধ্রুবক UnicodeSet*)retain);
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
uset_complementString(USet *set, ধ্রুবক char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complement(s);
}

U_CAPI শূন্য U_EXPORT2
uset_complementAllCodePoints(USet *set, ধ্রুবক char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complementAll(s);
}

U_CAPI শূন্য U_EXPORT2
uset_complementAll(USet* set, ধ্রুবক USet* complement) {
    ((UnicodeSet*) set)->UnicodeSet::complementAll(*(ধ্রুবক UnicodeSet*)complement);
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
uset_isEmpty(ধ্রুবক USet* set) {
    ফেরত ((ধ্রুবক UnicodeSet*) set)->UnicodeSet::isEmpty();
}

U_CAPI UBool U_EXPORT2
uset_hasStrings(ধ্রুবক USet* set) {
    ফেরত ((ধ্রুবক UnicodeSet*) set)->UnicodeSet::hasStrings();
}

U_CAPI UBool U_EXPORT2
uset_contains(ধ্রুবক USet* set, UChar32 c) {
    ফেরত ((ধ্রুবক UnicodeSet*) set)->UnicodeSet::contains(c);
}

U_CAPI UBool U_EXPORT2
uset_containsRange(ধ্রুবক USet* set, UChar32 start, UChar32 end) {
    ফেরত ((ধ্রুবক UnicodeSet*) set)->UnicodeSet::contains(start, end);
}

U_CAPI UBool U_EXPORT2
uset_containsString(ধ্রুবক USet* set, ধ্রুবক char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ফেরত ((ধ্রুবক UnicodeSet*) set)->UnicodeSet::contains(s);
}

U_CAPI UBool U_EXPORT2
uset_containsAll(ধ্রুবক USet* set1, ধ্রুবক USet* set2) {
    ফেরত ((ধ্রুবক UnicodeSet*) set1)->UnicodeSet::containsAll(* (ধ্রুবক UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsAllCodePoints(ধ্রুবক USet* set, ধ্রুবক char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    ফেরত ((ধ্রুবক UnicodeSet*) set)->UnicodeSet::containsAll(s);
}

U_CAPI UBool U_EXPORT2
uset_containsNone(ধ্রুবক USet* set1, ধ্রুবক USet* set2) {
    ফেরত ((ধ্রুবক UnicodeSet*) set1)->UnicodeSet::containsNone(* (ধ্রুবক UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsSome(ধ্রুবক USet* set1, ধ্রুবক USet* set2) {
    ফেরত ((ধ্রুবক UnicodeSet*) set1)->UnicodeSet::containsSome(* (ধ্রুবক UnicodeSet*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_span(ধ্রুবক USet *set, ধ্রুবক char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    ফেরত ((UnicodeSet*) set)->UnicodeSet::span(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBack(ধ্রুবক USet *set, ধ্রুবক char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    ফেরত ((UnicodeSet*) set)->UnicodeSet::spanBack(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanUTF8(ধ্রুবক USet *set, ধ্রুবক অক্ষর *s, int32_t length, USetSpanCondition spanCondition) {
    ফেরত ((UnicodeSet*) set)->UnicodeSet::spanUTF8(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBackUTF8(ধ্রুবক USet *set, ধ্রুবক অক্ষর *s, int32_t length, USetSpanCondition spanCondition) {
    ফেরত ((UnicodeSet*) set)->UnicodeSet::spanBackUTF8(s, length, spanCondition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ধ্রুবক USet* set1, ধ্রুবক USet* set2) {
    ফেরত *(ধ্রুবক UnicodeSet*)set1 == *(ধ্রুবক UnicodeSet*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_সূচক_Of(ধ্রুবক USet* set, UChar32 c) {
    ফেরত ((UnicodeSet*) set)->UnicodeSet::সূচক_Of(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ধ্রুবক USet* set, int32_t সূচক) {
    ফেরত ((UnicodeSet*) set)->UnicodeSet::charAt(সূচক);
}

U_CAPI int32_t U_EXPORT2
uset_আকার(ধ্রুবক USet* set) {
    ফেরত ((ধ্রুবক UnicodeSet*) set)->UnicodeSet::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
শ্রেণী USetAccess /* not : public UObject because all methods are static */ {
সর্বজনীন:
    /* Try to have the compiler inline these*/
    অন্তঃরেখ স্থির int32_t get_String_গুনতি(ধ্রুবক UnicodeSet& set) {
        ফেরত set.strings_আকার();
    }
    অন্তঃরেখ স্থির ধ্রুবক UnicodeString* getString(ধ্রুবক UnicodeSet& set U_LIFETIME_BOUND,
                                                 int32_t i) {
        ফেরত set.getString(i);
    }
ব্যক্তিগত:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_END

U_CAPI int32_t U_EXPORT2
uset_get_Range_গুনতি(ধ্রুবক USet *set) {
    ফেরত ((ধ্রুবক UnicodeSet *)set)->UnicodeSet::get_Range_গুনতি();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_গুনতি(ধ্রুবক USet *uset) {
    ধ্রুবক UnicodeSet &set = *(ধ্রুবক UnicodeSet *)uset;
    ফেরত USetAccess::get_String_গুনতি(set);
}

U_CAPI int32_t U_EXPORT2
uset_get_Item_গুনতি(ধ্রুবক USet* uset) {
    ধ্রুবক UnicodeSet& set = *(ধ্রুবক UnicodeSet*)uset;
    ফেরত set.get_Range_গুনতি() + USetAccess::get_String_গুনতি(set);
}

U_CAPI ধ্রুবক UChar* U_EXPORT2
uset_getString(ধ্রুবক USet* uset U_LIFETIME_BOUND, int32_t সূচক, int32_t* p_দৈর্ঘ্য) {
    যদি (p_দৈর্ঘ্য == nullptr) { ফেরত nullptr; }
    ধ্রুবক UnicodeSet &set = *(ধ্রুবক UnicodeSet *)uset;
    int32_t count = USetAccess::get_String_গুনতি(set);
    যদি (সূচক < 0 || count <= সূচক) {
        *p_দৈর্ঘ্য = 0;
        ফেরত nullptr;
    }
    ধ্রুবক UnicodeString *s = USetAccess::getString(set, সূচক);
    *p_দৈর্ঘ্য = s->length();
    ফেরত toUCharPtr(s->get_বাফার());
}

U_CAPI int32_t U_EXPORT2
uset_getItem(ধ্রুবক USet* uset, int32_t item_সূচক,
             UChar32* start, UChar32* end,
             char16_t* str, int32_t strCapacity,
             UErrorCode* ec) {
    যদি (U_FAILURE(*ec)) ফেরত 0;
    ধ্রুবক UnicodeSet& set = *(ধ্রুবক UnicodeSet*)uset;
    int32_t range_গুনতি;

    যদি (item_সূচক < 0) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত -1;
    } অন্যথা যদি (item_সূচক < (range_গুনতি = set.get_Range_গুনতি())) {
        *start = set.getRangeStart(item_সূচক);
        *end = set.getRangeEnd(item_সূচক);
        ফেরত 0;
    } অন্যথা {
        item_সূচক -= range_গুনতি;
        যদি (item_সূচক < USetAccess::get_String_গুনতি(set)) {
            ধ্রুবক UnicodeString* s = USetAccess::getString(set, item_সূচক);
            ফেরত s->extract(str, strCapacity, *ec);
        } অন্যথা {
            *ec = U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত -1;
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
uset_serialize(ধ্রুবক USet* set, uint16_t* dest, int32_t destCapacity, UErrorCode* ec) {
    যদি (ec==nullptr || U_FAILURE(*ec)) {
        ফেরত 0;
    }

    ফেরত ((ধ্রুবক UnicodeSet*) set)->UnicodeSet::serialize(dest, destCapacity,* ec);
}

U_CAPI UBool U_EXPORT2
uset_getSerializedSet(USerializedSet* fillSet, ধ্রুবক uint16_t* src, int32_t src_দৈর্ঘ্য) {
    int32_t length;

    যদি(fillSet==nullptr) {
        ফেরত মিথ্যা;
    }
    যদি(src==nullptr || src_দৈর্ঘ্য<=0) {
        fillSet->length=fillSet->bmp_দৈর্ঘ্য=0;
        ফেরত মিথ্যা;
    }

    length=*src++;
    যদি(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        যদি(src_দৈর্ঘ্য<(2+length)) {
            fillSet->length=fillSet->bmp_দৈর্ঘ্য=0;
            ফেরত মিথ্যা;
        }
        fillSet->bmp_দৈর্ঘ্য=*src++;
    } অন্যথা {
        /* only BMP values */
        যদি(src_দৈর্ঘ্য<(1+length)) {
            fillSet->length=fillSet->bmp_দৈর্ঘ্য=0;
            ফেরত মিথ্যা;
        }
        fillSet->bmp_দৈর্ঘ্য=length;
    }
    fillSet->array=src;
    fillSet->length=length;
    ফেরত সত্য;
}

U_CAPI শূন্য U_EXPORT2
uset_setSerializedToOne(USerializedSet* fillSet, UChar32 c) {
    যদি(fillSet==nullptr || (uint32_t)c>0x10ffff) {
        ফেরত;
    }

    fillSet->array=fillSet->staticArray;
    যদি(c<0xffff) {
        fillSet->bmp_দৈর্ঘ্য=fillSet->length=2;
        fillSet->staticArray[0]=(uint16_t)c;
        fillSet->staticArray[1]=(uint16_t)c+1;
    } অন্যথা যদি(c==0xffff) {
        fillSet->bmp_দৈর্ঘ্য=1;
        fillSet->length=3;
        fillSet->staticArray[0]=0xffff;
        fillSet->staticArray[1]=1;
        fillSet->staticArray[2]=0;
    } অন্যথা যদি(c<0x10ffff) {
        fillSet->bmp_দৈর্ঘ্য=0;
        fillSet->length=4;
        fillSet->staticArray[0]=(uint16_t)(c>>16);
        fillSet->staticArray[1]=(uint16_t)c;
        ++c;
        fillSet->staticArray[2]=(uint16_t)(c>>16);
        fillSet->staticArray[3]=(uint16_t)c;
    } অন্যথা /* c==0x10ffff */ {
        fillSet->bmp_দৈর্ঘ্য=0;
        fillSet->length=2;
        fillSet->staticArray[0]=0x10;
        fillSet->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serializedContains(ধ্রুবক USerializedSet* set, UChar32 c) {
    ধ্রুবক uint16_t* array;

    যদি(set==nullptr || (uint32_t)c>0x10ffff) {
        ফেরত মিথ্যা;
    }

    array=set->array;
    যদি(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = set->bmp_দৈর্ঘ্য-1;
        যদি (c < array[0]) {
            hi = 0;
        } অন্যথা যদি (c < array[hi]) {
            জন্য(;;) {
                int32_t i = (lo + hi) >> 1;
                যদি (i == lo) {
                    বিরতি;  // Done!
                } অন্যথা যদি (c < array[i]) {
                    hi = i;
                } অন্যথা {
                    lo = i;
                }
            }
        } অন্যথা {
            hi += 1;
        }
        ফেরত hi&1;
    } অন্যথা {
        /* find c in the supplementary part */
        uint16_t উপরের=(uint16_t)(c>>16), নিচের=(uint16_t)c;
        int32_t base = set->bmp_দৈর্ঘ্য;
        int32_t lo = 0;
        int32_t hi = set->length - 2 - base;
        যদি (উপরের < array[base] || (উপরের==array[base] && নিচের<array[base+1])) {
            hi = 0;
        } অন্যথা যদি (উপরের < array[base+hi] || (উপরের==array[base+hi] && নিচের<array[base+hi+1])) {
            জন্য (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + base;
                যদি (i == lo) {
                    বিরতি;  // Done!
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
        ফেরত ((hi+(base<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_গুনতি(ধ্রুবক USerializedSet* set) {
    যদি(set==nullptr) {
        ফেরত 0;
    }

    ফেরত (set->bmp_দৈর্ঘ্য+(set->length-set->bmp_দৈর্ঘ্য)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ধ্রুবক USerializedSet* set, int32_t range_সূচক,
                        UChar32* pStart, UChar32* pEnd) {
    ধ্রুবক uint16_t* array;
    int32_t bmp_দৈর্ঘ্য, length;

    যদি(set==nullptr || range_সূচক<0 || pStart==nullptr || pEnd==nullptr) {
        ফেরত মিথ্যা;
    }

    array=set->array;
    length=set->length;
    bmp_দৈর্ঘ্য=set->bmp_দৈর্ঘ্য;

    range_সূচক*=2; /* address start/limit pairs */
    যদি(range_সূচক<bmp_দৈর্ঘ্য) {
        *pStart=array[range_সূচক++];
        যদি(range_সূচক<bmp_দৈর্ঘ্য) {
            *pEnd=array[range_সূচক]-1;
        } অন্যথা যদি(range_সূচক<length) {
            *pEnd=((((int32_t)array[range_সূচক])<<16)|array[range_সূচক+1])-1;
        } অন্যথা {
            *pEnd=0x10ffff;
        }
        ফেরত সত্য;
    } অন্যথা {
        range_সূচক-=bmp_দৈর্ঘ্য;
        range_সূচক*=2; /* address pairs of pairs of units */
        length-=bmp_দৈর্ঘ্য;
        যদি(range_সূচক<length) {
            array+=bmp_দৈর্ঘ্য;
            *pStart=(((int32_t)array[range_সূচক])<<16)|array[range_সূচক+1];
            range_সূচক+=2;
            যদি(range_সূচক<length) {
                *pEnd=((((int32_t)array[range_সূচক])<<16)|array[range_সূচক+1])-1;
            } অন্যথা {
                *pEnd=0x10ffff;
            }
            ফেরত সত্য;
        } অন্যথা {
            ফেরত মিথ্যা;
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
