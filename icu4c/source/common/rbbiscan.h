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


#నిర్వచితం_కాకపోతే RBBISCAN_H
#నిర్వచించు RBBISCAN_H

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/rbbi.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/parseerr.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "uvector.h"
#అన్తర్భావ "unicode/symtable.h"// For UnicodeSet parsing, is the interface that
                          //    looks up references to $variables within a set.
#అన్తర్భావ "rbbinode.h"
#అన్తర్భావ "rbbirpt.h"

U_NAMESPACE_BEGIN

class   RBBIRuleBuilder;
class   RBBISymbol_పట్టిక;


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

class RBBIRuleScanner : బహిరంగ UMemory {
బహిరంగ:

    గణన {
        k_దొంతర_కొలత = 100            // The size of the state stack for
    };                              //   rules parsing.  Corresponds roughly
                                    //   to the depth of parentheses nesting
                                    //   that is allowed in the rules.

    నిర్మాణం RBBIRuleChar {
        UChar32             fChar;
        UBool               fEscaped;
        RBBIRuleChar() : fChar(0), fEscaped(false) {}
    };

    RBBIRuleScanner(RBBIRuleBuilder  *rb);


    మాయా    ~RBBIRuleScanner();

    శూన్య        తదుపరి_Char(RBBIRuleChar &c);          // Get the next char from the input stream.
                                                    // Return false if at end.

    UBool       నెట్టు(స్థిరరాశి RBBIRuleChar &c);        // Push (unget) one character.
                                                    //   Only a single character may be pushed.

    శూన్య        parse();                            // Parse the rules, generating two parse
                                                    //   trees, one each for the forward and
                                                    //   reverse rules,
                                                    //   and a list of UnicodeSets encountered.

    int32_t     అంకె_నియమాలు();                         // Return the number of rules that have been seen.

    /**
     * Return a rules string without unnecessary
     * characters.
     */
    స్థిర UnicodeString strip_నియమాలు(స్థిరరాశి UnicodeString &నియమాలు);
అంతరంగ:

    UBool       doParseActions(int32_t a);
    శూన్య        error(UError_సంకేతం e);                   // error reporting convenience function.
    శూన్య        fix_Op_దొంతర(RBBINode::OpPrecedence p);
                                                       //   a character.
    శూన్య        కనుగొను_సమితి_For(స్థిరరాశి UnicodeString &s, RBBINode *కణుపు, Unicode_సమితి *సమితి_గమ్యం_Adopt = nullptr);

    UChar32     తదుపరి_Char_LL();
#నిర్వచితమైతే RBBI_DEBUG
    శూన్య        print_కణుపు_దొంతర(స్థిరరాశి అక్షరం *title);
#అయితే_ముగింపు
    RBBINode    *నెట్టు_New_కణుపు(RBBINode::కణుపు_Type  t);
    శూన్య        పరిశీలన_సమితి();


    RBBIRuleBuilder               *fRB;              // The rule builder that we are part of.

    int32_t                       f_పరిశీలన_సూచీ;        // Index of current character being processed
                                                     //   in the rule input string.
    int32_t                       f_తదుపరి_సూచీ;        // Index of the next character, which
                                                     //   is the first character not yet scanned.
    UBool                         f_Quote_బహుళకం;        // Scan is in a 'quoted region'
    int32_t                       f_పంక్తి_అంకె;          // Line number in input file.
    int32_t                       f_Char_అంకె;          // Char position within the line.
    UChar32                       f_చివరి_Char;         // Previous char, needed to count CR-LF
                                                     //   as a single line, not two.

    RBBIRuleChar                  fC;                // Current char for parse state machine
                                                     //   processing.
    UnicodeString                 fVarName;          // $variableName, valid when we've just
                                                     //   scanned one.

    RBBIRule_పట్టిక_El               **f_State_పట్టిక;     // State Transition Table for RBBI Rule
                                                     //   parsing.  index by p[state][char-class]

    uint16_t                      f_దొంతర[k_దొంతర_కొలత];  // State stack, holds state pushes
    int32_t                       f_దొంతర_Ptr;           //  and pops as specified in the state
                                                       //  transition rules.

    RBBINode                      *f_కణుపు_దొంతర[k_దొంతర_కొలత]; // Node stack, holds nodes created
                                                           //  during the parse of a rule
    int32_t                        f_కణుపు_దొంతర_Ptr;


    UBool                          f_తిరగేయి_నియమం;     // True if the rule currently being scanned
                                                     //  is a reverse direction rule (if it
                                                     //  starts with a '!')

    UBool                          f_Look_Ahead_నియమం;   // True if the rule includes a '/'
                                                     //   somewhere within it.

    UBool                          f_No_Chain_లోపల_నియమం;   // True if the current rule starts with a '^'.

    RBBISymbol_పట్టిక               *f_చిహ్నం_పట్టిక;     // symbol table, holds definitions of
                                                     //   $variable symbols.

    UHashtable                    *f_సమితి_పట్టిక;        // UnicocodeSet hash table, holds indexes to
                                                     //   the sets created while parsing rules.
                                                     //   The key is the string used for creating
                                                     //   the set.

    Unicode_సమితి                     f_నియమం_Sets[10];    // Unicode Sets that are needed during
                                                     //  the scanning of RBBI rules.  The
                                                     //  indices for these are assigned by the
                                                     //  perl script that builds the state tables.
                                                     //  See rbbirpt.h.

    int32_t                        f_నియమం_అంకె;         // Counts each rule as it is scanned.

    int32_t                        f_Option_ప్రారంభం;     // Input index of start of a !!option
                                                     //   keyword, while being scanned.

    Unicode_సమితి *g_నియమం_సమితి_నియమం_char;
    Unicode_సమితి *g_నియమం_సమితి_white_space;
    Unicode_సమితి *g_నియమం_సమితి_name_char;
    Unicode_సమితి *g_నియమం_సమితి_name_ప్రారంభం_char;

    RBBIRuleScanner(స్థిరరాశి RBBIRuleScanner &other) = delete; // forbid copying of this class
    RBBIRuleScanner &క్రియాకారకం=(స్థిరరాశి RBBIRuleScanner &other) = delete; // forbid copying of this class
};

U_NAMESPACE_చివర

#అయితే_ముగింపు
