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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parsepos.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "cstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbinode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbirb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"


//
//  RBBISymbolTableEntry_deleter    Used by the UHashTable to delete the contents
//                                  when the hash table is deleted.
//
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ_deleter(ਖਾਲੀ *p) {
    icu::RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ *px = (icu::RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ *)p;
    ਮਿਟਾਓ px;
}
U_CDECL_ਸਿਰਾ



U_NAMESPACE_BEGIN

RBBISymbol_ਸਾਰਣੀ::RBBISymbol_ਸਾਰਣੀ(RBBIRuleScanner *rs, ਅਚਲ UnicodeString &ਨਿਯਮਾਂ, UError_ਕੋਡ &ਸਥਿਤੀ)
    : f_ਨਿਯਮਾਂ(ਨਿਯਮਾਂ), f_ਨਿਯਮ_Scanner(rs)
{
    f_Hash_ਸਾਰਣੀ       = ਖਾਲੀ_ਸੂਚਕ;

    f_Hash_ਸਾਰਣੀ = uhash_open(uhash_hashUnicodeString, uhash_compareUnicodeString, ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ);
    // uhash_open checks status
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    uhash_ਸੈੱਟ_ਮੁੱਲ_Deleter(f_Hash_ਸਾਰਣੀ, RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ_deleter);
}



RBBISymbol_ਸਾਰਣੀ::~RBBISymbol_ਸਾਰਣੀ()
{
    uhash_close(f_Hash_ਸਾਰਣੀ);
}


