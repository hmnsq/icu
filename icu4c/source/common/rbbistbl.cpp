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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/parsepos.h"

#അന്തര്ഭാവ "cstr.h"
#അന്തര്ഭാവ "rbbinode.h"
#അന്തര്ഭാവ "rbbirb.h"
#അന്തര്ഭാവ "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
സ്ഥിതം ശൂന്യം U_CALLCONV RBBISymbolTableEntry_deleter(ശൂന്യം *p) {
    icu::RBBISymbolTableEntry *px = (icu::RBBISymbolTableEntry *)p;
    delete px;
}
U_CDECL_END



U_NAMESPACE_BEGIN

RBBISymbolTable::RBBISymbolTable(RBBIRuleScanner *rs, സ്ഥിരം UnicodeString &rules, UErrorCode &status)
    : fRules(rules), fRuleScanner(rs)
{
    fHashTable       = nullptr;

    fHashTable = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, &status);
    // uhash_open checks status
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    uhash_set_മൂല്യം_Deleter(fHashTable, RBBISymbolTableEntry_deleter);
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
സ്ഥിരം UnicodeString  *RBBISymbolTable::lookup(സ്ഥിരം UnicodeString& s) സ്ഥിരം
{
    സ്ഥിരം RBBISymbolTableEntry* സ്ഥിരം el =
        static_cast<സ്ഥിരം RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    എങ്കിൽ (el == nullptr) {
        തിരികെ nullptr;
    }
    സ്ഥിരം RBBINode& വ്യഞ്ജകം_കണ്ണി = *el->val->f_ഇടത്_Child; // Root node of expression for variable
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
    തിരികെ &വ്യഞ്ജകം_കണ്ണി.fText;
}

സ്ഥിരം UnicodeSet* RBBISymbolTable::lookupSet(സ്ഥിരം UnicodeString& s) സ്ഥിരം {
    സ്ഥിരം RBBISymbolTableEntry* സ്ഥിരം el = static_cast<സ്ഥിരം RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    എങ്കിൽ (el == nullptr) {
        തിരികെ nullptr;
    }
    സ്ഥിരം RBBINode& വ്യഞ്ജകം_കണ്ണി = *el->val->f_ഇടത്_Child;
    എങ്കിൽ (വ്യഞ്ജകം_കണ്ണി.fType == RBBINode::setRef) {
        തിരികെ വ്യഞ്ജകം_കണ്ണി.f_ഇടത്_Child->f_നിവേശം_Set;
    } അല്ലെങ്കിൽ {
        തിരികെ nullptr;
    }
}



//  No longer used, see ICU-23297.
സ്ഥിരം UnicodeFunctor* RBBISymbolTable::lookupMatcher(UChar32 /*ch*/) സ്ഥിരം {
    തിരികെ nullptr;
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
UnicodeString   RBBISymbolTable::parseReference(സ്ഥിരം UnicodeString& text,
                                                ParsePosition& pos, int32_t limit) സ്ഥിരം
{
    int32_t start = pos.getIndex();
    int32_t i = start;
    UnicodeString result;
    ആകുമ്പോൾ (i < limit) {
        char16_t c = text.charAt(i);
        എങ്കിൽ ((i==start && !u_isIDStart(c)) || !u_isIDPart(c)) {
            നിർത്തുക;
        }
        ++i;
    }
    എങ്കിൽ (i == start) { // No valid name chars
        തിരികെ result; // Indicate failure with empty string
    }
    pos.setIndex(i);
    text.extractBetween(start, i, result);
    തിരികെ result;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbolTable::lookup_കണ്ണി(സ്ഥിരം UnicodeString &താക്കോൽ) സ്ഥിരം{

    RBBINode             *ret_കണ്ണി = nullptr;
    RBBISymbolTableEntry *el;

    el = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &താക്കോൽ));
    എങ്കിൽ (el != nullptr) {
        ret_കണ്ണി = el->val;
    }
    തിരികെ ret_കണ്ണി;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
ശൂന്യം            RBBISymbolTable::addEntry  (സ്ഥിരം UnicodeString &താക്കോൽ, RBBINode *val, UErrorCode &err) {
    RBBISymbolTableEntry *e;
    /* test for buffer overflows */
    എങ്കിൽ (U_FAILURE(err)) {
        തിരികെ;
    }
    e = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &താക്കോൽ));
    എങ്കിൽ (e != nullptr) {
        err = U_BRK_VARIABLE_REDFINITION;
        തിരികെ;
    }

    e = പുതിയ RBBISymbolTableEntry;
    എങ്കിൽ (e == nullptr) {
        err = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    e->താക്കോൽ = താക്കോൽ;
    e->val = val;
    uhash_put( fHashTable, &e->താക്കോൽ, e, &err);
}


RBBISymbolTableEntry::RBBISymbolTableEntry() : UMemory(), താക്കോൽ(), val(nullptr) {}

RBBISymbolTableEntry::~RBBISymbolTableEntry() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    delete val->f_ഇടത്_Child;
    val->f_ഇടത്_Child = nullptr;

    delete  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#ifdef RBBI_DEBUG
ശൂന്യം RBBISymbolTable::rbbiSymtablePrint() സ്ഥിരം {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_FIRST;
    സ്ഥിരം UHashElement  *e   = nullptr;
    വേണ്ടി (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        എങ്കിൽ (e == nullptr ) {
            നിർത്തുക;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->താക്കോൽ)(), (ശൂന്യം *)s->val, s->val->f_Serial_അക്കം);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_ഇടത്_Child->fText)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    വേണ്ടി (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        എങ്കിൽ (e == nullptr ) {
            നിർത്തുക;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;
        RBBIDebugPrintf("%s\n", CStr(s->താക്കോൽ)());
        RBBINode::print_വൃക്ഷം(s->val, സത്യം);
        RBBINode::print_വൃക്ഷം(s->val->f_ഇടത്_Child, അസത്യം);
        RBBIDebugPrintf("\n");
    }
}
#endif





U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
