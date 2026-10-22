// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 * Copyright (C) 2015, International Business Machines Corporation and
 * others. All Rights Reserved.
 */

#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "pluralmap.h"

U_NAMESPACE_BEGIN

స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి gPluralForms[] = {
        "other", "zero", "one", "two", "few", "many"};

Plural_Map_పునాది::Category
Plural_Map_పునాది::గమ్యం_Category(స్థిరరాశి అక్షరం *pluralForm) {
    కొరకు (int32_t i = 0; i < UPRV_LENGTHOF(gPluralForms); ++i) {
        ఒకవేళ (uprv_strcmp(pluralForm, gPluralForms[i]) == 0) {
            తిరిగి static_cast<Category>(i);
        }
    }
    తిరిగి NONE;
}

Plural_Map_పునాది::Category
Plural_Map_పునాది::గమ్యం_Category(స్థిరరాశి UnicodeString &pluralForm) {
    CharString cCategory;
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    cCategory.appendInvariantChars(pluralForm, స్థితి_2);    
    తిరిగి U_FAILURE(స్థితి_2) ? NONE : గమ్యం_Category(cCategory.దత్తాంశం());
}

స్థిరరాశి అక్షరం *Plural_Map_పునాది::getCategoryName(Category c) {
    int32_t సూచీ = c;
    తిరిగి (సూచీ < 0 || సూచీ >= UPRV_LENGTHOF(gPluralForms)) ?
            శూన్యసూచిక : gPluralForms[సూచీ];
}


U_NAMESPACE_చివర

