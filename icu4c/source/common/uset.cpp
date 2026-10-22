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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/char16ptr.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_open_ఖాళీలు() {
    తిరిగి (USet*) కొత్త Unicode_సమితి();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 ప్రారంభం, UChar32 చివర) {
    తిరిగి (USet*) కొత్త Unicode_సమితి(ప్రారంభం, చివర);
}

U_CAPI శూన్య U_EXPORT2
uset_close(USet* సమితి) {
    తొలగించు (Unicode_సమితి*) సమితి;
}

U_CAPI USet * U_EXPORT2
uset_clone(స్థిరరాశి USet *సమితి) {
    తిరిగి (USet*) (((Unicode_సమితి*) సమితి)->Unicode_సమితి::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(స్థిరరాశి USet *సమితి) {
    తిరిగి ((Unicode_సమితి*) సమితి)->Unicode_సమితి::isFrozen();
}

U_CAPI శూన్య U_EXPORT2
uset_freeze(USet *సమితి) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(స్థిరరాశి USet *సమితి) {
    తిరిగి (USet*) (((Unicode_సమితి*) సమితి)->Unicode_సమితి::cloneAsThawed());
}

U_CAPI శూన్య U_EXPORT2
uset_సమితి(USet* సమితి,
     UChar32 ప్రారంభం, UChar32 చివర) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::సమితి(ప్రారంభం, చివర);
}

U_CAPI శూన్య U_EXPORT2
uset_కూడు_All(USet* సమితి, స్థిరరాశి USet *additional_సమితి) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::కూడు_All(*((స్థిరరాశి Unicode_సమితి*)additional_సమితి));
}

U_CAPI శూన్య U_EXPORT2
uset_కూడు(USet* సమితి, UChar32 c) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::కూడు(c);
}

U_CAPI శూన్య U_EXPORT2
uset_కూడు_Range(USet* సమితి, UChar32 ప్రారంభం, UChar32 చివర) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::కూడు(ప్రారంభం, చివర);    
}

U_CAPI శూన్య U_EXPORT2
uset_కూడు_String(USet* సమితి, స్థిరరాశి char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::కూడు(s);
}

U_CAPI శూన్య U_EXPORT2
uset_కూడు_All_సంకేతం_Points(USet* సమితి, స్థిరరాశి char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::కూడు_All(s);
}

U_CAPI శూన్య U_EXPORT2
uset_remove(USet* సమితి, UChar32 c) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::remove(c);
}

U_CAPI శూన్య U_EXPORT2
uset_removeRange(USet* సమితి, UChar32 ప్రారంభం, UChar32 చివర) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::remove(ప్రారంభం, చివర);
}

U_CAPI శూన్య U_EXPORT2
uset_removeString(USet* సమితి, స్థిరరాశి char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::remove(s);
}

U_CAPI శూన్య U_EXPORT2
uset_remove_All_సంకేతం_Points(USet *సమితి, స్థిరరాశి char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::removeAll(s);
}

U_CAPI శూన్య U_EXPORT2
uset_removeAll(USet* సమితి, స్థిరరాశి USet* remove) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::removeAll(*(స్థిరరాశి Unicode_సమితి*)remove);
}

U_CAPI శూన్య U_EXPORT2
uset_retain(USet* సమితి, UChar32 ప్రారంభం, UChar32 చివర) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::retain(ప్రారంభం, చివర);
}

U_CAPI శూన్య U_EXPORT2
uset_retainString(USet *సమితి, స్థిరరాశి char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::retain(s);
}

U_CAPI శూన్య U_EXPORT2
uset_retain_All_సంకేతం_Points(USet *సమితి, స్థిరరాశి char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::retainAll(s);
}

U_CAPI శూన్య U_EXPORT2
uset_retainAll(USet* సమితి, స్థిరరాశి USet* retain) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::retainAll(*(స్థిరరాశి Unicode_సమితి*)retain);
}

U_CAPI శూన్య U_EXPORT2
uset_compact(USet* సమితి) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::compact();
}

U_CAPI శూన్య U_EXPORT2
uset_complement(USet* సమితి) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::complement();
}

U_CAPI శూన్య U_EXPORT2
uset_complementRange(USet *సమితి, UChar32 ప్రారంభం, UChar32 చివర) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::complement(ప్రారంభం, చివర);
}

