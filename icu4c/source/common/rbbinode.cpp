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

#ຖ້າ !UCONFIG_NO_BREAK_ITERATION

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
static ຈຳນວນເຕັມ  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(NodeType t, UErrorCode& status) : UMemory() {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
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
    ຖ້າ (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    ຖ້າ      (t==opCat)    {fPrecedence = precOpCat;}
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (t==opOr)     {fPrecedence = precOpOr;}
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (t==opStart)  {fPrecedence = precStart;}
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(const RBBINode &other, UErrorCode& status) : UMemory(other) {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
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
    ຖ້າ (U_SUCCESS(status) &&
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
    ຖ້າ (node == nullptr) {
        ສົ່ງຄືນ;
    }

    RBBINode *stopNode = node->fParent;
    RBBINode *nextNode = node;
    ໃນຂະນະທີ່ (nextNode != stopNode && nextNode != nullptr) {
        RBBINode *currentNode = nextNode;

        ຖ້າ ((currentNode->fLeftChild == nullptr && currentNode->fRightChild == nullptr) ||
                currentNode->fType == varRef ||      // varRef and setRef nodes do not
                currentNode->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            nextNode = currentNode->fParent;
            ຖ້າ (nextNode) {
                ຖ້າ (nextNode->fLeftChild == currentNode) {
                    nextNode->fLeftChild = nullptr;
                } ບໍ່ດັ່ງນັ້ນ ຖ້າ (nextNode->fRightChild == currentNode) {
                    nextNode->fRightChild = nullptr;
                }
            }
            delete currentNode;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (currentNode->fLeftChild) {
            nextNode = currentNode->fLeftChild;
            ຖ້າ (nextNode->fParent == nullptr) {
                nextNode->fParent = currentNode;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(nextNode->fParent == currentNode);
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (currentNode->fRightChild) {
            nextNode = currentNode->fRightChild;
            ຖ້າ (nextNode->fParent == nullptr) {
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
constexpr ຈຳນວນເຕັມ kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::cloneTree(UErrorCode &status, ຈຳນວນເຕັມ depth) {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ຖ້າ (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        ສົ່ງຄືນ nullptr;
    }
    RBBINode    *n;

    ຖ້າ (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = fLeftChild->cloneTree(status, depth+1);
        ຖ້າ (U_FAILURE(status)) {
            ສົ່ງຄືນ nullptr;
        }
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (fType == RBBINode::uset) {
        n = this;
    } ບໍ່ດັ່ງນັ້ນ {
        n = new RBBINode(*this, status);
        ຖ້າ (U_FAILURE(status)) {
            delete n;
            ສົ່ງຄືນ nullptr;
        }
        // Check for null pointer.
        ຖ້າ (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ nullptr;
        }
        ຖ້າ (fLeftChild != nullptr) {
            n->fLeftChild          = fLeftChild->cloneTree(status, depth+1);
            ຖ້າ (U_FAILURE(status)) {
                delete n;
                ສົ່ງຄືນ nullptr;
            }
            n->fLeftChild->fParent = n;
        }
        ຖ້າ (fRightChild != nullptr) {
            n->fRightChild          = fRightChild->cloneTree(status, depth+1);
            ຖ້າ (U_FAILURE(status)) {
                delete n;
                ສົ່ງຄືນ nullptr;
            }
            n->fRightChild->fParent = n;
        }
    }
    ສົ່ງຄືນ n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, ຈຳນວນເຕັມ depth) {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ this;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ຖ້າ (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        ສົ່ງຄືນ this;
    }
    ຖ້າ (fType == varRef) {
        RBBINode *retNode  = fLeftChild->cloneTree(status, depth+1);
        ຖ້າ (U_FAILURE(status)) {
            ສົ່ງຄືນ this;
        }
        retNode->fRuleRoot = this->fRuleRoot;
        retNode->fChainIn  = this->fChainIn;
        delete this;   // TODO: undefined behavior. Fix.
        ສົ່ງຄືນ retNode;
    }

    ຖ້າ (fLeftChild != nullptr) {
        fLeftChild = fLeftChild->flattenVariables(status, depth+1);
        ຖ້າ (fLeftChild == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ຖ້າ (U_FAILURE(status)) {
            ສົ່ງຄືນ this;
        }
        fLeftChild->fParent  = this;
    }
    ຖ້າ (fRightChild != nullptr) {
        fRightChild = fRightChild->flattenVariables(status, depth+1);
        ຖ້າ (fRightChild == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ຖ້າ (U_FAILURE(status)) {
            ສົ່ງຄືນ this;
        }
        fRightChild->fParent = this;
    }
    ສົ່ງຄືນ this;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
void RBBINode::flattenSets(UErrorCode &status, ຈຳນວນເຕັມ depth) {
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    ຖ້າ (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        ສົ່ງຄືນ;
    }
    U_ASSERT(fType != setRef);

    ຖ້າ (fLeftChild != nullptr) {
        ຖ້າ (fLeftChild->fType==setRef) {
            RBBINode *setRefNode = fLeftChild;
            RBBINode *usetNode   = setRefNode->fLeftChild;
            RBBINode *replTree   = usetNode->fLeftChild;
            fLeftChild           = replTree->cloneTree(status, depth+1);
            ຖ້າ (U_FAILURE(status)) {
                delete setRefNode;
                ສົ່ງຄືນ;
            }
            fLeftChild->fParent  = this;
            delete setRefNode;
        } ບໍ່ດັ່ງນັ້ນ {
            fLeftChild->flattenSets(status, depth+1);
        }
    }

    ຖ້າ (fRightChild != nullptr) {
        ຖ້າ (fRightChild->fType==setRef) {
            RBBINode *setRefNode = fRightChild;
            RBBINode *usetNode   = setRefNode->fLeftChild;
            RBBINode *replTree   = usetNode->fLeftChild;
            fRightChild           = replTree->cloneTree(status, depth+1);
            ຖ້າ (U_FAILURE(status)) {
                delete setRefNode;
                ສົ່ງຄືນ;
            }
            fRightChild->fParent  = this;
            delete setRefNode;
        } ບໍ່ດັ່ງນັ້ນ {
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
    ຖ້າ (U_FAILURE(status)) {
        ສົ່ງຄືນ;
    }
    U_ASSERT(!dest->hasDeleter());
    ຖ້າ (fType == kind) {
        dest->addElement(this, status);
    }
    ຖ້າ (fLeftChild != nullptr) {
        fLeftChild->findNodes(dest, kind, status);
    }
    ຖ້າ (fRightChild != nullptr) {
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
    ສົ່ງຄືນ (node == nullptr? -1 : node->fSerialNum);
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

    ຖ້າ (node==nullptr) {
        RBBIDebugPrintf("%10p", (void *)node);
    } ບໍ່ດັ່ງນັ້ນ {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (void *)node, node->fSerialNum, nodeTypeNames[node->fType],
            node->fRuleRoot?'R':' ', node->fChainIn?'C':' ',
            serial(node->fLeftChild), serial(node->fRightChild), serial(node->fParent),
            node->fFirstPos, node->fVal);
        ຖ້າ (node->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(node->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC void RBBI_DEBUG_printUnicodeString(const UnicodeString &s, ຈຳນວນເຕັມ minWidth) {
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
    ຖ້າ (printHeading) {
        printNodeHeader();
    }
    printNode(node);
    ຖ້າ (node != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        ຖ້າ (node->fType != varRef) {
            ຖ້າ (node->fLeftChild != nullptr) {
                printTree(node->fLeftChild, false);
            }
            
            ຖ້າ (node->fRightChild != nullptr) {
                printTree(node->fRightChild, false);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
