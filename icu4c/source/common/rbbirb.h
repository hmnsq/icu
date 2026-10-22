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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাৱ <utility>

#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "unicode/rbbi.h"
#অন্তর্ভাৱ "unicode/uniset.h"
#অন্তর্ভাৱ "unicode/parseerr.h"
#অন্তর্ভাৱ "uhash.h"
#অন্তর্ভাৱ "uvector.h"
#অন্তর্ভাৱ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
সংরচনা              RBBIRuleTableEl;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbolTableEntry : সার্ৱজনিক UMemory { // The symbol table hash table contains one
সার্ৱজনিক:                                       //   of these structs for each entry.
    RBBISymbolTableEntry();
    UnicodeString          চাবি;
    RBBINode               *val;
    ~RBBISymbolTableEntry();

গুপ্ত:
    RBBISymbolTableEntry(ধ্রুৱ RBBISymbolTableEntry &other) = delete; // forbid copying of this class
    RBBISymbolTableEntry &operator=(ধ্রুৱ RBBISymbolTableEntry &other) = delete; // forbid copying of this class
};


class RBBISymbolTable : সার্ৱজনিক UMemory, সার্ৱজনিক প্ৰতীক_Table {
গুপ্ত:
    ধ্রুৱ UnicodeString      &fRules;
    UHashtable               *fHashTable;
    RBBIRuleScanner          *fRuleScanner;

সার্ৱজনিক:
    //  API inherited from class SymbolTable
    আভাস ধ্রুৱ UnicodeString*  lookup(ধ্রুৱ UnicodeString& s) ধ্রুৱ override;
    আভাস ধ্রুৱ UnicodeSet* lookupSet(ধ্রুৱ UnicodeString& s) ধ্রুৱ override;
    আভাস ধ্রুৱ UnicodeFunctor* lookupMatcher(UChar32 ch) ধ্রুৱ override;
    আভাস UnicodeString parseReference(ধ্রুৱ UnicodeString& text,
                                         ParsePosition& pos, int32_t limit) ধ্রুৱ override;

    //  Additional Functions
    RBBISymbolTable(RBBIRuleScanner *, ধ্রুৱ UnicodeString &fRules, UErrorCode &status);
    আভাস ~RBBISymbolTable();

    আভাস RBBINode *lookup_গ্ৰন্থি(ধ্রুৱ UnicodeString &চাবি) ধ্রুৱ;
    আভাস শূন্য      addEntry  (ধ্রুৱ UnicodeString &চাবি, RBBINode *val, UErrorCode &err);

#ifdef RBBI_DEBUG
    আভাস শূন্য      rbbiSymtablePrint() ধ্রুৱ;
#অন্যথা
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t fFakeField;
    #define rbbiSymtablePrint() fFakeField=0;
#endif

গুপ্ত:
    RBBISymbolTable(ধ্রুৱ RBBISymbolTable &other); // forbid copying of this class
    RBBISymbolTable &operator=(ধ্রুৱ RBBISymbolTable &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : সার্ৱজনিক UMemory {
সার্ৱজনিক:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    স্থির BreakIterator * createRuleBasedBreakIterator( ধ্রুৱ UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status);

সার্ৱজনিক:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ধ্রুৱ UnicodeString  &rules,
                    UParseError          *parseErr,
                    UErrorCode           &status
    );

    আভাস    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIDataHeader  *build(UErrorCode &status);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    শূন্য optimizeTables();

    ৱর্ণ                          *fDebugEnv;        // controls debug trace output
    UErrorCode                    *fStatus;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ধ্রুৱ UnicodeString           &fRules;           // The rule string that we are compiling
    UnicodeString                 fStrippedRules;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_বৃক্ষ;     // The parse trees, generated by the scanner,
    RBBINode                      *f_Reverse_বৃক্ষ;     //   then manipulated by subsequent steps.
    RBBINode                      *f_Safe_Fwd_বৃক্ষ;
    RBBINode                      *f_Safe_Rev_বৃক্ষ;

    RBBINode                      **f_Default_বৃক্ষ;    // For rules not qualified with a !
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
গুপ্ত:
    RBBIRuleBuilder(ধ্রুৱ RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &operator=(ধ্রুৱ RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
সংরচনা RBBISetTableEl {
    UnicodeString *চাবি;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
প্রকারনির্দেশ std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
#অন্তর্ভাৱ <stdio.h>
#define RBBIDebugPrintf printf
#define RBBIDebugPuts puts
#অন্যথা
#undef RBBIDebugPrintf
#define RBBIDebugPuts(arg)
#endif

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif



