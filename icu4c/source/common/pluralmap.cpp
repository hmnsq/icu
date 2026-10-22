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

ایستا ثابت نویسه * ثابت gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(ثابت نویسه *pluralForm) {
    برای (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        اگر (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            بازگشت static_cast<Category>(i);
        }
    }
    بازگشت NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(ثابت UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    بازگشت U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

ثابت نویسه *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    بازگشت (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

