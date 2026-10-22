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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/parsepos.h"

#અન્તર્ભાવ "cstr.h"
#અન્તર્ભાવ "rbbinode.h"
#અન્તર્ભાવ "rbbirb.h"
#અન્તર્ભાવ "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
સ્થિર શૂન્ય U_CALLCONV RBBISymbolTableEntry_deleter(શૂન્ય *p) {
    icu::RBBISymbolTableEntry *px = (icu::RBBISymbolTableEntry *)p;
    delete px;
}
U_CDECL_END



U_NAMESPACE_BEGIN

RBBISymbolTable::RBBISymbolTable(RBBIRuleScanner *rs, અચલ UnicodeString &rules, UErrorCode &status)
    : fRules(rules), fRuleScanner(rs)
{
    fHashTable       = nullptr;

    fHashTable = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, &status);
    // uhash_open checks status
    જો (U_FAILURE(status)) {
        પાછા;
    }
    uhash_set_કિંમત_Deleter(fHashTable, RBBISymbolTableEntry_deleter);
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
અચલ UnicodeString  *RBBISymbolTable::lookup(અચલ UnicodeString& s) અચલ
{
    અચલ RBBISymbolTableEntry* અચલ el =
        static_cast<અચલ RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    જો (el == nullptr) {
        પાછા nullptr;
    }
    અચલ RBBINode& પદાવલિ_ગાંઠ = *el->val->f_ડાબું_Child; // Root node of expression for variable
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
    પાછા &પદાવલિ_ગાંઠ.fText;
}

અચલ UnicodeSet* RBBISymbolTable::lookupSet(અચલ UnicodeString& s) અચલ {
    અચલ RBBISymbolTableEntry* અચલ el = static_cast<અચલ RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    જો (el == nullptr) {
        પાછા nullptr;
    }
    અચલ RBBINode& પદાવલિ_ગાંઠ = *el->val->f_ડાબું_Child;
    જો (પદાવલિ_ગાંઠ.fType == RBBINode::setRef) {
        પાછા પદાવલિ_ગાંઠ.f_ડાબું_Child->f_આગત_Set;
    } નહીંતર {
        પાછા nullptr;
    }
}



//  No longer used, see ICU-23297.
અચલ UnicodeFunctor* RBBISymbolTable::lookupMatcher(UChar32 /*ch*/) અચલ {
    પાછા nullptr;
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
UnicodeString   RBBISymbolTable::parseReference(અચલ UnicodeString& text,
                                                ParsePosition& pos, int32_t limit) અચલ
{
    int32_t start = pos.getIndex();
    int32_t i = start;
    UnicodeString result;
    જ્યારે (i < limit) {
        char16_t c = text.charAt(i);
        જો ((i==start && !u_isIDStart(c)) || !u_isIDPart(c)) {
            તોડો;
        }
        ++i;
    }
    જો (i == start) { // No valid name chars
        પાછા result; // Indicate failure with empty string
    }
    pos.setIndex(i);
    text.extractBetween(start, i, result);
    પાછા result;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbolTable::lookup_ગાંઠ(અચલ UnicodeString &ચાવી) અચલ{

    RBBINode             *ret_ગાંઠ = nullptr;
    RBBISymbolTableEntry *el;

    el = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &ચાવી));
    જો (el != nullptr) {
        ret_ગાંઠ = el->val;
    }
    પાછા ret_ગાંઠ;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
શૂન્ય            RBBISymbolTable::addEntry  (અચલ UnicodeString &ચાવી, RBBINode *val, UErrorCode &err) {
    RBBISymbolTableEntry *e;
    /* test for buffer overflows */
    જો (U_FAILURE(err)) {
        પાછા;
    }
    e = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &ચાવી));
    જો (e != nullptr) {
        err = U_BRK_VARIABLE_REDFINITION;
        પાછા;
    }

    e = નવું RBBISymbolTableEntry;
    જો (e == nullptr) {
        err = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    e->ચાવી = ચાવી;
    e->val = val;
    uhash_put( fHashTable, &e->ચાવી, e, &err);
}


RBBISymbolTableEntry::RBBISymbolTableEntry() : UMemory(), ચાવી(), val(nullptr) {}

RBBISymbolTableEntry::~RBBISymbolTableEntry() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    delete val->f_ડાબું_Child;
    val->f_ડાબું_Child = nullptr;

    delete  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#ifdef RBBI_DEBUG
શૂન્ય RBBISymbolTable::rbbiSymtablePrint() અચલ {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_FIRST;
    અચલ UHashElement  *e   = nullptr;
    માટે (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        જો (e == nullptr ) {
            તોડો;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->ચાવી)(), (શૂન્ય *)s->val, s->val->f_Serial_અંક);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_ડાબું_Child->fText)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    માટે (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        જો (e == nullptr ) {
            તોડો;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;
        RBBIDebugPrintf("%s\n", CStr(s->ચાવી)());
        RBBINode::print_વૃક્ષ(s->val, સાચું);
        RBBINode::print_વૃક્ષ(s->val->f_ડાબું_Child, ખોટું);
        RBBIDebugPrintf("\n");
    }
}
#endif





U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
