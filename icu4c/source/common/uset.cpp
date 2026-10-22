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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/char16ptr.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_open_रिक्तानि() {
    प्रत्यागम (USet*) नूतन Unicode_समुच्चयः();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 start, UChar32 अन्तः) {
    प्रत्यागम (USet*) नूतन Unicode_समुच्चयः(start, अन्तः);
}

U_CAPI शून्य U_EXPORT2
uset_close(USet* समुच्चयः) {
    विलोपय (Unicode_समुच्चयः*) समुच्चयः;
}

U_CAPI USet * U_EXPORT2
uset_clone(ध्रुव USet *समुच्चयः) {
    प्रत्यागम (USet*) (((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ध्रुव USet *समुच्चयः) {
    प्रत्यागम ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::isFrozen();
}

U_CAPI शून्य U_EXPORT2
uset_freeze(USet *समुच्चयः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ध्रुव USet *समुच्चयः) {
    प्रत्यागम (USet*) (((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::cloneAsThawed());
}

U_CAPI शून्य U_EXPORT2
uset_समुच्चयः(USet* समुच्चयः,
     UChar32 start, UChar32 अन्तः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::समुच्चयः(start, अन्तः);
}

U_CAPI शून्य U_EXPORT2
uset_सङ्कलय_All(USet* समुच्चयः, ध्रुव USet *additional_समुच्चयः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::सङ्कलय_All(*((ध्रुव Unicode_समुच्चयः*)additional_समुच्चयः));
}

U_CAPI शून्य U_EXPORT2
uset_सङ्कलय(USet* समुच्चयः, UChar32 c) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::सङ्कलय(c);
}

U_CAPI शून्य U_EXPORT2
uset_सङ्कलय_Range(USet* समुच्चयः, UChar32 start, UChar32 अन्तः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::सङ्कलय(start, अन्तः);    
}

U_CAPI शून्य U_EXPORT2
uset_सङ्कलय_String(USet* समुच्चयः, ध्रुव char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::सङ्कलय(s);
}

U_CAPI शून्य U_EXPORT2
uset_सङ्कलय_All_संहिता_Points(USet* समुच्चयः, ध्रुव char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::सङ्कलय_All(s);
}

U_CAPI शून्य U_EXPORT2
uset_remove(USet* समुच्चयः, UChar32 c) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::remove(c);
}

U_CAPI शून्य U_EXPORT2
uset_removeRange(USet* समुच्चयः, UChar32 start, UChar32 अन्तः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::remove(start, अन्तः);
}

U_CAPI शून्य U_EXPORT2
uset_removeString(USet* समुच्चयः, ध्रुव char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::remove(s);
}

U_CAPI शून्य U_EXPORT2
uset_remove_All_संहिता_Points(USet *समुच्चयः, ध्रुव char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::removeAll(s);
}

U_CAPI शून्य U_EXPORT2
uset_removeAll(USet* समुच्चयः, ध्रुव USet* remove) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::removeAll(*(ध्रुव Unicode_समुच्चयः*)remove);
}

U_CAPI शून्य U_EXPORT2
uset_retain(USet* समुच्चयः, UChar32 start, UChar32 अन्तः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::retain(start, अन्तः);
}

U_CAPI शून्य U_EXPORT2
uset_retainString(USet *समुच्चयः, ध्रुव char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::retain(s);
}

U_CAPI शून्य U_EXPORT2
uset_retain_All_संहिता_Points(USet *समुच्चयः, ध्रुव char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::retainAll(s);
}

U_CAPI शून्य U_EXPORT2
uset_retainAll(USet* समुच्चयः, ध्रुव USet* retain) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::retainAll(*(ध्रुव Unicode_समुच्चयः*)retain);
}

U_CAPI शून्य U_EXPORT2
uset_compact(USet* समुच्चयः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::compact();
}

U_CAPI शून्य U_EXPORT2
uset_complement(USet* समुच्चयः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::complement();
}

U_CAPI शून्य U_EXPORT2
uset_complementRange(USet *समुच्चयः, UChar32 start, UChar32 अन्तः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::complement(start, अन्तः);
}

U_CAPI शून्य U_EXPORT2
uset_complementString(USet *समुच्चयः, ध्रुव char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::complement(s);
}

U_CAPI शून्य U_EXPORT2
uset_complement_All_संहिता_Points(USet *समुच्चयः, ध्रुव char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::complementAll(s);
}

U_CAPI शून्य U_EXPORT2
uset_complementAll(USet* समुच्चयः, ध्रुव USet* complement) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::complementAll(*(ध्रुव Unicode_समुच्चयः*)complement);
}

