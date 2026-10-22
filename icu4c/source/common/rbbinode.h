// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#ifndef RBBINODE_H
#define RBBINODE_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    UnicodeSet;
class    UVector;

class RBBINode : عام UMemory {
    عام:
        تعداد نود_Type {
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

        تعداد OpPrecedence {      
            precZero,
            precStart,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        نود_Type      fType;
        RBBINode      *fParent;
        RBBINode      *f_کيري_Child;
        RBBINode      *f_کانن_Child;
        UnicodeSet    *f_ماسوقن_Set;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString fText;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        صحيح           fFirstPos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        صحيح           fLastPos;             //  Last position in the rule source string
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

        UBool         f_Rule_اکر;            // True if this node is the root of a rule.
        UBool         fChainIn;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *fFirstPosSet;
        UVector       *fLastPosSet;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(نود_Type t, UErrorCode& status);
        RBBINode(ثابت RBBINode &other, UErrorCode& status);
        ~RBBINode();
        ساكن فراغ  NRDelete_نود(RBBINode *نود);
        
        RBBINode    *clone_ڤوکوق(UErrorCode &status, صحيح depth=0);
        RBBINode    *flattenVariables(UErrorCode &status, صحيح depth=0);
        فراغ         flattenSets(UErrorCode &status, صحيح depth=0);
        فراغ         findNodes(UVector *dest, RBBINode::نود_Type kind, UErrorCode &status);

#ifdef RBBI_DEBUG
        ساكن فراغ print_نود_Header();
        ساكن فراغ print_نود(ثابت RBBINode *n);
        ساكن فراغ print_ڤوکوق(ثابت RBBINode *n, UBool withHeading);
#endif

    خاص:
        RBBINode &operator = (ثابت RBBINode &other); // No defs.
        bool operator == (ثابت RBBINode &other);     // Private, so these functions won't accidentally be used.

#ifdef RBBI_DEBUG
    عام:
        صحيح           f_Serial_اڠک;           //  Debugging aids.
#endif
};

#ifdef RBBI_DEBUG
U_CFUNC فراغ 
RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحيح minWidth=0);
#endif

U_NAMESPACE_END

#endif

