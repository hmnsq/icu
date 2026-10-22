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

#شامل "unicode/utypes.h"
#شامل "unicode/uobject.h"
#شامل "unicode/uset.h"
#شامل "unicode/uniset.h"
#شامل "cmemory.h"
#شامل "unicode/ustring.h"
#شامل "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_open_نمونہ(ثابت char16_t* نمونہ, int32_t نمونہ_لمبائی,
                 UError_رمز* ec)
{
    UnicodeString pat(نمونہ_لمبائی==-1, نمونہ, نمونہ_لمبائی);
    Unicode_سیٹ* سیٹ = نیا Unicode_سیٹ(pat, *ec);
    /* test for nullptr */
    اگر (سیٹ == خالی_اشاریہ) {
        *ec = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر (U_FAILURE(*ec)) {
        حذف سیٹ;
        سیٹ = خالی_اشاریہ;
    }
    واپس (USet*) سیٹ;
}

U_CAPI USet* U_EXPORT2
uset_open_نمونہ_Options(ثابت char16_t* نمونہ, int32_t نمونہ_لمبائی,
                 uint32_t options,
                 UError_رمز* ec)
{
    UnicodeString pat(نمونہ_لمبائی==-1, نمونہ, نمونہ_لمبائی);
    Unicode_سیٹ* سیٹ = نیا Unicode_سیٹ(pat, options, خالی_اشاریہ, *ec);
    /* test for nullptr */
    اگر (سیٹ == خالی_اشاریہ) {
        *ec = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر (U_FAILURE(*ec)) {
        حذف سیٹ;
        سیٹ = خالی_اشاریہ;
    }
    واپس (USet*) سیٹ;
}


U_CAPI int32_t U_EXPORT2 
uset_apply_نمونہ(USet *سیٹ,
                  ثابت char16_t *نمونہ, int32_t نمونہ_لمبائی,
                  uint32_t options,
                  UError_رمز *حالت_2){

    // status code needs to be checked since we 
    // dereference it
    اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    اگر(سیٹ == خالی_اشاریہ){
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    UnicodeString pat(نمونہ, نمونہ_لمبائی);

    ParsePosition pos;
   
    ((Unicode_سیٹ*) سیٹ)->apply_نمونہ(pat, pos, options, خالی_اشاریہ, *حالت_2);
    
    واپس pos.get_اشاریہ();
}

U_CAPI خالی U_EXPORT2
uset_apply_Int_Property_قدر(USet* سیٹ,
               UProperty prop, int32_t value, UError_رمز* ec) {
    ((Unicode_سیٹ*) سیٹ)->apply_Int_Property_قدر(prop, value, *ec);
}

U_CAPI خالی U_EXPORT2
uset_applyPropertyAlias(USet* سیٹ,
                        ثابت char16_t *prop, int32_t prop_لمبائی,
                        ثابت char16_t *value, int32_t قدر_لمبائی,
            UError_رمز* ec) {

    UnicodeString p(prop, prop_لمبائی);
    UnicodeString v(value, قدر_لمبائی);

    ((Unicode_سیٹ*) سیٹ)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resembles_نمونہ(ثابت char16_t *نمونہ, int32_t نمونہ_لمبائی,
                      int32_t pos) {

    UnicodeString pat(نمونہ, نمونہ_لمبائی);

    واپس ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            Unicode_سیٹ::resembles_نمونہ(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_مقصد_نمونہ(ثابت USet* سیٹ,
               char16_t* نتیجہ, int32_t نتیجہ_استعداد,
               UBool escapeUnprintable,
               UError_رمز* ec) {
    UnicodeString pat;
    ((ثابت Unicode_سیٹ*) سیٹ)->مقصد_نمونہ(pat, escapeUnprintable);
    واپس pat.extract(نتیجہ, نتیجہ_استعداد, *ec);
}

U_CAPI خالی U_EXPORT2
uset_closeOver(USet* سیٹ, int32_t اوصاف) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::closeOver(اوصاف);
}
