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


#अगर_अघोषित RBBIRB_H
#घोषणा RBBIRB_H

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश <utility>

#समावेश "unicode/uobject.h"
#समावेश "unicode/rbbi.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/parseerr.h"
#समावेश "uhash.h"
#समावेश "uvector.h"
#समावेश "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
काष्ठा              RBBIRuleTableEl;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbolTableEntry : खुला UMemory { // The symbol table hash table contains one
खुला:                                       //   of these structs for each entry.
    RBBISymbolTableEntry();
    UnicodeString          कुंजी;
    RBBINode               *val;
    ~RBBISymbolTableEntry();

निजी:
    RBBISymbolTableEntry(स्थिर RBBISymbolTableEntry &other) = delete; // forbid copying of this class
    RBBISymbolTableEntry &चालक=(स्थिर RBBISymbolTableEntry &other) = delete; // forbid copying of this class
};


class RBBISymbolTable : खुला UMemory, खुला प्रतीक_Table {
निजी:
    स्थिर UnicodeString      &fRules;
    UHashtable               *fHashTable;
    RBBIRuleScanner          *fRuleScanner;

खुला:
    //  API inherited from class SymbolTable
    भव स्थिर UnicodeString*  lookup(स्थिर UnicodeString& s) स्थिर override;
    भव स्थिर UnicodeSet* lookupSet(स्थिर UnicodeString& s) स्थिर override;
    भव स्थिर UnicodeFunctor* lookupMatcher(UChar32 ch) स्थिर override;
    भव UnicodeString parseReference(स्थिर UnicodeString& text,
                                         ParsePosition& pos, int32_t limit) स्थिर override;

    //  Additional Functions
    RBBISymbolTable(RBBIRuleScanner *, स्थिर UnicodeString &fRules, UErrorCode &status);
    भव ~RBBISymbolTable();

    भव RBBINode *lookup_गाँठ(स्थिर UnicodeString &कुंजी) स्थिर;
    भव व्योम      addEntry  (स्थिर UnicodeString &कुंजी, RBBINode *val, UErrorCode &err);

#अगर_घोषित RBBI_DEBUG
    भव व्योम      rbbiSymtablePrint() स्थिर;
#अन्यथा
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t fFakeField;
    #घोषणा rbbiSymtablePrint() fFakeField=0;
#पूर्ण_अगर

निजी:
    RBBISymbolTable(स्थिर RBBISymbolTable &other); // forbid copying of this class
    RBBISymbolTable &चालक=(स्थिर RBBISymbolTable &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : खुला UMemory {
खुला:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    अटल BreakIterator * createRuleBasedBreakIterator( स्थिर UnicodeString    &rules,
                                    UParseError      *parseError,
                                    UErrorCode       &status);

खुला:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(स्थिर UnicodeString  &rules,
                    UParseError          *parseErr,
                    UErrorCode           &status
    );

    भव    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIDataHeader  *build(UErrorCode &status);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    व्योम optimizeTables();

    अक्षर                          *fDebugEnv;        // controls debug trace output
    UErrorCode                    *fStatus;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    स्थिर UnicodeString           &fRules;           // The rule string that we are compiling
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

    RBBIDataHeader                *flatten_तथ्य();    // Create the flattened (runtime format)
                                                     // data tables..
निजी:
    RBBIRuleBuilder(स्थिर RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &चालक=(स्थिर RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
काष्ठा RBBISetTableEl {
    UnicodeString *कुंजी;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
प्रकार std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG
#समावेश <stdio.h>
#घोषणा RBBIDebugPrintf म_लिखो
#घोषणा RBBIDebugPuts माला_दो
#अन्यथा
#अघोषित RBBIDebugPrintf
#घोषणा RBBIDebugPuts(arg)
#पूर्ण_अगर

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */

#पूर्ण_अगर



