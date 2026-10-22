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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"

#ಅನ್ತರ್ಭಾವ "cstr.h"
#ಅನ್ತರ್ಭಾವ "rbbinode.h"
#ಅನ್ತರ್ಭಾವ "rbbirb.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು_deleter(ಶೂನ್ಯ *p) {
    icu::RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು *px = (icu::RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು *)p;
    ಅಳಿಸು px;
}
U_CDECL_ಕೊನೆ



U_NAMESPACE_BEGIN

RBBISymbol_ಕೋಷ್ಟಕ::RBBISymbol_ಕೋಷ್ಟಕ(RBBIRuleScanner *rs, ಸ್ಥಿರಾಂಕ UnicodeString &ನಿಯಮಗಳು, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
    : f_ನಿಯಮಗಳು(ನಿಯಮಗಳು), f_ನಿಯಮ_Scanner(rs)
{
    f_Hash_ಕೋಷ್ಟಕ       = ಶೂನ್ಯಸೂಚಕ;

    f_Hash_ಕೋಷ್ಟಕ = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2);
    // uhash_open checks status
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    uhash_ಗಣ_ಮೌಲ್ಯ_Deleter(f_Hash_ಕೋಷ್ಟಕ, RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು_deleter);
}



RBBISymbol_ಕೋಷ್ಟಕ::~RBBISymbol_ಕೋಷ್ಟಕ()
{
    uhash_close(f_Hash_ಕೋಷ್ಟಕ);
}


