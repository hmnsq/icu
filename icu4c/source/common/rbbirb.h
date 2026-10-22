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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_BREAK_ITERATION

#அந்தர்பாவ <utility>

#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "unicode/rbbi.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/parseerr.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "uvector.h"
#அந்தர்பாவ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
கட்டமைப்பு              RBBIRuleTableEl;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbolTableEntry : பொது UMemory { // The symbol table hash table contains one
பொது:                                       //   of these structs for each entry.
    RBBISymbolTableEntry();
    UnicodeString          சாவி;
    RBBINode               *val;
    ~RBBISymbolTableEntry();

தனிப்பட்ட:
    RBBISymbolTableEntry(மாறிலி RBBISymbolTableEntry &other) = delete; // forbid copying of this class
    RBBISymbolTableEntry &operator=(மாறிலி RBBISymbolTableEntry &other) = delete; // forbid copying of this class
};


class RBBISymbolTable : பொது UMemory, பொது குறியீடு_Table {
தனிப்பட்ட:
    மாறிலி UnicodeString      &fRules;
    UHashtable               *fHashTable;
    RBBIRuleScanner          *fRuleScanner;

பொது:
    //  API inherited from class SymbolTable
    மெய்நிகர் மாறிலி UnicodeString*  lookup(மாறிலி UnicodeString& s) மாறிலி override;
    மெய்நிகர் மாறிலி UnicodeSet* lookupSet(மாறிலி UnicodeString& s) மாறிலி override;
    மெய்நிகர் மாறிலி UnicodeFunctor* lookupMatcher(UChar32 ch) மாறிலி override;
    மெய்நிகர் UnicodeString parseReference(மாறிலி UnicodeString& text,
                                         ParsePosition& pos, int32_t limit) மாறிலி override;

    //  Additional Functions
    RBBISymbolTable(RBBIRuleScanner *, மாறிலி UnicodeString &fRules, UErrorCode &status);
    மெய்நிகர் ~RBBISymbolTable();

    மெய்நிகர் RBBINode *lookup_முனை(மாறிலி UnicodeString &சாவி) மாறிலி;
    மெய்நிகர் வெறுமை      addEntry  (மாறிலி UnicodeString &சாவி, RBBINode *val, UErrorCode &err);

#ifdef RBBI_DEBUG
    மெய்நிகர் வெறுமை      rbbiSymtablePrint() மாறிலி;
#இல்லையெனில்
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t fFakeField;
    #define rbbiSymtablePrint() fFakeField=0;
#endif

தனிப்பட்ட:
    RBBISymbolTable(மாறிலி RBBISymbolTable &other); // forbid copying of this class
    RBBISymbolTable &operator=(மாறிலி RBBISymbolTable &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : பொது UMemory {
பொது:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    நிலையான BreakIterator * createRuleBasedBreakIterator( மாறிலி UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status);

பொது:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(மாறிலி UnicodeString  &rules,
                    UParseError          *parseErr,
                    UErrorCode           &status
    );

    மெய்நிகர்    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIDataHeader  *build(UErrorCode &status);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    வெறுமை optimizeTables();

    எழுத்து                          *fDebugEnv;        // controls debug trace output
    UErrorCode                    *fStatus;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    மாறிலி UnicodeString           &fRules;           // The rule string that we are compiling
    UnicodeString                 fStrippedRules;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_மரம்;     // The parse trees, generated by the scanner,
    RBBINode                      *f_Reverse_மரம்;     //   then manipulated by subsequent steps.
    RBBINode                      *f_Safe_Fwd_மரம்;
    RBBINode                      *f_Safe_Rev_மரம்;

    RBBINode                      **f_Default_மரம்;    // For rules not qualified with a !
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
தனிப்பட்ட:
    RBBIRuleBuilder(மாறிலி RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &operator=(மாறிலி RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
கட்டமைப்பு RBBISetTableEl {
    UnicodeString *சாவி;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
வகைவரை std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
#அந்தர்பாவ <stdio.h>
#define RBBIDebugPrintf printf
#define RBBIDebugPuts puts
#இல்லையெனில்
#undef RBBIDebugPrintf
#define RBBIDebugPuts(arg)
#endif

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif



