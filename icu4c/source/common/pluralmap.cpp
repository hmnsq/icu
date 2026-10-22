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

स्थिर अचर वर्ण * अचर gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(अचर वर्ण *pluralForm) {
    लागि (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        यदि (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            फर्काऊ static_cast<Category>(i);
        }
    }
    फर्काऊ NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(अचर UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    फर्काऊ U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

अचर वर्ण *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    फर्काऊ (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

