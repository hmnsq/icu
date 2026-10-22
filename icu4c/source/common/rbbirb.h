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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_BREAK_ITERATION

#ଅନ୍ତର୍ଭାଵ <utility>

#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/rbbi.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uniset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/parseerr.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"
#ଅନ୍ତର୍ଭାଵ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
ସଂରଚନା              RBBIRuleTableEl;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbolTableEntry : ସର୍ବସାଧାରଣ UMemory { // The symbol table hash table contains one
ସର୍ବସାଧାରଣ:                                       //   of these structs for each entry.
    RBBISymbolTableEntry();
    UnicodeString          ଚାବି;
    RBBINode               *val;
    ~RBBISymbolTableEntry();

ବ୍ୟକ୍ତିଗତ:
    RBBISymbolTableEntry(ଅଚଳ RBBISymbolTableEntry &other) = delete; // forbid copying of this class
    RBBISymbolTableEntry &operator=(ଅଚଳ RBBISymbolTableEntry &other) = delete; // forbid copying of this class
};


class RBBISymbolTable : ସର୍ବସାଧାରଣ UMemory, ସର୍ବସାଧାରଣ ପ୍ରତୀକ_Table {
ବ୍ୟକ୍ତିଗତ:
    ଅଚଳ UnicodeString      &fRules;
    UHashtable               *fHashTable;
    RBBIRuleScanner          *fRuleScanner;

ସର୍ବସାଧାରଣ:
    //  API inherited from class SymbolTable
    ଆଭାସୀ ଅଚଳ UnicodeString*  lookup(ଅଚଳ UnicodeString& s) ଅଚଳ override;
    ଆଭାସୀ ଅଚଳ UnicodeSet* lookupSet(ଅଚଳ UnicodeString& s) ଅଚଳ override;
    ଆଭାସୀ ଅଚଳ UnicodeFunctor* lookupMatcher(UChar32 ch) ଅଚଳ override;
    ଆଭାସୀ UnicodeString parseReference(ଅଚଳ UnicodeString& text,
                                         ParsePosition& pos, int32_t limit) ଅଚଳ override;

    //  Additional Functions
    RBBISymbolTable(RBBIRuleScanner *, ଅଚଳ UnicodeString &fRules, UErrorCode &status);
    ଆଭାସୀ ~RBBISymbolTable();

    ଆଭାସୀ RBBINode *lookup_ଗଣ୍ଠି(ଅଚଳ UnicodeString &ଚାବି) ଅଚଳ;
    ଆଭାସୀ ଶୂନ୍ୟ      addEntry  (ଅଚଳ UnicodeString &ଚାବି, RBBINode *val, UErrorCode &err);

#ifdef RBBI_DEBUG
    ଆଭାସୀ ଶୂନ୍ୟ      rbbiSymtablePrint() ଅଚଳ;
#ଅନ୍ୟଥା
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t fFakeField;
    #define rbbiSymtablePrint() fFakeField=0;
#endif

ବ୍ୟକ୍ତିଗତ:
    RBBISymbolTable(ଅଚଳ RBBISymbolTable &other); // forbid copying of this class
    RBBISymbolTable &operator=(ଅଚଳ RBBISymbolTable &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    ସ୍ଥିର BreakIterator * createRuleBasedBreakIterator( ଅଚଳ UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status);

ସର୍ବସାଧାରଣ:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ଅଚଳ UnicodeString  &rules,
                    UParseError          *parseErr,
                    UErrorCode           &status
    );

    ଆଭାସୀ    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIDataHeader  *build(UErrorCode &status);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    ଶୂନ୍ୟ optimizeTables();

    ଅକ୍ଷର                          *fDebugEnv;        // controls debug trace output
    UErrorCode                    *fStatus;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ଅଚଳ UnicodeString           &fRules;           // The rule string that we are compiling
    UnicodeString                 fStrippedRules;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_ବୃକ୍ଷ;     // The parse trees, generated by the scanner,
    RBBINode                      *f_Reverse_ବୃକ୍ଷ;     //   then manipulated by subsequent steps.
    RBBINode                      *f_Safe_Fwd_ବୃକ୍ଷ;
    RBBINode                      *f_Safe_Rev_ବୃକ୍ଷ;

    RBBINode                      **f_Default_ବୃକ୍ଷ;    // For rules not qualified with a !
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
ବ୍ୟକ୍ତିଗତ:
    RBBIRuleBuilder(ଅଚଳ RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &operator=(ଅଚଳ RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
ସଂରଚନା RBBISetTableEl {
    UnicodeString *ଚାବି;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#define RBBIDebugPrintf printf
#define RBBIDebugPuts puts
#ଅନ୍ୟଥା
#undef RBBIDebugPrintf
#define RBBIDebugPuts(arg)
#endif

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif



