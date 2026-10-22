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
RBBINode::RBBINode(گرێ_Type t, UErrorCode& status) : UMemory() {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
#ifdef RBBI_DEBUG
    f_Serial_ژمارە    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_چەپ_Child    = nullptr;
    f_ڕاست_Child   = nullptr;
    f_تێخستن_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = خطأ;
    fLookAheadEnd = خطأ;
    f_Rule_ڕەگ     = خطأ;
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
    f_Serial_ژمارە   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_چەپ_Child   = nullptr;
    f_ڕاست_Child  = nullptr;
    f_تێخستن_Set    = other.f_تێخستن_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_ڕەگ    = خطأ;
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
    delete f_تێخستن_Set;
    f_تێخستن_Set = nullptr;

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
        NRDelete_گرێ(f_چەپ_Child);
        f_چەپ_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_گرێ(f_ڕاست_Child);
        f_ڕاست_Child = nullptr;
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
فراغ RBBINode::NRDelete_گرێ(RBBINode *گرێ) {
    إذا (گرێ == nullptr) {
        أرجع;
    }

    RBBINode *stop_گرێ = گرێ->fParent;
    RBBINode *next_گرێ = گرێ;
    طالما (next_گرێ != stop_گرێ && next_گرێ != nullptr) {
        RBBINode *current_گرێ = next_گرێ;

        إذا ((current_گرێ->f_چەپ_Child == nullptr && current_گرێ->f_ڕاست_Child == nullptr) ||
                current_گرێ->fType == varRef ||      // varRef and setRef nodes do not
                current_گرێ->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_گرێ = current_گرێ->fParent;
            إذا (next_گرێ) {
                إذا (next_گرێ->f_چەپ_Child == current_گرێ) {
                    next_گرێ->f_چەپ_Child = nullptr;
                } وإلا إذا (next_گرێ->f_ڕاست_Child == current_گرێ) {
                    next_گرێ->f_ڕاست_Child = nullptr;
                }
            }
            delete current_گرێ;
        } وإلا إذا (current_گرێ->f_چەپ_Child) {
            next_گرێ = current_گرێ->f_چەپ_Child;
            إذا (next_گرێ->fParent == nullptr) {
                next_گرێ->fParent = current_گرێ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_گرێ->fParent == current_گرێ);
        } وإلا إذا (current_گرێ->f_ڕاست_Child) {
            next_گرێ = current_گرێ->f_ڕاست_Child;
            إذا (next_گرێ->fParent == nullptr) {
                next_گرێ->fParent = current_گرێ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_گرێ->fParent == current_گرێ);
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
RBBINode *RBBINode::clone_درەخت(UErrorCode &status, صحيح depth) {
    إذا (U_FAILURE(status)) {
        أرجع nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (depth > kRecursiveDepthLimit) {
        status = U_تێخستن_TOO_LONG_ERROR;
        أرجع nullptr;
    }
    RBBINode    *n;

    إذا (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_چەپ_Child->clone_درەخت(status, depth+1);
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
        إذا (f_چەپ_Child != nullptr) {
            n->f_چەپ_Child          = f_چەپ_Child->clone_درەخت(status, depth+1);
            إذا (U_FAILURE(status)) {
                delete n;
                أرجع nullptr;
            }
            n->f_چەپ_Child->fParent = n;
        }
        إذا (f_ڕاست_Child != nullptr) {
            n->f_ڕاست_Child          = f_ڕاست_Child->clone_درەخت(status, depth+1);
            إذا (U_FAILURE(status)) {
                delete n;
                أرجع nullptr;
            }
            n->f_ڕاست_Child->fParent = n;
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
        status = U_تێخستن_TOO_LONG_ERROR;
        أرجع هذا;
    }
    إذا (fType == varRef) {
        RBBINode *ret_گرێ  = f_چەپ_Child->clone_درەخت(status, depth+1);
        إذا (U_FAILURE(status)) {
            أرجع هذا;
        }
        ret_گرێ->f_Rule_ڕەگ = هذا->f_Rule_ڕەگ;
        ret_گرێ->fChainIn  = هذا->fChainIn;
        delete هذا;   // TODO: undefined behavior. Fix.
        أرجع ret_گرێ;
    }

    إذا (f_چەپ_Child != nullptr) {
        f_چەپ_Child = f_چەپ_Child->flattenVariables(status, depth+1);
        إذا (f_چەپ_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        إذا (U_FAILURE(status)) {
            أرجع هذا;
        }
        f_چەپ_Child->fParent  = هذا;
    }
    إذا (f_ڕاست_Child != nullptr) {
        f_ڕاست_Child = f_ڕاست_Child->flattenVariables(status, depth+1);
        إذا (f_ڕاست_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        إذا (U_FAILURE(status)) {
            أرجع هذا;
        }
        f_ڕاست_Child->fParent = هذا;
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
        status = U_تێخستن_TOO_LONG_ERROR;
        أرجع;
    }
    U_ASSERT(fType != setRef);

    إذا (f_چەپ_Child != nullptr) {
        إذا (f_چەپ_Child->fType==setRef) {
            RBBINode *set_Ref_گرێ = f_چەپ_Child;
            RBBINode *uset_گرێ   = set_Ref_گرێ->f_چەپ_Child;
            RBBINode *repl_درەخت   = uset_گرێ->f_چەپ_Child;
            f_چەپ_Child           = repl_درەخت->clone_درەخت(status, depth+1);
            إذا (U_FAILURE(status)) {
                delete set_Ref_گرێ;
                أرجع;
            }
            f_چەپ_Child->fParent  = هذا;
            delete set_Ref_گرێ;
        } وإلا {
            f_چەپ_Child->flattenSets(status, depth+1);
        }
    }

    إذا (f_ڕاست_Child != nullptr) {
        إذا (f_ڕاست_Child->fType==setRef) {
            RBBINode *set_Ref_گرێ = f_ڕاست_Child;
            RBBINode *uset_گرێ   = set_Ref_گرێ->f_چەپ_Child;
            RBBINode *repl_درەخت   = uset_گرێ->f_چەپ_Child;
            f_ڕاست_Child           = repl_درەخت->clone_درەخت(status, depth+1);
            إذا (U_FAILURE(status)) {
                delete set_Ref_گرێ;
                أرجع;
            }
            f_ڕاست_Child->fParent  = هذا;
            delete set_Ref_گرێ;
        } وإلا {
            f_ڕاست_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
فراغ   RBBINode::findNodes(UVector *dest, RBBINode::گرێ_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    U_ASSERT(!dest->hasDeleter());
    إذا (fType == kind) {
        dest->addElement(هذا, status);
    }
    إذا (f_چەپ_Child != nullptr) {
        f_چەپ_Child->findNodes(dest, kind, status);
    }
    إذا (f_ڕاست_Child != nullptr) {
        f_ڕاست_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

ساكن int32_t serial(ثابت RBBINode *گرێ) {
    أرجع (گرێ == nullptr? -1 : گرێ->f_Serial_ژمارە);
}


فراغ RBBINode::print_گرێ(ثابت RBBINode *گرێ) {
    ساكن ثابت حرف * ثابت گرێ_Type_Names[] = {
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

    إذا (گرێ==nullptr) {
        RBBIDebugPrintf("%10p", (فراغ *)گرێ);
    } وإلا {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (فراغ *)گرێ, گرێ->f_Serial_ژمارە, گرێ_Type_Names[گرێ->fType],
            گرێ->f_Rule_ڕەگ?'R':' ', گرێ->fChainIn?'C':' ',
            serial(گرێ->f_چەپ_Child), serial(گرێ->f_ڕاست_Child), serial(گرێ->fParent),
            گرێ->fFirstPos, گرێ->fVal);
        إذا (گرێ->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(گرێ->fText);
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
فراغ RBBINode::print_گرێ_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
فراغ RBBINode::print_درەخت(ثابت RBBINode *گرێ, UBool printHeading) {
    إذا (printHeading) {
        print_گرێ_Header();
    }
    print_گرێ(گرێ);
    إذا (گرێ != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        إذا (گرێ->fType != varRef) {
            إذا (گرێ->f_چەپ_Child != nullptr) {
                print_درەخت(گرێ->f_چەپ_Child, خطأ);
            }
            
            إذا (گرێ->f_ڕاست_Child != nullptr) {
                print_درەخت(گرێ->f_ڕاست_Child, خطأ);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
