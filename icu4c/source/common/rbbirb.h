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


#اگر_تعریف_نشده RBBIRB_H
#تعریف_کن RBBIRB_H

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
class RBBISymbol_جدول_مدخل : عام UMemory { // The symbol table hash table contains one
عام:                                       //   of these structs for each entry.
    RBBISymbol_جدول_مدخل();
    UnicodeString          کلید;
    RBBINode               *val;
    ~RBBISymbol_جدول_مدخل();

خاص:
    RBBISymbol_جدول_مدخل(ثابت RBBISymbol_جدول_مدخل &other) = delete; // forbid copying of this class
    RBBISymbol_جدول_مدخل &عملگر=(ثابت RBBISymbol_جدول_مدخل &other) = delete; // forbid copying of this class
};


class RBBISymbol_جدول : عام UMemory, عام نماد_جدول {
خاص:
    ثابت UnicodeString      &f_قواعد;
    UHashtable               *f_Hash_جدول;
    RBBIRuleScanner          *f_قاعده_Scanner;

عام:
    //  API inherited from class SymbolTable
    مجازی ثابت UnicodeString*  lookup(ثابت UnicodeString& s) ثابت override;
    مجازی ثابت Unicode_مجموعه* lookup_مجموعه(ثابت UnicodeString& s) ثابت override;
    مجازی ثابت UnicodeFunctor* lookupMatcher(UChar32 ch) ثابت override;
    مجازی UnicodeString parseReference(ثابت UnicodeString& متن,
                                         ParsePosition& pos, int32_t حد) ثابت override;

    //  Additional Functions
    RBBISymbol_جدول(RBBIRuleScanner *, ثابت UnicodeString &f_قواعد, UError_کد &وضعیت);
    مجازی ~RBBISymbol_جدول();

    مجازی RBBINode *lookup_گره(ثابت UnicodeString &کلید) ثابت;
    مجازی فراغ      جمع_کن_مدخل  (ثابت UnicodeString &کلید, RBBINode *val, UError_کد &err);

#اگر_تعریف_شده RBBI_DEBUG
    مجازی فراغ      rbbiSymtablePrint() ثابت;
#وإلا
    // A do-nothing inline function for non-debug builds.  Member funcs can't be empty
    //  or the call sites won't compile.
    int32_t f_Fake_فیلد;
    #تعریف_کن rbbiSymtablePrint() f_Fake_فیلد=0;
#پایان_اگر

خاص:
    RBBISymbol_جدول(ثابت RBBISymbol_جدول &other); // forbid copying of this class
    RBBISymbol_جدول &عملگر=(ثابت RBBISymbol_جدول &other); // forbid copying of this class
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
                                    UError_کد       &وضعیت);

عام:
    // The "public" functions and data members that appear below are accessed
    //  (and shared) by the various parts that make up the rule builder.  They
    //  are NOT intended to be accessed by anything outside of the
    //  rule builder implementation.
    RBBIRuleBuilder(ثابت UnicodeString  &قواعد,
                    UParseError          *parseErr,
                    UError_کد           &وضعیت
    );

    مجازی    ~RBBIRuleBuilder();

    /**
     *  Build the state tables and char class Trie from the source rules.
     */
    RBBIData_سرآیند  *build(UError_کد &وضعیت);


    /**
     * Fold together redundant character classes (table columns) and
     * redundant states (table rows). Done after initial table generation,
     * before serializing the result.
     */
    فراغ optimizeTables();

    حرف                          *fDebugEnv;        // controls debug trace output
    UError_کد                    *f_وضعیت;          // Error reporting.  Keeping status
    UParseError                   *fParseError;      //   here avoids passing it everywhere.
    ثابت UnicodeString           &f_قواعد;           // The rule string that we are compiling
    UnicodeString                 f_Stripped_قواعد;    // The rule string, with comments stripped.

    RBBIRuleScanner               *fScanner;         // The scanner.
    RBBINode                      *f_Forward_درخت;     // The parse trees, generated by the scanner,
    RBBINode                      *f_معکوس_درخت;     //   then manipulated by subsequent steps.
    RBBINode                      *f_امن_Fwd_درخت;
    RBBINode                      *f_امن_Rev_درخت;

    RBBINode                      **f_Default_درخت;    // For rules not qualified with a !
                                                     //   the tree to which they belong to.

    UBool                         f_Chain_قواعد;       // True for chained Unicode TR style rules.
                                                     // False for traditional regexp rules.

    UBool                         fLookAheadHardBreak;  // True:  Look ahead matches cause an
                                                     // immediate break, no continuing for the
                                                     // longest match.

    RBBISetBuilder                *f_مجموعه_Builder;      // Set and Character Category builder.
    UVector                       *fUSetNodes;       // Vector of all uset nodes.

    RBBITableBuilder              *f_Forward_جدول;    // State transition table, build time form.

    UVector                       *f_قاعده_وضعیت_Vals;  // The values that can be returned
                                                     //   from getRuleStatus().

    RBBIData_سرآیند                *flatten_داده();    // Create the flattened (runtime format)
                                                     // data tables..
خاص:
    RBBIRuleBuilder(ثابت RBBIRuleBuilder &other) = delete; // forbid copying of this class
    RBBIRuleBuilder &عملگر=(ثابت RBBIRuleBuilder &other) = delete; // forbid copying of this class
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
    UnicodeString *کلید;
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
#اگر_تعریف_شده RBBI_DEBUG
#ضمن <stdio.h>
#تعریف_کن RBBIDebugPrintf چاپ_کن
#تعریف_کن RBBIDebugPuts puts
#وإلا
#لغو_تعریف RBBIDebugPrintf
#تعریف_کن RBBIDebugPuts(arg)
#پایان_اگر

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */

#پایان_اگر



