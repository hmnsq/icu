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
uset_open_خالی_تھاواں() {
    أرجع (USet*) جديد Unicode_سیٹ();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 شروع, UChar32 سرا) {
    أرجع (USet*) جديد Unicode_سیٹ(شروع, سرا);
}

U_CAPI فراغ U_EXPORT2
uset_close(USet* سیٹ) {
    مٹاؤ (Unicode_سیٹ*) سیٹ;
}

U_CAPI USet * U_EXPORT2
uset_clone(ثابت USet *سیٹ) {
    أرجع (USet*) (((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ثابت USet *سیٹ) {
    أرجع ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::isFrozen();
}

U_CAPI فراغ U_EXPORT2
uset_freeze(USet *سیٹ) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ثابت USet *سیٹ) {
    أرجع (USet*) (((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::cloneAsThawed());
}

U_CAPI فراغ U_EXPORT2
uset_سیٹ(USet* سیٹ,
     UChar32 شروع, UChar32 سرا) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::سیٹ(شروع, سرا);
}

U_CAPI فراغ U_EXPORT2
uset_جوڑو_All(USet* سیٹ, ثابت USet *additional_سیٹ) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::جوڑو_All(*((ثابت Unicode_سیٹ*)additional_سیٹ));
}

U_CAPI فراغ U_EXPORT2
uset_جوڑو(USet* سیٹ, UChar32 c) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::جوڑو(c);
}

U_CAPI فراغ U_EXPORT2
uset_جوڑو_Range(USet* سیٹ, UChar32 شروع, UChar32 سرا) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::جوڑو(شروع, سرا);    
}

U_CAPI فراغ U_EXPORT2
uset_جوڑو_String(USet* سیٹ, ثابت char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::جوڑو(s);
}

U_CAPI فراغ U_EXPORT2
uset_جوڑو_All_کوڈ_Points(USet* سیٹ, ثابت char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::جوڑو_All(s);
}

U_CAPI فراغ U_EXPORT2
uset_remove(USet* سیٹ, UChar32 c) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::remove(c);
}

U_CAPI فراغ U_EXPORT2
uset_removeRange(USet* سیٹ, UChar32 شروع, UChar32 سرا) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::remove(شروع, سرا);
}

U_CAPI فراغ U_EXPORT2
uset_removeString(USet* سیٹ, ثابت char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::remove(s);
}

U_CAPI فراغ U_EXPORT2
uset_remove_All_کوڈ_Points(USet *سیٹ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::removeAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_removeAll(USet* سیٹ, ثابت USet* remove) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::removeAll(*(ثابت Unicode_سیٹ*)remove);
}

U_CAPI فراغ U_EXPORT2
uset_retain(USet* سیٹ, UChar32 شروع, UChar32 سرا) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::retain(شروع, سرا);
}

U_CAPI فراغ U_EXPORT2
uset_retainString(USet *سیٹ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::retain(s);
}

U_CAPI فراغ U_EXPORT2
uset_retain_All_کوڈ_Points(USet *سیٹ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::retainAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_retainAll(USet* سیٹ, ثابت USet* retain) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::retainAll(*(ثابت Unicode_سیٹ*)retain);
}

U_CAPI فراغ U_EXPORT2
uset_compact(USet* سیٹ) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::compact();
}

U_CAPI فراغ U_EXPORT2
uset_complement(USet* سیٹ) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::complement();
}

U_CAPI فراغ U_EXPORT2
uset_complementRange(USet *سیٹ, UChar32 شروع, UChar32 سرا) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::complement(شروع, سرا);
}

U_CAPI فراغ U_EXPORT2
uset_complementString(USet *سیٹ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::complement(s);
}

U_CAPI فراغ U_EXPORT2
uset_complement_All_کوڈ_Points(USet *سیٹ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::complementAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_complementAll(USet* سیٹ, ثابت USet* complement) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::complementAll(*(ثابت Unicode_سیٹ*)complement);
}

U_CAPI فراغ U_EXPORT2
uset_clear(USet* سیٹ) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::clear();
}

U_CAPI فراغ U_EXPORT2
uset_remove_All_لڑیاں(USet* سیٹ) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::remove_All_لڑیاں();
}

U_CAPI UBool U_EXPORT2
uset_is_خالی_تھاواں(ثابت USet* سیٹ) {
    أرجع ((ثابت Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::is_خالی_تھاواں();
}

U_CAPI UBool U_EXPORT2
uset_has_لڑیاں(ثابت USet* سیٹ) {
    أرجع ((ثابت Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::has_لڑیاں();
}

U_CAPI UBool U_EXPORT2
uset_شامل_اے(ثابت USet* سیٹ, UChar32 c) {
    أرجع ((ثابت Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::شامل_اے(c);
}

U_CAPI UBool U_EXPORT2
uset_شامل_اے_Range(ثابت USet* سیٹ, UChar32 شروع, UChar32 سرا) {
    أرجع ((ثابت Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::شامل_اے(شروع, سرا);
}

U_CAPI UBool U_EXPORT2
uset_شامل_اے_String(ثابت USet* سیٹ, ثابت char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    أرجع ((ثابت Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::شامل_اے(s);
}

U_CAPI UBool U_EXPORT2
uset_شامل_اے_All(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_سیٹ*) set1)->Unicode_سیٹ::شامل_اے_All(* (ثابت Unicode_سیٹ*) set2);
}

U_CAPI UBool U_EXPORT2
uset_شامل_اے_All_کوڈ_Points(ثابت USet* سیٹ, ثابت char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    أرجع ((ثابت Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::شامل_اے_All(s);
}

U_CAPI UBool U_EXPORT2
uset_شامل_اے_None(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_سیٹ*) set1)->Unicode_سیٹ::شامل_اے_None(* (ثابت Unicode_سیٹ*) set2);
}

U_CAPI UBool U_EXPORT2
uset_شامل_اے_Some(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_سیٹ*) set1)->Unicode_سیٹ::شامل_اے_Some(* (ثابت Unicode_سیٹ*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_وسعت(ثابت USet *سیٹ, ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) {
    أرجع ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::وسعت(s, length, وسعت_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_وسعت_پٹھ(ثابت USet *سیٹ, ثابت char16_t *s, int32_t length, USet_وسعت_Condition وسعت_Condition) {
    أرجع ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::وسعت_پٹھ(s, length, وسعت_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_وسعت_UTF8(ثابت USet *سیٹ, ثابت حرف *s, int32_t length, USet_وسعت_Condition وسعت_Condition) {
    أرجع ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::وسعت_UTF8(s, length, وسعت_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_وسعت_پٹھ_UTF8(ثابت USet *سیٹ, ثابت حرف *s, int32_t length, USet_وسعت_Condition وسعت_Condition) {
    أرجع ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::وسعت_پٹھ_UTF8(s, length, وسعت_Condition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ثابت USet* set1, ثابت USet* set2) {
    أرجع *(ثابت Unicode_سیٹ*)set1 == *(ثابت Unicode_سیٹ*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_اشاریہ_دا(ثابت USet* سیٹ, UChar32 c) {
    أرجع ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::اشاریہ_دا(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ثابت USet* سیٹ, int32_t اشاریہ) {
    أرجع ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::charAt(اشاریہ);
}

U_CAPI int32_t U_EXPORT2
uset_ناپ(ثابت USet* سیٹ) {
    أرجع ((ثابت Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::size();
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
    مضمن ساكن int32_t get_String_گنتی(ثابت Unicode_سیٹ& سیٹ) {
        أرجع سیٹ.لڑیاں_ناپ();
    }
    مضمن ساكن ثابت UnicodeString* getString(ثابت Unicode_سیٹ& سیٹ U_LIFETIME_BOUND,
                                                 int32_t i) {
        أرجع سیٹ.getString(i);
    }
خاص:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_سرا

U_CAPI int32_t U_EXPORT2
uset_get_Range_گنتی(ثابت USet *سیٹ) {
    أرجع ((ثابت Unicode_سیٹ *)سیٹ)->Unicode_سیٹ::get_Range_گنتی();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_گنتی(ثابت USet *uset) {
    ثابت Unicode_سیٹ &سیٹ = *(ثابت Unicode_سیٹ *)uset;
    أرجع USetAccess::get_String_گنتی(سیٹ);
}

U_CAPI int32_t U_EXPORT2
uset_get_شے_گنتی(ثابت USet* uset) {
    ثابت Unicode_سیٹ& سیٹ = *(ثابت Unicode_سیٹ*)uset;
    أرجع سیٹ.get_Range_گنتی() + USetAccess::get_String_گنتی(سیٹ);
}

U_CAPI ثابت UChar* U_EXPORT2
uset_getString(ثابت USet* uset U_LIFETIME_BOUND, int32_t اشاریہ, int32_t* p_لمائی) {
    إذا (p_لمائی == خالی_سوچک) { أرجع خالی_سوچک; }
    ثابت Unicode_سیٹ &سیٹ = *(ثابت Unicode_سیٹ *)uset;
    int32_t count = USetAccess::get_String_گنتی(سیٹ);
    إذا (اشاریہ < 0 || count <= اشاریہ) {
        *p_لمائی = 0;
        أرجع خالی_سوچک;
    }
    ثابت UnicodeString *s = USetAccess::getString(سیٹ, اشاریہ);
    *p_لمائی = s->length();
    أرجع منزل_UChar_Ptr(s->get_بفر());
}

U_CAPI int32_t U_EXPORT2
uset_get_شے(ثابت USet* uset, int32_t شے_اشاریہ,
             UChar32* شروع, UChar32* سرا,
             char16_t* str, int32_t str_سمرتھا,
             UError_کوڈ* ec) {
    إذا (U_FAILURE(*ec)) أرجع 0;
    ثابت Unicode_سیٹ& سیٹ = *(ثابت Unicode_سیٹ*)uset;
    int32_t range_گنتی;

    إذا (شے_اشاریہ < 0) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        أرجع -1;
    } وإلا إذا (شے_اشاریہ < (range_گنتی = سیٹ.get_Range_گنتی())) {
        *شروع = سیٹ.get_Range_شروع(شے_اشاریہ);
        *سرا = سیٹ.get_Range_سرا(شے_اشاریہ);
        أرجع 0;
    } وإلا {
        شے_اشاریہ -= range_گنتی;
        إذا (شے_اشاریہ < USetAccess::get_String_گنتی(سیٹ)) {
            ثابت UnicodeString* s = USetAccess::getString(سیٹ, شے_اشاریہ);
            أرجع s->extract(str, str_سمرتھا, *ec);
        } وإلا {
            *ec = U_اشاریہ_OUTOFBOUNDS_ERROR;
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
uset_serialize(ثابت USet* سیٹ, uint16_t* dest, int32_t dest_سمرتھا, UError_کوڈ* ec) {
    إذا (ec==خالی_سوچک || U_FAILURE(*ec)) {
        أرجع 0;
    }

    أرجع ((ثابت Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::serialize(dest, dest_سمرتھا,* ec);
}

U_CAPI UBool U_EXPORT2
uset_get_Serialized_سیٹ(USerialized_سیٹ* fill_سیٹ, ثابت uint16_t* src, int32_t src_لمائی) {
    int32_t length;

    إذا(fill_سیٹ==خالی_سوچک) {
        أرجع خطأ;
    }
    إذا(src==خالی_سوچک || src_لمائی<=0) {
        fill_سیٹ->length=fill_سیٹ->bmp_لمائی=0;
        أرجع خطأ;
    }

    length=*src++;
    إذا(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        إذا(src_لمائی<(2+length)) {
            fill_سیٹ->length=fill_سیٹ->bmp_لمائی=0;
            أرجع خطأ;
        }
        fill_سیٹ->bmp_لمائی=*src++;
    } وإلا {
        /* only BMP values */
        إذا(src_لمائی<(1+length)) {
            fill_سیٹ->length=fill_سیٹ->bmp_لمائی=0;
            أرجع خطأ;
        }
        fill_سیٹ->bmp_لمائی=length;
    }
    fill_سیٹ->array=src;
    fill_سیٹ->length=length;
    أرجع سچ;
}

U_CAPI فراغ U_EXPORT2
uset_سیٹ_Serialized_منزل_One(USerialized_سیٹ* fill_سیٹ, UChar32 c) {
    إذا(fill_سیٹ==خالی_سوچک || (uint32_t)c>0x10ffff) {
        أرجع;
    }

    fill_سیٹ->array=fill_سیٹ->staticArray;
    إذا(c<0xffff) {
        fill_سیٹ->bmp_لمائی=fill_سیٹ->length=2;
        fill_سیٹ->staticArray[0]=(uint16_t)c;
        fill_سیٹ->staticArray[1]=(uint16_t)c+1;
    } وإلا إذا(c==0xffff) {
        fill_سیٹ->bmp_لمائی=1;
        fill_سیٹ->length=3;
        fill_سیٹ->staticArray[0]=0xffff;
        fill_سیٹ->staticArray[1]=1;
        fill_سیٹ->staticArray[2]=0;
    } وإلا إذا(c<0x10ffff) {
        fill_سیٹ->bmp_لمائی=0;
        fill_سیٹ->length=4;
        fill_سیٹ->staticArray[0]=(uint16_t)(c>>16);
        fill_سیٹ->staticArray[1]=(uint16_t)c;
        ++c;
        fill_سیٹ->staticArray[2]=(uint16_t)(c>>16);
        fill_سیٹ->staticArray[3]=(uint16_t)c;
    } وإلا /* c==0x10ffff */ {
        fill_سیٹ->bmp_لمائی=0;
        fill_سیٹ->length=2;
        fill_سیٹ->staticArray[0]=0x10;
        fill_سیٹ->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serialized_شامل_اے(ثابت USerialized_سیٹ* سیٹ, UChar32 c) {
    ثابت uint16_t* array;

    إذا(سیٹ==خالی_سوچک || (uint32_t)c>0x10ffff) {
        أرجع خطأ;
    }

    array=سیٹ->array;
    إذا(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = سیٹ->bmp_لمائی-1;
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
        uint16_t بلند=(uint16_t)(c>>16), کم=(uint16_t)c;
        int32_t ادھار = سیٹ->bmp_لمائی;
        int32_t lo = 0;
        int32_t hi = سیٹ->length - 2 - ادھار;
        إذا (بلند < array[ادھار] || (بلند==array[ادھار] && کم<array[ادھار+1])) {
            hi = 0;
        } وإلا إذا (بلند < array[ادھار+hi] || (بلند==array[ادھار+hi] && کم<array[ادھار+hi+1])) {
            لكل (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + ادھار;
                إذا (i == lo) {
                    اكسر;  // Done!
                } وإلا إذا (بلند < array[iabs] || (بلند==array[iabs] && کم<array[iabs+1])) {
                    hi = i;
                } وإلا {
                    lo = i;
                }
            }
        } وإلا {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        أرجع ((hi+(ادھار<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_گنتی(ثابت USerialized_سیٹ* سیٹ) {
    إذا(سیٹ==خالی_سوچک) {
        أرجع 0;
    }

    أرجع (سیٹ->bmp_لمائی+(سیٹ->length-سیٹ->bmp_لمائی)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ثابت USerialized_سیٹ* سیٹ, int32_t range_اشاریہ,
                        UChar32* p_شروع, UChar32* p_سرا) {
    ثابت uint16_t* array;
    int32_t bmp_لمائی, length;

    إذا(سیٹ==خالی_سوچک || range_اشاریہ<0 || p_شروع==خالی_سوچک || p_سرا==خالی_سوچک) {
        أرجع خطأ;
    }

    array=سیٹ->array;
    length=سیٹ->length;
    bmp_لمائی=سیٹ->bmp_لمائی;

    range_اشاریہ*=2; /* address start/limit pairs */
    إذا(range_اشاریہ<bmp_لمائی) {
        *p_شروع=array[range_اشاریہ++];
        إذا(range_اشاریہ<bmp_لمائی) {
            *p_سرا=array[range_اشاریہ]-1;
        } وإلا إذا(range_اشاریہ<length) {
            *p_سرا=((((int32_t)array[range_اشاریہ])<<16)|array[range_اشاریہ+1])-1;
        } وإلا {
            *p_سرا=0x10ffff;
        }
        أرجع سچ;
    } وإلا {
        range_اشاریہ-=bmp_لمائی;
        range_اشاریہ*=2; /* address pairs of pairs of units */
        length-=bmp_لمائی;
        إذا(range_اشاریہ<length) {
            array+=bmp_لمائی;
            *p_شروع=(((int32_t)array[range_اشاریہ])<<16)|array[range_اشاریہ+1];
            range_اشاریہ+=2;
            إذا(range_اشاریہ<length) {
                *p_سرا=((((int32_t)array[range_اشاریہ])<<16)|array[range_اشاریہ+1])-1;
            } وإلا {
                *p_سرا=0x10ffff;
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
