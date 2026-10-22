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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/unistr.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/parsepos.h"

#समावेश "cstr.h"
#समावेश "rbbinode.h"
#समावेश "rbbirb.h"
#समावेश "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
अटल व्योम U_CALLCONV RBBISymbolTableEntry_deleter(व्योम *p) {
    icu::RBBISymbolTableEntry *px = (icu::RBBISymbolTableEntry *)p;
    मिटाओ px;
}
U_CDECL_END



U_NAMESPACE_BEGIN

RBBISymbolTable::RBBISymbolTable(RBBIRuleScanner *rs, स्थिर UnicodeString &rules, UErrorCode &status)
    : fRules(rules), fRuleScanner(rs)
{
    fHashTable       = nullptr;

    fHashTable = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, &status);
    // uhash_open checks status
    अगर (U_FAILURE(status)) {
        वापस;
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
स्थिर UnicodeString  *RBBISymbolTable::lookup(स्थिर UnicodeString& s) स्थिर
{
    स्थिर RBBISymbolTableEntry* स्थिर el =
        static_cast<स्थिर RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    अगर (el == nullptr) {
        वापस nullptr;
    }
    स्थिर RBBINode& व्यंजक_गाँठ = *el->val->f_बायाँ_Child; // Root node of expression for variable
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
    वापस &व्यंजक_गाँठ.fText;
}

स्थिर UnicodeSet* RBBISymbolTable::lookupSet(स्थिर UnicodeString& s) स्थिर {
    स्थिर RBBISymbolTableEntry* स्थिर el = static_cast<स्थिर RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    अगर (el == nullptr) {
        वापस nullptr;
    }
    स्थिर RBBINode& व्यंजक_गाँठ = *el->val->f_बायाँ_Child;
    अगर (व्यंजक_गाँठ.fType == RBBINode::setRef) {
        वापस व्यंजक_गाँठ.f_बायाँ_Child->f_निवेश_Set;
    } अन्यथा {
        वापस nullptr;
    }
}



//  No longer used, see ICU-23297.
स्थिर UnicodeFunctor* RBBISymbolTable::lookupMatcher(UChar32 /*ch*/) स्थिर {
    वापस nullptr;
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
UnicodeString   RBBISymbolTable::parseReference(स्थिर UnicodeString& text,
                                                ParsePosition& pos, int32_t limit) स्थिर
{
    int32_t start = pos.get_सूचकांक();
    int32_t i = start;
    UnicodeString परिणाम;
    जबतक (i < limit) {
        char16_t c = text.charAt(i);
        अगर ((i==start && !u_isIDStart(c)) || !u_isIDPart(c)) {
            अवरोध;
        }
        ++i;
    }
    अगर (i == start) { // No valid name chars
        वापस परिणाम; // Indicate failure with empty string
    }
    pos.set_सूचकांक(i);
    text.extractBetween(start, i, परिणाम);
    वापस परिणाम;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbolTable::lookup_गाँठ(स्थिर UnicodeString &कुंजी) स्थिर{

    RBBINode             *ret_गाँठ = nullptr;
    RBBISymbolTableEntry *el;

    el = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &कुंजी));
    अगर (el != nullptr) {
        ret_गाँठ = el->val;
    }
    वापस ret_गाँठ;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
व्योम            RBBISymbolTable::addEntry  (स्थिर UnicodeString &कुंजी, RBBINode *val, UErrorCode &err) {
    RBBISymbolTableEntry *e;
    /* test for buffer overflows */
    अगर (U_FAILURE(err)) {
        वापस;
    }
    e = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &कुंजी));
    अगर (e != nullptr) {
        err = U_BRK_VARIABLE_REDFINITION;
        वापस;
    }

    e = नया RBBISymbolTableEntry;
    अगर (e == nullptr) {
        err = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    e->कुंजी = कुंजी;
    e->val = val;
    uhash_put( fHashTable, &e->कुंजी, e, &err);
}


RBBISymbolTableEntry::RBBISymbolTableEntry() : UMemory(), कुंजी(), val(nullptr) {}

RBBISymbolTableEntry::~RBBISymbolTableEntry() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    मिटाओ val->f_बायाँ_Child;
    val->f_बायाँ_Child = nullptr;

    मिटाओ  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#अगर_घोषित RBBI_DEBUG
व्योम RBBISymbolTable::rbbiSymtablePrint() स्थिर {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_FIRST;
    स्थिर UHashElement  *e   = nullptr;
    क्रम (;;) {
        e = uhash_अग्र_Element(fHashTable,  &pos);
        अगर (e == nullptr ) {
            अवरोध;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->कुंजी)(), (व्योम *)s->val, s->val->f_Serial_अंक);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_बायाँ_Child->fText)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    क्रम (;;) {
        e = uhash_अग्र_Element(fHashTable,  &pos);
        अगर (e == nullptr ) {
            अवरोध;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;
        RBBIDebugPrintf("%s\n", CStr(s->कुंजी)());
        RBBINode::print_वृक्ष(s->val, true);
        RBBINode::print_वृक्ष(s->val->f_बायाँ_Child, false);
        RBBIDebugPrintf("\n");
    }
}
#पूर्ण_अगर





U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */
