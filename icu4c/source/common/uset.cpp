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

#ضمن "unicode/utypes.h"
#ضمن "unicode/char16ptr.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/uset.h"
#ضمن "unicode/uniset.h"
#ضمن "cmemory.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openEmpty() {
    أرجع (USet*) جديد UnicodeSet();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 start, UChar32 end) {
    أرجع (USet*) جديد UnicodeSet(start, end);
}

U_CAPI فراغ U_EXPORT2
uset_close(USet* set) {
    delete (UnicodeSet*) set;
}

U_CAPI USet * U_EXPORT2
uset_clone(ثابت USet *set) {
    أرجع (USet*) (((UnicodeSet*) set)->UnicodeSet::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ثابت USet *set) {
    أرجع ((UnicodeSet*) set)->UnicodeSet::isFrozen();
}

U_CAPI فراغ U_EXPORT2
uset_freeze(USet *set) {
    ((UnicodeSet*) set)->UnicodeSet::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ثابت USet *set) {
    أرجع (USet*) (((UnicodeSet*) set)->UnicodeSet::cloneAsThawed());
}

U_CAPI فراغ U_EXPORT2
uset_set(USet* set,
     UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::set(start, end);
}

U_CAPI فراغ U_EXPORT2
uset_addAll(USet* set, ثابت USet *additionalSet) {
    ((UnicodeSet*) set)->UnicodeSet::addAll(*((ثابت UnicodeSet*)additionalSet));
}

U_CAPI فراغ U_EXPORT2
uset_add(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::add(c);
}

U_CAPI فراغ U_EXPORT2
uset_addRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::add(start, end);    
}

U_CAPI فراغ U_EXPORT2
uset_addString(USet* set, ثابت char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::add(s);
}

U_CAPI فراغ U_EXPORT2
uset_addAllCodePoints(USet* set, ثابت char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::addAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_remove(USet* set, UChar32 c) {
    ((UnicodeSet*) set)->UnicodeSet::remove(c);
}

U_CAPI فراغ U_EXPORT2
uset_removeRange(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::remove(start, end);
}

U_CAPI فراغ U_EXPORT2
uset_removeString(USet* set, ثابت char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((UnicodeSet*) set)->UnicodeSet::remove(s);
}

U_CAPI فراغ U_EXPORT2
uset_removeAllCodePoints(USet *set, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::removeAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_removeAll(USet* set, ثابت USet* remove) {
    ((UnicodeSet*) set)->UnicodeSet::removeAll(*(ثابت UnicodeSet*)remove);
}

U_CAPI فراغ U_EXPORT2
uset_retain(USet* set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::retain(start, end);
}

U_CAPI فراغ U_EXPORT2
uset_retainString(USet *set, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retain(s);
}

U_CAPI فراغ U_EXPORT2
uset_retainAllCodePoints(USet *set, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::retainAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_retainAll(USet* set, ثابت USet* retain) {
    ((UnicodeSet*) set)->UnicodeSet::retainAll(*(ثابت UnicodeSet*)retain);
}

U_CAPI فراغ U_EXPORT2
uset_compact(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::compact();
}

U_CAPI فراغ U_EXPORT2
uset_complement(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::complement();
}

U_CAPI فراغ U_EXPORT2
uset_complementRange(USet *set, UChar32 start, UChar32 end) {
    ((UnicodeSet*) set)->UnicodeSet::complement(start, end);
}

U_CAPI فراغ U_EXPORT2
uset_complementString(USet *set, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complement(s);
}

U_CAPI فراغ U_EXPORT2
uset_complementAllCodePoints(USet *set, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((UnicodeSet*) set)->UnicodeSet::complementAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_complementAll(USet* set, ثابت USet* complement) {
    ((UnicodeSet*) set)->UnicodeSet::complementAll(*(ثابت UnicodeSet*)complement);
}

U_CAPI فراغ U_EXPORT2
uset_clear(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::clear();
}

U_CAPI فراغ U_EXPORT2
uset_removeAllStrings(USet* set) {
    ((UnicodeSet*) set)->UnicodeSet::removeAllStrings();
}

U_CAPI UBool U_EXPORT2
uset_isEmpty(ثابت USet* set) {
    أرجع ((ثابت UnicodeSet*) set)->UnicodeSet::isEmpty();
}

U_CAPI UBool U_EXPORT2
uset_hasStrings(ثابت USet* set) {
    أرجع ((ثابت UnicodeSet*) set)->UnicodeSet::hasStrings();
}

U_CAPI UBool U_EXPORT2
uset_contains(ثابت USet* set, UChar32 c) {
    أرجع ((ثابت UnicodeSet*) set)->UnicodeSet::contains(c);
}

U_CAPI UBool U_EXPORT2
uset_containsRange(ثابت USet* set, UChar32 start, UChar32 end) {
    أرجع ((ثابت UnicodeSet*) set)->UnicodeSet::contains(start, end);
}

U_CAPI UBool U_EXPORT2
uset_containsString(ثابت USet* set, ثابت char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    أرجع ((ثابت UnicodeSet*) set)->UnicodeSet::contains(s);
}

U_CAPI UBool U_EXPORT2
uset_containsAll(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت UnicodeSet*) set1)->UnicodeSet::containsAll(* (ثابت UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsAllCodePoints(ثابت USet* set, ثابت char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    أرجع ((ثابت UnicodeSet*) set)->UnicodeSet::containsAll(s);
}

U_CAPI UBool U_EXPORT2
uset_containsNone(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت UnicodeSet*) set1)->UnicodeSet::containsNone(* (ثابت UnicodeSet*) set2);
}

U_CAPI UBool U_EXPORT2
uset_containsSome(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت UnicodeSet*) set1)->UnicodeSet::containsSome(* (ثابت UnicodeSet*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_span(ثابت USet *set, ثابت char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    أرجع ((UnicodeSet*) set)->UnicodeSet::span(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBack(ثابت USet *set, ثابت char16_t *s, int32_t length, USetSpanCondition spanCondition) {
    أرجع ((UnicodeSet*) set)->UnicodeSet::spanBack(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanUTF8(ثابت USet *set, ثابت حرف *s, int32_t length, USetSpanCondition spanCondition) {
    أرجع ((UnicodeSet*) set)->UnicodeSet::spanUTF8(s, length, spanCondition);
}

U_CAPI int32_t U_EXPORT2
uset_spanBackUTF8(ثابت USet *set, ثابت حرف *s, int32_t length, USetSpanCondition spanCondition) {
    أرجع ((UnicodeSet*) set)->UnicodeSet::spanBackUTF8(s, length, spanCondition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ثابت USet* set1, ثابت USet* set2) {
    أرجع *(ثابت UnicodeSet*)set1 == *(ثابت UnicodeSet*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_indexOf(ثابت USet* set, UChar32 c) {
    أرجع ((UnicodeSet*) set)->UnicodeSet::indexOf(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ثابت USet* set, int32_t index) {
    أرجع ((UnicodeSet*) set)->UnicodeSet::charAt(index);
}

U_CAPI int32_t U_EXPORT2
uset_size(ثابت USet* set) {
    أرجع ((ثابت UnicodeSet*) set)->UnicodeSet::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
صنف USetAccess /* not : public UObject because all methods are static */ {
عام:
    /* Try to have the compiler inline these*/
    مضمن ساكن int32_t get_String_سانى(ثابت UnicodeSet& set) {
        أرجع set.stringsSize();
    }
    مضمن ساكن ثابت UnicodeString* getString(ثابت UnicodeSet& set U_LIFETIME_BOUND,
                                                 int32_t i) {
        أرجع set.getString(i);
    }
خاص:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_END

U_CAPI int32_t U_EXPORT2
uset_get_Range_سانى(ثابت USet *set) {
    أرجع ((ثابت UnicodeSet *)set)->UnicodeSet::get_Range_سانى();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_سانى(ثابت USet *uset) {
    ثابت UnicodeSet &set = *(ثابت UnicodeSet *)uset;
    أرجع USetAccess::get_String_سانى(set);
}

U_CAPI int32_t U_EXPORT2
uset_get_Item_سانى(ثابت USet* uset) {
    ثابت UnicodeSet& set = *(ثابت UnicodeSet*)uset;
    أرجع set.get_Range_سانى() + USetAccess::get_String_سانى(set);
}

U_CAPI ثابت UChar* U_EXPORT2
uset_getString(ثابت USet* uset U_LIFETIME_BOUND, int32_t index, int32_t* pLength) {
    إذا (pLength == nullptr) { أرجع nullptr; }
    ثابت UnicodeSet &set = *(ثابت UnicodeSet *)uset;
    int32_t count = USetAccess::get_String_سانى(set);
    إذا (index < 0 || count <= index) {
        *pLength = 0;
        أرجع nullptr;
    }
    ثابت UnicodeString *s = USetAccess::getString(set, index);
    *pLength = s->length();
    أرجع toUCharPtr(s->getBuffer());
}

U_CAPI int32_t U_EXPORT2
uset_getItem(ثابت USet* uset, int32_t itemIndex,
             UChar32* start, UChar32* end,
             char16_t* str, int32_t strCapacity,
             UErrorCode* ec) {
    إذا (U_FAILURE(*ec)) أرجع 0;
    ثابت UnicodeSet& set = *(ثابت UnicodeSet*)uset;
    int32_t range_سانى;

    إذا (itemIndex < 0) {
        *ec = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع -1;
    } وإلا إذا (itemIndex < (range_سانى = set.get_Range_سانى())) {
        *start = set.getRangeStart(itemIndex);
        *end = set.getRangeEnd(itemIndex);
        أرجع 0;
    } وإلا {
        itemIndex -= range_سانى;
        إذا (itemIndex < USetAccess::get_String_سانى(set)) {
            ثابت UnicodeString* s = USetAccess::getString(set, itemIndex);
            أرجع s->extract(str, strCapacity, *ec);
        } وإلا {
            *ec = U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع -1;
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
uset_serialize(ثابت USet* set, uint16_t* dest, int32_t destCapacity, UErrorCode* ec) {
    إذا (ec==nullptr || U_FAILURE(*ec)) {
        أرجع 0;
    }

    أرجع ((ثابت UnicodeSet*) set)->UnicodeSet::serialize(dest, destCapacity,* ec);
}

U_CAPI UBool U_EXPORT2
uset_getSerializedSet(USerializedSet* fillSet, ثابت uint16_t* src, int32_t srcLength) {
    int32_t length;

    إذا(fillSet==nullptr) {
        أرجع خطأ;
    }
    إذا(src==nullptr || srcLength<=0) {
        fillSet->length=fillSet->bmpLength=0;
        أرجع خطأ;
    }

    length=*src++;
    إذا(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        إذا(srcLength<(2+length)) {
            fillSet->length=fillSet->bmpLength=0;
            أرجع خطأ;
        }
        fillSet->bmpLength=*src++;
    } وإلا {
        /* only BMP values */
        إذا(srcLength<(1+length)) {
            fillSet->length=fillSet->bmpLength=0;
            أرجع خطأ;
        }
        fillSet->bmpLength=length;
    }
    fillSet->array=src;
    fillSet->length=length;
    أرجع true;
}

U_CAPI فراغ U_EXPORT2
uset_setSerializedToOne(USerializedSet* fillSet, UChar32 c) {
    إذا(fillSet==nullptr || (uint32_t)c>0x10ffff) {
        أرجع;
    }

    fillSet->array=fillSet->staticArray;
    إذا(c<0xffff) {
        fillSet->bmpLength=fillSet->length=2;
        fillSet->staticArray[0]=(uint16_t)c;
        fillSet->staticArray[1]=(uint16_t)c+1;
    } وإلا إذا(c==0xffff) {
        fillSet->bmpLength=1;
        fillSet->length=3;
        fillSet->staticArray[0]=0xffff;
        fillSet->staticArray[1]=1;
        fillSet->staticArray[2]=0;
    } وإلا إذا(c<0x10ffff) {
        fillSet->bmpLength=0;
        fillSet->length=4;
        fillSet->staticArray[0]=(uint16_t)(c>>16);
        fillSet->staticArray[1]=(uint16_t)c;
        ++c;
        fillSet->staticArray[2]=(uint16_t)(c>>16);
        fillSet->staticArray[3]=(uint16_t)c;
    } وإلا /* c==0x10ffff */ {
        fillSet->bmpLength=0;
        fillSet->length=2;
        fillSet->staticArray[0]=0x10;
        fillSet->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serializedContains(ثابت USerializedSet* set, UChar32 c) {
    ثابت uint16_t* array;

    إذا(set==nullptr || (uint32_t)c>0x10ffff) {
        أرجع خطأ;
    }

    array=set->array;
    إذا(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = set->bmpLength-1;
        إذا (c < array[0]) {
            hi = 0;
        } وإلا إذا (c < array[hi]) {
            لكل(;;) {
                int32_t i = (lo + hi) >> 1;
                إذا (i == lo) {
                    اكسر;  // Done!
                } وإلا إذا (c < array[i]) {
                    hi = i;
                } وإلا {
                    lo = i;
                }
            }
        } وإلا {
            hi += 1;
        }
        أرجع hi&1;
    } وإلا {
        /* find c in the supplementary part */
        uint16_t يۇقىرى=(uint16_t)(c>>16), تۆۋەن=(uint16_t)c;
        int32_t base = set->bmpLength;
        int32_t lo = 0;
        int32_t hi = set->length - 2 - base;
        إذا (يۇقىرى < array[base] || (يۇقىرى==array[base] && تۆۋەن<array[base+1])) {
            hi = 0;
        } وإلا إذا (يۇقىرى < array[base+hi] || (يۇقىرى==array[base+hi] && تۆۋەن<array[base+hi+1])) {
            لكل (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + base;
                إذا (i == lo) {
                    اكسر;  // Done!
                } وإلا إذا (يۇقىرى < array[iabs] || (يۇقىرى==array[iabs] && تۆۋەن<array[iabs+1])) {
                    hi = i;
                } وإلا {
                    lo = i;
                }
            }
        } وإلا {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        أرجع ((hi+(base<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_سانى(ثابت USerializedSet* set) {
    إذا(set==nullptr) {
        أرجع 0;
    }

    أرجع (set->bmpLength+(set->length-set->bmpLength)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ثابت USerializedSet* set, int32_t rangeIndex,
                        UChar32* pStart, UChar32* pEnd) {
    ثابت uint16_t* array;
    int32_t bmpLength, length;

    إذا(set==nullptr || rangeIndex<0 || pStart==nullptr || pEnd==nullptr) {
        أرجع خطأ;
    }

    array=set->array;
    length=set->length;
    bmpLength=set->bmpLength;

    rangeIndex*=2; /* address start/limit pairs */
    إذا(rangeIndex<bmpLength) {
        *pStart=array[rangeIndex++];
        إذا(rangeIndex<bmpLength) {
            *pEnd=array[rangeIndex]-1;
        } وإلا إذا(rangeIndex<length) {
            *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
        } وإلا {
            *pEnd=0x10ffff;
        }
        أرجع true;
    } وإلا {
        rangeIndex-=bmpLength;
        rangeIndex*=2; /* address pairs of pairs of units */
        length-=bmpLength;
        إذا(rangeIndex<length) {
            array+=bmpLength;
            *pStart=(((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1];
            rangeIndex+=2;
            إذا(rangeIndex<length) {
                *pEnd=((((int32_t)array[rangeIndex])<<16)|array[rangeIndex+1])-1;
            } وإلا {
                *pEnd=0x10ffff;
            }
            أرجع true;
        } وإلا {
            أرجع خطأ;
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
