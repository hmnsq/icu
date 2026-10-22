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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/rbbi.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/parseerr.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                          //    looks up references to $variables within a set.
#ಅನ್ತರ್ಭಾವ "rbbinode.h"
#ಅನ್ತರ್ಭಾವ "rbbirpt.h"

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

class RBBIRuleScanner : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:

    ಗಣನಾ {
        k_ರಾಶಿ_Size = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    ಸಂರಚನಾ RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {}
    };

    RBBIRuleScanner(RBBIRuleBuilder  *rb);


    ಆಭಾಸ    ~RBBIRuleScanner();

    ಶೂನ್ಯ        nextChar(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       ತಳ್ಳು(ಧ್ರುವ RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    ಶೂನ್ಯ        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     ಅಂಕೆ_Rules();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    ಸ್ಥಿರ UnicodeString stripRules(ಧ್ರುವ UnicodeString &rules);
ಗುಪ್ತ:

    UBool       doParseActions(int32_t a);
    ಶೂನ್ಯ        error(UErrorCode e);                   // error reporting convenience function.
    ಶೂನ್ಯ        fix_Op_ರಾಶಿ(RBBINode::OpPrecedence p);
                                                       //   a character.
    ಶೂನ್ಯ        findSetFor(ಧ್ರುವ UnicodeString &s, RBBINode *ಗಂಟು, UnicodeSet *setToAdopt = nullptr);

    UChar32     nextCharLL();
#ifdef RBBI_DEBUG
    ಶೂನ್ಯ        print_ಗಂಟು_ರಾಶಿ(ಧ್ರುವ ವರ್ಣ *title);
#endif
    RBBINode    *ತಳ್ಳು_New_ಗಂಟು(RBBINode::ಗಂಟು_Type  t);
    ಶೂನ್ಯ        scanSet();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       fScanIndex;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       fNextIndex;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         fQuoteMode;        // Scan is in a 'quoted region'
    int32_t                       f_ಸಾಲು_ಅಂಕೆ;          // Line number in input file.
    int32_t                       f_Char_ಅಂಕೆ;          // Char position within the line.
    UChar32                       fLastChar;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRuleTableEl               **fStateTable;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_ರಾಶಿ[k_ರಾಶಿ_Size];  // State stack, holds state pushes
    int32_t                       f_ರಾಶಿ_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_ಗಂಟು_ರಾಶಿ[k_ರಾಶಿ_Size]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_ಗಂಟು_ರಾಶಿ_Ptr;


    UBool                          fReverseRule;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          fLookAheadRule;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          fNoChainInRule;   // True if the current rule starts with a '^'.

    RBBISymbolTable               *f_ಚಿಹ್ನೆ_Table;     // symbol table, holds definitions of
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

    int32_t                        f_Rule_ಅಂಕೆ;         // Counts each rule as it is scanned.

    int32_t                        fOptionStart;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    UnicodeSet *gRuleSet_rule_char;
    UnicodeSet *gRuleSet_white_space;
    UnicodeSet *gRuleSet_name_char;
    UnicodeSet *gRuleSet_name_start_char;

    RBBIRuleScanner(ಧ್ರುವ RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &operator=(ಧ್ರುವ RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_END

#endif