U_CAPI శూన్య U_EXPORT2
uset_complementString(USet *సమితి, స్థిరరాశి char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::complement(s);
}

U_CAPI శూన్య U_EXPORT2
uset_complement_All_సంకేతం_Points(USet *సమితి, స్థిరరాశి char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::complementAll(s);
}

U_CAPI శూన్య U_EXPORT2
uset_complementAll(USet* సమితి, స్థిరరాశి USet* complement) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::complementAll(*(స్థిరరాశి Unicode_సమితి*)complement);
}

U_CAPI శూన్య U_EXPORT2
uset_clear(USet* సమితి) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::clear();
}

U_CAPI శూన్య U_EXPORT2
uset_remove_All_అక్షరశ్రేణులు(USet* సమితి) {
    ((Unicode_సమితి*) సమితి)->Unicode_సమితి::remove_All_అక్షరశ్రేణులు();
}

U_CAPI UBool U_EXPORT2
uset_is_ఖాళీలు(స్థిరరాశి USet* సమితి) {
    తిరిగి ((స్థిరరాశి Unicode_సమితి*) సమితి)->Unicode_సమితి::is_ఖాళీలు();
}

U_CAPI UBool U_EXPORT2
uset_has_అక్షరశ్రేణులు(స్థిరరాశి USet* సమితి) {
    తిరిగి ((స్థిరరాశి Unicode_సమితి*) సమితి)->Unicode_సమితి::has_అక్షరశ్రేణులు();
}

U_CAPI UBool U_EXPORT2
uset_కలిగియుంది(స్థిరరాశి USet* సమితి, UChar32 c) {
    తిరిగి ((స్థిరరాశి Unicode_సమితి*) సమితి)->Unicode_సమితి::కలిగియుంది(c);
}

U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_Range(స్థిరరాశి USet* సమితి, UChar32 ప్రారంభం, UChar32 చివర) {
    తిరిగి ((స్థిరరాశి Unicode_సమితి*) సమితి)->Unicode_సమితి::కలిగియుంది(ప్రారంభం, చివర);
}

U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_String(స్థిరరాశి USet* సమితి, స్థిరరాశి char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    తిరిగి ((స్థిరరాశి Unicode_సమితి*) సమితి)->Unicode_సమితి::కలిగియుంది(s);
}

U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_All(స్థిరరాశి USet* set1, స్థిరరాశి USet* set2) {
    తిరిగి ((స్థిరరాశి Unicode_సమితి*) set1)->Unicode_సమితి::కలిగియుంది_All(* (స్థిరరాశి Unicode_సమితి*) set2);
}

U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_All_సంకేతం_Points(స్థిరరాశి USet* సమితి, స్థిరరాశి char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    తిరిగి ((స్థిరరాశి Unicode_సమితి*) సమితి)->Unicode_సమితి::కలిగియుంది_All(s);
}

U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_None(స్థిరరాశి USet* set1, స్థిరరాశి USet* set2) {
    తిరిగి ((స్థిరరాశి Unicode_సమితి*) set1)->Unicode_సమితి::కలిగియుంది_None(* (స్థిరరాశి Unicode_సమితి*) set2);
}

