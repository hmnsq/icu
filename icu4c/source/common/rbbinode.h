// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#ifndef RBBINODE_H
#define RBBINODE_H

#شامل "unicode/utypes.h"
#شامل "unicode/unistr.h"
#شامل "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    UnicodeSet;
class    UVector;

class RBBINode : عمومی UMemory {
    عمومی:
        شمارش گره_Type {
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

        شمارش OpPrecedence {      
            precZero,
            precStart,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        گره_Type      fType;
        RBBINode      *fParent;
        RBBINode      *f_چپ_Child;
        RBBINode      *f_راست_Child;
        UnicodeSet    *f_درون_داد_Set;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString fText;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        صحیح           fFirstPos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        صحیح           fLastPos;             //  Last position in the rule source string
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

        UBool         f_Rule_ریشه;            // True if this node is the root of a rule.
        UBool         fChainIn;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *fFirstPosSet;
        UVector       *fLastPosSet;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(گره_Type t, UErrorCode& status);
        RBBINode(ثابت RBBINode &other, UErrorCode& status);
        ~RBBINode();
        ایستا تهی  NRDelete_گره(RBBINode *گره);
        
        RBBINode    *clone_درخت(UErrorCode &status, صحیح depth=0);
        RBBINode    *flattenVariables(UErrorCode &status, صحیح depth=0);
        تهی         flattenSets(UErrorCode &status, صحیح depth=0);
        تهی         findNodes(UVector *dest, RBBINode::گره_Type kind, UErrorCode &status);

#ifdef RBBI_DEBUG
        ایستا تهی print_گره_Header();
        ایستا تهی print_گره(ثابت RBBINode *n);
        ایستا تهی print_درخت(ثابت RBBINode *n, UBool withHeading);
#endif

    خصوصی:
        RBBINode &operator = (ثابت RBBINode &other); // No defs.
        bool operator == (ثابت RBBINode &other);     // Private, so these functions won't accidentally be used.

#ifdef RBBI_DEBUG
    عمومی:
        صحیح           f_Serial_رقم;           //  Debugging aids.
#endif
};

#ifdef RBBI_DEBUG
U_CFUNC تهی 
RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحیح minWidth=0);
#endif

U_NAMESPACE_END

#endif

