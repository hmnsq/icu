// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#ifndef RBBINODE_H
#define RBBINODE_H

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    UnicodeSet;
class    UVector;

class RBBINode : പൊതു UMemory {
    പൊതു:
        എണ്ണൽ കണ്ണി_Type {
            setRef,
            uset,
            varRef,
            leafChar,
            lookAhead,
            tag,
            endMark,
            opStart,
            opCat,
            opOr,
            opStar,
            opPlus,
            opQuestion,
            opBreak,
            opReverse,
            opLParen
        };

        എണ്ണൽ OpPrecedence {      
            precZero,
            precStart,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        കണ്ണി_Type      fType;
        RBBINode      *fParent;
        RBBINode      *f_ഇടത്_Child;
        RBBINode      *f_വലത്_Child;
        UnicodeSet    *f_നിവേശം_Set;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString fText;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        പൂർണ്ണസംഖ്യ           fFirstPos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        പൂർണ്ണസംഖ്യ           fLastPos;             //  Last position in the rule source string
                                            //    of any text associated with this node.
                                            //    If there's a right child, this will be the same
                                            //    as that child's last position.

        UBool         fNullable;            // See Aho.
        int32_t       fVal;                 // For leafChar nodes, the value.
                                            //   Values are the character category,
                                            //   corresponds to columns in the final
                                            //   state transition table.

        UBool         fLookAheadEnd;        // For endMark nodes, set true if
                                            //   marking the end of a look-ahead rule.

        UBool         f_Rule_വേര്;            // True if this node is the root of a rule.
        UBool         fChainIn;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *fFirstPosSet;
        UVector       *fLastPosSet;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(കണ്ണി_Type t, UErrorCode& status);
        RBBINode(സ്ഥിരം RBBINode &other, UErrorCode& status);
        ~RBBINode();
        സ്ഥിതം ശൂന്യം  NRDelete_കണ്ണി(RBBINode *കണ്ണി);
        
        RBBINode    *clone_വൃക്ഷം(UErrorCode &status, പൂർണ്ണസംഖ്യ depth=0);
        RBBINode    *flattenVariables(UErrorCode &status, പൂർണ്ണസംഖ്യ depth=0);
        ശൂന്യം         flattenSets(UErrorCode &status, പൂർണ്ണസംഖ്യ depth=0);
        ശൂന്യം         findNodes(UVector *dest, RBBINode::കണ്ണി_Type kind, UErrorCode &status);

#ifdef RBBI_DEBUG
        സ്ഥിതം ശൂന്യം print_കണ്ണി_Header();
        സ്ഥിതം ശൂന്യം print_കണ്ണി(സ്ഥിരം RBBINode *n);
        സ്ഥിതം ശൂന്യം print_വൃക്ഷം(സ്ഥിരം RBBINode *n, UBool withHeading);
#endif

    സ്വകാര്യ:
        RBBINode &operator = (സ്ഥിരം RBBINode &other); // No defs.
        bool operator == (സ്ഥിരം RBBINode &other);     // Private, so these functions won't accidentally be used.

#ifdef RBBI_DEBUG
    പൊതു:
        പൂർണ്ണസംഖ്യ           f_Serial_അക്കം;           //  Debugging aids.
#endif
};

#ifdef RBBI_DEBUG
U_CFUNC ശൂന്യം 
RBBI_DEBUG_printUnicodeString(സ്ഥിരം UnicodeString &s, പൂർണ്ണസംഖ്യ minWidth=0);
#endif

U_NAMESPACE_END

#endif

