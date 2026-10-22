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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_BREAK_ITERATION

#અન્તર્ભાવ <utility>

#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "unicode/rbbi.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/parseerr.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "uvector.h"
#અન્તર્ભાવ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
સંરચના              RBBIRuleTableEl;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbolTableEntry : જાહેર UMemory { // The symbol table hash table contains one
જાહેર:                                       //   of these structs for each entry.
    RBBISymbolTableEntry();
    UnicodeString          ચાવી;
    RBBINode               *val;
    ~RBBISymbolTableEntry();

ખાનગી:
    RBBISymbolTableEntry(અચલ RBBISymbolTableEntry &other) = delete; // forbid copying of this class
    RBBISymbolTableEntry &operator=(અચલ RBBISymbolTableEntry &other) = delete; // forbid copying of this class
};


class RBBISymbolTable : જાહેર UMemory, જાહેર પ્રતીક_Table {
ખાનગી:
    અચલ UnicodeString      &fRules;
    UHashtable               *fHashTable;
    RBBIRuleScanner          *fRuleScanner;

જાહેર:
    //  API inherited from class SymbolTable
    આભાસી અચલ UnicodeString*  lookup(અચલ UnicodeString& s) અચલ override;
    આભાસી અચલ UnicodeSet* lookupSet(અચલ UnicodeString& s) અચલ override;
    આભાસી અચલ UnicodeFunctor* lookupMatcher(UChar32 ch) અચલ override;
    આભાસી UnicodeString parseReference(અચલ UnicodeString& text,
                                         ParsePosition& pos, int32_t limit) અચલ override;

    //  Additional Functions
    RBBISymbolTable(RBBIRuleScanner *, અચલ UnicodeString &fRules, UErrorCode &status);
    આભાસી ~RBBISymbolTable();

    આભાસી RBBINode *lookup_ગાંઠ(અચલ UnicodeString &ચાવી) અચલ;
    આભાસી શૂન્ય      addEntry  (અચલ UnicodeString &ચાવી, RBBINode *val, UErrorCode &err);

#ifdef RBBI_DEBUG
    આભાસી શૂન્ય      rbbiSymtablePrint() અચલ;
#નહીંતર
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t fFakeField;
    #define rbbiSymtablePrint() fFakeField=0;
#endif

ખાનગી:
    RBBISymbolTable(અચલ RBBISymbolTable &other); // forbid copying of this class
    RBBISymbolTable &operator=(અચલ RBBISymbolTable &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : જાહેર UMemory {
જાહેર:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    સ્થિર BreakIterator * createRuleBasedBreakIterator( અચલ UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status);

જાહેર:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(અચલ UnicodeString  &rules,
                    UParseError          *parseErr,
                    UErrorCode           &status
    );

    આભાસી    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIDataHeader  *build(UErrorCode &status);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    શૂન્ય optimizeTables();

    અક્ષર                          *fDebugEnv;        // controls debug trace output
    UErrorCode                    *fStatus;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    અચલ UnicodeString           &fRules;           // The rule string that we are compiling
    UnicodeString                 fStrippedRules;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_વૃક્ષ;     // The parse trees, generated by the scanner,
    RBBINode                      *f_Reverse_વૃક્ષ;     //   then manipulated by subsequent steps.
    RBBINode                      *f_Safe_Fwd_વૃક્ષ;
    RBBINode                      *f_Safe_Rev_વૃક્ષ;

    RBBINode                      **f_Default_વૃક્ષ;    // For rules not qualified with a !
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
ખાનગી:
    RBBIRuleBuilder(અચલ RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &operator=(અચલ RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
સંરચના RBBISetTableEl {
    UnicodeString *ચાવી;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
પ્રકારવ્યાખ્યા std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
#અન્તર્ભાવ <stdio.h>
#define RBBIDebugPrintf printf
#define RBBIDebugPuts puts
#નહીંતર
#undef RBBIDebugPrintf
#define RBBIDebugPuts(arg)
#endif

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif



