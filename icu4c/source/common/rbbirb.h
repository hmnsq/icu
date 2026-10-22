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


#که_نه_تعریف_شوی RBBIRB_H
#تعریف_کړه RBBIRB_H

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن <utility>

#ضمن "unicode/uobject.h"
#ضمن "unicode/rbbi.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/parseerr.h"
#ضمن "uhash.h"
#ضمن "uvector.h"
#ضمن "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                             //    looks up references to $variables within a set.


U_NAMESPACE_BEGIN

class               RBBIRuleScanner;
بنية              RBBIRule_جدول_El;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbol_جدول_ثبت : عام UMemory { // The symbol table hash table contains one
عام:                                       //   of these structs for each entry.
    RBBISymbol_جدول_ثبت();
    UnicodeString          کلی;
    RBBINode               *val;
    ~RBBISymbol_جدول_ثبت();

خاص:
    RBBISymbol_جدول_ثبت(ثابت RBBISymbol_جدول_ثبت &other) = delete; // forbid copying of this class
    RBBISymbol_جدول_ثبت &چلوونکی=(ثابت RBBISymbol_جدول_ثبت &other) = delete; // forbid copying of this class
};


class RBBISymbol_جدول : عام UMemory, عام نښه_جدول {
خاص:
    ثابت UnicodeString      &f_قواعد;
    UHashtable               *f_Hash_جدول;
    RBBIRuleScanner          *f_قاعده_Scanner;

عام:
    //  API inherited from class SymbolTable
    مجازي ثابت UnicodeString*  lookup(ثابت UnicodeString& s) ثابت override;
    مجازي ثابت Unicode_سېټ* lookup_سېټ(ثابت UnicodeString& s) ثابت override;
    مجازي ثابت UnicodeFunctor* lookupMatcher(UChar32 ch) ثابت override;
    مجازي UnicodeString parseReference(ثابت UnicodeString& متن,
                                         ParsePosition& pos, int32_t حد) ثابت override;

    //  Additional Functions
    RBBISymbol_جدول(RBBIRuleScanner *, ثابت UnicodeString &f_قواعد, UError_کوډ &حالت);
    مجازي ~RBBISymbol_جدول();

    مجازي RBBINode *lookup_غوټه(ثابت UnicodeString &کلی) ثابت;
    مجازي فراغ      جمع_کړه_ثبت  (ثابت UnicodeString &کلی, RBBINode *val, UError_کوډ &err);

#که_تعریف_شوی RBBI_DEBUG
    مجازي فراغ      rbbiSymtablePrint() ثابت;
#وإلا
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t f_Fake_ډګر;
    #تعریف_کړه rbbiSymtablePrint() f_Fake_ډګر=0;
#د_که_پای

خاص:
    RBBISymbol_جدول(ثابت RBBISymbol_جدول &other); // forbid copying of this class
    RBBISymbol_جدول &چلوونکی=(ثابت RBBISymbol_جدول &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : عام UMemory {
عام:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    ساكن BreakIterator * create_قاعده_Based_Break_Iterator( ثابت UnicodeString    &قواعد,
                                    UParseError      *parseError,
                                    UError_کوډ       &حالت);

عام:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ثابت UnicodeString  &قواعد,
                    UParseError          *parseErr,
                    UError_کوډ           &حالت
    );

    مجازي    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIData_سرلیک  *build(UError_کوډ &حالت);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    فراغ optimizeTables();

    حرف                          *fDebugEnv;        // controls debug trace output
    UError_کوډ                    *f_حالت;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ثابت UnicodeString           &f_قواعد;           // The rule string that we are compiling
    UnicodeString                 f_Stripped_قواعد;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_ونه;     // The parse trees, generated by the scanner,
    RBBINode                      *f_سرچپه_ونه;     //   then manipulated by subsequent steps.
    RBBINode                      *f_خوندي_Fwd_ونه;
    RBBINode                      *f_خوندي_Rev_ونه;

    RBBINode                      **f_Default_ونه;    // For rules not qualified with a !
                                                     //   the tree to which they belong to.

    UBool                         f_Chain_قواعد;       // True for chained Unicode TR style rules.
                                                     // False for traditional regexp rules.

    UBool                         fLookAheadHardBreak;  // True:  Look ahead matches cause an
                                                     // immediate break, no continuing for the
                                                     // longest match.

    RBBISetBuilder                *f_سېټ_Builder;      // Set and Character Category builder.
    UVector                       *fUSetNodes;       // Vector of all uset nodes.

    RBBITableBuilder              *f_Forward_جدول;    // State transition table, build time form.

    UVector                       *f_قاعده_حالت_Vals;  // The values that can be returned
                                                     //   from getRuleStatus().

    RBBIData_سرلیک                *flatten_معلومات();    // Create the flattened (runtime format)
                                                     // data tables..
خاص:
    RBBIRuleBuilder(ثابت RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &چلوونکی=(ثابت RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
بنية RBBISet_جدول_El {
    UnicodeString *کلی;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
تعريفنوع std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#که_تعریف_شوی RBBI_DEBUG
#ضمن <stdio.h>
#تعریف_کړه RBBIDebugPrintf چاپ_کړه
#تعریف_کړه RBBIDebugPuts puts
#وإلا
#تعریف_لغوه RBBIDebugPrintf
#تعریف_کړه RBBIDebugPuts(arg)
#د_که_پای

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */

#د_که_پای



