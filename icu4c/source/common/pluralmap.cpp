// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "pluralmap.h"

U_NAMESPACE_BEGIN

स्थिर स्थिरांक अक्षर * स्थिरांक gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(स्थिरांक अक्षर *pluralForm) {
    साठी (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        जर (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            परत static_cast<Category>(i);
        }
    }
    परत NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(स्थिरांक UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    परत U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

स्थिरांक अक्षर *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    परत (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

