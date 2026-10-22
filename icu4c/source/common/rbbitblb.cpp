// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (c) 2002-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
//
//  rbbitblb.cpp
//

#ضمن <functional>

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_BREAK_ITERATION

#ضمن "unicode/unistr.h"
#ضمن "rbbitblb.h"
#ضمن "rbbirb.h"
#ضمن "rbbiscan.h"
#ضمن "rbbisetb.h"
#ضمن "rbbidata.h"
#ضمن "cstring.h"
#ضمن "uassert.h"
#ضمن "uvectr32.h"
#ضمن "cmemory.h"

U_NAMESPACE_BEGIN

نطاق {

// Given the `RBBITableBuilder::fDStates` vector of `RBBIStateDescriptor`s, returns
// true if a state for which `isSink` returns true is reachable from state `source` by following
// transitions without going through any state for which `excludedState` returns true.
// Note that “going through” is distinct from “reaching”, and means entering and then leaving: A
// state can be both a sink and excluded, and can still be reachable.
منطقي reachableByTransitions(ثابت UVector &حالتها, ثابت int32_t منبع,
                            ثابت std::function<منطقي(int32_t)> isSink,
                            ثابت std::function<منطقي(int32_t)> excludedState, UError_کد &وضعیت) {
    UStack boundary(وضعیت);
    {
        UVector32 &transitions_From_منبع =
            *static_cast<RBBIStateDescriptor *>(حالتها.elementAt(منبع))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        لكل (int32_t نماد = 0; نماد < transitions_From_منبع.size(); ++نماد) {
            ثابت int32_t state = transitions_From_منبع.elementAti(نماد);
            إذا (state != 0) {
                boundary.push(state, وضعیت);
            }
        }
    }
    ثابت خودکار دیدهشده = prv::make_unique<منطقي[]>(حالتها.size(), وضعیت);
    طالما (U_SUCCESS(وضعیت) && !boundary.empty()) {
        ثابت int32_t s = boundary.popi();
        إذا (isSink(s)) {
            أرجع درست;
        }
        إذا (excludedState(s) || دیدهشده[s]) {
            استمر;
        }
        دیدهشده[s] = درست;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(حالتها.elementAt(s))->fDtran;
        لكل (int32_t نماد = 0; نماد < transitions.size(); ++نماد) {
            ثابت int32_t t = transitions.elementAti(نماد);
            إذا (t != 0 && !دیدهشده[t]) {
                boundary.push(t, وضعیت);
            }
        }
    }
    أرجع خطأ;
}

} // namespace

ثابت int32_t k_Max_State_For8_بیتها_جدول = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **ریشه_گره, UError_کد &وضعیت) :
        fRB(rb),
        f_درخت(*ریشه_گره),
        f_وضعیت(&وضعیت),
        fDStates(اشاره_تهی),
        f_امن_جدول(اشاره_تهی) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = جديد UVector(وضعیت);
    إذا (U_SUCCESS(وضعیت) && fDStates == اشاره_تهی ) {
        وضعیت = U_حافظه_تخصیص_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    صحيح i;
    لكل (i=0; i<fDStates->size(); i++) {
        حذف static_cast<RBBIStateDescriptor*>(fDStates->elementAt(i));
    }
    حذف fDStates;
    حذف f_امن_جدول;
    حذف f_Look_Ahead_قاعده_Map;
}


//-----------------------------------------------------------------------------
//
//   RBBITableBuilder::buildForwardTable  -  This is the main function for building
//                               the DFA state transition table from the RBBI rules parse tree.
//
//-----------------------------------------------------------------------------
فراغ  RBBITableBuilder::build_Forward_جدول() {

    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    إذا (f_درخت==اشاره_تهی) {
        أرجع;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_درخت = f_درخت->flattenVariables(*f_وضعیت, 0);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
#اگر_تعریف_شده RBBI_DEBUG
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_درخت(f_درخت, درست);
    }
#پایان_اگر

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = جديد RBBINode(RBBINode::opCat, *f_وضعیت);
    // Exit if memory allocation failed.
    إذا (cn == اشاره_تهی) {
        *f_وضعیت = U_حافظه_تخصیص_ERROR;
    }
    إذا (U_FAILURE(*f_وضعیت)) {
        حذف cn;
        أرجع;
    }
    cn->f_چپ_فرزند = f_درخت;
    f_درخت->f_والد = cn;
    RBBINode *انتها_Marker_گره = cn->f_راست_فرزند = جديد RBBINode(RBBINode::انتها_Mark, *f_وضعیت);
    // Delete and exit if memory allocation failed.
    إذا (cn->f_راست_فرزند == اشاره_تهی) {
        *f_وضعیت = U_حافظه_تخصیص_ERROR;
    }
    إذا (U_FAILURE(*f_وضعیت)) {
        حذف cn;
        أرجع;
    }
    cn->f_راست_فرزند->f_والد = cn;
    f_درخت = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_درخت->flattenSets(*f_وضعیت, 0);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
#اگر_تعریف_شده RBBI_DEBUG
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_درخت(f_درخت, درست);
    }
#پایان_اگر


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_درخت);
    calc_اولی_Pos(f_درخت);
    calc_آخرین_Pos(f_درخت);
    calcFollowPos(f_درخت);
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_درخت);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    إذا (fRB->f_Chain_قواعد) {
        calcChainedFollowPos(f_درخت, انتها_Marker_گره);
    }

    //
    // Build the DFA state transition tables.
    //
    build_State_جدول();
    map_Look_Ahead_قواعد();
    flag_Accepting_حالتها();
    flag_Look_Ahead_حالتها();
    flag_Tagged_حالتها();

    //
    // Update the global table of rule status {tag} values
    // The rule builder has a global vector of status values that are common
    //    for all tables.  Merge the ones from this table into the global set.
    //
    ادغام_قاعده_وضعیت_Vals();
}



//-----------------------------------------------------------------------------
//
//   calcNullable.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcNullable(RBBINode *n) {
    إذا (n == اشاره_تهی) {
        أرجع;
    }
    إذا (n->fType == RBBINode::مجموعه_Ref ||
        n->fType == RBBINode::انتها_Mark ) {
        // These are non-empty leaf node types.
        n->fNullable = خطأ;
        أرجع;
    }

    إذا (n->fType == RBBINode::lookAhead || n->fType == RBBINode::برچسب) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = درست;
        أرجع;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_چپ_فرزند);
    calcNullable(n->f_راست_فرزند);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_چپ_فرزند->fNullable || n->f_راست_فرزند->fNullable;
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_چپ_فرزند->fNullable && n->f_راست_فرزند->fNullable;
    }
    وإلا إذا (n->fType == RBBINode::op_ستاره || n->fType == RBBINode::opQuestion) {
        n->fNullable = درست;
    }
    وإلا {
        n->fNullable = خطأ;
    }
}




