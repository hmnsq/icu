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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/rbbi.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/parseerr.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
संरचना              RBBIRuleTableEl;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbolTableEntry : सार्वजनिक UMemory { // The symbol table hash table contains one
सार्वजनिक:                                       //   of these structs for each entry.
    RBBISymbolTableEntry();
    UnicodeString          किल्ली;
    RBBINode               *val;
    ~RBBISymbolTableEntry();

गुप्त:
    RBBISymbolTableEntry(ध्रुव RBBISymbolTableEntry &other) = delete; // forbid copying of this class
    RBBISymbolTableEntry &operator=(ध्रुव RBBISymbolTableEntry &other) = delete; // forbid copying of this class
};


class RBBISymbolTable : सार्वजनिक UMemory, सार्वजनिक चिन्ह_Table {
गुप्त:
    ध्रुव UnicodeString      &fRules;
    UHashtable               *fHashTable;
    RBBIRuleScanner          *fRuleScanner;

सार्वजनिक:
    //  API inherited from class SymbolTable
    आभास ध्रुव UnicodeString*  lookup(ध्रुव UnicodeString& s) ध्रुव override;
    आभास ध्रुव UnicodeSet* lookupSet(ध्रुव UnicodeString& s) ध्रुव override;
    आभास ध्रुव UnicodeFunctor* lookupMatcher(UChar32 ch) ध्रुव override;
    आभास UnicodeString parseReference(ध्रुव UnicodeString& text,
                                         ParsePosition& pos, int32_t limit) ध्रुव override;

    //  Additional Functions
    RBBISymbolTable(RBBIRuleScanner *, ध्रुव UnicodeString &fRules, UErrorCode &status);
    आभास ~RBBISymbolTable();

    आभास RBBINode *lookup_गाठ(ध्रुव UnicodeString &किल्ली) ध्रुव;
    आभास शून्य      addEntry  (ध्रुव UnicodeString &किल्ली, RBBINode *val, UErrorCode &err);

#ifdef RBBI_DEBUG
    आभास शून्य      rbbiSymtablePrint() ध्रुव;
#अन्यथा
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t fFakeField;
    #define rbbiSymtablePrint() fFakeField=0;
#endif

गुप्त:
    RBBISymbolTable(ध्रुव RBBISymbolTable &other); // forbid copying of this class
    RBBISymbolTable &operator=(ध्रुव RBBISymbolTable &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : सार्वजनिक UMemory {
सार्वजनिक:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    स्थिर BreakIterator * createRuleBasedBreakIterator( ध्रुव UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status);

सार्वजनिक:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ध्रुव UnicodeString  &rules,
                    UParseError          *parseErr,
                    UErrorCode           &status
    );

    आभास    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIDataHeader  *build(UErrorCode &status);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    शून्य optimizeTables();

    वर्ण                          *fDebugEnv;        // controls debug trace output
    UErrorCode                    *fStatus;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ध्रुव UnicodeString           &fRules;           // The rule string that we are compiling
    UnicodeString                 fStrippedRules;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_वृक्ष;     // The parse trees, generated by the scanner,
    RBBINode                      *f_Reverse_वृक्ष;     //   then manipulated by subsequent steps.
    RBBINode                      *f_Safe_Fwd_वृक्ष;
    RBBINode                      *f_Safe_Rev_वृक्ष;

    RBBINode                      **f_Default_वृक्ष;    // For rules not qualified with a !
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
गुप्त:
    RBBIRuleBuilder(ध्रुव RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &operator=(ध्रुव RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
संरचना RBBISetTableEl {
    UnicodeString *किल्ली;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
प्रकारनिर्देश std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
#अन्तर्भाव <stdio.h>
#define RBBIDebugPrintf printf
#define RBBIDebugPuts puts
#अन्यथा
#undef RBBIDebugPrintf
#define RBBIDebugPuts(arg)
#endif

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */

#endif