U_CAPI शून्य U_EXPORT2
uset_clear(USet* समुच्चयः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::clear();
}

U_CAPI शून्य U_EXPORT2
uset_remove_All_वाक्यानि(USet* समुच्चयः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::remove_All_वाक्यानि();
}

U_CAPI UBool U_EXPORT2
uset_is_रिक्तानि(ध्रुव USet* समुच्चयः) {
    प्रत्यागम ((ध्रुव Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::is_रिक्तानि();
}

U_CAPI UBool U_EXPORT2
uset_has_वाक्यानि(ध्रुव USet* समुच्चयः) {
    प्रत्यागम ((ध्रुव Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::has_वाक्यानि();
}

U_CAPI UBool U_EXPORT2
uset_अन्तर्भवति(ध्रुव USet* समुच्चयः, UChar32 c) {
    प्रत्यागम ((ध्रुव Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::अन्तर्भवति(c);
}

U_CAPI UBool U_EXPORT2
uset_अन्तर्भवति_Range(ध्रुव USet* समुच्चयः, UChar32 start, UChar32 अन्तः) {
    प्रत्यागम ((ध्रुव Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::अन्तर्भवति(start, अन्तः);
}

U_CAPI UBool U_EXPORT2
uset_अन्तर्भवति_String(ध्रुव USet* समुच्चयः, ध्रुव char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    प्रत्यागम ((ध्रुव Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::अन्तर्भवति(s);
}

U_CAPI UBool U_EXPORT2
uset_अन्तर्भवति_All(ध्रुव USet* set1, ध्रुव USet* set2) {
    प्रत्यागम ((ध्रुव Unicode_समुच्चयः*) set1)->Unicode_समुच्चयः::अन्तर्भवति_All(* (ध्रुव Unicode_समुच्चयः*) set2);
}

U_CAPI UBool U_EXPORT2
uset_अन्तर्भवति_All_संहिता_Points(ध्रुव USet* समुच्चयः, ध्रुव char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    प्रत्यागम ((ध्रुव Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::अन्तर्भवति_All(s);
}

U_CAPI UBool U_EXPORT2
uset_अन्तर्भवति_None(ध्रुव USet* set1, ध्रुव USet* set2) {
    प्रत्यागम ((ध्रुव Unicode_समुच्चयः*) set1)->Unicode_समुच्चयः::अन्तर्भवति_None(* (ध्रुव Unicode_समुच्चयः*) set2);
}

U_CAPI UBool U_EXPORT2
uset_अन्तर्भवति_Some(ध्रुव USet* set1, ध्रुव USet* set2) {
    प्रत्यागम ((ध्रुव Unicode_समुच्चयः*) set1)->Unicode_समुच्चयः::अन्तर्भवति_Some(* (ध्रुव Unicode_समुच्चयः*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_विस्तृतिः(ध्रुव USet *समुच्चयः, ध्रुव char16_t *s, int32_t length, USet_विस्तृतिः_Condition विस्तृतिः_Condition) {
    प्रत्यागम ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::विस्तृतिः(s, length, विस्तृतिः_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_विस्तृतिः_पृष्ठभागः(ध्रुव USet *समुच्चयः, ध्रुव char16_t *s, int32_t length, USet_विस्तृतिः_Condition विस्तृतिः_Condition) {
    प्रत्यागम ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::विस्तृतिः_पृष्ठभागः(s, length, विस्तृतिः_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_विस्तृतिः_UTF8(ध्रुव USet *समुच्चयः, ध्रुव वर्ण *s, int32_t length, USet_विस्तृतिः_Condition विस्तृतिः_Condition) {
    प्रत्यागम ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::विस्तृतिः_UTF8(s, length, विस्तृतिः_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_विस्तृतिः_पृष्ठभागः_UTF8(ध्रुव USet *समुच्चयः, ध्रुव वर्ण *s, int32_t length, USet_विस्तृतिः_Condition विस्तृतिः_Condition) {
    प्रत्यागम ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::विस्तृतिः_पृष्ठभागः_UTF8(s, length, विस्तृतिः_Condition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ध्रुव USet* set1, ध्रुव USet* set2) {
    प्रत्यागम *(ध्रुव Unicode_समुच्चयः*)set1 == *(ध्रुव Unicode_समुच्चयः*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_सूचकांक_सम्बन्धि(ध्रुव USet* समुच्चयः, UChar32 c) {
    प्रत्यागम ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::सूचकांक_सम्बन्धि(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ध्रुव USet* समुच्चयः, int32_t सूचकांक) {
    प्रत्यागम ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::charAt(सूचकांक);
}

U_CAPI int32_t U_EXPORT2
uset_आकार(ध्रुव USet* समुच्चयः) {
    प्रत्यागम ((ध्रुव Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
वर्ग USetAccess /* not : public UObject because all methods are static */ {
सार्वजनिक:
    /* Try to have the compiler inline these*/
    अन्तःस्थ स्थिर int32_t get_String_गणनम्(ध्रुव Unicode_समुच्चयः& समुच्चयः) {
        प्रत्यागम समुच्चयः.वाक्यानि_आकार();
    }
    अन्तःस्थ स्थिर ध्रुव UnicodeString* getString(ध्रुव Unicode_समुच्चयः& समुच्चयः U_LIFETIME_BOUND,
                                                 int32_t i) {
        प्रत्यागम समुच्चयः.getString(i);
    }
गुप्त:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_अन्तः

U_CAPI int32_t U_EXPORT2
uset_get_Range_गणनम्(ध्रुव USet *समुच्चयः) {
    प्रत्यागम ((ध्रुव Unicode_समुच्चयः *)समुच्चयः)->Unicode_समुच्चयः::get_Range_गणनम्();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_गणनम्(ध्रुव USet *uset) {
    ध्रुव Unicode_समुच्चयः &समुच्चयः = *(ध्रुव Unicode_समुच्चयः *)uset;
    प्रत्यागम USetAccess::get_String_गणनम्(समुच्चयः);
}

U_CAPI int32_t U_EXPORT2
uset_get_Item_गणनम्(ध्रुव USet* uset) {
    ध्रुव Unicode_समुच्चयः& समुच्चयः = *(ध्रुव Unicode_समुच्चयः*)uset;
    प्रत्यागम समुच्चयः.get_Range_गणनम्() + USetAccess::get_String_गणनम्(समुच्चयः);
}

U_CAPI ध्रुव UChar* U_EXPORT2
uset_getString(ध्रुव USet* uset U_LIFETIME_BOUND, int32_t सूचकांक, int32_t* p_दीर्घता) {
    यदि (p_दीर्घता == शून्यसूचकः) { प्रत्यागम शून्यसूचकः; }
    ध्रुव Unicode_समुच्चयः &समुच्चयः = *(ध्रुव Unicode_समुच्चयः *)uset;
    int32_t count = USetAccess::get_String_गणनम्(समुच्चयः);
    यदि (सूचकांक < 0 || count <= सूचकांक) {
        *p_दीर्घता = 0;
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव UnicodeString *s = USetAccess::getString(समुच्चयः, सूचकांक);
    *p_दीर्घता = s->length();
    प्रत्यागम गन्तव्यम्_UChar_Ptr(s->get_बफर());
}

U_CAPI int32_t U_EXPORT2
uset_getItem(ध्रुव USet* uset, int32_t item_सूचकांक,
             UChar32* start, UChar32* अन्तः,
             char16_t* str, int32_t str_क्षमता,
             UError_संहिता* ec) {
    यदि (U_FAILURE(*ec)) प्रत्यागम 0;
    ध्रुव Unicode_समुच्चयः& समुच्चयः = *(ध्रुव Unicode_समुच्चयः*)uset;
    int32_t range_गणनम्;

    यदि (item_सूचकांक < 0) {
        *ec = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम -1;
    } अन्यथा यदि (item_सूचकांक < (range_गणनम् = समुच्चयः.get_Range_गणनम्())) {
        *start = समुच्चयः.getRangeStart(item_सूचकांक);
        *अन्तः = समुच्चयः.get_Range_अन्तः(item_सूचकांक);
        प्रत्यागम 0;
    } अन्यथा {
        item_सूचकांक -= range_गणनम्;
        यदि (item_सूचकांक < USetAccess::get_String_गणनम्(समुच्चयः)) {
            ध्रुव UnicodeString* s = USetAccess::getString(समुच्चयः, item_सूचकांक);
            प्रत्यागम s->extract(str, str_क्षमता, *ec);
        } अन्यथा {
            *ec = U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम -1;
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
uset_serialize(ध्रुव USet* समुच्चयः, uint16_t* dest, int32_t dest_क्षमता, UError_संहिता* ec) {
    यदि (ec==शून्यसूचकः || U_FAILURE(*ec)) {
        प्रत्यागम 0;
    }

    प्रत्यागम ((ध्रुव Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::serialize(dest, dest_क्षमता,* ec);
}

U_CAPI UBool U_EXPORT2
uset_get_Serialized_समुच्चयः(USerialized_समुच्चयः* fill_समुच्चयः, ध्रुव uint16_t* src, int32_t src_दीर्घता) {
    int32_t length;

    यदि(fill_समुच्चयः==शून्यसूचकः) {
        प्रत्यागम असत्य;
    }
    यदि(src==शून्यसूचकः || src_दीर्घता<=0) {
        fill_समुच्चयः->length=fill_समुच्चयः->bmp_दीर्घता=0;
        प्रत्यागम असत्य;
    }

    length=*src++;
    यदि(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        यदि(src_दीर्घता<(2+length)) {
            fill_समुच्चयः->length=fill_समुच्चयः->bmp_दीर्घता=0;
            प्रत्यागम असत्य;
        }
        fill_समुच्चयः->bmp_दीर्घता=*src++;
    } अन्यथा {
        /* only BMP values */
        यदि(src_दीर्घता<(1+length)) {
            fill_समुच्चयः->length=fill_समुच्चयः->bmp_दीर्घता=0;
            प्रत्यागम असत्य;
        }
        fill_समुच्चयः->bmp_दीर्घता=length;
    }
    fill_समुच्चयः->array=src;
    fill_समुच्चयः->length=length;
    प्रत्यागम सत्य;
}

U_CAPI शून्य U_EXPORT2
uset_समुच्चयः_Serialized_गन्तव्यम्_One(USerialized_समुच्चयः* fill_समुच्चयः, UChar32 c) {
    यदि(fill_समुच्चयः==शून्यसूचकः || (uint32_t)c>0x10ffff) {
        प्रत्यागम;
    }

    fill_समुच्चयः->array=fill_समुच्चयः->staticArray;
    यदि(c<0xffff) {
        fill_समुच्चयः->bmp_दीर्घता=fill_समुच्चयः->length=2;
        fill_समुच्चयः->staticArray[0]=(uint16_t)c;
        fill_समुच्चयः->staticArray[1]=(uint16_t)c+1;
    } अन्यथा यदि(c==0xffff) {
        fill_समुच्चयः->bmp_दीर्घता=1;
        fill_समुच्चयः->length=3;
        fill_समुच्चयः->staticArray[0]=0xffff;
        fill_समुच्चयः->staticArray[1]=1;
        fill_समुच्चयः->staticArray[2]=0;
    } अन्यथा यदि(c<0x10ffff) {
        fill_समुच्चयः->bmp_दीर्घता=0;
        fill_समुच्चयः->length=4;
        fill_समुच्चयः->staticArray[0]=(uint16_t)(c>>16);
        fill_समुच्चयः->staticArray[1]=(uint16_t)c;
        ++c;
        fill_समुच्चयः->staticArray[2]=(uint16_t)(c>>16);
        fill_समुच्चयः->staticArray[3]=(uint16_t)c;
    } अन्यथा /* c==0x10ffff */ {
        fill_समुच्चयः->bmp_दीर्घता=0;
        fill_समुच्चयः->length=2;
        fill_समुच्चयः->staticArray[0]=0x10;
        fill_समुच्चयः->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serialized_अन्तर्भवति(ध्रुव USerialized_समुच्चयः* समुच्चयः, UChar32 c) {
    ध्रुव uint16_t* array;

    यदि(समुच्चयः==शून्यसूचकः || (uint32_t)c>0x10ffff) {
        प्रत्यागम असत्य;
    }

    array=समुच्चयः->array;
    यदि(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = समुच्चयः->bmp_दीर्घता-1;
        यदि (c < array[0]) {
            hi = 0;
        } अन्यथा यदि (c < array[hi]) {
            प्रति(;;) {
                int32_t i = (lo + hi) >> 1;
                यदि (i == lo) {
                    भङ्ग;  // Done!
                } अन्यथा यदि (c < array[i]) {
                    hi = i;
                } अन्यथा {
                    lo = i;
                }
            }
        } अन्यथा {
            hi += 1;
        }
        प्रत्यागम hi&1;
    } अन्यथा {
        /* find c in the supplementary part */
        uint16_t उच्चः=(uint16_t)(c>>16), नीचः=(uint16_t)c;
        int32_t आधारः = समुच्चयः->bmp_दीर्घता;
        int32_t lo = 0;
        int32_t hi = समुच्चयः->length - 2 - आधारः;
        यदि (उच्चः < array[आधारः] || (उच्चः==array[आधारः] && नीचः<array[आधारः+1])) {
            hi = 0;
        } अन्यथा यदि (उच्चः < array[आधारः+hi] || (उच्चः==array[आधारः+hi] && नीचः<array[आधारः+hi+1])) {
            प्रति (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + आधारः;
                यदि (i == lo) {
                    भङ्ग;  // Done!
                } अन्यथा यदि (उच्चः < array[iabs] || (उच्चः==array[iabs] && नीचः<array[iabs+1])) {
                    hi = i;
                } अन्यथा {
                    lo = i;
                }
            }
        } अन्यथा {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        प्रत्यागम ((hi+(आधारः<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_गणनम्(ध्रुव USerialized_समुच्चयः* समुच्चयः) {
    यदि(समुच्चयः==शून्यसूचकः) {
        प्रत्यागम 0;
    }

    प्रत्यागम (समुच्चयः->bmp_दीर्घता+(समुच्चयः->length-समुच्चयः->bmp_दीर्घता)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ध्रुव USerialized_समुच्चयः* समुच्चयः, int32_t range_सूचकांक,
                        UChar32* pStart, UChar32* p_अन्तः) {
    ध्रुव uint16_t* array;
    int32_t bmp_दीर्घता, length;

    यदि(समुच्चयः==शून्यसूचकः || range_सूचकांक<0 || pStart==शून्यसूचकः || p_अन्तः==शून्यसूचकः) {
        प्रत्यागम असत्य;
    }

    array=समुच्चयः->array;
    length=समुच्चयः->length;
    bmp_दीर्घता=समुच्चयः->bmp_दीर्घता;

    range_सूचकांक*=2; /* address start/limit pairs */
    यदि(range_सूचकांक<bmp_दीर्घता) {
        *pStart=array[range_सूचकांक++];
        यदि(range_सूचकांक<bmp_दीर्घता) {
            *p_अन्तः=array[range_सूचकांक]-1;
        } अन्यथा यदि(range_सूचकांक<length) {
            *p_अन्तः=((((int32_t)array[range_सूचकांक])<<16)|array[range_सूचकांक+1])-1;
        } अन्यथा {
            *p_अन्तः=0x10ffff;
        }
        प्रत्यागम सत्य;
    } अन्यथा {
        range_सूचकांक-=bmp_दीर्घता;
        range_सूचकांक*=2; /* address pairs of pairs of units */
        length-=bmp_दीर्घता;
        यदि(range_सूचकांक<length) {
            array+=bmp_दीर्घता;
            *pStart=(((int32_t)array[range_सूचकांक])<<16)|array[range_सूचकांक+1];
            range_सूचकांक+=2;
            यदि(range_सूचकांक<length) {
                *p_अन्तः=((((int32_t)array[range_सूचकांक])<<16)|array[range_सूचकांक+1])-1;
            } अन्यथा {
                *p_अन्तः=0x10ffff;
            }
            प्रत्यागम सत्य;
        } अन्यथा {
            प्रत्यागम असत्य;
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
