// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "pluralmap.h"

U_NAMESPACE_BEGIN

സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

PluralMapBase::Category
PluralMapBase::toCategory(സ്ഥിരം അക്ഷരം *pluralForm) {
    വേണ്ടി (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        എങ്കിൽ (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            തിരികെ static_cast<Category>(i);
        }
    }
    തിരികെ NONE;
}

PluralMapBase::Category
PluralMapBase::toCategory(സ്ഥിരം UnicodeString &pluralForm) {
    CharString cCategory;
    UErrorCode status = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, status);    
    തിരികെ U_FAILURE(status) ? NONE : toCategory(cCategory.data());
}

സ്ഥിരം അക്ഷരം *PluralMapBase::getCategoryName(Category c) {
    int32_t index = c;
    തിരികെ (index < 0 || index >= UPRV_LENGTHOF(gPluralForms)) ?
            nullptr : gPluralForms[index];
}


U_NAMESPACE_END

