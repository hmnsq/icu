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

#nếu !UCONFIG_NO_BREAK_ITERATION

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
static số_nguyên  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(NodeType t, UErrorCode& status) : UMemory() {
    nếu (U_FAILURE(status)) {
        trả_về;
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
    nếu (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    nếu      (t==opCat)    {fPrecedence = precOpCat;}
    không_thì nếu (t==opOr)     {fPrecedence = precOpOr;}
    không_thì nếu (t==opStart)  {fPrecedence = precStart;}
    không_thì nếu (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(const RBBINode &other, UErrorCode& status) : UMemory(other) {
    nếu (U_FAILURE(status)) {
        trả_về;
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
    nếu (U_SUCCESS(status) &&
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
    case varRef:
    case setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        break;

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
    nếu (node == nullptr) {
        trả_về;
    }

    RBBINode *stopNode = node->fParent;
    RBBINode *nextNode = node;
    trong_khi (nextNode != stopNode && nextNode != nullptr) {
        RBBINode *currentNode = nextNode;

        nếu ((currentNode->fLeftChild == nullptr && currentNode->fRightChild == nullptr) ||
                currentNode->fType == varRef ||      // varRef and setRef nodes do not
                currentNode->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            nextNode = currentNode->fParent;
            nếu (nextNode) {
                nếu (nextNode->fLeftChild == currentNode) {
                    nextNode->fLeftChild = nullptr;
                } không_thì nếu (nextNode->fRightChild == currentNode) {
                    nextNode->fRightChild = nullptr;
                }
            }
            delete currentNode;
        } không_thì nếu (currentNode->fLeftChild) {
            nextNode = currentNode->fLeftChild;
            nếu (nextNode->fParent == nullptr) {
                nextNode->fParent = currentNode;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(nextNode->fParent == currentNode);
        } không_thì nếu (currentNode->fRightChild) {
            nextNode = currentNode->fRightChild;
            nếu (nextNode->fParent == nullptr) {
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
constexpr số_nguyên kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::cloneTree(UErrorCode &status, số_nguyên depth) {
    nếu (U_FAILURE(status)) {
        trả_về nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    nếu (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        trả_về nullptr;
    }
    RBBINode    *n;

    nếu (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = fLeftChild->cloneTree(status, depth+1);
        nếu (U_FAILURE(status)) {
            trả_về nullptr;
        }
    } không_thì nếu (fType == RBBINode::uset) {
        n = this;
    } không_thì {
        n = new RBBINode(*this, status);
        nếu (U_FAILURE(status)) {
            delete n;
            trả_về nullptr;
        }
        // Check for null pointer.
        nếu (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            trả_về nullptr;
        }
        nếu (fLeftChild != nullptr) {
            n->fLeftChild          = fLeftChild->cloneTree(status, depth+1);
            nếu (U_FAILURE(status)) {
                delete n;
                trả_về nullptr;
            }
            n->fLeftChild->fParent = n;
        }
        nếu (fRightChild != nullptr) {
            n->fRightChild          = fRightChild->cloneTree(status, depth+1);
            nếu (U_FAILURE(status)) {
                delete n;
                trả_về nullptr;
            }
            n->fRightChild->fParent = n;
        }
    }
    trả_về n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, số_nguyên depth) {
    nếu (U_FAILURE(status)) {
        trả_về this;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    nếu (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        trả_về this;
    }
    nếu (fType == varRef) {
        RBBINode *retNode  = fLeftChild->cloneTree(status, depth+1);
        nếu (U_FAILURE(status)) {
            trả_về this;
        }
        retNode->fRuleRoot = this->fRuleRoot;
        retNode->fChainIn  = this->fChainIn;
        delete this;   // TODO: undefined behavior. Fix.
        trả_về retNode;
    }

    nếu (fLeftChild != nullptr) {
        fLeftChild = fLeftChild->flattenVariables(status, depth+1);
        nếu (fLeftChild == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        nếu (U_FAILURE(status)) {
            trả_về this;
        }
        fLeftChild->fParent  = this;
    }
    nếu (fRightChild != nullptr) {
        fRightChild = fRightChild->flattenVariables(status, depth+1);
        nếu (fRightChild == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        nếu (U_FAILURE(status)) {
            trả_về this;
        }
        fRightChild->fParent = this;
    }
    trả_về this;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
void RBBINode::flattenSets(UErrorCode &status, số_nguyên depth) {
    nếu (U_FAILURE(status)) {
        trả_về;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    nếu (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        trả_về;
    }
    U_ASSERT(fType != setRef);

    nếu (fLeftChild != nullptr) {
        nếu (fLeftChild->fType==setRef) {
            RBBINode *setRefNode = fLeftChild;
            RBBINode *usetNode   = setRefNode->fLeftChild;
            RBBINode *replTree   = usetNode->fLeftChild;
            fLeftChild           = replTree->cloneTree(status, depth+1);
            nếu (U_FAILURE(status)) {
                delete setRefNode;
                trả_về;
            }
            fLeftChild->fParent  = this;
            delete setRefNode;
        } không_thì {
            fLeftChild->flattenSets(status, depth+1);
        }
    }

    nếu (fRightChild != nullptr) {
        nếu (fRightChild->fType==setRef) {
            RBBINode *setRefNode = fRightChild;
            RBBINode *usetNode   = setRefNode->fLeftChild;
            RBBINode *replTree   = usetNode->fLeftChild;
            fRightChild           = replTree->cloneTree(status, depth+1);
            nếu (U_FAILURE(status)) {
                delete setRefNode;
                trả_về;
            }
            fRightChild->fParent  = this;
            delete setRefNode;
        } không_thì {
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
    nếu (U_FAILURE(status)) {
        trả_về;
    }
    U_ASSERT(!dest->hasDeleter());
    nếu (fType == kind) {
        dest->addElement(this, status);
    }
    nếu (fLeftChild != nullptr) {
        fLeftChild->findNodes(dest, kind, status);
    }
    nếu (fRightChild != nullptr) {
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
    trả_về (node == nullptr? -1 : node->fSerialNum);
}


void RBBINode::printNode(const RBBINode *node) {
    static const char * const nodeTypeNames[] = {
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

    nếu (node==nullptr) {
        RBBIDebugPrintf("%10p", (void *)node);
    } không_thì {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (void *)node, node->fSerialNum, nodeTypeNames[node->fType],
            node->fRuleRoot?'R':' ', node->fChainIn?'C':' ',
            serial(node->fLeftChild), serial(node->fRightChild), serial(node->fParent),
            node->fFirstPos, node->fVal);
        nếu (node->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(node->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC void RBBI_DEBUG_printUnicodeString(const UnicodeString &s, số_nguyên minWidth) {
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
    nếu (printHeading) {
        printNodeHeader();
    }
    printNode(node);
    nếu (node != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        nếu (node->fType != varRef) {
            nếu (node->fLeftChild != nullptr) {
                printTree(node->fLeftChild, false);
            }
            
            nếu (node->fRightChild != nullptr) {
                printTree(node->fRightChild, false);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
