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

#إذا !UCONFIG_NO_BREAK_ITERATION

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
RBBINode::RBBINode(كلي_Type t, UErrorCode& status) : UMemory() {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
#ifdef RBBI_DEBUG
    f_Serial_لمب    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_هغو_Child    = nullptr;
    f_دام_Child   = nullptr;
    f_شغرو_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = خطأ;
    fLookAheadEnd = خطأ;
    f_Rule_سيو     = خطأ;
    fChainIn      = خطأ;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = جديد UVector(status);
    fLastPosSet   = جديد UVector(status);
    fFollowPos    = جديد UVector(status);
    إذا (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    إذا      (t==opCat)    {fPrecedence = precOpCat;}
    وإلا إذا (t==opOr)     {fPrecedence = precOpOr;}
    وإلا إذا (t==opStart)  {fPrecedence = precStart;}
    وإلا إذا (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ثابت RBBINode &other, UErrorCode& status) : UMemory(other) {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
#ifdef RBBI_DEBUG
    f_Serial_لمب   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_هغو_Child   = nullptr;
    f_دام_Child  = nullptr;
    f_شغرو_Set    = other.f_شغرو_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_سيو    = خطأ;
    fChainIn     = other.fChainIn;
    fFirstPosSet = جديد UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = جديد UVector(status);
    fFollowPos   = جديد UVector(status);
    إذا (U_SUCCESS(status) &&
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
    delete f_شغرو_Set;
    f_شغرو_Set = nullptr;

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
        NRDelete_كلي(f_هغو_Child);
        f_هغو_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_كلي(f_دام_Child);
        f_دام_Child = nullptr;
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
فراغ RBBINode::NRDelete_كلي(RBBINode *كلي) {
    إذا (كلي == nullptr) {
        أرجع;
    }

    RBBINode *stop_كلي = كلي->fParent;
    RBBINode *next_كلي = كلي;
    طالما (next_كلي != stop_كلي && next_كلي != nullptr) {
        RBBINode *current_كلي = next_كلي;

        إذا ((current_كلي->f_هغو_Child == nullptr && current_كلي->f_دام_Child == nullptr) ||
                current_كلي->fType == varRef ||      // varRef and setRef nodes do not
                current_كلي->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_كلي = current_كلي->fParent;
            إذا (next_كلي) {
                إذا (next_كلي->f_هغو_Child == current_كلي) {
                    next_كلي->f_هغو_Child = nullptr;
                } وإلا إذا (next_كلي->f_دام_Child == current_كلي) {
                    next_كلي->f_دام_Child = nullptr;
                }
            }
            delete current_كلي;
        } وإلا إذا (current_كلي->f_هغو_Child) {
            next_كلي = current_كلي->f_هغو_Child;
            إذا (next_كلي->fParent == nullptr) {
                next_كلي->fParent = current_كلي;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_كلي->fParent == current_كلي);
        } وإلا إذا (current_كلي->f_دام_Child) {
            next_كلي = current_كلي->f_دام_Child;
            إذا (next_كلي->fParent == nullptr) {
                next_كلي->fParent = current_كلي;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_كلي->fParent == current_كلي);
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
RBBINode *RBBINode::clone_بشيا(UErrorCode &status, صحيح depth) {
    إذا (U_FAILURE(status)) {
        أرجع nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (depth > kRecursiveDepthLimit) {
        status = U_شغرو_TOO_LONG_ERROR;
        أرجع nullptr;
    }
    RBBINode    *n;

    إذا (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_هغو_Child->clone_بشيا(status, depth+1);
        إذا (U_FAILURE(status)) {
            أرجع nullptr;
        }
    } وإلا إذا (fType == RBBINode::uset) {
        n = هذا;
    } وإلا {
        n = جديد RBBINode(*هذا, status);
        إذا (U_FAILURE(status)) {
            delete n;
            أرجع nullptr;
        }
        // Check for null pointer.
        إذا (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            أرجع nullptr;
        }
        إذا (f_هغو_Child != nullptr) {
            n->f_هغو_Child          = f_هغو_Child->clone_بشيا(status, depth+1);
            إذا (U_FAILURE(status)) {
                delete n;
                أرجع nullptr;
            }
            n->f_هغو_Child->fParent = n;
        }
        إذا (f_دام_Child != nullptr) {
            n->f_دام_Child          = f_دام_Child->clone_بشيا(status, depth+1);
            إذا (U_FAILURE(status)) {
                delete n;
                أرجع nullptr;
            }
            n->f_دام_Child->fParent = n;
        }
    }
    أرجع n;
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
    إذا (U_FAILURE(status)) {
        أرجع هذا;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (depth > kRecursiveDepthLimit) {
        status = U_شغرو_TOO_LONG_ERROR;
        أرجع هذا;
    }
    إذا (fType == varRef) {
        RBBINode *ret_كلي  = f_هغو_Child->clone_بشيا(status, depth+1);
        إذا (U_FAILURE(status)) {
            أرجع هذا;
        }
        ret_كلي->f_Rule_سيو = هذا->f_Rule_سيو;
        ret_كلي->fChainIn  = هذا->fChainIn;
        delete هذا;   // TODO: undefined behavior. Fix.
        أرجع ret_كلي;
    }

    إذا (f_هغو_Child != nullptr) {
        f_هغو_Child = f_هغو_Child->flattenVariables(status, depth+1);
        إذا (f_هغو_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        إذا (U_FAILURE(status)) {
            أرجع هذا;
        }
        f_هغو_Child->fParent  = هذا;
    }
    إذا (f_دام_Child != nullptr) {
        f_دام_Child = f_دام_Child->flattenVariables(status, depth+1);
        إذا (f_دام_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        إذا (U_FAILURE(status)) {
            أرجع هذا;
        }
        f_دام_Child->fParent = هذا;
    }
    أرجع هذا;
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
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (depth > kRecursiveDepthLimit) {
        status = U_شغرو_TOO_LONG_ERROR;
        أرجع;
    }
    U_ASSERT(fType != setRef);

    إذا (f_هغو_Child != nullptr) {
        إذا (f_هغو_Child->fType==setRef) {
            RBBINode *set_Ref_كلي = f_هغو_Child;
            RBBINode *uset_كلي   = set_Ref_كلي->f_هغو_Child;
            RBBINode *repl_بشيا   = uset_كلي->f_هغو_Child;
            f_هغو_Child           = repl_بشيا->clone_بشيا(status, depth+1);
            إذا (U_FAILURE(status)) {
                delete set_Ref_كلي;
                أرجع;
            }
            f_هغو_Child->fParent  = هذا;
            delete set_Ref_كلي;
        } وإلا {
            f_هغو_Child->flattenSets(status, depth+1);
        }
    }

    إذا (f_دام_Child != nullptr) {
        إذا (f_دام_Child->fType==setRef) {
            RBBINode *set_Ref_كلي = f_دام_Child;
            RBBINode *uset_كلي   = set_Ref_كلي->f_هغو_Child;
            RBBINode *repl_بشيا   = uset_كلي->f_هغو_Child;
            f_دام_Child           = repl_بشيا->clone_بشيا(status, depth+1);
            إذا (U_FAILURE(status)) {
                delete set_Ref_كلي;
                أرجع;
            }
            f_دام_Child->fParent  = هذا;
            delete set_Ref_كلي;
        } وإلا {
            f_دام_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
فراغ   RBBINode::findNodes(UVector *dest, RBBINode::كلي_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    U_ASSERT(!dest->hasDeleter());
    إذا (fType == kind) {
        dest->addElement(هذا, status);
    }
    إذا (f_هغو_Child != nullptr) {
        f_هغو_Child->findNodes(dest, kind, status);
    }
    إذا (f_دام_Child != nullptr) {
        f_دام_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

ساكن int32_t serial(ثابت RBBINode *كلي) {
    أرجع (كلي == nullptr? -1 : كلي->f_Serial_لمب);
}


فراغ RBBINode::بغ_كلي(ثابت RBBINode *كلي) {
    ساكن ثابت حرف * ثابت كلي_Type_Names[] = {
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

    إذا (كلي==nullptr) {
        RBBIDebugPrintf("%10p", (فراغ *)كلي);
    } وإلا {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (فراغ *)كلي, كلي->f_Serial_لمب, كلي_Type_Names[كلي->fType],
            كلي->f_Rule_سيو?'R':' ', كلي->fChainIn?'C':' ',
            serial(كلي->f_هغو_Child), serial(كلي->f_دام_Child), serial(كلي->fParent),
            كلي->fFirstPos, كلي->fVal);
        إذا (كلي->fType == varRef) {
            RBBI_DEBUG_بغ_Unicode_String(كلي->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC فراغ RBBI_DEBUG_بغ_Unicode_String(ثابت UnicodeString &s, صحيح minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
فراغ RBBINode::بغ_كلي_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
فراغ RBBINode::بغ_بشيا(ثابت RBBINode *كلي, UBool بغ_Heading) {
    إذا (بغ_Heading) {
        بغ_كلي_Header();
    }
    بغ_كلي(كلي);
    إذا (كلي != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        إذا (كلي->fType != varRef) {
            إذا (كلي->f_هغو_Child != nullptr) {
                بغ_بشيا(كلي->f_هغو_Child, خطأ);
            }
            
            إذا (كلي->f_دام_Child != nullptr) {
                بغ_بشيا(كلي->f_دام_Child, خطأ);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
