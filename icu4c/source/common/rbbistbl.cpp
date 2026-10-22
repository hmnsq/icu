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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/parsepos.h"

#అన్తర్భావ "cstr.h"
#అన్తర్భావ "rbbinode.h"
#అన్తర్భావ "rbbirb.h"
#అన్తర్భావ "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV RBBISymbol_పట్టిక_నమోదు_deleter(శూన్య *p) {
    icu::RBBISymbol_పట్టిక_నమోదు *px = (icu::RBBISymbol_పట్టిక_నమోదు *)p;
    తొలగించు px;
}
U_CDECL_చివర



U_NAMESPACE_BEGIN

RBBISymbol_పట్టిక::RBBISymbol_పట్టిక(RBBIRuleScanner *rs, స్థిరరాశి UnicodeString &నియమాలు, UError_సంకేతం &స్థితి_2)
    : f_నియమాలు(నియమాలు), f_నియమం_Scanner(rs)
{
    f_Hash_పట్టిక       = శూన్యసూచిక;

    f_Hash_పట్టిక = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, శూన్యసూచిక, &స్థితి_2);
    // uhash_open checks status
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    uhash_సమితి_విలువ_Deleter(f_Hash_పట్టిక, RBBISymbol_పట్టిక_నమోదు_deleter);
}



RBBISymbol_పట్టిక::~RBBISymbol_పట్టిక()
{
    uhash_close(f_Hash_పట్టిక);
}


