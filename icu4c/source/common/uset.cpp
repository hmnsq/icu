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
uset_open_خالی() {
    أرجع (USet*) جديد Unicode_مجموعه();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 شروع, UChar32 انتها) {
    أرجع (USet*) جديد Unicode_مجموعه(شروع, انتها);
}

U_CAPI فراغ U_EXPORT2
uset_close(USet* مجموعه) {
    حذف (Unicode_مجموعه*) مجموعه;
}

U_CAPI USet * U_EXPORT2
uset_clone(ثابت USet *مجموعه) {
    أرجع (USet*) (((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ثابت USet *مجموعه) {
    أرجع ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::isFrozen();
}

U_CAPI فراغ U_EXPORT2
uset_freeze(USet *مجموعه) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ثابت USet *مجموعه) {
    أرجع (USet*) (((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::cloneAsThawed());
}

U_CAPI فراغ U_EXPORT2
uset_مجموعه(USet* مجموعه,
     UChar32 شروع, UChar32 انتها) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::مجموعه(شروع, انتها);
}

U_CAPI فراغ U_EXPORT2
uset_جمع_کن_All(USet* مجموعه, ثابت USet *additional_مجموعه) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::جمع_کن_All(*((ثابت Unicode_مجموعه*)additional_مجموعه));
}

U_CAPI فراغ U_EXPORT2
uset_جمع_کن(USet* مجموعه, UChar32 c) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::جمع_کن(c);
}

U_CAPI فراغ U_EXPORT2
uset_جمع_کن_Range(USet* مجموعه, UChar32 شروع, UChar32 انتها) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::جمع_کن(شروع, انتها);    
}

U_CAPI فراغ U_EXPORT2
uset_جمع_کن_String(USet* مجموعه, ثابت char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::جمع_کن(s);
}

U_CAPI فراغ U_EXPORT2
uset_جمع_کن_All_کد_Points(USet* مجموعه, ثابت char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::جمع_کن_All(s);
}

U_CAPI فراغ U_EXPORT2
uset_remove(USet* مجموعه, UChar32 c) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::remove(c);
}

U_CAPI فراغ U_EXPORT2
uset_removeRange(USet* مجموعه, UChar32 شروع, UChar32 انتها) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::remove(شروع, انتها);
}

U_CAPI فراغ U_EXPORT2
uset_removeString(USet* مجموعه, ثابت char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::remove(s);
}

U_CAPI فراغ U_EXPORT2
uset_remove_All_کد_Points(USet *مجموعه, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::removeAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_removeAll(USet* مجموعه, ثابت USet* remove) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::removeAll(*(ثابت Unicode_مجموعه*)remove);
}

U_CAPI فراغ U_EXPORT2
uset_retain(USet* مجموعه, UChar32 شروع, UChar32 انتها) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::retain(شروع, انتها);
}

U_CAPI فراغ U_EXPORT2
uset_retainString(USet *مجموعه, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::retain(s);
}

U_CAPI فراغ U_EXPORT2
uset_retain_All_کد_Points(USet *مجموعه, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::retainAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_retainAll(USet* مجموعه, ثابت USet* retain) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::retainAll(*(ثابت Unicode_مجموعه*)retain);
}

U_CAPI فراغ U_EXPORT2
uset_compact(USet* مجموعه) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::compact();
}

U_CAPI فراغ U_EXPORT2
uset_complement(USet* مجموعه) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::complement();
}

U_CAPI فراغ U_EXPORT2
uset_complementRange(USet *مجموعه, UChar32 شروع, UChar32 انتها) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::complement(شروع, انتها);
}

U_CAPI فراغ U_EXPORT2
uset_complementString(USet *مجموعه, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::complement(s);
}

U_CAPI فراغ U_EXPORT2
uset_complement_All_کد_Points(USet *مجموعه, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::complementAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_complementAll(USet* مجموعه, ثابت USet* complement) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::complementAll(*(ثابت Unicode_مجموعه*)complement);
}

U_CAPI فراغ U_EXPORT2
uset_clear(USet* مجموعه) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::clear();
}

U_CAPI فراغ U_EXPORT2
uset_remove_All_رشتهها(USet* مجموعه) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::remove_All_رشتهها();
}

U_CAPI UBool U_EXPORT2
uset_is_خالی(ثابت USet* مجموعه) {
    أرجع ((ثابت Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::is_خالی();
}

U_CAPI UBool U_EXPORT2
uset_has_رشتهها(ثابت USet* مجموعه) {
    أرجع ((ثابت Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::has_رشتهها();
}

U_CAPI UBool U_EXPORT2
uset_شامل_است(ثابت USet* مجموعه, UChar32 c) {
    أرجع ((ثابت Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::شامل_است(c);
}

U_CAPI UBool U_EXPORT2
uset_شامل_است_Range(ثابت USet* مجموعه, UChar32 شروع, UChar32 انتها) {
    أرجع ((ثابت Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::شامل_است(شروع, انتها);
}

U_CAPI UBool U_EXPORT2
uset_شامل_است_String(ثابت USet* مجموعه, ثابت char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    أرجع ((ثابت Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::شامل_است(s);
}

U_CAPI UBool U_EXPORT2
uset_شامل_است_All(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_مجموعه*) set1)->Unicode_مجموعه::شامل_است_All(* (ثابت Unicode_مجموعه*) set2);
}

U_CAPI UBool U_EXPORT2
uset_شامل_است_All_کد_Points(ثابت USet* مجموعه, ثابت char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    أرجع ((ثابت Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::شامل_است_All(s);
}

U_CAPI UBool U_EXPORT2
uset_شامل_است_None(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_مجموعه*) set1)->Unicode_مجموعه::شامل_است_None(* (ثابت Unicode_مجموعه*) set2);
}

U_CAPI UBool U_EXPORT2
uset_شامل_است_Some(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_مجموعه*) set1)->Unicode_مجموعه::شامل_است_Some(* (ثابت Unicode_مجموعه*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_بازه(ثابت USet *مجموعه, ثابت char16_t *s, int32_t length, USet_بازه_Condition بازه_Condition) {
    أرجع ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::بازه(s, length, بازه_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_بازه_پشت(ثابت USet *مجموعه, ثابت char16_t *s, int32_t length, USet_بازه_Condition بازه_Condition) {
    أرجع ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::بازه_پشت(s, length, بازه_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_بازه_UTF8(ثابت USet *مجموعه, ثابت حرف *s, int32_t length, USet_بازه_Condition بازه_Condition) {
    أرجع ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::بازه_UTF8(s, length, بازه_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_بازه_پشت_UTF8(ثابت USet *مجموعه, ثابت حرف *s, int32_t length, USet_بازه_Condition بازه_Condition) {
    أرجع ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::بازه_پشت_UTF8(s, length, بازه_Condition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ثابت USet* set1, ثابت USet* set2) {
    أرجع *(ثابت Unicode_مجموعه*)set1 == *(ثابت Unicode_مجموعه*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_شاخص_از(ثابت USet* مجموعه, UChar32 c) {
    أرجع ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::شاخص_از(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ثابت USet* مجموعه, int32_t شاخص) {
    أرجع ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::charAt(شاخص);
}

U_CAPI int32_t U_EXPORT2
uset_اندازه(ثابت USet* مجموعه) {
    أرجع ((ثابت Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::size();
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
    مضمن ساكن int32_t get_String_شمار(ثابت Unicode_مجموعه& مجموعه) {
        أرجع مجموعه.رشتهها_اندازه();
    }
    مضمن ساكن ثابت UnicodeString* getString(ثابت Unicode_مجموعه& مجموعه U_LIFETIME_BOUND,
                                                 int32_t i) {
        أرجع مجموعه.getString(i);
    }
خاص:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_انتها

U_CAPI int32_t U_EXPORT2
uset_get_Range_شمار(ثابت USet *مجموعه) {
    أرجع ((ثابت Unicode_مجموعه *)مجموعه)->Unicode_مجموعه::get_Range_شمار();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_شمار(ثابت USet *uset) {
    ثابت Unicode_مجموعه &مجموعه = *(ثابت Unicode_مجموعه *)uset;
    أرجع USetAccess::get_String_شمار(مجموعه);
}

U_CAPI int32_t U_EXPORT2
uset_get_قلم_شمار(ثابت USet* uset) {
    ثابت Unicode_مجموعه& مجموعه = *(ثابت Unicode_مجموعه*)uset;
    أرجع مجموعه.get_Range_شمار() + USetAccess::get_String_شمار(مجموعه);
}

U_CAPI ثابت UChar* U_EXPORT2
uset_getString(ثابت USet* uset U_LIFETIME_BOUND, int32_t شاخص, int32_t* p_درازی) {
    إذا (p_درازی == اشاره_تهی) { أرجع اشاره_تهی; }
    ثابت Unicode_مجموعه &مجموعه = *(ثابت Unicode_مجموعه *)uset;
    int32_t count = USetAccess::get_String_شمار(مجموعه);
    إذا (شاخص < 0 || count <= شاخص) {
        *p_درازی = 0;
        أرجع اشاره_تهی;
    }
    ثابت UnicodeString *s = USetAccess::getString(مجموعه, شاخص);
    *p_درازی = s->length();
    أرجع مقصد_UChar_Ptr(s->get_میانگیر());
}

U_CAPI int32_t U_EXPORT2
uset_get_قلم(ثابت USet* uset, int32_t قلم_شاخص,
             UChar32* شروع, UChar32* انتها,
             char16_t* str, int32_t str_ظرفیت,
             UError_کد* ec) {
    إذا (U_FAILURE(*ec)) أرجع 0;
    ثابت Unicode_مجموعه& مجموعه = *(ثابت Unicode_مجموعه*)uset;
    int32_t range_شمار;

    إذا (قلم_شاخص < 0) {
        *ec = U_ILLEGAL_آرگومان_ERROR;
        أرجع -1;
    } وإلا إذا (قلم_شاخص < (range_شمار = مجموعه.get_Range_شمار())) {
        *شروع = مجموعه.get_Range_شروع(قلم_شاخص);
        *انتها = مجموعه.get_Range_انتها(قلم_شاخص);
        أرجع 0;
    } وإلا {
        قلم_شاخص -= range_شمار;
        إذا (قلم_شاخص < USetAccess::get_String_شمار(مجموعه)) {
            ثابت UnicodeString* s = USetAccess::getString(مجموعه, قلم_شاخص);
            أرجع s->extract(str, str_ظرفیت, *ec);
        } وإلا {
            *ec = U_شاخص_OUTOFBOUNDS_ERROR;
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
uset_serialize(ثابت USet* مجموعه, uint16_t* dest, int32_t dest_ظرفیت, UError_کد* ec) {
    إذا (ec==اشاره_تهی || U_FAILURE(*ec)) {
        أرجع 0;
    }

    أرجع ((ثابت Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::serialize(dest, dest_ظرفیت,* ec);
}

U_CAPI UBool U_EXPORT2
uset_get_Serialized_مجموعه(USerialized_مجموعه* fill_مجموعه, ثابت uint16_t* src, int32_t src_درازی) {
    int32_t length;

    إذا(fill_مجموعه==اشاره_تهی) {
        أرجع خطأ;
    }
    إذا(src==اشاره_تهی || src_درازی<=0) {
        fill_مجموعه->length=fill_مجموعه->bmp_درازی=0;
        أرجع خطأ;
    }

    length=*src++;
    إذا(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        إذا(src_درازی<(2+length)) {
            fill_مجموعه->length=fill_مجموعه->bmp_درازی=0;
            أرجع خطأ;
        }
        fill_مجموعه->bmp_درازی=*src++;
    } وإلا {
        /* only BMP values */
        إذا(src_درازی<(1+length)) {
            fill_مجموعه->length=fill_مجموعه->bmp_درازی=0;
            أرجع خطأ;
        }
        fill_مجموعه->bmp_درازی=length;
    }
    fill_مجموعه->array=src;
    fill_مجموعه->length=length;
    أرجع درست;
}

U_CAPI فراغ U_EXPORT2
uset_مجموعه_Serialized_مقصد_One(USerialized_مجموعه* fill_مجموعه, UChar32 c) {
    إذا(fill_مجموعه==اشاره_تهی || (uint32_t)c>0x10ffff) {
        أرجع;
    }

    fill_مجموعه->array=fill_مجموعه->staticArray;
    إذا(c<0xffff) {
        fill_مجموعه->bmp_درازی=fill_مجموعه->length=2;
        fill_مجموعه->staticArray[0]=(uint16_t)c;
        fill_مجموعه->staticArray[1]=(uint16_t)c+1;
    } وإلا إذا(c==0xffff) {
        fill_مجموعه->bmp_درازی=1;
        fill_مجموعه->length=3;
        fill_مجموعه->staticArray[0]=0xffff;
        fill_مجموعه->staticArray[1]=1;
        fill_مجموعه->staticArray[2]=0;
    } وإلا إذا(c<0x10ffff) {
        fill_مجموعه->bmp_درازی=0;
        fill_مجموعه->length=4;
        fill_مجموعه->staticArray[0]=(uint16_t)(c>>16);
        fill_مجموعه->staticArray[1]=(uint16_t)c;
        ++c;
        fill_مجموعه->staticArray[2]=(uint16_t)(c>>16);
        fill_مجموعه->staticArray[3]=(uint16_t)c;
    } وإلا /* c==0x10ffff */ {
        fill_مجموعه->bmp_درازی=0;
        fill_مجموعه->length=2;
        fill_مجموعه->staticArray[0]=0x10;
        fill_مجموعه->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serialized_شامل_است(ثابت USerialized_مجموعه* مجموعه, UChar32 c) {
    ثابت uint16_t* array;

    إذا(مجموعه==اشاره_تهی || (uint32_t)c>0x10ffff) {
        أرجع خطأ;
    }

    array=مجموعه->array;
    إذا(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = مجموعه->bmp_درازی-1;
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
        uint16_t بالا=(uint16_t)(c>>16), کم=(uint16_t)c;
        int32_t پایه = مجموعه->bmp_درازی;
        int32_t lo = 0;
        int32_t hi = مجموعه->length - 2 - پایه;
        إذا (بالا < array[پایه] || (بالا==array[پایه] && کم<array[پایه+1])) {
            hi = 0;
        } وإلا إذا (بالا < array[پایه+hi] || (بالا==array[پایه+hi] && کم<array[پایه+hi+1])) {
            لكل (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + پایه;
                إذا (i == lo) {
                    اكسر;  // Done!
                } وإلا إذا (بالا < array[iabs] || (بالا==array[iabs] && کم<array[iabs+1])) {
                    hi = i;
                } وإلا {
                    lo = i;
                }
            }
        } وإلا {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        أرجع ((hi+(پایه<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_شمار(ثابت USerialized_مجموعه* مجموعه) {
    إذا(مجموعه==اشاره_تهی) {
        أرجع 0;
    }

    أرجع (مجموعه->bmp_درازی+(مجموعه->length-مجموعه->bmp_درازی)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ثابت USerialized_مجموعه* مجموعه, int32_t range_شاخص,
                        UChar32* p_شروع, UChar32* p_انتها) {
    ثابت uint16_t* array;
    int32_t bmp_درازی, length;

    إذا(مجموعه==اشاره_تهی || range_شاخص<0 || p_شروع==اشاره_تهی || p_انتها==اشاره_تهی) {
        أرجع خطأ;
    }

    array=مجموعه->array;
    length=مجموعه->length;
    bmp_درازی=مجموعه->bmp_درازی;

    range_شاخص*=2; /* address start/limit pairs */
    إذا(range_شاخص<bmp_درازی) {
        *p_شروع=array[range_شاخص++];
        إذا(range_شاخص<bmp_درازی) {
            *p_انتها=array[range_شاخص]-1;
        } وإلا إذا(range_شاخص<length) {
            *p_انتها=((((int32_t)array[range_شاخص])<<16)|array[range_شاخص+1])-1;
        } وإلا {
            *p_انتها=0x10ffff;
        }
        أرجع درست;
    } وإلا {
        range_شاخص-=bmp_درازی;
        range_شاخص*=2; /* address pairs of pairs of units */
        length-=bmp_درازی;
        إذا(range_شاخص<length) {
            array+=bmp_درازی;
            *p_شروع=(((int32_t)array[range_شاخص])<<16)|array[range_شاخص+1];
            range_شاخص+=2;
            إذا(range_شاخص<length) {
                *p_انتها=((((int32_t)array[range_شاخص])<<16)|array[range_شاخص+1])-1;
            } وإلا {
                *p_انتها=0x10ffff;
            }
            أرجع درست;
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