//-----------------------------------------------------------------------------
//
//   calcFirstPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calc_اولی_Pos(RBBINode *n) {
    إذا (n == اشاره_تهی) {
        أرجع;
    }
    إذا (n->fType == RBBINode::برگ_Char  ||
        n->fType == RBBINode::انتها_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::برچسب) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_اولی_Pos_مجموعه->جمع_کن_Element(n, *f_وضعیت);
        أرجع;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calc_اولی_Pos(n->f_چپ_فرزند);
    calc_اولی_Pos(n->f_راست_فرزند);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        مجموعه_جمع_کن_2(n->f_اولی_Pos_مجموعه, n->f_چپ_فرزند->f_اولی_Pos_مجموعه);
        مجموعه_جمع_کن_2(n->f_اولی_Pos_مجموعه, n->f_راست_فرزند->f_اولی_Pos_مجموعه);
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        مجموعه_جمع_کن_2(n->f_اولی_Pos_مجموعه, n->f_چپ_فرزند->f_اولی_Pos_مجموعه);
        إذا (n->f_چپ_فرزند->fNullable) {
            مجموعه_جمع_کن_2(n->f_اولی_Pos_مجموعه, n->f_راست_فرزند->f_اولی_Pos_مجموعه);
        }
    }
    وإلا إذا (n->fType == RBBINode::op_ستاره ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        مجموعه_جمع_کن_2(n->f_اولی_Pos_مجموعه, n->f_چپ_فرزند->f_اولی_Pos_مجموعه);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calc_آخرین_Pos(RBBINode *n) {
    إذا (n == اشاره_تهی) {
        أرجع;
    }
    إذا (n->fType == RBBINode::برگ_Char  ||
        n->fType == RBBINode::انتها_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::برچسب) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_آخرین_Pos_مجموعه->جمع_کن_Element(n, *f_وضعیت);
        أرجع;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calc_آخرین_Pos(n->f_چپ_فرزند);
    calc_آخرین_Pos(n->f_راست_فرزند);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        مجموعه_جمع_کن_2(n->f_آخرین_Pos_مجموعه, n->f_چپ_فرزند->f_آخرین_Pos_مجموعه);
        مجموعه_جمع_کن_2(n->f_آخرین_Pos_مجموعه, n->f_راست_فرزند->f_آخرین_Pos_مجموعه);
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        مجموعه_جمع_کن_2(n->f_آخرین_Pos_مجموعه, n->f_راست_فرزند->f_آخرین_Pos_مجموعه);
        إذا (n->f_راست_فرزند->fNullable) {
            مجموعه_جمع_کن_2(n->f_آخرین_Pos_مجموعه, n->f_چپ_فرزند->f_آخرین_Pos_مجموعه);
        }
    }
    وإلا إذا (n->fType == RBBINode::op_ستاره     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        مجموعه_جمع_کن_2(n->f_آخرین_Pos_مجموعه, n->f_چپ_فرزند->f_آخرین_Pos_مجموعه);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcFollowPos(RBBINode *n) {
    إذا (n == اشاره_تهی ||
        n->fType == RBBINode::برگ_Char ||
        n->fType == RBBINode::انتها_Mark) {
        أرجع;
    }

    calcFollowPos(n->f_چپ_فرزند);
    calcFollowPos(n->f_راست_فرزند);

    // Aho rule #1
    إذا (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *آخرین_Pos_از_چپ_فرزند = n->f_چپ_فرزند->f_آخرین_Pos_مجموعه;

        لكل (ix = 0; ix < static_cast<uint32_t>(آخرین_Pos_از_چپ_فرزند->size()); ix++) {
            i = static_cast<RBBINode*>(آخرین_Pos_از_چپ_فرزند->elementAt(ix));
            مجموعه_جمع_کن_2(i->fFollowPos, n->f_راست_فرزند->f_اولی_Pos_مجموعه);
        }
    }

    // Aho rule #2
    إذا (n->fType == RBBINode::op_ستاره ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        لكل (ix = 0; ix < static_cast<uint32_t>(n->f_آخرین_Pos_مجموعه->size()); ix++) {
            i = static_cast<RBBINode*>(n->f_آخرین_Pos_مجموعه->elementAt(ix));
            مجموعه_جمع_کن_2(i->fFollowPos, n->f_اولی_Pos_مجموعه);
        }
    }



}

//-----------------------------------------------------------------------------
//
//    addRuleRootNodes    Recursively walk a parse tree, adding all nodes flagged
//                        as roots of a rule to a destination vector.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::جمع_کن_قاعده_ریشه_Nodes(UVector *dest, RBBINode *گره) {
    إذا (گره == اشاره_تهی || U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    U_ASSERT(!dest->hasDeleter());
    إذا (گره->f_قاعده_ریشه) {
        dest->جمع_کن_Element(گره, *f_وضعیت);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        أرجع;
    }
    جمع_کن_قاعده_ریشه_Nodes(dest, گره->f_چپ_فرزند);
    جمع_کن_قاعده_ریشه_Nodes(dest, گره->f_راست_فرزند);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcChainedFollowPos(RBBINode *درخت, RBBINode *انتها_Mark_گره) {

    UVector         برگ_Nodes(*f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }

    // get a list all leaf nodes
    درخت->پیدا_کن_Nodes(&برگ_Nodes, RBBINode::برگ_Char, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector قاعده_ریشه_Nodes(*f_وضعیت);
    جمع_کن_قاعده_ریشه_Nodes(&قاعده_ریشه_Nodes, درخت);

    UVector تطابق_شروع_Nodes(*f_وضعیت);
    لكل (صحيح j=0; j<قاعده_ریشه_Nodes.size(); ++j) {
        RBBINode *گره = static_cast<RBBINode *>(قاعده_ریشه_Nodes.elementAt(j));
        إذا (گره->f_Chain_درون) {
            مجموعه_جمع_کن_2(&تطابق_شروع_Nodes, گره->f_اولی_Pos_مجموعه);
        }
    }
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }

    int32_t  انتها_گره_Ix;
    int32_t  شروع_گره_Ix;

    لكل (انتها_گره_Ix=0; انتها_گره_Ix<برگ_Nodes.size(); انتها_گره_Ix++) {
        RBBINode* انتها_گره = static_cast<RBBINode*>(برگ_Nodes.elementAt(انتها_گره_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        إذا (!انتها_گره->fFollowPos->شامل_است(انتها_Mark_گره)) {
            استمر;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *شروع_گره;
        لكل (شروع_گره_Ix = 0; شروع_گره_Ix<تطابق_شروع_Nodes.size(); شروع_گره_Ix++) {
            شروع_گره = static_cast<RBBINode*>(تطابق_شروع_Nodes.elementAt(شروع_گره_Ix));
            إذا (شروع_گره->fType != RBBINode::برگ_Char) {
                استمر;
            }

            إذا (انتها_گره->fVal == شروع_گره->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                مجموعه_جمع_کن_2(انتها_گره->fFollowPos, شروع_گره->fFollowPos);
            }
        }
    }
}


//-----------------------------------------------------------------------------
//
//   bofFixup.    Fixup for state tables that include {bof} beginning of input testing.
//                Do an swizzle similar to chaining, modifying the followPos set of
//                the bofNode to include the followPos nodes from other {bot} nodes
//                scattered through the tree.
//
//                This function has much in common with calcChainedFollowPos().
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::bofFixup() {

    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }

    //   The parse tree looks like this ...
    //         fTree root  --->       <cat>
    //                               /     \       .
    //                            <cat>   <#end node>
    //                           /     \  .
    //                     <bofNode>   rest
    //                               of tree
    //
    //    We will be adding things to the followPos set of the <bofNode>
    //
    RBBINode  *bof_گره = f_درخت->f_چپ_فرزند->f_چپ_فرزند;
    U_ASSERT(bof_گره->fType == RBBINode::برگ_Char);
    U_ASSERT(bof_گره->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *تطابق_شروع_Nodes = f_درخت->f_چپ_فرزند->f_راست_فرزند->f_اولی_Pos_مجموعه;

    RBBINode *شروع_گره;
    صحيح       شروع_گره_Ix;
    لكل (شروع_گره_Ix = 0; شروع_گره_Ix<تطابق_شروع_Nodes->size(); شروع_گره_Ix++) {
        شروع_گره = static_cast<RBBINode*>(تطابق_شروع_Nodes->elementAt(شروع_گره_Ix));
        إذا (شروع_گره->fType != RBBINode::برگ_Char) {
            استمر;
        }

        إذا (شروع_گره->fVal == bof_گره->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            مجموعه_جمع_کن_2(bof_گره->fFollowPos, شروع_گره->fFollowPos);
        }
    }
}

//-----------------------------------------------------------------------------
//
//   buildStateTable()    Determine the set of runtime DFA states and the
//                        transition tables for these states, by the algorithm
//                        of fig. 3.44 in Aho.
//
//                        Most of the comments are quotes of Aho's pseudo-code.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::build_State_جدول() {
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = اشاره_تهی;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    صحيح      آخرین_درون_داد_نماد = fRB->f_مجموعه_Builder->get_رقم_Char_Categories() - 1;
    failState = جديد RBBIStateDescriptor(آخرین_درون_داد_نماد, f_وضعیت);
    إذا (failState == اشاره_تهی) {
        *f_وضعیت = U_حافظه_تخصیص_ERROR;
        اذهب ExitBuildSTdeleteall;
    }
    failState->fPositions = جديد UVector(*f_وضعیت);
    إذا (failState->fPositions == اشاره_تهی) {
        *f_وضعیت = U_حافظه_تخصیص_ERROR;
    }
    إذا (failState->fPositions == اشاره_تهی || U_FAILURE(*f_وضعیت)) {
        اذهب ExitBuildSTdeleteall;
    }
    fDStates->جمع_کن_Element(failState, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        اذهب ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = جديد RBBIStateDescriptor(آخرین_درون_داد_نماد, f_وضعیت);
    إذا (initialState == اشاره_تهی) {
        *f_وضعیت = U_حافظه_تخصیص_ERROR;
    }
    إذا (U_FAILURE(*f_وضعیت)) {
        اذهب ExitBuildSTdeleteall;
    }
    initialState->fPositions = جديد UVector(*f_وضعیت);
    إذا (initialState->fPositions == اشاره_تهی) {
        *f_وضعیت = U_حافظه_تخصیص_ERROR;
    }
    إذا (U_FAILURE(*f_وضعیت)) {
        اذهب ExitBuildSTdeleteall;
    }
    مجموعه_جمع_کن_2(initialState->fPositions, f_درخت->f_اولی_Pos_مجموعه);
    fDStates->جمع_کن_Element(initialState, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        اذهب ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    لكل (;;) {
        RBBIStateDescriptor *T = اشاره_تهی;
        int32_t              tx;
        لكل (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *موقت;
            موقت = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            إذا (موقت->fMarked == خطأ) {
                T = موقت;
                اكسر;
            }
        }
        إذا (T == اشاره_تهی) {
            اكسر;
        }

        // mark T;
        T->fMarked = درست;

        // for each input symbol a do begin
        int32_t  a;
        لكل (a = 1; a<=آخرین_درون_داد_نماد; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = اشاره_تهی;
            RBBINode   *p;
            int32_t     px;
            لكل (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                إذا ((p->fType == RBBINode::برگ_Char) &&  (p->fVal == a)) {
                    إذا (U == اشاره_تهی) {
                        U = جديد UVector(*f_وضعیت);
                        إذا (U == اشاره_تهی) {
                        	*f_وضعیت = U_حافظه_تخصیص_ERROR;
                        	اذهب ExitBuildSTdeleteall;
                        }
                    }
                    مجموعه_جمع_کن_2(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = خطأ;
            إذا (U != اشاره_تهی) {
                U_ASSERT(U->size() > 0);
                صحيح  ix;
                لكل (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    إذا (مجموعه_Equals(U, temp2->fPositions)) {
                        حذف U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = درست;
                        اكسر;
                    }
                }

                // Add U as an unmarked state to Dstates
                إذا (!UinDstates)
                {
                    RBBIStateDescriptor *newState = جديد RBBIStateDescriptor(آخرین_درون_داد_نماد, f_وضعیت);
                    إذا (newState == اشاره_تهی) {
                    	*f_وضعیت = U_حافظه_تخصیص_ERROR;
                    }
                    إذا (U_FAILURE(*f_وضعیت)) {
                        اذهب ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->جمع_کن_Element(newState, *f_وضعیت);
                    إذا (U_FAILURE(*f_وضعیت)) {
                        أرجع;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->مجموعه_Element_At(ux, a);
            }
        }
    }
    أرجع;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    حذف initialState;
    حذف failState;
}


/**
 * mapLookAheadRules
 *
 */
فراغ RBBITableBuilder::map_Look_Ahead_قواعد() {
    f_Look_Ahead_قاعده_Map =  جديد UVector32(fRB->fScanner->رقم_قواعد() + 1, *f_وضعیت);
    إذا (f_Look_Ahead_قاعده_Map == اشاره_تهی) {
        *f_وضعیت = U_حافظه_تخصیص_ERROR;
    }
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    f_Look_Ahead_قاعده_Map->مجموعه_اندازه(fRB->fScanner->رقم_قواعد() + 1);

    لكل (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t la_جایگاه_For_State = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        منطقي saw_Look_Ahead_گره = خطأ;
        لكل (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *گره = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            إذا (گره->fType != RBBINode::گره_Type::lookAhead) {
                استمر;
            }
            saw_Look_Ahead_گره = درست;
            int32_t قاعده_رقم = گره->fVal;     // Set when rule was originally parsed.
            U_ASSERT(قاعده_رقم < f_Look_Ahead_قاعده_Map->size());
            U_ASSERT(قاعده_رقم > 0);
            int32_t la_جایگاه = f_Look_Ahead_قاعده_Map->elementAti(قاعده_رقم);
            إذا (la_جایگاه != 0) {
                إذا (la_جایگاه_For_State == 0) {
                    la_جایگاه_For_State = la_جایگاه;
                } وإلا {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(la_جایگاه == la_جایگاه_For_State);
                }
            }
        }
        إذا (!saw_Look_Ahead_گره) {
            استمر;
        }

        إذا (la_جایگاه_For_State == 0) {
            la_جایگاه_For_State = ++f_LASlots_درون_Use;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        لكل (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *گره = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            إذا (گره->fType != RBBINode::گره_Type::lookAhead) {
                استمر;
            }
            int32_t قاعده_رقم = گره->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = f_Look_Ahead_قاعده_Map->elementAti(قاعده_رقم);
            (فراغ)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == la_جایگاه_For_State);
            f_Look_Ahead_قاعده_Map->مجموعه_Element_At(la_جایگاه_For_State, قاعده_رقم);
        }
    }

}

//-----------------------------------------------------------------------------
//
//   flagAcceptingStates    Identify accepting states.
//                          First get a list of all of the end marker nodes.
//                          Then, for each state s,
//                              if s contains one of the end marker nodes in its list of tree positions then
//                                  s is an accepting state.
//
//-----------------------------------------------------------------------------
فراغ     RBBITableBuilder::flag_Accepting_حالتها() {
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    UVector     انتها_Marker_Nodes(*f_وضعیت);
    RBBINode    *انتها_Marker;
    int32_t     i;
    int32_t     n;

    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }

    f_درخت->پیدا_کن_Nodes(&انتها_Marker_Nodes, RBBINode::انتها_Mark, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }

    لكل (i=0; i<انتها_Marker_Nodes.size(); i++) {
        انتها_Marker = static_cast<RBBINode*>(انتها_Marker_Nodes.elementAt(i));
        لكل (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            إذا (sd->fPositions->شاخص_از(انتها_Marker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                إذا (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = f_Look_Ahead_قاعده_Map->elementAti(انتها_Marker->fVal);
                    إذا (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                إذا (sd->fAccepting==ACCEPTING_UNCONDITIONAL && انتها_Marker->fVal != 0) {
                    // Both lookahead and non-lookahead accepting for this state.
                    // Favor the look-ahead, because a look-ahead match needs to
                    // immediately stop the run-time engine. First match, not longest.
                    sd->fAccepting = f_Look_Ahead_قاعده_Map->elementAti(انتها_Marker->fVal);
                }
                // implicit else:
                // if sd->fAccepting already had a value other than 0 or 1, leave it be.
            }
        }
    }
}


//-----------------------------------------------------------------------------
//
//    flagLookAheadStates   Very similar to flagAcceptingStates, above.
//
//-----------------------------------------------------------------------------
فراغ     RBBITableBuilder::flag_Look_Ahead_حالتها() {
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    UVector     lookAheadNodes(*f_وضعیت);
    RBBINode    *look_Ahead_گره;
    int32_t     i;
    int32_t     n;

    f_درخت->پیدا_کن_Nodes(&lookAheadNodes, RBBINode::lookAhead, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    لكل (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_گره = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_گره->fType == RBBINode::گره_Type::lookAhead);

        لكل (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->شاخص_از(look_Ahead_گره);
            إذا (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_گره == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookahead_جایگاه = f_Look_Ahead_قاعده_Map->elementAti(look_Ahead_گره->fVal);
                U_ASSERT(sd->fLookAhead == 0 || sd->fLookAhead == lookahead_جایگاه);
                // if (sd->fLookAhead != 0 && sd->fLookAhead != lookaheadSlot) {
                //     printf("%s:%d Bingo. sd->fLookAhead:%d   lookaheadSlot:%d\n",
                //            __FILE__, __LINE__, sd->fLookAhead, lookaheadSlot);
                // }
                sd->fLookAhead = lookahead_جایگاه;
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//    flagTaggedStates
//
//-----------------------------------------------------------------------------
فراغ     RBBITableBuilder::flag_Tagged_حالتها() {
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    UVector     برچسب_Nodes(*f_وضعیت);
    RBBINode    *برچسب_گره;
    int32_t     i;
    int32_t     n;

    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    f_درخت->پیدا_کن_Nodes(&برچسب_Nodes, RBBINode::برچسب, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    لكل (i=0; i<برچسب_Nodes.size(); i++) {                   // For each tag node t (all of 'em)
        برچسب_گره = static_cast<RBBINode*>(برچسب_Nodes.elementAt(i));

        لكل (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            إذا (sd->fPositions->شاخص_از(برچسب_گره) >= 0) {  //       if  s include the tag node t
                sorted_جمع_کن(&sd->f_برچسب_Vals, برچسب_گره->fVal);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//  mergeRuleStatusVals
//
//      Update the global table of rule status {tag} values
//      The rule builder has a global vector of status values that are common
//      for all tables.  Merge the ones from this table into the global set.
//
//-----------------------------------------------------------------------------
فراغ  RBBITableBuilder::ادغام_قاعده_وضعیت_Vals() {
    //
    //  The basic outline of what happens here is this...
    //
    //    for each state in this state table
    //       if the status tag list for this state is in the global statuses list
    //           record where and
    //           continue with the next state
    //       else
    //           add the tag list for this state to the global list.
    //
    صحيح i;
    صحيح n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    إذا (fRB->f_قاعده_وضعیت_Vals->size() == 0) {
        fRB->f_قاعده_وضعیت_Vals->جمع_کن_Element(1, *f_وضعیت);  // Num of statuses in group
        fRB->f_قاعده_وضعیت_Vals->جمع_کن_Element(static_cast<int32_t>(0), *f_وضعیت); // and our single status of zero
    }

    //    For each state
    لكل (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_حالتها_برچسب_مقادیر = sd->f_برچسب_Vals;
        إذا (this_حالتها_برچسب_مقادیر == اشاره_تهی) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->f_برچسبها_Idx = 0;
            استمر;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->f_برچسبها_Idx = -1;
        int32_t  this_برچسب_گروه_شروع = 0;   // indexes into the global rule status vals list
        int32_t  بعدی_برچسب_گروه_شروع = 0;

        // Loop runs once per group of tags in the global list
        طالما (بعدی_برچسب_گروه_شروع < fRB->f_قاعده_وضعیت_Vals->size()) {
            this_برچسب_گروه_شروع = بعدی_برچسب_گروه_شروع;
            بعدی_برچسب_گروه_شروع += fRB->f_قاعده_وضعیت_Vals->elementAti(this_برچسب_گروه_شروع) + 1;
            إذا (this_حالتها_برچسب_مقادیر->size() != fRB->f_قاعده_وضعیت_Vals->elementAti(this_برچسب_گروه_شروع)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                استمر;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            لكل (i=0; i<this_حالتها_برچسب_مقادیر->size(); i++) {
                إذا (this_حالتها_برچسب_مقادیر->elementAti(i) !=
                    fRB->f_قاعده_وضعیت_Vals->elementAti(this_برچسب_گروه_شروع + 1 + i) ) {
                    // Mismatch.
                    اكسر;
                }
            }

            إذا (i == this_حالتها_برچسب_مقادیر->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->f_برچسبها_Idx = this_برچسب_گروه_شروع;
                اكسر;
            }
        }

        إذا (sd->f_برچسبها_Idx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->f_برچسبها_Idx = fRB->f_قاعده_وضعیت_Vals->size();
            fRB->f_قاعده_وضعیت_Vals->جمع_کن_Element(this_حالتها_برچسب_مقادیر->size(), *f_وضعیت);
            لكل (i=0; i<this_حالتها_برچسب_مقادیر->size(); i++) {
                fRB->f_قاعده_وضعیت_Vals->جمع_کن_Element(this_حالتها_برچسب_مقادیر->elementAti(i), *f_وضعیت);
            }
        }
    }
}





//-----------------------------------------------------------------------------
//
//    minimizeStates    Minimize the number of states of the DFA, by Algorithm
//                      3.6 in Aho.  The one twist is that instead of starting
//                      from a partition in two groups (accepting and non-
//                      accepting states), we partition according to all
//                      relevant properties of the states, namely the values of:
//                      - fAccepting—which may be non-accepting,
//                        unconditionally accepting, or the index of a
//                        lookahead—;
//                      - fLookAhead;
//                      - fTagsIdx.
//
//-----------------------------------------------------------------------------

فراغ RBBITableBuilder::minimize_حالتها() {
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }

#إذا UPRV_HAS_SANITIZER
    إذا (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *f_وضعیت = U_REGEX_الگو_TOO_BIG;
        أرجع;
    }
#پایان_اگر
    بنية StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t f_برچسبها_Idx;
        منطقي عملگر==(StateType ثابت &other) ثابت {
            أرجع fAccepting == other.fAccepting &&
                   fLookAhead == other.fLookAhead &&
                   f_برچسبها_Idx == other.f_برچسبها_Idx;
        }
    };
    // We wrap `UVector`s in `unique_ptr``s throughout so we can move them,
    // including into `UVector`s (by releasing them from the `unique_ptr`
    // and having the enclosing `UVector` adopt them). 
    // Group the states by types (but we have no maps so this is verbose).
    // If there are no lookaheads and no tags, there are only two types
    // (accepting and non-accepting) in which case this is exactly step 1
    // of Algorithm 3.6.
    بنية Type_مقصد_حالتها : UMemory {
        Type_مقصد_حالتها(ثابت StateType &type, UError_کد &وضعیت)
            : type(type), حالتها(prv::make_unique<UVector>(وضعیت, وضعیت)) {}
        StateType type;
        prv::unique_ptr<UVector> حالتها;
    };
    UVector initial_افراز(*f_وضعیت);
    initial_افراز.مجموعه_Deleter(
        [](فراغ *p) { حذف static_cast<Type_مقصد_حالتها *>(p); });
    لكل (int32_t i = 0; i < fDStates->size(); ++i) {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ثابت StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.f_برچسبها_Idx};
        int32_t j = 0;
        لكل (; j < initial_افراز.size(); ++j) {
            خودکار &[type_j, حالتها] =
                *static_cast<Type_مقصد_حالتها *>(initial_افراز.elementAt(j));
            إذا (type_j == type) {
                حالتها->جمع_کن_Element(i, *f_وضعیت);
                اكسر;
            }
        }
        إذا (j == initial_افراز.size()) {
            خودکار new_مدخل = prv::make_unique<Type_مقصد_حالتها>(type, *f_وضعیت, *f_وضعیت);
            إذا (U_FAILURE(*f_وضعیت)) {
                أرجع;
            }
            new_مدخل->حالتها->جمع_کن_Element(i, *f_وضعیت);
            initial_افراز.adoptElement(new_مدخل.release(), *f_وضعیت);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    خودکار افراز = prv::make_unique<UVector>(*f_وضعیت, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    افراز->مجموعه_Deleter(
        [](فراغ *p) { حذف static_cast<UVector*>(p); });
    
    لكل (int32_t i = 0; i < initial_افراز.size(); ++i) {
        افراز->adoptElement(
            static_cast<Type_مقصد_حالتها*>(initial_افراز.elementAt(i))->حالتها.release(),
            *f_وضعیت);
    }
    // Given the index of a state 𝑠 in `fDStates`, returns a UVector of integers
    // σ(𝑠) such that 𝑠.fDtran[i] (the index of the state reached by the
    // transition from 𝑠 on the character class with index i) is in Π[σ(𝑠)[i]].
    // We then have σ(𝑠) == σ(𝑡) if and only if “for all input symbols 𝑎, states
    // 𝑠 and 𝑡 have transitions on 𝑎 to states in the same group of Π” (which is
    // what defines the refinement of 𝐺 in Fig. 3.45).
    خودکار افراز_signature = [&افراز, هذا](int32_t state_شاخص) -> prv::unique_ptr<UVector> {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state_شاخص));
        خودکار نتیجه = prv::make_unique<UVector>(state.fDtran->size(), *f_وضعیت, *f_وضعیت);
        إذا (U_FAILURE(*f_وضعیت)) {
            أرجع اشاره_تهی;
        }
        لكل (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t مقصد_State = state.fDtran->elementAti(i);
            لكل (int32_t part_شاخص = 0; part_شاخص < افراز->size(); ++part_شاخص) {
                ثابت UVector &part = *static_cast<UVector *>(افراز->elementAt(part_شاخص));
                إذا (part.شامل_است(مقصد_State)) {
                    نتیجه->جمع_کن_Element(part_شاخص, *f_وضعیت);
                    اكسر;
                }
            }
        }
        نتیجه->مجموعه_Comparer([](ثابت UElement چپ, ثابت UElement راست) -> UBool {
            أرجع چپ.integer == راست.integer;
        });
        أرجع نتیجه;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    لكل (;;) {
        // Π_new.
        خودکار افراز_New = prv::make_unique<UVector>(*f_وضعیت, *f_وضعیت);
        إذا (U_FAILURE(*f_وضعیت)) {
            أرجع;
        }
        افراز_New->مجموعه_Deleter(
            [](فراغ *p) { حذف static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        منطقي refined = خطأ;
        لكل (int32_t i = 0; i < افراز->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ثابت UVector &گروه = *static_cast<UVector*>(افراز->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            بنية Signature_مقصد_حالتها : UMemory {
                Signature_مقصد_حالتها(prv::unique_ptr<UVector> signature, UError_کد &وضعیت)
                    : signature(std::move(signature)), حالتها(prv::make_unique<UVector>(وضعیت, وضعیت)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> حالتها;
            };
            UVector گروه_Refinement(*f_وضعیت);
            گروه_Refinement.مجموعه_Deleter([](فراغ *p) { حذف static_cast<Signature_مقصد_حالتها*>(p); });
            لكل (int32_t j = 0; j < گروه.size(); ++j) {
                // Index of a state in the group.
                ثابت int32_t s = گروه.elementAti(j);
                خودکار signature = افراز_signature(s);
                int32_t k = 0;
                لكل (; k < گروه_Refinement.size(); ++k) {
                    ثابت خودکار &[subgroupSignature, subgroup] =
                        *static_cast<Signature_مقصد_حالتها*>(گروه_Refinement.elementAt(k));
                    إذا (*subgroupSignature == *signature) {
                        subgroup->جمع_کن_Element(s, *f_وضعیت);
                        اكسر;
                    }
                }
                إذا (k == گروه_Refinement.size()) {
                    ثابت خودکار new_مدخل =
                        جديد Signature_مقصد_حالتها(std::move(signature), *f_وضعیت);
                    إذا (U_FAILURE(*f_وضعیت)) {
                        أرجع;
                    }
                    إذا (new_مدخل == اشاره_تهی) {
                      *f_وضعیت = U_حافظه_تخصیص_ERROR;
                      أرجع;
                    }
                    new_مدخل->حالتها->جمع_کن_Element(s, *f_وضعیت);
                    گروه_Refinement.adoptElement(new_مدخل, *f_وضعیت);
                }
            }
            refined |= گروه_Refinement.size() > 1;
            لكل (int32_t j = 0; j < گروه_Refinement.size(); ++j) {
                خودکار &[_, subgroup] = *static_cast<Signature_مقصد_حالتها*>(
                    گروه_Refinement.elementAt(j));
                افراز_New->adoptElement(subgroup.release(), *f_وضعیت);
            }
        }
        إذا (refined) {
            افراز = std::move(افراز_New);
        } وإلا {
            اكسر;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    لكل (صحيح i = 0; i < افراز->size(); ++i) {
        UVector *ثابت part_i = static_cast<UVector *>(افراز->elementAt(i));
        إذا (part_i->شامل_است(0)) {
            فراغ *ثابت part_0 = افراز->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ثابت خودکار deleter = افراز->مجموعه_Deleter(اشاره_تهی);
            افراز->مجموعه_Element_At(part_i, 0);
            افراز->مجموعه_Element_At(part_0, i);
            افراز->مجموعه_Deleter(deleter);
        } وإلا إذا (part_i->شامل_است(1)) {
            فراغ *ثابت part_1 = افراز->elementAt(1);
            ثابت خودکار deleter = افراز->مجموعه_Deleter(اشاره_تهی);
            افراز->مجموعه_Element_At(part_i, 1);
            افراز->مجموعه_Element_At(part_1, i);
            افراز->مجموعه_Deleter(deleter);
        }
    }
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    خودکار old_State_مقصد_Part = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    لكل (صحيح i = 0; i < افراز->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(افراز->elementAt(i));
        لكل (صحيح j = 0; j < part.size(); ++j) {
            old_State_مقصد_Part[part.elementAti(j)] = i;
        }
    }
    خودکار ثابت old_حالتها = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*f_وضعیت, *f_وضعیت).release();
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    لكل (صحيح i = 0; i < افراز->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(افراز->elementAt(i));
        RBBIStateDescriptor *ثابت state =
            static_cast<RBBIStateDescriptor*>(old_حالتها->elementAt(part.elementAti(0)));
        fDStates->جمع_کن_Element(state, *f_وضعیت);
        لكل (صحيح j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->مجموعه_Element_At(old_State_مقصد_Part[state->fDtran->elementAti(j)], j);
        }
        old_حالتها->مجموعه_Element_At(اشاره_تهی, part.elementAti(0));
    }
    old_حالتها->مجموعه_Deleter([](فراغ *p) { حذف static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::sorted_جمع_کن(UVector **vector, int32_t val) {
    int32_t i;

    إذا (*vector == اشاره_تهی) {
        *vector = جديد UVector(*f_وضعیت);
    }
    إذا (*vector == اشاره_تهی || U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    UVector *vec = *vector;
    int32_t  v_اندازه = vec->size();
    لكل (i=0; i<v_اندازه; i++) {
        int32_t valAtI = vec->elementAti(i);
        إذا (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            أرجع;
        }
        إذا (valAtI > val) {
            اكسر;
        }
    }
    vec->درج_Element_At(val, i, *f_وضعیت);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::مجموعه_جمع_کن_2(UVector *dest, UVector *منبع) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!منبع->hasDeleter());
    int32_t dest_Original_اندازه = dest->size();
    int32_t منبع_اندازه       = منبع->size();
    int32_t di           = 0;
    Maybe_پشته_Array<فراغ *, 16> destArray, منبع_Array;  // Handle small cases without malloc
    فراغ **destPtr, **منبع_Ptr;
    فراغ **destLim, **منبع_Lim;

    إذا (dest_Original_اندازه > destArray.get_ظرفیت()) {
        إذا (destArray.resize(dest_Original_اندازه) == اشاره_تهی) {
            أرجع;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + dest_Original_اندازه;  // destArray.getArrayLimit()?

    إذا (منبع_اندازه > منبع_Array.get_ظرفیت()) {
        إذا (منبع_Array.resize(منبع_اندازه) == اشاره_تهی) {
            أرجع;
        }
    }
    منبع_Ptr = منبع_Array.getAlias();
    منبع_Lim = منبع_Ptr + منبع_اندازه;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (فراغ) dest->مقصد_Array(destPtr);
    (فراغ) منبع->مقصد_Array(منبع_Ptr);

    dest->مجموعه_اندازه(منبع_اندازه+dest_Original_اندازه, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }

    طالما (منبع_Ptr < منبع_Lim && destPtr < destLim) {
        إذا (*destPtr == *منبع_Ptr) {
            dest->مجموعه_Element_At(*منبع_Ptr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        وإلا إذا (uprv_memcmp(destPtr, منبع_Ptr, حجم(فراغ *)) < 0) {
            dest->مجموعه_Element_At(*destPtr++, di++);
        }
        وإلا { /* *sourcePtr < *destPtr */
            dest->مجموعه_Element_At(*منبع_Ptr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    طالما (destPtr < destLim) {
        dest->مجموعه_Element_At(*destPtr++, di++);
    }
    طالما (منبع_Ptr < منبع_Lim) {
        dest->مجموعه_Element_At(*منبع_Ptr++, di++);
    }

    dest->مجموعه_اندازه(di, *f_وضعیت);
}



//-----------------------------------------------------------------------------
//
//  setEqual    Set operation on UVector.
//              Compare for equality.
//              Elements must be sorted.
//
//-----------------------------------------------------------------------------
UBool RBBITableBuilder::مجموعه_Equals(UVector *a, UVector *b) {
    أرجع a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#اگر_تعریف_شده RBBI_DEBUG
فراغ RBBITableBuilder::printPosSets(RBBINode *n) {
    إذا (n==اشاره_تهی) {
        أرجع;
    }
    چاپ_کن("\n");
    RBBINode::print_گره_سرآیند();
    RBBINode::print_گره(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    print_مجموعه(n->f_اولی_Pos_مجموعه);

    RBBIDebugPrintf("         lastpos:   ");
    print_مجموعه(n->f_آخرین_Pos_مجموعه);

    RBBIDebugPrintf("         followpos: ");
    print_مجموعه(n->fFollowPos);

    printPosSets(n->f_چپ_فرزند);
    printPosSets(n->f_راست_فرزند);
}
#پایان_اگر

//
//    findDuplCharClassFrom()
//
منطقي RBBITableBuilder::پیدا_کن_Dupl_Char_Class_From(IntPair *categories) {
    int32_t رقم_حالتها = fDStates->size();
    int32_t رقم_تعداد_ستون = fRB->f_مجموعه_Builder->get_رقم_Char_Categories();

    لكل (; categories->first < رقم_تعداد_ستون-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        صحيح حد_دومی = categories->first < fRB->f_مجموعه_Builder->get_Dict_Categories_شروع() ?
            fRB->f_مجموعه_Builder->get_Dict_Categories_شروع() : رقم_تعداد_ستون;
        لكل (categories->دومی=categories->first+1; categories->دومی < حد_دومی; categories->دومی++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t جدول_پایه = 0;
            uint16_t جدول_dupl = 1;
            لكل (int32_t state=0; state<رقم_حالتها; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                جدول_پایه = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                جدول_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->دومی));
                إذا (جدول_پایه != جدول_dupl) {
                    اكسر;
                }
            }
            إذا (جدول_پایه == جدول_dupl) {
                أرجع درست;
            }
        }
    }
    أرجع خطأ;
}


//
//    removeColumn()
//
فراغ RBBITableBuilder::remove_ستونی(int32_t ستونی) {
    int32_t رقم_حالتها = fDStates->size();
    لكل (int32_t state=0; state<رقم_حالتها; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(ستونی < sd->fDtran->size());
        sd->fDtran->removeElementAt(ستونی);
    }
}


منطقي RBBITableBuilder::پیدا_کن_Duplicate_امن_State(IntPair *حالتها) {
    int32_t رقم_حالتها = f_امن_جدول->size();

    لكل (; حالتها->first<رقم_حالتها-1; حالتها->first++) {
        UnicodeString *اولی_ردیف = static_cast<UnicodeString *>(f_امن_جدول->elementAt(حالتها->first));
        لكل (حالتها->دومی=حالتها->first+1; حالتها->دومی<رقم_حالتها; حالتها->دومی++) {
            UnicodeString *dupl_ردیف = static_cast<UnicodeString *>(f_امن_جدول->elementAt(حالتها->دومی));
            منطقي ردیفها_تطابق = درست;
            int32_t رقم_تعداد_ستون = اولی_ردیف->length();
            لكل (int32_t ستون=0; ستون < رقم_تعداد_ستون; ++ستون) {
                int32_t اولی_Val = اولی_ردیف->charAt(ستون);
                int32_t duplVal = dupl_ردیف->charAt(ستون);
                إذا (!((اولی_Val == duplVal) ||
                        ((اولی_Val == حالتها->first || اولی_Val == حالتها->دومی) &&
                        (duplVal  == حالتها->first || duplVal  == حالتها->دومی)))) {
                    ردیفها_تطابق = خطأ;
                    اكسر;
                }
            }
            إذا (ردیفها_تطابق) {
                أرجع درست;
            }
        }
    }
    أرجع خطأ;
}

فراغ RBBITableBuilder::remove_امن_State(IntPair dupl_حالتها) {
    ثابت int32_t keepState = dupl_حالتها.first;
    ثابت int32_t duplState = dupl_حالتها.دومی;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < f_امن_جدول->size());

    f_امن_جدول->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t رقم_حالتها = f_امن_جدول->size();
    لكل (int32_t state=0; state<رقم_حالتها; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(f_امن_جدول->elementAt(state));
        int32_t رقم_تعداد_ستون = sd->length();
        لكل (int32_t ستون=0; ستون<رقم_تعداد_ستون; ستون++) {
            int32_t existingVal = sd->charAt(ستون);
            int32_t newVal = existingVal;
            إذا (existingVal == duplState) {
                newVal = keepState;
            } وإلا إذا (existingVal > duplState) {
                newVal = existingVal - 1;
            }
            sd->مجموعه_Char_At(ستون, static_cast<char16_t>(newVal));
        }
    }
}


/*
 * RemoveDuplicateStates
 */
int32_t RBBITableBuilder::remove_Duplicate_حالتها() {
    ثابت int32_t old_State_شمار = fDStates->size();
    minimize_حالتها();
    أرجع fDStates->size() - old_State_شمار;
}


/*
 * Minimizes the number of slots used by the lookaheads.
 *
 * When the state machine is first generated, every lookahead rule occupies a different slot, for instance,
 * given
 *   x / z;  # Lookahead 1 below.
 *   y / z;  # Lookahead 2 below.
 *   [xyz]*;
 * the following state machine is produced,
 *           x
 *         x ⮏  z
 *         → 𝑠₁ → 𝑎₁
 *   START  x⇅y
 *     ↻z  → 𝑠₂ → 𝑎₂
 *         y ↻  z
 *           y
 *
 * where 𝑠ᵢ sets the current position for lookahead i (assigning pᵢ=p_current), and 𝑎ᵢ accepts
 * lookahead i if set, i.e., on state 𝑎ᵢ,the break iterator returns pᵢ if set. Because they accept
 * different lookaheads, states 𝑎₁ and 𝑎₂ are distinct in the initial partition constructed by
 * `minimizeStates`, and cannot be merged, and the state machine cannot be simplified.
 *
 * However, there is no need for lookaheads 1 and 2 to occupy different slots, i.e., there is no
 * need for the state machine to separately store the positions set by 𝑠₁ and 𝑠₂ (for p₁ and p₂ to
 * be distinct variables). Indeed, if state 𝑠₁ is encountered, state 𝑎₂ cannot be reached without
 * going through state 𝑠₂, and vice versa, so if p₁ and p₂ are backed by the same variable p, the
 * value set by p=p₁=p_current on 𝑠₁ will have been overriden by p=p₂=p_current on 𝑠₂, so that the
 * p₁-p₂ merger has no effect.
 *
 * If the lookaheads are merged, states 𝑎₁ and 𝑎₂ are no longer initially distinct in
 * `minimizeStates` (they both accept the only lookahead), and indeed the whole state machine
 * simplifies to
 *         [xy]     z
 *   START   →  𝑠  →  𝑎
 *     ↻z       ↻
 *             [xy]
 *
 * Lookaheads i and j can be merged if:
 * 1. From any state that sets lookahead i, no state that accepts lookahead j can be reached without
 *    going through a state that sets lookahead j; and
 * 2. From any state that sets lookahead j, no state that accepts lookahead i can be reached without
 *    going through a state that sets lookahead i.
 * This reachability relation defines a directed graph of lookaheads, and an optimal merging of
 * lookaheads is then a colouring of that graph. (In the example above, the graph of lookaheads has
 * no edges, and is thus 1-colourable; more interesting examples can be found in the test
 * TestLookaheadPolychromy.)
 *
 * This function first computes the adjacency matrix of lookahead reachability, and then colours the
 * graph of lookaheads and reassigns lookahead slots accordingly.
 */
فراغ RBBITableBuilder::minimizeLookaheads() {
    إذا (f_LASlots_درون_Use == ACCEPTING_UNCONDITIONAL) {
        أرجع;
    }
    ثابت int32_t lookahead_شمار = f_LASlots_درون_Use - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ثابت int32_t اولی_Lookahead = ACCEPTING_UNCONDITIONAL + 1;
    ثابت خودکار stateDescriptor = [هذا](ثابت int32_t state) -> RBBIStateDescriptor & {
        أرجع *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ثابت خودکار lookaheadReachability =
        prv::make_unique<منطقي[]>(lookahead_شمار * lookahead_شمار, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    لكل (int32_t l = اولی_Lookahead; l <= f_LASlots_درون_Use; ++l) {
        لكل (int32_t k = اولی_Lookahead; k <= f_LASlots_درون_Use; ++k) {
            إذا (k != l) {
                لكل (int32_t منبع = 1; منبع < fDStates->size(); ++منبع) {
                    إذا (static_cast<int32_t>(stateDescriptor(منبع).fLookAhead) != l) {
                        استمر;
                    }
                    إذا (reachableByTransitions(
                            *fDStates, منبع, /*isSink=*/
                            [&](ثابت int32_t state) {
                                أرجع static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](ثابت int32_t state) {
                                أرجع static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *f_وضعیت)) {
                        lookaheadReachability[(l - اولی_Lookahead) * lookahead_شمار + k -
                                              اولی_Lookahead] = درست;
                    }
                }
            }
        }
    }

    ثابت خودکار colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_شمار, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    لكل (int32_t chromatic_عدد = 1;; ++chromatic_عدد) {
        uprv_memset(colours.get(), 0, حجم(int32_t) * lookahead_شمار);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        لكل (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            لكل (صحيح منبع = 0; منبع < lookahead_شمار; ++منبع) {
                لكل (صحيح sink = 0; sink < lookahead_شمار; ++sink) {
                    إذا (lookaheadReachability[منبع * lookahead_شمار + sink] &&
                        colours[منبع] == colours[sink]) {
                        اذهب بعدی_Colouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            لكل (صحيح i = 0; i < fDStates->size(); ++i) {
                إذا (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        اولی_Lookahead +
                        colours[stateDescriptor(i).fAccepting - اولی_Lookahead];
                }
                إذا (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - اولی_Lookahead];
                }
            }
            f_LASlots_درون_Use = ACCEPTING_UNCONDITIONAL + chromatic_عدد;
            أرجع;

          بعدی_Colouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            لكل (int32_t i = 0; i < lookahead_شمار - 1 && colours[i] == chromatic_عدد;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            إذا (colours[lookahead_شمار - 1] == chromatic_عدد) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                اكسر;
            }
        }
    }
}


//-----------------------------------------------------------------------------
//
//   getTableSize()    Calculate the size of the runtime form of this
//                     state transition table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::get_جدول_اندازه() ثابت {
    int32_t    size = 0;
    int32_t    رقم_ردیفها;
    int32_t    رقم_تعداد_ستون;
    int32_t    ردیف_اندازه;

    إذا (f_درخت == اشاره_تهی) {
        أرجع 0;
    }

    size    = offsetof(RBBIState_جدول, f_جدول_داده);    // The header, with no rows to the table.

    رقم_ردیفها = fDStates->size();
    رقم_تعداد_ستون = fRB->f_مجموعه_Builder->get_رقم_Char_Categories();

    إذا (use8_بیتها_For_جدول()) {
        ردیف_اندازه = offsetof(RBBIState_جدول_Row8, f_بعدی_State) + حجم(int8_t)*رقم_تعداد_ستون;
    } وإلا {
        ردیف_اندازه = offsetof(RBBIState_جدول_Row16, f_بعدی_State) + حجم(int16_t)*رقم_تعداد_ستون;
    }
    size   += رقم_ردیفها * ردیف_اندازه;
    أرجع size;
}

منطقي RBBITableBuilder::use8_بیتها_For_جدول() ثابت {
    أرجع fDStates->size() <= k_Max_State_For8_بیتها_جدول;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::export_جدول(فراغ *where) {
    RBBIState_جدول* جدول = static_cast<RBBIState_جدول*>(where);
    uint32_t           state;
    صحيح                ستون;

    إذا (U_FAILURE(*f_وضعیت) || f_درخت == اشاره_تهی) {
        أرجع;
    }

    int32_t cat_شمار = fRB->f_مجموعه_Builder->get_رقم_Char_Categories();
    إذا (cat_شمار > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *f_وضعیت = U_BRK_داخلی_ERROR;
        أرجع;
    }

    جدول->f_رقم_حالتها = fDStates->size();
    جدول->f_Dict_Categories_شروع = fRB->f_مجموعه_Builder->get_Dict_Categories_شروع();
    جدول->f_Look_Ahead_Results_اندازه = f_LASlots_درون_Use == ACCEPTING_UNCONDITIONAL ? 0 : f_LASlots_درون_Use + 1;
    جدول->fFlags     = 0;
    إذا (use8_بیتها_For_جدول()) {
        جدول->f_ردیف_Len    = offsetof(RBBIState_جدول_Row8, f_بعدی_State) + حجم(uint8_t) * cat_شمار;
        جدول->fFlags  |= RBBI_8_بیتها_ردیفها;
    } وإلا {
        جدول->f_ردیف_Len    = offsetof(RBBIState_جدول_Row16, f_بعدی_State) + حجم(int16_t) * cat_شمار;
    }
    إذا (fRB->fLookAheadHardBreak) {
        جدول->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    لكل (state=0; state<جدول->f_رقم_حالتها; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIState_جدول_ردیف* ردیف = reinterpret_cast<RBBIState_جدول_ردیف*>(جدول->f_جدول_داده + state * جدول->f_ردیف_Len);
        إذا (use8_بیتها_For_جدول()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->f_برچسبها_Idx && sd->f_برچسبها_Idx <= 255);
            RBBIState_جدول_Row8* r8 = reinterpret_cast<RBBIState_جدول_Row8*>(ردیف);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->f_برچسبها_Idx   = sd->f_برچسبها_Idx;
            لكل (ستون=0; ستون<cat_شمار; ستون++) {
                U_ASSERT (sd->fDtran->elementAti(ستون) <= k_Max_State_For8_بیتها_جدول);
                r8->f_بعدی_State[ستون] = sd->fDtran->elementAti(ستون);
            }
        } وإلا {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->f_برچسبها_Idx && sd->f_برچسبها_Idx <= 0xffff);
            ردیف->r16.fAccepting = sd->fAccepting;
            ردیف->r16.fLookAhead = sd->fLookAhead;
            ردیف->r16.f_برچسبها_Idx   = sd->f_برچسبها_Idx;
            لكل (ستون=0; ستون<cat_شمار; ستون++) {
                ردیف->r16.f_بعدی_State[ستون] = sd->fDtran->elementAti(ستون);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
فراغ RBBITableBuilder::build_امن_معکوس_جدول(UError_کد &وضعیت) {
    // The safe table creation has three steps:

    // 1. Identify pairs of character classes that are "safe." Safe means that boundaries
    // following the pair do not depend on context or state before the pair. To test
    // whether a pair is safe, run it through the main forward state table, starting
    // from each possible configuration. If the final configuration is the same, no matter what the
    // starting configuration, the pair is safe.
    // Note that the configuration includes not only the state, but also the last accepting position
    // and the lookaheads.  Some differences in final lookaheads can be ignored when it can be shown
    // that they do not affect breaks after the pair: this happens when we come back out of the pair
    // with a matching configuration when the lookahead matches.
    // TODO(egg): It is possible for pairs to be safe even if the outgoing states differ depending
    // on the state before the pair; this would require a more thorough analysis to handle, probably
    // in the style of what happens in minimizeStates.
    //
    // 2. Build a state table that recognizes the safe pairs. It's similar to their
    // forward table, with a column for each input character [class], and a row for
    // each state. Row 1 is the start state, and row 0 is the stop state. Initially
    // create an additional state for each input character category; being in
    // one of these states means that the character has been seen, and is potentially
    // the first of a pair. In each of these rows, the entry for the second character
    // of a safe pair is set to the stop state (0), indicating that a match was found.
    // All other table entries are set to the state corresponding the current input
    // character, allowing that character to be the of a start following pair.
    //
    // Because the safe rules are to be run in reverse, moving backwards in the text,
    // the first and second pair categories are swapped when building the table.
    //
    // 3. Compress the table. There are typically many rows (states) that are
    // equivalent - that have zeroes (match completed) in the same columns -
    // and can be folded together.

    // Each safe pair is stored as two UChars in the safePair string.
    UnicodeString امن_جفتها;

    int32_t رقم_Char_Classes = fRB->f_مجموعه_Builder->get_رقم_Char_Categories();
    int32_t رقم_حالتها = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ثابت خودکار lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_درون_Use + 1, وضعیت);
    ثابت خودکار wanted_Lookaheads_درون_Pair =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_درون_Use + 1, وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t شروع_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_مجموعه = -1;
    لكل (int32_t c1 = 1; c1 < رقم_Char_Classes; ++c1) {
        إذا (fRB->f_مجموعه_Builder->get_اولی_Char(c1) < 0) {
          استمر;
        }
        لكل (int32_t c2 = 1; c2 < رقم_Char_Classes; ++c2) {
            إذا (fRB->f_مجموعه_Builder->get_اولی_Char(c2) < 0) {
                استمر;
            }
            int32_t wanted_انتها_State = -1;
            int32_t wanted_آخرین_Accepting_Position;
            منطقي must_Check_Consistent_With_شروع_C2 = خطأ;
            ثابت int32_t متن[2] = {c1, c2};
            لكل (int32_t injectedState = شروع_STATE; injectedState < رقم_حالتها; ++injectedState) {
                لكل (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= f_LASlots_درون_Use; ++injectedLookahead) {
                    int32_t آخرین_Break = BEFORE_PAIR;
                    int32_t آخرین_Accepting_Position = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_مجموعه, f_LASlots_درون_Use + 1);
                    إذا (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ثابت RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        إذا (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            آخرین_Accepting_Position = 0;
                        } وإلا إذا (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            استمر;
                        }
                        إذا (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    اذهب dfa_گام;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    لكل (;;) {
                        s = شروع_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_مجموعه, f_LASlots_درون_Use + 1);
                        i = آخرین_Break;
                        // Loop Find_Next_Break in L2/26-135.
                        لكل (;;) {
                        dfa_گام:
                            إذا (i == BEFORE_PAIR || i == 2) {
                                اذهب inspectFinalState;
                            }
                            ثابت int32_t نماد_Ahead = متن[i];
                            ++i;
                            ثابت RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            إذا (sDescriptor->fDtran->elementAti(نماد_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(نماد_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } وإلا {
                                إذا (آخرین_Break != BEFORE_PAIR &&
                                    آخرین_Accepting_Position == آخرین_Break) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    اذهب بعدی_نماد_Pair;
                                }
                                آخرین_Break = آخرین_Accepting_Position;
                                اكسر;
                            }

                            إذا (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                آخرین_Accepting_Position = i;
                            } وإلا إذا (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_مجموعه) {
                                آخرین_Break = lookaheadPositions[sDescriptor->fAccepting];
                                اكسر;
                            }
                            إذا (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    إذا (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        استمر;
                    }
                    إذا (wanted_انتها_State < 0) {
                        wanted_انتها_State = s;
                        wanted_آخرین_Accepting_Position = آخرین_Accepting_Position;
                        لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_درون_Use; ++l) {
                            إذا (lookaheadPositions[l] != LOOKAHEAD_NOT_مجموعه &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wanted_Lookaheads_درون_Pair[l] = lookaheadPositions[i];
                            } وإلا {
                                wanted_Lookaheads_درون_Pair[l] = LOOKAHEAD_NOT_مجموعه;
                            }
                        }
                    } وإلا {
                        إذا (wanted_انتها_State != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            اذهب بعدی_نماد_Pair;
                        }
                        إذا (wanted_آخرین_Accepting_Position != آخرین_Accepting_Position) {
                            اذهب بعدی_نماد_Pair;
                        }
                    }
                    // A lookahead some distance before the pair (set to BEFORE_PAIR) means we
                    // will come back to the pair in a different configuration if it matches, a
                    // lookahead at position 0 means we get back to the pair on the start state
                    // if it matches; either way, this is covered by other iterations of the
                    // loop over injected states and lookaheads.
                    // For lookaheads at position 1 (in the middle of the pair) and 2 (after the
                    // pair), these have two ways of being safe: either they are always set, or it
                    // does not matter if they are set, because if they match we come back with the
                    // same state.
                    لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_درون_Use; ++l) {
                        إذا (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wanted_Lookaheads_درون_Pair[l]) {
                            استمر;
                        }
                        إذا (lookaheadPositions[l] == 2 || wanted_Lookaheads_درون_Pair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            إذا (wanted_انتها_State != شروع_STATE) {
                                اذهب بعدی_نماد_Pair;
                            }
                        }
                        إذا (lookaheadPositions[l] == 1 || wanted_Lookaheads_درون_Pair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            must_Check_Consistent_With_شروع_C2 = درست;
                        }
                    }
                }
            }
            إذا (must_Check_Consistent_With_شروع_C2) {
                ثابت RBBIStateDescriptor& شروع_Descriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(شروع_STATE));
                ثابت خودکار c2State = شروع_Descriptor.fDtran->elementAti(c2);
                إذا (c2State != wanted_انتها_State) {
                    اذهب بعدی_نماد_Pair;
                }
                ثابت RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                إذا (wanted_آخرین_Accepting_Position != 2) {
                    اذهب بعدی_نماد_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_درون_Use; ++l) {
                    إذا (((wanted_Lookaheads_درون_Pair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wanted_انتها_State != شروع_STATE) {
                        اذهب بعدی_نماد_Pair;
                    }
                }
            }
            امن_جفتها.append(static_cast<char16_t>(c1));
            امن_جفتها.append(static_cast<char16_t>(c2));
        بعدی_نماد_Pair:
            استمر;
        }
    }

    // Populate the initial safe table.
    // The table as a whole is UVector<UnicodeString>
    // Each row is represented by a UnicodeString, being used as a Vector<int16>.
    // Row 0 is the stop state.
    // Row 1 is the start state.
    // Row 2 and beyond are other states, initially one per char class, but
    //   after initial construction, many of the states will be combined, compacting the table.
    // The String holds the nextState data only. The four leading fields of a row, fAccepting,
    // fLookAhead, etc. are not needed for the safe table, and are omitted at this stage of building.

    U_ASSERT(f_امن_جدول == اشاره_تهی);
    Local_اشاره_گر<UVector> lp_امن_جدول(
        جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, رقم_Char_Classes + 2, وضعیت), وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    f_امن_جدول = lp_امن_جدول.orphan();
    لكل (int32_t ردیف=0; ردیف<رقم_Char_Classes + 2; ++ردیف) {
        Local_اشاره_گر<UnicodeString> lpString(جديد UnicodeString(رقم_Char_Classes, 0, رقم_Char_Classes+4), وضعیت);
        f_امن_جدول->adoptElement(lpString.orphan(), وضعیت);
    }
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &شروع_State = *static_cast<UnicodeString *>(f_امن_جدول->elementAt(1));
    لكل (int32_t charClass=0; charClass < رقم_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        شروع_State.مجموعه_Char_At(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    لكل (int32_t ردیف=2; ردیف<رقم_Char_Classes+2; ++ردیف) {
        UnicodeString &ردیف_State = *static_cast<UnicodeString *>(f_امن_جدول->elementAt(ردیف));
        ردیف_State = شروع_State;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    لكل (int32_t pairIdx=0; pairIdx<امن_جفتها.length(); pairIdx+=2) {
        int32_t c1 = امن_جفتها.charAt(pairIdx);
        int32_t c2 = امن_جفتها.charAt(pairIdx + 1);

        UnicodeString &ردیف_State = *static_cast<UnicodeString *>(f_امن_جدول->elementAt(c2 + 2));
        ردیف_State.مجموعه_Char_At(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair حالتها = {1, 0};
    طالما (پیدا_کن_Duplicate_امن_State(&حالتها)) {
        // printf("Removing duplicate safe states (%d, %d)\n", states.first, states.second);
        remove_امن_State(حالتها);
    }
}


//-----------------------------------------------------------------------------
//
//   getSafeTableSize()    Calculate the size of the runtime form of this
//                         safe state table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::get_امن_جدول_اندازه() ثابت {
    int32_t    size = 0;
    int32_t    رقم_ردیفها;
    int32_t    رقم_تعداد_ستون;
    int32_t    ردیف_اندازه;

    إذا (f_امن_جدول == اشاره_تهی) {
        أرجع 0;
    }

    size    = offsetof(RBBIState_جدول, f_جدول_داده);    // The header, with no rows to the table.

    رقم_ردیفها = f_امن_جدول->size();
    رقم_تعداد_ستون = fRB->f_مجموعه_Builder->get_رقم_Char_Categories();

    إذا (use8_بیتها_For_امن_جدول()) {
        ردیف_اندازه = offsetof(RBBIState_جدول_Row8, f_بعدی_State) + حجم(int8_t)*رقم_تعداد_ستون;
    } وإلا {
        ردیف_اندازه = offsetof(RBBIState_جدول_Row16, f_بعدی_State) + حجم(int16_t)*رقم_تعداد_ستون;
    }
    size   += رقم_ردیفها * ردیف_اندازه;
    أرجع size;
}

منطقي RBBITableBuilder::use8_بیتها_For_امن_جدول() ثابت {
    أرجع f_امن_جدول->size() <= k_Max_State_For8_بیتها_جدول;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::export_امن_جدول(فراغ *where) {
    RBBIState_جدول* جدول = static_cast<RBBIState_جدول*>(where);
    uint32_t           state;
    صحيح                ستون;

    إذا (U_FAILURE(*f_وضعیت) || f_امن_جدول == اشاره_تهی) {
        أرجع;
    }

    int32_t cat_شمار = fRB->f_مجموعه_Builder->get_رقم_Char_Categories();
    إذا (cat_شمار > 0x7fff ||
            f_امن_جدول->size() > 0x7fff) {
        *f_وضعیت = U_BRK_داخلی_ERROR;
        أرجع;
    }

    جدول->f_رقم_حالتها = f_امن_جدول->size();
    جدول->fFlags     = 0;
    إذا (use8_بیتها_For_امن_جدول()) {
        جدول->f_ردیف_Len    = offsetof(RBBIState_جدول_Row8, f_بعدی_State) + حجم(uint8_t) * cat_شمار;
        جدول->fFlags  |= RBBI_8_بیتها_ردیفها;
    } وإلا {
        جدول->f_ردیف_Len    = offsetof(RBBIState_جدول_Row16, f_بعدی_State) + حجم(int16_t) * cat_شمار;
    }

    لكل (state=0; state<جدول->f_رقم_حالتها; state++) {
        UnicodeString* ردیف_String = static_cast<UnicodeString*>(f_امن_جدول->elementAt(state));
        RBBIState_جدول_ردیف* ردیف = reinterpret_cast<RBBIState_جدول_ردیف*>(جدول->f_جدول_داده + state * جدول->f_ردیف_Len);
        إذا (use8_بیتها_For_امن_جدول()) {
            RBBIState_جدول_Row8* r8 = reinterpret_cast<RBBIState_جدول_Row8*>(ردیف);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->f_برچسبها_Idx    = 0;
            لكل (ستون=0; ستون<cat_شمار; ستون++) {
                U_ASSERT(ردیف_String->charAt(ستون) <= k_Max_State_For8_بیتها_جدول);
                r8->f_بعدی_State[ستون] = static_cast<uint8_t>(ردیف_String->charAt(ستون));
            }
        } وإلا {
            ردیف->r16.fAccepting = 0;
            ردیف->r16.fLookAhead = 0;
            ردیف->r16.f_برچسبها_Idx    = 0;
            لكل (ستون=0; ستون<cat_شمار; ستون++) {
                ردیف->r16.f_بعدی_State[ستون] = ردیف_String->charAt(ستون);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//   printSet    Debug function.   Print the contents of a UVector
//
//-----------------------------------------------------------------------------
#اگر_تعریف_شده RBBI_DEBUG
فراغ RBBITableBuilder::print_مجموعه(UVector *s) {
    int32_t  i;
    لكل (i=0; i<s->size(); i++) {
        ثابت RBBINode *v = static_cast<ثابت RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==اشاره_تهی? -1 : v->f_Serial_رقم);
    }
    RBBIDebugPrintf("\n");
}
#پایان_اگر


//-----------------------------------------------------------------------------
//
//   printStates    Debug Function.  Dump the fully constructed state transition table.
//
//-----------------------------------------------------------------------------
#اگر_تعریف_شده RBBI_DEBUG
فراغ RBBITableBuilder::print_حالتها() {
    صحيح     c;    // input "character"
    صحيح     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    لكل (c=0; c<fRB->f_مجموعه_Builder->get_رقم_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    لكل (c=0; c<fRB->f_مجموعه_Builder->get_رقم_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    لكل (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->f_برچسبها_Idx);
        لكل (c=0; c<fRB->f_مجموعه_Builder->get_رقم_Char_Categories(); c++) {
            RBBIDebugPrintf(" %3d", sd->fDtran->elementAti(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#پایان_اگر


//-----------------------------------------------------------------------------
//
//   printSafeTable    Debug Function.  Dump the fully constructed safe table.
//
//-----------------------------------------------------------------------------
#اگر_تعریف_شده RBBI_DEBUG
فراغ RBBITableBuilder::print_معکوس_جدول() {
    صحيح     c;    // input "character"
    صحيح     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    إذا (f_امن_جدول == اشاره_تهی) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        أرجع;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    لكل (c=0; c<fRB->f_مجموعه_Builder->get_رقم_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    لكل (c=0; c<fRB->f_مجموعه_Builder->get_رقم_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    لكل (n=0; n<f_امن_جدول->size(); n++) {
        UnicodeString *ردیف_String = (UnicodeString *)f_امن_جدول->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        لكل (c=0; c<fRB->f_مجموعه_Builder->get_رقم_Char_Categories(); c++) {
            RBBIDebugPrintf(" %2d", ردیف_String->charAt(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#پایان_اگر



//-----------------------------------------------------------------------------
//
//   printRuleStatusTable    Debug Function.  Dump the common rule status table
//
//-----------------------------------------------------------------------------
#اگر_تعریف_شده RBBI_DEBUG
فراغ RBBITableBuilder::print_قاعده_وضعیت_جدول() {
    int32_t  this_رکورد = 0;
    int32_t  بعدی_رکورد = 0;
    صحيح      i;
    UVector  *tbl = fRB->f_قاعده_وضعیت_Vals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    طالما (بعدی_رکورد < tbl->size()) {
        this_رکورد = بعدی_رکورد;
        بعدی_رکورد = this_رکورد + tbl->elementAti(this_رکورد) + 1;
        RBBIDebugPrintf("%4d   ", this_رکورد);
        لكل (i=this_رکورد+1; i<بعدی_رکورد; i++) {
            RBBIDebugPrintf("  %5d", tbl->elementAti(i));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#پایان_اگر


//-----------------------------------------------------------------------------
//
//   RBBIStateDescriptor     Methods.  This is a very struct-like class
//                           Most access is directly to the fields.
//
//-----------------------------------------------------------------------------

RBBIStateDescriptor::RBBIStateDescriptor(صحيح آخرین_درون_داد_نماد, UError_کد *f_وضعیت) {
    fMarked    = خطأ;
    fAccepting = 0;
    fLookAhead = 0;
    f_برچسبها_Idx   = 0;
    f_برچسب_Vals   = اشاره_تهی;
    fPositions = اشاره_تهی;
    fDtran     = اشاره_تهی;

    fDtran     = جديد UVector32(آخرین_درون_داد_نماد+1, *f_وضعیت);
    إذا (U_FAILURE(*f_وضعیت)) {
        أرجع;
    }
    إذا (fDtran == اشاره_تهی) {
        *f_وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    fDtran->مجموعه_اندازه(آخرین_درون_داد_نماد+1);    // fDtran needs to be pre-sized.
                                           //   It is indexed by input symbols, and will
                                           //   hold  the next state number for each
                                           //   symbol.
}


RBBIStateDescriptor::~RBBIStateDescriptor() {
    حذف       fPositions;
    حذف       fDtran;
    حذف       f_برچسب_Vals;
    fPositions = اشاره_تهی;
    fDtran     = اشاره_تهی;
    f_برچسب_Vals   = اشاره_تهی;
}

U_NAMESPACE_انتها

#پایان_اگر /* #if !UCONFIG_NO_BREAK_ITERATION */
