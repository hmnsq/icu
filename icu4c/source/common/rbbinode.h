// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#ifndef RBBINODE_H
#define RBBINODE_H

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    UnicodeSet;
class    UVector;

class RBBINode : જાહેર UMemory {
    જાહેર:
        ગણના ગાંઠ_Type {
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

        ગણના OpPrecedence {      
            precZero,
            precStart,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        ગાંઠ_Type      fType;
        RBBINode      *fParent;
        RBBINode      *f_ડાબું_Child;
        RBBINode      *f_જમણું_Child;
        UnicodeSet    *f_આગત_Set;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString fText;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        પૂર્ણાંક           fFirstPos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        પૂર્ણાંક           fLastPos;             //  Last position in the rule source string
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

        UBool         f_Rule_મૂળ;            // True if this node is the root of a rule.
        UBool         fChainIn;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *fFirstPosSet;
        UVector       *fLastPosSet;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(ગાંઠ_Type t, UErrorCode& status);
        RBBINode(અચલ RBBINode &other, UErrorCode& status);
        ~RBBINode();
        સ્થિર શૂન્ય  NRDelete_ગાંઠ(RBBINode *ગાંઠ);
        
        RBBINode    *clone_વૃક્ષ(UErrorCode &status, પૂર્ણાંક depth=0);
        RBBINode    *flattenVariables(UErrorCode &status, પૂર્ણાંક depth=0);
        શૂન્ય         flattenSets(UErrorCode &status, પૂર્ણાંક depth=0);
        શૂન્ય         findNodes(UVector *dest, RBBINode::ગાંઠ_Type kind, UErrorCode &status);

#ifdef RBBI_DEBUG
        સ્થિર શૂન્ય print_ગાંઠ_Header();
        સ્થિર શૂન્ય print_ગાંઠ(અચલ RBBINode *n);
        સ્થિર શૂન્ય print_વૃક્ષ(અચલ RBBINode *n, UBool withHeading);
#endif

    ખાનગી:
        RBBINode &operator = (અચલ RBBINode &other); // No defs.
        bool operator == (અચલ RBBINode &other);     // Private, so these functions won't accidentally be used.

#ifdef RBBI_DEBUG
    જાહેર:
        પૂર્ણાંક           f_Serial_અંક;           //  Debugging aids.
#endif
};

#ifdef RBBI_DEBUG
U_CFUNC શૂન્ય 
RBBI_DEBUG_printUnicodeString(અચલ UnicodeString &s, પૂર્ણાંક minWidth=0);
#endif

U_NAMESPACE_END

#endif

