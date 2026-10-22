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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/rbbi.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/parseerr.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "uvector.h"
#അന്തര്ഭാവ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                          //    looks up references to $variables within a set.
#അന്തര്ഭാവ "rbbinode.h"
#അന്തര്ഭാവ "rbbirpt.h"

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

class RBBIRuleScanner : പൊതു UMemory {
പൊതു:

    എണ്ണൽ {
        k_അട്ടി_Size = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    ഘടന RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {}
    };

    RBBIRuleScanner(RBBIRuleBuilder  *rb);


    മായാ    ~RBBIRuleScanner();

    ശൂന്യം        nextChar(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       തള്ളുക(സ്ഥിരം RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    ശൂന്യം        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     അക്കം_Rules();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    സ്ഥിതം UnicodeString stripRules(സ്ഥിരം UnicodeString &rules);
സ്വകാര്യ:

    UBool       doParseActions(int32_t a);
    ശൂന്യം        error(UErrorCode e);                   // error reporting convenience function.
    ശൂന്യം        fix_Op_അട്ടി(RBBINode::OpPrecedence p);
                                                       //   a character.
    ശൂന്യം        findSetFor(സ്ഥിരം UnicodeString &s, RBBINode *കണ്ണി, UnicodeSet *setToAdopt = nullptr);

    UChar32     nextCharLL();
#ifdef RBBI_DEBUG
    ശൂന്യം        print_കണ്ണി_അട്ടി(സ്ഥിരം അക്ഷരം *title);
#endif
    RBBINode    *തള്ളുക_New_കണ്ണി(RBBINode::കണ്ണി_Type  t);
    ശൂന്യം        scanSet();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       fScanIndex;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       fNextIndex;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         fQuoteMode;        // Scan is in a 'quoted region'
    int32_t                       f_വരി_അക്കം;          // Line number in input file.
    int32_t                       f_Char_അക്കം;          // Char position within the line.
    UChar32                       fLastChar;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRuleTableEl               **fStateTable;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_അട്ടി[k_അട്ടി_Size];  // State stack, holds state pushes
    int32_t                       f_അട്ടി_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_കണ്ണി_അട്ടി[k_അട്ടി_Size]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_കണ്ണി_അട്ടി_Ptr;


    UBool                          fReverseRule;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          fLookAheadRule;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          fNoChainInRule;   // True if the current rule starts with a '^'.

    RBBISymbolTable               *f_ചിഹ്നം_Table;     // symbol table, holds definitions of
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

    int32_t                        f_Rule_അക്കം;         // Counts each rule as it is scanned.

    int32_t                        fOptionStart;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    UnicodeSet *gRuleSet_rule_char;
    UnicodeSet *gRuleSet_white_space;
    UnicodeSet *gRuleSet_name_char;
    UnicodeSet *gRuleSet_name_start_char;

    RBBIRuleScanner(സ്ഥിരം RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &operator=(സ്ഥിരം RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_END

#endif
