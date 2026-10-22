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


#यदि_अपरिभाषितम् RBBIRB_H
#परिभाषय RBBIRB_H

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
संरचना              RBBIRule_सारणी_El;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbol_सारणी_प्रविष्टिः : सार्वजनिक UMemory { // The symbol table hash table contains one
सार्वजनिक:                                       //   of these structs for each entry.
    RBBISymbol_सारणी_प्रविष्टिः();
    UnicodeString          कुञ्चिका;
    RBBINode               *val;
    ~RBBISymbol_सारणी_प्रविष्टिः();

गुप्त:
    RBBISymbol_सारणी_प्रविष्टिः(ध्रुव RBBISymbol_सारणी_प्रविष्टिः &other) = delete; // forbid copying of this class
    RBBISymbol_सारणी_प्रविष्टिः &संक्रियकः=(ध्रुव RBBISymbol_सारणी_प्रविष्टिः &other) = delete; // forbid copying of this class
};


class RBBISymbol_सारणी : सार्वजनिक UMemory, सार्वजनिक चिह्नम्_सारणी {
गुप्त:
    ध्रुव UnicodeString      &f_नियमाः;
    UHashtable               *f_Hash_सारणी;
    RBBIRuleScanner          *f_नियमः_Scanner;

सार्वजनिक:
    //  API inherited from class SymbolTable
    आभास ध्रुव UnicodeString*  lookup(ध्रुव UnicodeString& s) ध्रुव override;
    आभास ध्रुव Unicode_समुच्चयः* lookup_समुच्चयः(ध्रुव UnicodeString& s) ध्रुव override;
    आभास ध्रुव UnicodeFunctor* lookupMatcher(UChar32 ch) ध्रुव override;
    आभास UnicodeString parseReference(ध्रुव UnicodeString& text,
                                         ParsePosition& pos, int32_t सीमा) ध्रुव override;

    //  Additional Functions
    RBBISymbol_सारणी(RBBIRuleScanner *, ध्रुव UnicodeString &f_नियमाः, UError_संहिता &स्थितिः);
    आभास ~RBBISymbol_सारणी();

    आभास RBBINode *lookup_ग्रन्थिः(ध्रुव UnicodeString &कुञ्चिका) ध्रुव;
    आभास शून्य      सङ्कलय_प्रविष्टिः  (ध्रुव UnicodeString &कुञ्चिका, RBBINode *val, UError_संहिता &err);

#यदि_परिभाषितम् RBBI_DEBUG
    आभास शून्य      rbbiSymtablePrint() ध्रुव;
#अन्यथा
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t f_Fake_क्षेत्रम्;
    #परिभाषय rbbiSymtablePrint() f_Fake_क्षेत्रम्=0;
#यदि_अन्तः

गुप्त:
    RBBISymbol_सारणी(ध्रुव RBBISymbol_सारणी &other); // forbid copying of this class
    RBBISymbol_सारणी &संक्रियकः=(ध्रुव RBBISymbol_सारणी &other); // forbid copying of this class
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
    स्थिर BreakIterator * create_नियमः_Based_Break_Iterator( ध्रुव UnicodeString    &नियमाः,
                                    UParseError      *parseError,
                                    UError_संहिता       &स्थितिः);

सार्वजनिक:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ध्रुव UnicodeString  &नियमाः,
                    UParseError          *parseErr,
                    UError_संहिता           &स्थितिः
    );

    आभास    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIData_शीर्षकम्  *build(UError_संहिता &स्थितिः);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    शून्य optimizeTables();

    वर्ण                          *fDebugEnv;        // controls debug trace output
    UError_संहिता                    *f_स्थितिः;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ध्रुव UnicodeString           &f_नियमाः;           // The rule string that we are compiling
    UnicodeString                 f_Stripped_नियमाः;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_वृक्षः;     // The parse trees, generated by the scanner,
    RBBINode                      *f_Reverse_वृक्षः;     //   then manipulated by subsequent steps.
    RBBINode                      *f_Safe_Fwd_वृक्षः;
    RBBINode                      *f_Safe_Rev_वृक्षः;

    RBBINode                      **f_Default_वृक्षः;    // For rules not qualified with a !
                                                     //   the tree to which they belong to.

    UBool                         f_Chain_नियमाः;       // True for chained Unicode TR style rules.
                                                     // False for traditional regexp rules.

    UBool                         fLookAheadHardBreak;  // True:  Look ahead matches cause an
                                                     // immediate break, no continuing for the
                                                     // longest match.

    RBBISetBuilder                *f_समुच्चयः_Builder;      // Set and Character Category builder.
    UVector                       *fUSetNodes;       // Vector of all uset nodes.

    RBBITableBuilder              *f_Forward_सारणी;    // State transition table, build time form.

    UVector                       *f_नियमः_स्थितिः_Vals;  // The values that can be returned
                                                     //   from getRuleStatus().

    RBBIData_शीर्षकम्                *flatten_दत्त();    // Create the flattened (runtime format)
                                                     // data tables..
गुप्त:
    RBBIRuleBuilder(ध्रुव RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &संक्रियकः=(ध्रुव RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
संरचना RBBISet_सारणी_El {
    UnicodeString *कुञ्चिका;
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
#यदि_परिभाषितम् RBBI_DEBUG
#अन्तर्भाव <stdio.h>
#परिभाषय RBBIDebugPrintf प्रारूप_लिख
#परिभाषय RBBIDebugPuts पङ्क्तिं_लिख
#अन्यथा
#अपरिभाषय RBBIDebugPrintf
#परिभाषय RBBIDebugPuts(arg)
#यदि_अन्तः

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */

#यदि_अन्तः



