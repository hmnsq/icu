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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/unistr.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/parsepos.h"

#ضمن "cstr.h"
#ضمن "rbbinode.h"
#ضمن "rbbirb.h"
#ضمن "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV RBBISymbolTableEntry_deleter(فراغ *p) {
    icu::RBBISymbolTableEntry *px = (icu::RBBISymbolTableEntry *)p;
    delete px;
}
U_CDECL_END



U_NAMESPACE_BEGIN

RBBISymbolTable::RBBISymbolTable(RBBIRuleScanner *rs, ثابت UnicodeString &rules, UErrorCode &status)
    : fRules(rules), fRuleScanner(rs)
{
    fHashTable       = nullptr;

    fHashTable = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, &status);
    // uhash_open checks status
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    uhash_set_قىممەت_Deleter(fHashTable, RBBISymbolTableEntry_deleter);
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
ثابت UnicodeString  *RBBISymbolTable::lookup(ثابت UnicodeString& s) ثابت
{
    ثابت RBBISymbolTableEntry* ثابت el =
        static_cast<ثابت RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    إذا (el == nullptr) {
        أرجع nullptr;
    }
    ثابت RBBINode& ئىپادە_تۈگۈن = *el->val->f_سول_Child; // Root node of expression for variable
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
    أرجع &ئىپادە_تۈگۈن.fText;
}

ثابت UnicodeSet* RBBISymbolTable::lookupSet(ثابت UnicodeString& s) ثابت {
    ثابت RBBISymbolTableEntry* ثابت el = static_cast<ثابت RBBISymbolTableEntry*>(uhash_get(fHashTable, &s));
    إذا (el == nullptr) {
        أرجع nullptr;
    }
    ثابت RBBINode& ئىپادە_تۈگۈن = *el->val->f_سول_Child;
    إذا (ئىپادە_تۈگۈن.fType == RBBINode::setRef) {
        أرجع ئىپادە_تۈگۈن.f_سول_Child->f_كىرگۈزۈش_Set;
    } وإلا {
        أرجع nullptr;
    }
}



//  No longer used, see ICU-23297.
ثابت UnicodeFunctor* RBBISymbolTable::lookupMatcher(UChar32 /*ch*/) ثابت {
    أرجع nullptr;
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
UnicodeString   RBBISymbolTable::parseReference(ثابت UnicodeString& text,
                                                ParsePosition& pos, int32_t limit) ثابت
{
    int32_t start = pos.getIndex();
    int32_t i = start;
    UnicodeString result;
    طالما (i < limit) {
        char16_t c = text.charAt(i);
        إذا ((i==start && !u_isIDStart(c)) || !u_isIDPart(c)) {
            اكسر;
        }
        ++i;
    }
    إذا (i == start) { // No valid name chars
        أرجع result; // Indicate failure with empty string
    }
    pos.setIndex(i);
    text.extractBetween(start, i, result);
    أرجع result;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbolTable::lookup_تۈگۈن(ثابت UnicodeString &ئاچقۇچ) ثابت{

    RBBINode             *ret_تۈگۈن = nullptr;
    RBBISymbolTableEntry *el;

    el = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &ئاچقۇچ));
    إذا (el != nullptr) {
        ret_تۈگۈن = el->val;
    }
    أرجع ret_تۈگۈن;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
فراغ            RBBISymbolTable::addEntry  (ثابت UnicodeString &ئاچقۇچ, RBBINode *val, UErrorCode &err) {
    RBBISymbolTableEntry *e;
    /* test for buffer overflows */
    إذا (U_FAILURE(err)) {
        أرجع;
    }
    e = static_cast<RBBISymbolTableEntry*>(uhash_get(fHashTable, &ئاچقۇچ));
    إذا (e != nullptr) {
        err = U_BRK_VARIABLE_REDFINITION;
        أرجع;
    }

    e = جديد RBBISymbolTableEntry;
    إذا (e == nullptr) {
        err = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    e->ئاچقۇچ = ئاچقۇچ;
    e->val = val;
    uhash_put( fHashTable, &e->ئاچقۇچ, e, &err);
}


RBBISymbolTableEntry::RBBISymbolTableEntry() : UMemory(), ئاچقۇچ(), val(nullptr) {}

RBBISymbolTableEntry::~RBBISymbolTableEntry() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    delete val->f_سول_Child;
    val->f_سول_Child = nullptr;

    delete  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#ifdef RBBI_DEBUG
فراغ RBBISymbolTable::rbbiSymtablePrint() ثابت {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_FIRST;
    ثابت UHashElement  *e   = nullptr;
    لكل (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        إذا (e == nullptr ) {
            اكسر;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->ئاچقۇچ)(), (فراغ *)s->val, s->val->f_Serial_رەقەم);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_سول_Child->fText)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    لكل (;;) {
        e = uhash_nextElement(fHashTable,  &pos);
        إذا (e == nullptr ) {
            اكسر;
        }
        RBBISymbolTableEntry  *s   = (RBBISymbolTableEntry *)e->value.pointer;
        RBBIDebugPrintf("%s\n", CStr(s->ئاچقۇچ)());
        RBBINode::print_دەرەخ(s->val, true);
        RBBINode::print_دەرەخ(s->val->f_سول_Child, خطأ);
        RBBIDebugPrintf("\n");
    }
}
#endif





U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
