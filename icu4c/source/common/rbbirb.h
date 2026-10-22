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


#నిర్వచితం_కాకపోతే RBBIRB_H
#నిర్వచించు RBBIRB_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ <utility>

#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/rbbi.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/parseerr.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "uvector.h"
#అన్తర్భావ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
నిర్మాణం              RBBIRule_పట్టిక_El;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbol_పట్టిక_నమోదు : బహిరంగ UMemory { // The symbol table hash table contains one
బహిరంగ:                                       //   of these structs for each entry.
    RBBISymbol_పట్టిక_నమోదు();
    UnicodeString          కీలకం;
    RBBINode               *val;
    ~RBBISymbol_పట్టిక_నమోదు();

అంతరంగ:
    RBBISymbol_పట్టిక_నమోదు(స్థిరరాశి RBBISymbol_పట్టిక_నమోదు &other) = delete; // forbid copying of this class
    RBBISymbol_పట్టిక_నమోదు &క్రియాకారకం=(స్థిరరాశి RBBISymbol_పట్టిక_నమోదు &other) = delete; // forbid copying of this class
};


class RBBISymbol_పట్టిక : బహిరంగ UMemory, బహిరంగ చిహ్నం_పట్టిక {
అంతరంగ:
    స్థిరరాశి UnicodeString      &f_నియమాలు;
    UHashtable               *f_Hash_పట్టిక;
    RBBIRuleScanner          *f_నియమం_Scanner;

బహిరంగ:
    //  API inherited from class SymbolTable
    మాయా స్థిరరాశి UnicodeString*  lookup(స్థిరరాశి UnicodeString& s) స్థిరరాశి override;
    మాయా స్థిరరాశి Unicode_సమితి* lookup_సమితి(స్థిరరాశి UnicodeString& s) స్థిరరాశి override;
    మాయా స్థిరరాశి UnicodeFunctor* lookupMatcher(UChar32 ch) స్థిరరాశి override;
    మాయా UnicodeString parseReference(స్థిరరాశి UnicodeString& పాఠ్యం,
                                         ParsePosition& pos, int32_t హద్దు) స్థిరరాశి override;

    //  Additional Functions
    RBBISymbol_పట్టిక(RBBIRuleScanner *, స్థిరరాశి UnicodeString &f_నియమాలు, UError_సంకేతం &స్థితి_2);
    మాయా ~RBBISymbol_పట్టిక();

    మాయా RBBINode *lookup_కణుపు(స్థిరరాశి UnicodeString &కీలకం) స్థిరరాశి;
    మాయా శూన్య      కూడు_నమోదు  (స్థిరరాశి UnicodeString &కీలకం, RBBINode *val, UError_సంకేతం &err);

#నిర్వచితమైతే RBBI_DEBUG
    మాయా శూన్య      rbbiSymtablePrint() స్థిరరాశి;
#లేకపోతే
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t f_Fake_క్షేత్రం;
    #నిర్వచించు rbbiSymtablePrint() f_Fake_క్షేత్రం=0;
#అయితే_ముగింపు

అంతరంగ:
    RBBISymbol_పట్టిక(స్థిరరాశి RBBISymbol_పట్టిక &other); // forbid copying of this class
    RBBISymbol_పట్టిక &క్రియాకారకం=(స్థిరరాశి RBBISymbol_పట్టిక &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : బహిరంగ UMemory {
బహిరంగ:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    స్థిర BreakIterator * create_నియమం_Based_Break_Iterator( స్థిరరాశి UnicodeString    &నియమాలు,
                                    UParseError      *parseError,
                                    UError_సంకేతం       &స్థితి_2);

బహిరంగ:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(స్థిరరాశి UnicodeString  &నియమాలు,
                    UParseError          *parseErr,
                    UError_సంకేతం           &స్థితి_2
    );

    మాయా    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIData_శీర్షిక  *build(UError_సంకేతం &స్థితి_2);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    శూన్య optimizeTables();

    అక్షరం                          *fDebugEnv;        // controls debug trace output
    UError_సంకేతం                    *f_స్థితి;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    స్థిరరాశి UnicodeString           &f_నియమాలు;           // The rule string that we are compiling
    UnicodeString                 f_Stripped_నియమాలు;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_వృక్షం;     // The parse trees, generated by the scanner,
    RBBINode                      *f_తిరగేయి_వృక్షం;     //   then manipulated by subsequent steps.
    RBBINode                      *f_సురక్షిత_Fwd_వృక్షం;
    RBBINode                      *f_సురక్షిత_Rev_వృక్షం;

    RBBINode                      **f_Default_వృక్షం;    // For rules not qualified with a !
                                                     //   the tree to which they belong to.

    UBool                         f_Chain_నియమాలు;       // True for chained Unicode TR style rules.
                                                     // False for traditional regexp rules.

    UBool                         fLookAheadHardBreak;  // True:  Look ahead matches cause an
                                                     // immediate break, no continuing for the
                                                     // longest match.

    RBBISetBuilder                *f_సమితి_Builder;      // Set and Character Category builder.
    UVector                       *fUSetNodes;       // Vector of all uset nodes.

    RBBITableBuilder              *f_Forward_పట్టిక;    // State transition table, build time form.

    UVector                       *f_నియమం_స్థితి_Vals;  // The values that can be returned
                                                     //   from getRuleStatus().

    RBBIData_శీర్షిక                *flatten_దత్తాంశం();    // Create the flattened (runtime format)
                                                     // data tables..
అంతరంగ:
    RBBIRuleBuilder(స్థిరరాశి RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &క్రియాకారకం=(స్థిరరాశి RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
నిర్మాణం RBBISet_పట్టిక_El {
    UnicodeString *కీలకం;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
రకనిర్వచనం std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG
#అన్తర్భావ <stdio.h>
#నిర్వచించు RBBIDebugPrintf రాయి
#నిర్వచించు RBBIDebugPuts puts
#లేకపోతే
#నిర్వచనం_రద్దు RBBIDebugPrintf
#నిర్వచించు RBBIDebugPuts(arg)
#అయితే_ముగింపు

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */

#అయితే_ముగింపు



