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
منطقي reachableByTransitions(ثابت UVector &حالتاں, ثابت int32_t سروت,
                            ثابت std::function<منطقي(int32_t)> isSink,
                            ثابت std::function<منطقي(int32_t)> excludedState, UError_کوڈ &حالت) {
    UStack boundary(حالت);
    {
        UVector32 &transitions_From_سروت =
            *static_cast<RBBIStateDescriptor *>(حالتاں.elementAt(سروت))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        لكل (int32_t علامت = 0; علامت < transitions_From_سروت.size(); ++علامت) {
            ثابت int32_t state = transitions_From_سروت.elementAti(علامت);
            إذا (state != 0) {
                boundary.push(state, حالت);
            }
        }
    }
    ثابت خودکار ویکھیا = prv::make_unique<منطقي[]>(حالتاں.size(), حالت);
    طالما (U_SUCCESS(حالت) && !boundary.empty()) {
        ثابت int32_t s = boundary.popi();
        إذا (isSink(s)) {
            أرجع سچ;
        }
        إذا (excludedState(s) || ویکھیا[s]) {
            استمر;
        }
        ویکھیا[s] = سچ;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(حالتاں.elementAt(s))->fDtran;
        لكل (int32_t علامت = 0; علامت < transitions.size(); ++علامت) {
            ثابت int32_t t = transitions.elementAti(علامت);
            إذا (t != 0 && !ویکھیا[t]) {
                boundary.push(t, حالت);
            }
        }
    }
    أرجع خطأ;
}

} // namespace

ثابت int32_t k_Max_State_For8_بٹ_جدول = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **جڑ_گرہ, UError_کوڈ &حالت) :
        fRB(rb),
        f_درخت(*جڑ_گرہ),
        f_حالت(&حالت),
        fDStates(خالی_سوچک),
        f_محفوظ_جدول(خالی_سوچک) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = جديد UVector(حالت);
    إذا (U_SUCCESS(حالت) && fDStates == خالی_سوچک ) {
        حالت = U_حافظہ_ونڈ_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    صحيح i;
    لكل (i=0; i<fDStates->size(); i++) {
        مٹاؤ static_cast<RBBIStateDescriptor*>(fDStates->elementAt(i));
    }
    مٹاؤ fDStates;
    مٹاؤ f_محفوظ_جدول;
    مٹاؤ f_Look_Ahead_قاعدہ_Map;
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
    إذا (f_درخت==خالی_سوچک) {
        أرجع;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_درخت = f_درخت->flattenVariables(*f_حالت, 0);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
#جے_تعریف_شدہ RBBI_DEBUG
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_درخت(f_درخت, سچ);
    }
#جے_انت

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = جديد RBBINode(RBBINode::opCat, *f_حالت);
    // Exit if memory allocation failed.
    إذا (cn == خالی_سوچک) {
        *f_حالت = U_حافظہ_ونڈ_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        مٹاؤ cn;
        أرجع;
    }
    cn->f_بایاں_بچہ = f_درخت;
    f_درخت->f_ماپا = cn;
    RBBINode *سرا_Marker_گرہ = cn->f_دایاں_بچہ = جديد RBBINode(RBBINode::سرا_Mark, *f_حالت);
    // Delete and exit if memory allocation failed.
    إذا (cn->f_دایاں_بچہ == خالی_سوچک) {
        *f_حالت = U_حافظہ_ونڈ_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        مٹاؤ cn;
        أرجع;
    }
    cn->f_دایاں_بچہ->f_ماپا = cn;
    f_درخت = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_درخت->flattenSets(*f_حالت, 0);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
#جے_تعریف_شدہ RBBI_DEBUG
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_درخت(f_درخت, سچ);
    }
#جے_انت


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_درخت);
    calc_پہلا_Pos(f_درخت);
    calc_آخری_Pos(f_درخت);
    calcFollowPos(f_درخت);
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_درخت);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    إذا (fRB->f_Chain_قاعدے) {
        calcChainedFollowPos(f_درخت, سرا_Marker_گرہ);
    }

    //
    // Build the DFA state transition tables.
    //
    build_State_جدول();
    map_Look_Ahead_قاعدے();
    flag_Accepting_حالتاں();
    flag_Look_Ahead_حالتاں();
    flag_Tagged_حالتاں();

    //
    // Update the global table of rule status {tag} values
    // The rule builder has a global vector of status values that are common
    //    for all tables.  Merge the ones from this table into the global set.
    //
    ملاؤ_قاعدہ_حالت_Vals();
}



