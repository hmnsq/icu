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
        تعداد كلي_Type {
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
            
        كلي_Type      fType;
        RBBINode      *fParent;
        RBBINode      *f_هغو_Child;
        RBBINode      *f_دام_Child;
        UnicodeSet    *f_شغرو_Set;           // For uset nodes only.
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

        UBool         f_Rule_سيو;            // True if this node is the root of a rule.
        UBool         fChainIn;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *fFirstPosSet;
        UVector       *fLastPosSet;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(كلي_Type t, UErrorCode& status);
        RBBINode(ثابت RBBINode &other, UErrorCode& status);
        ~RBBINode();
        ساكن فراغ  NRDelete_كلي(RBBINode *كلي);
        
        RBBINode    *clone_بشيا(UErrorCode &status, صحيح depth=0);
        RBBINode    *flattenVariables(UErrorCode &status, صحيح depth=0);
        فراغ         flattenSets(UErrorCode &status, صحيح depth=0);
        فراغ         findNodes(UVector *dest, RBBINode::كلي_Type kind, UErrorCode &status);

#ifdef RBBI_DEBUG
        ساكن فراغ بغ_كلي_Header();
        ساكن فراغ بغ_كلي(ثابت RBBINode *n);
        ساكن فراغ بغ_بشيا(ثابت RBBINode *n, UBool withHeading);
#endif

    خاص:
        RBBINode &operator = (ثابت RBBINode &other); // No defs.
        bool operator == (ثابت RBBINode &other);     // Private, so these functions won't accidentally be used.

#ifdef RBBI_DEBUG
    عام:
        صحيح           f_Serial_لمب;           //  Debugging aids.
#endif
};

#ifdef RBBI_DEBUG
U_CFUNC فراغ 
RBBI_DEBUG_بغ_Unicode_String(ثابت UnicodeString &s, صحيح minWidth=0);
#endif

U_NAMESPACE_END

#endif

