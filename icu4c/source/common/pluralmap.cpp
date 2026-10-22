// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "pluralmap.h"

U_NAMESPACE_BEGIN

સ્થિર અચલ અક્ષર * અચલ gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(અચલ અક્ષર *pluralForm) {
    માટે (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        જો (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            પાછા static_cast<Category>(i);
        }
    }
    પાછા NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(અચલ UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    પાછા U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

અચલ અક્ષર *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    પાછા (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

