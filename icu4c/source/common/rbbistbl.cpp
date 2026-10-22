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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/parsepos.h"

#ଅନ୍ତର୍ଭାଵ "cstr.h"
#ଅନ୍ତର୍ଭାଵ "rbbinode.h"
#ଅନ୍ତର୍ଭାଵ "rbbirb.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV RBBISymbolTableEntry_deleter(ଶୂନ୍ୟ *p) {
    icu::RBBISymbolTableEntry *px = (icu::RBBISymbolTableEntry *)p;
    delete px;
}
U_CDECL_END



U_NAMESPACE_BEGIN

RBBISymbolTable::RBBISymbolTable(RBBIRuleScanner *rs, ଅଚଳ UnicodeString &rules, UErrorCode &status)
    : fRules(rules), fRuleScanner(rs)
{
    fHashTable       = nullptr;

    fHashTable = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, &status);
    // uhash_open checks status
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    uhash_set_ମୂଲ୍ୟ_Deleter(fHashTable, RBBISymbolTableEntry_deleter);
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
ଅଚଳ UnicodeString  *RBBISymbolTable::lookup(ଅଚଳ UnicodeString& s) ଅଚଳ
{
    ଅଚଳ RBBISymbolTableEntry* ଅଚଳ el =
        static_cast<ଅଚଳ RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    ଯଦି (el == nullptr) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ RBBINode& ପଦସମଷ୍ଟି_ଗଣ୍ଠି = *el->val->f_ବାମ_Child; // Root node of expression for variable
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
    ଫେରନ୍ତୁ &ପଦସମଷ୍ଟି_ଗଣ୍ଠି.fText;
}

ଅଚଳ UnicodeSet* RBBISymbolTable::lookupSet(ଅଚଳ UnicodeString& s) ଅଚଳ {
    ଅଚଳ RBBISymbolTableEntry* ଅଚଳ el = static_cast<ଅଚଳ RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    ଯଦି (el == nullptr) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ RBBINode& ପଦସମଷ୍ଟି_ଗଣ୍ଠି = *el->val->f_ବାମ_Child;
    ଯଦି (ପଦସମଷ୍ଟି_ଗଣ୍ଠି.fType == RBBINode::setRef) {
        ଫେରନ୍ତୁ ପଦସମଷ୍ଟି_ଗଣ୍ଠି.f_ବାମ_Child->f_ନିବେଶ_Set;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ nullptr;
    }
}



//  No longer used, see ICU-23297.
ଅଚଳ UnicodeFunctor* RBBISymbolTable::lookupMatcher(UChar32 /*ch*/) ଅଚଳ {
    ଫେରନ୍ତୁ nullptr;
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
UnicodeString   RBBISymbolTable::parseReference(ଅଚଳ UnicodeString& text,
                                                ParsePosition& pos, int32_t limit) ଅଚଳ
{
    int32_t start = pos.getIndex();
    int32_t i = start;
    UnicodeString result;
    ଯେତେବେଳେ (i < limit) {
        char16_t c = text.charAt(i);
        ଯଦି ((i==start && !u_isIDStart(c)) || !u_isIDPart(c)) {
            ଭାଙ୍ଗ;
        }
        ++i;
    }
    ଯଦି (i == start) { // No valid name chars
        ଫେରନ୍ତୁ result; // Indicate failure with empty string
    }
    pos.setIndex(i);
    text.extractBetween(start, i, result);
    ଫେରନ୍ତୁ result;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbolTable::lookup_ଗଣ୍ଠି(ଅଚଳ UnicodeString &ଚାବି) ଅଚଳ{

    RBBINode             *ret_ଗଣ୍ଠି = nullptr;
    RBBISymbolTableEntry *el;

    el = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &ଚାବି));
    ଯଦି (el != nullptr) {
        ret_ଗଣ୍ଠି = el->val;
    }
    ଫେରନ୍ତୁ ret_ଗଣ୍ଠି;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
ଶୂନ୍ୟ            RBBISymbolTable::addEntry  (ଅଚଳ UnicodeString &ଚାବି, RBBINode *val, UErrorCode &err) {
    RBBISymbolTableEntry *e;
    /* test for buffer overflows */
    ଯଦି (U_FAILURE(err)) {
        ଫେରନ୍ତୁ;
    }
    e = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &ଚାବି));
    ଯଦି (e != nullptr) {
        err = U_BRK_VARIABLE_REDFINITION;
        ଫେରନ୍ତୁ;
    }

    e = ନୂତନ RBBISymbolTableEntry;
    ଯଦି (e == nullptr) {
        err = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    e->ଚାବି = ଚାବି;
    e->val = val;
    uhash_put( fHashTable, &e->ଚାବି, e, &err);
}


RBBISymbolTableEntry::RBBISymbolTableEntry() : UMemory(), ଚାବି(), val(nullptr) {}

RBBISymbolTableEntry::~RBBISymbolTableEntry() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    delete val->f_ବାମ_Child;
    val->f_ବାମ_Child = nullptr;

    delete  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#ifdef RBBI_DEBUG
ଶୂନ୍ୟ RBBISymbolTable::rbbiSymtablePrint() ଅଚଳ {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_FIRST;
    ଅଚଳ UHashElement  *e   = nullptr;
    ପାଇଁ (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        ଯଦି (e == nullptr ) {
            ଭାଙ୍ଗ;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->ଚାବି)(), (ଶୂନ୍ୟ *)s->val, s->val->f_Serial_ଅଙ୍କ);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_ବାମ_Child->fText)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    ପାଇଁ (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        ଯଦି (e == nullptr ) {
            ଭାଙ୍ଗ;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;
        RBBIDebugPrintf("%s\n", CStr(s->ଚାବି)());
        RBBINode::print_ବୃକ୍ଷ(s->val, ସତ୍ୟ);
        RBBINode::print_ବୃକ୍ଷ(s->val->f_ବାମ_Child, ମିଥ୍ୟା);
        RBBIDebugPrintf("\n");
    }
}
#endif





U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
