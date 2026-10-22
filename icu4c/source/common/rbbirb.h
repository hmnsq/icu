// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
//  rbbirb.h
//
//  Copyright (C) 2002-2008, International Business Machines Corporation and others.
//  All Rights Reserved.
//
//  This file contains declarations for several classes from the
//    Rule Based Break Iterator rule builder.
//


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RBBIRB_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RBBIRB_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

#ਅਨ੍ਤਰ੍ਭਾਵ <utility>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/rbbi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parseerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
ਬਣਤਰ              RBBIRule_ਸਾਰਣੀ_El;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ : ਜਨਤਕ UMemory { // The symbol table hash table contains one
ਜਨਤਕ:                                       //   of these structs for each entry.
    RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ();
    UnicodeString          ਕੁੰਜੀ;
    RBBINode               *val;
    ~RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ();

ਨਿਜੀ:
    RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ(ਅਚਲ RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ &other) = delete; // forbid copying of this class
    RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ &ਸੰਚਾਲਕ=(ਅਚਲ RBBISymbol_ਸਾਰਣੀ_ਇੰਦਰਾਜ &other) = delete; // forbid copying of this class
};


class RBBISymbol_ਸਾਰਣੀ : ਜਨਤਕ UMemory, ਜਨਤਕ ਚਿੰਨ੍ਹ_ਸਾਰਣੀ {
ਨਿਜੀ:
    ਅਚਲ UnicodeString      &f_ਨਿਯਮਾਂ;
    UHashtable               *f_Hash_ਸਾਰਣੀ;
    RBBIRuleScanner          *f_ਨਿਯਮ_Scanner;

ਜਨਤਕ:
    //  API inherited from class SymbolTable
    ਆਭਾਸੀ ਅਚਲ UnicodeString*  lookup(ਅਚਲ UnicodeString& s) ਅਚਲ override;
    ਆਭਾਸੀ ਅਚਲ Unicode_ਸੈੱਟ* lookup_ਸੈੱਟ(ਅਚਲ UnicodeString& s) ਅਚਲ override;
    ਆਭਾਸੀ ਅਚਲ UnicodeFunctor* lookupMatcher(UChar32 ch) ਅਚਲ override;
    ਆਭਾਸੀ UnicodeString parseReference(ਅਚਲ UnicodeString& ਪਾਠ,
                                         ParsePosition& pos, int32_t ਹੱਦਬੰਦੀ) ਅਚਲ override;

    //  Additional Functions
    RBBISymbol_ਸਾਰਣੀ(RBBIRuleScanner *, ਅਚਲ UnicodeString &f_ਨਿਯਮਾਂ, UError_ਕੋਡ &ਸਥਿਤੀ);
    ਆਭਾਸੀ ~RBBISymbol_ਸਾਰਣੀ();

    ਆਭਾਸੀ RBBINode *lookup_ਗੰਢ(ਅਚਲ UnicodeString &ਕੁੰਜੀ) ਅਚਲ;
    ਆਭਾਸੀ ਖਾਲੀ      ਜੋੜੋ_ਇੰਦਰਾਜ  (ਅਚਲ UnicodeString &ਕੁੰਜੀ, RBBINode *val, UError_ਕੋਡ &err);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    ਆਭਾਸੀ ਖਾਲੀ      rbbiSymtablePrint() ਅਚਲ;
#ਨਹੀਂਤਾਂ
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t f_Fake_ਖੇਤਰ;
    #ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ rbbiSymtablePrint() f_Fake_ਖੇਤਰ=0;
#ਜੇ_ਅੰਤ

ਨਿਜੀ:
    RBBISymbol_ਸਾਰਣੀ(ਅਚਲ RBBISymbol_ਸਾਰਣੀ &other); // forbid copying of this class
    RBBISymbol_ਸਾਰਣੀ &ਸੰਚਾਲਕ=(ਅਚਲ RBBISymbol_ਸਾਰਣੀ &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : ਜਨਤਕ UMemory {
ਜਨਤਕ:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    ਸਥਿਰ BreakIterator * create_ਨਿਯਮ_Based_Break_Iterator( ਅਚਲ UnicodeString    &ਨਿਯਮਾਂ,
                                    UParseError      *parseError,
                                    UError_ਕੋਡ       &ਸਥਿਤੀ);

ਜਨਤਕ:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ਅਚਲ UnicodeString  &ਨਿਯਮਾਂ,
                    UParseError          *parseErr,
                    UError_ਕੋਡ           &ਸਥਿਤੀ
    );

    ਆਭਾਸੀ    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIData_ਸਿਰਲੇਖ  *build(UError_ਕੋਡ &ਸਥਿਤੀ);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    ਖਾਲੀ optimizeTables();

    ਅੱਖਰ                          *fDebugEnv;        // controls debug trace output
    UError_ਕੋਡ                    *f_ਸਥਿਤੀ;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ਅਚਲ UnicodeString           &f_ਨਿਯਮਾਂ;           // The rule string that we are compiling
    UnicodeString                 f_Stripped_ਨਿਯਮਾਂ;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_ਰੁੱਖ;     // The parse trees, generated by the scanner,
    RBBINode                      *f_ਉਲਟ_ਰੁੱਖ;     //   then manipulated by subsequent steps.
    RBBINode                      *f_ਸੁਰੱਖਿਅਤ_Fwd_ਰੁੱਖ;
    RBBINode                      *f_ਸੁਰੱਖਿਅਤ_Rev_ਰੁੱਖ;

    RBBINode                      **f_Default_ਰੁੱਖ;    // For rules not qualified with a !
                                                     //   the tree to which they belong to.

    UBool                         f_Chain_ਨਿਯਮਾਂ;       // True for chained Unicode TR style rules.
                                                     // False for traditional regexp rules.

    UBool                         fLookAheadHardBreak;  // True:  Look ahead matches cause an
                                                     // immediate break, no continuing for the
                                                     // longest match.

    RBBISetBuilder                *f_ਸੈੱਟ_Builder;      // Set and Character Category builder.
    UVector                       *fUSetNodes;       // Vector of all uset nodes.

    RBBITableBuilder              *f_Forward_ਸਾਰਣੀ;    // State transition table, build time form.

    UVector                       *f_ਨਿਯਮ_ਸਥਿਤੀ_Vals;  // The values that can be returned
                                                     //   from getRuleStatus().

    RBBIData_ਸਿਰਲੇਖ                *flatten_ਅੰਕੜੇ();    // Create the flattened (runtime format)
                                                     // data tables..
ਨਿਜੀ:
    RBBIRuleBuilder(ਅਚਲ RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &ਸੰਚਾਲਕ=(ਅਚਲ RBBIRuleBuilder &other) = delete; // forbid copying of this class
};




//----------------------------------------------------------------------------
//
//   RBBISetTableEl   is an entry in the hash table of UnicodeSets that have
//                    been encountered.  The val Node will be of nodetype uset
//                    and contain pointers to the actual UnicodeSets.
//                    The Key is the source string for initializing the set.
//
//                    The hash table is used to avoid creating duplicate
//                    unnamed (not $var references) UnicodeSets.
//
//                    Memory Management:
//                       The Hash Table owns these RBBISetTableEl structs and
//                            the key strings.  It does NOT own the val nodes.
//
//----------------------------------------------------------------------------
ਬਣਤਰ RBBISet_ਸਾਰਣੀ_El {
    UnicodeString *ਕੁੰਜੀ;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RBBIDebugPrintf ਲਿਖੋ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RBBIDebugPuts puts
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ RBBIDebugPrintf
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RBBIDebugPuts(arg)
#ਜੇ_ਅੰਤ

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_BREAK_ITERATION */

#ਜੇ_ਅੰਤ



