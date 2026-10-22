// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#अगर_अघोषित RBBINODE_H
#घोषणा RBBINODE_H

#समावेश "unicode/utypes.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    UnicodeSet;
class    UVector;

class RBBINode : खुला UMemory {
    खुला:
        क्रमागत गाँठ_Type {
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

        क्रमागत OpPrecedence {      
            precZero,
            precStart,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        गाँठ_Type      fType;
        RBBINode      *fParent;
        RBBINode      *f_बायाँ_Child;
        RBBINode      *f_दायाँ_Child;
        UnicodeSet    *f_निवेश_Set;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString fText;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        पूर्णांक           fFirstPos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        पूर्णांक           fLastPos;             //  Last position in the rule source string
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

        UBool         f_Rule_मूल;            // True if this node is the root of a rule.
        UBool         fChainIn;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *fFirstPosSet;
        UVector       *fLastPosSet;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(गाँठ_Type t, UErrorCode& status);
        RBBINode(स्थिर RBBINode &other, UErrorCode& status);
        ~RBBINode();
        अटल व्योम  NRDelete_गाँठ(RBBINode *गाँठ);
        
        RBBINode    *clone_वृक्ष(UErrorCode &status, पूर्णांक depth=0);
        RBBINode    *flattenVariables(UErrorCode &status, पूर्णांक depth=0);
        व्योम         flattenSets(UErrorCode &status, पूर्णांक depth=0);
        व्योम         findNodes(UVector *dest, RBBINode::गाँठ_Type kind, UErrorCode &status);

#अगर_घोषित RBBI_DEBUG
        अटल व्योम print_गाँठ_Header();
        अटल व्योम print_गाँठ(स्थिर RBBINode *n);
        अटल व्योम print_वृक्ष(स्थिर RBBINode *n, UBool withHeading);
#पूर्ण_अगर

    निजी:
        RBBINode &चालक = (स्थिर RBBINode &other); // No defs.
        bool चालक == (स्थिर RBBINode &other);     // Private, so these functions won't accidentally be used.

#अगर_घोषित RBBI_DEBUG
    खुला:
        पूर्णांक           f_Serial_अंक;           //  Debugging aids.
#पूर्ण_अगर
};

#अगर_घोषित RBBI_DEBUG
U_CFUNC व्योम 
RBBI_DEBUG_printUnicodeString(स्थिर UnicodeString &s, पूर्णांक minWidth=0);
#पूर्ण_अगर

U_NAMESPACE_END

#पूर्ण_अगर

