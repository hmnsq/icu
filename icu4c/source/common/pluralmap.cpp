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

স্থির ধ্রুব বর্ণ * ধ্রুব gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(ধ্রুব বর্ণ *pluralForm) {
    প্রতি (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        যদি (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            প্রত্যাগম static_cast<Category>(i);
        }
    }
    প্রত্যাগম NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(ধ্রুব UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    প্রত্যাগম U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

ধ্রুব বর্ণ *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    প্রত্যাগম (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

