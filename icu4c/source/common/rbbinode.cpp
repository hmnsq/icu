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

#جے_تعریف_شدہ RBBI_DEBUG
ساكن صحيح  g_آخری_Serial = 0;
#جے_انت


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(گرہ_Type t, UError_کوڈ& حالت) : UMemory() {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
#جے_تعریف_شدہ RBBI_DEBUG
    f_Serial_عدد    = ++g_آخری_Serial;
#جے_انت
    fType         = t;
    f_ماپا       = خالی_سوچک;
    f_بایاں_بچہ    = خالی_سوچک;
    f_دایاں_بچہ   = خالی_سوچک;
    f_داخلہ_سیٹ     = خالی_سوچک;
    f_پہلا_Pos     = 0;
    f_آخری_Pos      = 0;
    fNullable     = خطأ;
    f_Look_Ahead_سرا = خطأ;
    f_قاعدہ_جڑ     = خطأ;
    f_Chain_وچ      = خطأ;
    fVal          = 0;
    fPrecedence   = precZero;

    f_پہلا_Pos_سیٹ  = جديد UVector(حالت);
    f_آخری_Pos_سیٹ   = جديد UVector(حالت);
    fFollowPos    = جديد UVector(حالت);
    إذا (U_SUCCESS(حالت) &&
        (f_پہلا_Pos_سیٹ == خالی_سوچک || f_آخری_Pos_سیٹ == خالی_سوچک || fFollowPos == خالی_سوچک)) {
        حالت =  U_حافظہ_ونڈ_ERROR;
    }
    إذا      (t==opCat)    {fPrecedence = precOpCat;}
    وإلا إذا (t==opOr)     {fPrecedence = precOpOr;}
    وإلا إذا (t==op_شروع)  {fPrecedence = prec_شروع;}
    وإلا إذا (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ثابت RBBINode &other, UError_کوڈ& حالت) : UMemory(other) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
#جے_تعریف_شدہ RBBI_DEBUG
    f_Serial_عدد   = ++g_آخری_Serial;
#جے_انت
    fType        = other.fType;
    f_ماپا      = خالی_سوچک;
    f_بایاں_بچہ   = خالی_سوچک;
    f_دایاں_بچہ  = خالی_سوچک;
    f_داخلہ_سیٹ    = other.f_داخلہ_سیٹ;
    fPrecedence  = other.fPrecedence;
    f_متن        = other.f_متن;
    f_پہلا_Pos    = other.f_پہلا_Pos;
    f_آخری_Pos     = other.f_آخری_Pos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_قاعدہ_جڑ    = خطأ;
    f_Chain_وچ     = other.f_Chain_وچ;
    f_پہلا_Pos_سیٹ = جديد UVector(حالت);   // TODO - get a real status from somewhere
    f_آخری_Pos_سیٹ  = جديد UVector(حالت);
    fFollowPos   = جديد UVector(حالت);
    إذا (U_SUCCESS(حالت) &&
        (f_پہلا_Pos_سیٹ == خالی_سوچک || f_آخری_Pos_سیٹ == خالی_سوچک || fFollowPos == خالی_سوچک)) {
        حالت =  U_حافظہ_ونڈ_ERROR;
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
    مٹاؤ f_داخلہ_سیٹ;
    f_داخلہ_سیٹ = خالی_سوچک;

    بدل (هذا->fType) {
    حالة varRef:
    حالة سیٹ_Ref:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        اكسر;

    افتراضي:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_گرہ(f_بایاں_بچہ);
        f_بایاں_بچہ =   خالی_سوچک;
        // delete        fRightChild;
        NRDelete_گرہ(f_دایاں_بچہ);
        f_دایاں_بچہ = خالی_سوچک;
    }

    مٹاؤ f_پہلا_Pos_سیٹ;
    مٹاؤ f_آخری_Pos_سیٹ;
    مٹاؤ fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
فراغ RBBINode::NRDelete_گرہ(RBBINode *گرہ) {
    إذا (گرہ == خالی_سوچک) {
        أرجع;
    }

    RBBINode *stop_گرہ = گرہ->f_ماپا;
    RBBINode *اگلا_گرہ = گرہ;
    طالما (اگلا_گرہ != stop_گرہ && اگلا_گرہ != خالی_سوچک) {
        RBBINode *current_گرہ = اگلا_گرہ;

        إذا ((current_گرہ->f_بایاں_بچہ == خالی_سوچک && current_گرہ->f_دایاں_بچہ == خالی_سوچک) ||
                current_گرہ->fType == varRef ||      // varRef and setRef nodes do not
                current_گرہ->fType == سیٹ_Ref) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            اگلا_گرہ = current_گرہ->f_ماپا;
            إذا (اگلا_گرہ) {
                إذا (اگلا_گرہ->f_بایاں_بچہ == current_گرہ) {
                    اگلا_گرہ->f_بایاں_بچہ = خالی_سوچک;
                } وإلا إذا (اگلا_گرہ->f_دایاں_بچہ == current_گرہ) {
                    اگلا_گرہ->f_دایاں_بچہ = خالی_سوچک;
                }
            }
            مٹاؤ current_گرہ;
        } وإلا إذا (current_گرہ->f_بایاں_بچہ) {
            اگلا_گرہ = current_گرہ->f_بایاں_بچہ;
            إذا (اگلا_گرہ->f_ماپا == خالی_سوچک) {
                اگلا_گرہ->f_ماپا = current_گرہ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(اگلا_گرہ->f_ماپا == current_گرہ);
        } وإلا إذا (current_گرہ->f_دایاں_بچہ) {
            اگلا_گرہ = current_گرہ->f_دایاں_بچہ;
            إذا (اگلا_گرہ->f_ماپا == خالی_سوچک) {
                اگلا_گرہ->f_ماپا = current_گرہ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(اگلا_گرہ->f_ماپا == current_گرہ);
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
constexpr صحيح k_Recursive_ڈونگھائی_حد_بندی = 3500;
RBBINode *RBBINode::clone_درخت(UError_کوڈ &حالت, صحيح ڈونگھائی) {
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (ڈونگھائی > k_Recursive_ڈونگھائی_حد_بندی) {
        حالت = U_داخلہ_TOO_LONG_ERROR;
        أرجع خالی_سوچک;
    }
    RBBINode    *n;

    إذا (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_بایاں_بچہ->clone_درخت(حالت, ڈونگھائی+1);
        إذا (U_FAILURE(حالت)) {
            أرجع خالی_سوچک;
        }
    } وإلا إذا (fType == RBBINode::uset) {
        n = هذا;
    } وإلا {
        n = جديد RBBINode(*هذا, حالت);
        إذا (U_FAILURE(حالت)) {
            مٹاؤ n;
            أرجع خالی_سوچک;
        }
        // Check for null pointer.
        إذا (n == خالی_سوچک) {
            حالت =  U_حافظہ_ونڈ_ERROR;
            أرجع خالی_سوچک;
        }
        إذا (f_بایاں_بچہ != خالی_سوچک) {
            n->f_بایاں_بچہ          = f_بایاں_بچہ->clone_درخت(حالت, ڈونگھائی+1);
            إذا (U_FAILURE(حالت)) {
                مٹاؤ n;
                أرجع خالی_سوچک;
            }
            n->f_بایاں_بچہ->f_ماپا = n;
        }
        إذا (f_دایاں_بچہ != خالی_سوچک) {
            n->f_دایاں_بچہ          = f_دایاں_بچہ->clone_درخت(حالت, ڈونگھائی+1);
            إذا (U_FAILURE(حالت)) {
                مٹاؤ n;
                أرجع خالی_سوچک;
            }
            n->f_دایاں_بچہ->f_ماپا = n;
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
RBBINode *RBBINode::flattenVariables(UError_کوڈ& حالت, صحيح ڈونگھائی) {
    إذا (U_FAILURE(حالت)) {
        أرجع هذا;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (ڈونگھائی > k_Recursive_ڈونگھائی_حد_بندی) {
        حالت = U_داخلہ_TOO_LONG_ERROR;
        أرجع هذا;
    }
    إذا (fType == varRef) {
        RBBINode *ret_گرہ  = f_بایاں_بچہ->clone_درخت(حالت, ڈونگھائی+1);
        إذا (U_FAILURE(حالت)) {
            أرجع هذا;
        }
        ret_گرہ->f_قاعدہ_جڑ = هذا->f_قاعدہ_جڑ;
        ret_گرہ->f_Chain_وچ  = هذا->f_Chain_وچ;
        مٹاؤ هذا;   // TODO: undefined behavior. Fix.
        أرجع ret_گرہ;
    }

    إذا (f_بایاں_بچہ != خالی_سوچک) {
        f_بایاں_بچہ = f_بایاں_بچہ->flattenVariables(حالت, ڈونگھائی+1);
        إذا (f_بایاں_بچہ == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
        }
        إذا (U_FAILURE(حالت)) {
            أرجع هذا;
        }
        f_بایاں_بچہ->f_ماپا  = هذا;
    }
    إذا (f_دایاں_بچہ != خالی_سوچک) {
        f_دایاں_بچہ = f_دایاں_بچہ->flattenVariables(حالت, ڈونگھائی+1);
        إذا (f_دایاں_بچہ == خالی_سوچک) {
            حالت = U_حافظہ_ونڈ_ERROR;
        }
        إذا (U_FAILURE(حالت)) {
            أرجع هذا;
        }
        f_دایاں_بچہ->f_ماپا = هذا;
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
فراغ RBBINode::flattenSets(UError_کوڈ &حالت, صحيح ڈونگھائی) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (ڈونگھائی > k_Recursive_ڈونگھائی_حد_بندی) {
        حالت = U_داخلہ_TOO_LONG_ERROR;
        أرجع;
    }
    U_ASSERT(fType != سیٹ_Ref);

    إذا (f_بایاں_بچہ != خالی_سوچک) {
        إذا (f_بایاں_بچہ->fType==سیٹ_Ref) {
            RBBINode *سیٹ_Ref_گرہ = f_بایاں_بچہ;
            RBBINode *uset_گرہ   = سیٹ_Ref_گرہ->f_بایاں_بچہ;
            RBBINode *repl_درخت   = uset_گرہ->f_بایاں_بچہ;
            f_بایاں_بچہ           = repl_درخت->clone_درخت(حالت, ڈونگھائی+1);
            إذا (U_FAILURE(حالت)) {
                مٹاؤ سیٹ_Ref_گرہ;
                أرجع;
            }
            f_بایاں_بچہ->f_ماپا  = هذا;
            مٹاؤ سیٹ_Ref_گرہ;
        } وإلا {
            f_بایاں_بچہ->flattenSets(حالت, ڈونگھائی+1);
        }
    }

    إذا (f_دایاں_بچہ != خالی_سوچک) {
        إذا (f_دایاں_بچہ->fType==سیٹ_Ref) {
            RBBINode *سیٹ_Ref_گرہ = f_دایاں_بچہ;
            RBBINode *uset_گرہ   = سیٹ_Ref_گرہ->f_بایاں_بچہ;
            RBBINode *repl_درخت   = uset_گرہ->f_بایاں_بچہ;
            f_دایاں_بچہ           = repl_درخت->clone_درخت(حالت, ڈونگھائی+1);
            إذا (U_FAILURE(حالت)) {
                مٹاؤ سیٹ_Ref_گرہ;
                أرجع;
            }
            f_دایاں_بچہ->f_ماپا  = هذا;
            مٹاؤ سیٹ_Ref_گرہ;
        } وإلا {
            f_دایاں_بچہ->flattenSets(حالت, ڈونگھائی+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
فراغ   RBBINode::لبھو_Nodes(UVector *dest, RBBINode::گرہ_Type قسم, UError_کوڈ &حالت) {
    /* test for buffer overflows */
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    U_ASSERT(!dest->hasDeleter());
    إذا (fType == قسم) {
        dest->جوڑو_Element(هذا, حالت);
    }
    إذا (f_بایاں_بچہ != خالی_سوچک) {
        f_بایاں_بچہ->لبھو_Nodes(dest, قسم, حالت);
    }
    إذا (f_دایاں_بچہ != خالی_سوچک) {
        f_دایاں_بچہ->لبھو_Nodes(dest, قسم, حالت);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#جے_تعریف_شدہ RBBI_DEBUG

ساكن int32_t serial(ثابت RBBINode *گرہ) {
    أرجع (گرہ == خالی_سوچک? -1 : گرہ->f_Serial_عدد);
}


فراغ RBBINode::print_گرہ(ثابت RBBINode *گرہ) {
    ساكن ثابت حرف * ثابت گرہ_Type_ناں[] = {
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

    إذا (گرہ==خالی_سوچک) {
        RBBIDebugPrintf("%10p", (فراغ *)گرہ);
    } وإلا {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (فراغ *)گرہ, گرہ->f_Serial_عدد, گرہ_Type_ناں[گرہ->fType],
            گرہ->f_قاعدہ_جڑ?'R':' ', گرہ->f_Chain_وچ?'C':' ',
            serial(گرہ->f_بایاں_بچہ), serial(گرہ->f_دایاں_بچہ), serial(گرہ->f_ماپا),
            گرہ->f_پہلا_Pos, گرہ->fVal);
        إذا (گرہ->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(گرہ->f_متن);
        }
    }
    RBBIDebugPrintf("\n");
}
#جے_انت


#جے_تعریف_شدہ RBBI_DEBUG
U_CFUNC فراغ RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحيح min_چوڑائی) {
    RBBIDebugPrintf("%*s", min_چوڑائی, CStr(s)());
}
#جے_انت


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#جے_تعریف_شدہ RBBI_DEBUG
فراغ RBBINode::print_گرہ_سرخی() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
فراغ RBBINode::print_درخت(ثابت RBBINode *گرہ, UBool printHeading) {
    إذا (printHeading) {
        print_گرہ_سرخی();
    }
    print_گرہ(گرہ);
    إذا (گرہ != خالی_سوچک) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        إذا (گرہ->fType != varRef) {
            إذا (گرہ->f_بایاں_بچہ != خالی_سوچک) {
                print_درخت(گرہ->f_بایاں_بچہ, خطأ);
            }
            
            إذا (گرہ->f_دایاں_بچہ != خالی_سوچک) {
                print_درخت(گرہ->f_دایاں_بچہ, خطأ);
            }
        }
    }
}
#جے_انت



U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */
