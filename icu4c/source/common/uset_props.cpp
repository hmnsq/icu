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
uset_open_نمونه(ثابت char16_t* نمونه, int32_t نمونه_اوږدتیا,
                 UError_کوډ* ec)
{
    UnicodeString pat(نمونه_اوږدتیا==-1, نمونه, نمونه_اوږدتیا);
    Unicode_سېټ* سېټ = جديد Unicode_سېټ(pat, *ec);
    /* test for nullptr */
    إذا (سېټ == تش_اشاره) {
        *ec = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }

    إذا (U_FAILURE(*ec)) {
        ړنګول سېټ;
        سېټ = تش_اشاره;
    }
    أرجع (USet*) سېټ;
}

U_CAPI USet* U_EXPORT2
uset_open_نمونه_Options(ثابت char16_t* نمونه, int32_t نمونه_اوږدتیا,
                 uint32_t options,
                 UError_کوډ* ec)
{
    UnicodeString pat(نمونه_اوږدتیا==-1, نمونه, نمونه_اوږدتیا);
    Unicode_سېټ* سېټ = جديد Unicode_سېټ(pat, options, تش_اشاره, *ec);
    /* test for nullptr */
    إذا (سېټ == تش_اشاره) {
        *ec = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }

    إذا (U_FAILURE(*ec)) {
        ړنګول سېټ;
        سېټ = تش_اشاره;
    }
    أرجع (USet*) سېټ;
}


U_CAPI int32_t U_EXPORT2 
uset_apply_نمونه(USet *سېټ,
                  ثابت char16_t *نمونه, int32_t نمونه_اوږدتیا,
                  uint32_t options,
                  UError_کوډ *حالت){

    // status code needs to be checked since we 
    // dereference it
    إذا(حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    إذا(سېټ == تش_اشاره){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    UnicodeString pat(نمونه, نمونه_اوږدتیا);

    ParsePosition pos;
   
    ((Unicode_سېټ*) سېټ)->apply_نمونه(pat, pos, options, تش_اشاره, *حالت);
    
    أرجع pos.get_شاخص();
}

U_CAPI فراغ U_EXPORT2
uset_apply_Int_Property_ارزښت(USet* سېټ,
               UProperty prop, int32_t value, UError_کوډ* ec) {
    ((Unicode_سېټ*) سېټ)->apply_Int_Property_ارزښت(prop, value, *ec);
}

U_CAPI فراغ U_EXPORT2
uset_applyPropertyAlias(USet* سېټ,
                        ثابت char16_t *prop, int32_t prop_اوږدتیا,
                        ثابت char16_t *value, int32_t ارزښت_اوږدتیا,
            UError_کوډ* ec) {

    UnicodeString p(prop, prop_اوږدتیا);
    UnicodeString v(value, ارزښت_اوږدتیا);

    ((Unicode_سېټ*) سېټ)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resembles_نمونه(ثابت char16_t *نمونه, int32_t نمونه_اوږدتیا,
                      int32_t pos) {

    UnicodeString pat(نمونه, نمونه_اوږدتیا);

    أرجع ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            Unicode_سېټ::resembles_نمونه(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_منزل_نمونه(ثابت USet* سېټ,
               char16_t* پایله, int32_t پایله_ظرفیت,
               UBool escapeUnprintable,
               UError_کوډ* ec) {
    UnicodeString pat;
    ((ثابت Unicode_سېټ*) سېټ)->منزل_نمونه(pat, escapeUnprintable);
    أرجع pat.extract(پایله, پایله_ظرفیت, *ec);
}

U_CAPI فراغ U_EXPORT2
uset_closeOver(USet* سېټ, int32_t ځانګړتیاوې) {
    ((Unicode_سېټ*) سېټ)->Unicode_سېټ::closeOver(ځانګړتیاوې);
}
