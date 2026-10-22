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
uset_open_الگو(ثابت char16_t* الگو, int32_t الگو_درازی,
                 UError_کد* ec)
{
    UnicodeString pat(الگو_درازی==-1, الگو, الگو_درازی);
    Unicode_مجموعه* مجموعه = جديد Unicode_مجموعه(pat, *ec);
    /* test for nullptr */
    إذا (مجموعه == اشاره_تهی) {
        *ec = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }

    إذا (U_FAILURE(*ec)) {
        حذف مجموعه;
        مجموعه = اشاره_تهی;
    }
    أرجع (USet*) مجموعه;
}

U_CAPI USet* U_EXPORT2
uset_open_الگو_Options(ثابت char16_t* الگو, int32_t الگو_درازی,
                 uint32_t options,
                 UError_کد* ec)
{
    UnicodeString pat(الگو_درازی==-1, الگو, الگو_درازی);
    Unicode_مجموعه* مجموعه = جديد Unicode_مجموعه(pat, options, اشاره_تهی, *ec);
    /* test for nullptr */
    إذا (مجموعه == اشاره_تهی) {
        *ec = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }

    إذا (U_FAILURE(*ec)) {
        حذف مجموعه;
        مجموعه = اشاره_تهی;
    }
    أرجع (USet*) مجموعه;
}


U_CAPI int32_t U_EXPORT2 
uset_apply_الگو(USet *مجموعه,
                  ثابت char16_t *الگو, int32_t الگو_درازی,
                  uint32_t options,
                  UError_کد *وضعیت){

    // status code needs to be checked since we 
    // dereference it
    إذا(وضعیت == اشاره_تهی || U_FAILURE(*وضعیت)){
        أرجع 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    إذا(مجموعه == اشاره_تهی){
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    UnicodeString pat(الگو, الگو_درازی);

    ParsePosition pos;
   
    ((Unicode_مجموعه*) مجموعه)->apply_الگو(pat, pos, options, اشاره_تهی, *وضعیت);
    
    أرجع pos.get_شاخص();
}

U_CAPI فراغ U_EXPORT2
uset_apply_Int_Property_مقدار(USet* مجموعه,
               UProperty prop, int32_t value, UError_کد* ec) {
    ((Unicode_مجموعه*) مجموعه)->apply_Int_Property_مقدار(prop, value, *ec);
}

U_CAPI فراغ U_EXPORT2
uset_applyPropertyAlias(USet* مجموعه,
                        ثابت char16_t *prop, int32_t prop_درازی,
                        ثابت char16_t *value, int32_t مقدار_درازی,
            UError_کد* ec) {

    UnicodeString p(prop, prop_درازی);
    UnicodeString v(value, مقدار_درازی);

    ((Unicode_مجموعه*) مجموعه)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resembles_الگو(ثابت char16_t *الگو, int32_t الگو_درازی,
                      int32_t pos) {

    UnicodeString pat(الگو, الگو_درازی);

    أرجع ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            Unicode_مجموعه::resembles_الگو(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_مقصد_الگو(ثابت USet* مجموعه,
               char16_t* نتیجه, int32_t نتیجه_ظرفیت,
               UBool escapeUnprintable,
               UError_کد* ec) {
    UnicodeString pat;
    ((ثابت Unicode_مجموعه*) مجموعه)->مقصد_الگو(pat, escapeUnprintable);
    أرجع pat.extract(نتیجه, نتیجه_ظرفیت, *ec);
}

U_CAPI فراغ U_EXPORT2
uset_closeOver(USet* مجموعه, int32_t صفات) {
    ((Unicode_مجموعه*) مجموعه)->Unicode_مجموعه::closeOver(صفات);
}
