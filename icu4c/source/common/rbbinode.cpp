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

#اگر_تعریف_شده RBBI_DEBUG
ساكن صحيح  g_آخرین_Serial = 0;
#پایان_اگر


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(گره_Type t, UError_کد& وضعیت) : UMemory() {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
#اگر_تعریف_شده RBBI_DEBUG
    f_Serial_رقم    = ++g_آخرین_Serial;
#پایان_اگر
    fType         = t;
    f_والد       = اشاره_تهی;
    f_چپ_فرزند    = اشاره_تهی;
    f_راست_فرزند   = اشاره_تهی;
    f_درون_داد_مجموعه     = اشاره_تهی;
    f_اولی_Pos     = 0;
    f_آخرین_Pos      = 0;
    fNullable     = خطأ;
    f_Look_Ahead_انتها = خطأ;
    f_قاعده_ریشه     = خطأ;
    f_Chain_درون      = خطأ;
    fVal          = 0;
    fPrecedence   = precZero;

    f_اولی_Pos_مجموعه  = جديد UVector(وضعیت);
    f_آخرین_Pos_مجموعه   = جديد UVector(وضعیت);
    fFollowPos    = جديد UVector(وضعیت);
    إذا (U_SUCCESS(وضعیت) &&
        (f_اولی_Pos_مجموعه == اشاره_تهی || f_آخرین_Pos_مجموعه == اشاره_تهی || fFollowPos == اشاره_تهی)) {
        وضعیت =  U_حافظه_تخصیص_ERROR;
    }
    إذا      (t==opCat)    {fPrecedence = precOpCat;}
    وإلا إذا (t==opOr)     {fPrecedence = precOpOr;}
    وإلا إذا (t==op_شروع)  {fPrecedence = prec_شروع;}
    وإلا إذا (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ثابت RBBINode &other, UError_کد& وضعیت) : UMemory(other) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
#اگر_تعریف_شده RBBI_DEBUG
    f_Serial_رقم   = ++g_آخرین_Serial;
#پایان_اگر
    fType        = other.fType;
    f_والد      = اشاره_تهی;
    f_چپ_فرزند   = اشاره_تهی;
    f_راست_فرزند  = اشاره_تهی;
    f_درون_داد_مجموعه    = other.f_درون_داد_مجموعه;
    fPrecedence  = other.fPrecedence;
    f_متن        = other.f_متن;
    f_اولی_Pos    = other.f_اولی_Pos;
    f_آخرین_Pos     = other.f_آخرین_Pos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_قاعده_ریشه    = خطأ;
    f_Chain_درون     = other.f_Chain_درون;
    f_اولی_Pos_مجموعه = جديد UVector(وضعیت);   // TODO - get a real status from somewhere
    f_آخرین_Pos_مجموعه  = جديد UVector(وضعیت);
    fFollowPos   = جديد UVector(وضعیت);
    إذا (U_SUCCESS(وضعیت) &&
        (f_اولی_Pos_مجموعه == اشاره_تهی || f_آخرین_Pos_مجموعه == اشاره_تهی || fFollowPos == اشاره_تهی)) {
        وضعیت =  U_حافظه_تخصیص_ERROR;
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
    حذف f_درون_داد_مجموعه;
    f_درون_داد_مجموعه = اشاره_تهی;

    بدل (هذا->fType) {
    حالة varRef:
    حالة مجموعه_Ref:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        اكسر;

    افتراضي:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_گره(f_چپ_فرزند);
        f_چپ_فرزند =   اشاره_تهی;
        // delete        fRightChild;
        NRDelete_گره(f_راست_فرزند);
        f_راست_فرزند = اشاره_تهی;
    }

    حذف f_اولی_Pos_مجموعه;
    حذف f_آخرین_Pos_مجموعه;
    حذف fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
فراغ RBBINode::NRDelete_گره(RBBINode *گره) {
    إذا (گره == اشاره_تهی) {
        أرجع;
    }

    RBBINode *stop_گره = گره->f_والد;
    RBBINode *بعدی_گره = گره;
    طالما (بعدی_گره != stop_گره && بعدی_گره != اشاره_تهی) {
        RBBINode *current_گره = بعدی_گره;

        إذا ((current_گره->f_چپ_فرزند == اشاره_تهی && current_گره->f_راست_فرزند == اشاره_تهی) ||
                current_گره->fType == varRef ||      // varRef and setRef nodes do not
                current_گره->fType == مجموعه_Ref) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            بعدی_گره = current_گره->f_والد;
            إذا (بعدی_گره) {
                إذا (بعدی_گره->f_چپ_فرزند == current_گره) {
                    بعدی_گره->f_چپ_فرزند = اشاره_تهی;
                } وإلا إذا (بعدی_گره->f_راست_فرزند == current_گره) {
                    بعدی_گره->f_راست_فرزند = اشاره_تهی;
                }
            }
            حذف current_گره;
        } وإلا إذا (current_گره->f_چپ_فرزند) {
            بعدی_گره = current_گره->f_چپ_فرزند;
            إذا (بعدی_گره->f_والد == اشاره_تهی) {
                بعدی_گره->f_والد = current_گره;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(بعدی_گره->f_والد == current_گره);
        } وإلا إذا (current_گره->f_راست_فرزند) {
            بعدی_گره = current_گره->f_راست_فرزند;
            إذا (بعدی_گره->f_والد == اشاره_تهی) {
                بعدی_گره->f_والد = current_گره;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(بعدی_گره->f_والد == current_گره);
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
constexpr صحيح k_Recursive_عمق_حد = 3500;
RBBINode *RBBINode::clone_درخت(UError_کد &وضعیت, صحيح عمق) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (عمق > k_Recursive_عمق_حد) {
        وضعیت = U_درون_داد_TOO_LONG_ERROR;
        أرجع اشاره_تهی;
    }
    RBBINode    *n;

    إذا (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_چپ_فرزند->clone_درخت(وضعیت, عمق+1);
        إذا (U_FAILURE(وضعیت)) {
            أرجع اشاره_تهی;
        }
    } وإلا إذا (fType == RBBINode::uset) {
        n = هذا;
    } وإلا {
        n = جديد RBBINode(*هذا, وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            حذف n;
            أرجع اشاره_تهی;
        }
        // Check for null pointer.
        إذا (n == اشاره_تهی) {
            وضعیت =  U_حافظه_تخصیص_ERROR;
            أرجع اشاره_تهی;
        }
        إذا (f_چپ_فرزند != اشاره_تهی) {
            n->f_چپ_فرزند          = f_چپ_فرزند->clone_درخت(وضعیت, عمق+1);
            إذا (U_FAILURE(وضعیت)) {
                حذف n;
                أرجع اشاره_تهی;
            }
            n->f_چپ_فرزند->f_والد = n;
        }
        إذا (f_راست_فرزند != اشاره_تهی) {
            n->f_راست_فرزند          = f_راست_فرزند->clone_درخت(وضعیت, عمق+1);
            إذا (U_FAILURE(وضعیت)) {
                حذف n;
                أرجع اشاره_تهی;
            }
            n->f_راست_فرزند->f_والد = n;
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
RBBINode *RBBINode::flattenVariables(UError_کد& وضعیت, صحيح عمق) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع هذا;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (عمق > k_Recursive_عمق_حد) {
        وضعیت = U_درون_داد_TOO_LONG_ERROR;
        أرجع هذا;
    }
    إذا (fType == varRef) {
        RBBINode *ret_گره  = f_چپ_فرزند->clone_درخت(وضعیت, عمق+1);
        إذا (U_FAILURE(وضعیت)) {
            أرجع هذا;
        }
        ret_گره->f_قاعده_ریشه = هذا->f_قاعده_ریشه;
        ret_گره->f_Chain_درون  = هذا->f_Chain_درون;
        حذف هذا;   // TODO: undefined behavior. Fix.
        أرجع ret_گره;
    }

    إذا (f_چپ_فرزند != اشاره_تهی) {
        f_چپ_فرزند = f_چپ_فرزند->flattenVariables(وضعیت, عمق+1);
        إذا (f_چپ_فرزند == اشاره_تهی) {
            وضعیت = U_حافظه_تخصیص_ERROR;
        }
        إذا (U_FAILURE(وضعیت)) {
            أرجع هذا;
        }
        f_چپ_فرزند->f_والد  = هذا;
    }
    إذا (f_راست_فرزند != اشاره_تهی) {
        f_راست_فرزند = f_راست_فرزند->flattenVariables(وضعیت, عمق+1);
        إذا (f_راست_فرزند == اشاره_تهی) {
            وضعیت = U_حافظه_تخصیص_ERROR;
        }
        إذا (U_FAILURE(وضعیت)) {
            أرجع هذا;
        }
        f_راست_فرزند->f_والد = هذا;
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
فراغ RBBINode::flattenSets(UError_کد &وضعیت, صحيح عمق) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (عمق > k_Recursive_عمق_حد) {
        وضعیت = U_درون_داد_TOO_LONG_ERROR;
        أرجع;
    }
    U_ASSERT(fType != مجموعه_Ref);

    إذا (f_چپ_فرزند != اشاره_تهی) {
        إذا (f_چپ_فرزند->fType==مجموعه_Ref) {
            RBBINode *مجموعه_Ref_گره = f_چپ_فرزند;
            RBBINode *uset_گره   = مجموعه_Ref_گره->f_چپ_فرزند;
            RBBINode *repl_درخت   = uset_گره->f_چپ_فرزند;
            f_چپ_فرزند           = repl_درخت->clone_درخت(وضعیت, عمق+1);
            إذا (U_FAILURE(وضعیت)) {
                حذف مجموعه_Ref_گره;
                أرجع;
            }
            f_چپ_فرزند->f_والد  = هذا;
            حذف مجموعه_Ref_گره;
        } وإلا {
            f_چپ_فرزند->flattenSets(وضعیت, عمق+1);
        }
    }

    إذا (f_راست_فرزند != اشاره_تهی) {
        إذا (f_راست_فرزند->fType==مجموعه_Ref) {
            RBBINode *مجموعه_Ref_گره = f_راست_فرزند;
            RBBINode *uset_گره   = مجموعه_Ref_گره->f_چپ_فرزند;
            RBBINode *repl_درخت   = uset_گره->f_چپ_فرزند;
            f_راست_فرزند           = repl_درخت->clone_درخت(وضعیت, عمق+1);
            إذا (U_FAILURE(وضعیت)) {
                حذف مجموعه_Ref_گره;
                أرجع;
            }
            f_راست_فرزند->f_والد  = هذا;
            حذف مجموعه_Ref_گره;
        } وإلا {
            f_راست_فرزند->flattenSets(وضعیت, عمق+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
فراغ   RBBINode::پیدا_کن_Nodes(UVector *dest, RBBINode::گره_Type نوع, UError_کد &وضعیت) {
    /* test for buffer overflows */
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    U_ASSERT(!dest->hasDeleter());
    إذا (fType == نوع) {
        dest->جمع_کن_Element(هذا, وضعیت);
    }
    إذا (f_چپ_فرزند != اشاره_تهی) {
        f_چپ_فرزند->پیدا_کن_Nodes(dest, نوع, وضعیت);
    }
    إذا (f_راست_فرزند != اشاره_تهی) {
        f_راست_فرزند->پیدا_کن_Nodes(dest, نوع, وضعیت);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#اگر_تعریف_شده RBBI_DEBUG

ساكن int32_t serial(ثابت RBBINode *گره) {
    أرجع (گره == اشاره_تهی? -1 : گره->f_Serial_رقم);
}


فراغ RBBINode::print_گره(ثابت RBBINode *گره) {
    ساكن ثابت حرف * ثابت گره_Type_نامها[] = {
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

    إذا (گره==اشاره_تهی) {
        RBBIDebugPrintf("%10p", (فراغ *)گره);
    } وإلا {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (فراغ *)گره, گره->f_Serial_رقم, گره_Type_نامها[گره->fType],
            گره->f_قاعده_ریشه?'R':' ', گره->f_Chain_درون?'C':' ',
            serial(گره->f_چپ_فرزند), serial(گره->f_راست_فرزند), serial(گره->f_والد),
            گره->f_اولی_Pos, گره->fVal);
        إذا (گره->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(گره->f_متن);
        }
    }
    RBBIDebugPrintf("\n");
}
#پایان_اگر


#اگر_تعریف_شده RBBI_DEBUG
U_CFUNC فراغ RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحيح min_عرض) {
    RBBIDebugPrintf("%*s", min_عرض, CStr(s)());
}
#پایان_اگر


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#اگر_تعریف_شده RBBI_DEBUG
فراغ RBBINode::print_گره_سرآیند() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
فراغ RBBINode::print_درخت(ثابت RBBINode *گره, UBool printHeading) {
    إذا (printHeading) {
        print_گره_سرآیند();
    }
    print_گره(گره);
    إذا (گره != اشاره_تهی) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        إذا (گره->fType != varRef) {
            إذا (گره->f_چپ_فرزند != اشاره_تهی) {
                print_درخت(گره->f_چپ_فرزند, خطأ);
            }
            
            إذا (گره->f_راست_فرزند != اشاره_تهی) {
                print_درخت(گره->f_راست_فرزند, خطأ);
            }
        }
    }
}
#پایان_اگر



U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */
