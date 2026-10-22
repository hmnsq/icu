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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/parsepos.h"

#அந்தர்பாவ "cstr.h"
#அந்தர்பாவ "rbbinode.h"
#அந்தர்பாவ "rbbirb.h"
#அந்தர்பாவ "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
நிலையான வெறுமை U_CALLCONV RBBISymbolTableEntry_deleter(வெறுமை *p) {
    icu::RBBISymbolTableEntry *px = (icu::RBBISymbolTableEntry *)p;
    delete px;
}
U_CDECL_END



U_NAMESPACE_BEGIN

RBBISymbolTable::RBBISymbolTable(RBBIRuleScanner *rs, மாறிலி UnicodeString &rules, UErrorCode &status)
    : fRules(rules), fRuleScanner(rs)
{
    fHashTable       = nullptr;

    fHashTable = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, &status);
    // uhash_open checks status
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    uhash_set_மதிப்பு_Deleter(fHashTable, RBBISymbolTableEntry_deleter);
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
மாறிலி UnicodeString  *RBBISymbolTable::lookup(மாறிலி UnicodeString& s) மாறிலி
{
    மாறிலி RBBISymbolTableEntry* மாறிலி el =
        static_cast<மாறிலி RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    எனில் (el == nullptr) {
        திரும்பு nullptr;
    }
    மாறிலி RBBINode& கோவை_முனை = *el->val->f_இடது_Child; // Root node of expression for variable
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
    திரும்பு &கோவை_முனை.fText;
}

மாறிலி UnicodeSet* RBBISymbolTable::lookupSet(மாறிலி UnicodeString& s) மாறிலி {
    மாறிலி RBBISymbolTableEntry* மாறிலி el = static_cast<மாறிலி RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    எனில் (el == nullptr) {
        திரும்பு nullptr;
    }
    மாறிலி RBBINode& கோவை_முனை = *el->val->f_இடது_Child;
    எனில் (கோவை_முனை.fType == RBBINode::setRef) {
        திரும்பு கோவை_முனை.f_இடது_Child->f_உள்ளீடு_Set;
    } இல்லையெனில் {
        திரும்பு nullptr;
    }
}



//  No longer used, see ICU-23297.
மாறிலி UnicodeFunctor* RBBISymbolTable::lookupMatcher(UChar32 /*ch*/) மாறிலி {
    திரும்பு nullptr;
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
UnicodeString   RBBISymbolTable::parseReference(மாறிலி UnicodeString& text,
                                                ParsePosition& pos, int32_t limit) மாறிலி
{
    int32_t start = pos.getIndex();
    int32_t i = start;
    UnicodeString result;
    போது (i < limit) {
        char16_t c = text.charAt(i);
        எனில் ((i==start && !u_isIDStart(c)) || !u_isIDPart(c)) {
            நிறுத்து;
        }
        ++i;
    }
    எனில் (i == start) { // No valid name chars
        திரும்பு result; // Indicate failure with empty string
    }
    pos.setIndex(i);
    text.extractBetween(start, i, result);
    திரும்பு result;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbolTable::lookup_முனை(மாறிலி UnicodeString &சாவி) மாறிலி{

    RBBINode             *ret_முனை = nullptr;
    RBBISymbolTableEntry *el;

    el = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &சாவி));
    எனில் (el != nullptr) {
        ret_முனை = el->val;
    }
    திரும்பு ret_முனை;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
வெறுமை            RBBISymbolTable::addEntry  (மாறிலி UnicodeString &சாவி, RBBINode *val, UErrorCode &err) {
    RBBISymbolTableEntry *e;
    /* test for buffer overflows */
    எனில் (U_FAILURE(err)) {
        திரும்பு;
    }
    e = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &சாவி));
    எனில் (e != nullptr) {
        err = U_BRK_VARIABLE_REDFINITION;
        திரும்பு;
    }

    e = புதிய RBBISymbolTableEntry;
    எனில் (e == nullptr) {
        err = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    e->சாவி = சாவி;
    e->val = val;
    uhash_put( fHashTable, &e->சாவி, e, &err);
}


RBBISymbolTableEntry::RBBISymbolTableEntry() : UMemory(), சாவி(), val(nullptr) {}

RBBISymbolTableEntry::~RBBISymbolTableEntry() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    delete val->f_இடது_Child;
    val->f_இடது_Child = nullptr;

    delete  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#ifdef RBBI_DEBUG
வெறுமை RBBISymbolTable::rbbiSymtablePrint() மாறிலி {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_FIRST;
    மாறிலி UHashElement  *e   = nullptr;
    க்கு (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        எனில் (e == nullptr ) {
            நிறுத்து;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->சாவி)(), (வெறுமை *)s->val, s->val->f_Serial_இலக்கம்);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_இடது_Child->fText)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    க்கு (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        எனில் (e == nullptr ) {
            நிறுத்து;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;
        RBBIDebugPrintf("%s\n", CStr(s->சாவி)());
        RBBINode::print_மரம்(s->val, உண்மை);
        RBBINode::print_மரம்(s->val->f_இடது_Child, பொய்);
        RBBIDebugPrintf("\n");
    }
}
#endif





U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
