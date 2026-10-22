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


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RBBIRB_H
#ವ್ಯಾಖ್ಯಾನಿಸು RBBIRB_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

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
ರಚನೆ              RBBIRule_ಕೋಷ್ಟಕ_El;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು : ಸಾರ್ವಜನಿಕ UMemory { // The symbol table hash table contains one
ಸಾರ್ವಜನಿಕ:                                       //   of these structs for each entry.
    RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು();
    UnicodeString          ಕೀಲಿ;
    RBBINode               *val;
    ~RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು();

ಖಾಸಗಿ:
    RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು(ಸ್ಥಿರಾಂಕ RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು &other) = delete; // forbid copying of this class
    RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ RBBISymbol_ಕೋಷ್ಟಕ_ನಮೂದು &other) = delete; // forbid copying of this class
};


class RBBISymbol_ಕೋಷ್ಟಕ : ಸಾರ್ವಜನಿಕ UMemory, ಸಾರ್ವಜನಿಕ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ {
ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ UnicodeString      &f_ನಿಯಮಗಳು;
    UHashtable               *f_Hash_ಕೋಷ್ಟಕ;
    RBBIRuleScanner          *f_ನಿಯಮ_Scanner;

ಸಾರ್ವಜನಿಕ:
    //  API inherited from class SymbolTable
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ UnicodeString*  lookup(ಸ್ಥಿರಾಂಕ UnicodeString& s) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ Unicode_ಗಣ* lookup_ಗಣ(ಸ್ಥಿರಾಂಕ UnicodeString& s) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ UnicodeFunctor* lookupMatcher(UChar32 ch) ಸ್ಥಿರಾಂಕ override;
    ವಾಸ್ತವಿಕ UnicodeString parseReference(ಸ್ಥಿರಾಂಕ UnicodeString& ಪಠ್ಯ,
                                         ParsePosition& pos, int32_t ಎಲ್ಲೆ) ಸ್ಥಿರಾಂಕ override;

    //  Additional Functions
    RBBISymbol_ಕೋಷ್ಟಕ(RBBIRuleScanner *, ಸ್ಥಿರಾಂಕ UnicodeString &f_ನಿಯಮಗಳು, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);
    ವಾಸ್ತವಿಕ ~RBBISymbol_ಕೋಷ್ಟಕ();

    ವಾಸ್ತವಿಕ RBBINode *lookup_ಗಂಟು(ಸ್ಥಿರಾಂಕ UnicodeString &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ;
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ      ಕೂಡಿಸು_ನಮೂದು  (ಸ್ಥಿರಾಂಕ UnicodeString &ಕೀಲಿ, RBBINode *val, UError_ಸಂಕೇತ &err);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ      rbbiSymtablePrint() ಸ್ಥಿರಾಂಕ;
#ಇಲ್ಲದಿದ್ದರೆ
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t f_Fake_ಕ್ಷೇತ್ರ;
    #ವ್ಯಾಖ್ಯಾನಿಸು rbbiSymtablePrint() f_Fake_ಕ್ಷೇತ್ರ=0;
#ಆದರೆ_ಅಂತ್ಯ

ಖಾಸಗಿ:
    RBBISymbol_ಕೋಷ್ಟಕ(ಸ್ಥಿರಾಂಕ RBBISymbol_ಕೋಷ್ಟಕ &other); // forbid copying of this class
    RBBISymbol_ಕೋಷ್ಟಕ &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ RBBISymbol_ಕೋಷ್ಟಕ &other); // forbid copying of this class
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
    ಸ್ಥಿರ BreakIterator * create_ನಿಯಮ_Based_Break_Iterator( ಸ್ಥಿರಾಂಕ UnicodeString    &ನಿಯಮಗಳು,
                                    UParseError      *parseError,
                                    UError_ಸಂಕೇತ       &ಸ್ಥಿತಿ_2);

ಸಾರ್ವಜನಿಕ:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ಸ್ಥಿರಾಂಕ UnicodeString  &ನಿಯಮಗಳು,
                    UParseError          *parseErr,
                    UError_ಸಂಕೇತ           &ಸ್ಥಿತಿ_2
    );

    ವಾಸ್ತವಿಕ    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIData_ಶೀರ್ಷಿಕೆ  *build(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    ಶೂನ್ಯ optimizeTables();

    ಅಕ್ಷರ                          *fDebugEnv;        // controls debug trace output
    UError_ಸಂಕೇತ                    *f_ಸ್ಥಿತಿ;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ಸ್ಥಿರಾಂಕ UnicodeString           &f_ನಿಯಮಗಳು;           // The rule string that we are compiling
    UnicodeString                 f_Stripped_ನಿಯಮಗಳು;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_ವೃಕ್ಷ;     // The parse trees, generated by the scanner,
    RBBINode                      *f_ಹಿಮ್ಮುಖ_ವೃಕ್ಷ;     //   then manipulated by subsequent steps.
    RBBINode                      *f_ಸುರಕ್ಷಿತ_Fwd_ವೃಕ್ಷ;
    RBBINode                      *f_ಸುರಕ್ಷಿತ_Rev_ವೃಕ್ಷ;

    RBBINode                      **f_Default_ವೃಕ್ಷ;    // For rules not qualified with a !
                                                     //   the tree to which they belong to.

    UBool                         f_Chain_ನಿಯಮಗಳು;       // True for chained Unicode TR style rules.
                                                     // False for traditional regexp rules.

    UBool                         fLookAheadHardBreak;  // True:  Look ahead matches cause an
                                                     // immediate break, no continuing for the
                                                     // longest match.

    RBBISetBuilder                *f_ಗಣ_Builder;      // Set and Character Category builder.
    UVector                       *fUSetNodes;       // Vector of all uset nodes.

    RBBITableBuilder              *f_Forward_ಕೋಷ್ಟಕ;    // State transition table, build time form.

    UVector                       *f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals;  // The values that can be returned
                                                     //   from getRuleStatus().

    RBBIData_ಶೀರ್ಷಿಕೆ                *flatten_ದತ್ತಾಂಶ();    // Create the flattened (runtime format)
                                                     // data tables..
ಖಾಸಗಿ:
    RBBIRuleBuilder(ಸ್ಥಿರಾಂಕ RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
ರಚನೆ RBBISet_ಕೋಷ್ಟಕ_El {
    UnicodeString *ಕೀಲಿ;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ವ್ಯಾಖ್ಯಾನಿಸು RBBIDebugPrintf ಬರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು RBBIDebugPuts puts
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು RBBIDebugPrintf
#ವ್ಯಾಖ್ಯಾನಿಸು RBBIDebugPuts(arg)
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */

#ಆದರೆ_ಅಂತ್ಯ



