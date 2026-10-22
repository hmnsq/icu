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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব <utility>

#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/rbbi.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/parseerr.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "uvector.h"
#অন্তর্ভাব "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
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
class RBBISymbolTableEntry : সার্বজনিক UMemory { // The symbol table hash table contains one
সার্বজনিক:                                       //   of these structs for each entry.
    RBBISymbolTableEntry();
    UnicodeString          চাবি;
    RBBINode               *val;
    ~RBBISymbolTableEntry();

গুপ্ত:
    RBBISymbolTableEntry(ধ্রুব RBBISymbolTableEntry &other) = delete; // forbid copying of this class
    RBBISymbolTableEntry &operator=(ধ্রুব RBBISymbolTableEntry &other) = delete; // forbid copying of this class
};


class RBBISymbolTable : সার্বজনিক UMemory, সার্বজনিক প্রতীক_Table {
গুপ্ত:
    ধ্রুব UnicodeString      &fRules;
    UHashtable               *fHashTable;
    RBBIRuleScanner          *fRuleScanner;

সার্বজনিক:
    //  API inherited from class SymbolTable
    আভাস ধ্রুব UnicodeString*  lookup(ধ্রুব UnicodeString& s) ধ্রুব override;
    আভাস ধ্রুব UnicodeSet* lookupSet(ধ্রুব UnicodeString& s) ধ্রুব override;
    আভাস ধ্রুব UnicodeFunctor* lookupMatcher(UChar32 ch) ধ্রুব override;
    আভাস UnicodeString parseReference(ধ্রুব UnicodeString& text,
                                         ParsePosition& pos, int32_t limit) ধ্রুব override;

    //  Additional Functions
    RBBISymbolTable(RBBIRuleScanner *, ধ্রুব UnicodeString &fRules, UErrorCode &status);
    আভাস ~RBBISymbolTable();

    আভাস RBBINode *lookup_গ্রন্থি(ধ্রুব UnicodeString &চাবি) ধ্রুব;
    আভাস শূন্য      addEntry  (ধ্রুব UnicodeString &চাবি, RBBINode *val, UErrorCode &err);

#ifdef RBBI_DEBUG
    আভাস শূন্য      rbbiSymtablePrint() ধ্রুব;
#অন্যথা
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t fFakeField;
    #define rbbiSymtablePrint() fFakeField=0;
#endif

গুপ্ত:
    RBBISymbolTable(ধ্রুব RBBISymbolTable &other); // forbid copying of this class
    RBBISymbolTable &operator=(ধ্রুব RBBISymbolTable &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : সার্বজনিক UMemory {
সার্বজনিক:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    স্থির BreakIterator * createRuleBasedBreakIterator( ধ্রুব UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status);

সার্বজনিক:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ধ্রুব UnicodeString  &rules,
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

    বর্ণ                          *fDebugEnv;        // controls debug trace output
    UErrorCode                    *fStatus;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ধ্রুব UnicodeString           &fRules;           // The rule string that we are compiling
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
    RBBIRuleBuilder(ধ্রুব RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &operator=(ধ্রুব RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
#অন্তর্ভাব <stdio.h>
#define RBBIDebugPrintf printf
#define RBBIDebugPuts puts
#অন্যথা
#undef RBBIDebugPrintf
#define RBBIDebugPuts(arg)
#endif

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif



