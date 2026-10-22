// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uset_props.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004aug30
*   created by: Markus W. Scherer
*
*   C wrappers around UnicodeSet functions that are implemented in
*   uniset_props.cpp, split off for modularization.
*/

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_open_आदर्शः(ध्रुव char16_t* आदर्शः, int32_t आदर्शः_दीर्घता,
                 UError_संहिता* ec)
{
    UnicodeString pat(आदर्शः_दीर्घता==-1, आदर्शः, आदर्शः_दीर्घता);
    Unicode_समुच्चयः* समुच्चयः = नूतन Unicode_समुच्चयः(pat, *ec);
    /* test for nullptr */
    यदि (समुच्चयः == शून्यसूचकः) {
        *ec = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि (U_FAILURE(*ec)) {
        विलोपय समुच्चयः;
        समुच्चयः = शून्यसूचकः;
    }
    प्रत्यागम (USet*) समुच्चयः;
}

U_CAPI USet* U_EXPORT2
uset_open_आदर्शः_Options(ध्रुव char16_t* आदर्शः, int32_t आदर्शः_दीर्घता,
                 uint32_t options,
                 UError_संहिता* ec)
{
    UnicodeString pat(आदर्शः_दीर्घता==-1, आदर्शः, आदर्शः_दीर्घता);
    Unicode_समुच्चयः* समुच्चयः = नूतन Unicode_समुच्चयः(pat, options, शून्यसूचकः, *ec);
    /* test for nullptr */
    यदि (समुच्चयः == शून्यसूचकः) {
        *ec = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि (U_FAILURE(*ec)) {
        विलोपय समुच्चयः;
        समुच्चयः = शून्यसूचकः;
    }
    प्रत्यागम (USet*) समुच्चयः;
}


U_CAPI int32_t U_EXPORT2 
uset_apply_आदर्शः(USet *समुच्चयः,
                  ध्रुव char16_t *आदर्शः, int32_t आदर्शः_दीर्घता,
                  uint32_t options,
                  UError_संहिता *स्थितिः){

    // status code needs to be checked since we 
    // dereference it
    यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    यदि(समुच्चयः == शून्यसूचकः){
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    UnicodeString pat(आदर्शः, आदर्शः_दीर्घता);

    ParsePosition pos;
   
    ((Unicode_समुच्चयः*) समुच्चयः)->apply_आदर्शः(pat, pos, options, शून्यसूचकः, *स्थितिः);
    
    प्रत्यागम pos.get_सूचकांक();
}

U_CAPI शून्य U_EXPORT2
uset_apply_Int_Property_मानम्(USet* समुच्चयः,
               UProperty prop, int32_t value, UError_संहिता* ec) {
    ((Unicode_समुच्चयः*) समुच्चयः)->apply_Int_Property_मानम्(prop, value, *ec);
}

U_CAPI शून्य U_EXPORT2
uset_applyPropertyAlias(USet* समुच्चयः,
                        ध्रुव char16_t *prop, int32_t prop_दीर्घता,
                        ध्रुव char16_t *value, int32_t मानम्_दीर्घता,
            UError_संहिता* ec) {

    UnicodeString p(prop, prop_दीर्घता);
    UnicodeString v(value, मानम्_दीर्घता);

    ((Unicode_समुच्चयः*) समुच्चयः)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resembles_आदर्शः(ध्रुव char16_t *आदर्शः, int32_t आदर्शः_दीर्घता,
                      int32_t pos) {

    UnicodeString pat(आदर्शः, आदर्शः_दीर्घता);

    प्रत्यागम ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            Unicode_समुच्चयः::resembles_आदर्शः(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_गन्तव्यम्_आदर्शः(ध्रुव USet* समुच्चयः,
               char16_t* परिणाम, int32_t परिणाम_क्षमता,
               UBool escapeUnprintable,
               UError_संहिता* ec) {
    UnicodeString pat;
    ((ध्रुव Unicode_समुच्चयः*) समुच्चयः)->गन्तव्यम्_आदर्शः(pat, escapeUnprintable);
    प्रत्यागम pat.extract(परिणाम, परिणाम_क्षमता, *ec);
}

U_CAPI शून्य U_EXPORT2
uset_closeOver(USet* समुच्चयः, int32_t गुणाः) {
    ((Unicode_समुच्चयः*) समुच्चयः)->Unicode_समुच्चयः::closeOver(गुणाः);
}