//
//  RBBISymbolTable::lookup       This function from the abstract symbol table interface
//                                looks up a variable name and returns a UnicodeString
//                                containing the substitution text.
//
//                                The variable name does NOT include the leading $.
//
ಸ್ಥಿರಾಂಕ UnicodeString  *RBBISymbol_ಕೋಷ್ಟಕ::lookup(ಸ್ಥಿರಾಂಕ UnicodeString& s) ಸ್ಥಿರಾಂಕ
{
    ಸ್ಥಿರಾಂಕ RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು* ಸ್ಥಿರಾಂಕ el =
        static_cast<ಸ್ಥಿರಾಂಕ RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು*>(uhash_get(f_Hash_ಕೋಷ್ಟಕ, &s));
    ಆದರೆ (el == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ RBBINode& ಅಭಿವ್ಯಕ್ತಿ_ಗಂಟು = *el->val->f_ಎಡ_ಮಗು; // Root node of expression for variable
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
    ಹಿಂತಿರುಗಿ &ಅಭಿವ್ಯಕ್ತಿ_ಗಂಟು.f_ಪಠ್ಯ;
}

ಸ್ಥಿರಾಂಕ Unicode_ಗಣ* RBBISymbol_ಕೋಷ್ಟಕ::lookup_ಗಣ(ಸ್ಥಿರಾಂಕ UnicodeString& s) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು* ಸ್ಥಿರಾಂಕ el = static_cast<ಸ್ಥಿರಾಂಕ RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು*>(uhash_get(f_Hash_ಕೋಷ್ಟಕ, &s));
    ಆದರೆ (el == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ RBBINode& ಅಭಿವ್ಯಕ್ತಿ_ಗಂಟು = *el->val->f_ಎಡ_ಮಗು;
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಗಂಟು.fType == RBBINode::ಗಣ_Ref) {
        ಹಿಂತಿರುಗಿ ಅಭಿವ್ಯಕ್ತಿ_ಗಂಟು.f_ಎಡ_ಮಗು->f_ಒಳಹರಿವು_ಗಣ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}



//  No longer used, see ICU-23297.
ಸ್ಥಿರಾಂಕ UnicodeFunctor* RBBISymbol_ಕೋಷ್ಟಕ::lookupMatcher(UChar32 /*ch*/) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
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
UnicodeString   RBBISymbol_ಕೋಷ್ಟಕ::parseReference(ಸ್ಥಿರಾಂಕ UnicodeString& ಪಠ್ಯ,
                                                ParsePosition& pos, int32_t ಎಲ್ಲೆ) ಸ್ಥಿರಾಂಕ
{
    int32_t ಆರಂಭ = pos.get_ಸೂಚ್ಯಂಕ();
    int32_t i = ಆರಂಭ;
    UnicodeString ಫಲಿತಾಂಶ;
    ಆಗ (i < ಎಲ್ಲೆ) {
        char16_t c = ಪಠ್ಯ.charAt(i);
        ಆದರೆ ((i==ಆರಂಭ && !u_isIDStart(c)) || !u_isIDPart(c)) {
            ನಿಲ್ಲಿಸು;
        }
        ++i;
    }
    ಆದರೆ (i == ಆರಂಭ) { // No valid name chars
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; // Indicate failure with empty string
    }
    pos.ಗಣ_ಸೂಚ್ಯಂಕ(i);
    ಪಠ್ಯ.ಹೊರತೆಗೆ_Between(ಆರಂಭ, i, ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbol_ಕೋಷ್ಟಕ::lookup_ಗಂಟು(ಸ್ಥಿರಾಂಕ UnicodeString &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ{

    RBBINode             *ret_ಗಂಟು = ಶೂನ್ಯಸೂಚಕ;
    RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು *el;

    el = static_cast<RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು*>(uhash_get(f_Hash_ಕೋಷ್ಟಕ, &ಕೀಲಿ));
    ಆದರೆ (el != ಶೂನ್ಯಸೂಚಕ) {
        ret_ಗಂಟು = el->val;
    }
    ಹಿಂತಿರುಗಿ ret_ಗಂಟು;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
ಶೂನ್ಯ            RBBISymbol_ಕೋಷ್ಟಕ::ಕೂಡಿಸು_ನಮೂದು  (ಸ್ಥಿರಾಂಕ UnicodeString &ಕೀಲಿ, RBBINode *val, UError_ಸಂಕೇತ &err) {
    RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು *e;
    /* test for buffer overflows */
    ಆದರೆ (U_FAILURE(err)) {
        ಹಿಂತಿರುಗಿ;
    }
    e = static_cast<RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು*>(uhash_get(f_Hash_ಕೋಷ್ಟಕ, &ಕೀಲಿ));
    ಆದರೆ (e != ಶೂನ್ಯಸೂಚಕ) {
        err = U_BRK_VARIABLE_REDFINITION;
        ಹಿಂತಿರುಗಿ;
    }

    e = ಹೊಸ RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು;
    ಆದರೆ (e == ಶೂನ್ಯಸೂಚಕ) {
        err = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    e->ಕೀಲಿ = ಕೀಲಿ;
    e->val = val;
    uhash_put( f_Hash_ಕೋಷ್ಟಕ, &e->ಕೀಲಿ, e, &err);
}


RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು::RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು() : UMemory(), ಕೀಲಿ(), val(ಶೂನ್ಯಸೂಚಕ) {}

RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು::~RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    ಅಳಿಸು val->f_ಎಡ_ಮಗು;
    val->f_ಎಡ_ಮಗು = ಶೂನ್ಯಸೂಚಕ;

    ಅಳಿಸು  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಶೂನ್ಯ RBBISymbol_ಕೋಷ್ಟಕ::rbbiSymtablePrint() ಸ್ಥಿರಾಂಕ {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_ಮೊದಲ;
    ಸ್ಥಿರಾಂಕ UHashElement  *e   = ಶೂನ್ಯಸೂಚಕ;
    ಗಾಗಿ (;;) {
        e = uhash_ಮುಂದಿನ_Element(f_Hash_ಕೋಷ್ಟಕ,  &pos);
        ಆದರೆ (e == ಶೂನ್ಯಸೂಚಕ ) {
            ನಿಲ್ಲಿಸು;
        }
        RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು  *s   = (RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು *)e->value.ಸೂಚಕ;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->ಕೀಲಿ)(), (ಶೂನ್ಯ *)s->val, s->val->f_Serial_ಅಂಕೆ);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_ಎಡ_ಮಗು->f_ಪಠ್ಯ)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    ಗಾಗಿ (;;) {
        e = uhash_ಮುಂದಿನ_Element(f_Hash_ಕೋಷ್ಟಕ,  &pos);
        ಆದರೆ (e == ಶೂನ್ಯಸೂಚಕ ) {
            ನಿಲ್ಲಿಸು;
        }
        RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು  *s   = (RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು *)e->value.ಸೂಚಕ;
        RBBIDebugPrintf("%s\n", CStr(s->ಕೀಲಿ)());
        RBBINode::print_ವೃಕ್ಷ(s->val, ನಿಜ);
        RBBINode::print_ವೃಕ್ಷ(s->val->f_ಎಡ_ಮಗು, ಸುಳ್ಳು);
        RBBIDebugPrintf("\n");
    }
}
#ಆದರೆ_ಅಂತ್ಯ





U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */
