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

#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
ساكن صحيح  g_آخري_Serial = 0;
#جيڪڏهن_پورو


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(ڳنڍ_Type t, UError_ڪوڊ& حالت) : UMemory() {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    f_Serial_انگ    = ++g_آخري_Serial;
#جيڪڏهن_پورو
    fType         = t;
    f_والد       = خالي_اشارو;
    f_کاٻو_ٻار    = خالي_اشارو;
    f_ساڄو_ٻار   = خالي_اشارو;
    f_داخلا_سيٽ     = خالي_اشارو;
    f_پهريون_Pos     = 0;
    f_آخري_Pos      = 0;
    fNullable     = خطأ;
    f_Look_Ahead_پڇاڙي = خطأ;
    f_قاعدو_پاڙ     = خطأ;
    f_Chain_اندريون      = خطأ;
    fVal          = 0;
    fPrecedence   = precZero;

    f_پهريون_Pos_سيٽ  = جديد UVector(حالت);
    f_آخري_Pos_سيٽ   = جديد UVector(حالت);
    fFollowPos    = جديد UVector(حالت);
    إذا (U_SUCCESS(حالت) &&
        (f_پهريون_Pos_سيٽ == خالي_اشارو || f_آخري_Pos_سيٽ == خالي_اشارو || fFollowPos == خالي_اشارو)) {
        حالت =  U_يادگيري_ورهاست_ERROR;
    }
    إذا      (t==opCat)    {fPrecedence = precOpCat;}
    وإلا إذا (t==opOr)     {fPrecedence = precOpOr;}
    وإلا إذا (t==op_شروعات)  {fPrecedence = prec_شروعات;}
    وإلا إذا (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ثابت RBBINode &other, UError_ڪوڊ& حالت) : UMemory(other) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    f_Serial_انگ   = ++g_آخري_Serial;
#جيڪڏهن_پورو
    fType        = other.fType;
    f_والد      = خالي_اشارو;
    f_کاٻو_ٻار   = خالي_اشارو;
    f_ساڄو_ٻار  = خالي_اشارو;
    f_داخلا_سيٽ    = other.f_داخلا_سيٽ;
    fPrecedence  = other.fPrecedence;
    f_متن        = other.f_متن;
    f_پهريون_Pos    = other.f_پهريون_Pos;
    f_آخري_Pos     = other.f_آخري_Pos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_قاعدو_پاڙ    = خطأ;
    f_Chain_اندريون     = other.f_Chain_اندريون;
    f_پهريون_Pos_سيٽ = جديد UVector(حالت);   // TODO - get a real status from somewhere
    f_آخري_Pos_سيٽ  = جديد UVector(حالت);
    fFollowPos   = جديد UVector(حالت);
    إذا (U_SUCCESS(حالت) &&
        (f_پهريون_Pos_سيٽ == خالي_اشارو || f_آخري_Pos_سيٽ == خالي_اشارو || fFollowPos == خالي_اشارو)) {
        حالت =  U_يادگيري_ورهاست_ERROR;
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
    ميٽيو f_داخلا_سيٽ;
    f_داخلا_سيٽ = خالي_اشارو;

    بدل (هذا->fType) {
    حالة varRef:
    حالة سيٽ_Ref:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        اكسر;

    افتراضي:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_ڳنڍ(f_کاٻو_ٻار);
        f_کاٻو_ٻار =   خالي_اشارو;
        // delete        fRightChild;
        NRDelete_ڳنڍ(f_ساڄو_ٻار);
        f_ساڄو_ٻار = خالي_اشارو;
    }

    ميٽيو f_پهريون_Pos_سيٽ;
    ميٽيو f_آخري_Pos_سيٽ;
    ميٽيو fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
فراغ RBBINode::NRDelete_ڳنڍ(RBBINode *ڳنڍ) {
    إذا (ڳنڍ == خالي_اشارو) {
        أرجع;
    }

    RBBINode *stop_ڳنڍ = ڳنڍ->f_والد;
    RBBINode *ايندڙ_ڳنڍ = ڳنڍ;
    طالما (ايندڙ_ڳنڍ != stop_ڳنڍ && ايندڙ_ڳنڍ != خالي_اشارو) {
        RBBINode *current_ڳنڍ = ايندڙ_ڳنڍ;

        إذا ((current_ڳنڍ->f_کاٻو_ٻار == خالي_اشارو && current_ڳنڍ->f_ساڄو_ٻار == خالي_اشارو) ||
                current_ڳنڍ->fType == varRef ||      // varRef and setRef nodes do not
                current_ڳنڍ->fType == سيٽ_Ref) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            ايندڙ_ڳنڍ = current_ڳنڍ->f_والد;
            إذا (ايندڙ_ڳنڍ) {
                إذا (ايندڙ_ڳنڍ->f_کاٻو_ٻار == current_ڳنڍ) {
                    ايندڙ_ڳنڍ->f_کاٻو_ٻار = خالي_اشارو;
                } وإلا إذا (ايندڙ_ڳنڍ->f_ساڄو_ٻار == current_ڳنڍ) {
                    ايندڙ_ڳنڍ->f_ساڄو_ٻار = خالي_اشارو;
                }
            }
            ميٽيو current_ڳنڍ;
        } وإلا إذا (current_ڳنڍ->f_کاٻو_ٻار) {
            ايندڙ_ڳنڍ = current_ڳنڍ->f_کاٻو_ٻار;
            إذا (ايندڙ_ڳنڍ->f_والد == خالي_اشارو) {
                ايندڙ_ڳنڍ->f_والد = current_ڳنڍ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(ايندڙ_ڳنڍ->f_والد == current_ڳنڍ);
        } وإلا إذا (current_ڳنڍ->f_ساڄو_ٻار) {
            ايندڙ_ڳنڍ = current_ڳنڍ->f_ساڄو_ٻار;
            إذا (ايندڙ_ڳنڍ->f_والد == خالي_اشارو) {
                ايندڙ_ڳنڍ->f_والد = current_ڳنڍ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(ايندڙ_ڳنڍ->f_والد == current_ڳنڍ);
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
constexpr صحيح k_Recursive_اونهائي_حد_بندي = 3500;
RBBINode *RBBINode::clone_وڻ(UError_ڪوڊ &حالت, صحيح اونهائي) {
    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (اونهائي > k_Recursive_اونهائي_حد_بندي) {
        حالت = U_داخلا_TOO_LONG_ERROR;
        أرجع خالي_اشارو;
    }
    RBBINode    *n;

    إذا (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_کاٻو_ٻار->clone_وڻ(حالت, اونهائي+1);
        إذا (U_FAILURE(حالت)) {
            أرجع خالي_اشارو;
        }
    } وإلا إذا (fType == RBBINode::uset) {
        n = هذا;
    } وإلا {
        n = جديد RBBINode(*هذا, حالت);
        إذا (U_FAILURE(حالت)) {
            ميٽيو n;
            أرجع خالي_اشارو;
        }
        // Check for null pointer.
        إذا (n == خالي_اشارو) {
            حالت =  U_يادگيري_ورهاست_ERROR;
            أرجع خالي_اشارو;
        }
        إذا (f_کاٻو_ٻار != خالي_اشارو) {
            n->f_کاٻو_ٻار          = f_کاٻو_ٻار->clone_وڻ(حالت, اونهائي+1);
            إذا (U_FAILURE(حالت)) {
                ميٽيو n;
                أرجع خالي_اشارو;
            }
            n->f_کاٻو_ٻار->f_والد = n;
        }
        إذا (f_ساڄو_ٻار != خالي_اشارو) {
            n->f_ساڄو_ٻار          = f_ساڄو_ٻار->clone_وڻ(حالت, اونهائي+1);
            إذا (U_FAILURE(حالت)) {
                ميٽيو n;
                أرجع خالي_اشارو;
            }
            n->f_ساڄو_ٻار->f_والد = n;
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
RBBINode *RBBINode::flattenVariables(UError_ڪوڊ& حالت, صحيح اونهائي) {
    إذا (U_FAILURE(حالت)) {
        أرجع هذا;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (اونهائي > k_Recursive_اونهائي_حد_بندي) {
        حالت = U_داخلا_TOO_LONG_ERROR;
        أرجع هذا;
    }
    إذا (fType == varRef) {
        RBBINode *ret_ڳنڍ  = f_کاٻو_ٻار->clone_وڻ(حالت, اونهائي+1);
        إذا (U_FAILURE(حالت)) {
            أرجع هذا;
        }
        ret_ڳنڍ->f_قاعدو_پاڙ = هذا->f_قاعدو_پاڙ;
        ret_ڳنڍ->f_Chain_اندريون  = هذا->f_Chain_اندريون;
        ميٽيو هذا;   // TODO: undefined behavior. Fix.
        أرجع ret_ڳنڍ;
    }

    إذا (f_کاٻو_ٻار != خالي_اشارو) {
        f_کاٻو_ٻار = f_کاٻو_ٻار->flattenVariables(حالت, اونهائي+1);
        إذا (f_کاٻو_ٻار == خالي_اشارو) {
            حالت = U_يادگيري_ورهاست_ERROR;
        }
        إذا (U_FAILURE(حالت)) {
            أرجع هذا;
        }
        f_کاٻو_ٻار->f_والد  = هذا;
    }
    إذا (f_ساڄو_ٻار != خالي_اشارو) {
        f_ساڄو_ٻار = f_ساڄو_ٻار->flattenVariables(حالت, اونهائي+1);
        إذا (f_ساڄو_ٻار == خالي_اشارو) {
            حالت = U_يادگيري_ورهاست_ERROR;
        }
        إذا (U_FAILURE(حالت)) {
            أرجع هذا;
        }
        f_ساڄو_ٻار->f_والد = هذا;
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
فراغ RBBINode::flattenSets(UError_ڪوڊ &حالت, صحيح اونهائي) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    إذا (اونهائي > k_Recursive_اونهائي_حد_بندي) {
        حالت = U_داخلا_TOO_LONG_ERROR;
        أرجع;
    }
    U_ASSERT(fType != سيٽ_Ref);

    إذا (f_کاٻو_ٻار != خالي_اشارو) {
        إذا (f_کاٻو_ٻار->fType==سيٽ_Ref) {
            RBBINode *سيٽ_Ref_ڳنڍ = f_کاٻو_ٻار;
            RBBINode *uset_ڳنڍ   = سيٽ_Ref_ڳنڍ->f_کاٻو_ٻار;
            RBBINode *repl_وڻ   = uset_ڳنڍ->f_کاٻو_ٻار;
            f_کاٻو_ٻار           = repl_وڻ->clone_وڻ(حالت, اونهائي+1);
            إذا (U_FAILURE(حالت)) {
                ميٽيو سيٽ_Ref_ڳنڍ;
                أرجع;
            }
            f_کاٻو_ٻار->f_والد  = هذا;
            ميٽيو سيٽ_Ref_ڳنڍ;
        } وإلا {
            f_کاٻو_ٻار->flattenSets(حالت, اونهائي+1);
        }
    }

    إذا (f_ساڄو_ٻار != خالي_اشارو) {
        إذا (f_ساڄو_ٻار->fType==سيٽ_Ref) {
            RBBINode *سيٽ_Ref_ڳنڍ = f_ساڄو_ٻار;
            RBBINode *uset_ڳنڍ   = سيٽ_Ref_ڳنڍ->f_کاٻو_ٻار;
            RBBINode *repl_وڻ   = uset_ڳنڍ->f_کاٻو_ٻار;
            f_ساڄو_ٻار           = repl_وڻ->clone_وڻ(حالت, اونهائي+1);
            إذا (U_FAILURE(حالت)) {
                ميٽيو سيٽ_Ref_ڳنڍ;
                أرجع;
            }
            f_ساڄو_ٻار->f_والد  = هذا;
            ميٽيو سيٽ_Ref_ڳنڍ;
        } وإلا {
            f_ساڄو_ٻار->flattenSets(حالت, اونهائي+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
فراغ   RBBINode::ڳوليو_Nodes(UVector *dest, RBBINode::ڳنڍ_Type قسم, UError_ڪوڊ &حالت) {
    /* test for buffer overflows */
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    U_ASSERT(!dest->hasDeleter());
    إذا (fType == قسم) {
        dest->جوڙيو_Element(هذا, حالت);
    }
    إذا (f_کاٻو_ٻار != خالي_اشارو) {
        f_کاٻو_ٻار->ڳوليو_Nodes(dest, قسم, حالت);
    }
    إذا (f_ساڄو_ٻار != خالي_اشارو) {
        f_ساڄو_ٻار->ڳوليو_Nodes(dest, قسم, حالت);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG

ساكن int32_t serial(ثابت RBBINode *ڳنڍ) {
    أرجع (ڳنڍ == خالي_اشارو? -1 : ڳنڍ->f_Serial_انگ);
}


فراغ RBBINode::print_ڳنڍ(ثابت RBBINode *ڳنڍ) {
    ساكن ثابت حرف * ثابت ڳنڍ_Type_نالا[] = {
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

    إذا (ڳنڍ==خالي_اشارو) {
        RBBIDebugPrintf("%10p", (فراغ *)ڳنڍ);
    } وإلا {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (فراغ *)ڳنڍ, ڳنڍ->f_Serial_انگ, ڳنڍ_Type_نالا[ڳنڍ->fType],
            ڳنڍ->f_قاعدو_پاڙ?'R':' ', ڳنڍ->f_Chain_اندريون?'C':' ',
            serial(ڳنڍ->f_کاٻو_ٻار), serial(ڳنڍ->f_ساڄو_ٻار), serial(ڳنڍ->f_والد),
            ڳنڍ->f_پهريون_Pos, ڳنڍ->fVal);
        إذا (ڳنڍ->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(ڳنڍ->f_متن);
        }
    }
    RBBIDebugPrintf("\n");
}
#جيڪڏهن_پورو


#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
U_CFUNC فراغ RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحيح min_ويڪر) {
    RBBIDebugPrintf("%*s", min_ويڪر, CStr(s)());
}
#جيڪڏهن_پورو


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
فراغ RBBINode::print_ڳنڍ_سرو() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
فراغ RBBINode::print_وڻ(ثابت RBBINode *ڳنڍ, UBool printHeading) {
    إذا (printHeading) {
        print_ڳنڍ_سرو();
    }
    print_ڳنڍ(ڳنڍ);
    إذا (ڳنڍ != خالي_اشارو) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        إذا (ڳنڍ->fType != varRef) {
            إذا (ڳنڍ->f_کاٻو_ٻار != خالي_اشارو) {
                print_وڻ(ڳنڍ->f_کاٻو_ٻار, خطأ);
            }
            
            إذا (ڳنڍ->f_ساڄو_ٻار != خالي_اشارو) {
                print_وڻ(ڳنڍ->f_ساڄو_ٻار, خطأ);
            }
        }
    }
}
#جيڪڏهن_پورو



U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */
