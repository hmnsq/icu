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

#اگر_تعریف_شدہ RBBI_DEBUG
ساکن صحیح  g_آخری_Serial = 0;
#اگر_ختم


//-------------------------------------------------------------------------
//
//    Constructor.   Just set the fields to reasonable default values.
//
//-------------------------------------------------------------------------
RBBINode::RBBINode(گرہ_Type t, UError_رمز& حالت_2) : UMemory() {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
#اگر_تعریف_شدہ RBBI_DEBUG
    f_Serial_عدد    = ++g_آخری_Serial;
#اگر_ختم
    fType         = t;
    f_والد       = خالی_اشاریہ;
    f_بایاں_بچہ    = خالی_اشاریہ;
    f_دایاں_بچہ   = خالی_اشاریہ;
    f_داخلہ_سیٹ     = خالی_اشاریہ;
    f_پہلا_Pos     = 0;
    f_آخری_Pos      = 0;
    fNullable     = جھوٹ;
    f_Look_Ahead_انتہا = جھوٹ;
    f_قاعدہ_جڑ     = جھوٹ;
    f_Chain_اندر      = جھوٹ;
    fVal          = 0;
    fPrecedence   = precZero;

    f_پہلا_Pos_سیٹ  = نیا UVector(حالت_2);
    f_آخری_Pos_سیٹ   = نیا UVector(حالت_2);
    fFollowPos    = نیا UVector(حالت_2);
    اگر (U_SUCCESS(حالت_2) &&
        (f_پہلا_Pos_سیٹ == خالی_اشاریہ || f_آخری_Pos_سیٹ == خالی_اشاریہ || fFollowPos == خالی_اشاریہ)) {
        حالت_2 =  U_حافظہ_تخصیص_ERROR;
    }
    اگر      (t==opCat)    {fPrecedence = precOpCat;}
    ورنہ اگر (t==opOr)     {fPrecedence = precOpOr;}
    ورنہ اگر (t==op_آغاز)  {fPrecedence = prec_آغاز;}
    ورنہ اگر (t==opLParen) {fPrecedence = precLParen;}

}


RBBINode::RBBINode(ثابت RBBINode &other, UError_رمز& حالت_2) : UMemory(other) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
#اگر_تعریف_شدہ RBBI_DEBUG
    f_Serial_عدد   = ++g_آخری_Serial;
#اگر_ختم
    fType        = other.fType;
    f_والد      = خالی_اشاریہ;
    f_بایاں_بچہ   = خالی_اشاریہ;
    f_دایاں_بچہ  = خالی_اشاریہ;
    f_داخلہ_سیٹ    = other.f_داخلہ_سیٹ;
    fPrecedence  = other.fPrecedence;
    f_متن        = other.f_متن;
    f_پہلا_Pos    = other.f_پہلا_Pos;
    f_آخری_Pos     = other.f_آخری_Pos;
    fNullable    = other.fNullable;
    fVal         = other.fVal;
    f_قاعدہ_جڑ    = جھوٹ;
    f_Chain_اندر     = other.f_Chain_اندر;
    f_پہلا_Pos_سیٹ = نیا UVector(حالت_2);   // TODO - get a real status from somewhere
    f_آخری_Pos_سیٹ  = نیا UVector(حالت_2);
    fFollowPos   = نیا UVector(حالت_2);
    اگر (U_SUCCESS(حالت_2) &&
        (f_پہلا_Pos_سیٹ == خالی_اشاریہ || f_آخری_Pos_سیٹ == خالی_اشاریہ || fFollowPos == خالی_اشاریہ)) {
        حالت_2 =  U_حافظہ_تخصیص_ERROR;
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
    حذف f_داخلہ_سیٹ;
    f_داخلہ_سیٹ = خالی_اشاریہ;

    انتخاب (یہ->fType) {
    حالت varRef:
    حالت سیٹ_Ref:
        // for these node types, multiple instances point to the same "children"
        // Storage ownership of children handled elsewhere.  Don't delete here.
        توڑو;

    طےشدہ:
        // Avoid using a recursive implementation because of stack overflow problems.
        // See bug ICU-22584.
        // delete        fLeftChild;
        NRDelete_گرہ(f_بایاں_بچہ);
        f_بایاں_بچہ =   خالی_اشاریہ;
        // delete        fRightChild;
        NRDelete_گرہ(f_دایاں_بچہ);
        f_دایاں_بچہ = خالی_اشاریہ;
    }

    حذف f_پہلا_Pos_سیٹ;
    حذف f_آخری_Pos_سیٹ;
    حذف fFollowPos;
}

/**
 * Non-recursive delete of a node + its children. Used from the node destructor
 * instead of the more obvious recursive implementation to avoid problems with
 * stack overflow with some perverse test rule data (from fuzzing).
 */
خالی RBBINode::NRDelete_گرہ(RBBINode *گرہ) {
    اگر (گرہ == خالی_اشاریہ) {
        واپس;
    }

    RBBINode *stop_گرہ = گرہ->f_والد;
    RBBINode *اگلا_گرہ = گرہ;
    جب (اگلا_گرہ != stop_گرہ && اگلا_گرہ != خالی_اشاریہ) {
        RBBINode *current_گرہ = اگلا_گرہ;

        اگر ((current_گرہ->f_بایاں_بچہ == خالی_اشاریہ && current_گرہ->f_دایاں_بچہ == خالی_اشاریہ) ||
                current_گرہ->fType == varRef ||      // varRef and setRef nodes do not
                current_گرہ->fType == سیٹ_Ref) {      // own their children nodes.
            // CurrentNode is effectively a leaf node; it's safe to go ahead and delete it.
            اگلا_گرہ = current_گرہ->f_والد;
            اگر (اگلا_گرہ) {
                اگر (اگلا_گرہ->f_بایاں_بچہ == current_گرہ) {
                    اگلا_گرہ->f_بایاں_بچہ = خالی_اشاریہ;
                } ورنہ اگر (اگلا_گرہ->f_دایاں_بچہ == current_گرہ) {
                    اگلا_گرہ->f_دایاں_بچہ = خالی_اشاریہ;
                }
            }
            حذف current_گرہ;
        } ورنہ اگر (current_گرہ->f_بایاں_بچہ) {
            اگلا_گرہ = current_گرہ->f_بایاں_بچہ;
            اگر (اگلا_گرہ->f_والد == خالی_اشاریہ) {
                اگلا_گرہ->f_والد = current_گرہ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(اگلا_گرہ->f_والد == current_گرہ);
        } ورنہ اگر (current_گرہ->f_دایاں_بچہ) {
            اگلا_گرہ = current_گرہ->f_دایاں_بچہ;
            اگر (اگلا_گرہ->f_والد == خالی_اشاریہ) {
                اگلا_گرہ->f_والد = current_گرہ;
                // fParent isn't always set; do it now if not.
            }
            U_ASSERT(اگلا_گرہ->f_والد == current_گرہ);
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
constexpr صحیح k_Recursive_گہرائی_حد_بندی = 3500;
RBBINode *RBBINode::clone_درخت(UError_رمز &حالت_2, صحیح گہرائی) {
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    اگر (گہرائی > k_Recursive_گہرائی_حد_بندی) {
        حالت_2 = U_داخلہ_TOO_LONG_ERROR;
        واپس خالی_اشاریہ;
    }
    RBBINode    *n;

    اگر (fType == RBBINode::varRef) {
        // If the current node is a variable reference, skip over it
        //   and clone the definition of the variable instead.
        n = f_بایاں_بچہ->clone_درخت(حالت_2, گہرائی+1);
        اگر (U_FAILURE(حالت_2)) {
            واپس خالی_اشاریہ;
        }
    } ورنہ اگر (fType == RBBINode::uset) {
        n = یہ;
    } ورنہ {
        n = نیا RBBINode(*یہ, حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            حذف n;
            واپس خالی_اشاریہ;
        }
        // Check for null pointer.
        اگر (n == خالی_اشاریہ) {
            حالت_2 =  U_حافظہ_تخصیص_ERROR;
            واپس خالی_اشاریہ;
        }
        اگر (f_بایاں_بچہ != خالی_اشاریہ) {
            n->f_بایاں_بچہ          = f_بایاں_بچہ->clone_درخت(حالت_2, گہرائی+1);
            اگر (U_FAILURE(حالت_2)) {
                حذف n;
                واپس خالی_اشاریہ;
            }
            n->f_بایاں_بچہ->f_والد = n;
        }
        اگر (f_دایاں_بچہ != خالی_اشاریہ) {
            n->f_دایاں_بچہ          = f_دایاں_بچہ->clone_درخت(حالت_2, گہرائی+1);
            اگر (U_FAILURE(حالت_2)) {
                حذف n;
                واپس خالی_اشاریہ;
            }
            n->f_دایاں_بچہ->f_والد = n;
        }
    }
    واپس n;
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
RBBINode *RBBINode::flattenVariables(UError_رمز& حالت_2, صحیح گہرائی) {
    اگر (U_FAILURE(حالت_2)) {
        واپس یہ;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    اگر (گہرائی > k_Recursive_گہرائی_حد_بندی) {
        حالت_2 = U_داخلہ_TOO_LONG_ERROR;
        واپس یہ;
    }
    اگر (fType == varRef) {
        RBBINode *ret_گرہ  = f_بایاں_بچہ->clone_درخت(حالت_2, گہرائی+1);
        اگر (U_FAILURE(حالت_2)) {
            واپس یہ;
        }
        ret_گرہ->f_قاعدہ_جڑ = یہ->f_قاعدہ_جڑ;
        ret_گرہ->f_Chain_اندر  = یہ->f_Chain_اندر;
        حذف یہ;   // TODO: undefined behavior. Fix.
        واپس ret_گرہ;
    }

    اگر (f_بایاں_بچہ != خالی_اشاریہ) {
        f_بایاں_بچہ = f_بایاں_بچہ->flattenVariables(حالت_2, گہرائی+1);
        اگر (f_بایاں_بچہ == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
        }
        اگر (U_FAILURE(حالت_2)) {
            واپس یہ;
        }
        f_بایاں_بچہ->f_والد  = یہ;
    }
    اگر (f_دایاں_بچہ != خالی_اشاریہ) {
        f_دایاں_بچہ = f_دایاں_بچہ->flattenVariables(حالت_2, گہرائی+1);
        اگر (f_دایاں_بچہ == خالی_اشاریہ) {
            حالت_2 = U_حافظہ_تخصیص_ERROR;
        }
        اگر (U_FAILURE(حالت_2)) {
            واپس یہ;
        }
        f_دایاں_بچہ->f_والد = یہ;
    }
    واپس یہ;
}


//-------------------------------------------------------------------------
//
//  flattenSets    Walk the parse tree, replacing any nodes of type setRef
//                 with a copy of the expression tree for the set.  A set's
//                 equivalent expression tree is precomputed and saved as
//                 the left child of the uset node.
//
//-------------------------------------------------------------------------
خالی RBBINode::flattenSets(UError_رمز &حالت_2, صحیح گہرائی) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    // If the depth of the stack is too deep, we return U_INPUT_TOO_LONG_ERROR
    // to avoid stack overflow crash.
    اگر (گہرائی > k_Recursive_گہرائی_حد_بندی) {
        حالت_2 = U_داخلہ_TOO_LONG_ERROR;
        واپس;
    }
    U_ASSERT(fType != سیٹ_Ref);

    اگر (f_بایاں_بچہ != خالی_اشاریہ) {
        اگر (f_بایاں_بچہ->fType==سیٹ_Ref) {
            RBBINode *سیٹ_Ref_گرہ = f_بایاں_بچہ;
            RBBINode *uset_گرہ   = سیٹ_Ref_گرہ->f_بایاں_بچہ;
            RBBINode *repl_درخت   = uset_گرہ->f_بایاں_بچہ;
            f_بایاں_بچہ           = repl_درخت->clone_درخت(حالت_2, گہرائی+1);
            اگر (U_FAILURE(حالت_2)) {
                حذف سیٹ_Ref_گرہ;
                واپس;
            }
            f_بایاں_بچہ->f_والد  = یہ;
            حذف سیٹ_Ref_گرہ;
        } ورنہ {
            f_بایاں_بچہ->flattenSets(حالت_2, گہرائی+1);
        }
    }

    اگر (f_دایاں_بچہ != خالی_اشاریہ) {
        اگر (f_دایاں_بچہ->fType==سیٹ_Ref) {
            RBBINode *سیٹ_Ref_گرہ = f_دایاں_بچہ;
            RBBINode *uset_گرہ   = سیٹ_Ref_گرہ->f_بایاں_بچہ;
            RBBINode *repl_درخت   = uset_گرہ->f_بایاں_بچہ;
            f_دایاں_بچہ           = repl_درخت->clone_درخت(حالت_2, گہرائی+1);
            اگر (U_FAILURE(حالت_2)) {
                حذف سیٹ_Ref_گرہ;
                واپس;
            }
            f_دایاں_بچہ->f_والد  = یہ;
            حذف سیٹ_Ref_گرہ;
        } ورنہ {
            f_دایاں_بچہ->flattenSets(حالت_2, گہرائی+1);
        }
    }
}



//-------------------------------------------------------------------------
//
//   findNodes()     Locate all the nodes of the specified type, starting
//                   at the specified root.
//
//-------------------------------------------------------------------------
خالی   RBBINode::ڈھونڈو_Nodes(UVector *dest, RBBINode::گرہ_Type قسم, UError_رمز &حالت_2) {
    /* test for buffer overflows */
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    U_ASSERT(!dest->hasDeleter());
    اگر (fType == قسم) {
        dest->جوڑو_Element(یہ, حالت_2);
    }
    اگر (f_بایاں_بچہ != خالی_اشاریہ) {
        f_بایاں_بچہ->ڈھونڈو_Nodes(dest, قسم, حالت_2);
    }
    اگر (f_دایاں_بچہ != خالی_اشاریہ) {
        f_دایاں_بچہ->ڈھونڈو_Nodes(dest, قسم, حالت_2);
    }
}


//-------------------------------------------------------------------------
//
//    print.         Print out a single node, for debugging.
//
//-------------------------------------------------------------------------
#اگر_تعریف_شدہ RBBI_DEBUG

ساکن int32_t serial(ثابت RBBINode *گرہ) {
    واپس (گرہ == خالی_اشاریہ? -1 : گرہ->f_Serial_عدد);
}


خالی RBBINode::print_گرہ(ثابت RBBINode *گرہ) {
    ساکن ثابت حرف * ثابت گرہ_Type_نام[] = {
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

    اگر (گرہ==خالی_اشاریہ) {
        RBBIDebugPrintf("%10p", (خالی *)گرہ);
    } ورنہ {
        RBBIDebugPrintf("%10p %5d %12s %c%c  %5d       %5d     %5d       %6d     %d ",
            (خالی *)گرہ, گرہ->f_Serial_عدد, گرہ_Type_نام[گرہ->fType],
            گرہ->f_قاعدہ_جڑ?'R':' ', گرہ->f_Chain_اندر?'C':' ',
            serial(گرہ->f_بایاں_بچہ), serial(گرہ->f_دایاں_بچہ), serial(گرہ->f_والد),
            گرہ->f_پہلا_Pos, گرہ->fVal);
        اگر (گرہ->fType == varRef) {
            RBBI_DEBUG_printUnicodeString(گرہ->f_متن);
        }
    }
    RBBIDebugPrintf("\n");
}
#اگر_ختم


#اگر_تعریف_شدہ RBBI_DEBUG
U_CFUNC خالی RBBI_DEBUG_printUnicodeString(ثابت UnicodeString &s, صحیح min_چوڑائی) {
    RBBIDebugPrintf("%*s", min_چوڑائی, CStr(s)());
}
#اگر_ختم


//-------------------------------------------------------------------------
//
//    print.         Print out the tree of nodes rooted at "this"
//
//-------------------------------------------------------------------------
#اگر_تعریف_شدہ RBBI_DEBUG
خالی RBBINode::print_گرہ_سرخی() {
    RBBIDebugPrintf(" Address   serial        type     LeftChild  RightChild   Parent   position value\n");
}
    
خالی RBBINode::print_درخت(ثابت RBBINode *گرہ, UBool printHeading) {
    اگر (printHeading) {
        print_گرہ_سرخی();
    }
    print_گرہ(گرہ);
    اگر (گرہ != خالی_اشاریہ) {
        // Only dump the definition under a variable reference if asked to.
        // Unconditionally dump children of all other node types.
        اگر (گرہ->fType != varRef) {
            اگر (گرہ->f_بایاں_بچہ != خالی_اشاریہ) {
                print_درخت(گرہ->f_بایاں_بچہ, جھوٹ);
            }
            
            اگر (گرہ->f_دایاں_بچہ != خالی_اشاریہ) {
                print_درخت(گرہ->f_دایاں_بچہ, جھوٹ);
            }
        }
    }
}
#اگر_ختم



U_NAMESPACE_انتہا

#اگر_ختم /* #if !UCONFIG_NO_BREAK_ITERATION */
