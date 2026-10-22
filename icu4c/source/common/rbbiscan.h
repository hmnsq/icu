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


#ifndef RBBISCAN_H
#define RBBISCAN_H

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
class   RBBISymbolTable;


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

class RBBIRuleScanner : عمومی UMemory {
عمومی:

    شمارش {
        k_پشته_Size = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    ساختار RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {}
    };

    RBBIRuleScanner(RBBIRuleBuilder  *rb);


    مجازی    ~RBBIRuleScanner();

    تهی        nextChar(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       افزودن(ثابت RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    تهی        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     رقم_Rules();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    ایستا UnicodeString stripRules(ثابت UnicodeString &rules);
خصوصی:

    UBool       doParseActions(int32_t a);
    تهی        error(UErrorCode e);                   // error reporting convenience function.
    تهی        fix_Op_پشته(RBBINode::OpPrecedence p);
                                                       //   a character.
    تهی        findSetFor(ثابت UnicodeString &s, RBBINode *گره, UnicodeSet *setToAdopt = nullptr);

    UChar32     nextCharLL();
#ifdef RBBI_DEBUG
    تهی        print_گره_پشته(ثابت نویسه *title);
#endif
    RBBINode    *افزودن_New_گره(RBBINode::گره_Type  t);
    تهی        scanSet();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       fScanIndex;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       fNextIndex;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         fQuoteMode;        // Scan is in a 'quoted region'
    int32_t                       f_سطر_رقم;          // Line number in input file.
    int32_t                       f_Char_رقم;          // Char position within the line.
    UChar32                       fLastChar;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRuleTableEl               **fStateTable;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_پشته[k_پشته_Size];  // State stack, holds state pushes
    int32_t                       f_پشته_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_گره_پشته[k_پشته_Size]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_گره_پشته_Ptr;


    UBool                          fReverseRule;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          fLookAheadRule;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          fNoChainInRule;   // True if the current rule starts with a '^'.

    RBBISymbolTable               *f_نماد_Table;     // symbol table, holds definitions of
                                                     //   $variable symbols.

    UHashtable                    *fSetTable;        // UnicocodeSet hash table, holds indexes to
                                                     //   the sets created while parsing rules.
                                                     //   The key is the string used for creating
                                                     //   the set.

    UnicodeSet                     fRuleSets[10];    // Unicode Sets that are needed during
                                                     //  the scanning of RBBI rules.  The
                                                     //  indices for these are assigned by the
                                                     //  perl script that builds the state tables.
                                                     //  See rbbirpt.h.

    int32_t                        f_Rule_رقم;         // Counts each rule as it is scanned.

    int32_t                        fOptionStart;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    UnicodeSet *gRuleSet_rule_char;
    UnicodeSet *gRuleSet_white_space;
    UnicodeSet *gRuleSet_name_char;
    UnicodeSet *gRuleSet_name_start_char;

    RBBIRuleScanner(ثابت RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &operator=(ثابت RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_END

#endif
