// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "pluralmap.h"

U_NAMESPACE_BEGIN

স্থির ধ্রুবক অক্ষর * ধ্রুবক gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(ধ্রুবক অক্ষর *pluralForm) {
    জন্য (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        যদি (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            ফেরত static_cast<Category>(i);
        }
    }
    ফেরত NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(ধ্রুবক UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    ফেরত U_FAILURE(status) ? NONE : toCategory(cCategory.তথ্য());
}

ধ্রুবক অক্ষর *PluralMapBase::getCategoryName(Category c) {
    int32_t সূচক = c;
    ফেরত (সূচক < 0 || সূচক >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[সূচক];
}


U_NAMESPACE_END

