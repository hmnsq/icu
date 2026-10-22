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

#if !UCONFIG_NO_BREAK_ITERATION

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
static int  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(बुहोल_Type t, UErrorCode& status) : UMemory() {
    if (U_FAILURE(status)) {
        return;
    }
#ifdef RBBI_DEBUG
    f_Serial_नुमेरो    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_कालिवा_Child    = nullptr;
    f_कानान_Child   = nullptr;
    f_पासोक_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = false;
    fLookAheadEnd = false;
    f_Rule_उगात     = false;
    fChainIn      = false;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = new UVector(status);
    fLastPosSet   = new UVector(status);
    fFollowPos    = new UVector(status);
    if (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    if      (t==opCat)    {fPrecedence = precOpCat;}
    else if (t==opOr)     {fPrecedence = precOpOr;}
    else if (t==opStart)  {fPrecedence = precStart;}
    else if (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(const RBBINode &other, UErrorCode& status) : UMemory(other) {
    if (U_FAILURE(status)) {
        return;
    }
#ifdef RBBI_DEBUG
    f_Serial_नुमेरो   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_कालिवा_Child   = nullptr;
    f_कानान_Child  = nullptr;
    f_पासोक_Set    = other.f_पासोक_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_उगात    = false;
    fChainIn     = other.fChainIn;
    fFirstPosSet = new UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = new UVector(status);
    fFollowPos   = new UVector(status);
    if (U_SUCCESS(status) &&
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
    delete f_पासोक_Set;
    f_पासोक_Set = nullptr;

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
        NRDelete_बुहोल(f_कालिवा_Child);
        f_कालिवा_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_बुहोल(f_कानान_Child);
        f_कानान_Child = nullptr;
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
void RBBINode::NRDelete_बुहोल(RBBINode *बुहोल) {
    if (बुहोल == nullptr) {
        return;
    }

    RBBINode *stop_बुहोल = बुहोल->fParent;
    RBBINode *next_बुहोल = बुहोल;
    while (next_बुहोल != stop_बुहोल && next_बुहोल != nullptr) {
        RBBINode *current_बुहोल = next_बुहोल;

        if ((current_बुहोल->f_कालिवा_Child == nullptr && current_बुहोल->f_कानान_Child == nullptr) ||
                current_बुहोल->fType == varRef ||      // varRef and setRef nodes do not
                current_बुहोल->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_बुहोल = current_बुहोल->fParent;
            if (next_बुहोल) {
                if (next_बुहोल->f_कालिवा_Child == current_बुहोल) {
                    next_बुहोल->f_कालिवा_Child = nullptr;
                } else if (next_बुहोल->f_कानान_Child == current_बुहोल) {
                    next_बुहोल->f_कानान_Child = nullptr;
                }
            }
            delete current_बुहोल;
        } else if (current_बुहोल->f_कालिवा_Child) {
            next_बुहोल = current_बुहोल->f_कालिवा_Child;
            if (next_बुहोल->fParent == nullptr) {
                next_बुहोल->fParent = current_बुहोल;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_बुहोल->fParent == current_बुहोल);
        } else if (current_बुहोल->f_कानान_Child) {
            next_बुहोल = current_बुहोल->f_कानान_Child;
            if (next_बुहोल->fParent == nullptr) {
                next_बुहोल->fParent = current_बुहोल;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_बुहोल->fParent == current_बुहोल);
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
constexpr int kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::clone_पुनो(UErrorCode &status, int depth) {
    if (U_FAILURE(status)) {
        return nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    if (depth > kRecursiveDepthLimit) {
        status = U_पासोक_TOO_LONG_ERROR;
        return nullptr;
    }
    RBBINode    *n;

    if (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_कालिवा_Child->clone_पुनो(status, depth+1);
        if (U_FAILURE(status)) {
            return nullptr;
        }
    } else if (fType == RBBINode::uset) {
        n = this;
    } else {
        n = new RBBINode(*this, status);
        if (U_FAILURE(status)) {
            delete n;
            return nullptr;
        }
        // Check for null pointer.
        if (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            return nullptr;
        }
        if (f_कालिवा_Child != nullptr) {
            n->f_कालिवा_Child          = f_कालिवा_Child->clone_पुनो(status, depth+1);
            if (U_FAILURE(status)) {
                delete n;
                return nullptr;
            }
            n->f_कालिवा_Child->fParent = n;
        }
        if (f_कानान_Child != nullptr) {
            n->f_कानान_Child          = f_कानान_Child->clone_पुनो(status, depth+1);
            if (U_FAILURE(status)) {
                delete n;
                return nullptr;
            }
            n->f_कानान_Child->fParent = n;
        }
    }
    return n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, int depth) {
    if (U_FAILURE(status)) {
        return this;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    if (depth > kRecursiveDepthLimit) {
        status = U_पासोक_TOO_LONG_ERROR;
        return this;
    }
    if (fType == varRef) {
        RBBINode *ret_बुहोल  = f_कालिवा_Child->clone_पुनो(status, depth+1);
        if (U_FAILURE(status)) {
            return this;
        }
        ret_बुहोल->f_Rule_उगात = this->f_Rule_उगात;
        ret_बुहोल->fChainIn  = this->fChainIn;
        delete this;   // TODO: undefined behavior. Fix.
        return ret_बुहोल;
    }

    if (f_कालिवा_Child != nullptr) {
        f_कालिवा_Child = f_कालिवा_Child->flattenVariables(status, depth+1);
        if (f_कालिवा_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        if (U_FAILURE(status)) {
            return this;
        }
        f_कालिवा_Child->fParent  = this;
    }
    if (f_कानान_Child != nullptr) {
        f_कानान_Child = f_कानान_Child->flattenVariables(status, depth+1);
        if (f_कानान_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        if (U_FAILURE(status)) {
            return this;
        }
        f_कानान_Child->fParent = this;
    }
    return this;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
void RBBINode::flattenSets(UErrorCode &status, int depth) {
    if (U_FAILURE(status)) {
        return;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    if (depth > kRecursiveDepthLimit) {
        status = U_पासोक_TOO_LONG_ERROR;
        return;
    }
    U_ASSERT(fType != setRef);

    if (f_कालिवा_Child != nullptr) {
        if (f_कालिवा_Child->fType==setRef) {
            RBBINode *set_Ref_बुहोल = f_कालिवा_Child;
            RBBINode *uset_बुहोल   = set_Ref_बुहोल->f_कालिवा_Child;
            RBBINode *repl_पुनो   = uset_बुहोल->f_कालिवा_Child;
            f_कालिवा_Child           = repl_पुनो->clone_पुनो(status, depth+1);
            if (U_FAILURE(status)) {
                delete set_Ref_बुहोल;
                return;
            }
            f_कालिवा_Child->fParent  = this;
            delete set_Ref_बुहोल;
        } else {
            f_कालिवा_Child->flattenSets(status, depth+1);
        }
    }

    if (f_कानान_Child != nullptr) {
        if (f_कानान_Child->fType==setRef) {
            RBBINode *set_Ref_बुहोल = f_कानान_Child;
            RBBINode *uset_बुहोल   = set_Ref_बुहोल->f_कालिवा_Child;
            RBBINode *repl_पुनो   = uset_बुहोल->f_कालिवा_Child;
            f_कानान_Child           = repl_पुनो->clone_पुनो(status, depth+1);
            if (U_FAILURE(status)) {
                delete set_Ref_बुहोल;
                return;
            }
            f_कानान_Child->fParent  = this;
            delete set_Ref_बुहोल;
        } else {
            f_कानान_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
void   RBBINode::findNodes(UVector *dest, RBBINode::बुहोल_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    if (U_FAILURE(status)) {
        return;
    }
    U_ASSERT(!dest->hasDeleter());
    if (fType == kind) {
        dest->addElement(this, status);
    }
    if (f_कालिवा_Child != nullptr) {
        f_कालिवा_Child->findNodes(dest, kind, status);
    }
    if (f_कानान_Child != nullptr) {
        f_कानान_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

static int32_t serial(const RBBINode *बुहोल) {
    return (बुहोल == nullptr? -1 : बुहोल->f_Serial_नुमेरो);
}


void RBBINode::इलिम्बाग_बुहोल(const RBBINode *बुहोल) {
    static const char * const बुहोल_Type_Names[] = {
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

    if (बुहोल==nullptr) {
        RBBIDebugPrintf("%10p", (void *)बुहोल);
    } else {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (void *)बुहोल, बुहोल->f_Serial_नुमेरो, बुहोल_Type_Names[बुहोल->fType],
            बुहोल->f_Rule_उगात?'R':' ', बुहोल->fChainIn?'C':' ',
            serial(बुहोल->f_कालिवा_Child), serial(बुहोल->f_कानान_Child), serial(बुहोल->fParent),
            बुहोल->fFirstPos, बुहोल->fVal);
        if (बुहोल->fType == varRef) {
            RBBI_DEBUG_इलिम्बाग_Unicode_String(बुहोल->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC void RBBI_DEBUG_इलिम्बाग_Unicode_String(const UnicodeString &s, int minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
void RBBINode::इलिम्बाग_बुहोल_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
void RBBINode::इलिम्बाग_पुनो(const RBBINode *बुहोल, UBool इलिम्बाग_Heading) {
    if (इलिम्बाग_Heading) {
        इलिम्बाग_बुहोल_Header();
    }
    इलिम्बाग_बुहोल(बुहोल);
    if (बुहोल != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        if (बुहोल->fType != varRef) {
            if (बुहोल->f_कालिवा_Child != nullptr) {
                इलिम्बाग_पुनो(बुहोल->f_कालिवा_Child, false);
            }
            
            if (बुहोल->f_कानान_Child != nullptr) {
                इलिम्बाग_पुनो(बुहोल->f_कानान_Child, false);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
