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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/unistr.h"
#شامل "unicode/uniset.h"
#شامل "unicode/uchar.h"
#شامل "unicode/parsepos.h"

#شامل "cstr.h"
#شامل "uvector.h"

#شامل "rbbirb.h"
#شامل "rbbinode.h"

#شامل "uassert.h"


U_NAMESPACE_BEGIN

#ifdef RBBI_DEBUG
ایستا صحیح  gLastSerial = 0;
#endif


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(گره_Type t, UErrorCode& status) : UMemory() {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
#ifdef RBBI_DEBUG
    f_Serial_رقم    = ++gLastSerial;
#endif
    fType         = t;
    fParent       = nullptr;
    f_چپ_Child    = nullptr;
    f_راست_Child   = nullptr;
    f_درون_داد_Set     = nullptr;
    fFirstPos     = 0;
    fLastPos      = 0;
    fNullable     = نادرست;
    fLookAheadEnd = نادرست;
    f_Rule_ریشه     = نادرست;
    fChainIn      = نادرست;
    fVal          = 0;
    fPrecedence   = precZero;

    fFirstPosSet  = جدید UVector(status);
    fLastPosSet   = جدید UVector(status);
    fFollowPos    = جدید UVector(status);
    اگر (U_SUCCESS(status) &&
        (fFirstPosSet == nullptr || fLastPosSet == nullptr || fFollowPos == nullptr)) {
        status =  U_MEMORY_ALLOCATION_ERROR;
    }
    اگر      (t==opCat)    {fPrecedence = precOpCat;}
    وگرنه اگر (t==opOr)     {fPrecedence = precOpOr;}
    وگرنه اگر (t==opStart)  {fPrecedence = precStart;}
    وگرنه اگر (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ثابت RBBINode &other, UErrorCode& status) : UMemory(other) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
#ifdef RBBI_DEBUG
    f_Serial_رقم   = ++gLastSerial;
#endif
    fType        = other.fType;
    fParent      = nullptr;
    f_چپ_Child   = nullptr;
    f_راست_Child  = nullptr;
    f_درون_داد_Set    = other.f_درون_داد_Set;
    fPrecedence  = other.fPrecedence;
    fText        = other.fText;
    fFirstPos    = other.fFirstPos;
    fLastPos     = other.fLastPos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_Rule_ریشه    = نادرست;
    fChainIn     = other.fChainIn;
    fFirstPosSet = جدید UVector(status);   // TODO - get a real status from somewhere
    fLastPosSet  = جدید UVector(status);
    fFollowPos   = جدید UVector(status);
    اگر (U_SUCCESS(status) &&
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
    delete f_درون_داد_Set;
    f_درون_داد_Set = nullptr;

    انتخاب (این->fType) {
    حالت varRef:
    حالت setRef:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        بشکن;

    پیشفرض:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_گره(f_چپ_Child);
        f_چپ_Child =   nullptr;
        // delete        fRightChild;
        NRDelete_گره(f_راست_Child);
        f_راست_Child = nullptr;
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
تهی RBBINode::NRDelete_گره(RBBINode *گره) {
    اگر (گره == nullptr) {
        بازگشت;
    }

    RBBINode *stop_گره = گره->fParent;
    RBBINode *next_گره = گره;
    وقتی (next_گره != stop_گره && next_گره != nullptr) {
        RBBINode *current_گره = next_گره;

        اگر ((current_گره->f_چپ_Child == nullptr && current_گره->f_راست_Child == nullptr) ||
                current_گره->fType == varRef ||      // varRef and setRef nodes do not
                current_گره->fType == setRef) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            next_گره = current_گره->fParent;
            اگر (next_گره) {
                اگر (next_گره->f_چپ_Child == current_گره) {
                    next_گره->f_چپ_Child = nullptr;
                } وگرنه اگر (next_گره->f_راست_Child == current_گره) {
                    next_گره->f_راست_Child = nullptr;
                }
            }
            delete current_گره;
        } وگرنه اگر (current_گره->f_چپ_Child) {
            next_گره = current_گره->f_چپ_Child;
            اگر (next_گره->fParent == nullptr) {
                next_گره->fParent = current_گره;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_گره->fParent == current_گره);
        } وگرنه اگر (current_گره->f_راست_Child) {
            next_گره = current_گره->f_راست_Child;
            اگر (next_گره->fParent == nullptr) {
                next_گره->fParent = current_گره;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(next_گره->fParent == current_گره);
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
constexpr صحیح kRecursiveDepthLimit = 3500;
RBBINode *RBBINode::clone_درخت(UErrorCode &status, صحیح depth) {
    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    اگر (depth > kRecursiveDepthLimit) {
        status = U_درون_داد_TOO_LONG_ERROR;
        بازگشت nullptr;
    }
    RBBINode    *n;

    اگر (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_چپ_Child->clone_درخت(status, depth+1);
        اگر (U_FAILURE(status)) {
            بازگشت nullptr;
        }
    } وگرنه اگر (fType == RBBINode::uset) {
        n = این;
    } وگرنه {
        n = جدید RBBINode(*این, status);
        اگر (U_FAILURE(status)) {
            delete n;
            بازگشت nullptr;
        }
        // Check for null pointer.
        اگر (n == nullptr) {
            status =  U_MEMORY_ALLOCATION_ERROR;
            بازگشت nullptr;
        }
        اگر (f_چپ_Child != nullptr) {
            n->f_چپ_Child          = f_چپ_Child->clone_درخت(status, depth+1);
            اگر (U_FAILURE(status)) {
                delete n;
                بازگشت nullptr;
            }
            n->f_چپ_Child->fParent = n;
        }
        اگر (f_راست_Child != nullptr) {
            n->f_راست_Child          = f_راست_Child->clone_درخت(status, depth+1);
            اگر (U_FAILURE(status)) {
                delete n;
                بازگشت nullptr;
            }
            n->f_راست_Child->fParent = n;
        }
    }
    بازگشت n;
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
RBBINode *RBBINode::flattenVariables(UErrorCode& status, صحیح depth) {
    اگر (U_FAILURE(status)) {
        بازگشت این;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    اگر (depth > kRecursiveDepthLimit) {
        status = U_درون_داد_TOO_LONG_ERROR;
        بازگشت این;
    }
    اگر (fType == varRef) {
        RBBINode *ret_گره  = f_چپ_Child->clone_درخت(status, depth+1);
        اگر (U_FAILURE(status)) {
            بازگشت این;
        }
        ret_گره->f_Rule_ریشه = این->f_Rule_ریشه;
        ret_گره->fChainIn  = این->fChainIn;
        delete این;   // TODO: undefined behavior. Fix.
        بازگشت ret_گره;
    }

    اگر (f_چپ_Child != nullptr) {
        f_چپ_Child = f_چپ_Child->flattenVariables(status, depth+1);
        اگر (f_چپ_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        اگر (U_FAILURE(status)) {
            بازگشت این;
        }
        f_چپ_Child->fParent  = این;
    }
    اگر (f_راست_Child != nullptr) {
        f_راست_Child = f_راست_Child->flattenVariables(status, depth+1);
        اگر (f_راست_Child == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        اگر (U_FAILURE(status)) {
            بازگشت این;
        }
        f_راست_Child->fParent = این;
    }
    بازگشت این;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
تهی RBBINode::flattenSets(UErrorCode &status, صحیح depth) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    اگر (depth > kRecursiveDepthLimit) {
        status = U_درون_داد_TOO_LONG_ERROR;
        بازگشت;
    }
    U_ASSERT(fType != setRef);

    اگر (f_چپ_Child != nullptr) {
        اگر (f_چپ_Child->fType==setRef) {
            RBBINode *set_Ref_گره = f_چپ_Child;
            RBBINode *uset_گره   = set_Ref_گره->f_چپ_Child;
            RBBINode *repl_درخت   = uset_گره->f_چپ_Child;
            f_چپ_Child           = repl_درخت->clone_درخت(status, depth+1);
            اگر (U_FAILURE(status)) {
                delete set_Ref_گره;
                بازگشت;
            }
            f_چپ_Child->fParent  = این;
            delete set_Ref_گره;
        } وگرنه {
            f_چپ_Child->flattenSets(status, depth+1);
        }
    }

    اگر (f_راست_Child != nullptr) {
        اگر (f_راست_Child->fType==setRef) {
            RBBINode *set_Ref_گره = f_راست_Child;
            RBBINode *uset_گره   = set_Ref_گره->f_چپ_Child;
            RBBINode *repl_درخت   = uset_گره->f_چپ_Child;
            f_راست_Child           = repl_درخت->clone_درخت(status, depth+1);
            اگر (U_FAILURE(status)) {
                delete set_Ref_گره;
                بازگشت;
            }
            f_راست_Child->fParent  = این;
            delete set_Ref_گره;
        } وگرنه {
            f_راست_Child->flattenSets(status, depth+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
تهی   RBBINode::findNodes(UVector *dest, RBBINode::گره_Type kind, UErrorCode &status) {
    /* test for buffer overflows */
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    U_ASSERT(!dest->hasDeleter());
    اگر (fType == kind) {
        dest->addElement(این, status);
    }
    اگر (f_چپ_Child != nullptr) {
        f_چپ_Child->findNodes(dest, kind, status);
    }
    اگر (f_راست_Child != nullptr) {
        f_راست_Child->findNodes(dest, kind, status);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG

ایستا int32_t serial(ثابت RBBINode *گره) {
    بازگشت (گره == nullptr? -1 : گره->f_Serial_رقم);
}


تهی RBBINode::print_گره(ثابت RBBINode *گره) {
    ایستا ثابت نویسه * ثابت گره_Type_Names[] = {
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

    اگر (گره==nullptr) {
        RBBIDebugPrintf("%10p", (تهی *)گره);
    } وگرنه {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (تهی *)گره, گره->f_Serial_رقم, گره_Type_Names[گره->fType],
            گره->f_Rule_ریشه?'R':' ', گره->fChainIn?'C':' ',
            serial(گره->f_چپ_Child), serial(گره->f_راست_Child), serial(گره->fParent),
            گره->fFirstPos, گره->fVal);
        اگر (گره->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(گره->fText);
        }
    }
    RBBIDebugPrintf("\n");
}
#endif


#ifdef RBBI_DEBUG
U_CFUNC تهی RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحیح minWidth) {
    RBBIDebugPrintf("%*s", minWidth, CStr(s)());
}
#endif


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#ifdef RBBI_DEBUG
تهی RBBINode::print_گره_Header() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
تهی RBBINode::print_درخت(ثابت RBBINode *گره, UBool printHeading) {
    اگر (printHeading) {
        print_گره_Header();
    }
    print_گره(گره);
    اگر (گره != nullptr) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        اگر (گره->fType != varRef) {
            اگر (گره->f_چپ_Child != nullptr) {
                print_درخت(گره->f_چپ_Child, نادرست);
            }
            
            اگر (گره->f_راست_Child != nullptr) {
                print_درخت(گره->f_راست_Child, نادرست);
            }
        }
    }
}
#endif



U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
