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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل <utility>

#شامل "unicode/uobject.h"
#شامل "unicode/rbbi.h"
#شامل "unicode/uniset.h"
#شامل "unicode/parseerr.h"
#شامل "uhash.h"
#شامل "uvector.h"
#شامل "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
ساختار              RBBIRuleTableEl;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbolTableEntry : عمومی UMemory { // The symbol table hash table contains one
عمومی:                                       //   of these structs for each entry.
    RBBISymbolTableEntry();
    UnicodeString          کلید;
    RBBINode               *val;
    ~RBBISymbolTableEntry();

خصوصی:
    RBBISymbolTableEntry(ثابت RBBISymbolTableEntry &other) = delete; // forbid copying of this class
    RBBISymbolTableEntry &operator=(ثابت RBBISymbolTableEntry &other) = delete; // forbid copying of this class
};


class RBBISymbolTable : عمومی UMemory, عمومی نماد_Table {
خصوصی:
    ثابت UnicodeString      &fRules;
    UHashtable               *fHashTable;
    RBBIRuleScanner          *fRuleScanner;

عمومی:
    //  API inherited from class SymbolTable
    مجازی ثابت UnicodeString*  lookup(ثابت UnicodeString& s) ثابت override;
    مجازی ثابت UnicodeSet* lookupSet(ثابت UnicodeString& s) ثابت override;
    مجازی ثابت UnicodeFunctor* lookupMatcher(UChar32 ch) ثابت override;
    مجازی UnicodeString parseReference(ثابت UnicodeString& text,
                                         ParsePosition& pos, int32_t limit) ثابت override;

    //  Additional Functions
    RBBISymbolTable(RBBIRuleScanner *, ثابت UnicodeString &fRules, UErrorCode &status);
    مجازی ~RBBISymbolTable();

    مجازی RBBINode *lookup_گره(ثابت UnicodeString &کلید) ثابت;
    مجازی تهی      addEntry  (ثابت UnicodeString &کلید, RBBINode *val, UErrorCode &err);

#ifdef RBBI_DEBUG
    مجازی تهی      rbbiSymtablePrint() ثابت;
#وگرنه
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t fFakeField;
    #define rbbiSymtablePrint() fFakeField=0;
#endif

خصوصی:
    RBBISymbolTable(ثابت RBBISymbolTable &other); // forbid copying of this class
    RBBISymbolTable &operator=(ثابت RBBISymbolTable &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : عمومی UMemory {
عمومی:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    ایستا BreakIterator * createRuleBasedBreakIterator( ثابت UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status);

عمومی:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ثابت UnicodeString  &rules,
                    UParseError          *parseErr,
                    UErrorCode           &status
    );

    مجازی    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIDataHeader  *build(UErrorCode &status);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    تهی optimizeTables();

    نویسه                          *fDebugEnv;        // controls debug trace output
    UErrorCode                    *fStatus;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ثابت UnicodeString           &fRules;           // The rule string that we are compiling
    UnicodeString                 fStrippedRules;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_درخت;     // The parse trees, generated by the scanner,
    RBBINode                      *f_Reverse_درخت;     //   then manipulated by subsequent steps.
    RBBINode                      *f_Safe_Fwd_درخت;
    RBBINode                      *f_Safe_Rev_درخت;

    RBBINode                      **f_Default_درخت;    // For rules not qualified with a !
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
خصوصی:
    RBBIRuleBuilder(ثابت RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &operator=(ثابت RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
ساختار RBBISetTableEl {
    UnicodeString *کلید;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
تعریفنوع std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
#شامل <stdio.h>
#define RBBIDebugPrintf printf
#define RBBIDebugPuts puts
#وگرنه
#undef RBBIDebugPrintf
#define RBBIDebugPuts(arg)
#endif

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif



