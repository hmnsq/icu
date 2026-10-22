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
uset_open_خالي_جايون() {
    أرجع (USet*) جديد Unicode_سيٽ();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 شروعات, UChar32 پڇاڙي) {
    أرجع (USet*) جديد Unicode_سيٽ(شروعات, پڇاڙي);
}

U_CAPI فراغ U_EXPORT2
uset_close(USet* سيٽ) {
    ميٽيو (Unicode_سيٽ*) سيٽ;
}

U_CAPI USet * U_EXPORT2
uset_clone(ثابت USet *سيٽ) {
    أرجع (USet*) (((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ثابت USet *سيٽ) {
    أرجع ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::isFrozen();
}

U_CAPI فراغ U_EXPORT2
uset_freeze(USet *سيٽ) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ثابت USet *سيٽ) {
    أرجع (USet*) (((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::cloneAsThawed());
}

U_CAPI فراغ U_EXPORT2
uset_سيٽ(USet* سيٽ,
     UChar32 شروعات, UChar32 پڇاڙي) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::سيٽ(شروعات, پڇاڙي);
}

U_CAPI فراغ U_EXPORT2
uset_جوڙيو_All(USet* سيٽ, ثابت USet *additional_سيٽ) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::جوڙيو_All(*((ثابت Unicode_سيٽ*)additional_سيٽ));
}

U_CAPI فراغ U_EXPORT2
uset_جوڙيو(USet* سيٽ, UChar32 c) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::جوڙيو(c);
}

U_CAPI فراغ U_EXPORT2
uset_جوڙيو_Range(USet* سيٽ, UChar32 شروعات, UChar32 پڇاڙي) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::جوڙيو(شروعات, پڇاڙي);    
}

U_CAPI فراغ U_EXPORT2
uset_جوڙيو_String(USet* سيٽ, ثابت char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::جوڙيو(s);
}

U_CAPI فراغ U_EXPORT2
uset_جوڙيو_All_ڪوڊ_Points(USet* سيٽ, ثابت char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::جوڙيو_All(s);
}

U_CAPI فراغ U_EXPORT2
uset_remove(USet* سيٽ, UChar32 c) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::remove(c);
}

U_CAPI فراغ U_EXPORT2
uset_removeRange(USet* سيٽ, UChar32 شروعات, UChar32 پڇاڙي) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::remove(شروعات, پڇاڙي);
}

U_CAPI فراغ U_EXPORT2
uset_removeString(USet* سيٽ, ثابت char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::remove(s);
}

U_CAPI فراغ U_EXPORT2
uset_remove_All_ڪوڊ_Points(USet *سيٽ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::removeAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_removeAll(USet* سيٽ, ثابت USet* remove) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::removeAll(*(ثابت Unicode_سيٽ*)remove);
}

U_CAPI فراغ U_EXPORT2
uset_retain(USet* سيٽ, UChar32 شروعات, UChar32 پڇاڙي) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::retain(شروعات, پڇاڙي);
}

U_CAPI فراغ U_EXPORT2
uset_retainString(USet *سيٽ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::retain(s);
}

U_CAPI فراغ U_EXPORT2
uset_retain_All_ڪوڊ_Points(USet *سيٽ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::retainAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_retainAll(USet* سيٽ, ثابت USet* retain) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::retainAll(*(ثابت Unicode_سيٽ*)retain);
}

U_CAPI فراغ U_EXPORT2
uset_compact(USet* سيٽ) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::compact();
}

U_CAPI فراغ U_EXPORT2
uset_complement(USet* سيٽ) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::complement();
}

U_CAPI فراغ U_EXPORT2
uset_complementRange(USet *سيٽ, UChar32 شروعات, UChar32 پڇاڙي) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::complement(شروعات, پڇاڙي);
}

U_CAPI فراغ U_EXPORT2
uset_complementString(USet *سيٽ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::complement(s);
}

U_CAPI فراغ U_EXPORT2
uset_complement_All_ڪوڊ_Points(USet *سيٽ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::complementAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_complementAll(USet* سيٽ, ثابت USet* complement) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::complementAll(*(ثابت Unicode_سيٽ*)complement);
}

U_CAPI فراغ U_EXPORT2
uset_clear(USet* سيٽ) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::clear();
}

U_CAPI فراغ U_EXPORT2
uset_remove_All_لڙيون(USet* سيٽ) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::remove_All_لڙيون();
}

U_CAPI UBool U_EXPORT2
uset_is_خالي_جايون(ثابت USet* سيٽ) {
    أرجع ((ثابت Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::is_خالي_جايون();
}

U_CAPI UBool U_EXPORT2
uset_has_لڙيون(ثابت USet* سيٽ) {
    أرجع ((ثابت Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::has_لڙيون();
}

U_CAPI UBool U_EXPORT2
uset_شامل_آهي(ثابت USet* سيٽ, UChar32 c) {
    أرجع ((ثابت Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::شامل_آهي(c);
}

U_CAPI UBool U_EXPORT2
uset_شامل_آهي_Range(ثابت USet* سيٽ, UChar32 شروعات, UChar32 پڇاڙي) {
    أرجع ((ثابت Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::شامل_آهي(شروعات, پڇاڙي);
}

U_CAPI UBool U_EXPORT2
uset_شامل_آهي_String(ثابت USet* سيٽ, ثابت char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    أرجع ((ثابت Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::شامل_آهي(s);
}

U_CAPI UBool U_EXPORT2
uset_شامل_آهي_All(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_سيٽ*) set1)->Unicode_سيٽ::شامل_آهي_All(* (ثابت Unicode_سيٽ*) set2);
}

U_CAPI UBool U_EXPORT2
uset_شامل_آهي_All_ڪوڊ_Points(ثابت USet* سيٽ, ثابت char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    أرجع ((ثابت Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::شامل_آهي_All(s);
}

U_CAPI UBool U_EXPORT2
uset_شامل_آهي_None(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_سيٽ*) set1)->Unicode_سيٽ::شامل_آهي_None(* (ثابت Unicode_سيٽ*) set2);
}

U_CAPI UBool U_EXPORT2
uset_شامل_آهي_Some(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_سيٽ*) set1)->Unicode_سيٽ::شامل_آهي_Some(* (ثابت Unicode_سيٽ*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_وسعت(ثابت USet *سيٽ, ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) {
    أرجع ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::وسعت(s, length, وسعت_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_وسعت_پٺ(ثابت USet *سيٽ, ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) {
    أرجع ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::وسعت_پٺ(s, length, وسعت_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_وسعت_UTF8(ثابت USet *سيٽ, ثابت حرف *s, int32_t length, USet_وسعت_Condition وسعت_Condition) {
    أرجع ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::وسعت_UTF8(s, length, وسعت_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_وسعت_پٺ_UTF8(ثابت USet *سيٽ, ثابت حرف *s, int32_t length, USet_وسعت_Condition وسعت_Condition) {
    أرجع ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::وسعت_پٺ_UTF8(s, length, وسعت_Condition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ثابت USet* set1, ثابت USet* set2) {
    أرجع *(ثابت Unicode_سيٽ*)set1 == *(ثابت Unicode_سيٽ*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_اشاريو_جو(ثابت USet* سيٽ, UChar32 c) {
    أرجع ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::اشاريو_جو(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ثابت USet* سيٽ, int32_t اشاريو) {
    أرجع ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::charAt(اشاريو);
}

U_CAPI int32_t U_EXPORT2
uset_سائيز(ثابت USet* سيٽ) {
    أرجع ((ثابت Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::size();
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
    مضمن ساكن int32_t get_String_ڳڻپ(ثابت Unicode_سيٽ& سيٽ) {
        أرجع سيٽ.لڙيون_سائيز();
    }
    مضمن ساكن ثابت UnicodeString* getString(ثابت Unicode_سيٽ& سيٽ U_LIFETIME_BOUND,
                                                 int32_t i) {
        أرجع سيٽ.getString(i);
    }
خاص:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_پڇاڙي

U_CAPI int32_t U_EXPORT2
uset_get_Range_ڳڻپ(ثابت USet *سيٽ) {
    أرجع ((ثابت Unicode_سيٽ *)سيٽ)->Unicode_سيٽ::get_Range_ڳڻپ();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_ڳڻپ(ثابت USet *uset) {
    ثابت Unicode_سيٽ &سيٽ = *(ثابت Unicode_سيٽ *)uset;
    أرجع USetAccess::get_String_ڳڻپ(سيٽ);
}

U_CAPI int32_t U_EXPORT2
uset_get_شي_ڳڻپ(ثابت USet* uset) {
    ثابت Unicode_سيٽ& سيٽ = *(ثابت Unicode_سيٽ*)uset;
    أرجع سيٽ.get_Range_ڳڻپ() + USetAccess::get_String_ڳڻپ(سيٽ);
}

U_CAPI ثابت UChar* U_EXPORT2
uset_getString(ثابت USet* uset U_LIFETIME_BOUND, int32_t اشاريو, int32_t* p_ڊيگهائي) {
    إذا (p_ڊيگهائي == خالي_اشارو) { أرجع خالي_اشارو; }
    ثابت Unicode_سيٽ &سيٽ = *(ثابت Unicode_سيٽ *)uset;
    int32_t count = USetAccess::get_String_ڳڻپ(سيٽ);
    إذا (اشاريو < 0 || count <= اشاريو) {
        *p_ڊيگهائي = 0;
        أرجع خالي_اشارو;
    }
    ثابت UnicodeString *s = USetAccess::getString(سيٽ, اشاريو);
    *p_ڊيگهائي = s->length();
    أرجع منزل_UChar_Ptr(s->get_بفر());
}

U_CAPI int32_t U_EXPORT2
uset_get_شي(ثابت USet* uset, int32_t شي_اشاريو,
             UChar32* شروعات, UChar32* پڇاڙي,
             char16_t* str, int32_t str_صلاحيت,
             UError_ڪوڊ* ec) {
    إذا (U_FAILURE(*ec)) أرجع 0;
    ثابت Unicode_سيٽ& سيٽ = *(ثابت Unicode_سيٽ*)uset;
    int32_t range_ڳڻپ;

    إذا (شي_اشاريو < 0) {
        *ec = U_ILLEGAL_دليل_ERROR;
        أرجع -1;
    } وإلا إذا (شي_اشاريو < (range_ڳڻپ = سيٽ.get_Range_ڳڻپ())) {
        *شروعات = سيٽ.get_Range_شروعات(شي_اشاريو);
        *پڇاڙي = سيٽ.get_Range_پڇاڙي(شي_اشاريو);
        أرجع 0;
    } وإلا {
        شي_اشاريو -= range_ڳڻپ;
        إذا (شي_اشاريو < USetAccess::get_String_ڳڻپ(سيٽ)) {
            ثابت UnicodeString* s = USetAccess::getString(سيٽ, شي_اشاريو);
            أرجع s->extract(str, str_صلاحيت, *ec);
        } وإلا {
            *ec = U_اشاريو_OUTOFBOUNDS_ERROR;
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
uset_serialize(ثابت USet* سيٽ, uint16_t* dest, int32_t dest_صلاحيت, UError_ڪوڊ* ec) {
    إذا (ec==خالي_اشارو || U_FAILURE(*ec)) {
        أرجع 0;
    }

    أرجع ((ثابت Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::serialize(dest, dest_صلاحيت,* ec);
}

U_CAPI UBool U_EXPORT2
uset_get_Serialized_سيٽ(USerialized_سيٽ* fill_سيٽ, ثابت uint16_t* src, int32_t src_ڊيگهائي) {
    int32_t length;

    إذا(fill_سيٽ==خالي_اشارو) {
        أرجع خطأ;
    }
    إذا(src==خالي_اشارو || src_ڊيگهائي<=0) {
        fill_سيٽ->length=fill_سيٽ->bmp_ڊيگهائي=0;
        أرجع خطأ;
    }

    length=*src++;
    إذا(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        إذا(src_ڊيگهائي<(2+length)) {
            fill_سيٽ->length=fill_سيٽ->bmp_ڊيگهائي=0;
            أرجع خطأ;
        }
        fill_سيٽ->bmp_ڊيگهائي=*src++;
    } وإلا {
        /* only BMP values */
        إذا(src_ڊيگهائي<(1+length)) {
            fill_سيٽ->length=fill_سيٽ->bmp_ڊيگهائي=0;
            أرجع خطأ;
        }
        fill_سيٽ->bmp_ڊيگهائي=length;
    }
    fill_سيٽ->array=src;
    fill_سيٽ->length=length;
    أرجع سچ;
}

U_CAPI فراغ U_EXPORT2
uset_سيٽ_Serialized_منزل_One(USerialized_سيٽ* fill_سيٽ, UChar32 c) {
    إذا(fill_سيٽ==خالي_اشارو || (uint32_t)c>0x10ffff) {
        أرجع;
    }

    fill_سيٽ->array=fill_سيٽ->staticArray;
    إذا(c<0xffff) {
        fill_سيٽ->bmp_ڊيگهائي=fill_سيٽ->length=2;
        fill_سيٽ->staticArray[0]=(uint16_t)c;
        fill_سيٽ->staticArray[1]=(uint16_t)c+1;
    } وإلا إذا(c==0xffff) {
        fill_سيٽ->bmp_ڊيگهائي=1;
        fill_سيٽ->length=3;
        fill_سيٽ->staticArray[0]=0xffff;
        fill_سيٽ->staticArray[1]=1;
        fill_سيٽ->staticArray[2]=0;
    } وإلا إذا(c<0x10ffff) {
        fill_سيٽ->bmp_ڊيگهائي=0;
        fill_سيٽ->length=4;
        fill_سيٽ->staticArray[0]=(uint16_t)(c>>16);
        fill_سيٽ->staticArray[1]=(uint16_t)c;
        ++c;
        fill_سيٽ->staticArray[2]=(uint16_t)(c>>16);
        fill_سيٽ->staticArray[3]=(uint16_t)c;
    } وإلا /* c==0x10ffff */ {
        fill_سيٽ->bmp_ڊيگهائي=0;
        fill_سيٽ->length=2;
        fill_سيٽ->staticArray[0]=0x10;
        fill_سيٽ->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serialized_شامل_آهي(ثابت USerialized_سيٽ* سيٽ, UChar32 c) {
    ثابت uint16_t* array;

    إذا(سيٽ==خالي_اشارو || (uint32_t)c>0x10ffff) {
        أرجع خطأ;
    }

    array=سيٽ->array;
    إذا(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = سيٽ->bmp_ڊيگهائي-1;
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
        uint16_t مٿاهون=(uint16_t)(c>>16), هيٺيون=(uint16_t)c;
        int32_t بنياد = سيٽ->bmp_ڊيگهائي;
        int32_t lo = 0;
        int32_t hi = سيٽ->length - 2 - بنياد;
        إذا (مٿاهون < array[بنياد] || (مٿاهون==array[بنياد] && هيٺيون<array[بنياد+1])) {
            hi = 0;
        } وإلا إذا (مٿاهون < array[بنياد+hi] || (مٿاهون==array[بنياد+hi] && هيٺيون<array[بنياد+hi+1])) {
            لكل (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + بنياد;
                إذا (i == lo) {
                    اكسر;  // Done!
                } وإلا إذا (مٿاهون < array[iabs] || (مٿاهون==array[iabs] && هيٺيون<array[iabs+1])) {
                    hi = i;
                } وإلا {
                    lo = i;
                }
            }
        } وإلا {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        أرجع ((hi+(بنياد<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_ڳڻپ(ثابت USerialized_سيٽ* سيٽ) {
    إذا(سيٽ==خالي_اشارو) {
        أرجع 0;
    }

    أرجع (سيٽ->bmp_ڊيگهائي+(سيٽ->length-سيٽ->bmp_ڊيگهائي)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ثابت USerialized_سيٽ* سيٽ, int32_t range_اشاريو,
                        UChar32* p_شروعات, UChar32* p_پڇاڙي) {
    ثابت uint16_t* array;
    int32_t bmp_ڊيگهائي, length;

    إذا(سيٽ==خالي_اشارو || range_اشاريو<0 || p_شروعات==خالي_اشارو || p_پڇاڙي==خالي_اشارو) {
        أرجع خطأ;
    }

    array=سيٽ->array;
    length=سيٽ->length;
    bmp_ڊيگهائي=سيٽ->bmp_ڊيگهائي;

    range_اشاريو*=2; /* address start/limit pairs */
    إذا(range_اشاريو<bmp_ڊيگهائي) {
        *p_شروعات=array[range_اشاريو++];
        إذا(range_اشاريو<bmp_ڊيگهائي) {
            *p_پڇاڙي=array[range_اشاريو]-1;
        } وإلا إذا(range_اشاريو<length) {
            *p_پڇاڙي=((((int32_t)array[range_اشاريو])<<16)|array[range_اشاريو+1])-1;
        } وإلا {
            *p_پڇاڙي=0x10ffff;
        }
        أرجع سچ;
    } وإلا {
        range_اشاريو-=bmp_ڊيگهائي;
        range_اشاريو*=2; /* address pairs of pairs of units */
        length-=bmp_ڊيگهائي;
        إذا(range_اشاريو<length) {
            array+=bmp_ڊيگهائي;
            *p_شروعات=(((int32_t)array[range_اشاريو])<<16)|array[range_اشاريو+1];
            range_اشاريو+=2;
            إذا(range_اشاريو<length) {
                *p_پڇاڙي=((((int32_t)array[range_اشاريو])<<16)|array[range_اشاريو+1])-1;
            } وإلا {
                *p_پڇاڙي=0x10ffff;
            }
            أرجع سچ;
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
