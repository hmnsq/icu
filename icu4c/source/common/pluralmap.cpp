// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#समावेश "unicode/unistr.h"
#समावेश "charstr.h"
#समावेश "cstring.h"
#समावेश "pluralmap.h"

U_NAMESPACE_BEGIN

अटल स्थिर अक्षर * स्थिर gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(स्थिर अक्षर *pluralForm) {
    क्रम (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        अगर (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            वापस static_cast<Category>(i);
        }
    }
    वापस NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(स्थिर UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    वापस U_FAILURE(status) ? NONE : toCategory(cCategory.तथ्य());
}

स्थिर अक्षर *PluralMapBase::getCategoryName(Category c) {
    int32_t सूचकांक = c;
    वापस (सूचकांक < 0 || सूचकांक >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[सूचकांक];
}


U_NAMESPACE_END