U_CAPI UBool U_EXPORT2
uset_కలిగియుంది_Some(స్థిరరాశి USet* set1, స్థిరరాశి USet* set2) {
    తిరిగి ((స్థిరరాశి Unicode_సమితి*) set1)->Unicode_సమితి::కలిగియుంది_Some(* (స్థిరరాశి Unicode_సమితి*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_పరిధి(స్థిరరాశి USet *సమితి, స్థిరరాశి char16_t *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) {
    తిరిగి ((Unicode_సమితి*) సమితి)->Unicode_సమితి::పరిధి(s, length, పరిధి_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_పరిధి_వెనుక(స్థిరరాశి USet *సమితి, స్థిరరాశి char16_t *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) {
    తిరిగి ((Unicode_సమితి*) సమితి)->Unicode_సమితి::పరిధి_వెనుక(s, length, పరిధి_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_పరిధి_UTF8(స్థిరరాశి USet *సమితి, స్థిరరాశి అక్షరం *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) {
    తిరిగి ((Unicode_సమితి*) సమితి)->Unicode_సమితి::పరిధి_UTF8(s, length, పరిధి_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_పరిధి_వెనుక_UTF8(స్థిరరాశి USet *సమితి, స్థిరరాశి అక్షరం *s, int32_t length, USet_పరిధి_Condition పరిధి_Condition) {
    తిరిగి ((Unicode_సమితి*) సమితి)->Unicode_సమితి::పరిధి_వెనుక_UTF8(s, length, పరిధి_Condition);
}

U_CAPI UBool U_EXPORT2
uset_equals(స్థిరరాశి USet* set1, స్థిరరాశి USet* set2) {
    తిరిగి *(స్థిరరాశి Unicode_సమితి*)set1 == *(స్థిరరాశి Unicode_సమితి*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_సూచీ_యొక్క(స్థిరరాశి USet* సమితి, UChar32 c) {
    తిరిగి ((Unicode_సమితి*) సమితి)->Unicode_సమితి::సూచీ_యొక్క(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(స్థిరరాశి USet* సమితి, int32_t సూచీ) {
    తిరిగి ((Unicode_సమితి*) సమితి)->Unicode_సమితి::charAt(సూచీ);
}

U_CAPI int32_t U_EXPORT2
uset_కొలత(స్థిరరాశి USet* సమితి) {
    తిరిగి ((స్థిరరాశి Unicode_సమితి*) సమితి)->Unicode_సమితి::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
తరగతి USetAccess /* not : public UObject because all methods are static */ {
బహిరంగ:
    /* Try to have the compiler inline these*/
    అంతఃవరుస స్థిర int32_t get_String_లెక్క(స్థిరరాశి Unicode_సమితి& సమితి) {
        తిరిగి సమితి.అక్షరశ్రేణులు_కొలత();
    }
    అంతఃవరుస స్థిర స్థిరరాశి UnicodeString* getString(స్థిరరాశి Unicode_సమితి& సమితి U_LIFETIME_BOUND,
                                                 int32_t i) {
        తిరిగి సమితి.getString(i);
    }
అంతరంగ:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_చివర

U_CAPI int32_t U_EXPORT2
uset_get_Range_లెక్క(స్థిరరాశి USet *సమితి) {
    తిరిగి ((స్థిరరాశి Unicode_సమితి *)సమితి)->Unicode_సమితి::get_Range_లెక్క();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_లెక్క(స్థిరరాశి USet *uset) {
    స్థిరరాశి Unicode_సమితి &సమితి = *(స్థిరరాశి Unicode_సమితి *)uset;
    తిరిగి USetAccess::get_String_లెక్క(సమితి);
}

U_CAPI int32_t U_EXPORT2
uset_get_అంశం_లెక్క(స్థిరరాశి USet* uset) {
    స్థిరరాశి Unicode_సమితి& సమితి = *(స్థిరరాశి Unicode_సమితి*)uset;
    తిరిగి సమితి.get_Range_లెక్క() + USetAccess::get_String_లెక్క(సమితి);
}

U_CAPI స్థిరరాశి UChar* U_EXPORT2
uset_getString(స్థిరరాశి USet* uset U_LIFETIME_BOUND, int32_t సూచీ, int32_t* p_నిడివి) {
    ఒకవేళ (p_నిడివి == శూన్యసూచిక) { తిరిగి శూన్యసూచిక; }
    స్థిరరాశి Unicode_సమితి &సమితి = *(స్థిరరాశి Unicode_సమితి *)uset;
    int32_t count = USetAccess::get_String_లెక్క(సమితి);
    ఒకవేళ (సూచీ < 0 || count <= సూచీ) {
        *p_నిడివి = 0;
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి UnicodeString *s = USetAccess::getString(సమితి, సూచీ);
    *p_నిడివి = s->length();
    తిరిగి గమ్యం_UChar_Ptr(s->get_బఫరు());
}

U_CAPI int32_t U_EXPORT2
uset_get_అంశం(స్థిరరాశి USet* uset, int32_t అంశం_సూచీ,
             UChar32* ప్రారంభం, UChar32* చివర,
             char16_t* str, int32_t str_సామర్థ్యం,
             UError_సంకేతం* ec) {
    ఒకవేళ (U_FAILURE(*ec)) తిరిగి 0;
    స్థిరరాశి Unicode_సమితి& సమితి = *(స్థిరరాశి Unicode_సమితి*)uset;
    int32_t range_లెక్క;

    ఒకవేళ (అంశం_సూచీ < 0) {
        *ec = U_ILLEGAL_వాదం_ERROR;
        తిరిగి -1;
    } లేకపోతే ఒకవేళ (అంశం_సూచీ < (range_లెక్క = సమితి.get_Range_లెక్క())) {
        *ప్రారంభం = సమితి.get_Range_ప్రారంభం(అంశం_సూచీ);
        *చివర = సమితి.get_Range_చివర(అంశం_సూచీ);
        తిరిగి 0;
    } లేకపోతే {
        అంశం_సూచీ -= range_లెక్క;
        ఒకవేళ (అంశం_సూచీ < USetAccess::get_String_లెక్క(సమితి)) {
            స్థిరరాశి UnicodeString* s = USetAccess::getString(సమితి, అంశం_సూచీ);
            తిరిగి s->extract(str, str_సామర్థ్యం, *ec);
        } లేకపోతే {
            *ec = U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి -1;
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
uset_serialize(స్థిరరాశి USet* సమితి, uint16_t* dest, int32_t dest_సామర్థ్యం, UError_సంకేతం* ec) {
    ఒకవేళ (ec==శూన్యసూచిక || U_FAILURE(*ec)) {
        తిరిగి 0;
    }

    తిరిగి ((స్థిరరాశి Unicode_సమితి*) సమితి)->Unicode_సమితి::serialize(dest, dest_సామర్థ్యం,* ec);
}

U_CAPI UBool U_EXPORT2
uset_get_Serialized_సమితి(USerialized_సమితి* fill_సమితి, స్థిరరాశి uint16_t* src, int32_t src_నిడివి) {
    int32_t length;

    ఒకవేళ(fill_సమితి==శూన్యసూచిక) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ(src==శూన్యసూచిక || src_నిడివి<=0) {
        fill_సమితి->length=fill_సమితి->bmp_నిడివి=0;
        తిరిగి అబద్ధం;
    }

    length=*src++;
    ఒకవేళ(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        ఒకవేళ(src_నిడివి<(2+length)) {
            fill_సమితి->length=fill_సమితి->bmp_నిడివి=0;
            తిరిగి అబద్ధం;
        }
        fill_సమితి->bmp_నిడివి=*src++;
    } లేకపోతే {
        /* only BMP values */
        ఒకవేళ(src_నిడివి<(1+length)) {
            fill_సమితి->length=fill_సమితి->bmp_నిడివి=0;
            తిరిగి అబద్ధం;
        }
        fill_సమితి->bmp_నిడివి=length;
    }
    fill_సమితి->array=src;
    fill_సమితి->length=length;
    తిరిగి నిజం;
}

U_CAPI శూన్య U_EXPORT2
uset_సమితి_Serialized_గమ్యం_One(USerialized_సమితి* fill_సమితి, UChar32 c) {
    ఒకవేళ(fill_సమితి==శూన్యసూచిక || (uint32_t)c>0x10ffff) {
        తిరిగి;
    }

    fill_సమితి->array=fill_సమితి->staticArray;
    ఒకవేళ(c<0xffff) {
        fill_సమితి->bmp_నిడివి=fill_సమితి->length=2;
        fill_సమితి->staticArray[0]=(uint16_t)c;
        fill_సమితి->staticArray[1]=(uint16_t)c+1;
    } లేకపోతే ఒకవేళ(c==0xffff) {
        fill_సమితి->bmp_నిడివి=1;
        fill_సమితి->length=3;
        fill_సమితి->staticArray[0]=0xffff;
        fill_సమితి->staticArray[1]=1;
        fill_సమితి->staticArray[2]=0;
    } లేకపోతే ఒకవేళ(c<0x10ffff) {
        fill_సమితి->bmp_నిడివి=0;
        fill_సమితి->length=4;
        fill_సమితి->staticArray[0]=(uint16_t)(c>>16);
        fill_సమితి->staticArray[1]=(uint16_t)c;
        ++c;
        fill_సమితి->staticArray[2]=(uint16_t)(c>>16);
        fill_సమితి->staticArray[3]=(uint16_t)c;
    } లేకపోతే /* c==0x10ffff */ {
        fill_సమితి->bmp_నిడివి=0;
        fill_సమితి->length=2;
        fill_సమితి->staticArray[0]=0x10;
        fill_సమితి->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serialized_కలిగియుంది(స్థిరరాశి USerialized_సమితి* సమితి, UChar32 c) {
    స్థిరరాశి uint16_t* array;

    ఒకవేళ(సమితి==శూన్యసూచిక || (uint32_t)c>0x10ffff) {
        తిరిగి అబద్ధం;
    }

    array=సమితి->array;
    ఒకవేళ(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = సమితి->bmp_నిడివి-1;
        ఒకవేళ (c < array[0]) {
            hi = 0;
        } లేకపోతే ఒకవేళ (c < array[hi]) {
            కొరకు(;;) {
                int32_t i = (lo + hi) >> 1;
                ఒకవేళ (i == lo) {
                    ఆపు;  // Done!
                } లేకపోతే ఒకవేళ (c < array[i]) {
                    hi = i;
                } లేకపోతే {
                    lo = i;
                }
            }
        } లేకపోతే {
            hi += 1;
        }
        తిరిగి hi&1;
    } లేకపోతే {
        /* find c in the supplementary part */
        uint16_t ఎగువ=(uint16_t)(c>>16), దిగువ=(uint16_t)c;
        int32_t పునాది = సమితి->bmp_నిడివి;
        int32_t lo = 0;
        int32_t hi = సమితి->length - 2 - పునాది;
        ఒకవేళ (ఎగువ < array[పునాది] || (ఎగువ==array[పునాది] && దిగువ<array[పునాది+1])) {
            hi = 0;
        } లేకపోతే ఒకవేళ (ఎగువ < array[పునాది+hi] || (ఎగువ==array[పునాది+hi] && దిగువ<array[పునాది+hi+1])) {
            కొరకు (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + పునాది;
                ఒకవేళ (i == lo) {
                    ఆపు;  // Done!
                } లేకపోతే ఒకవేళ (ఎగువ < array[iabs] || (ఎగువ==array[iabs] && దిగువ<array[iabs+1])) {
                    hi = i;
                } లేకపోతే {
                    lo = i;
                }
            }
        } లేకపోతే {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        తిరిగి ((hi+(పునాది<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_లెక్క(స్థిరరాశి USerialized_సమితి* సమితి) {
    ఒకవేళ(సమితి==శూన్యసూచిక) {
        తిరిగి 0;
    }

    తిరిగి (సమితి->bmp_నిడివి+(సమితి->length-సమితి->bmp_నిడివి)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(స్థిరరాశి USerialized_సమితి* సమితి, int32_t range_సూచీ,
                        UChar32* p_ప్రారంభం, UChar32* p_చివర) {
    స్థిరరాశి uint16_t* array;
    int32_t bmp_నిడివి, length;

    ఒకవేళ(సమితి==శూన్యసూచిక || range_సూచీ<0 || p_ప్రారంభం==శూన్యసూచిక || p_చివర==శూన్యసూచిక) {
        తిరిగి అబద్ధం;
    }

    array=సమితి->array;
    length=సమితి->length;
    bmp_నిడివి=సమితి->bmp_నిడివి;

    range_సూచీ*=2; /* address start/limit pairs */
    ఒకవేళ(range_సూచీ<bmp_నిడివి) {
        *p_ప్రారంభం=array[range_సూచీ++];
        ఒకవేళ(range_సూచీ<bmp_నిడివి) {
            *p_చివర=array[range_సూచీ]-1;
        } లేకపోతే ఒకవేళ(range_సూచీ<length) {
            *p_చివర=((((int32_t)array[range_సూచీ])<<16)|array[range_సూచీ+1])-1;
        } లేకపోతే {
            *p_చివర=0x10ffff;
        }
        తిరిగి నిజం;
    } లేకపోతే {
        range_సూచీ-=bmp_నిడివి;
        range_సూచీ*=2; /* address pairs of pairs of units */
        length-=bmp_నిడివి;
        ఒకవేళ(range_సూచీ<length) {
            array+=bmp_నిడివి;
            *p_ప్రారంభం=(((int32_t)array[range_సూచీ])<<16)|array[range_సూచీ+1];
            range_సూచీ+=2;
            ఒకవేళ(range_సూచీ<length) {
                *p_చివర=((((int32_t)array[range_సూచీ])<<16)|array[range_సూచీ+1])-1;
            } లేకపోతే {
                *p_చివర=0x10ffff;
            }
            తిరిగి నిజం;
        } లేకపోతే {
            తిరిగి అబద్ధం;
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