//
//  RBBISymbolTable::lookup       This function from the abstract symbol table interface
//                                looks up a variable name and returns a UnicodeString
//                                containing the substitution text.
//
//                                The variable name does NOT include the leading $.
//
స్థిరరాశి UnicodeString  *RBBISymbol_పట్టిక::lookup(స్థిరరాశి UnicodeString& s) స్థిరరాశి
{
    స్థిరరాశి RBBISymbol_పట్టిక_నమోదు* స్థిరరాశి el =
        static_cast<స్థిరరాశి RBBISymbol_పట్టిక_నమోదు*>(uhash_get(f_Hash_పట్టిక, &s));
    ఒకవేళ (el == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి RBBINode& సమాసం_కణుపు = *el->val->f_ఎడమ_బిడ్డ; // Root node of expression for variable
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
    తిరిగి &సమాసం_కణుపు.f_పాఠ్యం;
}

స్థిరరాశి Unicode_సమితి* RBBISymbol_పట్టిక::lookup_సమితి(స్థిరరాశి UnicodeString& s) స్థిరరాశి {
    స్థిరరాశి RBBISymbol_పట్టిక_నమోదు* స్థిరరాశి el = static_cast<స్థిరరాశి RBBISymbol_పట్టిక_నమోదు*>(uhash_get(f_Hash_పట్టిక, &s));
    ఒకవేళ (el == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి RBBINode& సమాసం_కణుపు = *el->val->f_ఎడమ_బిడ్డ;
    ఒకవేళ (సమాసం_కణుపు.fType == RBBINode::సమితి_Ref) {
        తిరిగి సమాసం_కణుపు.f_ఎడమ_బిడ్డ->f_ఆదానం_సమితి;
    } లేకపోతే {
        తిరిగి శూన్యసూచిక;
    }
}



//  No longer used, see ICU-23297.
స్థిరరాశి UnicodeFunctor* RBBISymbol_పట్టిక::lookupMatcher(UChar32 /*ch*/) స్థిరరాశి {
    తిరిగి శూన్యసూచిక;
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
UnicodeString   RBBISymbol_పట్టిక::parseReference(స్థిరరాశి UnicodeString& పాఠ్యం,
                                                ParsePosition& pos, int32_t హద్దు) స్థిరరాశి
{
    int32_t ప్రారంభం = pos.get_సూచీ();
    int32_t i = ప్రారంభం;
    UnicodeString ఫలితం;
    అయితే (i < హద్దు) {
        char16_t c = పాఠ్యం.charAt(i);
        ఒకవేళ ((i==ప్రారంభం && !u_isIDStart(c)) || !u_isIDPart(c)) {
            ఆపు;
        }
        ++i;
    }
    ఒకవేళ (i == ప్రారంభం) { // No valid name chars
        తిరిగి ఫలితం; // Indicate failure with empty string
    }
    pos.సమితి_సూచీ(i);
    పాఠ్యం.వెలికితీయి_Between(ప్రారంభం, i, ఫలితం);
    తిరిగి ఫలితం;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbol_పట్టిక::lookup_కణుపు(స్థిరరాశి UnicodeString &కీలకం) స్థిరరాశి{

    RBBINode             *ret_కణుపు = శూన్యసూచిక;
    RBBISymbol_పట్టిక_నమోదు *el;

    el = static_cast<RBBISymbol_పట్టిక_నమోదు*>(uhash_get(f_Hash_పట్టిక, &కీలకం));
    ఒకవేళ (el != శూన్యసూచిక) {
        ret_కణుపు = el->val;
    }
    తిరిగి ret_కణుపు;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
శూన్య            RBBISymbol_పట్టిక::కూడు_నమోదు  (స్థిరరాశి UnicodeString &కీలకం, RBBINode *val, UError_సంకేతం &err) {
    RBBISymbol_పట్టిక_నమోదు *e;
    /* test for buffer overflows */
    ఒకవేళ (U_FAILURE(err)) {
        తిరిగి;
    }
    e = static_cast<RBBISymbol_పట్టిక_నమోదు*>(uhash_get(f_Hash_పట్టిక, &కీలకం));
    ఒకవేళ (e != శూన్యసూచిక) {
        err = U_BRK_VARIABLE_REDFINITION;
        తిరిగి;
    }

    e = కొత్త RBBISymbol_పట్టిక_నమోదు;
    ఒకవేళ (e == శూన్యసూచిక) {
        err = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    e->కీలకం = కీలకం;
    e->val = val;
    uhash_put( f_Hash_పట్టిక, &e->కీలకం, e, &err);
}


RBBISymbol_పట్టిక_నమోదు::RBBISymbol_పట్టిక_నమోదు() : UMemory(), కీలకం(), val(శూన్యసూచిక) {}

RBBISymbol_పట్టిక_నమోదు::~RBBISymbol_పట్టిక_నమోదు() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    తొలగించు val->f_ఎడమ_బిడ్డ;
    val->f_ఎడమ_బిడ్డ = శూన్యసూచిక;

    తొలగించు  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#నిర్వచితమైతే RBBI_DEBUG
శూన్య RBBISymbol_పట్టిక::rbbiSymtablePrint() స్థిరరాశి {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_మొదటి;
    స్థిరరాశి UHashElement  *e   = శూన్యసూచిక;
    కొరకు (;;) {
        e = uhash_తదుపరి_Element(f_Hash_పట్టిక,  &pos);
        ఒకవేళ (e == శూన్యసూచిక ) {
            ఆపు;
        }
        RBBISymbol_పట్టిక_నమోదు  *s   = (RBBISymbol_పట్టిక_నమోదు *)e->value.సూచిక;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->కీలకం)(), (శూన్య *)s->val, s->val->f_Serial_అంకె);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_ఎడమ_బిడ్డ->f_పాఠ్యం)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    కొరకు (;;) {
        e = uhash_తదుపరి_Element(f_Hash_పట్టిక,  &pos);
        ఒకవేళ (e == శూన్యసూచిక ) {
            ఆపు;
        }
        RBBISymbol_పట్టిక_నమోదు  *s   = (RBBISymbol_పట్టిక_నమోదు *)e->value.సూచిక;
        RBBIDebugPrintf("%s\n", CStr(s->కీలకం)());
        RBBINode::print_వృక్షం(s->val, నిజం);
        RBBINode::print_వృక్షం(s->val->f_ఎడమ_బిడ్డ, అబద్ధం);
        RBBIDebugPrintf("\n");
    }
}
#అయితే_ముగింపు





U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */
