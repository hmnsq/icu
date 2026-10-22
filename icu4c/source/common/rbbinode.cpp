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

#ضمن "unicode/utypes.h"

#אם !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/unistr.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/parsepos.h"

#ضمن "cstr.h"
#ضمن "uvector.h"

#ضمن "rbbirb.h"
#ضمن "rbbinode.h"

#ضمن "uassert.h"


U_NAMESPACE_BEGIN

#ifdef RBBI_DEBUG
ساكن صحيح  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(NodeType t, UErrorCode& status) : UMemory() {
    אם (U_FAILURE(status)) {
        החזר;
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
    fNullable     = שקר;
    fLookAheadEnd = שקר;
    fRuleRoot     = שקר;
    fChainIn      = שקר;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = جديد UVector(status);
    fLastPosSet   = جديد UVector(status);
    fFollowPos    = جديد UVector(status);
    אם (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    אם      (t==opCat)    {fPrecedence = precOpCat;}
    אחרת אם (t==opOr)     {fPrecedence = precOpOr;}
    אחרת אם (t==opStart)  {fPrecedence = precStart;}
    אחרת אם (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ثابت RBBINode &other, UErrorCode& status) : UMemory(other) {
    אם (U_FAILURE(status)) {
        החזר;
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
    fRuleRoot    = שקר;
    fChainIn     = other.fChainIn;
    fFirstPosSet = جديد UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = جديد UVector(status);
    fFollowPos   = جديد UVector(status);
    אם (U_SUCCESS(status) &&
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

    بدل (هذا->fType) {
    حالة varRef:
    حالة setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        اكسر;

    افتراضي:
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
فراغ RBBINode::NRDeleteNode(RBBINode *node) {
    אם (node == nullptr) {
        החזר;
    }

    RBBINode *stopNode = node->fParent;
    RBBINode *nextNode = node;
    כלעוד (nextNode != stopNode && nextNode != nullptr) {
        RBBINode *currentNode = nextNode;

        אם ((currentNode->fLeftChild == nullptr && currentNode->fRightChild == nullptr) ||
                currentNode->fType == varRef ||      // varRef and setRef nodes do not
                currentNode->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            nextNode = currentNode->fParent;
            אם (nextNode) {
                אם (nextNode->fLeftChild == currentNode) {
                    nextNode->fLeftChild = nullptr;
                } אחרת אם (nextNode->fRightChild == currentNode) {
                    nextNode->fRightChild = nullptr;
                }
            }
            delete currentNode;
        } אחרת אם (currentNode->fLeftChild) {
            nextNode = currentNode->fLeftChild;
            אם (nextNode->fParent == nullptr) {
                nextNode->fParent = currentNode;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(nextNode->fParent == currentNode);
        } אחרת אם (currentNode->fRightChild) {
            nextNode = currentNode->fRightChild;
            אם (nextNode->fParent == nullptr) {
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
constexpr صحيح kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::cloneTree(UErrorCode &status, صحيح depth) {
    אם (U_FAILURE(status)) {
        החזר nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    אם (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        החזר nullptr;
    }
    RBBINode    *n;

    אם (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = fLeftChild->cloneTree(status, depth+1);
        אם (U_FAILURE(status)) {
            החזר nullptr;
        }
    } אחרת אם (fType == RBBINode::uset) {
        n = هذا;
    } אחרת {
        n = جديد RBBINode(*هذا, status);
        אם (U_FAILURE(status)) {
            delete n;
            החזר nullptr;
        }
        // Check for null pointer.
        אם (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            החזר nullptr;
        }
        אם (fLeftChild != nullptr) {
            n->fLeftChild          = fLeftChild->cloneTree(status, depth+1);
            אם (U_FAILURE(status)) {
                delete n;
                החזר nullptr;
            }
            n->fLeftChild->fParent = n;
        }
        אם (fRightChild != nullptr) {
            n->fRightChild          = fRightChild->cloneTree(status, depth+1);
            אם (U_FAILURE(status)) {
                delete n;
                החזר nullptr;
            }
            n->fRightChild->fParent = n;
        }
    }
    החזר n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, صحيح depth) {
    אם (U_FAILURE(status)) {
        החזר هذا;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    אם (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        החזר هذا;
    }
    אם (fType == varRef) {
        RBBINode *retNode  = fLeftChild->cloneTree(status, depth+1);
        אם (U_FAILURE(status)) {
            החזר هذا;
        }
        retNode->fRuleRoot = هذا->fRuleRoot;
        retNode->fChainIn  = هذا->fChainIn;
        delete هذا;   // TODO: undefined behavior. Fix.
        החזר retNode;
    }

    אם (fLeftChild != nullptr) {
        fLeftChild = fLeftChild->flattenVariables(status, depth+1);
        אם (fLeftChild == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        אם (U_FAILURE(status)) {
            החזר هذا;
        }
        fLeftChild->fParent  = هذا;
    }
    אם (fRightChild != nullptr) {
        fRightChild = fRightChild->flattenVariables(status, depth+1);
        אם (fRightChild == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        אם (U_FAILURE(status)) {
            החזר هذا;
        }
        fRightChild->fParent = هذا;
    }
    החזר هذا;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
فراغ RBBINode::flattenSets(UErrorCode &status, صحيح depth) {
    אם (U_FAILURE(status)) {
        החזר;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    אם (depth > kRecursiveDepthLimit) {
        status = U_INPUT_TOO_LONG_ERROR;
        החזר;
    }
    U_ASSERT(fType != setRef);

    אם (fLeftChild != nullptr) {
        אם (fLeftChild->fType==setRef) {
            RBBINode *setRefNode = fLeftChild;
            RBBINode *usetNode   = setRefNode->fLeftChild;
            RBBINode *replTree   = usetNode->fLeftChild;
            fLeftChild           = replTree->cloneTree(status, depth+1);
            אם (U_FAILURE(status)) {
                delete setRefNode;
                החזר;
            }
            fLeftChild->fParent  = هذا;
            delete setRefNode;
        } אחרת {
            fLeftChild->flattenSets(status, depth+1);
        }
    }

    אם (fRightChild != nullptr) {
        אם (fRightChild->fType==setRef) {
            RBBINode *setRefNode = fRightChild;
            RBBINode *usetNode   = setRefNode->fLeftChild;
            RBBINode *replTree   = usetNode->fLeftChild;
            fRightChild           = replTree->cloneTree(status, depth+1);
            אם (U_FAILURE(status)) {
                delete setRefNode;
                החזר;
            }
            fRightChild->fParent  = هذا;
            delete setRefNode;
        } אחרת {
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
فراغ   RBBINode::findNodes(UVector *dest, RBBINode::NodeType kind, UErrorCode &status) {
    /* test for buffer overflows */
    אם (U_FAILURE(status)) {
        החזר;
    }
    U_ASSERT(!dest->hasDeleter());
    אם (fType == kind) {
        dest->addElement(هذا, status);
    }
    אם (fLeftChild != nullptr) {
        fLeftChild->findNodes(dest, kind, status);
    }
    אם (fRightChild != nullptr) {
        fRightChild->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

ساكن int32_t serial(ثابت RBBINode *node) {
    החזר (node == nullptr? -1 : node->fSerialNum);
}


فراغ RBBINode::printNode(ثابت RBBINode *node) {
    ساكن ثابت حرف * ثابت nodeTypeNames[] = {
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

    אם (node==nullptr) {
        RBBIDebugPrintf("%10p", (فراغ *)node);
    } אחרת {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (فراغ *)node, node->fSerialNum, nodeTypeNames[node->fType],
            node->fRuleRoot?'R':' ', node->fChainIn?'C':' ',
            serial(node->fLeftChild), serial(node->fRightChild), serial(node->fParent),
            node->fFirstPos, node->fVal);
        אם (node->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(node->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC فراغ RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحيح minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
فراغ RBBINode::printNodeHeader() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
فراغ RBBINode::printTree(ثابت RBBINode *node, UBool printHeading) {
    אם (printHeading) {
        printNodeHeader();
    }
    printNode(node);
    אם (node != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        אם (node->fType != varRef) {
            אם (node->fLeftChild != nullptr) {
                printTree(node->fLeftChild, שקר);
            }
            
            אם (node->fRightChild != nullptr) {
                printTree(node->fRightChild, שקר);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
