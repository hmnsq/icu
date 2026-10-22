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
منطقي reachableByTransitions(ثابت UVector &حالتونه, ثابت int32_t سرچینه,
                            ثابت std::function<منطقي(int32_t)> isSink,
                            ثابت std::function<منطقي(int32_t)> excludedState, UError_کوډ &حالت) {
    UStack boundary(حالت);
    {
        UVector32 &transitions_From_سرچینه =
            *static_cast<RBBIStateDescriptor *>(حالتونه.elementAt(سرچینه))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        لكل (int32_t نښه = 0; نښه < transitions_From_سرچینه.size(); ++نښه) {
            ثابت int32_t state = transitions_From_سرچینه.elementAti(نښه);
            إذا (state != 0) {
                boundary.push(state, حالت);
            }
        }
    }
    ثابت خپلکاری لیدل_شوی = prv::make_unique<منطقي[]>(حالتونه.size(), حالت);
    طالما (U_SUCCESS(حالت) && !boundary.empty()) {
        ثابت int32_t s = boundary.popi();
        إذا (isSink(s)) {
            أرجع ریښتیا;
        }
        إذا (excludedState(s) || لیدل_شوی[s]) {
            استمر;
        }
        لیدل_شوی[s] = ریښتیا;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(حالتونه.elementAt(s))->fDtran;
        لكل (int32_t نښه = 0; نښه < transitions.size(); ++نښه) {
            ثابت int32_t t = transitions.elementAti(نښه);
            إذا (t != 0 && !لیدل_شوی[t]) {
                boundary.push(t, حالت);
            }
        }
    }
    أرجع خطأ;
}

} // namespace

ثابت int32_t k_Max_State_For8_بټونه_جدول = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **ریښه_غوټه, UError_کوډ &حالت) :
        fRB(rb),
        f_ونه(*ریښه_غوټه),
        f_حالت(&حالت),
        fDStates(تش_اشاره),
        f_خوندي_جدول(تش_اشاره) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = جديد UVector(حالت);
    إذا (U_SUCCESS(حالت) && fDStates == تش_اشاره ) {
        حالت = U_حافظه_ویش_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    صحيح i;
    لكل (i=0; i<fDStates->size(); i++) {
        ړنګول static_cast<RBBIStateDescriptor*>(fDStates->elementAt(i));
    }
    ړنګول fDStates;
    ړنګول f_خوندي_جدول;
    ړنګول f_Look_Ahead_قاعده_Map;
}


//-----------------------------------------------------------------------------
//
//   RBBITableBuilder::buildForwardTable  -  This is the main function for building
//                               the DFA state transition table from the RBBI rules parse tree.
//
//-----------------------------------------------------------------------------
فراغ  RBBITableBuilder::build_Forward_جدول() {

    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    إذا (f_ونه==تش_اشاره) {
        أرجع;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_ونه = f_ونه->flattenVariables(*f_حالت, 0);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
#که_تعریف_شوی RBBI_DEBUG
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_ونه(f_ونه, ریښتیا);
    }
#د_که_پای

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = جديد RBBINode(RBBINode::opCat, *f_حالت);
    // Exit if memory allocation failed.
    إذا (cn == تش_اشاره) {
        *f_حالت = U_حافظه_ویش_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        ړنګول cn;
        أرجع;
    }
    cn->f_کیڼ_ماشوم = f_ونه;
    f_ونه->f_پلار = cn;
    RBBINode *پای_ټکی_Marker_غوټه = cn->f_ښی_ماشوم = جديد RBBINode(RBBINode::پای_ټکی_Mark, *f_حالت);
    // Delete and exit if memory allocation failed.
    إذا (cn->f_ښی_ماشوم == تش_اشاره) {
        *f_حالت = U_حافظه_ویش_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        ړنګول cn;
        أرجع;
    }
    cn->f_ښی_ماشوم->f_پلار = cn;
    f_ونه = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_ونه->flattenSets(*f_حالت, 0);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
#که_تعریف_شوی RBBI_DEBUG
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_ونه(f_ونه, ریښتیا);
    }
#د_که_پای


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_ونه);
    calc_لومړی_Pos(f_ونه);
    calc_وروستی_Pos(f_ونه);
    calcFollowPos(f_ونه);
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_ونه);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    إذا (fRB->f_Chain_قواعد) {
        calcChainedFollowPos(f_ونه, پای_ټکی_Marker_غوټه);
    }

    //
    // Build the DFA state transition tables.
    //
    build_State_جدول();
    map_Look_Ahead_قواعد();
    flag_Accepting_حالتونه();
    flag_Look_Ahead_حالتونه();
    flag_Tagged_حالتونه();

    //
    // Update the global table of rule status {tag} values
    // The rule builder has a global vector of status values that are common
    //    for all tables.  Merge the ones from this table into the global set.
    //
    یوځای_قاعده_حالت_Vals();
}



//-----------------------------------------------------------------------------
//
//   calcNullable.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcNullable(RBBINode *n) {
    إذا (n == تش_اشاره) {
        أرجع;
    }
    إذا (n->fType == RBBINode::سېټ_Ref ||
        n->fType == RBBINode::پای_ټکی_Mark ) {
        // These are non-empty leaf node types.
        n->fNullable = خطأ;
        أرجع;
    }

    إذا (n->fType == RBBINode::lookAhead || n->fType == RBBINode::نښان) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = ریښتیا;
        أرجع;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_کیڼ_ماشوم);
    calcNullable(n->f_ښی_ماشوم);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_کیڼ_ماشوم->fNullable || n->f_ښی_ماشوم->fNullable;
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_کیڼ_ماشوم->fNullable && n->f_ښی_ماشوم->fNullable;
    }
    وإلا إذا (n->fType == RBBINode::op_ستوری || n->fType == RBBINode::opQuestion) {
        n->fNullable = ریښتیا;
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
فراغ RBBITableBuilder::calc_لومړی_Pos(RBBINode *n) {
    إذا (n == تش_اشاره) {
        أرجع;
    }
    إذا (n->fType == RBBINode::ونې_پاڼه_Char  ||
        n->fType == RBBINode::پای_ټکی_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::نښان) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_لومړی_Pos_سېټ->جمع_کړه_Element(n, *f_حالت);
        أرجع;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calc_لومړی_Pos(n->f_کیڼ_ماشوم);
    calc_لومړی_Pos(n->f_ښی_ماشوم);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        سېټ_جمع_کړه_2(n->f_لومړی_Pos_سېټ, n->f_کیڼ_ماشوم->f_لومړی_Pos_سېټ);
        سېټ_جمع_کړه_2(n->f_لومړی_Pos_سېټ, n->f_ښی_ماشوم->f_لومړی_Pos_سېټ);
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        سېټ_جمع_کړه_2(n->f_لومړی_Pos_سېټ, n->f_کیڼ_ماشوم->f_لومړی_Pos_سېټ);
        إذا (n->f_کیڼ_ماشوم->fNullable) {
            سېټ_جمع_کړه_2(n->f_لومړی_Pos_سېټ, n->f_ښی_ماشوم->f_لومړی_Pos_سېټ);
        }
    }
    وإلا إذا (n->fType == RBBINode::op_ستوری ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        سېټ_جمع_کړه_2(n->f_لومړی_Pos_سېټ, n->f_کیڼ_ماشوم->f_لومړی_Pos_سېټ);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calc_وروستی_Pos(RBBINode *n) {
    إذا (n == تش_اشاره) {
        أرجع;
    }
    إذا (n->fType == RBBINode::ونې_پاڼه_Char  ||
        n->fType == RBBINode::پای_ټکی_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::نښان) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_وروستی_Pos_سېټ->جمع_کړه_Element(n, *f_حالت);
        أرجع;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calc_وروستی_Pos(n->f_کیڼ_ماشوم);
    calc_وروستی_Pos(n->f_ښی_ماشوم);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        سېټ_جمع_کړه_2(n->f_وروستی_Pos_سېټ, n->f_کیڼ_ماشوم->f_وروستی_Pos_سېټ);
        سېټ_جمع_کړه_2(n->f_وروستی_Pos_سېټ, n->f_ښی_ماشوم->f_وروستی_Pos_سېټ);
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        سېټ_جمع_کړه_2(n->f_وروستی_Pos_سېټ, n->f_ښی_ماشوم->f_وروستی_Pos_سېټ);
        إذا (n->f_ښی_ماشوم->fNullable) {
            سېټ_جمع_کړه_2(n->f_وروستی_Pos_سېټ, n->f_کیڼ_ماشوم->f_وروستی_Pos_سېټ);
        }
    }
    وإلا إذا (n->fType == RBBINode::op_ستوری     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        سېټ_جمع_کړه_2(n->f_وروستی_Pos_سېټ, n->f_کیڼ_ماشوم->f_وروستی_Pos_سېټ);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcFollowPos(RBBINode *n) {
    إذا (n == تش_اشاره ||
        n->fType == RBBINode::ونې_پاڼه_Char ||
        n->fType == RBBINode::پای_ټکی_Mark) {
        أرجع;
    }

    calcFollowPos(n->f_کیڼ_ماشوم);
    calcFollowPos(n->f_ښی_ماشوم);

    // Aho rule #1
    إذا (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *وروستی_Pos_د_کیڼ_ماشوم = n->f_کیڼ_ماشوم->f_وروستی_Pos_سېټ;

        لكل (ix = 0; ix < static_cast<uint32_t>(وروستی_Pos_د_کیڼ_ماشوم->size()); ix++) {
            i = static_cast<RBBINode*>(وروستی_Pos_د_کیڼ_ماشوم->elementAt(ix));
            سېټ_جمع_کړه_2(i->fFollowPos, n->f_ښی_ماشوم->f_لومړی_Pos_سېټ);
        }
    }

    // Aho rule #2
    إذا (n->fType == RBBINode::op_ستوری ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        لكل (ix = 0; ix < static_cast<uint32_t>(n->f_وروستی_Pos_سېټ->size()); ix++) {
            i = static_cast<RBBINode*>(n->f_وروستی_Pos_سېټ->elementAt(ix));
            سېټ_جمع_کړه_2(i->fFollowPos, n->f_لومړی_Pos_سېټ);
        }
    }



}

//-----------------------------------------------------------------------------
//
//    addRuleRootNodes    Recursively walk a parse tree, adding all nodes flagged
//                        as roots of a rule to a destination vector.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::جمع_کړه_قاعده_ریښه_Nodes(UVector *dest, RBBINode *غوټه) {
    إذا (غوټه == تش_اشاره || U_FAILURE(*f_حالت)) {
        أرجع;
    }
    U_ASSERT(!dest->hasDeleter());
    إذا (غوټه->f_قاعده_ریښه) {
        dest->جمع_کړه_Element(غوټه, *f_حالت);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        أرجع;
    }
    جمع_کړه_قاعده_ریښه_Nodes(dest, غوټه->f_کیڼ_ماشوم);
    جمع_کړه_قاعده_ریښه_Nodes(dest, غوټه->f_ښی_ماشوم);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcChainedFollowPos(RBBINode *ونه, RBBINode *پای_ټکی_Mark_غوټه) {

    UVector         ونې_پاڼه_Nodes(*f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    // get a list all leaf nodes
    ونه->پیدا_کړه_Nodes(&ونې_پاڼه_Nodes, RBBINode::ونې_پاڼه_Char, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector قاعده_ریښه_Nodes(*f_حالت);
    جمع_کړه_قاعده_ریښه_Nodes(&قاعده_ریښه_Nodes, ونه);

    UVector سمون_پیل_Nodes(*f_حالت);
    لكل (صحيح j=0; j<قاعده_ریښه_Nodes.size(); ++j) {
        RBBINode *غوټه = static_cast<RBBINode *>(قاعده_ریښه_Nodes.elementAt(j));
        إذا (غوټه->f_Chain_دننه) {
            سېټ_جمع_کړه_2(&سمون_پیل_Nodes, غوټه->f_لومړی_Pos_سېټ);
        }
    }
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    int32_t  پای_ټکی_غوټه_Ix;
    int32_t  پیل_غوټه_Ix;

    لكل (پای_ټکی_غوټه_Ix=0; پای_ټکی_غوټه_Ix<ونې_پاڼه_Nodes.size(); پای_ټکی_غوټه_Ix++) {
        RBBINode* پای_ټکی_غوټه = static_cast<RBBINode*>(ونې_پاڼه_Nodes.elementAt(پای_ټکی_غوټه_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        إذا (!پای_ټکی_غوټه->fFollowPos->لري(پای_ټکی_Mark_غوټه)) {
            استمر;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *پیل_غوټه;
        لكل (پیل_غوټه_Ix = 0; پیل_غوټه_Ix<سمون_پیل_Nodes.size(); پیل_غوټه_Ix++) {
            پیل_غوټه = static_cast<RBBINode*>(سمون_پیل_Nodes.elementAt(پیل_غوټه_Ix));
            إذا (پیل_غوټه->fType != RBBINode::ونې_پاڼه_Char) {
                استمر;
            }

            إذا (پای_ټکی_غوټه->fVal == پیل_غوټه->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                سېټ_جمع_کړه_2(پای_ټکی_غوټه->fFollowPos, پیل_غوټه->fFollowPos);
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

    إذا (U_FAILURE(*f_حالت)) {
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
    RBBINode  *bof_غوټه = f_ونه->f_کیڼ_ماشوم->f_کیڼ_ماشوم;
    U_ASSERT(bof_غوټه->fType == RBBINode::ونې_پاڼه_Char);
    U_ASSERT(bof_غوټه->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *سمون_پیل_Nodes = f_ونه->f_کیڼ_ماشوم->f_ښی_ماشوم->f_لومړی_Pos_سېټ;

    RBBINode *پیل_غوټه;
    صحيح       پیل_غوټه_Ix;
    لكل (پیل_غوټه_Ix = 0; پیل_غوټه_Ix<سمون_پیل_Nodes->size(); پیل_غوټه_Ix++) {
        پیل_غوټه = static_cast<RBBINode*>(سمون_پیل_Nodes->elementAt(پیل_غوټه_Ix));
        إذا (پیل_غوټه->fType != RBBINode::ونې_پاڼه_Char) {
            استمر;
        }

        إذا (پیل_غوټه->fVal == bof_غوټه->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            سېټ_جمع_کړه_2(bof_غوټه->fFollowPos, پیل_غوټه->fFollowPos);
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
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = تش_اشاره;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    صحيح      وروستی_ننوت_نښه = fRB->f_سېټ_Builder->get_عدد_Char_Categories() - 1;
    failState = جديد RBBIStateDescriptor(وروستی_ننوت_نښه, f_حالت);
    إذا (failState == تش_اشاره) {
        *f_حالت = U_حافظه_ویش_ERROR;
        اذهب ExitBuildSTdeleteall;
    }
    failState->fPositions = جديد UVector(*f_حالت);
    إذا (failState->fPositions == تش_اشاره) {
        *f_حالت = U_حافظه_ویش_ERROR;
    }
    إذا (failState->fPositions == تش_اشاره || U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }
    fDStates->جمع_کړه_Element(failState, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = جديد RBBIStateDescriptor(وروستی_ننوت_نښه, f_حالت);
    إذا (initialState == تش_اشاره) {
        *f_حالت = U_حافظه_ویش_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }
    initialState->fPositions = جديد UVector(*f_حالت);
    إذا (initialState->fPositions == تش_اشاره) {
        *f_حالت = U_حافظه_ویش_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }
    سېټ_جمع_کړه_2(initialState->fPositions, f_ونه->f_لومړی_Pos_سېټ);
    fDStates->جمع_کړه_Element(initialState, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    لكل (;;) {
        RBBIStateDescriptor *T = تش_اشاره;
        int32_t              tx;
        لكل (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *لنډمهاله;
            لنډمهاله = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            إذا (لنډمهاله->fMarked == خطأ) {
                T = لنډمهاله;
                اكسر;
            }
        }
        إذا (T == تش_اشاره) {
            اكسر;
        }

        // mark T;
        T->fMarked = ریښتیا;

        // for each input symbol a do begin
        int32_t  a;
        لكل (a = 1; a<=وروستی_ننوت_نښه; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = تش_اشاره;
            RBBINode   *p;
            int32_t     px;
            لكل (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                إذا ((p->fType == RBBINode::ونې_پاڼه_Char) &&  (p->fVal == a)) {
                    إذا (U == تش_اشاره) {
                        U = جديد UVector(*f_حالت);
                        إذا (U == تش_اشاره) {
                        	*f_حالت = U_حافظه_ویش_ERROR;
                        	اذهب ExitBuildSTdeleteall;
                        }
                    }
                    سېټ_جمع_کړه_2(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = خطأ;
            إذا (U != تش_اشاره) {
                U_ASSERT(U->size() > 0);
                صحيح  ix;
                لكل (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    إذا (سېټ_Equals(U, temp2->fPositions)) {
                        ړنګول U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = ریښتیا;
                        اكسر;
                    }
                }

                // Add U as an unmarked state to Dstates
                إذا (!UinDstates)
                {
                    RBBIStateDescriptor *newState = جديد RBBIStateDescriptor(وروستی_ننوت_نښه, f_حالت);
                    إذا (newState == تش_اشاره) {
                    	*f_حالت = U_حافظه_ویش_ERROR;
                    }
                    إذا (U_FAILURE(*f_حالت)) {
                        اذهب ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->جمع_کړه_Element(newState, *f_حالت);
                    إذا (U_FAILURE(*f_حالت)) {
                        أرجع;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->سېټ_Element_At(ux, a);
            }
        }
    }
    أرجع;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    ړنګول initialState;
    ړنګول failState;
}


/**
 * mapLookAheadRules
 *
 */
فراغ RBBITableBuilder::map_Look_Ahead_قواعد() {
    f_Look_Ahead_قاعده_Map =  جديد UVector32(fRB->fScanner->عدد_قواعد() + 1, *f_حالت);
    إذا (f_Look_Ahead_قاعده_Map == تش_اشاره) {
        *f_حالت = U_حافظه_ویش_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    f_Look_Ahead_قاعده_Map->سېټ_اندازه(fRB->fScanner->عدد_قواعد() + 1);

    لكل (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t la_خونه_For_State = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        منطقي saw_Look_Ahead_غوټه = خطأ;
        لكل (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *غوټه = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            إذا (غوټه->fType != RBBINode::غوټه_Type::lookAhead) {
                استمر;
            }
            saw_Look_Ahead_غوټه = ریښتیا;
            int32_t قاعده_عدد = غوټه->fVal;     // Set when rule was originally parsed.
            U_ASSERT(قاعده_عدد < f_Look_Ahead_قاعده_Map->size());
            U_ASSERT(قاعده_عدد > 0);
            int32_t la_خونه = f_Look_Ahead_قاعده_Map->elementAti(قاعده_عدد);
            إذا (la_خونه != 0) {
                إذا (la_خونه_For_State == 0) {
                    la_خونه_For_State = la_خونه;
                } وإلا {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(la_خونه == la_خونه_For_State);
                }
            }
        }
        إذا (!saw_Look_Ahead_غوټه) {
            استمر;
        }

        إذا (la_خونه_For_State == 0) {
            la_خونه_For_State = ++f_LASlots_دننه_Use;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        لكل (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *غوټه = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            إذا (غوټه->fType != RBBINode::غوټه_Type::lookAhead) {
                استمر;
            }
            int32_t قاعده_عدد = غوټه->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = f_Look_Ahead_قاعده_Map->elementAti(قاعده_عدد);
            (فراغ)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == la_خونه_For_State);
            f_Look_Ahead_قاعده_Map->سېټ_Element_At(la_خونه_For_State, قاعده_عدد);
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
فراغ     RBBITableBuilder::flag_Accepting_حالتونه() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    UVector     پای_ټکی_Marker_Nodes(*f_حالت);
    RBBINode    *پای_ټکی_Marker;
    int32_t     i;
    int32_t     n;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    f_ونه->پیدا_کړه_Nodes(&پای_ټکی_Marker_Nodes, RBBINode::پای_ټکی_Mark, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    لكل (i=0; i<پای_ټکی_Marker_Nodes.size(); i++) {
        پای_ټکی_Marker = static_cast<RBBINode*>(پای_ټکی_Marker_Nodes.elementAt(i));
        لكل (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            إذا (sd->fPositions->شاخص_د(پای_ټکی_Marker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                إذا (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = f_Look_Ahead_قاعده_Map->elementAti(پای_ټکی_Marker->fVal);
                    إذا (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                إذا (sd->fAccepting==ACCEPTING_UNCONDITIONAL && پای_ټکی_Marker->fVal != 0) {
                    // Both lookahead and non-lookahead accepting for this state.
                    // Favor the look-ahead, because a look-ahead match needs to
                    // immediately stop the run-time engine. First match, not longest.
                    sd->fAccepting = f_Look_Ahead_قاعده_Map->elementAti(پای_ټکی_Marker->fVal);
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
فراغ     RBBITableBuilder::flag_Look_Ahead_حالتونه() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    UVector     lookAheadNodes(*f_حالت);
    RBBINode    *look_Ahead_غوټه;
    int32_t     i;
    int32_t     n;

    f_ونه->پیدا_کړه_Nodes(&lookAheadNodes, RBBINode::lookAhead, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_غوټه = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_غوټه->fType == RBBINode::غوټه_Type::lookAhead);

        لكل (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->شاخص_د(look_Ahead_غوټه);
            إذا (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_غوټه == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookahead_خونه = f_Look_Ahead_قاعده_Map->elementAti(look_Ahead_غوټه->fVal);
                U_ASSERT(sd->fLookAhead == 0 || sd->fLookAhead == lookahead_خونه);
                // if (sd->fLookAhead != 0 && sd->fLookAhead != lookaheadSlot) {
                //     printf("%s:%d Bingo. sd->fLookAhead:%d   lookaheadSlot:%d\n",
                //            __FILE__, __LINE__, sd->fLookAhead, lookaheadSlot);
                // }
                sd->fLookAhead = lookahead_خونه;
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//    flagTaggedStates
//
//-----------------------------------------------------------------------------
فراغ     RBBITableBuilder::flag_Tagged_حالتونه() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    UVector     نښان_Nodes(*f_حالت);
    RBBINode    *نښان_غوټه;
    int32_t     i;
    int32_t     n;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    f_ونه->پیدا_کړه_Nodes(&نښان_Nodes, RBBINode::نښان, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (i=0; i<نښان_Nodes.size(); i++) {                   // For each tag node t (all of 'em)
        نښان_غوټه = static_cast<RBBINode*>(نښان_Nodes.elementAt(i));

        لكل (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            إذا (sd->fPositions->شاخص_د(نښان_غوټه) >= 0) {  //       if  s include the tag node t
                sorted_جمع_کړه(&sd->f_نښان_Vals, نښان_غوټه->fVal);
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
فراغ  RBBITableBuilder::یوځای_قاعده_حالت_Vals() {
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
    إذا (fRB->f_قاعده_حالت_Vals->size() == 0) {
        fRB->f_قاعده_حالت_Vals->جمع_کړه_Element(1, *f_حالت);  // Num of statuses in group
        fRB->f_قاعده_حالت_Vals->جمع_کړه_Element(static_cast<int32_t>(0), *f_حالت); // and our single status of zero
    }

    //    For each state
    لكل (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_حالتونه_نښان_ارزښتونه = sd->f_نښان_Vals;
        إذا (this_حالتونه_نښان_ارزښتونه == تش_اشاره) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->f_نښانونه_Idx = 0;
            استمر;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->f_نښانونه_Idx = -1;
        int32_t  this_نښان_ډله_پیل = 0;   // indexes into the global rule status vals list
        int32_t  بل_نښان_ډله_پیل = 0;

        // Loop runs once per group of tags in the global list
        طالما (بل_نښان_ډله_پیل < fRB->f_قاعده_حالت_Vals->size()) {
            this_نښان_ډله_پیل = بل_نښان_ډله_پیل;
            بل_نښان_ډله_پیل += fRB->f_قاعده_حالت_Vals->elementAti(this_نښان_ډله_پیل) + 1;
            إذا (this_حالتونه_نښان_ارزښتونه->size() != fRB->f_قاعده_حالت_Vals->elementAti(this_نښان_ډله_پیل)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                استمر;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            لكل (i=0; i<this_حالتونه_نښان_ارزښتونه->size(); i++) {
                إذا (this_حالتونه_نښان_ارزښتونه->elementAti(i) !=
                    fRB->f_قاعده_حالت_Vals->elementAti(this_نښان_ډله_پیل + 1 + i) ) {
                    // Mismatch.
                    اكسر;
                }
            }

            إذا (i == this_حالتونه_نښان_ارزښتونه->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->f_نښانونه_Idx = this_نښان_ډله_پیل;
                اكسر;
            }
        }

        إذا (sd->f_نښانونه_Idx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->f_نښانونه_Idx = fRB->f_قاعده_حالت_Vals->size();
            fRB->f_قاعده_حالت_Vals->جمع_کړه_Element(this_حالتونه_نښان_ارزښتونه->size(), *f_حالت);
            لكل (i=0; i<this_حالتونه_نښان_ارزښتونه->size(); i++) {
                fRB->f_قاعده_حالت_Vals->جمع_کړه_Element(this_حالتونه_نښان_ارزښتونه->elementAti(i), *f_حالت);
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

فراغ RBBITableBuilder::minimize_حالتونه() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

#إذا UPRV_HAS_SANITIZER
    إذا (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *f_حالت = U_REGEX_نمونه_TOO_BIG;
        أرجع;
    }
#د_که_پای
    بنية StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t f_نښانونه_Idx;
        منطقي چلوونکی==(StateType ثابت &other) ثابت {
            أرجع fAccepting == other.fAccepting &&
                   fLookAhead == other.fLookAhead &&
                   f_نښانونه_Idx == other.f_نښانونه_Idx;
        }
    };
    // We wrap `UVector`s in `unique_ptr``s throughout so we can move them,
    // including into `UVector`s (by releasing them from the `unique_ptr`
    // and having the enclosing `UVector` adopt them). 
    // Group the states by types (but we have no maps so this is verbose).
    // If there are no lookaheads and no tags, there are only two types
    // (accepting and non-accepting) in which case this is exactly step 1
    // of Algorithm 3.6.
    بنية Type_منزل_حالتونه : UMemory {
        Type_منزل_حالتونه(ثابت StateType &type, UError_کوډ &حالت)
            : type(type), حالتونه(prv::make_unique<UVector>(حالت, حالت)) {}
        StateType type;
        prv::unique_ptr<UVector> حالتونه;
    };
    UVector initial_برخه_کول(*f_حالت);
    initial_برخه_کول.سېټ_Deleter(
        [](فراغ *p) { ړنګول static_cast<Type_منزل_حالتونه *>(p); });
    لكل (int32_t i = 0; i < fDStates->size(); ++i) {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ثابت StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.f_نښانونه_Idx};
        int32_t j = 0;
        لكل (; j < initial_برخه_کول.size(); ++j) {
            خپلکاری &[type_j, حالتونه] =
                *static_cast<Type_منزل_حالتونه *>(initial_برخه_کول.elementAt(j));
            إذا (type_j == type) {
                حالتونه->جمع_کړه_Element(i, *f_حالت);
                اكسر;
            }
        }
        إذا (j == initial_برخه_کول.size()) {
            خپلکاری new_ثبت = prv::make_unique<Type_منزل_حالتونه>(type, *f_حالت, *f_حالت);
            إذا (U_FAILURE(*f_حالت)) {
                أرجع;
            }
            new_ثبت->حالتونه->جمع_کړه_Element(i, *f_حالت);
            initial_برخه_کول.adoptElement(new_ثبت.release(), *f_حالت);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    خپلکاری برخه_کول = prv::make_unique<UVector>(*f_حالت, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    برخه_کول->سېټ_Deleter(
        [](فراغ *p) { ړنګول static_cast<UVector*>(p); });
    
    لكل (int32_t i = 0; i < initial_برخه_کول.size(); ++i) {
        برخه_کول->adoptElement(
            static_cast<Type_منزل_حالتونه*>(initial_برخه_کول.elementAt(i))->حالتونه.release(),
            *f_حالت);
    }
    // Given the index of a state 𝑠 in `fDStates`, returns a UVector of integers
    // σ(𝑠) such that 𝑠.fDtran[i] (the index of the state reached by the
    // transition from 𝑠 on the character class with index i) is in Π[σ(𝑠)[i]].
    // We then have σ(𝑠) == σ(𝑡) if and only if “for all input symbols 𝑎, states
    // 𝑠 and 𝑡 have transitions on 𝑎 to states in the same group of Π” (which is
    // what defines the refinement of 𝐺 in Fig. 3.45).
    خپلکاری برخه_کول_signature = [&برخه_کول, هذا](int32_t state_شاخص) -> prv::unique_ptr<UVector> {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state_شاخص));
        خپلکاری پایله = prv::make_unique<UVector>(state.fDtran->size(), *f_حالت, *f_حالت);
        إذا (U_FAILURE(*f_حالت)) {
            أرجع تش_اشاره;
        }
        لكل (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t منزل_State = state.fDtran->elementAti(i);
            لكل (int32_t part_شاخص = 0; part_شاخص < برخه_کول->size(); ++part_شاخص) {
                ثابت UVector &part = *static_cast<UVector *>(برخه_کول->elementAt(part_شاخص));
                إذا (part.لري(منزل_State)) {
                    پایله->جمع_کړه_Element(part_شاخص, *f_حالت);
                    اكسر;
                }
            }
        }
        پایله->سېټ_Comparer([](ثابت UElement کیڼ, ثابت UElement ښی) -> UBool {
            أرجع کیڼ.integer == ښی.integer;
        });
        أرجع پایله;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    لكل (;;) {
        // Π_new.
        خپلکاری برخه_کول_New = prv::make_unique<UVector>(*f_حالت, *f_حالت);
        إذا (U_FAILURE(*f_حالت)) {
            أرجع;
        }
        برخه_کول_New->سېټ_Deleter(
            [](فراغ *p) { ړنګول static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        منطقي refined = خطأ;
        لكل (int32_t i = 0; i < برخه_کول->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ثابت UVector &ډله = *static_cast<UVector*>(برخه_کول->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            بنية Signature_منزل_حالتونه : UMemory {
                Signature_منزل_حالتونه(prv::unique_ptr<UVector> signature, UError_کوډ &حالت)
                    : signature(std::move(signature)), حالتونه(prv::make_unique<UVector>(حالت, حالت)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> حالتونه;
            };
            UVector ډله_Refinement(*f_حالت);
            ډله_Refinement.سېټ_Deleter([](فراغ *p) { ړنګول static_cast<Signature_منزل_حالتونه*>(p); });
            لكل (int32_t j = 0; j < ډله.size(); ++j) {
                // Index of a state in the group.
                ثابت int32_t s = ډله.elementAti(j);
                خپلکاری signature = برخه_کول_signature(s);
                int32_t k = 0;
                لكل (; k < ډله_Refinement.size(); ++k) {
                    ثابت خپلکاری &[subgroupSignature, subgroup] =
                        *static_cast<Signature_منزل_حالتونه*>(ډله_Refinement.elementAt(k));
                    إذا (*subgroupSignature == *signature) {
                        subgroup->جمع_کړه_Element(s, *f_حالت);
                        اكسر;
                    }
                }
                إذا (k == ډله_Refinement.size()) {
                    ثابت خپلکاری new_ثبت =
                        جديد Signature_منزل_حالتونه(std::move(signature), *f_حالت);
                    إذا (U_FAILURE(*f_حالت)) {
                        أرجع;
                    }
                    إذا (new_ثبت == تش_اشاره) {
                      *f_حالت = U_حافظه_ویش_ERROR;
                      أرجع;
                    }
                    new_ثبت->حالتونه->جمع_کړه_Element(s, *f_حالت);
                    ډله_Refinement.adoptElement(new_ثبت, *f_حالت);
                }
            }
            refined |= ډله_Refinement.size() > 1;
            لكل (int32_t j = 0; j < ډله_Refinement.size(); ++j) {
                خپلکاری &[_, subgroup] = *static_cast<Signature_منزل_حالتونه*>(
                    ډله_Refinement.elementAt(j));
                برخه_کول_New->adoptElement(subgroup.release(), *f_حالت);
            }
        }
        إذا (refined) {
            برخه_کول = std::move(برخه_کول_New);
        } وإلا {
            اكسر;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    لكل (صحيح i = 0; i < برخه_کول->size(); ++i) {
        UVector *ثابت part_i = static_cast<UVector *>(برخه_کول->elementAt(i));
        إذا (part_i->لري(0)) {
            فراغ *ثابت part_0 = برخه_کول->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ثابت خپلکاری deleter = برخه_کول->سېټ_Deleter(تش_اشاره);
            برخه_کول->سېټ_Element_At(part_i, 0);
            برخه_کول->سېټ_Element_At(part_0, i);
            برخه_کول->سېټ_Deleter(deleter);
        } وإلا إذا (part_i->لري(1)) {
            فراغ *ثابت part_1 = برخه_کول->elementAt(1);
            ثابت خپلکاری deleter = برخه_کول->سېټ_Deleter(تش_اشاره);
            برخه_کول->سېټ_Element_At(part_i, 1);
            برخه_کول->سېټ_Element_At(part_1, i);
            برخه_کول->سېټ_Deleter(deleter);
        }
    }
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    خپلکاری old_State_منزل_Part = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (صحيح i = 0; i < برخه_کول->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(برخه_کول->elementAt(i));
        لكل (صحيح j = 0; j < part.size(); ++j) {
            old_State_منزل_Part[part.elementAti(j)] = i;
        }
    }
    خپلکاری ثابت old_حالتونه = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*f_حالت, *f_حالت).release();
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (صحيح i = 0; i < برخه_کول->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(برخه_کول->elementAt(i));
        RBBIStateDescriptor *ثابت state =
            static_cast<RBBIStateDescriptor*>(old_حالتونه->elementAt(part.elementAti(0)));
        fDStates->جمع_کړه_Element(state, *f_حالت);
        لكل (صحيح j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->سېټ_Element_At(old_State_منزل_Part[state->fDtran->elementAti(j)], j);
        }
        old_حالتونه->سېټ_Element_At(تش_اشاره, part.elementAti(0));
    }
    old_حالتونه->سېټ_Deleter([](فراغ *p) { ړنګول static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::sorted_جمع_کړه(UVector **vector, int32_t val) {
    int32_t i;

    إذا (*vector == تش_اشاره) {
        *vector = جديد UVector(*f_حالت);
    }
    إذا (*vector == تش_اشاره || U_FAILURE(*f_حالت)) {
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
    vec->ورننباسه_Element_At(val, i, *f_حالت);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::سېټ_جمع_کړه_2(UVector *dest, UVector *سرچینه) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!سرچینه->hasDeleter());
    int32_t dest_Original_اندازه = dest->size();
    int32_t سرچینه_اندازه       = سرچینه->size();
    int32_t di           = 0;
    Maybe_ډېری_Array<فراغ *, 16> destArray, سرچینه_Array;  // Handle small cases without malloc
    فراغ **destPtr, **سرچینه_Ptr;
    فراغ **destLim, **سرچینه_Lim;

    إذا (dest_Original_اندازه > destArray.get_ظرفیت()) {
        إذا (destArray.resize(dest_Original_اندازه) == تش_اشاره) {
            أرجع;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + dest_Original_اندازه;  // destArray.getArrayLimit()?

    إذا (سرچینه_اندازه > سرچینه_Array.get_ظرفیت()) {
        إذا (سرچینه_Array.resize(سرچینه_اندازه) == تش_اشاره) {
            أرجع;
        }
    }
    سرچینه_Ptr = سرچینه_Array.getAlias();
    سرچینه_Lim = سرچینه_Ptr + سرچینه_اندازه;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (فراغ) dest->منزل_Array(destPtr);
    (فراغ) سرچینه->منزل_Array(سرچینه_Ptr);

    dest->سېټ_اندازه(سرچینه_اندازه+dest_Original_اندازه, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    طالما (سرچینه_Ptr < سرچینه_Lim && destPtr < destLim) {
        إذا (*destPtr == *سرچینه_Ptr) {
            dest->سېټ_Element_At(*سرچینه_Ptr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        وإلا إذا (uprv_memcmp(destPtr, سرچینه_Ptr, حجم(فراغ *)) < 0) {
            dest->سېټ_Element_At(*destPtr++, di++);
        }
        وإلا { /* *sourcePtr < *destPtr */
            dest->سېټ_Element_At(*سرچینه_Ptr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    طالما (destPtr < destLim) {
        dest->سېټ_Element_At(*destPtr++, di++);
    }
    طالما (سرچینه_Ptr < سرچینه_Lim) {
        dest->سېټ_Element_At(*سرچینه_Ptr++, di++);
    }

    dest->سېټ_اندازه(di, *f_حالت);
}



//-----------------------------------------------------------------------------
//
//  setEqual    Set operation on UVector.
//              Compare for equality.
//              Elements must be sorted.
//
//-----------------------------------------------------------------------------
UBool RBBITableBuilder::سېټ_Equals(UVector *a, UVector *b) {
    أرجع a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#که_تعریف_شوی RBBI_DEBUG
فراغ RBBITableBuilder::printPosSets(RBBINode *n) {
    إذا (n==تش_اشاره) {
        أرجع;
    }
    چاپ_کړه("\n");
    RBBINode::print_غوټه_سرلیک();
    RBBINode::print_غوټه(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    print_سېټ(n->f_لومړی_Pos_سېټ);

    RBBIDebugPrintf("         lastpos:   ");
    print_سېټ(n->f_وروستی_Pos_سېټ);

    RBBIDebugPrintf("         followpos: ");
    print_سېټ(n->fFollowPos);

    printPosSets(n->f_کیڼ_ماشوم);
    printPosSets(n->f_ښی_ماشوم);
}
#د_که_پای

//
//    findDuplCharClassFrom()
//
منطقي RBBITableBuilder::پیدا_کړه_Dupl_Char_Class_From(IntPair *categories) {
    int32_t عدد_حالتونه = fDStates->size();
    int32_t عدد_ستون_شمېر = fRB->f_سېټ_Builder->get_عدد_Char_Categories();

    لكل (; categories->first < عدد_ستون_شمېر-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        صحيح حد_دوهم = categories->first < fRB->f_سېټ_Builder->get_Dict_Categories_پیل() ?
            fRB->f_سېټ_Builder->get_Dict_Categories_پیل() : عدد_ستون_شمېر;
        لكل (categories->دوهم=categories->first+1; categories->دوهم < حد_دوهم; categories->دوهم++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t جدول_بنسټ = 0;
            uint16_t جدول_dupl = 1;
            لكل (int32_t state=0; state<عدد_حالتونه; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                جدول_بنسټ = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                جدول_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->دوهم));
                إذا (جدول_بنسټ != جدول_dupl) {
                    اكسر;
                }
            }
            إذا (جدول_بنسټ == جدول_dupl) {
                أرجع ریښتیا;
            }
        }
    }
    أرجع خطأ;
}


//
//    removeColumn()
//
فراغ RBBITableBuilder::remove_ستنه(int32_t ستنه) {
    int32_t عدد_حالتونه = fDStates->size();
    لكل (int32_t state=0; state<عدد_حالتونه; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(ستنه < sd->fDtran->size());
        sd->fDtran->removeElementAt(ستنه);
    }
}


منطقي RBBITableBuilder::پیدا_کړه_Duplicate_خوندي_State(IntPair *حالتونه) {
    int32_t عدد_حالتونه = f_خوندي_جدول->size();

    لكل (; حالتونه->first<عدد_حالتونه-1; حالتونه->first++) {
        UnicodeString *لومړی_لیکه = static_cast<UnicodeString *>(f_خوندي_جدول->elementAt(حالتونه->first));
        لكل (حالتونه->دوهم=حالتونه->first+1; حالتونه->دوهم<عدد_حالتونه; حالتونه->دوهم++) {
            UnicodeString *dupl_لیکه = static_cast<UnicodeString *>(f_خوندي_جدول->elementAt(حالتونه->دوهم));
            منطقي لیکې_سمون = ریښتیا;
            int32_t عدد_ستون_شمېر = لومړی_لیکه->length();
            لكل (int32_t ستون=0; ستون < عدد_ستون_شمېر; ++ستون) {
                int32_t لومړی_Val = لومړی_لیکه->charAt(ستون);
                int32_t duplVal = dupl_لیکه->charAt(ستون);
                إذا (!((لومړی_Val == duplVal) ||
                        ((لومړی_Val == حالتونه->first || لومړی_Val == حالتونه->دوهم) &&
                        (duplVal  == حالتونه->first || duplVal  == حالتونه->دوهم)))) {
                    لیکې_سمون = خطأ;
                    اكسر;
                }
            }
            إذا (لیکې_سمون) {
                أرجع ریښتیا;
            }
        }
    }
    أرجع خطأ;
}

فراغ RBBITableBuilder::remove_خوندي_State(IntPair dupl_حالتونه) {
    ثابت int32_t keepState = dupl_حالتونه.first;
    ثابت int32_t duplState = dupl_حالتونه.دوهم;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < f_خوندي_جدول->size());

    f_خوندي_جدول->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t عدد_حالتونه = f_خوندي_جدول->size();
    لكل (int32_t state=0; state<عدد_حالتونه; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(f_خوندي_جدول->elementAt(state));
        int32_t عدد_ستون_شمېر = sd->length();
        لكل (int32_t ستون=0; ستون<عدد_ستون_شمېر; ستون++) {
            int32_t existingVal = sd->charAt(ستون);
            int32_t newVal = existingVal;
            إذا (existingVal == duplState) {
                newVal = keepState;
            } وإلا إذا (existingVal > duplState) {
                newVal = existingVal - 1;
            }
            sd->سېټ_Char_At(ستون, static_cast<char16_t>(newVal));
        }
    }
}


/*
 * RemoveDuplicateStates
 */
int32_t RBBITableBuilder::remove_Duplicate_حالتونه() {
    ثابت int32_t old_State_شمېر = fDStates->size();
    minimize_حالتونه();
    أرجع fDStates->size() - old_State_شمېر;
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
    إذا (f_LASlots_دننه_Use == ACCEPTING_UNCONDITIONAL) {
        أرجع;
    }
    ثابت int32_t lookahead_شمېر = f_LASlots_دننه_Use - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ثابت int32_t لومړی_Lookahead = ACCEPTING_UNCONDITIONAL + 1;
    ثابت خپلکاری stateDescriptor = [هذا](ثابت int32_t state) -> RBBIStateDescriptor & {
        أرجع *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ثابت خپلکاری lookaheadReachability =
        prv::make_unique<منطقي[]>(lookahead_شمېر * lookahead_شمېر, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (int32_t l = لومړی_Lookahead; l <= f_LASlots_دننه_Use; ++l) {
        لكل (int32_t k = لومړی_Lookahead; k <= f_LASlots_دننه_Use; ++k) {
            إذا (k != l) {
                لكل (int32_t سرچینه = 1; سرچینه < fDStates->size(); ++سرچینه) {
                    إذا (static_cast<int32_t>(stateDescriptor(سرچینه).fLookAhead) != l) {
                        استمر;
                    }
                    إذا (reachableByTransitions(
                            *fDStates, سرچینه, /*isSink=*/
                            [&](ثابت int32_t state) {
                                أرجع static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](ثابت int32_t state) {
                                أرجع static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *f_حالت)) {
                        lookaheadReachability[(l - لومړی_Lookahead) * lookahead_شمېر + k -
                                              لومړی_Lookahead] = ریښتیا;
                    }
                }
            }
        }
    }

    ثابت خپلکاری colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_شمېر, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    لكل (int32_t chromatic_شمېره = 1;; ++chromatic_شمېره) {
        uprv_memset(colours.get(), 0, حجم(int32_t) * lookahead_شمېر);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        لكل (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            لكل (صحيح سرچینه = 0; سرچینه < lookahead_شمېر; ++سرچینه) {
                لكل (صحيح sink = 0; sink < lookahead_شمېر; ++sink) {
                    إذا (lookaheadReachability[سرچینه * lookahead_شمېر + sink] &&
                        colours[سرچینه] == colours[sink]) {
                        اذهب بل_Colouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            لكل (صحيح i = 0; i < fDStates->size(); ++i) {
                إذا (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        لومړی_Lookahead +
                        colours[stateDescriptor(i).fAccepting - لومړی_Lookahead];
                }
                إذا (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - لومړی_Lookahead];
                }
            }
            f_LASlots_دننه_Use = ACCEPTING_UNCONDITIONAL + chromatic_شمېره;
            أرجع;

          بل_Colouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            لكل (int32_t i = 0; i < lookahead_شمېر - 1 && colours[i] == chromatic_شمېره;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            إذا (colours[lookahead_شمېر - 1] == chromatic_شمېره) {
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
    int32_t    عدد_لیکې;
    int32_t    عدد_ستون_شمېر;
    int32_t    لیکه_اندازه;

    إذا (f_ونه == تش_اشاره) {
        أرجع 0;
    }

    size    = offsetof(RBBIState_جدول, f_جدول_معلومات);    // The header, with no rows to the table.

    عدد_لیکې = fDStates->size();
    عدد_ستون_شمېر = fRB->f_سېټ_Builder->get_عدد_Char_Categories();

    إذا (use8_بټونه_For_جدول()) {
        لیکه_اندازه = offsetof(RBBIState_جدول_Row8, f_بل_State) + حجم(int8_t)*عدد_ستون_شمېر;
    } وإلا {
        لیکه_اندازه = offsetof(RBBIState_جدول_Row16, f_بل_State) + حجم(int16_t)*عدد_ستون_شمېر;
    }
    size   += عدد_لیکې * لیکه_اندازه;
    أرجع size;
}

منطقي RBBITableBuilder::use8_بټونه_For_جدول() ثابت {
    أرجع fDStates->size() <= k_Max_State_For8_بټونه_جدول;
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

    إذا (U_FAILURE(*f_حالت) || f_ونه == تش_اشاره) {
        أرجع;
    }

    int32_t cat_شمېر = fRB->f_سېټ_Builder->get_عدد_Char_Categories();
    إذا (cat_شمېر > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *f_حالت = U_BRK_دننی_ERROR;
        أرجع;
    }

    جدول->f_عدد_حالتونه = fDStates->size();
    جدول->f_Dict_Categories_پیل = fRB->f_سېټ_Builder->get_Dict_Categories_پیل();
    جدول->f_Look_Ahead_Results_اندازه = f_LASlots_دننه_Use == ACCEPTING_UNCONDITIONAL ? 0 : f_LASlots_دننه_Use + 1;
    جدول->fFlags     = 0;
    إذا (use8_بټونه_For_جدول()) {
        جدول->f_لیکه_Len    = offsetof(RBBIState_جدول_Row8, f_بل_State) + حجم(uint8_t) * cat_شمېر;
        جدول->fFlags  |= RBBI_8_بټونه_لیکې;
    } وإلا {
        جدول->f_لیکه_Len    = offsetof(RBBIState_جدول_Row16, f_بل_State) + حجم(int16_t) * cat_شمېر;
    }
    إذا (fRB->fLookAheadHardBreak) {
        جدول->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    لكل (state=0; state<جدول->f_عدد_حالتونه; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIState_جدول_لیکه* لیکه = reinterpret_cast<RBBIState_جدول_لیکه*>(جدول->f_جدول_معلومات + state * جدول->f_لیکه_Len);
        إذا (use8_بټونه_For_جدول()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->f_نښانونه_Idx && sd->f_نښانونه_Idx <= 255);
            RBBIState_جدول_Row8* r8 = reinterpret_cast<RBBIState_جدول_Row8*>(لیکه);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->f_نښانونه_Idx   = sd->f_نښانونه_Idx;
            لكل (ستون=0; ستون<cat_شمېر; ستون++) {
                U_ASSERT (sd->fDtran->elementAti(ستون) <= k_Max_State_For8_بټونه_جدول);
                r8->f_بل_State[ستون] = sd->fDtran->elementAti(ستون);
            }
        } وإلا {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->f_نښانونه_Idx && sd->f_نښانونه_Idx <= 0xffff);
            لیکه->r16.fAccepting = sd->fAccepting;
            لیکه->r16.fLookAhead = sd->fLookAhead;
            لیکه->r16.f_نښانونه_Idx   = sd->f_نښانونه_Idx;
            لكل (ستون=0; ستون<cat_شمېر; ستون++) {
                لیکه->r16.f_بل_State[ستون] = sd->fDtran->elementAti(ستون);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
فراغ RBBITableBuilder::build_خوندي_سرچپه_جدول(UError_کوډ &حالت) {
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
    UnicodeString خوندي_جوړې;

    int32_t عدد_Char_Classes = fRB->f_سېټ_Builder->get_عدد_Char_Categories();
    int32_t عدد_حالتونه = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ثابت خپلکاری lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_دننه_Use + 1, حالت);
    ثابت خپلکاری wanted_Lookaheads_دننه_Pair =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_دننه_Use + 1, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t پیل_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_سېټ = -1;
    لكل (int32_t c1 = 1; c1 < عدد_Char_Classes; ++c1) {
        إذا (fRB->f_سېټ_Builder->get_لومړی_Char(c1) < 0) {
          استمر;
        }
        لكل (int32_t c2 = 1; c2 < عدد_Char_Classes; ++c2) {
            إذا (fRB->f_سېټ_Builder->get_لومړی_Char(c2) < 0) {
                استمر;
            }
            int32_t wanted_پای_ټکی_State = -1;
            int32_t wanted_وروستی_Accepting_Position;
            منطقي must_Check_Consistent_With_پیل_C2 = خطأ;
            ثابت int32_t متن[2] = {c1, c2};
            لكل (int32_t injectedState = پیل_STATE; injectedState < عدد_حالتونه; ++injectedState) {
                لكل (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= f_LASlots_دننه_Use; ++injectedLookahead) {
                    int32_t وروستی_Break = BEFORE_PAIR;
                    int32_t وروستی_Accepting_Position = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_سېټ, f_LASlots_دننه_Use + 1);
                    إذا (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ثابت RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        إذا (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            وروستی_Accepting_Position = 0;
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
                    اذهب dfa_ګام;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    لكل (;;) {
                        s = پیل_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_سېټ, f_LASlots_دننه_Use + 1);
                        i = وروستی_Break;
                        // Loop Find_Next_Break in L2/26-135.
                        لكل (;;) {
                        dfa_ګام:
                            إذا (i == BEFORE_PAIR || i == 2) {
                                اذهب inspectFinalState;
                            }
                            ثابت int32_t نښه_Ahead = متن[i];
                            ++i;
                            ثابت RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            إذا (sDescriptor->fDtran->elementAti(نښه_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(نښه_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } وإلا {
                                إذا (وروستی_Break != BEFORE_PAIR &&
                                    وروستی_Accepting_Position == وروستی_Break) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    اذهب بل_نښه_Pair;
                                }
                                وروستی_Break = وروستی_Accepting_Position;
                                اكسر;
                            }

                            إذا (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                وروستی_Accepting_Position = i;
                            } وإلا إذا (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_سېټ) {
                                وروستی_Break = lookaheadPositions[sDescriptor->fAccepting];
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
                    إذا (wanted_پای_ټکی_State < 0) {
                        wanted_پای_ټکی_State = s;
                        wanted_وروستی_Accepting_Position = وروستی_Accepting_Position;
                        لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_دننه_Use; ++l) {
                            إذا (lookaheadPositions[l] != LOOKAHEAD_NOT_سېټ &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wanted_Lookaheads_دننه_Pair[l] = lookaheadPositions[i];
                            } وإلا {
                                wanted_Lookaheads_دننه_Pair[l] = LOOKAHEAD_NOT_سېټ;
                            }
                        }
                    } وإلا {
                        إذا (wanted_پای_ټکی_State != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            اذهب بل_نښه_Pair;
                        }
                        إذا (wanted_وروستی_Accepting_Position != وروستی_Accepting_Position) {
                            اذهب بل_نښه_Pair;
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
                    لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_دننه_Use; ++l) {
                        إذا (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wanted_Lookaheads_دننه_Pair[l]) {
                            استمر;
                        }
                        إذا (lookaheadPositions[l] == 2 || wanted_Lookaheads_دننه_Pair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            إذا (wanted_پای_ټکی_State != پیل_STATE) {
                                اذهب بل_نښه_Pair;
                            }
                        }
                        إذا (lookaheadPositions[l] == 1 || wanted_Lookaheads_دننه_Pair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            must_Check_Consistent_With_پیل_C2 = ریښتیا;
                        }
                    }
                }
            }
            إذا (must_Check_Consistent_With_پیل_C2) {
                ثابت RBBIStateDescriptor& پیل_Descriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(پیل_STATE));
                ثابت خپلکاری c2State = پیل_Descriptor.fDtran->elementAti(c2);
                إذا (c2State != wanted_پای_ټکی_State) {
                    اذهب بل_نښه_Pair;
                }
                ثابت RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                إذا (wanted_وروستی_Accepting_Position != 2) {
                    اذهب بل_نښه_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_دننه_Use; ++l) {
                    إذا (((wanted_Lookaheads_دننه_Pair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wanted_پای_ټکی_State != پیل_STATE) {
                        اذهب بل_نښه_Pair;
                    }
                }
            }
            خوندي_جوړې.append(static_cast<char16_t>(c1));
            خوندي_جوړې.append(static_cast<char16_t>(c2));
        بل_نښه_Pair:
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

    U_ASSERT(f_خوندي_جدول == تش_اشاره);
    Local_اشاره_ګر<UVector> lp_خوندي_جدول(
        جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, عدد_Char_Classes + 2, حالت), حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    f_خوندي_جدول = lp_خوندي_جدول.orphan();
    لكل (int32_t لیکه=0; لیکه<عدد_Char_Classes + 2; ++لیکه) {
        Local_اشاره_ګر<UnicodeString> lpString(جديد UnicodeString(عدد_Char_Classes, 0, عدد_Char_Classes+4), حالت);
        f_خوندي_جدول->adoptElement(lpString.orphan(), حالت);
    }
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &پیل_State = *static_cast<UnicodeString *>(f_خوندي_جدول->elementAt(1));
    لكل (int32_t charClass=0; charClass < عدد_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        پیل_State.سېټ_Char_At(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    لكل (int32_t لیکه=2; لیکه<عدد_Char_Classes+2; ++لیکه) {
        UnicodeString &لیکه_State = *static_cast<UnicodeString *>(f_خوندي_جدول->elementAt(لیکه));
        لیکه_State = پیل_State;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    لكل (int32_t pairIdx=0; pairIdx<خوندي_جوړې.length(); pairIdx+=2) {
        int32_t c1 = خوندي_جوړې.charAt(pairIdx);
        int32_t c2 = خوندي_جوړې.charAt(pairIdx + 1);

        UnicodeString &لیکه_State = *static_cast<UnicodeString *>(f_خوندي_جدول->elementAt(c2 + 2));
        لیکه_State.سېټ_Char_At(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair حالتونه = {1, 0};
    طالما (پیدا_کړه_Duplicate_خوندي_State(&حالتونه)) {
        // printf("Removing duplicate safe states (%d, %d)\n", states.first, states.second);
        remove_خوندي_State(حالتونه);
    }
}


//-----------------------------------------------------------------------------
//
//   getSafeTableSize()    Calculate the size of the runtime form of this
//                         safe state table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::get_خوندي_جدول_اندازه() ثابت {
    int32_t    size = 0;
    int32_t    عدد_لیکې;
    int32_t    عدد_ستون_شمېر;
    int32_t    لیکه_اندازه;

    إذا (f_خوندي_جدول == تش_اشاره) {
        أرجع 0;
    }

    size    = offsetof(RBBIState_جدول, f_جدول_معلومات);    // The header, with no rows to the table.

    عدد_لیکې = f_خوندي_جدول->size();
    عدد_ستون_شمېر = fRB->f_سېټ_Builder->get_عدد_Char_Categories();

    إذا (use8_بټونه_For_خوندي_جدول()) {
        لیکه_اندازه = offsetof(RBBIState_جدول_Row8, f_بل_State) + حجم(int8_t)*عدد_ستون_شمېر;
    } وإلا {
        لیکه_اندازه = offsetof(RBBIState_جدول_Row16, f_بل_State) + حجم(int16_t)*عدد_ستون_شمېر;
    }
    size   += عدد_لیکې * لیکه_اندازه;
    أرجع size;
}

منطقي RBBITableBuilder::use8_بټونه_For_خوندي_جدول() ثابت {
    أرجع f_خوندي_جدول->size() <= k_Max_State_For8_بټونه_جدول;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::export_خوندي_جدول(فراغ *where) {
    RBBIState_جدول* جدول = static_cast<RBBIState_جدول*>(where);
    uint32_t           state;
    صحيح                ستون;

    إذا (U_FAILURE(*f_حالت) || f_خوندي_جدول == تش_اشاره) {
        أرجع;
    }

    int32_t cat_شمېر = fRB->f_سېټ_Builder->get_عدد_Char_Categories();
    إذا (cat_شمېر > 0x7fff ||
            f_خوندي_جدول->size() > 0x7fff) {
        *f_حالت = U_BRK_دننی_ERROR;
        أرجع;
    }

    جدول->f_عدد_حالتونه = f_خوندي_جدول->size();
    جدول->fFlags     = 0;
    إذا (use8_بټونه_For_خوندي_جدول()) {
        جدول->f_لیکه_Len    = offsetof(RBBIState_جدول_Row8, f_بل_State) + حجم(uint8_t) * cat_شمېر;
        جدول->fFlags  |= RBBI_8_بټونه_لیکې;
    } وإلا {
        جدول->f_لیکه_Len    = offsetof(RBBIState_جدول_Row16, f_بل_State) + حجم(int16_t) * cat_شمېر;
    }

    لكل (state=0; state<جدول->f_عدد_حالتونه; state++) {
        UnicodeString* لیکه_String = static_cast<UnicodeString*>(f_خوندي_جدول->elementAt(state));
        RBBIState_جدول_لیکه* لیکه = reinterpret_cast<RBBIState_جدول_لیکه*>(جدول->f_جدول_معلومات + state * جدول->f_لیکه_Len);
        إذا (use8_بټونه_For_خوندي_جدول()) {
            RBBIState_جدول_Row8* r8 = reinterpret_cast<RBBIState_جدول_Row8*>(لیکه);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->f_نښانونه_Idx    = 0;
            لكل (ستون=0; ستون<cat_شمېر; ستون++) {
                U_ASSERT(لیکه_String->charAt(ستون) <= k_Max_State_For8_بټونه_جدول);
                r8->f_بل_State[ستون] = static_cast<uint8_t>(لیکه_String->charAt(ستون));
            }
        } وإلا {
            لیکه->r16.fAccepting = 0;
            لیکه->r16.fLookAhead = 0;
            لیکه->r16.f_نښانونه_Idx    = 0;
            لكل (ستون=0; ستون<cat_شمېر; ستون++) {
                لیکه->r16.f_بل_State[ستون] = لیکه_String->charAt(ستون);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//   printSet    Debug function.   Print the contents of a UVector
//
//-----------------------------------------------------------------------------
#که_تعریف_شوی RBBI_DEBUG
فراغ RBBITableBuilder::print_سېټ(UVector *s) {
    int32_t  i;
    لكل (i=0; i<s->size(); i++) {
        ثابت RBBINode *v = static_cast<ثابت RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==تش_اشاره? -1 : v->f_Serial_عدد);
    }
    RBBIDebugPrintf("\n");
}
#د_که_پای


//-----------------------------------------------------------------------------
//
//   printStates    Debug Function.  Dump the fully constructed state transition table.
//
//-----------------------------------------------------------------------------
#که_تعریف_شوی RBBI_DEBUG
فراغ RBBITableBuilder::print_حالتونه() {
    صحيح     c;    // input "character"
    صحيح     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    لكل (c=0; c<fRB->f_سېټ_Builder->get_عدد_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    لكل (c=0; c<fRB->f_سېټ_Builder->get_عدد_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    لكل (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->f_نښانونه_Idx);
        لكل (c=0; c<fRB->f_سېټ_Builder->get_عدد_Char_Categories(); c++) {
            RBBIDebugPrintf(" %3d", sd->fDtran->elementAti(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#د_که_پای


//-----------------------------------------------------------------------------
//
//   printSafeTable    Debug Function.  Dump the fully constructed safe table.
//
//-----------------------------------------------------------------------------
#که_تعریف_شوی RBBI_DEBUG
فراغ RBBITableBuilder::print_سرچپه_جدول() {
    صحيح     c;    // input "character"
    صحيح     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    إذا (f_خوندي_جدول == تش_اشاره) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        أرجع;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    لكل (c=0; c<fRB->f_سېټ_Builder->get_عدد_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    لكل (c=0; c<fRB->f_سېټ_Builder->get_عدد_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    لكل (n=0; n<f_خوندي_جدول->size(); n++) {
        UnicodeString *لیکه_String = (UnicodeString *)f_خوندي_جدول->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        لكل (c=0; c<fRB->f_سېټ_Builder->get_عدد_Char_Categories(); c++) {
            RBBIDebugPrintf(" %2d", لیکه_String->charAt(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#د_که_پای



//-----------------------------------------------------------------------------
//
//   printRuleStatusTable    Debug Function.  Dump the common rule status table
//
//-----------------------------------------------------------------------------
#که_تعریف_شوی RBBI_DEBUG
فراغ RBBITableBuilder::print_قاعده_حالت_جدول() {
    int32_t  this_ریکارډ = 0;
    int32_t  بل_ریکارډ = 0;
    صحيح      i;
    UVector  *tbl = fRB->f_قاعده_حالت_Vals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    طالما (بل_ریکارډ < tbl->size()) {
        this_ریکارډ = بل_ریکارډ;
        بل_ریکارډ = this_ریکارډ + tbl->elementAti(this_ریکارډ) + 1;
        RBBIDebugPrintf("%4d   ", this_ریکارډ);
        لكل (i=this_ریکارډ+1; i<بل_ریکارډ; i++) {
            RBBIDebugPrintf("  %5d", tbl->elementAti(i));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#د_که_پای


//-----------------------------------------------------------------------------
//
//   RBBIStateDescriptor     Methods.  This is a very struct-like class
//                           Most access is directly to the fields.
//
//-----------------------------------------------------------------------------

RBBIStateDescriptor::RBBIStateDescriptor(صحيح وروستی_ننوت_نښه, UError_کوډ *f_حالت) {
    fMarked    = خطأ;
    fAccepting = 0;
    fLookAhead = 0;
    f_نښانونه_Idx   = 0;
    f_نښان_Vals   = تش_اشاره;
    fPositions = تش_اشاره;
    fDtran     = تش_اشاره;

    fDtran     = جديد UVector32(وروستی_ننوت_نښه+1, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    إذا (fDtran == تش_اشاره) {
        *f_حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    fDtran->سېټ_اندازه(وروستی_ننوت_نښه+1);    // fDtran needs to be pre-sized.
                                           //   It is indexed by input symbols, and will
                                           //   hold  the next state number for each
                                           //   symbol.
}


RBBIStateDescriptor::~RBBIStateDescriptor() {
    ړنګول       fPositions;
    ړنګول       fDtran;
    ړنګول       f_نښان_Vals;
    fPositions = تش_اشاره;
    fDtran     = تش_اشاره;
    f_نښان_Vals   = تش_اشاره;
}

U_NAMESPACE_پای_ټکی

#د_که_پای /* #if !UCONFIG_NO_BREAK_ITERATION */
