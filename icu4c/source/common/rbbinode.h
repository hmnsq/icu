// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#ifndef RBBINODE_H
#define RBBINODE_H

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    UnicodeSet;
class    UVector;

class RBBINode : பொது UMemory {
    பொது:
        எண்ணிக்கை முனை_Type {
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

        எண்ணிக்கை OpPrecedence {      
            precZero,
            precStart,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        முனை_Type      fType;
        RBBINode      *fParent;
        RBBINode      *f_இடது_Child;
        RBBINode      *f_வலது_Child;
        UnicodeSet    *f_உள்ளீடு_Set;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString fText;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        முழு           fFirstPos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        முழு           fLastPos;             //  Last position in the rule source string
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

        UBool         f_Rule_வேர்;            // True if this node is the root of a rule.
        UBool         fChainIn;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *fFirstPosSet;
        UVector       *fLastPosSet;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(முனை_Type t, UErrorCode& status);
        RBBINode(மாறிலி RBBINode &other, UErrorCode& status);
        ~RBBINode();
        நிலையான வெறுமை  NRDelete_முனை(RBBINode *முனை);
        
        RBBINode    *clone_மரம்(UErrorCode &status, முழு depth=0);
        RBBINode    *flattenVariables(UErrorCode &status, முழு depth=0);
        வெறுமை         flattenSets(UErrorCode &status, முழு depth=0);
        வெறுமை         findNodes(UVector *dest, RBBINode::முனை_Type kind, UErrorCode &status);

#ifdef RBBI_DEBUG
        நிலையான வெறுமை print_முனை_Header();
        நிலையான வெறுமை print_முனை(மாறிலி RBBINode *n);
        நிலையான வெறுமை print_மரம்(மாறிலி RBBINode *n, UBool withHeading);
#endif

    தனிப்பட்ட:
        RBBINode &operator = (மாறிலி RBBINode &other); // No defs.
        bool operator == (மாறிலி RBBINode &other);     // Private, so these functions won't accidentally be used.

#ifdef RBBI_DEBUG
    பொது:
        முழு           f_Serial_இலக்கம்;           //  Debugging aids.
#endif
};

#ifdef RBBI_DEBUG
U_CFUNC வெறுமை 
RBBI_DEBUG_printUnicodeString(மாறிலி UnicodeString &s, முழு minWidth=0);
#endif

U_NAMESPACE_END

#endif

