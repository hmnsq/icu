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
uset_open_نمونہ(ثابت char16_t* نمونہ, int32_t نمونہ_لمائی,
                 UError_کوڈ* ec)
{
    UnicodeString pat(نمونہ_لمائی==-1, نمونہ, نمونہ_لمائی);
    Unicode_سیٹ* سیٹ = جديد Unicode_سیٹ(pat, *ec);
    /* test for nullptr */
    إذا (سیٹ == خالی_سوچک) {
        *ec = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }

    إذا (U_FAILURE(*ec)) {
        مٹاؤ سیٹ;
        سیٹ = خالی_سوچک;
    }
    أرجع (USet*) سیٹ;
}

U_CAPI USet* U_EXPORT2
uset_open_نمونہ_Options(ثابت char16_t* نمونہ, int32_t نمونہ_لمائی,
                 uint32_t options,
                 UError_کوڈ* ec)
{
    UnicodeString pat(نمونہ_لمائی==-1, نمونہ, نمونہ_لمائی);
    Unicode_سیٹ* سیٹ = جديد Unicode_سیٹ(pat, options, خالی_سوچک, *ec);
    /* test for nullptr */
    إذا (سیٹ == خالی_سوچک) {
        *ec = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }

    إذا (U_FAILURE(*ec)) {
        مٹاؤ سیٹ;
        سیٹ = خالی_سوچک;
    }
    أرجع (USet*) سیٹ;
}


U_CAPI int32_t U_EXPORT2 
uset_apply_نمونہ(USet *سیٹ,
                  ثابت char16_t *نمونہ, int32_t نمونہ_لمائی,
                  uint32_t options,
                  UError_کوڈ *حالت){

    // status code needs to be checked since we 
    // dereference it
    إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    إذا(سیٹ == خالی_سوچک){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    UnicodeString pat(نمونہ, نمونہ_لمائی);

    ParsePosition pos;
   
    ((Unicode_سیٹ*) سیٹ)->apply_نمونہ(pat, pos, options, خالی_سوچک, *حالت);
    
    أرجع pos.get_اشاریہ();
}

U_CAPI فراغ U_EXPORT2
uset_apply_Int_Property_قدر(USet* سیٹ,
               UProperty prop, int32_t value, UError_کوڈ* ec) {
    ((Unicode_سیٹ*) سیٹ)->apply_Int_Property_قدر(prop, value, *ec);
}

U_CAPI فراغ U_EXPORT2
uset_applyPropertyAlias(USet* سیٹ,
                        ثابت char16_t *prop, int32_t prop_لمائی,
                        ثابت char16_t *value, int32_t قدر_لمائی,
            UError_کوڈ* ec) {

    UnicodeString p(prop, prop_لمائی);
    UnicodeString v(value, قدر_لمائی);

    ((Unicode_سیٹ*) سیٹ)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resembles_نمونہ(ثابت char16_t *نمونہ, int32_t نمونہ_لمائی,
                      int32_t pos) {

    UnicodeString pat(نمونہ, نمونہ_لمائی);

    أرجع ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            Unicode_سیٹ::resembles_نمونہ(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_منزل_نمونہ(ثابت USet* سیٹ,
               char16_t* نتیجہ, int32_t نتیجہ_سمرتھا,
               UBool escapeUnprintable,
               UError_کوڈ* ec) {
    UnicodeString pat;
    ((ثابت Unicode_سیٹ*) سیٹ)->منزل_نمونہ(pat, escapeUnprintable);
    أرجع pat.extract(نتیجہ, نتیجہ_سمرتھا, *ec);
}

U_CAPI فراغ U_EXPORT2
uset_closeOver(USet* سیٹ, int32_t گن) {
    ((Unicode_سیٹ*) سیٹ)->Unicode_سیٹ::closeOver(گن);
}
