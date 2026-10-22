// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 2002-2016 International Business Machines Corporation   *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

//
//  File:  rbbinode.cpp
//
//         Implementation of class RBBINode, which represents a node in the
//         tree generated when parsing the Rules Based Break Iterator rules.
//
//         This "Class" is actually closer to a struct.
//         Code using it is expected to directly access fields much of the time.
//

#include "unicode/utypes.h"

#ᯀᯮᯝ !UCONFIG_NO_BREAK_ITERATION

#include "unicode/unistr.h"
#include "unicode/uniset.h"
#include "unicode/uchar.h"
#include "unicode/parsepos.h"

#include "cstr.h"
#include "uvector.h"

#include "rbbirb.h"
#include "rbbinode.h"

#include "uassert.h"


U_NAMESPACE_BEGIN

#ifdef RBBI_DEBUG
static puur_naa_nka  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(NodeType t, UErrorCode& status) : UMemory() {
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ;
    }
#ifdef RBBI_DEBUG
    fSerialNum    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    fLeftChild    = nullptr;
    fRightChild   = nullptr;
    fInputSet     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = false;
    fLookAheadEnd = false;
    fRuleRoot     = false;
    fChainIn      = false;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = new UVector(status);
    fLastPosSet   = new UVector(status);
    fFollowPos    = new UVector(status);
    ᯀᯮᯝ (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    ᯀᯮᯝ      (t==opCat)    {fPrecedence = precOpCat;}
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (t==opOr)     {fPrecedence = precOpOr;}
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (t==opStart)  {fPrecedence = precStart;}
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(const RBBINode &other, UErrorCode& status) : UMemory(other) {
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ;
    }
#ifdef RBBI_DEBUG
    fSerialNum   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    fLeftChild   = nullptr;
    fRightChild  = nullptr;
    fInputSet    = other.fInputSet;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    fRuleRoot    = false;
    fChainIn     = other.fChainIn;
    fFirstPosSet = new UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = new UVector(status);
    fFollowPos   = new UVector(status);
    ᯀᯮᯝ (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
}


//-------------------------------------------------------------------------
//
//    Destructor.   Deletes both this node AND any child nodes,
//                  except in the case of variable reference nodes.  For
//                  these, the l. child points back to the definition, which
//                  is common for all references to the variable, meaning
//                  it can't be deleted here.
//
//-------------------------------------------------------------------------
RBBINode::~RBBINode() {
    // printf("deleting node %8x   serial %4d\n", this, this->fSerialNum);
    delete fInputSet;
    fInputSet = nullptr;

    switch (this->fType) {
    pak_sa varRef:
    pak_sa setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        bha_nga;

    default:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDeleteNode(fLeftChild);
        fLeftChild =   nullptr;
        // delete        fRightChild;
        NRDeleteNode(fRightChild);
        fRightChild = nullptr;
    }

    delete fFirstPosSet;
    delete fLastPosSet;
    delete fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
void RBBINode::NRDeleteNode(RBBINode *node) {
    ᯀᯮᯝ (node == nullptr) {
        ᯀᯮᯞ;
    }

    RBBINode *stopNode = node->fParent;
    RBBINode *nextNode = node;
    ᯀᯬᯝ (nextNode != stopNode && nextNode != nullptr) {
        RBBINode *currentNode = nextNode;

        ᯀᯮᯝ ((currentNode->fLeftChild == nullptr && currentNode->fRightChild == nullptr) ||
                currentNode->fType == varRef ||      // varRef and setRef nodes do not
                currentNode->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            nextNode = currentNode->fParent;
            ᯀᯮᯝ (nextNode) {
                ᯀᯮᯝ (nextNode->fLeftChild == currentNode) {
                    nextNode->fLeftChild = nullptr;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (nextNode->fRightChild == currentNode) {
                    nextNode->fRightChild = nullptr;
                }
            }
            delete currentNode;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (currentNode->fLeftChild) {
            nextNode = currentNode->fLeftChild;
            ᯀᯮᯝ (nextNode->fParent == nullptr) {
                nextNode->fParent = currentNode;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(nextNode->fParent == currentNode);
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (currentNode->fRightChild) {
            nextNode = currentNode->fRightChild;
            ᯀᯮᯝ (nextNode->fParent == nullptr) {
                nextNode->fParent = currentNode;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(nextNode->fParent == currentNode);
        }
    }
}

//-------------------------------------------------------------------------
//
//    cloneTree     Make a copy of the subtree rooted at this node.
//                  Discard any variable references encountered along the way,
//                  and replace with copies of the variable's definitions.
//                  Used to replicate the expression underneath variable
//                  references in preparation for generating the DFA tables.
//
//-------------------------------------------------------------------------
constexpr puur_naa_nka kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::cloneTree(UErrorCode &status, puur_naa_nka depth) {
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ᯀᯮᯝ (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        ᯀᯮᯞ nullptr;
    }
    RBBINode    *n;

    ᯀᯮᯝ (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = fLeftChild->cloneTree(status, depth+1);
        ᯀᯮᯝ (U_FAILURE(status)) {
            ᯀᯮᯞ nullptr;
        }
    } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (fType == RBBINode::uset) {
        n = this;
    } ᯀᯮᯝᯂᯪ {
        n = new RBBINode(*this, status);
        ᯀᯮᯝ (U_FAILURE(status)) {
            delete n;
            ᯀᯮᯞ nullptr;
        }
        // Check for null pointer.
        ᯀᯮᯝ (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            ᯀᯮᯞ nullptr;
        }
        ᯀᯮᯝ (fLeftChild != nullptr) {
            n->fLeftChild          = fLeftChild->cloneTree(status, depth+1);
            ᯀᯮᯝ (U_FAILURE(status)) {
                delete n;
                ᯀᯮᯞ nullptr;
            }
            n->fLeftChild->fParent = n;
        }
        ᯀᯮᯝ (fRightChild != nullptr) {
            n->fRightChild          = fRightChild->cloneTree(status, depth+1);
            ᯀᯮᯝ (U_FAILURE(status)) {
                delete n;
                ᯀᯮᯞ nullptr;
            }
            n->fRightChild->fParent = n;
        }
    }
    ᯀᯮᯞ n;
}



//-------------------------------------------------------------------------
//
//   flattenVariables   Walk a parse tree, replacing any variable
//                      references with a copy of the variable's definition.
//                      Aside from variables, the tree is not changed.
//
//                      Return the root of the tree.  If the root was not a variable
//                      reference, it remains unchanged - the root we started with
//                      is the root we return.  If, however, the root was a variable
//                      reference, the root of the newly cloned replacement tree will
//                      be returned, and the original tree deleted.
//
//                      This function works by recursively walking the tree
//                      without doing anything until a variable reference is
//                      found, then calling cloneTree() at that point.  Any
//                      nested references are handled by cloneTree(), not here.
//
//-------------------------------------------------------------------------
RBBINode *RBBINode::flattenVariables(UErrorCode& status, puur_naa_nka depth) {
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ this;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ᯀᯮᯝ (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        ᯀᯮᯞ this;
    }
    ᯀᯮᯝ (fType == varRef) {
        RBBINode *retNode  = fLeftChild->cloneTree(status, depth+1);
        ᯀᯮᯝ (U_FAILURE(status)) {
            ᯀᯮᯞ this;
        }
        retNode->fRuleRoot = this->fRuleRoot;
        retNode->fChainIn  = this->fChainIn;
        delete this;   // TODO: undefined behavior. Fix.
        ᯀᯮᯞ retNode;
    }

    ᯀᯮᯝ (fLeftChild != nullptr) {
        fLeftChild = fLeftChild->flattenVariables(status, depth+1);
        ᯀᯮᯝ (fLeftChild == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ᯀᯮᯝ (U_FAILURE(status)) {
            ᯀᯮᯞ this;
        }
        fLeftChild->fParent  = this;
    }
    ᯀᯮᯝ (fRightChild != nullptr) {
        fRightChild = fRightChild->flattenVariables(status, depth+1);
        ᯀᯮᯝ (fRightChild == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ᯀᯮᯝ (U_FAILURE(status)) {
            ᯀᯮᯞ this;
        }
        fRightChild->fParent = this;
    }
    ᯀᯮᯞ this;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
void RBBINode::flattenSets(UErrorCode &status, puur_naa_nka depth) {
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ᯀᯮᯝ (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        ᯀᯮᯞ;
    }
    U_ASSERT(fType != setRef);

    ᯀᯮᯝ (fLeftChild != nullptr) {
        ᯀᯮᯝ (fLeftChild->fType==setRef) {
            RBBINode *setRefNode = fLeftChild;
            RBBINode *usetNode   = setRefNode->fLeftChild;
            RBBINode *replTree   = usetNode->fLeftChild;
            fLeftChild           = replTree->cloneTree(status, depth+1);
            ᯀᯮᯝ (U_FAILURE(status)) {
                delete setRefNode;
                ᯀᯮᯞ;
            }
            fLeftChild->fParent  = this;
            delete setRefNode;
        } ᯀᯮᯝᯂᯪ {
            fLeftChild->flattenSets(status, depth+1);
        }
    }

    ᯀᯮᯝ (fRightChild != nullptr) {
        ᯀᯮᯝ (fRightChild->fType==setRef) {
            RBBINode *setRefNode = fRightChild;
            RBBINode *usetNode   = setRefNode->fLeftChild;
            RBBINode *replTree   = usetNode->fLeftChild;
            fRightChild           = replTree->cloneTree(status, depth+1);
            ᯀᯮᯝ (U_FAILURE(status)) {
                delete setRefNode;
                ᯀᯮᯞ;
            }
            fRightChild->fParent  = this;
            delete setRefNode;
        } ᯀᯮᯝᯂᯪ {
            fRightChild->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
void   RBBINode::findNodes(UVector *dest, RBBINode::NodeType kind, UErrorCode &status) {
    /* test for buffer overflows */
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ;
    }
    U_ASSERT(!dest->hasDeleter());
    ᯀᯮᯝ (fType == kind) {
        dest->addElement(this, status);
    }
    ᯀᯮᯝ (fLeftChild != nullptr) {
        fLeftChild->findNodes(dest, kind, status);
    }
    ᯀᯮᯝ (fRightChild != nullptr) {
        fRightChild->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

static int32_t serial(const RBBINode *node) {
    ᯀᯮᯞ (node == nullptr? -1 : node->fSerialNum);
}


void RBBINode::printNode(const RBBINode *node) {
    static const var_na * const nodeTypeNames[] = {
                "setRef",
                "uset",
                "varRef",
                "leafChar",
                "lookAhead",
                "tag",
                "endMark",
                "opStart",
                "opCat",
                "opOr",
                "opStar",
                "opPlus",
                "opQuestion",
                "opBreak",
                "opReverse",
                "opLParen"
    };

    ᯀᯮᯝ (node==nullptr) {
        RBBIDebugPrintf("%10p", (void *)node);
    } ᯀᯮᯝᯂᯪ {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (void *)node, node->fSerialNum, nodeTypeNames[node->fType],
            node->fRuleRoot?'R':' ', node->fChainIn?'C':' ',
            serial(node->fLeftChild), serial(node->fRightChild), serial(node->fParent),
            node->fFirstPos, node->fVal);
        ᯀᯮᯝ (node->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(node->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC void RBBI_DEBUG_printUnicodeString(const UnicodeString &s, puur_naa_nka minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
void RBBINode::printNodeHeader() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
void RBBINode::printTree(const RBBINode *node, UBool printHeading) {
    ᯀᯮᯝ (printHeading) {
        printNodeHeader();
    }
    printNode(node);
    ᯀᯮᯝ (node != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        ᯀᯮᯝ (node->fType != varRef) {
            ᯀᯮᯝ (node->fLeftChild != nullptr) {
                printTree(node->fLeftChild, false);
            }
            
            ᯀᯮᯝ (node->fRightChild != nullptr) {
                printTree(node->fRightChild, false);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
