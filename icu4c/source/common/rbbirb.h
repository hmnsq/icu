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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ <utility>

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/parseerr.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
ಸಂರಚನಾ              RBBIRuleTableEl;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbolTableEntry : ಸಾರ್ವಜನಿಕ UMemory { // The symbol table hash table contains one
ಸಾರ್ವಜನಿಕ:                                       //   of these structs for each entry.
    RBBISymbolTableEntry();
    UnicodeString          ಕೀಲಿ;
    RBBINode               *val;
    ~RBBISymbolTableEntry();

ಗುಪ್ತ:
    RBBISymbolTableEntry(ಧ್ರುವ RBBISymbolTableEntry &other) = delete; // forbid copying of this class
    RBBISymbolTableEntry &operator=(ಧ್ರುವ RBBISymbolTableEntry &other) = delete; // forbid copying of this class
};


class RBBISymbolTable : ಸಾರ್ವಜನಿಕ UMemory, ಸಾರ್ವಜನಿಕ ಚಿಹ್ನೆ_Table {
ಗುಪ್ತ:
    ಧ್ರುವ UnicodeString      &fRules;
    UHashtable               *fHashTable;
    RBBIRuleScanner          *fRuleScanner;

ಸಾರ್ವಜನಿಕ:
    //  API inherited from class SymbolTable
    ಆಭಾಸ ಧ್ರುವ UnicodeString*  lookup(ಧ್ರುವ UnicodeString& s) ಧ್ರುವ override;
    ಆಭಾಸ ಧ್ರುವ UnicodeSet* lookupSet(ಧ್ರುವ UnicodeString& s) ಧ್ರುವ override;
    ಆಭಾಸ ಧ್ರುವ UnicodeFunctor* lookupMatcher(UChar32 ch) ಧ್ರುವ override;
    ಆಭಾಸ UnicodeString parseReference(ಧ್ರುವ UnicodeString& text,
                                         ParsePosition& pos, int32_t limit) ಧ್ರುವ override;

    //  Additional Functions
    RBBISymbolTable(RBBIRuleScanner *, ಧ್ರುವ UnicodeString &fRules, UErrorCode &status);
    ಆಭಾಸ ~RBBISymbolTable();

    ಆಭಾಸ RBBINode *lookup_ಗಂಟು(ಧ್ರುವ UnicodeString &ಕೀಲಿ) ಧ್ರುವ;
    ಆಭಾಸ ಶೂನ್ಯ      addEntry  (ಧ್ರುವ UnicodeString &ಕೀಲಿ, RBBINode *val, UErrorCode &err);

#ifdef RBBI_DEBUG
    ಆಭಾಸ ಶೂನ್ಯ      rbbiSymtablePrint() ಧ್ರುವ;
#ಅನ್ಯಥಾ
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t fFakeField;
    #define rbbiSymtablePrint() fFakeField=0;
#endif

ಗುಪ್ತ:
    RBBISymbolTable(ಧ್ರುವ RBBISymbolTable &other); // forbid copying of this class
    RBBISymbolTable &operator=(ಧ್ರುವ RBBISymbolTable &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    ಸ್ಥಿರ BreakIterator * createRuleBasedBreakIterator( ಧ್ರುವ UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status);

ಸಾರ್ವಜನಿಕ:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ಧ್ರುವ UnicodeString  &rules,
                    UParseError          *parseErr,
                    UErrorCode           &status
    );

    ಆಭಾಸ    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIDataHeader  *build(UErrorCode &status);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    ಶೂನ್ಯ optimizeTables();

    ವರ್ಣ                          *fDebugEnv;        // controls debug trace output
    UErrorCode                    *fStatus;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ಧ್ರುವ UnicodeString           &fRules;           // The rule string that we are compiling
    UnicodeString                 fStrippedRules;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_ವೃಕ್ಷ;     // The parse trees, generated by the scanner,
    RBBINode                      *f_Reverse_ವೃಕ್ಷ;     //   then manipulated by subsequent steps.
    RBBINode                      *f_Safe_Fwd_ವೃಕ್ಷ;
    RBBINode                      *f_Safe_Rev_ವೃಕ್ಷ;

    RBBINode                      **f_Default_ವೃಕ್ಷ;    // For rules not qualified with a !
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
ಗುಪ್ತ:
    RBBIRuleBuilder(ಧ್ರುವ RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &operator=(ಧ್ರುವ RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
ಸಂರಚನಾ RBBISetTableEl {
    UnicodeString *ಕೀಲಿ;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
#ಅನ್ತರ್ಭಾವ <stdio.h>
#define RBBIDebugPrintf printf
#define RBBIDebugPuts puts
#ಅನ್ಯಥಾ
#undef RBBIDebugPrintf
#define RBBIDebugPuts(arg)
#endif

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif



