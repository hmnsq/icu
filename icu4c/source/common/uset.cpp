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
uset_open_تشه() {
    أرجع (USet*) جديد Unicode_سېټ();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 پیل, UChar32 پای_ټکی) {
    أرجع (USet*) جديد Unicode_سېټ(پیل, پای_ټکی);
}

U_CAPI فراغ U_EXPORT2
uset_close(USet* سېټ) {
    ړنګول (Unicode_سېټ*) سېټ;
}

U_CAPI USet * U_EXPORT2
uset_clone(ثابت USet *سېټ) {
    أرجع (USet*) (((Unicode_سېټ*) سېټ)->Unicode_سېټ::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ثابت USet *سېټ) {
    أرجع ((Unicode_سېټ*) سېټ)->Unicode_سېټ::isFrozen();
}

U_CAPI فراغ U_EXPORT2
uset_freeze(USet *سېټ) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ثابت USet *سېټ) {
    أرجع (USet*) (((Unicode_سېټ*) سېټ)->Unicode_سېټ::cloneAsThawed());
}

U_CAPI فراغ U_EXPORT2
uset_سېټ(USet* سېټ,
     UChar32 پیل, UChar32 پای_ټکی) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::سېټ(پیل, پای_ټکی);
}

U_CAPI فراغ U_EXPORT2
uset_جمع_کړه_All(USet* سېټ, ثابت USet *additional_سېټ) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::جمع_کړه_All(*((ثابت Unicode_سېټ*)additional_سېټ));
}

U_CAPI فراغ U_EXPORT2
uset_جمع_کړه(USet* سېټ, UChar32 c) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::جمع_کړه(c);
}

U_CAPI فراغ U_EXPORT2
uset_جمع_کړه_Range(USet* سېټ, UChar32 پیل, UChar32 پای_ټکی) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::جمع_کړه(پیل, پای_ټکی);    
}

U_CAPI فراغ U_EXPORT2
uset_جمع_کړه_String(USet* سېټ, ثابت char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::جمع_کړه(s);
}

U_CAPI فراغ U_EXPORT2
uset_جمع_کړه_All_کوډ_Points(USet* سېټ, ثابت char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::جمع_کړه_All(s);
}

U_CAPI فراغ U_EXPORT2
uset_remove(USet* سېټ, UChar32 c) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::remove(c);
}

U_CAPI فراغ U_EXPORT2
uset_removeRange(USet* سېټ, UChar32 پیل, UChar32 پای_ټکی) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::remove(پیل, پای_ټکی);
}

U_CAPI فراغ U_EXPORT2
uset_removeString(USet* سېټ, ثابت char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::remove(s);
}

U_CAPI فراغ U_EXPORT2
uset_remove_All_کوډ_Points(USet *سېټ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::removeAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_removeAll(USet* سېټ, ثابت USet* remove) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::removeAll(*(ثابت Unicode_سېټ*)remove);
}

U_CAPI فراغ U_EXPORT2
uset_retain(USet* سېټ, UChar32 پیل, UChar32 پای_ټکی) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::retain(پیل, پای_ټکی);
}

U_CAPI فراغ U_EXPORT2
uset_retainString(USet *سېټ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::retain(s);
}

U_CAPI فراغ U_EXPORT2
uset_retain_All_کوډ_Points(USet *سېټ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::retainAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_retainAll(USet* سېټ, ثابت USet* retain) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::retainAll(*(ثابت Unicode_سېټ*)retain);
}

U_CAPI فراغ U_EXPORT2
uset_compact(USet* سېټ) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::compact();
}

U_CAPI فراغ U_EXPORT2
uset_complement(USet* سېټ) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::complement();
}

U_CAPI فراغ U_EXPORT2
uset_complementRange(USet *سېټ, UChar32 پیل, UChar32 پای_ټکی) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::complement(پیل, پای_ټکی);
}

U_CAPI فراغ U_EXPORT2
uset_complementString(USet *سېټ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::complement(s);
}

U_CAPI فراغ U_EXPORT2
uset_complement_All_کوډ_Points(USet *سېټ, ثابت char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::complementAll(s);
}

U_CAPI فراغ U_EXPORT2
uset_complementAll(USet* سېټ, ثابت USet* complement) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::complementAll(*(ثابت Unicode_سېټ*)complement);
}

U_CAPI فراغ U_EXPORT2
uset_clear(USet* سېټ) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::clear();
}

U_CAPI فراغ U_EXPORT2
uset_remove_All_مزي_ګانې(USet* سېټ) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::remove_All_مزي_ګانې();
}

U_CAPI UBool U_EXPORT2
uset_is_تشه(ثابت USet* سېټ) {
    أرجع ((ثابت Unicode_سېټ*) سېټ)->Unicode_سېټ::is_تشه();
}

U_CAPI UBool U_EXPORT2
uset_has_مزي_ګانې(ثابت USet* سېټ) {
    أرجع ((ثابت Unicode_سېټ*) سېټ)->Unicode_سېټ::has_مزي_ګانې();
}

U_CAPI UBool U_EXPORT2
uset_لري(ثابت USet* سېټ, UChar32 c) {
    أرجع ((ثابت Unicode_سېټ*) سېټ)->Unicode_سېټ::لري(c);
}

U_CAPI UBool U_EXPORT2
uset_لري_Range(ثابت USet* سېټ, UChar32 پیل, UChar32 پای_ټکی) {
    أرجع ((ثابت Unicode_سېټ*) سېټ)->Unicode_سېټ::لري(پیل, پای_ټکی);
}

U_CAPI UBool U_EXPORT2
uset_لري_String(ثابت USet* سېټ, ثابت char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    أرجع ((ثابت Unicode_سېټ*) سېټ)->Unicode_سېټ::لري(s);
}

U_CAPI UBool U_EXPORT2
uset_لري_All(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_سېټ*) set1)->Unicode_سېټ::لري_All(* (ثابت Unicode_سېټ*) set2);
}

U_CAPI UBool U_EXPORT2
uset_لري_All_کوډ_Points(ثابت USet* سېټ, ثابت char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    أرجع ((ثابت Unicode_سېټ*) سېټ)->Unicode_سېټ::لري_All(s);
}

U_CAPI UBool U_EXPORT2
uset_لري_None(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_سېټ*) set1)->Unicode_سېټ::لري_None(* (ثابت Unicode_سېټ*) set2);
}

U_CAPI UBool U_EXPORT2
uset_لري_Some(ثابت USet* set1, ثابت USet* set2) {
    أرجع ((ثابت Unicode_سېټ*) set1)->Unicode_سېټ::لري_Some(* (ثابت Unicode_سېټ*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_پراختیا(ثابت USet *سېټ, ثابت char16_t *s, int32_t length, USet_پراختیا_Condition پراختیا_Condition) {
    أرجع ((Unicode_سېټ*) سېټ)->Unicode_سېټ::پراختیا(s, length, پراختیا_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_پراختیا_شا(ثابت USet *سېټ, ثابت char16_t *s, int32_t length, USet_پراختیا_Condition پراختیا_Condition) {
    أرجع ((Unicode_سېټ*) سېټ)->Unicode_سېټ::پراختیا_شا(s, length, پراختیا_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_پراختیا_UTF8(ثابت USet *سېټ, ثابت حرف *s, int32_t length, USet_پراختیا_Condition پراختیا_Condition) {
    أرجع ((Unicode_سېټ*) سېټ)->Unicode_سېټ::پراختیا_UTF8(s, length, پراختیا_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_پراختیا_شا_UTF8(ثابت USet *سېټ, ثابت حرف *s, int32_t length, USet_پراختیا_Condition پراختیا_Condition) {
    أرجع ((Unicode_سېټ*) سېټ)->Unicode_سېټ::پراختیا_شا_UTF8(s, length, پراختیا_Condition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ثابت USet* set1, ثابت USet* set2) {
    أرجع *(ثابت Unicode_سېټ*)set1 == *(ثابت Unicode_سېټ*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_شاخص_د(ثابت USet* سېټ, UChar32 c) {
    أرجع ((Unicode_سېټ*) سېټ)->Unicode_سېټ::شاخص_د(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ثابت USet* سېټ, int32_t شاخص) {
    أرجع ((Unicode_سېټ*) سېټ)->Unicode_سېټ::charAt(شاخص);
}

U_CAPI int32_t U_EXPORT2
uset_اندازه(ثابت USet* سېټ) {
    أرجع ((ثابت Unicode_سېټ*) سېټ)->Unicode_سېټ::size();
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
    مضمن ساكن int32_t get_String_شمېر(ثابت Unicode_سېټ& سېټ) {
        أرجع سېټ.مزي_ګانې_اندازه();
    }
    مضمن ساكن ثابت UnicodeString* getString(ثابت Unicode_سېټ& سېټ U_LIFETIME_BOUND,
                                                 int32_t i) {
        أرجع سېټ.getString(i);
    }
خاص:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_پای_ټکی

U_CAPI int32_t U_EXPORT2
uset_get_Range_شمېر(ثابت USet *سېټ) {
    أرجع ((ثابت Unicode_سېټ *)سېټ)->Unicode_سېټ::get_Range_شمېر();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_شمېر(ثابت USet *uset) {
    ثابت Unicode_سېټ &سېټ = *(ثابت Unicode_سېټ *)uset;
    أرجع USetAccess::get_String_شمېر(سېټ);
}

U_CAPI int32_t U_EXPORT2
uset_get_توکی_شمېر(ثابت USet* uset) {
    ثابت Unicode_سېټ& سېټ = *(ثابت Unicode_سېټ*)uset;
    أرجع سېټ.get_Range_شمېر() + USetAccess::get_String_شمېر(سېټ);
}

U_CAPI ثابت UChar* U_EXPORT2
uset_getString(ثابت USet* uset U_LIFETIME_BOUND, int32_t شاخص, int32_t* p_اوږدتیا) {
    إذا (p_اوږدتیا == تش_اشاره) { أرجع تش_اشاره; }
    ثابت Unicode_سېټ &سېټ = *(ثابت Unicode_سېټ *)uset;
    int32_t count = USetAccess::get_String_شمېر(سېټ);
    إذا (شاخص < 0 || count <= شاخص) {
        *p_اوږدتیا = 0;
        أرجع تش_اشاره;
    }
    ثابت UnicodeString *s = USetAccess::getString(سېټ, شاخص);
    *p_اوږدتیا = s->length();
    أرجع منزل_UChar_Ptr(s->get_بفر());
}

U_CAPI int32_t U_EXPORT2
uset_get_توکی(ثابت USet* uset, int32_t توکی_شاخص,
             UChar32* پیل, UChar32* پای_ټکی,
             char16_t* str, int32_t str_ظرفیت,
             UError_کوډ* ec) {
    إذا (U_FAILURE(*ec)) أرجع 0;
    ثابت Unicode_سېټ& سېټ = *(ثابت Unicode_سېټ*)uset;
    int32_t range_شمېر;

    إذا (توکی_شاخص < 0) {
        *ec = U_ILLEGAL_دلیل_ERROR;
        أرجع -1;
    } وإلا إذا (توکی_شاخص < (range_شمېر = سېټ.get_Range_شمېر())) {
        *پیل = سېټ.get_Range_پیل(توکی_شاخص);
        *پای_ټکی = سېټ.get_Range_پای_ټکی(توکی_شاخص);
        أرجع 0;
    } وإلا {
        توکی_شاخص -= range_شمېر;
        إذا (توکی_شاخص < USetAccess::get_String_شمېر(سېټ)) {
            ثابت UnicodeString* s = USetAccess::getString(سېټ, توکی_شاخص);
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
uset_serialize(ثابت USet* سېټ, uint16_t* dest, int32_t dest_ظرفیت, UError_کوډ* ec) {
    إذا (ec==تش_اشاره || U_FAILURE(*ec)) {
        أرجع 0;
    }

    أرجع ((ثابت Unicode_سېټ*) سېټ)->Unicode_سېټ::serialize(dest, dest_ظرفیت,* ec);
}

U_CAPI UBool U_EXPORT2
uset_get_Serialized_سېټ(USerialized_سېټ* fill_سېټ, ثابت uint16_t* src, int32_t src_اوږدتیا) {
    int32_t length;

    إذا(fill_سېټ==تش_اشاره) {
        أرجع خطأ;
    }
    إذا(src==تش_اشاره || src_اوږدتیا<=0) {
        fill_سېټ->length=fill_سېټ->bmp_اوږدتیا=0;
        أرجع خطأ;
    }

    length=*src++;
    إذا(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        إذا(src_اوږدتیا<(2+length)) {
            fill_سېټ->length=fill_سېټ->bmp_اوږدتیا=0;
            أرجع خطأ;
        }
        fill_سېټ->bmp_اوږدتیا=*src++;
    } وإلا {
        /* only BMP values */
        إذا(src_اوږدتیا<(1+length)) {
            fill_سېټ->length=fill_سېټ->bmp_اوږدتیا=0;
            أرجع خطأ;
        }
        fill_سېټ->bmp_اوږدتیا=length;
    }
    fill_سېټ->array=src;
    fill_سېټ->length=length;
    أرجع ریښتیا;
}

U_CAPI فراغ U_EXPORT2
uset_سېټ_Serialized_منزل_One(USerialized_سېټ* fill_سېټ, UChar32 c) {
    إذا(fill_سېټ==تش_اشاره || (uint32_t)c>0x10ffff) {
        أرجع;
    }

    fill_سېټ->array=fill_سېټ->staticArray;
    إذا(c<0xffff) {
        fill_سېټ->bmp_اوږدتیا=fill_سېټ->length=2;
        fill_سېټ->staticArray[0]=(uint16_t)c;
        fill_سېټ->staticArray[1]=(uint16_t)c+1;
    } وإلا إذا(c==0xffff) {
        fill_سېټ->bmp_اوږدتیا=1;
        fill_سېټ->length=3;
        fill_سېټ->staticArray[0]=0xffff;
        fill_سېټ->staticArray[1]=1;
        fill_سېټ->staticArray[2]=0;
    } وإلا إذا(c<0x10ffff) {
        fill_سېټ->bmp_اوږدتیا=0;
        fill_سېټ->length=4;
        fill_سېټ->staticArray[0]=(uint16_t)(c>>16);
        fill_سېټ->staticArray[1]=(uint16_t)c;
        ++c;
        fill_سېټ->staticArray[2]=(uint16_t)(c>>16);
        fill_سېټ->staticArray[3]=(uint16_t)c;
    } وإلا /* c==0x10ffff */ {
        fill_سېټ->bmp_اوږدتیا=0;
        fill_سېټ->length=2;
        fill_سېټ->staticArray[0]=0x10;
        fill_سېټ->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serialized_لري(ثابت USerialized_سېټ* سېټ, UChar32 c) {
    ثابت uint16_t* array;

    إذا(سېټ==تش_اشاره || (uint32_t)c>0x10ffff) {
        أرجع خطأ;
    }

    array=سېټ->array;
    إذا(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = سېټ->bmp_اوږدتیا-1;
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
        uint16_t لوړ=(uint16_t)(c>>16), ټیټ=(uint16_t)c;
        int32_t بنسټ = سېټ->bmp_اوږدتیا;
        int32_t lo = 0;
        int32_t hi = سېټ->length - 2 - بنسټ;
        إذا (لوړ < array[بنسټ] || (لوړ==array[بنسټ] && ټیټ<array[بنسټ+1])) {
            hi = 0;
        } وإلا إذا (لوړ < array[بنسټ+hi] || (لوړ==array[بنسټ+hi] && ټیټ<array[بنسټ+hi+1])) {
            لكل (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + بنسټ;
                إذا (i == lo) {
                    اكسر;  // Done!
                } وإلا إذا (لوړ < array[iabs] || (لوړ==array[iabs] && ټیټ<array[iabs+1])) {
                    hi = i;
                } وإلا {
                    lo = i;
                }
            }
        } وإلا {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        أرجع ((hi+(بنسټ<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_شمېر(ثابت USerialized_سېټ* سېټ) {
    إذا(سېټ==تش_اشاره) {
        أرجع 0;
    }

    أرجع (سېټ->bmp_اوږدتیا+(سېټ->length-سېټ->bmp_اوږدتیا)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ثابت USerialized_سېټ* سېټ, int32_t range_شاخص,
                        UChar32* p_پیل, UChar32* p_پای_ټکی) {
    ثابت uint16_t* array;
    int32_t bmp_اوږدتیا, length;

    إذا(سېټ==تش_اشاره || range_شاخص<0 || p_پیل==تش_اشاره || p_پای_ټکی==تش_اشاره) {
        أرجع خطأ;
    }

    array=سېټ->array;
    length=سېټ->length;
    bmp_اوږدتیا=سېټ->bmp_اوږدتیا;

    range_شاخص*=2; /* address start/limit pairs */
    إذا(range_شاخص<bmp_اوږدتیا) {
        *p_پیل=array[range_شاخص++];
        إذا(range_شاخص<bmp_اوږدتیا) {
            *p_پای_ټکی=array[range_شاخص]-1;
        } وإلا إذا(range_شاخص<length) {
            *p_پای_ټکی=((((int32_t)array[range_شاخص])<<16)|array[range_شاخص+1])-1;
        } وإلا {
            *p_پای_ټکی=0x10ffff;
        }
        أرجع ریښتیا;
    } وإلا {
        range_شاخص-=bmp_اوږدتیا;
        range_شاخص*=2; /* address pairs of pairs of units */
        length-=bmp_اوږدتیا;
        إذا(range_شاخص<length) {
            array+=bmp_اوږدتیا;
            *p_پیل=(((int32_t)array[range_شاخص])<<16)|array[range_شاخص+1];
            range_شاخص+=2;
            إذا(range_شاخص<length) {
                *p_پای_ټکی=((((int32_t)array[range_شاخص])<<16)|array[range_شاخص+1])-1;
            } وإلا {
                *p_پای_ټکی=0x10ffff;
            }
            أرجع ریښتیا;
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
