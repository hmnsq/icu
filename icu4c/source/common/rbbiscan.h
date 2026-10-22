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


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RBBISCAN_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RBBISCAN_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/rbbi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parseerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                          //    looks up references to $variables within a set.
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbinode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rbbirpt.h"

U_NAMESPACE_BEGIN

class   RBBIRuleBuilder;
class   RBBISymbol_ਸਾਰਣੀ;


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

class RBBIRuleScanner : ਜਨਤਕ UMemory {
ਜਨਤਕ:

    ਗਿਣਤੀ {
        k_ਢੇਰ_ਮਾਪ = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    ਬਣਤਰ RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {}
    };

    RBBIRuleScanner(RBBIRuleBuilder  *rb);


    ਆਭਾਸੀ    ~RBBIRuleScanner();

    ਖਾਲੀ        ਅਗਲਾ_Char(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       ਧੱਕੋ(ਅਚਲ RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    ਖਾਲੀ        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     ਅੰਕ_ਨਿਯਮਾਂ();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    ਸਥਿਰ UnicodeString strip_ਨਿਯਮਾਂ(ਅਚਲ UnicodeString &ਨਿਯਮਾਂ);
ਨਿਜੀ:

    UBool       doParseActions(int32_t a);
    ਖਾਲੀ        error(UError_ਕੋਡ e);                   // error reporting convenience function.
    ਖਾਲੀ        fix_Op_ਢੇਰ(RBBINode::OpPrecedence p);
                                                       //   a character.
    ਖਾਲੀ        ਲੱਭੋ_ਸੈੱਟ_For(ਅਚਲ UnicodeString &s, RBBINode *ਗੰਢ, Unicode_ਸੈੱਟ *ਸੈੱਟ_ਮੰਜ਼ਿਲ_Adopt = nullptr);

    UChar32     ਅਗਲਾ_Char_LL();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ RBBI_DEBUG
    ਖਾਲੀ        print_ਗੰਢ_ਢੇਰ(ਅਚਲ ਅੱਖਰ *title);
#ਜੇ_ਅੰਤ
    RBBINode    *ਧੱਕੋ_New_ਗੰਢ(RBBINode::ਗੰਢ_Type  t);
    ਖਾਲੀ        ਜਾਂਚ_ਸੈੱਟ();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       f_ਜਾਂਚ_ਸੂਚਕਾਂਕ;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       f_ਅਗਲਾ_ਸੂਚਕਾਂਕ;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         f_Quote_ਬਹੁਲਕ;        // Scan is in a 'quoted region'
    int32_t                       f_ਸਤਰ_ਅੰਕ;          // Line number in input file.
    int32_t                       f_Char_ਅੰਕ;          // Char position within the line.
    UChar32                       f_ਆਖਰੀ_Char;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRule_ਸਾਰਣੀ_El               **f_State_ਸਾਰਣੀ;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_ਢੇਰ[k_ਢੇਰ_ਮਾਪ];  // State stack, holds state pushes
    int32_t                       f_ਢੇਰ_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_ਗੰਢ_ਢੇਰ[k_ਢੇਰ_ਮਾਪ]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_ਗੰਢ_ਢੇਰ_Ptr;


    UBool                          f_ਉਲਟ_ਨਿਯਮ;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          f_Look_Ahead_ਨਿਯਮ;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          f_No_Chain_ਵਿੱਚ_ਨਿਯਮ;   // True if the current rule starts with a '^'.

    RBBISymbol_ਸਾਰਣੀ               *f_ਚਿੰਨ੍ਹ_ਸਾਰਣੀ;     // symbol table, holds definitions of
                                                     //   $variable symbols.

    UHashtable                    *f_ਸੈੱਟ_ਸਾਰਣੀ;        // UnicocodeSet hash table, holds indexes to
                                                     //   the sets created while parsing rules.
                                                     //   The key is the string used for creating
                                                     //   the set.

    Unicode_ਸੈੱਟ                     f_ਨਿਯਮ_Sets[10];    // Unicode Sets that are needed during
                                                     //  the scanning of RBBI rules.  The
                                                     //  indices for these are assigned by the
                                                     //  perl script that builds the state tables.
                                                     //  See rbbirpt.h.

    int32_t                        f_ਨਿਯਮ_ਅੰਕ;         // Counts each rule as it is scanned.

    int32_t                        f_Option_ਸ਼ੁਰੂ;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    Unicode_ਸੈੱਟ *g_ਨਿਯਮ_ਸੈੱਟ_ਨਿਯਮ_char;
    Unicode_ਸੈੱਟ *g_ਨਿਯਮ_ਸੈੱਟ_white_space;
    Unicode_ਸੈੱਟ *g_ਨਿਯਮ_ਸੈੱਟ_name_char;
    Unicode_ਸੈੱਟ *g_ਨਿਯਮ_ਸੈੱਟ_name_ਸ਼ੁਰੂ_char;

    RBBIRuleScanner(ਅਚਲ RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &ਸੰਚਾਲਕ=(ਅਚਲ RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
