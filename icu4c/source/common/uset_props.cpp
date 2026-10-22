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

#ضمن "unicode/utypes.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/uset.h"
#ضمن "unicode/uniset.h"
#ضمن "cmemory.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_open_نمونو(ثابت char16_t* نمونو, int32_t نمونو_ڊيگهائي,
                 UError_ڪوڊ* ec)
{
    UnicodeString pat(نمونو_ڊيگهائي==-1, نمونو, نمونو_ڊيگهائي);
    Unicode_سيٽ* سيٽ = جديد Unicode_سيٽ(pat, *ec);
    /* test for nullptr */
    إذا (سيٽ == خالي_اشارو) {
        *ec = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }

    إذا (U_FAILURE(*ec)) {
        ميٽيو سيٽ;
        سيٽ = خالي_اشارو;
    }
    أرجع (USet*) سيٽ;
}

U_CAPI USet* U_EXPORT2
uset_open_نمونو_Options(ثابت char16_t* نمونو, int32_t نمونو_ڊيگهائي,
                 uint32_t options,
                 UError_ڪوڊ* ec)
{
    UnicodeString pat(نمونو_ڊيگهائي==-1, نمونو, نمونو_ڊيگهائي);
    Unicode_سيٽ* سيٽ = جديد Unicode_سيٽ(pat, options, خالي_اشارو, *ec);
    /* test for nullptr */
    إذا (سيٽ == خالي_اشارو) {
        *ec = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }

    إذا (U_FAILURE(*ec)) {
        ميٽيو سيٽ;
        سيٽ = خالي_اشارو;
    }
    أرجع (USet*) سيٽ;
}


U_CAPI int32_t U_EXPORT2 
uset_apply_نمونو(USet *سيٽ,
                  ثابت char16_t *نمونو, int32_t نمونو_ڊيگهائي,
                  uint32_t options,
                  UError_ڪوڊ *حالت){

    // status code needs to be checked since we 
    // dereference it
    إذا(حالت == خالي_اشارو || U_FAILURE(*حالت)){
        أرجع 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    إذا(سيٽ == خالي_اشارو){
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    UnicodeString pat(نمونو, نمونو_ڊيگهائي);

    ParsePosition pos;
   
    ((Unicode_سيٽ*) سيٽ)->apply_نمونو(pat, pos, options, خالي_اشارو, *حالت);
    
    أرجع pos.get_اشاريو();
}

U_CAPI فراغ U_EXPORT2
uset_apply_Int_Property_قدر(USet* سيٽ,
               UProperty prop, int32_t value, UError_ڪوڊ* ec) {
    ((Unicode_سيٽ*) سيٽ)->apply_Int_Property_قدر(prop, value, *ec);
}

U_CAPI فراغ U_EXPORT2
uset_applyPropertyAlias(USet* سيٽ,
                        ثابت char16_t *prop, int32_t prop_ڊيگهائي,
                        ثابت char16_t *value, int32_t قدر_ڊيگهائي,
            UError_ڪوڊ* ec) {

    UnicodeString p(prop, prop_ڊيگهائي);
    UnicodeString v(value, قدر_ڊيگهائي);

    ((Unicode_سيٽ*) سيٽ)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resembles_نمونو(ثابت char16_t *نمونو, int32_t نمونو_ڊيگهائي,
                      int32_t pos) {

    UnicodeString pat(نمونو, نمونو_ڊيگهائي);

    أرجع ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            Unicode_سيٽ::resembles_نمونو(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_منزل_نمونو(ثابت USet* سيٽ,
               char16_t* نتيجو, int32_t نتيجو_صلاحيت,
               UBool escapeUnprintable,
               UError_ڪوڊ* ec) {
    UnicodeString pat;
    ((ثابت Unicode_سيٽ*) سيٽ)->منزل_نمونو(pat, escapeUnprintable);
    أرجع pat.extract(نتيجو, نتيجو_صلاحيت, *ec);
}

U_CAPI فراغ U_EXPORT2
uset_closeOver(USet* سيٽ, int32_t خاصيتون) {
    ((Unicode_سيٽ*) سيٽ)->Unicode_سيٽ::closeOver(خاصيتون);
}
