// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#ifndef RBBINODE_H
#define RBBINODE_H

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    UnicodeSet;
class    UVector;

class RBBINode : সার্বজনিক UMemory {
    সার্বজনিক:
        গণনা গ্রন্থি_Type {
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

        গণনা OpPrecedence {      
            precZero,
            precStart,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        গ্রন্থি_Type      fType;
        RBBINode      *fParent;
        RBBINode      *f_বাম_Child;
        RBBINode      *f_ডান_Child;
        UnicodeSet    *f_প্রবেশ_Set;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString fText;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        পূর্ণাঙ্ক           fFirstPos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        পূর্ণাঙ্ক           fLastPos;             //  Last position in the rule source string
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

        UBool         f_Rule_মূল;            // True if this node is the root of a rule.
        UBool         fChainIn;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *fFirstPosSet;
        UVector       *fLastPosSet;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(গ্রন্থি_Type t, UErrorCode& status);
        RBBINode(ধ্রুব RBBINode &other, UErrorCode& status);
        ~RBBINode();
        স্থির শূন্য  NRDelete_গ্রন্থি(RBBINode *গ্রন্থি);
        
        RBBINode    *clone_বৃক্ষ(UErrorCode &status, পূর্ণাঙ্ক depth=0);
        RBBINode    *flattenVariables(UErrorCode &status, পূর্ণাঙ্ক depth=0);
        শূন্য         flattenSets(UErrorCode &status, পূর্ণাঙ্ক depth=0);
        শূন্য         findNodes(UVector *dest, RBBINode::গ্রন্থি_Type kind, UErrorCode &status);

#ifdef RBBI_DEBUG
        স্থির শূন্য print_গ্রন্থি_Header();
        স্থির শূন্য print_গ্রন্থি(ধ্রুব RBBINode *n);
        স্থির শূন্য print_বৃক্ষ(ধ্রুব RBBINode *n, UBool withHeading);
#endif

    গুপ্ত:
        RBBINode &operator = (ধ্রুব RBBINode &other); // No defs.
        bool operator == (ধ্রুব RBBINode &other);     // Private, so these functions won't accidentally be used.

#ifdef RBBI_DEBUG
    সার্বজনিক:
        পূর্ণাঙ্ক           f_Serial_অঙ্ক;           //  Debugging aids.
#endif
};

#ifdef RBBI_DEBUG
U_CFUNC শূন্য 
RBBI_DEBUG_printUnicodeString(ধ্রুব UnicodeString &s, পূর্ণাঙ্ক minWidth=0);
#endif

U_NAMESPACE_END

#endif

