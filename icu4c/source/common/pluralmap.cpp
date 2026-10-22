// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#شامل "unicode/unistr.h"
#شامل "charstr.h"
#شامل "cstring.h"
#شامل "pluralmap.h"

U_NAMESPACE_BEGIN

ساکن ثابت حرف * ثابت gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

Plural_Map_بنیاد::Category
Plural_Map_بنیاد::مقصد_Category(ثابت حرف *pluralForm) {
    برائے (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        اگر (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            واپس static_cast<Category>(i);
        }
    }
    واپس NONE;
}

Plural_Map_بنیاد::Category
Plural_Map_بنیاد::مقصد_Category(ثابت UnicodeString &pluralForm) {
    CharString cCategory;
    UError_رمز حالت_2 = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, حالت_2);    
    واپس U_FAILURE(حالت_2) ? NONE : مقصد_Category(cCategory.ڈیٹا());
}

ثابت حرف *Plural_Map_بنیاد::getCategoryName(Category c) {
    int32_t اشاریہ = c;
    واپس (اشاریہ < 0 || اشاریہ >= UPRV_LENGTHOF(gPluralForms)) ?
            خالی_اشاریہ : gPluralForms[اشاریہ];
}


U_NAMESPACE_انتہا

