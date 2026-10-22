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


#ifndef RBBIRB_H
#define RBBIRB_H

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ <utility>

#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/rbbi.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/parseerr.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "uvector.h"
#അന്തര്ഭാവ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
ഘടന              RBBIRuleTableEl;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbolTableEntry : പൊതു UMemory { // The symbol table hash table contains one
പൊതു:                                       //   of these structs for each entry.
    RBBISymbolTableEntry();
    UnicodeString          താക്കോൽ;
    RBBINode               *val;
    ~RBBISymbolTableEntry();

സ്വകാര്യ:
    RBBISymbolTableEntry(സ്ഥിരം RBBISymbolTableEntry &other) = delete; // forbid copying of this class
    RBBISymbolTableEntry &operator=(സ്ഥിരം RBBISymbolTableEntry &other) = delete; // forbid copying of this class
};


class RBBISymbolTable : പൊതു UMemory, പൊതു ചിഹ്നം_Table {
സ്വകാര്യ:
    സ്ഥിരം UnicodeString      &fRules;
    UHashtable               *fHashTable;
    RBBIRuleScanner          *fRuleScanner;

പൊതു:
    //  API inherited from class SymbolTable
    മായാ സ്ഥിരം UnicodeString*  lookup(സ്ഥിരം UnicodeString& s) സ്ഥിരം override;
    മായാ സ്ഥിരം UnicodeSet* lookupSet(സ്ഥിരം UnicodeString& s) സ്ഥിരം override;
    മായാ സ്ഥിരം UnicodeFunctor* lookupMatcher(UChar32 ch) സ്ഥിരം override;
    മായാ UnicodeString parseReference(സ്ഥിരം UnicodeString& text,
                                         ParsePosition& pos, int32_t limit) സ്ഥിരം override;

    //  Additional Functions
    RBBISymbolTable(RBBIRuleScanner *, സ്ഥിരം UnicodeString &fRules, UErrorCode &status);
    മായാ ~RBBISymbolTable();

    മായാ RBBINode *lookup_കണ്ണി(സ്ഥിരം UnicodeString &താക്കോൽ) സ്ഥിരം;
    മായാ ശൂന്യം      addEntry  (സ്ഥിരം UnicodeString &താക്കോൽ, RBBINode *val, UErrorCode &err);

#ifdef RBBI_DEBUG
    മായാ ശൂന്യം      rbbiSymtablePrint() സ്ഥിരം;
#അല്ലെങ്കിൽ
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t fFakeField;
    #define rbbiSymtablePrint() fFakeField=0;
#endif

സ്വകാര്യ:
    RBBISymbolTable(സ്ഥിരം RBBISymbolTable &other); // forbid copying of this class
    RBBISymbolTable &operator=(സ്ഥിരം RBBISymbolTable &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : പൊതു UMemory {
പൊതു:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    സ്ഥിതം BreakIterator * createRuleBasedBreakIterator( സ്ഥിരം UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status);

പൊതു:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(സ്ഥിരം UnicodeString  &rules,
                    UParseError          *parseErr,
                    UErrorCode           &status
    );

    മായാ    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIDataHeader  *build(UErrorCode &status);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    ശൂന്യം optimizeTables();

    അക്ഷരം                          *fDebugEnv;        // controls debug trace output
    UErrorCode                    *fStatus;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    സ്ഥിരം UnicodeString           &fRules;           // The rule string that we are compiling
    UnicodeString                 fStrippedRules;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_വൃക്ഷം;     // The parse trees, generated by the scanner,
    RBBINode                      *f_Reverse_വൃക്ഷം;     //   then manipulated by subsequent steps.
    RBBINode                      *f_Safe_Fwd_വൃക്ഷം;
    RBBINode                      *f_Safe_Rev_വൃക്ഷം;

    RBBINode                      **f_Default_വൃക്ഷം;    // For rules not qualified with a !
                                                     //   the tree to which they belong to.

    UBool                         fChainRules;       // True for chained Unicode TR style rules.
                                                     // False for traditional regexp rules.

    UBool                         fLookAheadHardBreak;  // True:  Look ahead matches cause an
                                                     // immediate break, no continuing for the
                                                     // longest match.

    RBBISetBuilder                *fSetBuilder;      // Set and Character Category builder.
    UVector                       *fUSetNodes;       // Vector of all uset nodes.

    RBBITableBuilder              *fForwardTable;    // State transition table, build time form.

    UVector                       *fRuleStatusVals;  // The values that can be returned
                                                     //   from getRuleStatus().

    RBBIDataHeader                *flattenData();    // Create the flattened (runtime format)
                                                     // data tables..
സ്വകാര്യ:
    RBBIRuleBuilder(സ്ഥിരം RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &operator=(സ്ഥിരം RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
ഘടന RBBISetTableEl {
    UnicodeString *താക്കോൽ;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
തരനിർവചനം std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
#അന്തര്ഭാവ <stdio.h>
#define RBBIDebugPrintf printf
#define RBBIDebugPuts puts
#അല്ലെങ്കിൽ
#undef RBBIDebugPrintf
#define RBBIDebugPuts(arg)
#endif

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif



