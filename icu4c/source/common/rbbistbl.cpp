// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
//  file:  rbbistbl.cpp    Implementation of the ICU RBBISymbolTable class
//
/*
***************************************************************************
*   Copyright (C) 2002-2014 International Business Machines Corporation
*   and others. All rights reserved.
***************************************************************************
*/

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/parsepos.h"

#अन्तर्भाव "cstr.h"
#अन्तर्भाव "rbbinode.h"
#अन्तर्भाव "rbbirb.h"
#अन्तर्भाव "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV RBBISymbolTableEntry_deleter(शून्य *p) {
    icu::RBBISymbolTableEntry *px = (icu::RBBISymbolTableEntry *)p;
    delete px;
}
U_CDECL_END



U_NAMESPACE_BEGIN

RBBISymbolTable::RBBISymbolTable(RBBIRuleScanner *rs, स्थिरांक UnicodeString &rules, UErrorCode &status)
    : fRules(rules), fRuleScanner(rs)
{
    fHashTable       = nullptr;

    fHashTable = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, &status);
    // uhash_open checks status
    जर (U_FAILURE(status)) {
        परत;
    }
    uhash_set_मूल्य_Deleter(fHashTable, RBBISymbolTableEntry_deleter);
}



RBBISymbolTable::~RBBISymbolTable()
{
    uhash_close(fHashTable);
}


//
//  RBBISymbolTable::lookup       This function from the abstract symbol table interface
//                                looks up a variable name and returns a UnicodeString
//                                containing the substitution text.
//
//                                The variable name does NOT include the leading $.
//
स्थिरांक UnicodeString  *RBBISymbolTable::lookup(स्थिरांक UnicodeString& s) स्थिरांक
{
    स्थिरांक RBBISymbolTableEntry* स्थिरांक el =
        static_cast<स्थिरांक RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    जर (el == nullptr) {
        परत nullptr;
    }
    स्थिरांक RBBINode& अभिव्यक्ती_गाठ = *el->val->f_डावा_Child; // Root node of expression for variable
    // Return the original source string for the expression.
    // Note that for set-valued variables used in UnicodeSet expressions, this would be rejected by
    // the UnicodeSet parser if the source itself contains variable references.  For instance, with
    //     $CaseIgnorable   = [[:Mn:][:Me:][:Cf:][:Lm:][:Sk:] \u0027 \u00AD \u2019];
    //     $Cased = [[:Upper_Case:][:Lower_Case:][:Lt:] - $CaseIgnorable];
    // If lookupSet were not overridden, when parsing the right-hand side of
    //     $NotCased        = [[^ $Cased] - $CaseIgnorable];
    // there would be a call to lookup("Cased") which would return
    //     "[[:Upper_Case:][:Lower_Case:][:Lt:]-$CaseIgnorable]". This contains a variable, which is
    // disallowed by the UnicodeSet parser inside a variable expansion.
    // However, set-valued variables are pre-parsed, and returned by lookupSet instead, so this call
    // to lookup() never happens; instead, lookupSet("CaseIgnorable") is called when computing
    // $Cased and returns the non-null value of $CaseIgnorable, and then when computing $NotCased,
    // lookupSet("Cased") returns the value computed for $Cased.
    परत &अभिव्यक्ती_गाठ.fText;
}

स्थिरांक UnicodeSet* RBBISymbolTable::lookupSet(स्थिरांक UnicodeString& s) स्थिरांक {
    स्थिरांक RBBISymbolTableEntry* स्थिरांक el = static_cast<स्थिरांक RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    जर (el == nullptr) {
        परत nullptr;
    }
    स्थिरांक RBBINode& अभिव्यक्ती_गाठ = *el->val->f_डावा_Child;
    जर (अभिव्यक्ती_गाठ.fType == RBBINode::setRef) {
        परत अभिव्यक्ती_गाठ.f_डावा_Child->f_आदान_Set;
    } अन्यथा {
        परत nullptr;
    }
}



//  No longer used, see ICU-23297.
स्थिरांक UnicodeFunctor* RBBISymbolTable::lookupMatcher(UChar32 /*ch*/) स्थिरांक {
    परत nullptr;
}

//
// RBBISymbolTable::parseReference   This function from the abstract symbol table interface
//                                   looks for a $variable name in the source text.
//                                   It does not look it up, only scans for it.
//                                   It is used by the UnicodeSet parser.
//
//                                   This implementation is lifted pretty much verbatim
//                                   from the rules based transliterator implementation.
//                                   I didn't see an obvious way of sharing it.
//
UnicodeString   RBBISymbolTable::parseReference(स्थिरांक UnicodeString& text,
                                                ParsePosition& pos, int32_t limit) स्थिरांक
{
    int32_t start = pos.getIndex();
    int32_t i = start;
    UnicodeString result;
    पर्यंत (i < limit) {
        char16_t c = text.charAt(i);
        जर ((i==start && !u_isIDStart(c)) || !u_isIDPart(c)) {
            थांबा;
        }
        ++i;
    }
    जर (i == start) { // No valid name chars
        परत result; // Indicate failure with empty string
    }
    pos.setIndex(i);
    text.extractBetween(start, i, result);
    परत result;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbolTable::lookup_गाठ(स्थिरांक UnicodeString &किल्ली) स्थिरांक{

    RBBINode             *ret_गाठ = nullptr;
    RBBISymbolTableEntry *el;

    el = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &किल्ली));
    जर (el != nullptr) {
        ret_गाठ = el->val;
    }
    परत ret_गाठ;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
शून्य            RBBISymbolTable::addEntry  (स्थिरांक UnicodeString &किल्ली, RBBINode *val, UErrorCode &err) {
    RBBISymbolTableEntry *e;
    /* test for buffer overflows */
    जर (U_FAILURE(err)) {
        परत;
    }
    e = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &किल्ली));
    जर (e != nullptr) {
        err = U_BRK_VARIABLE_REDFINITION;
        परत;
    }

    e = नवीन RBBISymbolTableEntry;
    जर (e == nullptr) {
        err = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    e->किल्ली = किल्ली;
    e->val = val;
    uhash_put( fHashTable, &e->किल्ली, e, &err);
}


RBBISymbolTableEntry::RBBISymbolTableEntry() : UMemory(), किल्ली(), val(nullptr) {}

RBBISymbolTableEntry::~RBBISymbolTableEntry() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    delete val->f_डावा_Child;
    val->f_डावा_Child = nullptr;

    delete  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#ifdef RBBI_DEBUG
शून्य RBBISymbolTable::rbbiSymtablePrint() स्थिरांक {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_FIRST;
    स्थिरांक UHashElement  *e   = nullptr;
    साठी (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        जर (e == nullptr ) {
            थांबा;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->किल्ली)(), (शून्य *)s->val, s->val->f_Serial_अंक);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_डावा_Child->fText)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    साठी (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        जर (e == nullptr ) {
            थांबा;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;
        RBBIDebugPrintf("%s\n", CStr(s->किल्ली)());
        RBBINode::print_वृक्ष(s->val, सत्य);
        RBBINode::print_वृक्ष(s->val->f_डावा_Child, असत्य);
        RBBIDebugPrintf("\n");
    }
}
#endif





U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
