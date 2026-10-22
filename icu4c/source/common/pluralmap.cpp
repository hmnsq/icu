// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#ضمن "unicode/unistr.h"
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "pluralmap.h"

U_NAMESPACE_BEGIN

ساكن ثابت حرف * ثابت gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

Plural_Map_بنياد::Category
Plural_Map_بنياد::منزل_Category(ثابت حرف *pluralForm) {
    لكل (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        إذا (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            أرجع static_cast<Category>(i);
        }
    }
    أرجع NONE;
}

Plural_Map_بنياد::Category
Plural_Map_بنياد::منزل_Category(ثابت UnicodeString &pluralForm) {
    CharString cCategory;
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, حالت);    
    أرجع U_FAILURE(حالت) ? NONE : منزل_Category(cCategory.معلومات());
}

ثابت حرف *Plural_Map_بنياد::getCategoryName(Category c) {
    int32_t اشاريو = c;
    أرجع (اشاريو < 0 || اشاريو >= UPRV_LENGTHOF(gPluralForms)) ?
            خالي_اشارو : gPluralForms[اشاريو];
}


U_NAMESPACE_پڇاڙي