//
//  RBBISymbolTable::lookup       This function from the abstract symbol table interface
//                                looks up a variable name and returns a UnicodeString
//                                containing the substitution text.
//
//                                The variable name does NOT include the leading $.
//
ਅਚਲ UnicodeString  *RBBISymbol_ਸਾਰਣੀ::lookup(ਅਚਲ UnicodeString& s) ਅਚਲ
{
    ਅਚਲ RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ* ਅਚਲ el =
        static_cast<ਅਚਲ RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ*>(uhash_get(f_Hash_ਸਾਰਣੀ, &s));
    ਜੇ (el == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ RBBINode& ਵਿਅੰਜਕ_ਗੰਢ = *el->val->f_ਖੱਬਾ_ਬੱਚਾ; // Root node of expression for variable
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
    ਵਾਪਸ &ਵਿਅੰਜਕ_ਗੰਢ.f_ਪਾਠ;
}

ਅਚਲ Unicode_ਸੈੱਟ* RBBISymbol_ਸਾਰਣੀ::lookup_ਸੈੱਟ(ਅਚਲ UnicodeString& s) ਅਚਲ {
    ਅਚਲ RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ* ਅਚਲ el = static_cast<ਅਚਲ RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ*>(uhash_get(f_Hash_ਸਾਰਣੀ, &s));
    ਜੇ (el == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ RBBINode& ਵਿਅੰਜਕ_ਗੰਢ = *el->val->f_ਖੱਬਾ_ਬੱਚਾ;
    ਜੇ (ਵਿਅੰਜਕ_ਗੰਢ.fType == RBBINode::ਸੈੱਟ_Ref) {
        ਵਾਪਸ ਵਿਅੰਜਕ_ਗੰਢ.f_ਖੱਬਾ_ਬੱਚਾ->f_ਨਿਵੇਸ਼_ਸੈੱਟ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}



//  No longer used, see ICU-23297.
ਅਚਲ UnicodeFunctor* RBBISymbol_ਸਾਰਣੀ::lookupMatcher(UChar32 /*ch*/) ਅਚਲ {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
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
UnicodeString   RBBISymbol_ਸਾਰਣੀ::parseReference(ਅਚਲ UnicodeString& ਪਾਠ,
                                                ParsePosition& pos, int32_t ਹੱਦਬੰਦੀ) ਅਚਲ
{
    int32_t ਸ਼ੁਰੂ = pos.get_ਸੂਚਕਾਂਕ();
    int32_t i = ਸ਼ੁਰੂ;
    UnicodeString ਨਤੀਜਾ;
    ਜਦੋਂ (i < ਹੱਦਬੰਦੀ) {
        char16_t c = ਪਾਠ.charAt(i);
        ਜੇ ((i==ਸ਼ੁਰੂ && !u_isIDStart(c)) || !u_isIDPart(c)) {
            ਰੋਕੋ;
        }
        ++i;
    }
    ਜੇ (i == ਸ਼ੁਰੂ) { // No valid name chars
        ਵਾਪਸ ਨਤੀਜਾ; // Indicate failure with empty string
    }
    pos.ਸੈੱਟ_ਸੂਚਕਾਂਕ(i);
    ਪਾਠ.ਕੱਢੋ_Between(ਸ਼ੁਰੂ, i, ਨਤੀਜਾ);
    ਵਾਪਸ ਨਤੀਜਾ;
}



//
// RBBISymbolTable::lookupNode      Given a key (a variable name), return the
//                                  corresponding RBBI Node.  If there is no entry
//                                  in the table for this name, return nullptr.
//
RBBINode       *RBBISymbol_ਸਾਰਣੀ::lookup_ਗੰਢ(ਅਚਲ UnicodeString &ਕੁੰਜੀ) ਅਚਲ{

    RBBINode             *ret_ਗੰਢ = ਖਾਲੀ_ਸੂਚਕ;
    RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ *el;

    el = static_cast<RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ*>(uhash_get(f_Hash_ਸਾਰਣੀ, &ਕੁੰਜੀ));
    ਜੇ (el != ਖਾਲੀ_ਸੂਚਕ) {
        ret_ਗੰਢ = el->val;
    }
    ਵਾਪਸ ret_ਗੰਢ;
}


//
//    RBBISymbolTable::addEntry     Add a new entry to the symbol table.
//                                  Indicate an error if the name already exists -
//                                    this will only occur in the case of duplicate
//                                    variable assignments.
//
ਖਾਲੀ            RBBISymbol_ਸਾਰਣੀ::ਜੋੜੋ_ਇੰਦਰਾਜ  (ਅਚਲ UnicodeString &ਕੁੰਜੀ, RBBINode *val, UError_ਕੋਡ &err) {
    RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ *e;
    /* test for buffer overflows */
    ਜੇ (U_FAILURE(err)) {
        ਵਾਪਸ;
    }
    e = static_cast<RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ*>(uhash_get(f_Hash_ਸਾਰਣੀ, &ਕੁੰਜੀ));
    ਜੇ (e != ਖਾਲੀ_ਸੂਚਕ) {
        err = U_BRK_VARIABLE_REDFINITION;
        ਵਾਪਸ;
    }

    e = ਨਵਾਂ RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ;
    ਜੇ (e == ਖਾਲੀ_ਸੂਚਕ) {
        err = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    e->ਕੁੰਜੀ = ਕੁੰਜੀ;
    e->val = val;
    uhash_put( f_Hash_ਸਾਰਣੀ, &e->ਕੁੰਜੀ, e, &err);
}


RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ::RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ() : UMemory(), ਕੁੰਜੀ(), val(ਖਾਲੀ_ਸੂਚਕ) {}

RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ::~RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ() {
    // The "val" of a symbol table entry is a variable reference node.
    // The l. child of the val is the rhs expression from the assignment.
    // Unlike other node types, children of variable reference nodes are not
    //    automatically recursively deleted.  We do it manually here.
    ਮਿਟਾਓ val->f_ਖੱਬਾ_ਬੱਚਾ;
    val->f_ਖੱਬਾ_ਬੱਚਾ = ਖਾਲੀ_ਸੂਚਕ;

    ਮਿਟਾਓ  val;

    // Note: the key UnicodeString is destructed by virtue of being in the object by value.
}


//
//  RBBISymbolTable::print    Debugging function, dump out the symbol table contents.
//
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
ਖਾਲੀ RBBISymbol_ਸਾਰਣੀ::rbbiSymtablePrint() ਅਚਲ {
    RBBIDebugPrintf("Variable Definitions Symbol Table\n"
           "Name                  Node         serial  String Val\n"
           "-------------------------------------------------------------------\n");

    int32_t pos = UHASH_ਪਹਿਲਾ;
    ਅਚਲ UHashElement  *e   = ਖਾਲੀ_ਸੂਚਕ;
    ਲਈ (;;) {
        e = uhash_ਅਗਲਾ_Element(f_Hash_ਸਾਰਣੀ,  &pos);
        ਜੇ (e == ਖਾਲੀ_ਸੂਚਕ ) {
            ਰੋਕੋ;
        }
        RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ  *s   = (RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ *)e->value.ਸੂਚਕ;

        RBBIDebugPrintf("%-19s   %8p %7d ", CStr(s->ਕੁੰਜੀ)(), (ਖਾਲੀ *)s->val, s->val->f_Serial_ਅੰਕ);
        RBBIDebugPrintf(" %s\n", CStr(s->val->f_ਖੱਬਾ_ਬੱਚਾ->f_ਪਾਠ)());
    }

    RBBIDebugPrintf("\nParsed Variable Definitions\n");
    pos = -1;
    ਲਈ (;;) {
        e = uhash_ਅਗਲਾ_Element(f_Hash_ਸਾਰਣੀ,  &pos);
        ਜੇ (e == ਖਾਲੀ_ਸੂਚਕ ) {
            ਰੋਕੋ;
        }
        RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ  *s   = (RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ *)e->value.ਸੂਚਕ;
        RBBIDebugPrintf("%s\n", CStr(s->ਕੁੰਜੀ)());
        RBBINode::print_ਰੁੱਖ(s->val, ਸੱਚ);
        RBBINode::print_ਰੁੱਖ(s->val->f_ਖੱਬਾ_ਬੱਚਾ, ਝੂਠ);
        RBBIDebugPrintf("\n");
    }
}
#ਜੇ_ਅੰਤ





U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */
