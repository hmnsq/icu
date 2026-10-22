// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#ifndef RBBINODE_H
#define RBBINODE_H

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    UnicodeSet;
class    UVector;

class RBBINode : सार्वजनिक UMemory {
    सार्वजनिक:
        गणना गाँठो_Type {
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

        गणना OpPrecedence {      
            precZero,
            precStart,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        गाँठो_Type      fType;
        RBBINode      *fParent;
        RBBINode      *f_बायाँ_Child;
        RBBINode      *f_दायाँ_Child;
        UnicodeSet    *f_आगत_Set;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString fText;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        पूर्णाङ्क           fFirstPos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        पूर्णाङ्क           fLastPos;             //  Last position in the rule source string
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

        UBool         f_Rule_जरा;            // True if this node is the root of a rule.
        UBool         fChainIn;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *fFirstPosSet;
        UVector       *fLastPosSet;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(गाँठो_Type t, UErrorCode& status);
        RBBINode(अचर RBBINode &other, UErrorCode& status);
        ~RBBINode();
        स्थिर शून्य  NRDelete_गाँठो(RBBINode *गाँठो);
        
        RBBINode    *clone_रूख(UErrorCode &status, पूर्णाङ्क depth=0);
        RBBINode    *flattenVariables(UErrorCode &status, पूर्णाङ्क depth=0);
        शून्य         flattenSets(UErrorCode &status, पूर्णाङ्क depth=0);
        शून्य         findNodes(UVector *dest, RBBINode::गाँठो_Type kind, UErrorCode &status);

#ifdef RBBI_DEBUG
        स्थिर शून्य print_गाँठो_Header();
        स्थिर शून्य print_गाँठो(अचर RBBINode *n);
        स्थिर शून्य print_रूख(अचर RBBINode *n, UBool withHeading);
#endif

    गुप्त:
        RBBINode &चालक = (अचर RBBINode &other); // No defs.
        bool चालक == (अचर RBBINode &other);     // Private, so these functions won't accidentally be used.

#ifdef RBBI_DEBUG
    सार्वजनिक:
        पूर्णाङ्क           f_Serial_अङ्क;           //  Debugging aids.
#endif
};

#ifdef RBBI_DEBUG
U_CFUNC शून्य 
RBBI_DEBUG_printUnicodeString(अचर UnicodeString &s, पूर्णाङ्क minWidth=0);
#endif

U_NAMESPACE_END

#endif

