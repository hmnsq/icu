// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
//
//  rbbiscan.h
//
//  Copyright (C) 2002-2016, International Business Machines Corporation and others.
//  All Rights Reserved.
//
//  This file contains declarations for class RBBIRuleScanner
//


#اگر_غیر_تعریف_شدہ RBBISCAN_H
#تعریف_کرو RBBISCAN_H

#شامل "unicode/utypes.h"
#شامل "unicode/uobject.h"
#شامل "unicode/rbbi.h"
#شامل "unicode/uniset.h"
#شامل "unicode/parseerr.h"
#شامل "uhash.h"
#شامل "uvector.h"
#شامل "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                          //    looks up references to $variables within a set.
#شامل "rbbinode.h"
#شامل "rbbirpt.h"

U_NAMESPACE_BEGIN

class   RBBIRuleBuilder;
class   RBBISymbol_جدول;


//--------------------------------------------------------------------------------
//
//  class RBBIRuleScanner does the lowest level, character-at-a-time
//                        scanning of break iterator rules.  
//
//                        The output of the scanner is parse trees for
//                        the rule expressions and a list of all Unicode Sets
//                        encountered.
//
//--------------------------------------------------------------------------------

class RBBIRuleScanner : عوامی UMemory {
عوامی:

    شمار {
        k_ڈھیر_سائز = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    ساخت RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {}
    };

    RBBIRuleScanner(RBBIRuleBuilder  *rb);


    مجازی    ~RBBIRuleScanner();

    خالی        اگلا_Char(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       ڈالو(ثابت RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    خالی        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     عدد_قواعد();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    ساکن UnicodeString strip_قواعد(ثابت UnicodeString &قواعد);
نجی:

    UBool       doParseActions(int32_t a);
    خالی        error(UError_رمز e);                   // error reporting convenience function.
    خالی        fix_Op_ڈھیر(RBBINode::OpPrecedence p);
                                                       //   a character.
    خالی        ڈھونڈو_سیٹ_For(ثابت UnicodeString &s, RBBINode *گرہ, Unicode_سیٹ *سیٹ_مقصد_Adopt = nullptr);

    UChar32     اگلا_Char_LL();
#اگر_تعریف_شدہ RBBI_DEBUG
    خالی        print_گرہ_ڈھیر(ثابت حرف *title);
#اگر_ختم
    RBBINode    *ڈالو_New_گرہ(RBBINode::گرہ_Type  t);
    خالی        جائزہ_سیٹ();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       f_جائزہ_اشاریہ;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       f_اگلا_اشاریہ;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         f_Quote_منوال;        // Scan is in a 'quoted region'
    int32_t                       f_سطر_عدد;          // Line number in input file.
    int32_t                       f_Char_عدد;          // Char position within the line.
    UChar32                       f_آخری_Char;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRule_جدول_El               **f_State_جدول;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_ڈھیر[k_ڈھیر_سائز];  // State stack, holds state pushes
    int32_t                       f_ڈھیر_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_گرہ_ڈھیر[k_ڈھیر_سائز]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_گرہ_ڈھیر_Ptr;


    UBool                          f_الٹ_قاعدہ;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          f_Look_Ahead_قاعدہ;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          f_No_Chain_اندر_قاعدہ;   // True if the current rule starts with a '^'.

    RBBISymbol_جدول               *f_علامت_جدول;     // symbol table, holds definitions of
                                                     //   $variable symbols.

    UHashtable                    *f_سیٹ_جدول;        // UnicocodeSet hash table, holds indexes to
                                                     //   the sets created while parsing rules.
                                                     //   The key is the string used for creating
                                                     //   the set.

    Unicode_سیٹ                     f_قاعدہ_Sets[10];    // Unicode Sets that are needed during
                                                     //  the scanning of RBBI rules.  The
                                                     //  indices for these are assigned by the
                                                     //  perl script that builds the state tables.
                                                     //  See rbbirpt.h.

    int32_t                        f_قاعدہ_عدد;         // Counts each rule as it is scanned.

    int32_t                        f_Option_آغاز;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    Unicode_سیٹ *g_قاعدہ_سیٹ_قاعدہ_char;
    Unicode_سیٹ *g_قاعدہ_سیٹ_white_space;
    Unicode_سیٹ *g_قاعدہ_سیٹ_name_char;
    Unicode_سیٹ *g_قاعدہ_سیٹ_name_آغاز_char;

    RBBIRuleScanner(ثابت RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &عامل=(ثابت RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_انتہا

#اگر_ختم