//-----------------------------------------------------------------------------
//
//   calcNullable.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcNullable(RBBINode *n) {
    إذا (n == خالی_سوچک) {
        أرجع;
    }
    إذا (n->fType == RBBINode::سیٹ_Ref ||
        n->fType == RBBINode::سرا_Mark ) {
        // These are non-empty leaf node types.
        n->fNullable = خطأ;
        أرجع;
    }

    إذا (n->fType == RBBINode::lookAhead || n->fType == RBBINode::ٹیگ) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = سچ;
        أرجع;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_بایاں_بچہ);
    calcNullable(n->f_دایاں_بچہ);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_بایاں_بچہ->fNullable || n->f_دایاں_بچہ->fNullable;
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_بایاں_بچہ->fNullable && n->f_دایاں_بچہ->fNullable;
    }
    وإلا إذا (n->fType == RBBINode::op_تارا || n->fType == RBBINode::opQuestion) {
        n->fNullable = سچ;
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
فراغ RBBITableBuilder::calc_پہلا_Pos(RBBINode *n) {
    إذا (n == خالی_سوچک) {
        أرجع;
    }
    إذا (n->fType == RBBINode::پتی_Char  ||
        n->fType == RBBINode::سرا_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::ٹیگ) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_پہلا_Pos_سیٹ->جوڑو_Element(n, *f_حالت);
        أرجع;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calc_پہلا_Pos(n->f_بایاں_بچہ);
    calc_پہلا_Pos(n->f_دایاں_بچہ);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        سیٹ_جوڑو_2(n->f_پہلا_Pos_سیٹ, n->f_بایاں_بچہ->f_پہلا_Pos_سیٹ);
        سیٹ_جوڑو_2(n->f_پہلا_Pos_سیٹ, n->f_دایاں_بچہ->f_پہلا_Pos_سیٹ);
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        سیٹ_جوڑو_2(n->f_پہلا_Pos_سیٹ, n->f_بایاں_بچہ->f_پہلا_Pos_سیٹ);
        إذا (n->f_بایاں_بچہ->fNullable) {
            سیٹ_جوڑو_2(n->f_پہلا_Pos_سیٹ, n->f_دایاں_بچہ->f_پہلا_Pos_سیٹ);
        }
    }
    وإلا إذا (n->fType == RBBINode::op_تارا ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        سیٹ_جوڑو_2(n->f_پہلا_Pos_سیٹ, n->f_بایاں_بچہ->f_پہلا_Pos_سیٹ);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calc_آخری_Pos(RBBINode *n) {
    إذا (n == خالی_سوچک) {
        أرجع;
    }
    إذا (n->fType == RBBINode::پتی_Char  ||
        n->fType == RBBINode::سرا_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::ٹیگ) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_آخری_Pos_سیٹ->جوڑو_Element(n, *f_حالت);
        أرجع;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calc_آخری_Pos(n->f_بایاں_بچہ);
    calc_آخری_Pos(n->f_دایاں_بچہ);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        سیٹ_جوڑو_2(n->f_آخری_Pos_سیٹ, n->f_بایاں_بچہ->f_آخری_Pos_سیٹ);
        سیٹ_جوڑو_2(n->f_آخری_Pos_سیٹ, n->f_دایاں_بچہ->f_آخری_Pos_سیٹ);
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        سیٹ_جوڑو_2(n->f_آخری_Pos_سیٹ, n->f_دایاں_بچہ->f_آخری_Pos_سیٹ);
        إذا (n->f_دایاں_بچہ->fNullable) {
            سیٹ_جوڑو_2(n->f_آخری_Pos_سیٹ, n->f_بایاں_بچہ->f_آخری_Pos_سیٹ);
        }
    }
    وإلا إذا (n->fType == RBBINode::op_تارا     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        سیٹ_جوڑو_2(n->f_آخری_Pos_سیٹ, n->f_بایاں_بچہ->f_آخری_Pos_سیٹ);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcFollowPos(RBBINode *n) {
    إذا (n == خالی_سوچک ||
        n->fType == RBBINode::پتی_Char ||
        n->fType == RBBINode::سرا_Mark) {
        أرجع;
    }

    calcFollowPos(n->f_بایاں_بچہ);
    calcFollowPos(n->f_دایاں_بچہ);

    // Aho rule #1
    إذا (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *آخری_Pos_دا_بایاں_بچہ = n->f_بایاں_بچہ->f_آخری_Pos_سیٹ;

        لكل (ix = 0; ix < static_cast<uint32_t>(آخری_Pos_دا_بایاں_بچہ->size()); ix++) {
            i = static_cast<RBBINode*>(آخری_Pos_دا_بایاں_بچہ->elementAt(ix));
            سیٹ_جوڑو_2(i->fFollowPos, n->f_دایاں_بچہ->f_پہلا_Pos_سیٹ);
        }
    }

    // Aho rule #2
    إذا (n->fType == RBBINode::op_تارا ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        لكل (ix = 0; ix < static_cast<uint32_t>(n->f_آخری_Pos_سیٹ->size()); ix++) {
            i = static_cast<RBBINode*>(n->f_آخری_Pos_سیٹ->elementAt(ix));
            سیٹ_جوڑو_2(i->fFollowPos, n->f_پہلا_Pos_سیٹ);
        }
    }



}

//-----------------------------------------------------------------------------
//
//    addRuleRootNodes    Recursively walk a parse tree, adding all nodes flagged
//                        as roots of a rule to a destination vector.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::جوڑو_قاعدہ_جڑ_Nodes(UVector *dest, RBBINode *گرہ) {
    إذا (گرہ == خالی_سوچک || U_FAILURE(*f_حالت)) {
        أرجع;
    }
    U_ASSERT(!dest->hasDeleter());
    إذا (گرہ->f_قاعدہ_جڑ) {
        dest->جوڑو_Element(گرہ, *f_حالت);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        أرجع;
    }
    جوڑو_قاعدہ_جڑ_Nodes(dest, گرہ->f_بایاں_بچہ);
    جوڑو_قاعدہ_جڑ_Nodes(dest, گرہ->f_دایاں_بچہ);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcChainedFollowPos(RBBINode *درخت, RBBINode *سرا_Mark_گرہ) {

    UVector         پتی_Nodes(*f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    // get a list all leaf nodes
    درخت->لبھو_Nodes(&پتی_Nodes, RBBINode::پتی_Char, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector قاعدہ_جڑ_Nodes(*f_حالت);
    جوڑو_قاعدہ_جڑ_Nodes(&قاعدہ_جڑ_Nodes, درخت);

    UVector میل_شروع_Nodes(*f_حالت);
    لكل (صحيح j=0; j<قاعدہ_جڑ_Nodes.size(); ++j) {
        RBBINode *گرہ = static_cast<RBBINode *>(قاعدہ_جڑ_Nodes.elementAt(j));
        إذا (گرہ->f_Chain_وچ) {
            سیٹ_جوڑو_2(&میل_شروع_Nodes, گرہ->f_پہلا_Pos_سیٹ);
        }
    }
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    int32_t  سرا_گرہ_Ix;
    int32_t  شروع_گرہ_Ix;

    لكل (سرا_گرہ_Ix=0; سرا_گرہ_Ix<پتی_Nodes.size(); سرا_گرہ_Ix++) {
        RBBINode* سرا_گرہ = static_cast<RBBINode*>(پتی_Nodes.elementAt(سرا_گرہ_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        إذا (!سرا_گرہ->fFollowPos->شامل_اے(سرا_Mark_گرہ)) {
            استمر;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *شروع_گرہ;
        لكل (شروع_گرہ_Ix = 0; شروع_گرہ_Ix<میل_شروع_Nodes.size(); شروع_گرہ_Ix++) {
            شروع_گرہ = static_cast<RBBINode*>(میل_شروع_Nodes.elementAt(شروع_گرہ_Ix));
            إذا (شروع_گرہ->fType != RBBINode::پتی_Char) {
                استمر;
            }

            إذا (سرا_گرہ->fVal == شروع_گرہ->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                سیٹ_جوڑو_2(سرا_گرہ->fFollowPos, شروع_گرہ->fFollowPos);
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
    RBBINode  *bof_گرہ = f_درخت->f_بایاں_بچہ->f_بایاں_بچہ;
    U_ASSERT(bof_گرہ->fType == RBBINode::پتی_Char);
    U_ASSERT(bof_گرہ->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *میل_شروع_Nodes = f_درخت->f_بایاں_بچہ->f_دایاں_بچہ->f_پہلا_Pos_سیٹ;

    RBBINode *شروع_گرہ;
    صحيح       شروع_گرہ_Ix;
    لكل (شروع_گرہ_Ix = 0; شروع_گرہ_Ix<میل_شروع_Nodes->size(); شروع_گرہ_Ix++) {
        شروع_گرہ = static_cast<RBBINode*>(میل_شروع_Nodes->elementAt(شروع_گرہ_Ix));
        إذا (شروع_گرہ->fType != RBBINode::پتی_Char) {
            استمر;
        }

        إذا (شروع_گرہ->fVal == bof_گرہ->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            سیٹ_جوڑو_2(bof_گرہ->fFollowPos, شروع_گرہ->fFollowPos);
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
    RBBIStateDescriptor *initialState = خالی_سوچک;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    صحيح      آخری_داخلہ_علامت = fRB->f_سیٹ_Builder->get_عدد_Char_Categories() - 1;
    failState = جديد RBBIStateDescriptor(آخری_داخلہ_علامت, f_حالت);
    إذا (failState == خالی_سوچک) {
        *f_حالت = U_حافظہ_ونڈ_ERROR;
        اذهب ExitBuildSTdeleteall;
    }
    failState->fPositions = جديد UVector(*f_حالت);
    إذا (failState->fPositions == خالی_سوچک) {
        *f_حالت = U_حافظہ_ونڈ_ERROR;
    }
    إذا (failState->fPositions == خالی_سوچک || U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }
    fDStates->جوڑو_Element(failState, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = جديد RBBIStateDescriptor(آخری_داخلہ_علامت, f_حالت);
    إذا (initialState == خالی_سوچک) {
        *f_حالت = U_حافظہ_ونڈ_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }
    initialState->fPositions = جديد UVector(*f_حالت);
    إذا (initialState->fPositions == خالی_سوچک) {
        *f_حالت = U_حافظہ_ونڈ_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }
    سیٹ_جوڑو_2(initialState->fPositions, f_درخت->f_پہلا_Pos_سیٹ);
    fDStates->جوڑو_Element(initialState, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    لكل (;;) {
        RBBIStateDescriptor *T = خالی_سوچک;
        int32_t              tx;
        لكل (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *عارضی;
            عارضی = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            إذا (عارضی->fMarked == خطأ) {
                T = عارضی;
                اكسر;
            }
        }
        إذا (T == خالی_سوچک) {
            اكسر;
        }

        // mark T;
        T->fMarked = سچ;

        // for each input symbol a do begin
        int32_t  a;
        لكل (a = 1; a<=آخری_داخلہ_علامت; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = خالی_سوچک;
            RBBINode   *p;
            int32_t     px;
            لكل (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                إذا ((p->fType == RBBINode::پتی_Char) &&  (p->fVal == a)) {
                    إذا (U == خالی_سوچک) {
                        U = جديد UVector(*f_حالت);
                        إذا (U == خالی_سوچک) {
                        	*f_حالت = U_حافظہ_ونڈ_ERROR;
                        	اذهب ExitBuildSTdeleteall;
                        }
                    }
                    سیٹ_جوڑو_2(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = خطأ;
            إذا (U != خالی_سوچک) {
                U_ASSERT(U->size() > 0);
                صحيح  ix;
                لكل (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    إذا (سیٹ_Equals(U, temp2->fPositions)) {
                        مٹاؤ U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = سچ;
                        اكسر;
                    }
                }

                // Add U as an unmarked state to Dstates
                إذا (!UinDstates)
                {
                    RBBIStateDescriptor *newState = جديد RBBIStateDescriptor(آخری_داخلہ_علامت, f_حالت);
                    إذا (newState == خالی_سوچک) {
                    	*f_حالت = U_حافظہ_ونڈ_ERROR;
                    }
                    إذا (U_FAILURE(*f_حالت)) {
                        اذهب ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->جوڑو_Element(newState, *f_حالت);
                    إذا (U_FAILURE(*f_حالت)) {
                        أرجع;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->سیٹ_Element_At(ux, a);
            }
        }
    }
    أرجع;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    مٹاؤ initialState;
    مٹاؤ failState;
}


/**
 * mapLookAheadRules
 *
 */
فراغ RBBITableBuilder::map_Look_Ahead_قاعدے() {
    f_Look_Ahead_قاعدہ_Map =  جديد UVector32(fRB->fScanner->عدد_قاعدے() + 1, *f_حالت);
    إذا (f_Look_Ahead_قاعدہ_Map == خالی_سوچک) {
        *f_حالت = U_حافظہ_ونڈ_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    f_Look_Ahead_قاعدہ_Map->سیٹ_ناپ(fRB->fScanner->عدد_قاعدے() + 1);

    لكل (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t la_تھاں_For_State = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        منطقي saw_Look_Ahead_گرہ = خطأ;
        لكل (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *گرہ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            إذا (گرہ->fType != RBBINode::گرہ_Type::lookAhead) {
                استمر;
            }
            saw_Look_Ahead_گرہ = سچ;
            int32_t قاعدہ_عدد = گرہ->fVal;     // Set when rule was originally parsed.
            U_ASSERT(قاعدہ_عدد < f_Look_Ahead_قاعدہ_Map->size());
            U_ASSERT(قاعدہ_عدد > 0);
            int32_t la_تھاں = f_Look_Ahead_قاعدہ_Map->elementAti(قاعدہ_عدد);
            إذا (la_تھاں != 0) {
                إذا (la_تھاں_For_State == 0) {
                    la_تھاں_For_State = la_تھاں;
                } وإلا {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(la_تھاں == la_تھاں_For_State);
                }
            }
        }
        إذا (!saw_Look_Ahead_گرہ) {
            استمر;
        }

        إذا (la_تھاں_For_State == 0) {
            la_تھاں_For_State = ++f_LASlots_وچ_Use;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        لكل (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *گرہ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            إذا (گرہ->fType != RBBINode::گرہ_Type::lookAhead) {
                استمر;
            }
            int32_t قاعدہ_عدد = گرہ->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = f_Look_Ahead_قاعدہ_Map->elementAti(قاعدہ_عدد);
            (فراغ)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == la_تھاں_For_State);
            f_Look_Ahead_قاعدہ_Map->سیٹ_Element_At(la_تھاں_For_State, قاعدہ_عدد);
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
فراغ     RBBITableBuilder::flag_Accepting_حالتاں() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    UVector     سرا_Marker_Nodes(*f_حالت);
    RBBINode    *سرا_Marker;
    int32_t     i;
    int32_t     n;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    f_درخت->لبھو_Nodes(&سرا_Marker_Nodes, RBBINode::سرا_Mark, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    لكل (i=0; i<سرا_Marker_Nodes.size(); i++) {
        سرا_Marker = static_cast<RBBINode*>(سرا_Marker_Nodes.elementAt(i));
        لكل (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            إذا (sd->fPositions->اشاریہ_دا(سرا_Marker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                إذا (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = f_Look_Ahead_قاعدہ_Map->elementAti(سرا_Marker->fVal);
                    إذا (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                إذا (sd->fAccepting==ACCEPTING_UNCONDITIONAL && سرا_Marker->fVal != 0) {
                    // Both lookahead and non-lookahead accepting for this state.
                    // Favor the look-ahead, because a look-ahead match needs to
                    // immediately stop the run-time engine. First match, not longest.
                    sd->fAccepting = f_Look_Ahead_قاعدہ_Map->elementAti(سرا_Marker->fVal);
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
فراغ     RBBITableBuilder::flag_Look_Ahead_حالتاں() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    UVector     lookAheadNodes(*f_حالت);
    RBBINode    *look_Ahead_گرہ;
    int32_t     i;
    int32_t     n;

    f_درخت->لبھو_Nodes(&lookAheadNodes, RBBINode::lookAhead, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_گرہ = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_گرہ->fType == RBBINode::گرہ_Type::lookAhead);

        لكل (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->اشاریہ_دا(look_Ahead_گرہ);
            إذا (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_گرہ == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookahead_تھاں = f_Look_Ahead_قاعدہ_Map->elementAti(look_Ahead_گرہ->fVal);
                U_ASSERT(sd->fLookAhead == 0 || sd->fLookAhead == lookahead_تھاں);
                // if (sd->fLookAhead != 0 && sd->fLookAhead != lookaheadSlot) {
                //     printf("%s:%d Bingo. sd->fLookAhead:%d   lookaheadSlot:%d\n",
                //            __FILE__, __LINE__, sd->fLookAhead, lookaheadSlot);
                // }
                sd->fLookAhead = lookahead_تھاں;
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//    flagTaggedStates
//
//-----------------------------------------------------------------------------
فراغ     RBBITableBuilder::flag_Tagged_حالتاں() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    UVector     ٹیگ_Nodes(*f_حالت);
    RBBINode    *ٹیگ_گرہ;
    int32_t     i;
    int32_t     n;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    f_درخت->لبھو_Nodes(&ٹیگ_Nodes, RBBINode::ٹیگ, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (i=0; i<ٹیگ_Nodes.size(); i++) {                   // For each tag node t (all of 'em)
        ٹیگ_گرہ = static_cast<RBBINode*>(ٹیگ_Nodes.elementAt(i));

        لكل (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            إذا (sd->fPositions->اشاریہ_دا(ٹیگ_گرہ) >= 0) {  //       if  s include the tag node t
                sorted_جوڑو(&sd->f_ٹیگ_Vals, ٹیگ_گرہ->fVal);
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
فراغ  RBBITableBuilder::ملاؤ_قاعدہ_حالت_Vals() {
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
    إذا (fRB->f_قاعدہ_حالت_Vals->size() == 0) {
        fRB->f_قاعدہ_حالت_Vals->جوڑو_Element(1, *f_حالت);  // Num of statuses in group
        fRB->f_قاعدہ_حالت_Vals->جوڑو_Element(static_cast<int32_t>(0), *f_حالت); // and our single status of zero
    }

    //    For each state
    لكل (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_حالتاں_ٹیگ_اقدار = sd->f_ٹیگ_Vals;
        إذا (this_حالتاں_ٹیگ_اقدار == خالی_سوچک) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->f_ٹیگ_سوچی_Idx = 0;
            استمر;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->f_ٹیگ_سوچی_Idx = -1;
        int32_t  this_ٹیگ_گروہ_شروع = 0;   // indexes into the global rule status vals list
        int32_t  اگلا_ٹیگ_گروہ_شروع = 0;

        // Loop runs once per group of tags in the global list
        طالما (اگلا_ٹیگ_گروہ_شروع < fRB->f_قاعدہ_حالت_Vals->size()) {
            this_ٹیگ_گروہ_شروع = اگلا_ٹیگ_گروہ_شروع;
            اگلا_ٹیگ_گروہ_شروع += fRB->f_قاعدہ_حالت_Vals->elementAti(this_ٹیگ_گروہ_شروع) + 1;
            إذا (this_حالتاں_ٹیگ_اقدار->size() != fRB->f_قاعدہ_حالت_Vals->elementAti(this_ٹیگ_گروہ_شروع)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                استمر;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            لكل (i=0; i<this_حالتاں_ٹیگ_اقدار->size(); i++) {
                إذا (this_حالتاں_ٹیگ_اقدار->elementAti(i) !=
                    fRB->f_قاعدہ_حالت_Vals->elementAti(this_ٹیگ_گروہ_شروع + 1 + i) ) {
                    // Mismatch.
                    اكسر;
                }
            }

            إذا (i == this_حالتاں_ٹیگ_اقدار->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->f_ٹیگ_سوچی_Idx = this_ٹیگ_گروہ_شروع;
                اكسر;
            }
        }

        إذا (sd->f_ٹیگ_سوچی_Idx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->f_ٹیگ_سوچی_Idx = fRB->f_قاعدہ_حالت_Vals->size();
            fRB->f_قاعدہ_حالت_Vals->جوڑو_Element(this_حالتاں_ٹیگ_اقدار->size(), *f_حالت);
            لكل (i=0; i<this_حالتاں_ٹیگ_اقدار->size(); i++) {
                fRB->f_قاعدہ_حالت_Vals->جوڑو_Element(this_حالتاں_ٹیگ_اقدار->elementAti(i), *f_حالت);
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

فراغ RBBITableBuilder::minimize_حالتاں() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

#إذا UPRV_HAS_SANITIZER
    إذا (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *f_حالت = U_REGEX_نمونہ_TOO_BIG;
        أرجع;
    }
#جے_انت
    بنية StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t f_ٹیگ_سوچی_Idx;
        منطقي عامل==(StateType ثابت &other) ثابت {
            أرجع fAccepting == other.fAccepting &&
                   fLookAhead == other.fLookAhead &&
                   f_ٹیگ_سوچی_Idx == other.f_ٹیگ_سوچی_Idx;
        }
    };
    // We wrap `UVector`s in `unique_ptr``s throughout so we can move them,
    // including into `UVector`s (by releasing them from the `unique_ptr`
    // and having the enclosing `UVector` adopt them). 
    // Group the states by types (but we have no maps so this is verbose).
    // If there are no lookaheads and no tags, there are only two types
    // (accepting and non-accepting) in which case this is exactly step 1
    // of Algorithm 3.6.
    بنية Type_منزل_حالتاں : UMemory {
        Type_منزل_حالتاں(ثابت StateType &type, UError_کوڈ &حالت)
            : type(type), حالتاں(prv::make_unique<UVector>(حالت, حالت)) {}
        StateType type;
        prv::unique_ptr<UVector> حالتاں;
    };
    UVector initial_حصے_کرو(*f_حالت);
    initial_حصے_کرو.سیٹ_Deleter(
        [](فراغ *p) { مٹاؤ static_cast<Type_منزل_حالتاں *>(p); });
    لكل (int32_t i = 0; i < fDStates->size(); ++i) {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ثابت StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.f_ٹیگ_سوچی_Idx};
        int32_t j = 0;
        لكل (; j < initial_حصے_کرو.size(); ++j) {
            خودکار &[type_j, حالتاں] =
                *static_cast<Type_منزل_حالتاں *>(initial_حصے_کرو.elementAt(j));
            إذا (type_j == type) {
                حالتاں->جوڑو_Element(i, *f_حالت);
                اكسر;
            }
        }
        إذا (j == initial_حصے_کرو.size()) {
            خودکار new_اندراج = prv::make_unique<Type_منزل_حالتاں>(type, *f_حالت, *f_حالت);
            إذا (U_FAILURE(*f_حالت)) {
                أرجع;
            }
            new_اندراج->حالتاں->جوڑو_Element(i, *f_حالت);
            initial_حصے_کرو.adoptElement(new_اندراج.release(), *f_حالت);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    خودکار حصے_کرو = prv::make_unique<UVector>(*f_حالت, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    حصے_کرو->سیٹ_Deleter(
        [](فراغ *p) { مٹاؤ static_cast<UVector*>(p); });
    
    لكل (int32_t i = 0; i < initial_حصے_کرو.size(); ++i) {
        حصے_کرو->adoptElement(
            static_cast<Type_منزل_حالتاں*>(initial_حصے_کرو.elementAt(i))->حالتاں.release(),
            *f_حالت);
    }
    // Given the index of a state 𝑠 in `fDStates`, returns a UVector of integers
    // σ(𝑠) such that 𝑠.fDtran[i] (the index of the state reached by the
    // transition from 𝑠 on the character class with index i) is in Π[σ(𝑠)[i]].
    // We then have σ(𝑠) == σ(𝑡) if and only if “for all input symbols 𝑎, states
    // 𝑠 and 𝑡 have transitions on 𝑎 to states in the same group of Π” (which is
    // what defines the refinement of 𝐺 in Fig. 3.45).
    خودکار حصے_کرو_signature = [&حصے_کرو, هذا](int32_t state_اشاریہ) -> prv::unique_ptr<UVector> {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state_اشاریہ));
        خودکار نتیجہ = prv::make_unique<UVector>(state.fDtran->size(), *f_حالت, *f_حالت);
        إذا (U_FAILURE(*f_حالت)) {
            أرجع خالی_سوچک;
        }
        لكل (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t منزل_State = state.fDtran->elementAti(i);
            لكل (int32_t part_اشاریہ = 0; part_اشاریہ < حصے_کرو->size(); ++part_اشاریہ) {
                ثابت UVector &part = *static_cast<UVector *>(حصے_کرو->elementAt(part_اشاریہ));
                إذا (part.شامل_اے(منزل_State)) {
                    نتیجہ->جوڑو_Element(part_اشاریہ, *f_حالت);
                    اكسر;
                }
            }
        }
        نتیجہ->سیٹ_Comparer([](ثابت UElement کھبے, ثابت UElement سجے) -> UBool {
            أرجع کھبے.integer == سجے.integer;
        });
        أرجع نتیجہ;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    لكل (;;) {
        // Π_new.
        خودکار حصے_کرو_New = prv::make_unique<UVector>(*f_حالت, *f_حالت);
        إذا (U_FAILURE(*f_حالت)) {
            أرجع;
        }
        حصے_کرو_New->سیٹ_Deleter(
            [](فراغ *p) { مٹاؤ static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        منطقي refined = خطأ;
        لكل (int32_t i = 0; i < حصے_کرو->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ثابت UVector &گروہ = *static_cast<UVector*>(حصے_کرو->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            بنية Signature_منزل_حالتاں : UMemory {
                Signature_منزل_حالتاں(prv::unique_ptr<UVector> signature, UError_کوڈ &حالت)
                    : signature(std::move(signature)), حالتاں(prv::make_unique<UVector>(حالت, حالت)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> حالتاں;
            };
            UVector گروہ_Refinement(*f_حالت);
            گروہ_Refinement.سیٹ_Deleter([](فراغ *p) { مٹاؤ static_cast<Signature_منزل_حالتاں*>(p); });
            لكل (int32_t j = 0; j < گروہ.size(); ++j) {
                // Index of a state in the group.
                ثابت int32_t s = گروہ.elementAti(j);
                خودکار signature = حصے_کرو_signature(s);
                int32_t k = 0;
                لكل (; k < گروہ_Refinement.size(); ++k) {
                    ثابت خودکار &[subgroupSignature, subgroup] =
                        *static_cast<Signature_منزل_حالتاں*>(گروہ_Refinement.elementAt(k));
                    إذا (*subgroupSignature == *signature) {
                        subgroup->جوڑو_Element(s, *f_حالت);
                        اكسر;
                    }
                }
                إذا (k == گروہ_Refinement.size()) {
                    ثابت خودکار new_اندراج =
                        جديد Signature_منزل_حالتاں(std::move(signature), *f_حالت);
                    إذا (U_FAILURE(*f_حالت)) {
                        أرجع;
                    }
                    إذا (new_اندراج == خالی_سوچک) {
                      *f_حالت = U_حافظہ_ونڈ_ERROR;
                      أرجع;
                    }
                    new_اندراج->حالتاں->جوڑو_Element(s, *f_حالت);
                    گروہ_Refinement.adoptElement(new_اندراج, *f_حالت);
                }
            }
            refined |= گروہ_Refinement.size() > 1;
            لكل (int32_t j = 0; j < گروہ_Refinement.size(); ++j) {
                خودکار &[_, subgroup] = *static_cast<Signature_منزل_حالتاں*>(
                    گروہ_Refinement.elementAt(j));
                حصے_کرو_New->adoptElement(subgroup.release(), *f_حالت);
            }
        }
        إذا (refined) {
            حصے_کرو = std::move(حصے_کرو_New);
        } وإلا {
            اكسر;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    لكل (صحيح i = 0; i < حصے_کرو->size(); ++i) {
        UVector *ثابت part_i = static_cast<UVector *>(حصے_کرو->elementAt(i));
        إذا (part_i->شامل_اے(0)) {
            فراغ *ثابت part_0 = حصے_کرو->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ثابت خودکار deleter = حصے_کرو->سیٹ_Deleter(خالی_سوچک);
            حصے_کرو->سیٹ_Element_At(part_i, 0);
            حصے_کرو->سیٹ_Element_At(part_0, i);
            حصے_کرو->سیٹ_Deleter(deleter);
        } وإلا إذا (part_i->شامل_اے(1)) {
            فراغ *ثابت part_1 = حصے_کرو->elementAt(1);
            ثابت خودکار deleter = حصے_کرو->سیٹ_Deleter(خالی_سوچک);
            حصے_کرو->سیٹ_Element_At(part_i, 1);
            حصے_کرو->سیٹ_Element_At(part_1, i);
            حصے_کرو->سیٹ_Deleter(deleter);
        }
    }
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    خودکار old_State_منزل_Part = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (صحيح i = 0; i < حصے_کرو->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(حصے_کرو->elementAt(i));
        لكل (صحيح j = 0; j < part.size(); ++j) {
            old_State_منزل_Part[part.elementAti(j)] = i;
        }
    }
    خودکار ثابت old_حالتاں = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*f_حالت, *f_حالت).release();
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (صحيح i = 0; i < حصے_کرو->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(حصے_کرو->elementAt(i));
        RBBIStateDescriptor *ثابت state =
            static_cast<RBBIStateDescriptor*>(old_حالتاں->elementAt(part.elementAti(0)));
        fDStates->جوڑو_Element(state, *f_حالت);
        لكل (صحيح j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->سیٹ_Element_At(old_State_منزل_Part[state->fDtran->elementAti(j)], j);
        }
        old_حالتاں->سیٹ_Element_At(خالی_سوچک, part.elementAti(0));
    }
    old_حالتاں->سیٹ_Deleter([](فراغ *p) { مٹاؤ static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::sorted_جوڑو(UVector **vector, int32_t val) {
    int32_t i;

    إذا (*vector == خالی_سوچک) {
        *vector = جديد UVector(*f_حالت);
    }
    إذا (*vector == خالی_سوچک || U_FAILURE(*f_حالت)) {
        أرجع;
    }
    UVector *vec = *vector;
    int32_t  v_ناپ = vec->size();
    لكل (i=0; i<v_ناپ; i++) {
        int32_t valAtI = vec->elementAti(i);
        إذا (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            أرجع;
        }
        إذا (valAtI > val) {
            اكسر;
        }
    }
    vec->داخل_کرو_Element_At(val, i, *f_حالت);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::سیٹ_جوڑو_2(UVector *dest, UVector *سروت) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!سروت->hasDeleter());
    int32_t dest_Original_ناپ = dest->size();
    int32_t سروت_ناپ       = سروت->size();
    int32_t di           = 0;
    Maybe_ڈھیر_Array<فراغ *, 16> destArray, سروت_Array;  // Handle small cases without malloc
    فراغ **destPtr, **سروت_Ptr;
    فراغ **destLim, **سروت_Lim;

    إذا (dest_Original_ناپ > destArray.get_سمرتھا()) {
        إذا (destArray.resize(dest_Original_ناپ) == خالی_سوچک) {
            أرجع;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + dest_Original_ناپ;  // destArray.getArrayLimit()?

    إذا (سروت_ناپ > سروت_Array.get_سمرتھا()) {
        إذا (سروت_Array.resize(سروت_ناپ) == خالی_سوچک) {
            أرجع;
        }
    }
    سروت_Ptr = سروت_Array.getAlias();
    سروت_Lim = سروت_Ptr + سروت_ناپ;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (فراغ) dest->منزل_Array(destPtr);
    (فراغ) سروت->منزل_Array(سروت_Ptr);

    dest->سیٹ_ناپ(سروت_ناپ+dest_Original_ناپ, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    طالما (سروت_Ptr < سروت_Lim && destPtr < destLim) {
        إذا (*destPtr == *سروت_Ptr) {
            dest->سیٹ_Element_At(*سروت_Ptr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        وإلا إذا (uprv_memcmp(destPtr, سروت_Ptr, حجم(فراغ *)) < 0) {
            dest->سیٹ_Element_At(*destPtr++, di++);
        }
        وإلا { /* *sourcePtr < *destPtr */
            dest->سیٹ_Element_At(*سروت_Ptr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    طالما (destPtr < destLim) {
        dest->سیٹ_Element_At(*destPtr++, di++);
    }
    طالما (سروت_Ptr < سروت_Lim) {
        dest->سیٹ_Element_At(*سروت_Ptr++, di++);
    }

    dest->سیٹ_ناپ(di, *f_حالت);
}



//-----------------------------------------------------------------------------
//
//  setEqual    Set operation on UVector.
//              Compare for equality.
//              Elements must be sorted.
//
//-----------------------------------------------------------------------------
UBool RBBITableBuilder::سیٹ_Equals(UVector *a, UVector *b) {
    أرجع a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#جے_تعریف_شدہ RBBI_DEBUG
فراغ RBBITableBuilder::printPosSets(RBBINode *n) {
    إذا (n==خالی_سوچک) {
        أرجع;
    }
    لکھو("\n");
    RBBINode::print_گرہ_سرخی();
    RBBINode::print_گرہ(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    print_سیٹ(n->f_پہلا_Pos_سیٹ);

    RBBIDebugPrintf("         lastpos:   ");
    print_سیٹ(n->f_آخری_Pos_سیٹ);

    RBBIDebugPrintf("         followpos: ");
    print_سیٹ(n->fFollowPos);

    printPosSets(n->f_بایاں_بچہ);
    printPosSets(n->f_دایاں_بچہ);
}
#جے_انت

//
//    findDuplCharClassFrom()
//
منطقي RBBITableBuilder::لبھو_Dupl_Char_Class_From(IntPair *categories) {
    int32_t عدد_حالتاں = fDStates->size();
    int32_t عدد_خانے_گنتی = fRB->f_سیٹ_Builder->get_عدد_Char_Categories();

    لكل (; categories->first < عدد_خانے_گنتی-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        صحيح حد_بندی_دوجا = categories->first < fRB->f_سیٹ_Builder->get_Dict_Categories_شروع() ?
            fRB->f_سیٹ_Builder->get_Dict_Categories_شروع() : عدد_خانے_گنتی;
        لكل (categories->دوجا=categories->first+1; categories->دوجا < حد_بندی_دوجا; categories->دوجا++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t جدول_ادھار = 0;
            uint16_t جدول_dupl = 1;
            لكل (int32_t state=0; state<عدد_حالتاں; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                جدول_ادھار = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                جدول_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->دوجا));
                إذا (جدول_ادھار != جدول_dupl) {
                    اكسر;
                }
            }
            إذا (جدول_ادھار == جدول_dupl) {
                أرجع سچ;
            }
        }
    }
    أرجع خطأ;
}


//
//    removeColumn()
//
فراغ RBBITableBuilder::remove_کالم(int32_t کالم) {
    int32_t عدد_حالتاں = fDStates->size();
    لكل (int32_t state=0; state<عدد_حالتاں; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(کالم < sd->fDtran->size());
        sd->fDtran->removeElementAt(کالم);
    }
}


منطقي RBBITableBuilder::لبھو_Duplicate_محفوظ_State(IntPair *حالتاں) {
    int32_t عدد_حالتاں = f_محفوظ_جدول->size();

    لكل (; حالتاں->first<عدد_حالتاں-1; حالتاں->first++) {
        UnicodeString *پہلا_صف = static_cast<UnicodeString *>(f_محفوظ_جدول->elementAt(حالتاں->first));
        لكل (حالتاں->دوجا=حالتاں->first+1; حالتاں->دوجا<عدد_حالتاں; حالتاں->دوجا++) {
            UnicodeString *dupl_صف = static_cast<UnicodeString *>(f_محفوظ_جدول->elementAt(حالتاں->دوجا));
            منطقي صفاں_میل = سچ;
            int32_t عدد_خانے_گنتی = پہلا_صف->length();
            لكل (int32_t خانہ=0; خانہ < عدد_خانے_گنتی; ++خانہ) {
                int32_t پہلا_Val = پہلا_صف->charAt(خانہ);
                int32_t duplVal = dupl_صف->charAt(خانہ);
                إذا (!((پہلا_Val == duplVal) ||
                        ((پہلا_Val == حالتاں->first || پہلا_Val == حالتاں->دوجا) &&
                        (duplVal  == حالتاں->first || duplVal  == حالتاں->دوجا)))) {
                    صفاں_میل = خطأ;
                    اكسر;
                }
            }
            إذا (صفاں_میل) {
                أرجع سچ;
            }
        }
    }
    أرجع خطأ;
}

فراغ RBBITableBuilder::remove_محفوظ_State(IntPair dupl_حالتاں) {
    ثابت int32_t keepState = dupl_حالتاں.first;
    ثابت int32_t duplState = dupl_حالتاں.دوجا;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < f_محفوظ_جدول->size());

    f_محفوظ_جدول->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t عدد_حالتاں = f_محفوظ_جدول->size();
    لكل (int32_t state=0; state<عدد_حالتاں; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(f_محفوظ_جدول->elementAt(state));
        int32_t عدد_خانے_گنتی = sd->length();
        لكل (int32_t خانہ=0; خانہ<عدد_خانے_گنتی; خانہ++) {
            int32_t existingVal = sd->charAt(خانہ);
            int32_t newVal = existingVal;
            إذا (existingVal == duplState) {
                newVal = keepState;
            } وإلا إذا (existingVal > duplState) {
                newVal = existingVal - 1;
            }
            sd->سیٹ_Char_At(خانہ, static_cast<char16_t>(newVal));
        }
    }
}


/*
 * RemoveDuplicateStates
 */
int32_t RBBITableBuilder::remove_Duplicate_حالتاں() {
    ثابت int32_t old_State_گنتی = fDStates->size();
    minimize_حالتاں();
    أرجع fDStates->size() - old_State_گنتی;
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
    إذا (f_LASlots_وچ_Use == ACCEPTING_UNCONDITIONAL) {
        أرجع;
    }
    ثابت int32_t lookahead_گنتی = f_LASlots_وچ_Use - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ثابت int32_t پہلا_Lookahead = ACCEPTING_UNCONDITIONAL + 1;
    ثابت خودکار stateDescriptor = [هذا](ثابت int32_t state) -> RBBIStateDescriptor & {
        أرجع *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ثابت خودکار lookaheadReachability =
        prv::make_unique<منطقي[]>(lookahead_گنتی * lookahead_گنتی, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (int32_t l = پہلا_Lookahead; l <= f_LASlots_وچ_Use; ++l) {
        لكل (int32_t k = پہلا_Lookahead; k <= f_LASlots_وچ_Use; ++k) {
            إذا (k != l) {
                لكل (int32_t سروت = 1; سروت < fDStates->size(); ++سروت) {
                    إذا (static_cast<int32_t>(stateDescriptor(سروت).fLookAhead) != l) {
                        استمر;
                    }
                    إذا (reachableByTransitions(
                            *fDStates, سروت, /*isSink=*/
                            [&](ثابت int32_t state) {
                                أرجع static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](ثابت int32_t state) {
                                أرجع static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *f_حالت)) {
                        lookaheadReachability[(l - پہلا_Lookahead) * lookahead_گنتی + k -
                                              پہلا_Lookahead] = سچ;
                    }
                }
            }
        }
    }

    ثابت خودکار colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_گنتی, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    لكل (int32_t chromatic_نمبر = 1;; ++chromatic_نمبر) {
        uprv_memset(colours.get(), 0, حجم(int32_t) * lookahead_گنتی);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        لكل (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            لكل (صحيح سروت = 0; سروت < lookahead_گنتی; ++سروت) {
                لكل (صحيح sink = 0; sink < lookahead_گنتی; ++sink) {
                    إذا (lookaheadReachability[سروت * lookahead_گنتی + sink] &&
                        colours[سروت] == colours[sink]) {
                        اذهب اگلا_Colouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            لكل (صحيح i = 0; i < fDStates->size(); ++i) {
                إذا (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        پہلا_Lookahead +
                        colours[stateDescriptor(i).fAccepting - پہلا_Lookahead];
                }
                إذا (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - پہلا_Lookahead];
                }
            }
            f_LASlots_وچ_Use = ACCEPTING_UNCONDITIONAL + chromatic_نمبر;
            أرجع;

          اگلا_Colouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            لكل (int32_t i = 0; i < lookahead_گنتی - 1 && colours[i] == chromatic_نمبر;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            إذا (colours[lookahead_گنتی - 1] == chromatic_نمبر) {
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
int32_t  RBBITableBuilder::get_جدول_ناپ() ثابت {
    int32_t    size = 0;
    int32_t    عدد_صفاں;
    int32_t    عدد_خانے_گنتی;
    int32_t    صف_ناپ;

    إذا (f_درخت == خالی_سوچک) {
        أرجع 0;
    }

    size    = offsetof(RBBIState_جدول, f_جدول_انکڑے);    // The header, with no rows to the table.

    عدد_صفاں = fDStates->size();
    عدد_خانے_گنتی = fRB->f_سیٹ_Builder->get_عدد_Char_Categories();

    إذا (use8_بٹ_For_جدول()) {
        صف_ناپ = offsetof(RBBIState_جدول_Row8, f_اگلا_State) + حجم(int8_t)*عدد_خانے_گنتی;
    } وإلا {
        صف_ناپ = offsetof(RBBIState_جدول_Row16, f_اگلا_State) + حجم(int16_t)*عدد_خانے_گنتی;
    }
    size   += عدد_صفاں * صف_ناپ;
    أرجع size;
}

منطقي RBBITableBuilder::use8_بٹ_For_جدول() ثابت {
    أرجع fDStates->size() <= k_Max_State_For8_بٹ_جدول;
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
    صحيح                خانہ;

    إذا (U_FAILURE(*f_حالت) || f_درخت == خالی_سوچک) {
        أرجع;
    }

    int32_t cat_گنتی = fRB->f_سیٹ_Builder->get_عدد_Char_Categories();
    إذا (cat_گنتی > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *f_حالت = U_BRK_اندرونی_ERROR;
        أرجع;
    }

    جدول->f_عدد_حالتاں = fDStates->size();
    جدول->f_Dict_Categories_شروع = fRB->f_سیٹ_Builder->get_Dict_Categories_شروع();
    جدول->f_Look_Ahead_Results_ناپ = f_LASlots_وچ_Use == ACCEPTING_UNCONDITIONAL ? 0 : f_LASlots_وچ_Use + 1;
    جدول->fFlags     = 0;
    إذا (use8_بٹ_For_جدول()) {
        جدول->f_صف_Len    = offsetof(RBBIState_جدول_Row8, f_اگلا_State) + حجم(uint8_t) * cat_گنتی;
        جدول->fFlags  |= RBBI_8_بٹ_صفاں;
    } وإلا {
        جدول->f_صف_Len    = offsetof(RBBIState_جدول_Row16, f_اگلا_State) + حجم(int16_t) * cat_گنتی;
    }
    إذا (fRB->fLookAheadHardBreak) {
        جدول->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    لكل (state=0; state<جدول->f_عدد_حالتاں; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIState_جدول_صف* صف = reinterpret_cast<RBBIState_جدول_صف*>(جدول->f_جدول_انکڑے + state * جدول->f_صف_Len);
        إذا (use8_بٹ_For_جدول()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->f_ٹیگ_سوچی_Idx && sd->f_ٹیگ_سوچی_Idx <= 255);
            RBBIState_جدول_Row8* r8 = reinterpret_cast<RBBIState_جدول_Row8*>(صف);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->f_ٹیگ_سوچی_Idx   = sd->f_ٹیگ_سوچی_Idx;
            لكل (خانہ=0; خانہ<cat_گنتی; خانہ++) {
                U_ASSERT (sd->fDtran->elementAti(خانہ) <= k_Max_State_For8_بٹ_جدول);
                r8->f_اگلا_State[خانہ] = sd->fDtran->elementAti(خانہ);
            }
        } وإلا {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->f_ٹیگ_سوچی_Idx && sd->f_ٹیگ_سوچی_Idx <= 0xffff);
            صف->r16.fAccepting = sd->fAccepting;
            صف->r16.fLookAhead = sd->fLookAhead;
            صف->r16.f_ٹیگ_سوچی_Idx   = sd->f_ٹیگ_سوچی_Idx;
            لكل (خانہ=0; خانہ<cat_گنتی; خانہ++) {
                صف->r16.f_اگلا_State[خانہ] = sd->fDtran->elementAti(خانہ);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
فراغ RBBITableBuilder::build_محفوظ_الٹ_جدول(UError_کوڈ &حالت) {
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
    UnicodeString محفوظ_جوڑے;

    int32_t عدد_Char_Classes = fRB->f_سیٹ_Builder->get_عدد_Char_Categories();
    int32_t عدد_حالتاں = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ثابت خودکار lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_وچ_Use + 1, حالت);
    ثابت خودکار wanted_Lookaheads_وچ_Pair =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_وچ_Use + 1, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t شروع_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_سیٹ = -1;
    لكل (int32_t c1 = 1; c1 < عدد_Char_Classes; ++c1) {
        إذا (fRB->f_سیٹ_Builder->get_پہلا_Char(c1) < 0) {
          استمر;
        }
        لكل (int32_t c2 = 1; c2 < عدد_Char_Classes; ++c2) {
            إذا (fRB->f_سیٹ_Builder->get_پہلا_Char(c2) < 0) {
                استمر;
            }
            int32_t wanted_سرا_State = -1;
            int32_t wanted_آخری_Accepting_Position;
            منطقي must_Check_Consistent_With_شروع_C2 = خطأ;
            ثابت int32_t متن[2] = {c1, c2};
            لكل (int32_t injectedState = شروع_STATE; injectedState < عدد_حالتاں; ++injectedState) {
                لكل (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= f_LASlots_وچ_Use; ++injectedLookahead) {
                    int32_t آخری_Break = BEFORE_PAIR;
                    int32_t آخری_Accepting_Position = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_سیٹ, f_LASlots_وچ_Use + 1);
                    إذا (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ثابت RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        إذا (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            آخری_Accepting_Position = 0;
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
                    اذهب dfa_قدم;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    لكل (;;) {
                        s = شروع_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_سیٹ, f_LASlots_وچ_Use + 1);
                        i = آخری_Break;
                        // Loop Find_Next_Break in L2/26-135.
                        لكل (;;) {
                        dfa_قدم:
                            إذا (i == BEFORE_PAIR || i == 2) {
                                اذهب inspectFinalState;
                            }
                            ثابت int32_t علامت_Ahead = متن[i];
                            ++i;
                            ثابت RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            إذا (sDescriptor->fDtran->elementAti(علامت_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(علامت_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } وإلا {
                                إذا (آخری_Break != BEFORE_PAIR &&
                                    آخری_Accepting_Position == آخری_Break) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    اذهب اگلا_علامت_Pair;
                                }
                                آخری_Break = آخری_Accepting_Position;
                                اكسر;
                            }

                            إذا (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                آخری_Accepting_Position = i;
                            } وإلا إذا (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_سیٹ) {
                                آخری_Break = lookaheadPositions[sDescriptor->fAccepting];
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
                    إذا (wanted_سرا_State < 0) {
                        wanted_سرا_State = s;
                        wanted_آخری_Accepting_Position = آخری_Accepting_Position;
                        لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_وچ_Use; ++l) {
                            إذا (lookaheadPositions[l] != LOOKAHEAD_NOT_سیٹ &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wanted_Lookaheads_وچ_Pair[l] = lookaheadPositions[i];
                            } وإلا {
                                wanted_Lookaheads_وچ_Pair[l] = LOOKAHEAD_NOT_سیٹ;
                            }
                        }
                    } وإلا {
                        إذا (wanted_سرا_State != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            اذهب اگلا_علامت_Pair;
                        }
                        إذا (wanted_آخری_Accepting_Position != آخری_Accepting_Position) {
                            اذهب اگلا_علامت_Pair;
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
                    لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_وچ_Use; ++l) {
                        إذا (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wanted_Lookaheads_وچ_Pair[l]) {
                            استمر;
                        }
                        إذا (lookaheadPositions[l] == 2 || wanted_Lookaheads_وچ_Pair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            إذا (wanted_سرا_State != شروع_STATE) {
                                اذهب اگلا_علامت_Pair;
                            }
                        }
                        إذا (lookaheadPositions[l] == 1 || wanted_Lookaheads_وچ_Pair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            must_Check_Consistent_With_شروع_C2 = سچ;
                        }
                    }
                }
            }
            إذا (must_Check_Consistent_With_شروع_C2) {
                ثابت RBBIStateDescriptor& شروع_Descriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(شروع_STATE));
                ثابت خودکار c2State = شروع_Descriptor.fDtran->elementAti(c2);
                إذا (c2State != wanted_سرا_State) {
                    اذهب اگلا_علامت_Pair;
                }
                ثابت RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                إذا (wanted_آخری_Accepting_Position != 2) {
                    اذهب اگلا_علامت_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_وچ_Use; ++l) {
                    إذا (((wanted_Lookaheads_وچ_Pair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wanted_سرا_State != شروع_STATE) {
                        اذهب اگلا_علامت_Pair;
                    }
                }
            }
            محفوظ_جوڑے.append(static_cast<char16_t>(c1));
            محفوظ_جوڑے.append(static_cast<char16_t>(c2));
        اگلا_علامت_Pair:
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

    U_ASSERT(f_محفوظ_جدول == خالی_سوچک);
    Local_اشارہ_گر<UVector> lp_محفوظ_جدول(
        جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, عدد_Char_Classes + 2, حالت), حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    f_محفوظ_جدول = lp_محفوظ_جدول.orphan();
    لكل (int32_t صف=0; صف<عدد_Char_Classes + 2; ++صف) {
        Local_اشارہ_گر<UnicodeString> lpString(جديد UnicodeString(عدد_Char_Classes, 0, عدد_Char_Classes+4), حالت);
        f_محفوظ_جدول->adoptElement(lpString.orphan(), حالت);
    }
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &شروع_State = *static_cast<UnicodeString *>(f_محفوظ_جدول->elementAt(1));
    لكل (int32_t charClass=0; charClass < عدد_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        شروع_State.سیٹ_Char_At(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    لكل (int32_t صف=2; صف<عدد_Char_Classes+2; ++صف) {
        UnicodeString &صف_State = *static_cast<UnicodeString *>(f_محفوظ_جدول->elementAt(صف));
        صف_State = شروع_State;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    لكل (int32_t pairIdx=0; pairIdx<محفوظ_جوڑے.length(); pairIdx+=2) {
        int32_t c1 = محفوظ_جوڑے.charAt(pairIdx);
        int32_t c2 = محفوظ_جوڑے.charAt(pairIdx + 1);

        UnicodeString &صف_State = *static_cast<UnicodeString *>(f_محفوظ_جدول->elementAt(c2 + 2));
        صف_State.سیٹ_Char_At(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair حالتاں = {1, 0};
    طالما (لبھو_Duplicate_محفوظ_State(&حالتاں)) {
        // printf("Removing duplicate safe states (%d, %d)\n", states.first, states.second);
        remove_محفوظ_State(حالتاں);
    }
}


//-----------------------------------------------------------------------------
//
//   getSafeTableSize()    Calculate the size of the runtime form of this
//                         safe state table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::get_محفوظ_جدول_ناپ() ثابت {
    int32_t    size = 0;
    int32_t    عدد_صفاں;
    int32_t    عدد_خانے_گنتی;
    int32_t    صف_ناپ;

    إذا (f_محفوظ_جدول == خالی_سوچک) {
        أرجع 0;
    }

    size    = offsetof(RBBIState_جدول, f_جدول_انکڑے);    // The header, with no rows to the table.

    عدد_صفاں = f_محفوظ_جدول->size();
    عدد_خانے_گنتی = fRB->f_سیٹ_Builder->get_عدد_Char_Categories();

    إذا (use8_بٹ_For_محفوظ_جدول()) {
        صف_ناپ = offsetof(RBBIState_جدول_Row8, f_اگلا_State) + حجم(int8_t)*عدد_خانے_گنتی;
    } وإلا {
        صف_ناپ = offsetof(RBBIState_جدول_Row16, f_اگلا_State) + حجم(int16_t)*عدد_خانے_گنتی;
    }
    size   += عدد_صفاں * صف_ناپ;
    أرجع size;
}

منطقي RBBITableBuilder::use8_بٹ_For_محفوظ_جدول() ثابت {
    أرجع f_محفوظ_جدول->size() <= k_Max_State_For8_بٹ_جدول;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::export_محفوظ_جدول(فراغ *where) {
    RBBIState_جدول* جدول = static_cast<RBBIState_جدول*>(where);
    uint32_t           state;
    صحيح                خانہ;

    إذا (U_FAILURE(*f_حالت) || f_محفوظ_جدول == خالی_سوچک) {
        أرجع;
    }

    int32_t cat_گنتی = fRB->f_سیٹ_Builder->get_عدد_Char_Categories();
    إذا (cat_گنتی > 0x7fff ||
            f_محفوظ_جدول->size() > 0x7fff) {
        *f_حالت = U_BRK_اندرونی_ERROR;
        أرجع;
    }

    جدول->f_عدد_حالتاں = f_محفوظ_جدول->size();
    جدول->fFlags     = 0;
    إذا (use8_بٹ_For_محفوظ_جدول()) {
        جدول->f_صف_Len    = offsetof(RBBIState_جدول_Row8, f_اگلا_State) + حجم(uint8_t) * cat_گنتی;
        جدول->fFlags  |= RBBI_8_بٹ_صفاں;
    } وإلا {
        جدول->f_صف_Len    = offsetof(RBBIState_جدول_Row16, f_اگلا_State) + حجم(int16_t) * cat_گنتی;
    }

    لكل (state=0; state<جدول->f_عدد_حالتاں; state++) {
        UnicodeString* صف_String = static_cast<UnicodeString*>(f_محفوظ_جدول->elementAt(state));
        RBBIState_جدول_صف* صف = reinterpret_cast<RBBIState_جدول_صف*>(جدول->f_جدول_انکڑے + state * جدول->f_صف_Len);
        إذا (use8_بٹ_For_محفوظ_جدول()) {
            RBBIState_جدول_Row8* r8 = reinterpret_cast<RBBIState_جدول_Row8*>(صف);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->f_ٹیگ_سوچی_Idx    = 0;
            لكل (خانہ=0; خانہ<cat_گنتی; خانہ++) {
                U_ASSERT(صف_String->charAt(خانہ) <= k_Max_State_For8_بٹ_جدول);
                r8->f_اگلا_State[خانہ] = static_cast<uint8_t>(صف_String->charAt(خانہ));
            }
        } وإلا {
            صف->r16.fAccepting = 0;
            صف->r16.fLookAhead = 0;
            صف->r16.f_ٹیگ_سوچی_Idx    = 0;
            لكل (خانہ=0; خانہ<cat_گنتی; خانہ++) {
                صف->r16.f_اگلا_State[خانہ] = صف_String->charAt(خانہ);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//   printSet    Debug function.   Print the contents of a UVector
//
//-----------------------------------------------------------------------------
#جے_تعریف_شدہ RBBI_DEBUG
فراغ RBBITableBuilder::print_سیٹ(UVector *s) {
    int32_t  i;
    لكل (i=0; i<s->size(); i++) {
        ثابت RBBINode *v = static_cast<ثابت RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==خالی_سوچک? -1 : v->f_Serial_عدد);
    }
    RBBIDebugPrintf("\n");
}
#جے_انت


//-----------------------------------------------------------------------------
//
//   printStates    Debug Function.  Dump the fully constructed state transition table.
//
//-----------------------------------------------------------------------------
#جے_تعریف_شدہ RBBI_DEBUG
فراغ RBBITableBuilder::print_حالتاں() {
    صحيح     c;    // input "character"
    صحيح     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    لكل (c=0; c<fRB->f_سیٹ_Builder->get_عدد_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    لكل (c=0; c<fRB->f_سیٹ_Builder->get_عدد_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    لكل (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->f_ٹیگ_سوچی_Idx);
        لكل (c=0; c<fRB->f_سیٹ_Builder->get_عدد_Char_Categories(); c++) {
            RBBIDebugPrintf(" %3d", sd->fDtran->elementAti(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#جے_انت


//-----------------------------------------------------------------------------
//
//   printSafeTable    Debug Function.  Dump the fully constructed safe table.
//
//-----------------------------------------------------------------------------
#جے_تعریف_شدہ RBBI_DEBUG
فراغ RBBITableBuilder::print_الٹ_جدول() {
    صحيح     c;    // input "character"
    صحيح     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    إذا (f_محفوظ_جدول == خالی_سوچک) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        أرجع;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    لكل (c=0; c<fRB->f_سیٹ_Builder->get_عدد_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    لكل (c=0; c<fRB->f_سیٹ_Builder->get_عدد_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    لكل (n=0; n<f_محفوظ_جدول->size(); n++) {
        UnicodeString *صف_String = (UnicodeString *)f_محفوظ_جدول->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        لكل (c=0; c<fRB->f_سیٹ_Builder->get_عدد_Char_Categories(); c++) {
            RBBIDebugPrintf(" %2d", صف_String->charAt(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#جے_انت



//-----------------------------------------------------------------------------
//
//   printRuleStatusTable    Debug Function.  Dump the common rule status table
//
//-----------------------------------------------------------------------------
#جے_تعریف_شدہ RBBI_DEBUG
فراغ RBBITableBuilder::print_قاعدہ_حالت_جدول() {
    int32_t  this_ریکارڈ = 0;
    int32_t  اگلا_ریکارڈ = 0;
    صحيح      i;
    UVector  *tbl = fRB->f_قاعدہ_حالت_Vals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    طالما (اگلا_ریکارڈ < tbl->size()) {
        this_ریکارڈ = اگلا_ریکارڈ;
        اگلا_ریکارڈ = this_ریکارڈ + tbl->elementAti(this_ریکارڈ) + 1;
        RBBIDebugPrintf("%4d   ", this_ریکارڈ);
        لكل (i=this_ریکارڈ+1; i<اگلا_ریکارڈ; i++) {
            RBBIDebugPrintf("  %5d", tbl->elementAti(i));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#جے_انت


//-----------------------------------------------------------------------------
//
//   RBBIStateDescriptor     Methods.  This is a very struct-like class
//                           Most access is directly to the fields.
//
//-----------------------------------------------------------------------------

RBBIStateDescriptor::RBBIStateDescriptor(صحيح آخری_داخلہ_علامت, UError_کوڈ *f_حالت) {
    fMarked    = خطأ;
    fAccepting = 0;
    fLookAhead = 0;
    f_ٹیگ_سوچی_Idx   = 0;
    f_ٹیگ_Vals   = خالی_سوچک;
    fPositions = خالی_سوچک;
    fDtran     = خالی_سوچک;

    fDtran     = جديد UVector32(آخری_داخلہ_علامت+1, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    إذا (fDtran == خالی_سوچک) {
        *f_حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    fDtran->سیٹ_ناپ(آخری_داخلہ_علامت+1);    // fDtran needs to be pre-sized.
                                           //   It is indexed by input symbols, and will
                                           //   hold  the next state number for each
                                           //   symbol.
}


RBBIStateDescriptor::~RBBIStateDescriptor() {
    مٹاؤ       fPositions;
    مٹاؤ       fDtran;
    مٹاؤ       f_ٹیگ_Vals;
    fPositions = خالی_سوچک;
    fDtran     = خالی_سوچک;
    f_ٹیگ_Vals   = خالی_سوچک;
}

U_NAMESPACE_سرا

#جے_انت /* #if !UCONFIG_NO_BREAK_ITERATION */
