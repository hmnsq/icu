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

#که_تعریف_شوی RBBI_DEBUG
ساكن صحيح  g_وروستی_Serial = 0;
#د_که_پای


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(غوټه_Type t, UError_کوډ& حالت) : UMemory() {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
#که_تعریف_شوی RBBI_DEBUG
    f_Serial_عدد    = ++g_وروستی_Serial;
#د_که_پای
    fType         = t;
    f_پلار       = تش_اشاره;
    f_کیڼ_ماشوم    = تش_اشاره;
    f_ښی_ماشوم   = تش_اشاره;
    f_ننوت_سېټ     = تش_اشاره;
    f_لومړی_Pos     = 0;
    f_وروستی_Pos      = 0;
    fNullable     = خطأ;
    f_Look_Ahead_پای_ټکی = خطأ;
    f_قاعده_ریښه     = خطأ;
    f_Chain_دننه      = خطأ;
    fVal          = 0;
    fPrecedence   = precZero;

    f_لومړی_Pos_سېټ  = جديد UVector(حالت);
    f_وروستی_Pos_سېټ   = جديد UVector(حالت);
    fFollowPos    = جديد UVector(حالت);
    إذا (U_SUCCESS(حالت) &&
        (f_لومړی_Pos_سېټ == تش_اشاره || f_وروستی_Pos_سېټ == تش_اشاره || fFollowPos == تش_اشاره)) {
        حالت =  U_حافظه_ویش_ERROR;
    }
    إذا      (t==opCat)    {fPrecedence = precOpCat;}
    وإلا إذا (t==opOr)     {fPrecedence = precOpOr;}
    وإلا إذا (t==op_پیل)  {fPrecedence = prec_پیل;}
    وإلا إذا (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ثابت RBBINode &other, UError_کوډ& حالت) : UMemory(other) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
#که_تعریف_شوی RBBI_DEBUG
    f_Serial_عدد   = ++g_وروستی_Serial;
#د_که_پای
    fType        = other.fType;
    f_پلار      = تش_اشاره;
    f_کیڼ_ماشوم   = تش_اشاره;
    f_ښی_ماشوم  = تش_اشاره;
    f_ننوت_سېټ    = other.f_ننوت_سېټ;
    fPrecedence  = other.fPrecedence;
    f_متن        = other.f_متن;
    f_لومړی_Pos    = other.f_لومړی_Pos;
    f_وروستی_Pos     = other.f_وروستی_Pos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_قاعده_ریښه    = خطأ;
    f_Chain_دننه     = other.f_Chain_دننه;
    f_لومړی_Pos_سېټ = جديد UVector(حالت);   // TODO - get a real status from somewhere
    f_وروستی_Pos_سېټ  = جديد UVector(حالت);
    fFollowPos   = جديد UVector(حالت);
    إذا (U_SUCCESS(حالت) &&
        (f_لومړی_Pos_سېټ == تش_اشاره || f_وروستی_Pos_سېټ == تش_اشاره || fFollowPos == تش_اشاره)) {
        حالت =  U_حافظه_ویش_ERROR;
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
    ړنګول f_ننوت_سېټ;
    f_ننوت_سېټ = تش_اشاره;

    بدل (هذا->fType) {
    حالة varRef:
    حالة سېټ_Ref:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        اكسر;

    افتراضي:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_غوټه(f_کیڼ_ماشوم);
        f_کیڼ_ماشوم =   تش_اشاره;
        // delete        fRightChild;
        NRDelete_غوټه(f_ښی_ماشوم);
        f_ښی_ماشوم = تش_اشاره;
    }

    ړنګول f_لومړی_Pos_سېټ;
    ړنګول f_وروستی_Pos_سېټ;
    ړنګول fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
فراغ RBBINode::NRDelete_غوټه(RBBINode *غوټه) {
    إذا (غوټه == تش_اشاره) {
        أرجع;
    }

    RBBINode *stop_غوټه = غوټه->f_پلار;
    RBBINode *بل_غوټه = غوټه;
    طالما (بل_غوټه != stop_غوټه && بل_غوټه != تش_اشاره) {
        RBBINode *current_غوټه = بل_غوټه;

        إذا ((current_غوټه->f_کیڼ_ماشوم == تش_اشاره && current_غوټه->f_ښی_ماشوم == تش_اشاره) ||
                current_غوټه->fType == varRef ||      // varRef and setRef nodes do not
                current_غوټه->fType == سېټ_Ref) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            بل_غوټه = current_غوټه->f_پلار;
            إذا (بل_غوټه) {
                إذا (بل_غوټه->f_کیڼ_ماشوم == current_غوټه) {
                    بل_غوټه->f_کیڼ_ماشوم = تش_اشاره;
                } وإلا إذا (بل_غوټه->f_ښی_ماشوم == current_غوټه) {
                    بل_غوټه->f_ښی_ماشوم = تش_اشاره;
                }
            }
            ړنګول current_غوټه;
        } وإلا إذا (current_غوټه->f_کیڼ_ماشوم) {
            بل_غوټه = current_غوټه->f_کیڼ_ماشوم;
            إذا (بل_غوټه->f_پلار == تش_اشاره) {
                بل_غوټه->f_پلار = current_غوټه;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(بل_غوټه->f_پلار == current_غوټه);
        } وإلا إذا (current_غوټه->f_ښی_ماشوم) {
            بل_غوټه = current_غوټه->f_ښی_ماشوم;
            إذا (بل_غوټه->f_پلار == تش_اشاره) {
                بل_غوټه->f_پلار = current_غوټه;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(بل_غوټه->f_پلار == current_غوټه);
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
constexpr صحيح k_Recursive_ژوروالی_حد = 3500;
RBBINode *RBBINode::clone_ونه(UError_کوډ &حالت, صحيح ژوروالی) {
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (ژوروالی > k_Recursive_ژوروالی_حد) {
        حالت = U_ننوت_TOO_LONG_ERROR;
        أرجع تش_اشاره;
    }
    RBBINode    *n;

    إذا (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_کیڼ_ماشوم->clone_ونه(حالت, ژوروالی+1);
        إذا (U_FAILURE(حالت)) {
            أرجع تش_اشاره;
        }
    } وإلا إذا (fType == RBBINode::uset) {
        n = هذا;
    } وإلا {
        n = جديد RBBINode(*هذا, حالت);
        إذا (U_FAILURE(حالت)) {
            ړنګول n;
            أرجع تش_اشاره;
        }
        // Check for null pointer.
        إذا (n == تش_اشاره) {
            حالت =  U_حافظه_ویش_ERROR;
            أرجع تش_اشاره;
        }
        إذا (f_کیڼ_ماشوم != تش_اشاره) {
            n->f_کیڼ_ماشوم          = f_کیڼ_ماشوم->clone_ونه(حالت, ژوروالی+1);
            إذا (U_FAILURE(حالت)) {
                ړنګول n;
                أرجع تش_اشاره;
            }
            n->f_کیڼ_ماشوم->f_پلار = n;
        }
        إذا (f_ښی_ماشوم != تش_اشاره) {
            n->f_ښی_ماشوم          = f_ښی_ماشوم->clone_ونه(حالت, ژوروالی+1);
            إذا (U_FAILURE(حالت)) {
                ړنګول n;
                أرجع تش_اشاره;
            }
            n->f_ښی_ماشوم->f_پلار = n;
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
RBBINode *RBBINode::flattenVariables(UError_کوډ& حالت, صحيح ژوروالی) {
    إذا (U_FAILURE(حالت)) {
        أرجع هذا;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (ژوروالی > k_Recursive_ژوروالی_حد) {
        حالت = U_ننوت_TOO_LONG_ERROR;
        أرجع هذا;
    }
    إذا (fType == varRef) {
        RBBINode *ret_غوټه  = f_کیڼ_ماشوم->clone_ونه(حالت, ژوروالی+1);
        إذا (U_FAILURE(حالت)) {
            أرجع هذا;
        }
        ret_غوټه->f_قاعده_ریښه = هذا->f_قاعده_ریښه;
        ret_غوټه->f_Chain_دننه  = هذا->f_Chain_دننه;
        ړنګول هذا;   // TODO: undefined behavior. Fix.
        أرجع ret_غوټه;
    }

    إذا (f_کیڼ_ماشوم != تش_اشاره) {
        f_کیڼ_ماشوم = f_کیڼ_ماشوم->flattenVariables(حالت, ژوروالی+1);
        إذا (f_کیڼ_ماشوم == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
        }
        إذا (U_FAILURE(حالت)) {
            أرجع هذا;
        }
        f_کیڼ_ماشوم->f_پلار  = هذا;
    }
    إذا (f_ښی_ماشوم != تش_اشاره) {
        f_ښی_ماشوم = f_ښی_ماشوم->flattenVariables(حالت, ژوروالی+1);
        إذا (f_ښی_ماشوم == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
        }
        إذا (U_FAILURE(حالت)) {
            أرجع هذا;
        }
        f_ښی_ماشوم->f_پلار = هذا;
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
فراغ RBBINode::flattenSets(UError_کوډ &حالت, صحيح ژوروالی) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (ژوروالی > k_Recursive_ژوروالی_حد) {
        حالت = U_ننوت_TOO_LONG_ERROR;
        أرجع;
    }
    U_ASSERT(fType != سېټ_Ref);

    إذا (f_کیڼ_ماشوم != تش_اشاره) {
        إذا (f_کیڼ_ماشوم->fType==سېټ_Ref) {
            RBBINode *سېټ_Ref_غوټه = f_کیڼ_ماشوم;
            RBBINode *uset_غوټه   = سېټ_Ref_غوټه->f_کیڼ_ماشوم;
            RBBINode *repl_ونه   = uset_غوټه->f_کیڼ_ماشوم;
            f_کیڼ_ماشوم           = repl_ونه->clone_ونه(حالت, ژوروالی+1);
            إذا (U_FAILURE(حالت)) {
                ړنګول سېټ_Ref_غوټه;
                أرجع;
            }
            f_کیڼ_ماشوم->f_پلار  = هذا;
            ړنګول سېټ_Ref_غوټه;
        } وإلا {
            f_کیڼ_ماشوم->flattenSets(حالت, ژوروالی+1);
        }
    }

    إذا (f_ښی_ماشوم != تش_اشاره) {
        إذا (f_ښی_ماشوم->fType==سېټ_Ref) {
            RBBINode *سېټ_Ref_غوټه = f_ښی_ماشوم;
            RBBINode *uset_غوټه   = سېټ_Ref_غوټه->f_کیڼ_ماشوم;
            RBBINode *repl_ونه   = uset_غوټه->f_کیڼ_ماشوم;
            f_ښی_ماشوم           = repl_ونه->clone_ونه(حالت, ژوروالی+1);
            إذا (U_FAILURE(حالت)) {
                ړنګول سېټ_Ref_غوټه;
                أرجع;
            }
            f_ښی_ماشوم->f_پلار  = هذا;
            ړنګول سېټ_Ref_غوټه;
        } وإلا {
            f_ښی_ماشوم->flattenSets(حالت, ژوروالی+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
فراغ   RBBINode::پیدا_کړه_Nodes(UVector *dest, RBBINode::غوټه_Type ډول, UError_کوډ &حالت) {
    /* test for buffer overflows */
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    U_ASSERT(!dest->hasDeleter());
    إذا (fType == ډول) {
        dest->جمع_کړه_Element(هذا, حالت);
    }
    إذا (f_کیڼ_ماشوم != تش_اشاره) {
        f_کیڼ_ماشوم->پیدا_کړه_Nodes(dest, ډول, حالت);
    }
    إذا (f_ښی_ماشوم != تش_اشاره) {
        f_ښی_ماشوم->پیدا_کړه_Nodes(dest, ډول, حالت);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#که_تعریف_شوی RBBI_DEBUG

ساكن int32_t serial(ثابت RBBINode *غوټه) {
    أرجع (غوټه == تش_اشاره? -1 : غوټه->f_Serial_عدد);
}


فراغ RBBINode::print_غوټه(ثابت RBBINode *غوټه) {
    ساكن ثابت حرف * ثابت غوټه_Type_نومونه[] = {
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

    إذا (غوټه==تش_اشاره) {
        RBBIDebugPrintf("%10p", (فراغ *)غوټه);
    } وإلا {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (فراغ *)غوټه, غوټه->f_Serial_عدد, غوټه_Type_نومونه[غوټه->fType],
            غوټه->f_قاعده_ریښه?'R':' ', غوټه->f_Chain_دننه?'C':' ',
            serial(غوټه->f_کیڼ_ماشوم), serial(غوټه->f_ښی_ماشوم), serial(غوټه->f_پلار),
            غوټه->f_لومړی_Pos, غوټه->fVal);
        إذا (غوټه->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(غوټه->f_متن);
        }
    }
    RBBIDebugPrintf("\n");
}
#د_که_پای


#که_تعریف_شوی RBBI_DEBUG
U_CFUNC فراغ RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحيح min_پلنوالی) {
    RBBIDebugPrintf("%*s", min_پلنوالی, CStr(s)());
}
#د_که_پای


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#که_تعریف_شوی RBBI_DEBUG
فراغ RBBINode::print_غوټه_سرلیک() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
فراغ RBBINode::print_ونه(ثابت RBBINode *غوټه, UBool printHeading) {
    إذا (printHeading) {
        print_غوټه_سرلیک();
    }
    print_غوټه(غوټه);
    إذا (غوټه != تش_اشاره) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        إذا (غوټه->fType != varRef) {
            إذا (غوټه->f_کیڼ_ماشوم != تش_اشاره) {
                print_ونه(غوټه->f_کیڼ_ماشوم, خطأ);
            }
            
            إذا (غوټه->f_ښی_ماشوم != تش_اشاره) {
                print_ونه(غوټه->f_ښی_ماشوم, خطأ);
            }
        }
    }
}
#د_که_پای



U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */
