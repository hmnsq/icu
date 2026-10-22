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
منطقي reachableByTransitions(ثابت UVector &حالتون, ثابت int32_t ذريعو,
                            ثابت std::function<منطقي(int32_t)> isSink,
                            ثابت std::function<منطقي(int32_t)> excludedState, UError_ڪوڊ &حالت) {
    UStack boundary(حالت);
    {
        UVector32 &transitions_From_ذريعو =
            *static_cast<RBBIStateDescriptor *>(حالتون.elementAt(ذريعو))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        لكل (int32_t نشان = 0; نشان < transitions_From_ذريعو.size(); ++نشان) {
            ثابت int32_t state = transitions_From_ذريعو.elementAti(نشان);
            إذا (state != 0) {
                boundary.push(state, حالت);
            }
        }
    }
    ثابت خودڪار گهميل = prv::make_unique<منطقي[]>(حالتون.size(), حالت);
    طالما (U_SUCCESS(حالت) && !boundary.empty()) {
        ثابت int32_t s = boundary.popi();
        إذا (isSink(s)) {
            أرجع سچ;
        }
        إذا (excludedState(s) || گهميل[s]) {
            استمر;
        }
        گهميل[s] = سچ;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(حالتون.elementAt(s))->fDtran;
        لكل (int32_t نشان = 0; نشان < transitions.size(); ++نشان) {
            ثابت int32_t t = transitions.elementAti(نشان);
            إذا (t != 0 && !گهميل[t]) {
                boundary.push(t, حالت);
            }
        }
    }
    أرجع خطأ;
}

} // namespace

ثابت int32_t k_Max_State_For8_بٽ_جدول = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **پاڙ_ڳنڍ, UError_ڪوڊ &حالت) :
        fRB(rb),
        f_وڻ(*پاڙ_ڳنڍ),
        f_حالت(&حالت),
        fDStates(خالي_اشارو),
        f_محفوظ_جدول(خالي_اشارو) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = جديد UVector(حالت);
    إذا (U_SUCCESS(حالت) && fDStates == خالي_اشارو ) {
        حالت = U_يادگيري_ورهاست_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    صحيح i;
    لكل (i=0; i<fDStates->size(); i++) {
        ميٽيو static_cast<RBBIStateDescriptor*>(fDStates->elementAt(i));
    }
    ميٽيو fDStates;
    ميٽيو f_محفوظ_جدول;
    ميٽيو f_Look_Ahead_قاعدو_Map;
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
    إذا (f_وڻ==خالي_اشارو) {
        أرجع;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_وڻ = f_وڻ->flattenVariables(*f_حالت, 0);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_وڻ(f_وڻ, سچ);
    }
#جيڪڏهن_پورو

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = جديد RBBINode(RBBINode::opCat, *f_حالت);
    // Exit if memory allocation failed.
    إذا (cn == خالي_اشارو) {
        *f_حالت = U_يادگيري_ورهاست_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        ميٽيو cn;
        أرجع;
    }
    cn->f_کاٻو_ٻار = f_وڻ;
    f_وڻ->f_والد = cn;
    RBBINode *پڇاڙي_Marker_ڳنڍ = cn->f_ساڄو_ٻار = جديد RBBINode(RBBINode::پڇاڙي_Mark, *f_حالت);
    // Delete and exit if memory allocation failed.
    إذا (cn->f_ساڄو_ٻار == خالي_اشارو) {
        *f_حالت = U_يادگيري_ورهاست_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        ميٽيو cn;
        أرجع;
    }
    cn->f_ساڄو_ٻار->f_والد = cn;
    f_وڻ = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_وڻ->flattenSets(*f_حالت, 0);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_وڻ(f_وڻ, سچ);
    }
#جيڪڏهن_پورو


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_وڻ);
    calc_پهريون_Pos(f_وڻ);
    calc_آخري_Pos(f_وڻ);
    calcFollowPos(f_وڻ);
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_وڻ);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    إذا (fRB->f_Chain_قاعدا) {
        calcChainedFollowPos(f_وڻ, پڇاڙي_Marker_ڳنڍ);
    }

    //
    // Build the DFA state transition tables.
    //
    build_State_جدول();
    map_Look_Ahead_قاعدا();
    flag_Accepting_حالتون();
    flag_Look_Ahead_حالتون();
    flag_Tagged_حالتون();

    //
    // Update the global table of rule status {tag} values
    // The rule builder has a global vector of status values that are common
    //    for all tables.  Merge the ones from this table into the global set.
    //
    ملايو_قاعدو_حالت_Vals();
}



//-----------------------------------------------------------------------------
//
//   calcNullable.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcNullable(RBBINode *n) {
    إذا (n == خالي_اشارو) {
        أرجع;
    }
    إذا (n->fType == RBBINode::سيٽ_Ref ||
        n->fType == RBBINode::پڇاڙي_Mark ) {
        // These are non-empty leaf node types.
        n->fNullable = خطأ;
        أرجع;
    }

    إذا (n->fType == RBBINode::lookAhead || n->fType == RBBINode::ٽيگ) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = سچ;
        أرجع;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_کاٻو_ٻار);
    calcNullable(n->f_ساڄو_ٻار);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_کاٻو_ٻار->fNullable || n->f_ساڄو_ٻار->fNullable;
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_کاٻو_ٻار->fNullable && n->f_ساڄو_ٻار->fNullable;
    }
    وإلا إذا (n->fType == RBBINode::op_تارو || n->fType == RBBINode::opQuestion) {
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
فراغ RBBITableBuilder::calc_پهريون_Pos(RBBINode *n) {
    إذا (n == خالي_اشارو) {
        أرجع;
    }
    إذا (n->fType == RBBINode::پن_Char  ||
        n->fType == RBBINode::پڇاڙي_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::ٽيگ) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_پهريون_Pos_سيٽ->جوڙيو_Element(n, *f_حالت);
        أرجع;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calc_پهريون_Pos(n->f_کاٻو_ٻار);
    calc_پهريون_Pos(n->f_ساڄو_ٻار);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        سيٽ_جوڙيو_2(n->f_پهريون_Pos_سيٽ, n->f_کاٻو_ٻار->f_پهريون_Pos_سيٽ);
        سيٽ_جوڙيو_2(n->f_پهريون_Pos_سيٽ, n->f_ساڄو_ٻار->f_پهريون_Pos_سيٽ);
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        سيٽ_جوڙيو_2(n->f_پهريون_Pos_سيٽ, n->f_کاٻو_ٻار->f_پهريون_Pos_سيٽ);
        إذا (n->f_کاٻو_ٻار->fNullable) {
            سيٽ_جوڙيو_2(n->f_پهريون_Pos_سيٽ, n->f_ساڄو_ٻار->f_پهريون_Pos_سيٽ);
        }
    }
    وإلا إذا (n->fType == RBBINode::op_تارو ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        سيٽ_جوڙيو_2(n->f_پهريون_Pos_سيٽ, n->f_کاٻو_ٻار->f_پهريون_Pos_سيٽ);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calc_آخري_Pos(RBBINode *n) {
    إذا (n == خالي_اشارو) {
        أرجع;
    }
    إذا (n->fType == RBBINode::پن_Char  ||
        n->fType == RBBINode::پڇاڙي_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::ٽيگ) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_آخري_Pos_سيٽ->جوڙيو_Element(n, *f_حالت);
        أرجع;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calc_آخري_Pos(n->f_کاٻو_ٻار);
    calc_آخري_Pos(n->f_ساڄو_ٻار);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        سيٽ_جوڙيو_2(n->f_آخري_Pos_سيٽ, n->f_کاٻو_ٻار->f_آخري_Pos_سيٽ);
        سيٽ_جوڙيو_2(n->f_آخري_Pos_سيٽ, n->f_ساڄو_ٻار->f_آخري_Pos_سيٽ);
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        سيٽ_جوڙيو_2(n->f_آخري_Pos_سيٽ, n->f_ساڄو_ٻار->f_آخري_Pos_سيٽ);
        إذا (n->f_ساڄو_ٻار->fNullable) {
            سيٽ_جوڙيو_2(n->f_آخري_Pos_سيٽ, n->f_کاٻو_ٻار->f_آخري_Pos_سيٽ);
        }
    }
    وإلا إذا (n->fType == RBBINode::op_تارو     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        سيٽ_جوڙيو_2(n->f_آخري_Pos_سيٽ, n->f_کاٻو_ٻار->f_آخري_Pos_سيٽ);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcFollowPos(RBBINode *n) {
    إذا (n == خالي_اشارو ||
        n->fType == RBBINode::پن_Char ||
        n->fType == RBBINode::پڇاڙي_Mark) {
        أرجع;
    }

    calcFollowPos(n->f_کاٻو_ٻار);
    calcFollowPos(n->f_ساڄو_ٻار);

    // Aho rule #1
    إذا (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *آخري_Pos_جو_کاٻو_ٻار = n->f_کاٻو_ٻار->f_آخري_Pos_سيٽ;

        لكل (ix = 0; ix < static_cast<uint32_t>(آخري_Pos_جو_کاٻو_ٻار->size()); ix++) {
            i = static_cast<RBBINode*>(آخري_Pos_جو_کاٻو_ٻار->elementAt(ix));
            سيٽ_جوڙيو_2(i->fFollowPos, n->f_ساڄو_ٻار->f_پهريون_Pos_سيٽ);
        }
    }

    // Aho rule #2
    إذا (n->fType == RBBINode::op_تارو ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        لكل (ix = 0; ix < static_cast<uint32_t>(n->f_آخري_Pos_سيٽ->size()); ix++) {
            i = static_cast<RBBINode*>(n->f_آخري_Pos_سيٽ->elementAt(ix));
            سيٽ_جوڙيو_2(i->fFollowPos, n->f_پهريون_Pos_سيٽ);
        }
    }



}

//-----------------------------------------------------------------------------
//
//    addRuleRootNodes    Recursively walk a parse tree, adding all nodes flagged
//                        as roots of a rule to a destination vector.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::جوڙيو_قاعدو_پاڙ_Nodes(UVector *dest, RBBINode *ڳنڍ) {
    إذا (ڳنڍ == خالي_اشارو || U_FAILURE(*f_حالت)) {
        أرجع;
    }
    U_ASSERT(!dest->hasDeleter());
    إذا (ڳنڍ->f_قاعدو_پاڙ) {
        dest->جوڙيو_Element(ڳنڍ, *f_حالت);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        أرجع;
    }
    جوڙيو_قاعدو_پاڙ_Nodes(dest, ڳنڍ->f_کاٻو_ٻار);
    جوڙيو_قاعدو_پاڙ_Nodes(dest, ڳنڍ->f_ساڄو_ٻار);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcChainedFollowPos(RBBINode *وڻ, RBBINode *پڇاڙي_Mark_ڳنڍ) {

    UVector         پن_Nodes(*f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    // get a list all leaf nodes
    وڻ->ڳوليو_Nodes(&پن_Nodes, RBBINode::پن_Char, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector قاعدو_پاڙ_Nodes(*f_حالت);
    جوڙيو_قاعدو_پاڙ_Nodes(&قاعدو_پاڙ_Nodes, وڻ);

    UVector ميلاپ_شروعات_Nodes(*f_حالت);
    لكل (صحيح j=0; j<قاعدو_پاڙ_Nodes.size(); ++j) {
        RBBINode *ڳنڍ = static_cast<RBBINode *>(قاعدو_پاڙ_Nodes.elementAt(j));
        إذا (ڳنڍ->f_Chain_اندريون) {
            سيٽ_جوڙيو_2(&ميلاپ_شروعات_Nodes, ڳنڍ->f_پهريون_Pos_سيٽ);
        }
    }
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    int32_t  پڇاڙي_ڳنڍ_Ix;
    int32_t  شروعات_ڳنڍ_Ix;

    لكل (پڇاڙي_ڳنڍ_Ix=0; پڇاڙي_ڳنڍ_Ix<پن_Nodes.size(); پڇاڙي_ڳنڍ_Ix++) {
        RBBINode* پڇاڙي_ڳنڍ = static_cast<RBBINode*>(پن_Nodes.elementAt(پڇاڙي_ڳنڍ_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        إذا (!پڇاڙي_ڳنڍ->fFollowPos->شامل_آهي(پڇاڙي_Mark_ڳنڍ)) {
            استمر;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *شروعات_ڳنڍ;
        لكل (شروعات_ڳنڍ_Ix = 0; شروعات_ڳنڍ_Ix<ميلاپ_شروعات_Nodes.size(); شروعات_ڳنڍ_Ix++) {
            شروعات_ڳنڍ = static_cast<RBBINode*>(ميلاپ_شروعات_Nodes.elementAt(شروعات_ڳنڍ_Ix));
            إذا (شروعات_ڳنڍ->fType != RBBINode::پن_Char) {
                استمر;
            }

            إذا (پڇاڙي_ڳنڍ->fVal == شروعات_ڳنڍ->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                سيٽ_جوڙيو_2(پڇاڙي_ڳنڍ->fFollowPos, شروعات_ڳنڍ->fFollowPos);
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
    RBBINode  *bof_ڳنڍ = f_وڻ->f_کاٻو_ٻار->f_کاٻو_ٻار;
    U_ASSERT(bof_ڳنڍ->fType == RBBINode::پن_Char);
    U_ASSERT(bof_ڳنڍ->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *ميلاپ_شروعات_Nodes = f_وڻ->f_کاٻو_ٻار->f_ساڄو_ٻار->f_پهريون_Pos_سيٽ;

    RBBINode *شروعات_ڳنڍ;
    صحيح       شروعات_ڳنڍ_Ix;
    لكل (شروعات_ڳنڍ_Ix = 0; شروعات_ڳنڍ_Ix<ميلاپ_شروعات_Nodes->size(); شروعات_ڳنڍ_Ix++) {
        شروعات_ڳنڍ = static_cast<RBBINode*>(ميلاپ_شروعات_Nodes->elementAt(شروعات_ڳنڍ_Ix));
        إذا (شروعات_ڳنڍ->fType != RBBINode::پن_Char) {
            استمر;
        }

        إذا (شروعات_ڳنڍ->fVal == bof_ڳنڍ->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            سيٽ_جوڙيو_2(bof_ڳنڍ->fFollowPos, شروعات_ڳنڍ->fFollowPos);
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
    RBBIStateDescriptor *initialState = خالي_اشارو;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    صحيح      آخري_داخلا_نشان = fRB->f_سيٽ_Builder->get_انگ_Char_Categories() - 1;
    failState = جديد RBBIStateDescriptor(آخري_داخلا_نشان, f_حالت);
    إذا (failState == خالي_اشارو) {
        *f_حالت = U_يادگيري_ورهاست_ERROR;
        اذهب ExitBuildSTdeleteall;
    }
    failState->fPositions = جديد UVector(*f_حالت);
    إذا (failState->fPositions == خالي_اشارو) {
        *f_حالت = U_يادگيري_ورهاست_ERROR;
    }
    إذا (failState->fPositions == خالي_اشارو || U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }
    fDStates->جوڙيو_Element(failState, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = جديد RBBIStateDescriptor(آخري_داخلا_نشان, f_حالت);
    إذا (initialState == خالي_اشارو) {
        *f_حالت = U_يادگيري_ورهاست_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }
    initialState->fPositions = جديد UVector(*f_حالت);
    إذا (initialState->fPositions == خالي_اشارو) {
        *f_حالت = U_يادگيري_ورهاست_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }
    سيٽ_جوڙيو_2(initialState->fPositions, f_وڻ->f_پهريون_Pos_سيٽ);
    fDStates->جوڙيو_Element(initialState, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        اذهب ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    لكل (;;) {
        RBBIStateDescriptor *T = خالي_اشارو;
        int32_t              tx;
        لكل (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *عارضي;
            عارضي = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            إذا (عارضي->fMarked == خطأ) {
                T = عارضي;
                اكسر;
            }
        }
        إذا (T == خالي_اشارو) {
            اكسر;
        }

        // mark T;
        T->fMarked = سچ;

        // for each input symbol a do begin
        int32_t  a;
        لكل (a = 1; a<=آخري_داخلا_نشان; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = خالي_اشارو;
            RBBINode   *p;
            int32_t     px;
            لكل (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                إذا ((p->fType == RBBINode::پن_Char) &&  (p->fVal == a)) {
                    إذا (U == خالي_اشارو) {
                        U = جديد UVector(*f_حالت);
                        إذا (U == خالي_اشارو) {
                        	*f_حالت = U_يادگيري_ورهاست_ERROR;
                        	اذهب ExitBuildSTdeleteall;
                        }
                    }
                    سيٽ_جوڙيو_2(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = خطأ;
            إذا (U != خالي_اشارو) {
                U_ASSERT(U->size() > 0);
                صحيح  ix;
                لكل (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    إذا (سيٽ_Equals(U, temp2->fPositions)) {
                        ميٽيو U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = سچ;
                        اكسر;
                    }
                }

                // Add U as an unmarked state to Dstates
                إذا (!UinDstates)
                {
                    RBBIStateDescriptor *newState = جديد RBBIStateDescriptor(آخري_داخلا_نشان, f_حالت);
                    إذا (newState == خالي_اشارو) {
                    	*f_حالت = U_يادگيري_ورهاست_ERROR;
                    }
                    إذا (U_FAILURE(*f_حالت)) {
                        اذهب ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->جوڙيو_Element(newState, *f_حالت);
                    إذا (U_FAILURE(*f_حالت)) {
                        أرجع;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->سيٽ_Element_At(ux, a);
            }
        }
    }
    أرجع;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    ميٽيو initialState;
    ميٽيو failState;
}


/**
 * mapLookAheadRules
 *
 */
فراغ RBBITableBuilder::map_Look_Ahead_قاعدا() {
    f_Look_Ahead_قاعدو_Map =  جديد UVector32(fRB->fScanner->انگ_قاعدا() + 1, *f_حالت);
    إذا (f_Look_Ahead_قاعدو_Map == خالي_اشارو) {
        *f_حالت = U_يادگيري_ورهاست_ERROR;
    }
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    f_Look_Ahead_قاعدو_Map->سيٽ_سائيز(fRB->fScanner->انگ_قاعدا() + 1);

    لكل (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t la_جڳهه_For_State = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        منطقي saw_Look_Ahead_ڳنڍ = خطأ;
        لكل (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *ڳنڍ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            إذا (ڳنڍ->fType != RBBINode::ڳنڍ_Type::lookAhead) {
                استمر;
            }
            saw_Look_Ahead_ڳنڍ = سچ;
            int32_t قاعدو_انگ = ڳنڍ->fVal;     // Set when rule was originally parsed.
            U_ASSERT(قاعدو_انگ < f_Look_Ahead_قاعدو_Map->size());
            U_ASSERT(قاعدو_انگ > 0);
            int32_t la_جڳهه = f_Look_Ahead_قاعدو_Map->elementAti(قاعدو_انگ);
            إذا (la_جڳهه != 0) {
                إذا (la_جڳهه_For_State == 0) {
                    la_جڳهه_For_State = la_جڳهه;
                } وإلا {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(la_جڳهه == la_جڳهه_For_State);
                }
            }
        }
        إذا (!saw_Look_Ahead_ڳنڍ) {
            استمر;
        }

        إذا (la_جڳهه_For_State == 0) {
            la_جڳهه_For_State = ++f_LASlots_اندريون_Use;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        لكل (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *ڳنڍ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            إذا (ڳنڍ->fType != RBBINode::ڳنڍ_Type::lookAhead) {
                استمر;
            }
            int32_t قاعدو_انگ = ڳنڍ->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = f_Look_Ahead_قاعدو_Map->elementAti(قاعدو_انگ);
            (فراغ)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == la_جڳهه_For_State);
            f_Look_Ahead_قاعدو_Map->سيٽ_Element_At(la_جڳهه_For_State, قاعدو_انگ);
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
فراغ     RBBITableBuilder::flag_Accepting_حالتون() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    UVector     پڇاڙي_Marker_Nodes(*f_حالت);
    RBBINode    *پڇاڙي_Marker;
    int32_t     i;
    int32_t     n;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    f_وڻ->ڳوليو_Nodes(&پڇاڙي_Marker_Nodes, RBBINode::پڇاڙي_Mark, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    لكل (i=0; i<پڇاڙي_Marker_Nodes.size(); i++) {
        پڇاڙي_Marker = static_cast<RBBINode*>(پڇاڙي_Marker_Nodes.elementAt(i));
        لكل (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            إذا (sd->fPositions->اشاريو_جو(پڇاڙي_Marker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                إذا (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = f_Look_Ahead_قاعدو_Map->elementAti(پڇاڙي_Marker->fVal);
                    إذا (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                إذا (sd->fAccepting==ACCEPTING_UNCONDITIONAL && پڇاڙي_Marker->fVal != 0) {
                    // Both lookahead and non-lookahead accepting for this state.
                    // Favor the look-ahead, because a look-ahead match needs to
                    // immediately stop the run-time engine. First match, not longest.
                    sd->fAccepting = f_Look_Ahead_قاعدو_Map->elementAti(پڇاڙي_Marker->fVal);
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
فراغ     RBBITableBuilder::flag_Look_Ahead_حالتون() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    UVector     lookAheadNodes(*f_حالت);
    RBBINode    *look_Ahead_ڳنڍ;
    int32_t     i;
    int32_t     n;

    f_وڻ->ڳوليو_Nodes(&lookAheadNodes, RBBINode::lookAhead, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_ڳنڍ = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_ڳنڍ->fType == RBBINode::ڳنڍ_Type::lookAhead);

        لكل (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->اشاريو_جو(look_Ahead_ڳنڍ);
            إذا (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_ڳنڍ == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookahead_جڳهه = f_Look_Ahead_قاعدو_Map->elementAti(look_Ahead_ڳنڍ->fVal);
                U_ASSERT(sd->fLookAhead == 0 || sd->fLookAhead == lookahead_جڳهه);
                // if (sd->fLookAhead != 0 && sd->fLookAhead != lookaheadSlot) {
                //     printf("%s:%d Bingo. sd->fLookAhead:%d   lookaheadSlot:%d\n",
                //            __FILE__, __LINE__, sd->fLookAhead, lookaheadSlot);
                // }
                sd->fLookAhead = lookahead_جڳهه;
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//    flagTaggedStates
//
//-----------------------------------------------------------------------------
فراغ     RBBITableBuilder::flag_Tagged_حالتون() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    UVector     ٽيگ_Nodes(*f_حالت);
    RBBINode    *ٽيگ_ڳنڍ;
    int32_t     i;
    int32_t     n;

    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    f_وڻ->ڳوليو_Nodes(&ٽيگ_Nodes, RBBINode::ٽيگ, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (i=0; i<ٽيگ_Nodes.size(); i++) {                   // For each tag node t (all of 'em)
        ٽيگ_ڳنڍ = static_cast<RBBINode*>(ٽيگ_Nodes.elementAt(i));

        لكل (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            إذا (sd->fPositions->اشاريو_جو(ٽيگ_ڳنڍ) >= 0) {  //       if  s include the tag node t
                sorted_جوڙيو(&sd->f_ٽيگ_Vals, ٽيگ_ڳنڍ->fVal);
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
فراغ  RBBITableBuilder::ملايو_قاعدو_حالت_Vals() {
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
    إذا (fRB->f_قاعدو_حالت_Vals->size() == 0) {
        fRB->f_قاعدو_حالت_Vals->جوڙيو_Element(1, *f_حالت);  // Num of statuses in group
        fRB->f_قاعدو_حالت_Vals->جوڙيو_Element(static_cast<int32_t>(0), *f_حالت); // and our single status of zero
    }

    //    For each state
    لكل (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_حالتون_ٽيگ_قدرون = sd->f_ٽيگ_Vals;
        إذا (this_حالتون_ٽيگ_قدرون == خالي_اشارو) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->f_ٽيگ_فهرست_Idx = 0;
            استمر;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->f_ٽيگ_فهرست_Idx = -1;
        int32_t  this_ٽيگ_گروهه_شروعات = 0;   // indexes into the global rule status vals list
        int32_t  ايندڙ_ٽيگ_گروهه_شروعات = 0;

        // Loop runs once per group of tags in the global list
        طالما (ايندڙ_ٽيگ_گروهه_شروعات < fRB->f_قاعدو_حالت_Vals->size()) {
            this_ٽيگ_گروهه_شروعات = ايندڙ_ٽيگ_گروهه_شروعات;
            ايندڙ_ٽيگ_گروهه_شروعات += fRB->f_قاعدو_حالت_Vals->elementAti(this_ٽيگ_گروهه_شروعات) + 1;
            إذا (this_حالتون_ٽيگ_قدرون->size() != fRB->f_قاعدو_حالت_Vals->elementAti(this_ٽيگ_گروهه_شروعات)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                استمر;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            لكل (i=0; i<this_حالتون_ٽيگ_قدرون->size(); i++) {
                إذا (this_حالتون_ٽيگ_قدرون->elementAti(i) !=
                    fRB->f_قاعدو_حالت_Vals->elementAti(this_ٽيگ_گروهه_شروعات + 1 + i) ) {
                    // Mismatch.
                    اكسر;
                }
            }

            إذا (i == this_حالتون_ٽيگ_قدرون->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->f_ٽيگ_فهرست_Idx = this_ٽيگ_گروهه_شروعات;
                اكسر;
            }
        }

        إذا (sd->f_ٽيگ_فهرست_Idx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->f_ٽيگ_فهرست_Idx = fRB->f_قاعدو_حالت_Vals->size();
            fRB->f_قاعدو_حالت_Vals->جوڙيو_Element(this_حالتون_ٽيگ_قدرون->size(), *f_حالت);
            لكل (i=0; i<this_حالتون_ٽيگ_قدرون->size(); i++) {
                fRB->f_قاعدو_حالت_Vals->جوڙيو_Element(this_حالتون_ٽيگ_قدرون->elementAti(i), *f_حالت);
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

فراغ RBBITableBuilder::minimize_حالتون() {
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

#إذا UPRV_HAS_SANITIZER
    إذا (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *f_حالت = U_REGEX_نمونو_TOO_BIG;
        أرجع;
    }
#جيڪڏهن_پورو
    بنية StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t f_ٽيگ_فهرست_Idx;
        منطقي عامل==(StateType ثابت &other) ثابت {
            أرجع fAccepting == other.fAccepting &&
                   fLookAhead == other.fLookAhead &&
                   f_ٽيگ_فهرست_Idx == other.f_ٽيگ_فهرست_Idx;
        }
    };
    // We wrap `UVector`s in `unique_ptr``s throughout so we can move them,
    // including into `UVector`s (by releasing them from the `unique_ptr`
    // and having the enclosing `UVector` adopt them). 
    // Group the states by types (but we have no maps so this is verbose).
    // If there are no lookaheads and no tags, there are only two types
    // (accepting and non-accepting) in which case this is exactly step 1
    // of Algorithm 3.6.
    بنية Type_منزل_حالتون : UMemory {
        Type_منزل_حالتون(ثابت StateType &type, UError_ڪوڊ &حالت)
            : type(type), حالتون(prv::make_unique<UVector>(حالت, حالت)) {}
        StateType type;
        prv::unique_ptr<UVector> حالتون;
    };
    UVector initial_حصا_ڪريو(*f_حالت);
    initial_حصا_ڪريو.سيٽ_Deleter(
        [](فراغ *p) { ميٽيو static_cast<Type_منزل_حالتون *>(p); });
    لكل (int32_t i = 0; i < fDStates->size(); ++i) {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ثابت StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.f_ٽيگ_فهرست_Idx};
        int32_t j = 0;
        لكل (; j < initial_حصا_ڪريو.size(); ++j) {
            خودڪار &[type_j, حالتون] =
                *static_cast<Type_منزل_حالتون *>(initial_حصا_ڪريو.elementAt(j));
            إذا (type_j == type) {
                حالتون->جوڙيو_Element(i, *f_حالت);
                اكسر;
            }
        }
        إذا (j == initial_حصا_ڪريو.size()) {
            خودڪار new_داخلا = prv::make_unique<Type_منزل_حالتون>(type, *f_حالت, *f_حالت);
            إذا (U_FAILURE(*f_حالت)) {
                أرجع;
            }
            new_داخلا->حالتون->جوڙيو_Element(i, *f_حالت);
            initial_حصا_ڪريو.adoptElement(new_داخلا.release(), *f_حالت);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    خودڪار حصا_ڪريو = prv::make_unique<UVector>(*f_حالت, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    حصا_ڪريو->سيٽ_Deleter(
        [](فراغ *p) { ميٽيو static_cast<UVector*>(p); });
    
    لكل (int32_t i = 0; i < initial_حصا_ڪريو.size(); ++i) {
        حصا_ڪريو->adoptElement(
            static_cast<Type_منزل_حالتون*>(initial_حصا_ڪريو.elementAt(i))->حالتون.release(),
            *f_حالت);
    }
    // Given the index of a state 𝑠 in `fDStates`, returns a UVector of integers
    // σ(𝑠) such that 𝑠.fDtran[i] (the index of the state reached by the
    // transition from 𝑠 on the character class with index i) is in Π[σ(𝑠)[i]].
    // We then have σ(𝑠) == σ(𝑡) if and only if “for all input symbols 𝑎, states
    // 𝑠 and 𝑡 have transitions on 𝑎 to states in the same group of Π” (which is
    // what defines the refinement of 𝐺 in Fig. 3.45).
    خودڪار حصا_ڪريو_signature = [&حصا_ڪريو, هذا](int32_t state_اشاريو) -> prv::unique_ptr<UVector> {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state_اشاريو));
        خودڪار نتيجو = prv::make_unique<UVector>(state.fDtran->size(), *f_حالت, *f_حالت);
        إذا (U_FAILURE(*f_حالت)) {
            أرجع خالي_اشارو;
        }
        لكل (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t منزل_State = state.fDtran->elementAti(i);
            لكل (int32_t part_اشاريو = 0; part_اشاريو < حصا_ڪريو->size(); ++part_اشاريو) {
                ثابت UVector &part = *static_cast<UVector *>(حصا_ڪريو->elementAt(part_اشاريو));
                إذا (part.شامل_آهي(منزل_State)) {
                    نتيجو->جوڙيو_Element(part_اشاريو, *f_حالت);
                    اكسر;
                }
            }
        }
        نتيجو->سيٽ_Comparer([](ثابت UElement کاٻي, ثابت UElement ساڄي) -> UBool {
            أرجع کاٻي.integer == ساڄي.integer;
        });
        أرجع نتيجو;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    لكل (;;) {
        // Π_new.
        خودڪار حصا_ڪريو_New = prv::make_unique<UVector>(*f_حالت, *f_حالت);
        إذا (U_FAILURE(*f_حالت)) {
            أرجع;
        }
        حصا_ڪريو_New->سيٽ_Deleter(
            [](فراغ *p) { ميٽيو static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        منطقي refined = خطأ;
        لكل (int32_t i = 0; i < حصا_ڪريو->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ثابت UVector &گروهه = *static_cast<UVector*>(حصا_ڪريو->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            بنية Signature_منزل_حالتون : UMemory {
                Signature_منزل_حالتون(prv::unique_ptr<UVector> signature, UError_ڪوڊ &حالت)
                    : signature(std::move(signature)), حالتون(prv::make_unique<UVector>(حالت, حالت)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> حالتون;
            };
            UVector گروهه_Refinement(*f_حالت);
            گروهه_Refinement.سيٽ_Deleter([](فراغ *p) { ميٽيو static_cast<Signature_منزل_حالتون*>(p); });
            لكل (int32_t j = 0; j < گروهه.size(); ++j) {
                // Index of a state in the group.
                ثابت int32_t s = گروهه.elementAti(j);
                خودڪار signature = حصا_ڪريو_signature(s);
                int32_t k = 0;
                لكل (; k < گروهه_Refinement.size(); ++k) {
                    ثابت خودڪار &[subgroupSignature, subgroup] =
                        *static_cast<Signature_منزل_حالتون*>(گروهه_Refinement.elementAt(k));
                    إذا (*subgroupSignature == *signature) {
                        subgroup->جوڙيو_Element(s, *f_حالت);
                        اكسر;
                    }
                }
                إذا (k == گروهه_Refinement.size()) {
                    ثابت خودڪار new_داخلا =
                        جديد Signature_منزل_حالتون(std::move(signature), *f_حالت);
                    إذا (U_FAILURE(*f_حالت)) {
                        أرجع;
                    }
                    إذا (new_داخلا == خالي_اشارو) {
                      *f_حالت = U_يادگيري_ورهاست_ERROR;
                      أرجع;
                    }
                    new_داخلا->حالتون->جوڙيو_Element(s, *f_حالت);
                    گروهه_Refinement.adoptElement(new_داخلا, *f_حالت);
                }
            }
            refined |= گروهه_Refinement.size() > 1;
            لكل (int32_t j = 0; j < گروهه_Refinement.size(); ++j) {
                خودڪار &[_, subgroup] = *static_cast<Signature_منزل_حالتون*>(
                    گروهه_Refinement.elementAt(j));
                حصا_ڪريو_New->adoptElement(subgroup.release(), *f_حالت);
            }
        }
        إذا (refined) {
            حصا_ڪريو = std::move(حصا_ڪريو_New);
        } وإلا {
            اكسر;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    لكل (صحيح i = 0; i < حصا_ڪريو->size(); ++i) {
        UVector *ثابت part_i = static_cast<UVector *>(حصا_ڪريو->elementAt(i));
        إذا (part_i->شامل_آهي(0)) {
            فراغ *ثابت part_0 = حصا_ڪريو->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ثابت خودڪار deleter = حصا_ڪريو->سيٽ_Deleter(خالي_اشارو);
            حصا_ڪريو->سيٽ_Element_At(part_i, 0);
            حصا_ڪريو->سيٽ_Element_At(part_0, i);
            حصا_ڪريو->سيٽ_Deleter(deleter);
        } وإلا إذا (part_i->شامل_آهي(1)) {
            فراغ *ثابت part_1 = حصا_ڪريو->elementAt(1);
            ثابت خودڪار deleter = حصا_ڪريو->سيٽ_Deleter(خالي_اشارو);
            حصا_ڪريو->سيٽ_Element_At(part_i, 1);
            حصا_ڪريو->سيٽ_Element_At(part_1, i);
            حصا_ڪريو->سيٽ_Deleter(deleter);
        }
    }
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    خودڪار old_State_منزل_Part = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (صحيح i = 0; i < حصا_ڪريو->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(حصا_ڪريو->elementAt(i));
        لكل (صحيح j = 0; j < part.size(); ++j) {
            old_State_منزل_Part[part.elementAti(j)] = i;
        }
    }
    خودڪار ثابت old_حالتون = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*f_حالت, *f_حالت).release();
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (صحيح i = 0; i < حصا_ڪريو->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(حصا_ڪريو->elementAt(i));
        RBBIStateDescriptor *ثابت state =
            static_cast<RBBIStateDescriptor*>(old_حالتون->elementAt(part.elementAti(0)));
        fDStates->جوڙيو_Element(state, *f_حالت);
        لكل (صحيح j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->سيٽ_Element_At(old_State_منزل_Part[state->fDtran->elementAti(j)], j);
        }
        old_حالتون->سيٽ_Element_At(خالي_اشارو, part.elementAti(0));
    }
    old_حالتون->سيٽ_Deleter([](فراغ *p) { ميٽيو static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::sorted_جوڙيو(UVector **vector, int32_t val) {
    int32_t i;

    إذا (*vector == خالي_اشارو) {
        *vector = جديد UVector(*f_حالت);
    }
    إذا (*vector == خالي_اشارو || U_FAILURE(*f_حالت)) {
        أرجع;
    }
    UVector *vec = *vector;
    int32_t  v_سائيز = vec->size();
    لكل (i=0; i<v_سائيز; i++) {
        int32_t valAtI = vec->elementAti(i);
        إذا (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            أرجع;
        }
        إذا (valAtI > val) {
            اكسر;
        }
    }
    vec->وجهو_Element_At(val, i, *f_حالت);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::سيٽ_جوڙيو_2(UVector *dest, UVector *ذريعو) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!ذريعو->hasDeleter());
    int32_t dest_Original_سائيز = dest->size();
    int32_t ذريعو_سائيز       = ذريعو->size();
    int32_t di           = 0;
    Maybe_ڍير_Array<فراغ *, 16> destArray, ذريعو_Array;  // Handle small cases without malloc
    فراغ **destPtr, **ذريعو_Ptr;
    فراغ **destLim, **ذريعو_Lim;

    إذا (dest_Original_سائيز > destArray.get_صلاحيت()) {
        إذا (destArray.resize(dest_Original_سائيز) == خالي_اشارو) {
            أرجع;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + dest_Original_سائيز;  // destArray.getArrayLimit()?

    إذا (ذريعو_سائيز > ذريعو_Array.get_صلاحيت()) {
        إذا (ذريعو_Array.resize(ذريعو_سائيز) == خالي_اشارو) {
            أرجع;
        }
    }
    ذريعو_Ptr = ذريعو_Array.getAlias();
    ذريعو_Lim = ذريعو_Ptr + ذريعو_سائيز;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (فراغ) dest->منزل_Array(destPtr);
    (فراغ) ذريعو->منزل_Array(ذريعو_Ptr);

    dest->سيٽ_سائيز(ذريعو_سائيز+dest_Original_سائيز, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }

    طالما (ذريعو_Ptr < ذريعو_Lim && destPtr < destLim) {
        إذا (*destPtr == *ذريعو_Ptr) {
            dest->سيٽ_Element_At(*ذريعو_Ptr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        وإلا إذا (uprv_memcmp(destPtr, ذريعو_Ptr, حجم(فراغ *)) < 0) {
            dest->سيٽ_Element_At(*destPtr++, di++);
        }
        وإلا { /* *sourcePtr < *destPtr */
            dest->سيٽ_Element_At(*ذريعو_Ptr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    طالما (destPtr < destLim) {
        dest->سيٽ_Element_At(*destPtr++, di++);
    }
    طالما (ذريعو_Ptr < ذريعو_Lim) {
        dest->سيٽ_Element_At(*ذريعو_Ptr++, di++);
    }

    dest->سيٽ_سائيز(di, *f_حالت);
}



//-----------------------------------------------------------------------------
//
//  setEqual    Set operation on UVector.
//              Compare for equality.
//              Elements must be sorted.
//
//-----------------------------------------------------------------------------
UBool RBBITableBuilder::سيٽ_Equals(UVector *a, UVector *b) {
    أرجع a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
فراغ RBBITableBuilder::printPosSets(RBBINode *n) {
    إذا (n==خالي_اشارو) {
        أرجع;
    }
    لکو("\n");
    RBBINode::print_ڳنڍ_سرو();
    RBBINode::print_ڳنڍ(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    print_سيٽ(n->f_پهريون_Pos_سيٽ);

    RBBIDebugPrintf("         lastpos:   ");
    print_سيٽ(n->f_آخري_Pos_سيٽ);

    RBBIDebugPrintf("         followpos: ");
    print_سيٽ(n->fFollowPos);

    printPosSets(n->f_کاٻو_ٻار);
    printPosSets(n->f_ساڄو_ٻار);
}
#جيڪڏهن_پورو

//
//    findDuplCharClassFrom()
//
منطقي RBBITableBuilder::ڳوليو_Dupl_Char_Class_From(IntPair *categories) {
    int32_t انگ_حالتون = fDStates->size();
    int32_t انگ_کانن_ڳڻپ = fRB->f_سيٽ_Builder->get_انگ_Char_Categories();

    لكل (; categories->first < انگ_کانن_ڳڻپ-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        صحيح حد_بندي_ٻيو = categories->first < fRB->f_سيٽ_Builder->get_Dict_Categories_شروعات() ?
            fRB->f_سيٽ_Builder->get_Dict_Categories_شروعات() : انگ_کانن_ڳڻپ;
        لكل (categories->ٻيو=categories->first+1; categories->ٻيو < حد_بندي_ٻيو; categories->ٻيو++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t جدول_بنياد = 0;
            uint16_t جدول_dupl = 1;
            لكل (int32_t state=0; state<انگ_حالتون; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                جدول_بنياد = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                جدول_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->ٻيو));
                إذا (جدول_بنياد != جدول_dupl) {
                    اكسر;
                }
            }
            إذا (جدول_بنياد == جدول_dupl) {
                أرجع سچ;
            }
        }
    }
    أرجع خطأ;
}


//
//    removeColumn()
//
فراغ RBBITableBuilder::remove_ڪالم(int32_t ڪالم) {
    int32_t انگ_حالتون = fDStates->size();
    لكل (int32_t state=0; state<انگ_حالتون; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(ڪالم < sd->fDtran->size());
        sd->fDtran->removeElementAt(ڪالم);
    }
}


منطقي RBBITableBuilder::ڳوليو_Duplicate_محفوظ_State(IntPair *حالتون) {
    int32_t انگ_حالتون = f_محفوظ_جدول->size();

    لكل (; حالتون->first<انگ_حالتون-1; حالتون->first++) {
        UnicodeString *پهريون_صف = static_cast<UnicodeString *>(f_محفوظ_جدول->elementAt(حالتون->first));
        لكل (حالتون->ٻيو=حالتون->first+1; حالتون->ٻيو<انگ_حالتون; حالتون->ٻيو++) {
            UnicodeString *dupl_صف = static_cast<UnicodeString *>(f_محفوظ_جدول->elementAt(حالتون->ٻيو));
            منطقي صفون_ميلاپ = سچ;
            int32_t انگ_کانن_ڳڻپ = پهريون_صف->length();
            لكل (int32_t کانو=0; کانو < انگ_کانن_ڳڻپ; ++کانو) {
                int32_t پهريون_Val = پهريون_صف->charAt(کانو);
                int32_t duplVal = dupl_صف->charAt(کانو);
                إذا (!((پهريون_Val == duplVal) ||
                        ((پهريون_Val == حالتون->first || پهريون_Val == حالتون->ٻيو) &&
                        (duplVal  == حالتون->first || duplVal  == حالتون->ٻيو)))) {
                    صفون_ميلاپ = خطأ;
                    اكسر;
                }
            }
            إذا (صفون_ميلاپ) {
                أرجع سچ;
            }
        }
    }
    أرجع خطأ;
}

فراغ RBBITableBuilder::remove_محفوظ_State(IntPair dupl_حالتون) {
    ثابت int32_t keepState = dupl_حالتون.first;
    ثابت int32_t duplState = dupl_حالتون.ٻيو;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < f_محفوظ_جدول->size());

    f_محفوظ_جدول->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t انگ_حالتون = f_محفوظ_جدول->size();
    لكل (int32_t state=0; state<انگ_حالتون; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(f_محفوظ_جدول->elementAt(state));
        int32_t انگ_کانن_ڳڻپ = sd->length();
        لكل (int32_t کانو=0; کانو<انگ_کانن_ڳڻپ; کانو++) {
            int32_t existingVal = sd->charAt(کانو);
            int32_t newVal = existingVal;
            إذا (existingVal == duplState) {
                newVal = keepState;
            } وإلا إذا (existingVal > duplState) {
                newVal = existingVal - 1;
            }
            sd->سيٽ_Char_At(کانو, static_cast<char16_t>(newVal));
        }
    }
}


/*
 * RemoveDuplicateStates
 */
int32_t RBBITableBuilder::remove_Duplicate_حالتون() {
    ثابت int32_t old_State_ڳڻپ = fDStates->size();
    minimize_حالتون();
    أرجع fDStates->size() - old_State_ڳڻپ;
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
    إذا (f_LASlots_اندريون_Use == ACCEPTING_UNCONDITIONAL) {
        أرجع;
    }
    ثابت int32_t lookahead_ڳڻپ = f_LASlots_اندريون_Use - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ثابت int32_t پهريون_Lookahead = ACCEPTING_UNCONDITIONAL + 1;
    ثابت خودڪار stateDescriptor = [هذا](ثابت int32_t state) -> RBBIStateDescriptor & {
        أرجع *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ثابت خودڪار lookaheadReachability =
        prv::make_unique<منطقي[]>(lookahead_ڳڻپ * lookahead_ڳڻپ, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    لكل (int32_t l = پهريون_Lookahead; l <= f_LASlots_اندريون_Use; ++l) {
        لكل (int32_t k = پهريون_Lookahead; k <= f_LASlots_اندريون_Use; ++k) {
            إذا (k != l) {
                لكل (int32_t ذريعو = 1; ذريعو < fDStates->size(); ++ذريعو) {
                    إذا (static_cast<int32_t>(stateDescriptor(ذريعو).fLookAhead) != l) {
                        استمر;
                    }
                    إذا (reachableByTransitions(
                            *fDStates, ذريعو, /*isSink=*/
                            [&](ثابت int32_t state) {
                                أرجع static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](ثابت int32_t state) {
                                أرجع static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *f_حالت)) {
                        lookaheadReachability[(l - پهريون_Lookahead) * lookahead_ڳڻپ + k -
                                              پهريون_Lookahead] = سچ;
                    }
                }
            }
        }
    }

    ثابت خودڪار colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_ڳڻپ, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    لكل (int32_t chromatic_عدد = 1;; ++chromatic_عدد) {
        uprv_memset(colours.get(), 0, حجم(int32_t) * lookahead_ڳڻپ);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        لكل (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            لكل (صحيح ذريعو = 0; ذريعو < lookahead_ڳڻپ; ++ذريعو) {
                لكل (صحيح sink = 0; sink < lookahead_ڳڻپ; ++sink) {
                    إذا (lookaheadReachability[ذريعو * lookahead_ڳڻپ + sink] &&
                        colours[ذريعو] == colours[sink]) {
                        اذهب ايندڙ_Colouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            لكل (صحيح i = 0; i < fDStates->size(); ++i) {
                إذا (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        پهريون_Lookahead +
                        colours[stateDescriptor(i).fAccepting - پهريون_Lookahead];
                }
                إذا (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - پهريون_Lookahead];
                }
            }
            f_LASlots_اندريون_Use = ACCEPTING_UNCONDITIONAL + chromatic_عدد;
            أرجع;

          ايندڙ_Colouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            لكل (int32_t i = 0; i < lookahead_ڳڻپ - 1 && colours[i] == chromatic_عدد;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            إذا (colours[lookahead_ڳڻپ - 1] == chromatic_عدد) {
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
int32_t  RBBITableBuilder::get_جدول_سائيز() ثابت {
    int32_t    size = 0;
    int32_t    انگ_صفون;
    int32_t    انگ_کانن_ڳڻپ;
    int32_t    صف_سائيز;

    إذا (f_وڻ == خالي_اشارو) {
        أرجع 0;
    }

    size    = offsetof(RBBIState_جدول, f_جدول_معلومات);    // The header, with no rows to the table.

    انگ_صفون = fDStates->size();
    انگ_کانن_ڳڻپ = fRB->f_سيٽ_Builder->get_انگ_Char_Categories();

    إذا (use8_بٽ_For_جدول()) {
        صف_سائيز = offsetof(RBBIState_جدول_Row8, f_ايندڙ_State) + حجم(int8_t)*انگ_کانن_ڳڻپ;
    } وإلا {
        صف_سائيز = offsetof(RBBIState_جدول_Row16, f_ايندڙ_State) + حجم(int16_t)*انگ_کانن_ڳڻپ;
    }
    size   += انگ_صفون * صف_سائيز;
    أرجع size;
}

منطقي RBBITableBuilder::use8_بٽ_For_جدول() ثابت {
    أرجع fDStates->size() <= k_Max_State_For8_بٽ_جدول;
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
    صحيح                کانو;

    إذا (U_FAILURE(*f_حالت) || f_وڻ == خالي_اشارو) {
        أرجع;
    }

    int32_t cat_ڳڻپ = fRB->f_سيٽ_Builder->get_انگ_Char_Categories();
    إذا (cat_ڳڻپ > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *f_حالت = U_BRK_اندروني_ERROR;
        أرجع;
    }

    جدول->f_انگ_حالتون = fDStates->size();
    جدول->f_Dict_Categories_شروعات = fRB->f_سيٽ_Builder->get_Dict_Categories_شروعات();
    جدول->f_Look_Ahead_Results_سائيز = f_LASlots_اندريون_Use == ACCEPTING_UNCONDITIONAL ? 0 : f_LASlots_اندريون_Use + 1;
    جدول->fFlags     = 0;
    إذا (use8_بٽ_For_جدول()) {
        جدول->f_صف_Len    = offsetof(RBBIState_جدول_Row8, f_ايندڙ_State) + حجم(uint8_t) * cat_ڳڻپ;
        جدول->fFlags  |= RBBI_8_بٽ_صفون;
    } وإلا {
        جدول->f_صف_Len    = offsetof(RBBIState_جدول_Row16, f_ايندڙ_State) + حجم(int16_t) * cat_ڳڻپ;
    }
    إذا (fRB->fLookAheadHardBreak) {
        جدول->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    لكل (state=0; state<جدول->f_انگ_حالتون; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIState_جدول_صف* صف = reinterpret_cast<RBBIState_جدول_صف*>(جدول->f_جدول_معلومات + state * جدول->f_صف_Len);
        إذا (use8_بٽ_For_جدول()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->f_ٽيگ_فهرست_Idx && sd->f_ٽيگ_فهرست_Idx <= 255);
            RBBIState_جدول_Row8* r8 = reinterpret_cast<RBBIState_جدول_Row8*>(صف);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->f_ٽيگ_فهرست_Idx   = sd->f_ٽيگ_فهرست_Idx;
            لكل (کانو=0; کانو<cat_ڳڻپ; کانو++) {
                U_ASSERT (sd->fDtran->elementAti(کانو) <= k_Max_State_For8_بٽ_جدول);
                r8->f_ايندڙ_State[کانو] = sd->fDtran->elementAti(کانو);
            }
        } وإلا {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->f_ٽيگ_فهرست_Idx && sd->f_ٽيگ_فهرست_Idx <= 0xffff);
            صف->r16.fAccepting = sd->fAccepting;
            صف->r16.fLookAhead = sd->fLookAhead;
            صف->r16.f_ٽيگ_فهرست_Idx   = sd->f_ٽيگ_فهرست_Idx;
            لكل (کانو=0; کانو<cat_ڳڻپ; کانو++) {
                صف->r16.f_ايندڙ_State[کانو] = sd->fDtran->elementAti(کانو);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
فراغ RBBITableBuilder::build_محفوظ_ابتو_جدول(UError_ڪوڊ &حالت) {
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
    UnicodeString محفوظ_جوڙا;

    int32_t انگ_Char_Classes = fRB->f_سيٽ_Builder->get_انگ_Char_Categories();
    int32_t انگ_حالتون = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ثابت خودڪار lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_اندريون_Use + 1, حالت);
    ثابت خودڪار wanted_Lookaheads_اندريون_Pair =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_اندريون_Use + 1, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t شروعات_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_سيٽ = -1;
    لكل (int32_t c1 = 1; c1 < انگ_Char_Classes; ++c1) {
        إذا (fRB->f_سيٽ_Builder->get_پهريون_Char(c1) < 0) {
          استمر;
        }
        لكل (int32_t c2 = 1; c2 < انگ_Char_Classes; ++c2) {
            إذا (fRB->f_سيٽ_Builder->get_پهريون_Char(c2) < 0) {
                استمر;
            }
            int32_t wanted_پڇاڙي_State = -1;
            int32_t wanted_آخري_Accepting_Position;
            منطقي must_Check_Consistent_With_شروعات_C2 = خطأ;
            ثابت int32_t متن[2] = {c1, c2};
            لكل (int32_t injectedState = شروعات_STATE; injectedState < انگ_حالتون; ++injectedState) {
                لكل (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= f_LASlots_اندريون_Use; ++injectedLookahead) {
                    int32_t آخري_Break = BEFORE_PAIR;
                    int32_t آخري_Accepting_Position = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_سيٽ, f_LASlots_اندريون_Use + 1);
                    إذا (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ثابت RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        إذا (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            آخري_Accepting_Position = 0;
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
                        s = شروعات_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_سيٽ, f_LASlots_اندريون_Use + 1);
                        i = آخري_Break;
                        // Loop Find_Next_Break in L2/26-135.
                        لكل (;;) {
                        dfa_قدم:
                            إذا (i == BEFORE_PAIR || i == 2) {
                                اذهب inspectFinalState;
                            }
                            ثابت int32_t نشان_Ahead = متن[i];
                            ++i;
                            ثابت RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            إذا (sDescriptor->fDtran->elementAti(نشان_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(نشان_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } وإلا {
                                إذا (آخري_Break != BEFORE_PAIR &&
                                    آخري_Accepting_Position == آخري_Break) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    اذهب ايندڙ_نشان_Pair;
                                }
                                آخري_Break = آخري_Accepting_Position;
                                اكسر;
                            }

                            إذا (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                آخري_Accepting_Position = i;
                            } وإلا إذا (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_سيٽ) {
                                آخري_Break = lookaheadPositions[sDescriptor->fAccepting];
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
                    إذا (wanted_پڇاڙي_State < 0) {
                        wanted_پڇاڙي_State = s;
                        wanted_آخري_Accepting_Position = آخري_Accepting_Position;
                        لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_اندريون_Use; ++l) {
                            إذا (lookaheadPositions[l] != LOOKAHEAD_NOT_سيٽ &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wanted_Lookaheads_اندريون_Pair[l] = lookaheadPositions[i];
                            } وإلا {
                                wanted_Lookaheads_اندريون_Pair[l] = LOOKAHEAD_NOT_سيٽ;
                            }
                        }
                    } وإلا {
                        إذا (wanted_پڇاڙي_State != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            اذهب ايندڙ_نشان_Pair;
                        }
                        إذا (wanted_آخري_Accepting_Position != آخري_Accepting_Position) {
                            اذهب ايندڙ_نشان_Pair;
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
                    لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_اندريون_Use; ++l) {
                        إذا (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wanted_Lookaheads_اندريون_Pair[l]) {
                            استمر;
                        }
                        إذا (lookaheadPositions[l] == 2 || wanted_Lookaheads_اندريون_Pair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            إذا (wanted_پڇاڙي_State != شروعات_STATE) {
                                اذهب ايندڙ_نشان_Pair;
                            }
                        }
                        إذا (lookaheadPositions[l] == 1 || wanted_Lookaheads_اندريون_Pair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            must_Check_Consistent_With_شروعات_C2 = سچ;
                        }
                    }
                }
            }
            إذا (must_Check_Consistent_With_شروعات_C2) {
                ثابت RBBIStateDescriptor& شروعات_Descriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(شروعات_STATE));
                ثابت خودڪار c2State = شروعات_Descriptor.fDtran->elementAti(c2);
                إذا (c2State != wanted_پڇاڙي_State) {
                    اذهب ايندڙ_نشان_Pair;
                }
                ثابت RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                إذا (wanted_آخري_Accepting_Position != 2) {
                    اذهب ايندڙ_نشان_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_اندريون_Use; ++l) {
                    إذا (((wanted_Lookaheads_اندريون_Pair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wanted_پڇاڙي_State != شروعات_STATE) {
                        اذهب ايندڙ_نشان_Pair;
                    }
                }
            }
            محفوظ_جوڙا.append(static_cast<char16_t>(c1));
            محفوظ_جوڙا.append(static_cast<char16_t>(c2));
        ايندڙ_نشان_Pair:
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

    U_ASSERT(f_محفوظ_جدول == خالي_اشارو);
    Local_اشارو<UVector> lp_محفوظ_جدول(
        جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, انگ_Char_Classes + 2, حالت), حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    f_محفوظ_جدول = lp_محفوظ_جدول.orphan();
    لكل (int32_t صف=0; صف<انگ_Char_Classes + 2; ++صف) {
        Local_اشارو<UnicodeString> lpString(جديد UnicodeString(انگ_Char_Classes, 0, انگ_Char_Classes+4), حالت);
        f_محفوظ_جدول->adoptElement(lpString.orphan(), حالت);
    }
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &شروعات_State = *static_cast<UnicodeString *>(f_محفوظ_جدول->elementAt(1));
    لكل (int32_t charClass=0; charClass < انگ_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        شروعات_State.سيٽ_Char_At(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    لكل (int32_t صف=2; صف<انگ_Char_Classes+2; ++صف) {
        UnicodeString &صف_State = *static_cast<UnicodeString *>(f_محفوظ_جدول->elementAt(صف));
        صف_State = شروعات_State;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    لكل (int32_t pairIdx=0; pairIdx<محفوظ_جوڙا.length(); pairIdx+=2) {
        int32_t c1 = محفوظ_جوڙا.charAt(pairIdx);
        int32_t c2 = محفوظ_جوڙا.charAt(pairIdx + 1);

        UnicodeString &صف_State = *static_cast<UnicodeString *>(f_محفوظ_جدول->elementAt(c2 + 2));
        صف_State.سيٽ_Char_At(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair حالتون = {1, 0};
    طالما (ڳوليو_Duplicate_محفوظ_State(&حالتون)) {
        // printf("Removing duplicate safe states (%d, %d)\n", states.first, states.second);
        remove_محفوظ_State(حالتون);
    }
}


//-----------------------------------------------------------------------------
//
//   getSafeTableSize()    Calculate the size of the runtime form of this
//                         safe state table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::get_محفوظ_جدول_سائيز() ثابت {
    int32_t    size = 0;
    int32_t    انگ_صفون;
    int32_t    انگ_کانن_ڳڻپ;
    int32_t    صف_سائيز;

    إذا (f_محفوظ_جدول == خالي_اشارو) {
        أرجع 0;
    }

    size    = offsetof(RBBIState_جدول, f_جدول_معلومات);    // The header, with no rows to the table.

    انگ_صفون = f_محفوظ_جدول->size();
    انگ_کانن_ڳڻپ = fRB->f_سيٽ_Builder->get_انگ_Char_Categories();

    إذا (use8_بٽ_For_محفوظ_جدول()) {
        صف_سائيز = offsetof(RBBIState_جدول_Row8, f_ايندڙ_State) + حجم(int8_t)*انگ_کانن_ڳڻپ;
    } وإلا {
        صف_سائيز = offsetof(RBBIState_جدول_Row16, f_ايندڙ_State) + حجم(int16_t)*انگ_کانن_ڳڻپ;
    }
    size   += انگ_صفون * صف_سائيز;
    أرجع size;
}

منطقي RBBITableBuilder::use8_بٽ_For_محفوظ_جدول() ثابت {
    أرجع f_محفوظ_جدول->size() <= k_Max_State_For8_بٽ_جدول;
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
    صحيح                کانو;

    إذا (U_FAILURE(*f_حالت) || f_محفوظ_جدول == خالي_اشارو) {
        أرجع;
    }

    int32_t cat_ڳڻپ = fRB->f_سيٽ_Builder->get_انگ_Char_Categories();
    إذا (cat_ڳڻپ > 0x7fff ||
            f_محفوظ_جدول->size() > 0x7fff) {
        *f_حالت = U_BRK_اندروني_ERROR;
        أرجع;
    }

    جدول->f_انگ_حالتون = f_محفوظ_جدول->size();
    جدول->fFlags     = 0;
    إذا (use8_بٽ_For_محفوظ_جدول()) {
        جدول->f_صف_Len    = offsetof(RBBIState_جدول_Row8, f_ايندڙ_State) + حجم(uint8_t) * cat_ڳڻپ;
        جدول->fFlags  |= RBBI_8_بٽ_صفون;
    } وإلا {
        جدول->f_صف_Len    = offsetof(RBBIState_جدول_Row16, f_ايندڙ_State) + حجم(int16_t) * cat_ڳڻپ;
    }

    لكل (state=0; state<جدول->f_انگ_حالتون; state++) {
        UnicodeString* صف_String = static_cast<UnicodeString*>(f_محفوظ_جدول->elementAt(state));
        RBBIState_جدول_صف* صف = reinterpret_cast<RBBIState_جدول_صف*>(جدول->f_جدول_معلومات + state * جدول->f_صف_Len);
        إذا (use8_بٽ_For_محفوظ_جدول()) {
            RBBIState_جدول_Row8* r8 = reinterpret_cast<RBBIState_جدول_Row8*>(صف);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->f_ٽيگ_فهرست_Idx    = 0;
            لكل (کانو=0; کانو<cat_ڳڻپ; کانو++) {
                U_ASSERT(صف_String->charAt(کانو) <= k_Max_State_For8_بٽ_جدول);
                r8->f_ايندڙ_State[کانو] = static_cast<uint8_t>(صف_String->charAt(کانو));
            }
        } وإلا {
            صف->r16.fAccepting = 0;
            صف->r16.fLookAhead = 0;
            صف->r16.f_ٽيگ_فهرست_Idx    = 0;
            لكل (کانو=0; کانو<cat_ڳڻپ; کانو++) {
                صف->r16.f_ايندڙ_State[کانو] = صف_String->charAt(کانو);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//   printSet    Debug function.   Print the contents of a UVector
//
//-----------------------------------------------------------------------------
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
فراغ RBBITableBuilder::print_سيٽ(UVector *s) {
    int32_t  i;
    لكل (i=0; i<s->size(); i++) {
        ثابت RBBINode *v = static_cast<ثابت RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==خالي_اشارو? -1 : v->f_Serial_انگ);
    }
    RBBIDebugPrintf("\n");
}
#جيڪڏهن_پورو


//-----------------------------------------------------------------------------
//
//   printStates    Debug Function.  Dump the fully constructed state transition table.
//
//-----------------------------------------------------------------------------
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
فراغ RBBITableBuilder::print_حالتون() {
    صحيح     c;    // input "character"
    صحيح     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    لكل (c=0; c<fRB->f_سيٽ_Builder->get_انگ_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    لكل (c=0; c<fRB->f_سيٽ_Builder->get_انگ_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    لكل (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->f_ٽيگ_فهرست_Idx);
        لكل (c=0; c<fRB->f_سيٽ_Builder->get_انگ_Char_Categories(); c++) {
            RBBIDebugPrintf(" %3d", sd->fDtran->elementAti(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#جيڪڏهن_پورو


//-----------------------------------------------------------------------------
//
//   printSafeTable    Debug Function.  Dump the fully constructed safe table.
//
//-----------------------------------------------------------------------------
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
فراغ RBBITableBuilder::print_ابتو_جدول() {
    صحيح     c;    // input "character"
    صحيح     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    إذا (f_محفوظ_جدول == خالي_اشارو) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        أرجع;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    لكل (c=0; c<fRB->f_سيٽ_Builder->get_انگ_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    لكل (c=0; c<fRB->f_سيٽ_Builder->get_انگ_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    لكل (n=0; n<f_محفوظ_جدول->size(); n++) {
        UnicodeString *صف_String = (UnicodeString *)f_محفوظ_جدول->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        لكل (c=0; c<fRB->f_سيٽ_Builder->get_انگ_Char_Categories(); c++) {
            RBBIDebugPrintf(" %2d", صف_String->charAt(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#جيڪڏهن_پورو



//-----------------------------------------------------------------------------
//
//   printRuleStatusTable    Debug Function.  Dump the common rule status table
//
//-----------------------------------------------------------------------------
#جيڪڏهن_وصف_ٿيل RBBI_DEBUG
فراغ RBBITableBuilder::print_قاعدو_حالت_جدول() {
    int32_t  this_رڪارڊ = 0;
    int32_t  ايندڙ_رڪارڊ = 0;
    صحيح      i;
    UVector  *tbl = fRB->f_قاعدو_حالت_Vals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    طالما (ايندڙ_رڪارڊ < tbl->size()) {
        this_رڪارڊ = ايندڙ_رڪارڊ;
        ايندڙ_رڪارڊ = this_رڪارڊ + tbl->elementAti(this_رڪارڊ) + 1;
        RBBIDebugPrintf("%4d   ", this_رڪارڊ);
        لكل (i=this_رڪارڊ+1; i<ايندڙ_رڪارڊ; i++) {
            RBBIDebugPrintf("  %5d", tbl->elementAti(i));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#جيڪڏهن_پورو


//-----------------------------------------------------------------------------
//
//   RBBIStateDescriptor     Methods.  This is a very struct-like class
//                           Most access is directly to the fields.
//
//-----------------------------------------------------------------------------

RBBIStateDescriptor::RBBIStateDescriptor(صحيح آخري_داخلا_نشان, UError_ڪوڊ *f_حالت) {
    fMarked    = خطأ;
    fAccepting = 0;
    fLookAhead = 0;
    f_ٽيگ_فهرست_Idx   = 0;
    f_ٽيگ_Vals   = خالي_اشارو;
    fPositions = خالي_اشارو;
    fDtran     = خالي_اشارو;

    fDtran     = جديد UVector32(آخري_داخلا_نشان+1, *f_حالت);
    إذا (U_FAILURE(*f_حالت)) {
        أرجع;
    }
    إذا (fDtran == خالي_اشارو) {
        *f_حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    fDtran->سيٽ_سائيز(آخري_داخلا_نشان+1);    // fDtran needs to be pre-sized.
                                           //   It is indexed by input symbols, and will
                                           //   hold  the next state number for each
                                           //   symbol.
}


RBBIStateDescriptor::~RBBIStateDescriptor() {
    ميٽيو       fPositions;
    ميٽيو       fDtran;
    ميٽيو       f_ٽيگ_Vals;
    fPositions = خالي_اشارو;
    fDtran     = خالي_اشارو;
    f_ٽيگ_Vals   = خالي_اشارو;
}

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* #if !UCONFIG_NO_BREAK_ITERATION */
