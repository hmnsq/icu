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


#अगर_अघोषित RBBISCAN_H
#घोषणा RBBISCAN_H

#समावेश "unicode/utypes.h"
#समावेश "unicode/uobject.h"
#समावेश "unicode/rbbi.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/parseerr.h"
#समावेश "uhash.h"
#समावेश "uvector.h"
#समावेश "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                          //    looks up references to $variables within a set.
#समावेश "rbbinode.h"
#समावेश "rbbirpt.h"

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

class RBBIRuleScanner : खुला UMemory {
खुला:

    क्रमागत {
        k_ढेर_आकार = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    काष्ठा RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {}
    };

    RBBIRuleScanner(RBBIRuleBuilder  *rb);


    भव    ~RBBIRuleScanner();

    व्योम        अग्र_Char(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       धकेलो(स्थिर RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    व्योम        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     अंक_Rules();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    अटल UnicodeString stripRules(स्थिर UnicodeString &rules);
निजी:

    UBool       doParseActions(int32_t a);
    व्योम        error(UErrorCode e);                   // error reporting convenience function.
    व्योम        fix_Op_ढेर(RBBINode::OpPrecedence p);
                                                       //   a character.
    व्योम        findSetFor(स्थिर UnicodeString &s, RBBINode *गाँठ, UnicodeSet *setToAdopt = nullptr);

    UChar32     अग्र_Char_LL();
#अगर_घोषित RBBI_DEBUG
    व्योम        print_गाँठ_ढेर(स्थिर अक्षर *title);
#पूर्ण_अगर
    RBBINode    *धकेलो_New_गाँठ(RBBINode::गाँठ_Type  t);
    व्योम        scanSet();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       f_Scan_सूचकांक;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       f_अग्र_सूचकांक;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         fQuoteMode;        // Scan is in a 'quoted region'
    int32_t                       f_पंक्ति_अंक;          // Line number in input file.
    int32_t                       f_Char_अंक;          // Char position within the line.
    UChar32                       fLastChar;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRuleTableEl               **fStateTable;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_ढेर[k_ढेर_आकार];  // State stack, holds state pushes
    int32_t                       f_ढेर_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_गाँठ_ढेर[k_ढेर_आकार]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_गाँठ_ढेर_Ptr;


    UBool                          fReverseRule;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          fLookAheadRule;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          fNoChainInRule;   // True if the current rule starts with a '^'.

    RBBISymbolTable               *f_प्रतीक_Table;     // symbol table, holds definitions of
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

    int32_t                        f_Rule_अंक;         // Counts each rule as it is scanned.

    int32_t                        fOptionStart;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    UnicodeSet *gRuleSet_rule_char;
    UnicodeSet *gRuleSet_white_space;
    UnicodeSet *gRuleSet_name_char;
    UnicodeSet *gRuleSet_name_start_char;

    RBBIRuleScanner(स्थिर RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &चालक=(स्थिर RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_END

#पूर्ण_अगर
