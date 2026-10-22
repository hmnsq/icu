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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "unicode/rbbi.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "unicode/parseerr.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "uvector.h"
#அந்தர்பாவ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                          //    looks up references to $variables within a set.
#அந்தர்பாவ "rbbinode.h"
#அந்தர்பாவ "rbbirpt.h"

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

class RBBIRuleScanner : பொது UMemory {
பொது:

    எண்ணிக்கை {
        k_அடுக்கு_Size = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    கட்டமைப்பு RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {}
    };

    RBBIRuleScanner(RBBIRuleBuilder  *rb);


    மெய்நிகர்    ~RBBIRuleScanner();

    வெறுமை        nextChar(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       தள்ளு(மாறிலி RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    வெறுமை        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     இலக்கம்_Rules();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    நிலையான UnicodeString stripRules(மாறிலி UnicodeString &rules);
தனிப்பட்ட:

    UBool       doParseActions(int32_t a);
    வெறுமை        error(UErrorCode e);                   // error reporting convenience function.
    வெறுமை        fix_Op_அடுக்கு(RBBINode::OpPrecedence p);
                                                       //   a character.
    வெறுமை        findSetFor(மாறிலி UnicodeString &s, RBBINode *முனை, UnicodeSet *setToAdopt = nullptr);

    UChar32     nextCharLL();
#ifdef RBBI_DEBUG
    வெறுமை        print_முனை_அடுக்கு(மாறிலி எழுத்து *title);
#endif
    RBBINode    *தள்ளு_New_முனை(RBBINode::முனை_Type  t);
    வெறுமை        scanSet();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       fScanIndex;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       fNextIndex;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         fQuoteMode;        // Scan is in a 'quoted region'
    int32_t                       f_வரி_இலக்கம்;          // Line number in input file.
    int32_t                       f_Char_இலக்கம்;          // Char position within the line.
    UChar32                       fLastChar;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRuleTableEl               **fStateTable;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_அடுக்கு[k_அடுக்கு_Size];  // State stack, holds state pushes
    int32_t                       f_அடுக்கு_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_முனை_அடுக்கு[k_அடுக்கு_Size]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_முனை_அடுக்கு_Ptr;


    UBool                          fReverseRule;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          fLookAheadRule;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          fNoChainInRule;   // True if the current rule starts with a '^'.

    RBBISymbolTable               *f_குறியீடு_Table;     // symbol table, holds definitions of
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

    int32_t                        f_Rule_இலக்கம்;         // Counts each rule as it is scanned.

    int32_t                        fOptionStart;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    UnicodeSet *gRuleSet_rule_char;
    UnicodeSet *gRuleSet_white_space;
    UnicodeSet *gRuleSet_name_char;
    UnicodeSet *gRuleSet_name_start_char;

    RBBIRuleScanner(மாறிலி RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &operator=(மாறிலி RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_END

#endif
