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


#اگر_غیر_تعریف_شدہ RBBIRB_H
#تعریف_کرو RBBIRB_H

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
ساخت              RBBIRule_جدول_El;
class               RBBISetBuilder;
class               RBBINode;
class               RBBITableBuilder;



//--------------------------------------------------------------------------------
//
//   RBBISymbolTable.    Implements SymbolTable interface that is used by the
//                       UnicodeSet parser to resolve references to $variables.
//
//--------------------------------------------------------------------------------
class RBBISymbol_جدول_اندراج : عوامی UMemory { // The symbol table hash table contains one
عوامی:                                       //   of these structs for each entry.
    RBBISymbol_جدول_اندراج();
    UnicodeString          کلید;
    RBBINode               *val;
    ~RBBISymbol_جدول_اندراج();

نجی:
    RBBISymbol_جدول_اندراج(ثابت RBBISymbol_جدول_اندراج &other) = delete; // forbid copying of this class
    RBBISymbol_جدول_اندراج &عامل=(ثابت RBBISymbol_جدول_اندراج &other) = delete; // forbid copying of this class
};


class RBBISymbol_جدول : عوامی UMemory, عوامی علامت_جدول {
نجی:
    ثابت UnicodeString      &f_قواعد;
    UHashtable               *f_Hash_جدول;
    RBBIRuleScanner          *f_قاعدہ_Scanner;

عوامی:
    //  API inherited from class SymbolTable
    مجازی ثابت UnicodeString*  lookup(ثابت UnicodeString& s) ثابت override;
    مجازی ثابت Unicode_سیٹ* lookup_سیٹ(ثابت UnicodeString& s) ثابت override;
    مجازی ثابت UnicodeFunctor* lookupMatcher(UChar32 ch) ثابت override;
    مجازی UnicodeString parseReference(ثابت UnicodeString& متن,
                                         ParsePosition& pos, int32_t حد_بندی) ثابت override;

    //  Additional Functions
    RBBISymbol_جدول(RBBIRuleScanner *, ثابت UnicodeString &f_قواعد, UError_رمز &حالت_2);
    مجازی ~RBBISymbol_جدول();

    مجازی RBBINode *lookup_گرہ(ثابت UnicodeString &کلید) ثابت;
    مجازی خالی      جوڑو_اندراج  (ثابت UnicodeString &کلید, RBBINode *val, UError_رمز &err);

#اگر_تعریف_شدہ RBBI_DEBUG
    مجازی خالی      rbbiSymtablePrint() ثابت;
#ورنہ
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t f_Fake_میدان;
    #تعریف_کرو rbbiSymtablePrint() f_Fake_میدان=0;
#اگر_ختم

نجی:
    RBBISymbol_جدول(ثابت RBBISymbol_جدول &other); // forbid copying of this class
    RBBISymbol_جدول &عامل=(ثابت RBBISymbol_جدول &other); // forbid copying of this class
};


//--------------------------------------------------------------------------------
//
//  class RBBIRuleBuilder       The top-level class handling RBBI rule compiling.
//
//--------------------------------------------------------------------------------
class RBBIRuleBuilder : عوامی UMemory {
عوامی:

    //  Create a rule based break iterator from a set of rules.
    //  This function is the main entry point into the rule builder.  The
    //   public ICU API for creating RBBIs uses this function to do the actual work.
    //
    ساکن BreakIterator * create_قاعدہ_Based_Break_Iterator( ثابت UnicodeString    &قواعد,
                                    UParseError      *parseError,
                                    UError_رمز       &حالت_2);

عوامی:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ثابت UnicodeString  &قواعد,
                    UParseError          *parseErr,
                    UError_رمز           &حالت_2
    );

    مجازی    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIData_سرخی  *build(UError_رمز &حالت_2);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    خالی optimizeTables();

    حرف                          *fDebugEnv;        // controls debug trace output
    UError_رمز                    *f_حالت;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ثابت UnicodeString           &f_قواعد;           // The rule string that we are compiling
    UnicodeString                 f_Stripped_قواعد;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_درخت;     // The parse trees, generated by the scanner,
    RBBINode                      *f_الٹ_درخت;     //   then manipulated by subsequent steps.
    RBBINode                      *f_محفوظ_Fwd_درخت;
    RBBINode                      *f_محفوظ_Rev_درخت;

    RBBINode                      **f_Default_درخت;    // For rules not qualified with a !
                                                     //   the tree to which they belong to.

    UBool                         f_Chain_قواعد;       // True for chained Unicode TR style rules.
                                                     // False for traditional regexp rules.

    UBool                         fLookAheadHardBreak;  // True:  Look ahead matches cause an
                                                     // immediate break, no continuing for the
                                                     // longest match.

    RBBISetBuilder                *f_سیٹ_Builder;      // Set and Character Category builder.
    UVector                       *fUSetNodes;       // Vector of all uset nodes.

    RBBITableBuilder              *f_Forward_جدول;    // State transition table, build time form.

    UVector                       *f_قاعدہ_حالت_Vals;  // The values that can be returned
                                                     //   from getRuleStatus().

    RBBIData_سرخی                *flatten_ڈیٹا();    // Create the flattened (runtime format)
                                                     // data tables..
نجی:
    RBBIRuleBuilder(ثابت RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &عامل=(ثابت RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
ساخت RBBISet_جدول_El {
    UnicodeString *کلید;
    RBBINode      *val;
};

/**
 *   A pair of ints, used to bundle pairs of states or pairs of character classes.
 */
قسمتعریف std::pair<int32_t, int32_t> IntPair;


//----------------------------------------------------------------------------
//
//   RBBIDebugPrintf    Printf equivalent, for debugging output.
//                      Conditional compilation of the implementation lets us
//                      get rid of the stdio dependency in environments where it
//                      is unavailable.
//
//----------------------------------------------------------------------------
#اگر_تعریف_شدہ RBBI_DEBUG
#شامل <stdio.h>
#تعریف_کرو RBBIDebugPrintf لکھو
#تعریف_کرو RBBIDebugPuts puts
#ورنہ
#تعریف_منسوخ RBBIDebugPrintf
#تعریف_کرو RBBIDebugPuts(arg)
#اگر_ختم

U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */

#اگر_ختم



