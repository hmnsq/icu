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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/rbbi.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/parseerr.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "uvector.h"
#অন্তর্ভাব "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                          //    looks up references to $variables within a set.
#অন্তর্ভাব "rbbinode.h"
#অন্তর্ভাব "rbbirpt.h"

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

class RBBIRuleScanner : সর্বজনীন UMemory {
সর্বজনীন:

    গণনা {
        k_স্তূপ_আকার = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    কাঠামো RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {}
    };

    RBBIRuleScanner(RBBIRuleBuilder  *rb);


    আভাসী    ~RBBIRuleScanner();

    শূন্য        পরবর্তী_Char(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       ঠেলো(ধ্রুবক RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    শূন্য        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     অঙ্ক_Rules();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    স্থির UnicodeString stripRules(ধ্রুবক UnicodeString &rules);
ব্যক্তিগত:

    UBool       doParseActions(int32_t a);
    শূন্য        error(UErrorCode e);                   // error reporting convenience function.
    শূন্য        fix_Op_স্তূপ(RBBINode::OpPrecedence p);
                                                       //   a character.
    শূন্য        findSetFor(ধ্রুবক UnicodeString &s, RBBINode *গ্রন্থি, UnicodeSet *setToAdopt = nullptr);

    UChar32     পরবর্তী_Char_LL();
#ifdef RBBI_DEBUG
    শূন্য        print_গ্রন্থি_স্তূপ(ধ্রুবক অক্ষর *title);
#endif
    RBBINode    *ঠেলো_New_গ্রন্থি(RBBINode::গ্রন্থি_Type  t);
    শূন্য        scanSet();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       f_Scan_সূচক;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       f_পরবর্তী_সূচক;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         fQuoteMode;        // Scan is in a 'quoted region'
    int32_t                       f_সারি_অঙ্ক;          // Line number in input file.
    int32_t                       f_Char_অঙ্ক;          // Char position within the line.
    UChar32                       fLastChar;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRuleTableEl               **fStateTable;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_স্তূপ[k_স্তূপ_আকার];  // State stack, holds state pushes
    int32_t                       f_স্তূপ_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_গ্রন্থি_স্তূপ[k_স্তূপ_আকার]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_গ্রন্থি_স্তূপ_Ptr;


    UBool                          fReverseRule;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          fLookAheadRule;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          fNoChainInRule;   // True if the current rule starts with a '^'.

    RBBISymbolTable               *f_প্রতীক_Table;     // symbol table, holds definitions of
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

    int32_t                        f_Rule_অঙ্ক;         // Counts each rule as it is scanned.

    int32_t                        fOptionStart;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    UnicodeSet *gRuleSet_rule_char;
    UnicodeSet *gRuleSet_white_space;
    UnicodeSet *gRuleSet_name_char;
    UnicodeSet *gRuleSet_name_start_char;

    RBBIRuleScanner(ধ্রুবক RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &operator=(ধ্রুবক RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_END

#endif
