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


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RBBISCAN_H
#ವ್ಯಾಖ್ಯಾನಿಸು RBBISCAN_H

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
class   RBBISymbol_ಕೋಷ್ಟಕ;


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

    ಎಣಿಕೆ {
        k_ರಾಶಿ_ಗಾತ್ರ = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    ರಚನೆ RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {}
    };

    RBBIRuleScanner(RBBIRuleBuilder  *rb);


    ವಾಸ್ತವಿಕ    ~RBBIRuleScanner();

    ಶೂನ್ಯ        ಮುಂದಿನ_Char(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       ತಳ್ಳು(ಸ್ಥಿರಾಂಕ RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    ಶೂನ್ಯ        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     ಅಂಕೆ_ನಿಯಮಗಳು();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    ಸ್ಥಿರ UnicodeString strip_ನಿಯಮಗಳು(ಸ್ಥಿರಾಂಕ UnicodeString &ನಿಯಮಗಳು);
ಖಾಸಗಿ:

    UBool       doParseActions(int32_t a);
    ಶೂನ್ಯ        error(UError_ಸಂಕೇತ e);                   // error reporting convenience function.
    ಶೂನ್ಯ        fix_Op_ರಾಶಿ(RBBINode::OpPrecedence p);
                                                       //   a character.
    ಶೂನ್ಯ        ಕಂಡುಹಿಡಿ_ಗಣ_For(ಸ್ಥಿರಾಂಕ UnicodeString &s, RBBINode *ಗಂಟು, Unicode_ಗಣ *ಗಣ_ಗಮ್ಯ_Adopt = nullptr);

    UChar32     ಮುಂದಿನ_Char_LL();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    ಶೂನ್ಯ        print_ಗಂಟು_ರಾಶಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *title);
#ಆದರೆ_ಅಂತ್ಯ
    RBBINode    *ತಳ್ಳು_New_ಗಂಟು(RBBINode::ಗಂಟು_Type  t);
    ಶೂನ್ಯ        ಪರಿಶೀಲನೆ_ಗಣ();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       f_ಪರಿಶೀಲನೆ_ಸೂಚ್ಯಂಕ;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       f_ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         f_Quote_ಬಹುಲಕ;        // Scan is in a 'quoted region'
    int32_t                       f_ಸಾಲು_ಅಂಕೆ;          // Line number in input file.
    int32_t                       f_Char_ಅಂಕೆ;          // Char position within the line.
    UChar32                       f_ಕೊನೆಯ_Char;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRule_ಕೋಷ್ಟಕ_El               **f_State_ಕೋಷ್ಟಕ;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_ರಾಶಿ[k_ರಾಶಿ_ಗಾತ್ರ];  // State stack, holds state pushes
    int32_t                       f_ರಾಶಿ_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_ಗಂಟು_ರಾಶಿ[k_ರಾಶಿ_ಗಾತ್ರ]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_ಗಂಟು_ರಾಶಿ_Ptr;


    UBool                          f_ಹಿಮ್ಮುಖ_ನಿಯಮ;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          f_Look_Ahead_ನಿಯಮ;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          f_No_Chain_ಒಳ_ನಿಯಮ;   // True if the current rule starts with a '^'.

    RBBISymbol_ಕೋಷ್ಟಕ               *f_ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ;     // symbol table, holds definitions of
                                                     //   $variable symbols.

    UHashtable                    *f_ಗಣ_ಕೋಷ್ಟಕ;        // UnicocodeSet hash table, holds indexes to
                                                     //   the sets created while parsing rules.
                                                     //   The key is the string used for creating
                                                     //   the set.

    Unicode_ಗಣ                     f_ನಿಯಮ_Sets[10];    // Unicode Sets that are needed during
                                                     //  the scanning of RBBI rules.  The
                                                     //  indices for these are assigned by the
                                                     //  perl script that builds the state tables.
                                                     //  See rbbirpt.h.

    int32_t                        f_ನಿಯಮ_ಅಂಕೆ;         // Counts each rule as it is scanned.

    int32_t                        f_Option_ಆರಂಭ;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    Unicode_ಗಣ *g_ನಿಯಮ_ಗಣ_ನಿಯಮ_char;
    Unicode_ಗಣ *g_ನಿಯಮ_ಗಣ_white_space;
    Unicode_ಗಣ *g_ನಿಯಮ_ಗಣ_name_char;
    Unicode_ಗಣ *g_ನಿಯಮ_ಗಣ_name_ಆರಂಭ_char;

    RBBIRuleScanner(ಸ್ಥಿರಾಂಕ RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
