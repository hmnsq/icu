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

#অন্তর্ভাব <functional>

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "rbbitblb.h"
#অন্তর্ভাব "rbbirb.h"
#অন্তর্ভাব "rbbiscan.h"
#অন্তর্ভাব "rbbisetb.h"
#অন্তর্ভাব "rbbidata.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "uvectr32.h"
#অন্তর্ভাব "cmemory.h"

U_NAMESPACE_BEGIN

নামাকাশ {

// Given the `RBBITableBuilder::fDStates` vector of `RBBIStateDescriptor`s, returns
// true if a state for which `isSink` returns true is reachable from state `source` by following
// transitions without going through any state for which `excludedState` returns true.
// Note that “going through” is distinct from “reaching”, and means entering and then leaving: A
// state can be both a sink and excluded, and can still be reachable.
তার্কিক reachableByTransitions(ধ্রুব UVector &states, ধ্রুব int32_t source,
                            ধ্রুব std::function<তার্কিক(int32_t)> isSink,
                            ধ্রুব std::function<তার্কিক(int32_t)> excludedState, UErrorCode &status) {
    UStack boundary(status);
    {
        UVector32 &transitionsFromSource =
            *static_cast<RBBIStateDescriptor *>(states.elementAt(source))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        প্রতি (int32_t প্রতীক = 0; প্রতীক < transitionsFromSource.size(); ++প্রতীক) {
            ধ্রুব int32_t state = transitionsFromSource.elementAti(প্রতীক);
            যদি (state != 0) {
                boundary.push(state, status);
            }
        }
    }
    ধ্রুব auto visited = prv::make_unique<তার্কিক[]>(states.size(), status);
    যাবত্ (U_SUCCESS(status) && !boundary.empty()) {
        ধ্রুব int32_t s = boundary.popi();
        যদি (isSink(s)) {
            প্রত্যাগম সত্য;
        }
        যদি (excludedState(s) || visited[s]) {
            অনুবর্ত;
        }
        visited[s] = সত্য;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(states.elementAt(s))->fDtran;
        প্রতি (int32_t প্রতীক = 0; প্রতীক < transitions.size(); ++প্রতীক) {
            ধ্রুব int32_t t = transitions.elementAti(প্রতীক);
            যদি (t != 0 && !visited[t]) {
                boundary.push(t, status);
            }
        }
    }
    প্রত্যাগম অসত্য;
}

} // namespace

ধ্রুব int32_t kMaxStateFor8BitsTable = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **মূল_গ্রন্থি, UErrorCode &status) :
        fRB(rb),
        f_বৃক্ষ(*মূল_গ্রন্থি),
        fStatus(&status),
        fDStates(nullptr),
        fSafeTable(nullptr) {
    যদি (U_FAILURE(status)) {
        প্রত্যাগম;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = নূতন UVector(status);
    যদি (U_SUCCESS(status) && fDStates == nullptr ) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    পূর্ণাঙ্ক i;
    প্রতি (i=0; i<fDStates->size(); i++) {
        delete static_cast<RBBIStateDescriptor*>(fDStates->elementAt(i));
    }
    delete fDStates;
    delete fSafeTable;
    delete fLookAheadRuleMap;
}


//-----------------------------------------------------------------------------
//
//   RBBITableBuilder::buildForwardTable  -  This is the main function for building
//                               the DFA state transition table from the RBBI rules parse tree.
//
//-----------------------------------------------------------------------------
শূন্য  RBBITableBuilder::buildForwardTable() {

    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    যদি (f_বৃক্ষ==nullptr) {
        প্রত্যাগম;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_বৃক্ষ = f_বৃক্ষ->flattenVariables(*fStatus, 0);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
#ifdef RBBI_DEBUG
    যদি (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_বৃক্ষ(f_বৃক্ষ, সত্য);
    }
#endif

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = নূতন RBBINode(RBBINode::opCat, *fStatus);
    // Exit if memory allocation failed.
    যদি (cn == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    যদি (U_FAILURE(*fStatus)) {
        delete cn;
        প্রত্যাগম;
    }
    cn->f_বাম_Child = f_বৃক্ষ;
    f_বৃক্ষ->fParent = cn;
    RBBINode *end_Marker_গ্রন্থি = cn->f_ডান_Child = নূতন RBBINode(RBBINode::endMark, *fStatus);
    // Delete and exit if memory allocation failed.
    যদি (cn->f_ডান_Child == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    যদি (U_FAILURE(*fStatus)) {
        delete cn;
        প্রত্যাগম;
    }
    cn->f_ডান_Child->fParent = cn;
    f_বৃক্ষ = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_বৃক্ষ->flattenSets(*fStatus, 0);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
#ifdef RBBI_DEBUG
    যদি (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_বৃক্ষ(f_বৃক্ষ, সত্য);
    }
#endif


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_বৃক্ষ);
    calcFirstPos(f_বৃক্ষ);
    calcLastPos(f_বৃক্ষ);
    calcFollowPos(f_বৃক্ষ);
    যদি (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_বৃক্ষ);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    যদি (fRB->fChainRules) {
        calcChainedFollowPos(f_বৃক্ষ, end_Marker_গ্রন্থি);
    }

    //
    // Build the DFA state transition tables.
    //
    buildStateTable();
    mapLookAheadRules();
    flagAcceptingStates();
    flagLookAheadStates();
    flagTaggedStates();

    //
    // Update the global table of rule status {tag} values
    // The rule builder has a global vector of status values that are common
    //    for all tables.  Merge the ones from this table into the global set.
    //
    mergeRuleStatusVals();
}



//-----------------------------------------------------------------------------
//
//   calcNullable.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
শূন্য RBBITableBuilder::calcNullable(RBBINode *n) {
    যদি (n == nullptr) {
        প্রত্যাগম;
    }
    যদি (n->fType == RBBINode::setRef ||
        n->fType == RBBINode::endMark ) {
        // These are non-empty leaf node types.
        n->fNullable = অসত্য;
        প্রত্যাগম;
    }

    যদি (n->fType == RBBINode::lookAhead || n->fType == RBBINode::tag) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = সত্য;
        প্রত্যাগম;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_বাম_Child);
    calcNullable(n->f_ডান_Child);

    // Apply functions from table 3.40 in Aho
    যদি (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_বাম_Child->fNullable || n->f_ডান_Child->fNullable;
    }
    অন্যথা যদি (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_বাম_Child->fNullable && n->f_ডান_Child->fNullable;
    }
    অন্যথা যদি (n->fType == RBBINode::opStar || n->fType == RBBINode::opQuestion) {
        n->fNullable = সত্য;
    }
    অন্যথা {
        n->fNullable = অসত্য;
    }
}




//-----------------------------------------------------------------------------
//
//   calcFirstPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
শূন্য RBBITableBuilder::calcFirstPos(RBBINode *n) {
    যদি (n == nullptr) {
        প্রত্যাগম;
    }
    যদি (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fFirstPosSet->addElement(n, *fStatus);
        প্রত্যাগম;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calcFirstPos(n->f_বাম_Child);
    calcFirstPos(n->f_ডান_Child);

    // Apply functions from table 3.40 in Aho
    যদি (n->fType == RBBINode::opOr) {
        setAdd(n->fFirstPosSet, n->f_বাম_Child->fFirstPosSet);
        setAdd(n->fFirstPosSet, n->f_ডান_Child->fFirstPosSet);
    }
    অন্যথা যদি (n->fType == RBBINode::opCat) {
        setAdd(n->fFirstPosSet, n->f_বাম_Child->fFirstPosSet);
        যদি (n->f_বাম_Child->fNullable) {
            setAdd(n->fFirstPosSet, n->f_ডান_Child->fFirstPosSet);
        }
    }
    অন্যথা যদি (n->fType == RBBINode::opStar ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fFirstPosSet, n->f_বাম_Child->fFirstPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
শূন্য RBBITableBuilder::calcLastPos(RBBINode *n) {
    যদি (n == nullptr) {
        প্রত্যাগম;
    }
    যদি (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fLastPosSet->addElement(n, *fStatus);
        প্রত্যাগম;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calcLastPos(n->f_বাম_Child);
    calcLastPos(n->f_ডান_Child);

    // Apply functions from table 3.40 in Aho
    যদি (n->fType == RBBINode::opOr) {
        setAdd(n->fLastPosSet, n->f_বাম_Child->fLastPosSet);
        setAdd(n->fLastPosSet, n->f_ডান_Child->fLastPosSet);
    }
    অন্যথা যদি (n->fType == RBBINode::opCat) {
        setAdd(n->fLastPosSet, n->f_ডান_Child->fLastPosSet);
        যদি (n->f_ডান_Child->fNullable) {
            setAdd(n->fLastPosSet, n->f_বাম_Child->fLastPosSet);
        }
    }
    অন্যথা যদি (n->fType == RBBINode::opStar     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fLastPosSet, n->f_বাম_Child->fLastPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
শূন্য RBBITableBuilder::calcFollowPos(RBBINode *n) {
    যদি (n == nullptr ||
        n->fType == RBBINode::leafChar ||
        n->fType == RBBINode::endMark) {
        প্রত্যাগম;
    }

    calcFollowPos(n->f_বাম_Child);
    calcFollowPos(n->f_ডান_Child);

    // Aho rule #1
    যদি (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *Last_Pos_Of_বাম_Child = n->f_বাম_Child->fLastPosSet;

        প্রতি (ix = 0; ix < static_cast<uint32_t>(Last_Pos_Of_বাম_Child->size()); ix++) {
            i = static_cast<RBBINode*>(Last_Pos_Of_বাম_Child->elementAt(ix));
            setAdd(i->fFollowPos, n->f_ডান_Child->fFirstPosSet);
        }
    }

    // Aho rule #2
    যদি (n->fType == RBBINode::opStar ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        প্রতি (ix = 0; ix < static_cast<uint32_t>(n->fLastPosSet->size()); ix++) {
            i = static_cast<RBBINode*>(n->fLastPosSet->elementAt(ix));
            setAdd(i->fFollowPos, n->fFirstPosSet);
        }
    }



}

//-----------------------------------------------------------------------------
//
//    addRuleRootNodes    Recursively walk a parse tree, adding all nodes flagged
//                        as roots of a rule to a destination vector.
//
//-----------------------------------------------------------------------------
শূন্য RBBITableBuilder::add_Rule_মূল_Nodes(UVector *dest, RBBINode *গ্রন্থি) {
    যদি (গ্রন্থি == nullptr || U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    U_ASSERT(!dest->hasDeleter());
    যদি (গ্রন্থি->f_Rule_মূল) {
        dest->addElement(গ্রন্থি, *fStatus);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        প্রত্যাগম;
    }
    add_Rule_মূল_Nodes(dest, গ্রন্থি->f_বাম_Child);
    add_Rule_মূল_Nodes(dest, গ্রন্থি->f_ডান_Child);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
শূন্য RBBITableBuilder::calcChainedFollowPos(RBBINode *বৃক্ষ, RBBINode *end_Mark_গ্রন্থি) {

    UVector         leafNodes(*fStatus);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }

    // get a list all leaf nodes
    বৃক্ষ->findNodes(&leafNodes, RBBINode::leafChar, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector rule_মূল_Nodes(*fStatus);
    add_Rule_মূল_Nodes(&rule_মূল_Nodes, বৃক্ষ);

    UVector matchStartNodes(*fStatus);
    প্রতি (পূর্ণাঙ্ক j=0; j<rule_মূল_Nodes.size(); ++j) {
        RBBINode *গ্রন্থি = static_cast<RBBINode *>(rule_মূল_Nodes.elementAt(j));
        যদি (গ্রন্থি->fChainIn) {
            setAdd(&matchStartNodes, গ্রন্থি->fFirstPosSet);
        }
    }
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }

    int32_t  end_গ্রন্থি_Ix;
    int32_t  start_গ্রন্থি_Ix;

    প্রতি (end_গ্রন্থি_Ix=0; end_গ্রন্থি_Ix<leafNodes.size(); end_গ্রন্থি_Ix++) {
        RBBINode* end_গ্রন্থি = static_cast<RBBINode*>(leafNodes.elementAt(end_গ্রন্থি_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        যদি (!end_গ্রন্থি->fFollowPos->contains(end_Mark_গ্রন্থি)) {
            অনুবর্ত;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *start_গ্রন্থি;
        প্রতি (start_গ্রন্থি_Ix = 0; start_গ্রন্থি_Ix<matchStartNodes.size(); start_গ্রন্থি_Ix++) {
            start_গ্রন্থি = static_cast<RBBINode*>(matchStartNodes.elementAt(start_গ্রন্থি_Ix));
            যদি (start_গ্রন্থি->fType != RBBINode::leafChar) {
                অনুবর্ত;
            }

            যদি (end_গ্রন্থি->fVal == start_গ্রন্থি->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                setAdd(end_গ্রন্থি->fFollowPos, start_গ্রন্থি->fFollowPos);
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
শূন্য RBBITableBuilder::bofFixup() {

    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
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
    RBBINode  *bof_গ্রন্থি = f_বৃক্ষ->f_বাম_Child->f_বাম_Child;
    U_ASSERT(bof_গ্রন্থি->fType == RBBINode::leafChar);
    U_ASSERT(bof_গ্রন্থি->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *matchStartNodes = f_বৃক্ষ->f_বাম_Child->f_ডান_Child->fFirstPosSet;

    RBBINode *start_গ্রন্থি;
    পূর্ণাঙ্ক       start_গ্রন্থি_Ix;
    প্রতি (start_গ্রন্থি_Ix = 0; start_গ্রন্থি_Ix<matchStartNodes->size(); start_গ্রন্থি_Ix++) {
        start_গ্রন্থি = static_cast<RBBINode*>(matchStartNodes->elementAt(start_গ্রন্থি_Ix));
        যদি (start_গ্রন্থি->fType != RBBINode::leafChar) {
            অনুবর্ত;
        }

        যদি (start_গ্রন্থি->fVal == bof_গ্রন্থি->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            setAdd(bof_গ্রন্থি->fFollowPos, start_গ্রন্থি->fFollowPos);
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
শূন্য RBBITableBuilder::buildStateTable() {
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = nullptr;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    পূর্ণাঙ্ক      last_প্রবেশ_প্রতীক = fRB->fSetBuilder->get_অঙ্ক_Char_Categories() - 1;
    failState = নূতন RBBIStateDescriptor(last_প্রবেশ_প্রতীক, fStatus);
    যদি (failState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        গচ্ছ ExitBuildSTdeleteall;
    }
    failState->fPositions = নূতন UVector(*fStatus);
    যদি (failState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    যদি (failState->fPositions == nullptr || U_FAILURE(*fStatus)) {
        গচ্ছ ExitBuildSTdeleteall;
    }
    fDStates->addElement(failState, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        গচ্ছ ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = নূতন RBBIStateDescriptor(last_প্রবেশ_প্রতীক, fStatus);
    যদি (initialState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    যদি (U_FAILURE(*fStatus)) {
        গচ্ছ ExitBuildSTdeleteall;
    }
    initialState->fPositions = নূতন UVector(*fStatus);
    যদি (initialState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    যদি (U_FAILURE(*fStatus)) {
        গচ্ছ ExitBuildSTdeleteall;
    }
    setAdd(initialState->fPositions, f_বৃক্ষ->fFirstPosSet);
    fDStates->addElement(initialState, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        গচ্ছ ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    প্রতি (;;) {
        RBBIStateDescriptor *T = nullptr;
        int32_t              tx;
        প্রতি (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *temp;
            temp = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            যদি (temp->fMarked == অসত্য) {
                T = temp;
                ভঙ্গ;
            }
        }
        যদি (T == nullptr) {
            ভঙ্গ;
        }

        // mark T;
        T->fMarked = সত্য;

        // for each input symbol a do begin
        int32_t  a;
        প্রতি (a = 1; a<=last_প্রবেশ_প্রতীক; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = nullptr;
            RBBINode   *p;
            int32_t     px;
            প্রতি (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                যদি ((p->fType == RBBINode::leafChar) &&  (p->fVal == a)) {
                    যদি (U == nullptr) {
                        U = নূতন UVector(*fStatus);
                        যদি (U == nullptr) {
                        	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                        	গচ্ছ ExitBuildSTdeleteall;
                        }
                    }
                    setAdd(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = অসত্য;
            যদি (U != nullptr) {
                U_ASSERT(U->size() > 0);
                পূর্ণাঙ্ক  ix;
                প্রতি (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    যদি (setEquals(U, temp2->fPositions)) {
                        delete U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = সত্য;
                        ভঙ্গ;
                    }
                }

                // Add U as an unmarked state to Dstates
                যদি (!UinDstates)
                {
                    RBBIStateDescriptor *newState = নূতন RBBIStateDescriptor(last_প্রবেশ_প্রতীক, fStatus);
                    যদি (newState == nullptr) {
                    	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                    }
                    যদি (U_FAILURE(*fStatus)) {
                        গচ্ছ ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->addElement(newState, *fStatus);
                    যদি (U_FAILURE(*fStatus)) {
                        প্রত্যাগম;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->setElementAt(ux, a);
            }
        }
    }
    প্রত্যাগম;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    delete initialState;
    delete failState;
}


/**
 * mapLookAheadRules
 *
 */
শূন্য RBBITableBuilder::mapLookAheadRules() {
    fLookAheadRuleMap =  নূতন UVector32(fRB->fScanner->অঙ্ক_Rules() + 1, *fStatus);
    যদি (fLookAheadRuleMap == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    fLookAheadRuleMap->setSize(fRB->fScanner->অঙ্ক_Rules() + 1);

    প্রতি (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t laSlotForState = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        তার্কিক saw_Look_Ahead_গ্রন্থি = অসত্য;
        প্রতি (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *গ্রন্থি = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            যদি (গ্রন্থি->fType != RBBINode::গ্রন্থি_Type::lookAhead) {
                অনুবর্ত;
            }
            saw_Look_Ahead_গ্রন্থি = সত্য;
            int32_t rule_অঙ্ক = গ্রন্থি->fVal;     // Set when rule was originally parsed.
            U_ASSERT(rule_অঙ্ক < fLookAheadRuleMap->size());
            U_ASSERT(rule_অঙ্ক > 0);
            int32_t laSlot = fLookAheadRuleMap->elementAti(rule_অঙ্ক);
            যদি (laSlot != 0) {
                যদি (laSlotForState == 0) {
                    laSlotForState = laSlot;
                } অন্যথা {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(laSlot == laSlotForState);
                }
            }
        }
        যদি (!saw_Look_Ahead_গ্রন্থি) {
            অনুবর্ত;
        }

        যদি (laSlotForState == 0) {
            laSlotForState = ++fLASlotsInUse;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        প্রতি (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *গ্রন্থি = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            যদি (গ্রন্থি->fType != RBBINode::গ্রন্থি_Type::lookAhead) {
                অনুবর্ত;
            }
            int32_t rule_অঙ্ক = গ্রন্থি->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = fLookAheadRuleMap->elementAti(rule_অঙ্ক);
            (শূন্য)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == laSlotForState);
            fLookAheadRuleMap->setElementAt(laSlotForState, rule_অঙ্ক);
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
শূন্য     RBBITableBuilder::flagAcceptingStates() {
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    UVector     endMarkerNodes(*fStatus);
    RBBINode    *endMarker;
    int32_t     i;
    int32_t     n;

    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }

    f_বৃক্ষ->findNodes(&endMarkerNodes, RBBINode::endMark, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }

    প্রতি (i=0; i<endMarkerNodes.size(); i++) {
        endMarker = static_cast<RBBINode*>(endMarkerNodes.elementAt(i));
        প্রতি (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            যদি (sd->fPositions->indexOf(endMarker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                যদি (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = fLookAheadRuleMap->elementAti(endMarker->fVal);
                    যদি (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                যদি (sd->fAccepting==ACCEPTING_UNCONDITIONAL && endMarker->fVal != 0) {
                    // Both lookahead and non-lookahead accepting for this state.
                    // Favor the look-ahead, because a look-ahead match needs to
                    // immediately stop the run-time engine. First match, not longest.
                    sd->fAccepting = fLookAheadRuleMap->elementAti(endMarker->fVal);
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
শূন্য     RBBITableBuilder::flagLookAheadStates() {
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    UVector     lookAheadNodes(*fStatus);
    RBBINode    *look_Ahead_গ্রন্থি;
    int32_t     i;
    int32_t     n;

    f_বৃক্ষ->findNodes(&lookAheadNodes, RBBINode::lookAhead, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    প্রতি (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_গ্রন্থি = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_গ্রন্থি->fType == RBBINode::গ্রন্থি_Type::lookAhead);

        প্রতি (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->indexOf(look_Ahead_গ্রন্থি);
            যদি (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_গ্রন্থি == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookaheadSlot = fLookAheadRuleMap->elementAti(look_Ahead_গ্রন্থি->fVal);
                U_ASSERT(sd->fLookAhead == 0 || sd->fLookAhead == lookaheadSlot);
                // if (sd->fLookAhead != 0 && sd->fLookAhead != lookaheadSlot) {
                //     printf("%s:%d Bingo. sd->fLookAhead:%d   lookaheadSlot:%d\n",
                //            __FILE__, __LINE__, sd->fLookAhead, lookaheadSlot);
                // }
                sd->fLookAhead = lookaheadSlot;
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//    flagTaggedStates
//
//-----------------------------------------------------------------------------
শূন্য     RBBITableBuilder::flagTaggedStates() {
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    UVector     tagNodes(*fStatus);
    RBBINode    *tag_গ্রন্থি;
    int32_t     i;
    int32_t     n;

    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    f_বৃক্ষ->findNodes(&tagNodes, RBBINode::tag, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    প্রতি (i=0; i<tagNodes.size(); i++) {                   // For each tag node t (all of 'em)
        tag_গ্রন্থি = static_cast<RBBINode*>(tagNodes.elementAt(i));

        প্রতি (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            যদি (sd->fPositions->indexOf(tag_গ্রন্থি) >= 0) {  //       if  s include the tag node t
                sortedAdd(&sd->fTagVals, tag_গ্রন্থি->fVal);
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
শূন্য  RBBITableBuilder::mergeRuleStatusVals() {
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
    পূর্ণাঙ্ক i;
    পূর্ণাঙ্ক n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    যদি (fRB->fRuleStatusVals->size() == 0) {
        fRB->fRuleStatusVals->addElement(1, *fStatus);  // Num of statuses in group
        fRB->fRuleStatusVals->addElement(static_cast<int32_t>(0), *fStatus); // and our single status of zero
    }

    //    For each state
    প্রতি (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_States_Tag_মানসমূহ = sd->fTagVals;
        যদি (this_States_Tag_মানসমূহ == nullptr) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->fTagsIdx = 0;
            অনুবর্ত;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->fTagsIdx = -1;
        int32_t  thisTagGroupStart = 0;   // indexes into the global rule status vals list
        int32_t  nextTagGroupStart = 0;

        // Loop runs once per group of tags in the global list
        যাবত্ (nextTagGroupStart < fRB->fRuleStatusVals->size()) {
            thisTagGroupStart = nextTagGroupStart;
            nextTagGroupStart += fRB->fRuleStatusVals->elementAti(thisTagGroupStart) + 1;
            যদি (this_States_Tag_মানসমূহ->size() != fRB->fRuleStatusVals->elementAti(thisTagGroupStart)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                অনুবর্ত;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            প্রতি (i=0; i<this_States_Tag_মানসমূহ->size(); i++) {
                যদি (this_States_Tag_মানসমূহ->elementAti(i) !=
                    fRB->fRuleStatusVals->elementAti(thisTagGroupStart + 1 + i) ) {
                    // Mismatch.
                    ভঙ্গ;
                }
            }

            যদি (i == this_States_Tag_মানসমূহ->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->fTagsIdx = thisTagGroupStart;
                ভঙ্গ;
            }
        }

        যদি (sd->fTagsIdx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->fTagsIdx = fRB->fRuleStatusVals->size();
            fRB->fRuleStatusVals->addElement(this_States_Tag_মানসমূহ->size(), *fStatus);
            প্রতি (i=0; i<this_States_Tag_মানসমূহ->size(); i++) {
                fRB->fRuleStatusVals->addElement(this_States_Tag_মানসমূহ->elementAti(i), *fStatus);
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

শূন্য RBBITableBuilder::minimizeStates() {
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }

#যদি UPRV_HAS_SANITIZER
    যদি (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *fStatus = U_REGEX_PATTERN_TOO_BIG;
        প্রত্যাগম;
    }
#endif
    সংরচনা StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t fTagsIdx;
        তার্কিক operator==(StateType ধ্রুব &other) ধ্রুব {
            প্রত্যাগম fAccepting == other.fAccepting &&
                   fLookAhead == other.fLookAhead &&
                   fTagsIdx == other.fTagsIdx;
        }
    };
    // We wrap `UVector`s in `unique_ptr``s throughout so we can move them,
    // including into `UVector`s (by releasing them from the `unique_ptr`
    // and having the enclosing `UVector` adopt them). 
    // Group the states by types (but we have no maps so this is verbose).
    // If there are no lookaheads and no tags, there are only two types
    // (accepting and non-accepting) in which case this is exactly step 1
    // of Algorithm 3.6.
    সংরচনা TypeToStates : UMemory {
        TypeToStates(ধ্রুব StateType &type, UErrorCode &status)
            : type(type), states(prv::make_unique<UVector>(status, status)) {}
        StateType type;
        prv::unique_ptr<UVector> states;
    };
    UVector initialPartition(*fStatus);
    initialPartition.setDeleter(
        [](শূন্য *p) { delete static_cast<TypeToStates *>(p); });
    প্রতি (int32_t i = 0; i < fDStates->size(); ++i) {
        ধ্রুব RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ধ্রুব StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.fTagsIdx};
        int32_t j = 0;
        প্রতি (; j < initialPartition.size(); ++j) {
            auto &[type_j, states] =
                *static_cast<TypeToStates *>(initialPartition.elementAt(j));
            যদি (type_j == type) {
                states->addElement(i, *fStatus);
                ভঙ্গ;
            }
        }
        যদি (j == initialPartition.size()) {
            auto newEntry = prv::make_unique<TypeToStates>(type, *fStatus, *fStatus);
            যদি (U_FAILURE(*fStatus)) {
                প্রত্যাগম;
            }
            newEntry->states->addElement(i, *fStatus);
            initialPartition.adoptElement(newEntry.release(), *fStatus);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    auto partition = prv::make_unique<UVector>(*fStatus, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    partition->setDeleter(
        [](শূন্য *p) { delete static_cast<UVector*>(p); });
    
    প্রতি (int32_t i = 0; i < initialPartition.size(); ++i) {
        partition->adoptElement(
            static_cast<TypeToStates*>(initialPartition.elementAt(i))->states.release(),
            *fStatus);
    }
    // Given the index of a state 𝑠 in `fDStates`, returns a UVector of integers
    // σ(𝑠) such that 𝑠.fDtran[i] (the index of the state reached by the
    // transition from 𝑠 on the character class with index i) is in Π[σ(𝑠)[i]].
    // We then have σ(𝑠) == σ(𝑡) if and only if “for all input symbols 𝑎, states
    // 𝑠 and 𝑡 have transitions on 𝑎 to states in the same group of Π” (which is
    // what defines the refinement of 𝐺 in Fig. 3.45).
    auto partition_signature = [&partition, এতত্](int32_t stateIndex) -> prv::unique_ptr<UVector> {
        ধ্রুব RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(stateIndex));
        auto result = prv::make_unique<UVector>(state.fDtran->size(), *fStatus, *fStatus);
        যদি (U_FAILURE(*fStatus)) {
            প্রত্যাগম nullptr;
        }
        প্রতি (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t toState = state.fDtran->elementAti(i);
            প্রতি (int32_t partIndex = 0; partIndex < partition->size(); ++partIndex) {
                ধ্রুব UVector &part = *static_cast<UVector *>(partition->elementAt(partIndex));
                যদি (part.contains(toState)) {
                    result->addElement(partIndex, *fStatus);
                    ভঙ্গ;
                }
            }
        }
        result->setComparer([](ধ্রুব UElement left, ধ্রুব UElement ডান) -> UBool {
            প্রত্যাগম left.integer == ডান.integer;
        });
        প্রত্যাগম result;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    প্রতি (;;) {
        // Π_new.
        auto partitionNew = prv::make_unique<UVector>(*fStatus, *fStatus);
        যদি (U_FAILURE(*fStatus)) {
            প্রত্যাগম;
        }
        partitionNew->setDeleter(
            [](শূন্য *p) { delete static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        তার্কিক refined = অসত্য;
        প্রতি (int32_t i = 0; i < partition->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ধ্রুব UVector &group = *static_cast<UVector*>(partition->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            সংরচনা SignatureToStates : UMemory {
                SignatureToStates(prv::unique_ptr<UVector> signature, UErrorCode &status)
                    : signature(std::move(signature)), states(prv::make_unique<UVector>(status, status)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> states;
            };
            UVector groupRefinement(*fStatus);
            groupRefinement.setDeleter([](শূন্য *p) { delete static_cast<SignatureToStates*>(p); });
            প্রতি (int32_t j = 0; j < group.size(); ++j) {
                // Index of a state in the group.
                ধ্রুব int32_t s = group.elementAti(j);
                auto signature = partition_signature(s);
                int32_t k = 0;
                প্রতি (; k < groupRefinement.size(); ++k) {
                    ধ্রুব auto &[subgroupSignature, subgroup] =
                        *static_cast<SignatureToStates*>(groupRefinement.elementAt(k));
                    যদি (*subgroupSignature == *signature) {
                        subgroup->addElement(s, *fStatus);
                        ভঙ্গ;
                    }
                }
                যদি (k == groupRefinement.size()) {
                    ধ্রুব auto newEntry =
                        নূতন SignatureToStates(std::move(signature), *fStatus);
                    যদি (U_FAILURE(*fStatus)) {
                        প্রত্যাগম;
                    }
                    যদি (newEntry == nullptr) {
                      *fStatus = U_MEMORY_ALLOCATION_ERROR;
                      প্রত্যাগম;
                    }
                    newEntry->states->addElement(s, *fStatus);
                    groupRefinement.adoptElement(newEntry, *fStatus);
                }
            }
            refined |= groupRefinement.size() > 1;
            প্রতি (int32_t j = 0; j < groupRefinement.size(); ++j) {
                auto &[_, subgroup] = *static_cast<SignatureToStates*>(
                    groupRefinement.elementAt(j));
                partitionNew->adoptElement(subgroup.release(), *fStatus);
            }
        }
        যদি (refined) {
            partition = std::move(partitionNew);
        } অন্যথা {
            ভঙ্গ;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    প্রতি (পূর্ণাঙ্ক i = 0; i < partition->size(); ++i) {
        UVector *ধ্রুব part_i = static_cast<UVector *>(partition->elementAt(i));
        যদি (part_i->contains(0)) {
            শূন্য *ধ্রুব part_0 = partition->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ধ্রুব auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 0);
            partition->setElementAt(part_0, i);
            partition->setDeleter(deleter);
        } অন্যথা যদি (part_i->contains(1)) {
            শূন্য *ধ্রুব part_1 = partition->elementAt(1);
            ধ্রুব auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 1);
            partition->setElementAt(part_1, i);
            partition->setDeleter(deleter);
        }
    }
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    auto oldStateToPart = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    প্রতি (পূর্ণাঙ্ক i = 0; i < partition->size(); ++i) {
        ধ্রুব UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        প্রতি (পূর্ণাঙ্ক j = 0; j < part.size(); ++j) {
            oldStateToPart[part.elementAti(j)] = i;
        }
    }
    auto ধ্রুব oldStates = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*fStatus, *fStatus).release();
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    প্রতি (পূর্ণাঙ্ক i = 0; i < partition->size(); ++i) {
        ধ্রুব UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        RBBIStateDescriptor *ধ্রুব state =
            static_cast<RBBIStateDescriptor*>(oldStates->elementAt(part.elementAti(0)));
        fDStates->addElement(state, *fStatus);
        প্রতি (পূর্ণাঙ্ক j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->setElementAt(oldStateToPart[state->fDtran->elementAti(j)], j);
        }
        oldStates->setElementAt(nullptr, part.elementAti(0));
    }
    oldStates->setDeleter([](শূন্য *p) { delete static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
শূন্য RBBITableBuilder::sortedAdd(UVector **vector, int32_t val) {
    int32_t i;

    যদি (*vector == nullptr) {
        *vector = নূতন UVector(*fStatus);
    }
    যদি (*vector == nullptr || U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    UVector *vec = *vector;
    int32_t  vSize = vec->size();
    প্রতি (i=0; i<vSize; i++) {
        int32_t valAtI = vec->elementAti(i);
        যদি (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            প্রত্যাগম;
        }
        যদি (valAtI > val) {
            ভঙ্গ;
        }
    }
    vec->ঢোকাও_Element_At(val, i, *fStatus);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
শূন্য RBBITableBuilder::setAdd(UVector *dest, UVector *source) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!source->hasDeleter());
    int32_t destOriginalSize = dest->size();
    int32_t sourceSize       = source->size();
    int32_t di           = 0;
    Maybe_স্তূপ_Array<শূন্য *, 16> destArray, sourceArray;  // Handle small cases without malloc
    শূন্য **destPtr, **sourcePtr;
    শূন্য **destLim, **sourceLim;

    যদি (destOriginalSize > destArray.getCapacity()) {
        যদি (destArray.resize(destOriginalSize) == nullptr) {
            প্রত্যাগম;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + destOriginalSize;  // destArray.getArrayLimit()?

    যদি (sourceSize > sourceArray.getCapacity()) {
        যদি (sourceArray.resize(sourceSize) == nullptr) {
            প্রত্যাগম;
        }
    }
    sourcePtr = sourceArray.getAlias();
    sourceLim = sourcePtr + sourceSize;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (শূন্য) dest->toArray(destPtr);
    (শূন্য) source->toArray(sourcePtr);

    dest->setSize(sourceSize+destOriginalSize, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }

    যাবত্ (sourcePtr < sourceLim && destPtr < destLim) {
        যদি (*destPtr == *sourcePtr) {
            dest->setElementAt(*sourcePtr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        অন্যথা যদি (uprv_memcmp(destPtr, sourcePtr, পরিমাণ(শূন্য *)) < 0) {
            dest->setElementAt(*destPtr++, di++);
        }
        অন্যথা { /* *sourcePtr < *destPtr */
            dest->setElementAt(*sourcePtr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    যাবত্ (destPtr < destLim) {
        dest->setElementAt(*destPtr++, di++);
    }
    যাবত্ (sourcePtr < sourceLim) {
        dest->setElementAt(*sourcePtr++, di++);
    }

    dest->setSize(di, *fStatus);
}



//-----------------------------------------------------------------------------
//
//  setEqual    Set operation on UVector.
//              Compare for equality.
//              Elements must be sorted.
//
//-----------------------------------------------------------------------------
UBool RBBITableBuilder::setEquals(UVector *a, UVector *b) {
    প্রত্যাগম a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
শূন্য RBBITableBuilder::printPosSets(RBBINode *n) {
    যদি (n==nullptr) {
        প্রত্যাগম;
    }
    printf("\n");
    RBBINode::print_গ্রন্থি_Header();
    RBBINode::print_গ্রন্থি(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    printSet(n->fFirstPosSet);

    RBBIDebugPrintf("         lastpos:   ");
    printSet(n->fLastPosSet);

    RBBIDebugPrintf("         followpos: ");
    printSet(n->fFollowPos);

    printPosSets(n->f_বাম_Child);
    printPosSets(n->f_ডান_Child);
}
#endif

//
//    findDuplCharClassFrom()
//
তার্কিক RBBITableBuilder::findDuplCharClassFrom(IntPair *categories) {
    int32_t অঙ্ক_States = fDStates->size();
    int32_t অঙ্ক_Cols = fRB->fSetBuilder->get_অঙ্ক_Char_Categories();

    প্রতি (; categories->first < অঙ্ক_Cols-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        পূর্ণাঙ্ক limitSecond = categories->first < fRB->fSetBuilder->getDictCategoriesStart() ?
            fRB->fSetBuilder->getDictCategoriesStart() : অঙ্ক_Cols;
        প্রতি (categories->second=categories->first+1; categories->second < limitSecond; categories->second++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t table_base = 0;
            uint16_t table_dupl = 1;
            প্রতি (int32_t state=0; state<অঙ্ক_States; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                table_base = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                table_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->second));
                যদি (table_base != table_dupl) {
                    ভঙ্গ;
                }
            }
            যদি (table_base == table_dupl) {
                প্রত্যাগম সত্য;
            }
        }
    }
    প্রত্যাগম অসত্য;
}


//
//    removeColumn()
//
শূন্য RBBITableBuilder::removeColumn(int32_t column) {
    int32_t অঙ্ক_States = fDStates->size();
    প্রতি (int32_t state=0; state<অঙ্ক_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(column < sd->fDtran->size());
        sd->fDtran->removeElementAt(column);
    }
}


তার্কিক RBBITableBuilder::findDuplicateSafeState(IntPair *states) {
    int32_t অঙ্ক_States = fSafeTable->size();

    প্রতি (; states->first<অঙ্ক_States-1; states->first++) {
        UnicodeString *firstRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->first));
        প্রতি (states->second=states->first+1; states->second<অঙ্ক_States; states->second++) {
            UnicodeString *duplRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->second));
            তার্কিক rowsMatch = সত্য;
            int32_t অঙ্ক_Cols = firstRow->length();
            প্রতি (int32_t col=0; col < অঙ্ক_Cols; ++col) {
                int32_t firstVal = firstRow->charAt(col);
                int32_t duplVal = duplRow->charAt(col);
                যদি (!((firstVal == duplVal) ||
                        ((firstVal == states->first || firstVal == states->second) &&
                        (duplVal  == states->first || duplVal  == states->second)))) {
                    rowsMatch = অসত্য;
                    ভঙ্গ;
                }
            }
            যদি (rowsMatch) {
                প্রত্যাগম সত্য;
            }
        }
    }
    প্রত্যাগম অসত্য;
}

শূন্য RBBITableBuilder::removeSafeState(IntPair duplStates) {
    ধ্রুব int32_t keepState = duplStates.first;
    ধ্রুব int32_t duplState = duplStates.second;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < fSafeTable->size());

    fSafeTable->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t অঙ্ক_States = fSafeTable->size();
    প্রতি (int32_t state=0; state<অঙ্ক_States; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        int32_t অঙ্ক_Cols = sd->length();
        প্রতি (int32_t col=0; col<অঙ্ক_Cols; col++) {
            int32_t existingVal = sd->charAt(col);
            int32_t newVal = existingVal;
            যদি (existingVal == duplState) {
                newVal = keepState;
            } অন্যথা যদি (existingVal > duplState) {
                newVal = existingVal - 1;
            }
            sd->setCharAt(col, static_cast<char16_t>(newVal));
        }
    }
}


/*
 * RemoveDuplicateStates
 */
int32_t RBBITableBuilder::removeDuplicateStates() {
    ধ্রুব int32_t old_State_গুনতি = fDStates->size();
    minimizeStates();
    প্রত্যাগম fDStates->size() - old_State_গুনতি;
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
শূন্য RBBITableBuilder::minimizeLookaheads() {
    যদি (fLASlotsInUse == ACCEPTING_UNCONDITIONAL) {
        প্রত্যাগম;
    }
    ধ্রুব int32_t lookahead_গুনতি = fLASlotsInUse - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ধ্রুব int32_t firstLookahead = ACCEPTING_UNCONDITIONAL + 1;
    ধ্রুব auto stateDescriptor = [এতত্](ধ্রুব int32_t state) -> RBBIStateDescriptor & {
        প্রত্যাগম *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ধ্রুব auto lookaheadReachability =
        prv::make_unique<তার্কিক[]>(lookahead_গুনতি * lookahead_গুনতি, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    প্রতি (int32_t l = firstLookahead; l <= fLASlotsInUse; ++l) {
        প্রতি (int32_t k = firstLookahead; k <= fLASlotsInUse; ++k) {
            যদি (k != l) {
                প্রতি (int32_t source = 1; source < fDStates->size(); ++source) {
                    যদি (static_cast<int32_t>(stateDescriptor(source).fLookAhead) != l) {
                        অনুবর্ত;
                    }
                    যদি (reachableByTransitions(
                            *fDStates, source, /*isSink=*/
                            [&](ধ্রুব int32_t state) {
                                প্রত্যাগম static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](ধ্রুব int32_t state) {
                                প্রত্যাগম static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *fStatus)) {
                        lookaheadReachability[(l - firstLookahead) * lookahead_গুনতি + k -
                                              firstLookahead] = সত্য;
                    }
                }
            }
        }
    }

    ধ্রুব auto colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_গুনতি, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    প্রতি (int32_t chromatic_সংখ্যা = 1;; ++chromatic_সংখ্যা) {
        uprv_memset(colours.get(), 0, পরিমাণ(int32_t) * lookahead_গুনতি);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        প্রতি (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            প্রতি (পূর্ণাঙ্ক source = 0; source < lookahead_গুনতি; ++source) {
                প্রতি (পূর্ণাঙ্ক sink = 0; sink < lookahead_গুনতি; ++sink) {
                    যদি (lookaheadReachability[source * lookahead_গুনতি + sink] &&
                        colours[source] == colours[sink]) {
                        গচ্ছ nextColouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            প্রতি (পূর্ণাঙ্ক i = 0; i < fDStates->size(); ++i) {
                যদি (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        firstLookahead +
                        colours[stateDescriptor(i).fAccepting - firstLookahead];
                }
                যদি (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - firstLookahead];
                }
            }
            fLASlotsInUse = ACCEPTING_UNCONDITIONAL + chromatic_সংখ্যা;
            প্রত্যাগম;

          nextColouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            প্রতি (int32_t i = 0; i < lookahead_গুনতি - 1 && colours[i] == chromatic_সংখ্যা;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            যদি (colours[lookahead_গুনতি - 1] == chromatic_সংখ্যা) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                ভঙ্গ;
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
int32_t  RBBITableBuilder::getTableSize() ধ্রুব {
    int32_t    size = 0;
    int32_t    অঙ্ক_Rows;
    int32_t    অঙ্ক_Cols;
    int32_t    rowSize;

    যদি (f_বৃক্ষ == nullptr) {
        প্রত্যাগম 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    অঙ্ক_Rows = fDStates->size();
    অঙ্ক_Cols = fRB->fSetBuilder->get_অঙ্ক_Char_Categories();

    যদি (use8BitsForTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + পরিমাণ(int8_t)*অঙ্ক_Cols;
    } অন্যথা {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + পরিমাণ(int16_t)*অঙ্ক_Cols;
    }
    size   += অঙ্ক_Rows * rowSize;
    প্রত্যাগম size;
}

তার্কিক RBBITableBuilder::use8BitsForTable() ধ্রুব {
    প্রত্যাগম fDStates->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
শূন্য RBBITableBuilder::exportTable(শূন্য *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    পূর্ণাঙ্ক                col;

    যদি (U_FAILURE(*fStatus) || f_বৃক্ষ == nullptr) {
        প্রত্যাগম;
    }

    int32_t cat_গুনতি = fRB->fSetBuilder->get_অঙ্ক_Char_Categories();
    যদি (cat_গুনতি > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        প্রত্যাগম;
    }

    table->f_অঙ্ক_States = fDStates->size();
    table->fDictCategoriesStart = fRB->fSetBuilder->getDictCategoriesStart();
    table->fLookAheadResultsSize = fLASlotsInUse == ACCEPTING_UNCONDITIONAL ? 0 : fLASlotsInUse + 1;
    table->fFlags     = 0;
    যদি (use8BitsForTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + পরিমাণ(uint8_t) * cat_গুনতি;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } অন্যথা {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + পরিমাণ(int16_t) * cat_গুনতি;
    }
    যদি (fRB->fLookAheadHardBreak) {
        table->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    প্রতি (state=0; state<table->f_অঙ্ক_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        যদি (use8BitsForTable()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 255);
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->fTagsIdx   = sd->fTagsIdx;
            প্রতি (col=0; col<cat_গুনতি; col++) {
                U_ASSERT (sd->fDtran->elementAti(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = sd->fDtran->elementAti(col);
            }
        } অন্যথা {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 0xffff);
            row->r16.fAccepting = sd->fAccepting;
            row->r16.fLookAhead = sd->fLookAhead;
            row->r16.fTagsIdx   = sd->fTagsIdx;
            প্রতি (col=0; col<cat_গুনতি; col++) {
                row->r16.fNextState[col] = sd->fDtran->elementAti(col);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
শূন্য RBBITableBuilder::buildSafeReverseTable(UErrorCode &status) {
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
    UnicodeString safePairs;

    int32_t অঙ্ক_Char_Classes = fRB->fSetBuilder->get_অঙ্ক_Char_Categories();
    int32_t অঙ্ক_States = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ধ্রুব auto lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    ধ্রুব auto wantedLookaheadsInPair =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    যদি (U_FAILURE(status)) {
        প্রত্যাগম;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t START_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_SET = -1;
    প্রতি (int32_t c1 = 1; c1 < অঙ্ক_Char_Classes; ++c1) {
        যদি (fRB->fSetBuilder->getFirstChar(c1) < 0) {
          অনুবর্ত;
        }
        প্রতি (int32_t c2 = 1; c2 < অঙ্ক_Char_Classes; ++c2) {
            যদি (fRB->fSetBuilder->getFirstChar(c2) < 0) {
                অনুবর্ত;
            }
            int32_t wantedEndState = -1;
            int32_t wantedLastAcceptingPosition;
            তার্কিক mustCheckConsistentWithStartC2 = অসত্য;
            ধ্রুব int32_t text[2] = {c1, c2};
            প্রতি (int32_t injectedState = START_STATE; injectedState < অঙ্ক_States; ++injectedState) {
                প্রতি (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= fLASlotsInUse; ++injectedLookahead) {
                    int32_t lastBreak = BEFORE_PAIR;
                    int32_t lastAcceptingPosition = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                    যদি (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ধ্রুব RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        যদি (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            lastAcceptingPosition = 0;
                        } অন্যথা যদি (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            অনুবর্ত;
                        }
                        যদি (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    গচ্ছ dfaStep;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    প্রতি (;;) {
                        s = START_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                        i = lastBreak;
                        // Loop Find_Next_Break in L2/26-135.
                        প্রতি (;;) {
                        dfaStep:
                            যদি (i == BEFORE_PAIR || i == 2) {
                                গচ্ছ inspectFinalState;
                            }
                            ধ্রুব int32_t প্রতীক_Ahead = text[i];
                            ++i;
                            ধ্রুব RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            যদি (sDescriptor->fDtran->elementAti(প্রতীক_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(প্রতীক_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } অন্যথা {
                                যদি (lastBreak != BEFORE_PAIR &&
                                    lastAcceptingPosition == lastBreak) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    গচ্ছ next_প্রতীক_Pair;
                                }
                                lastBreak = lastAcceptingPosition;
                                ভঙ্গ;
                            }

                            যদি (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                lastAcceptingPosition = i;
                            } অন্যথা যদি (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_SET) {
                                lastBreak = lookaheadPositions[sDescriptor->fAccepting];
                                ভঙ্গ;
                            }
                            যদি (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    যদি (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        অনুবর্ত;
                    }
                    যদি (wantedEndState < 0) {
                        wantedEndState = s;
                        wantedLastAcceptingPosition = lastAcceptingPosition;
                        প্রতি (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                            যদি (lookaheadPositions[l] != LOOKAHEAD_NOT_SET &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wantedLookaheadsInPair[l] = lookaheadPositions[i];
                            } অন্যথা {
                                wantedLookaheadsInPair[l] = LOOKAHEAD_NOT_SET;
                            }
                        }
                    } অন্যথা {
                        যদি (wantedEndState != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            গচ্ছ next_প্রতীক_Pair;
                        }
                        যদি (wantedLastAcceptingPosition != lastAcceptingPosition) {
                            গচ্ছ next_প্রতীক_Pair;
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
                    প্রতি (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                        যদি (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wantedLookaheadsInPair[l]) {
                            অনুবর্ত;
                        }
                        যদি (lookaheadPositions[l] == 2 || wantedLookaheadsInPair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            যদি (wantedEndState != START_STATE) {
                                গচ্ছ next_প্রতীক_Pair;
                            }
                        }
                        যদি (lookaheadPositions[l] == 1 || wantedLookaheadsInPair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            mustCheckConsistentWithStartC2 = সত্য;
                        }
                    }
                }
            }
            যদি (mustCheckConsistentWithStartC2) {
                ধ্রুব RBBIStateDescriptor& startDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(START_STATE));
                ধ্রুব auto c2State = startDescriptor.fDtran->elementAti(c2);
                যদি (c2State != wantedEndState) {
                    গচ্ছ next_প্রতীক_Pair;
                }
                ধ্রুব RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                যদি (wantedLastAcceptingPosition != 2) {
                    গচ্ছ next_প্রতীক_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                প্রতি (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                    যদি (((wantedLookaheadsInPair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wantedEndState != START_STATE) {
                        গচ্ছ next_প্রতীক_Pair;
                    }
                }
            }
            safePairs.append(static_cast<char16_t>(c1));
            safePairs.append(static_cast<char16_t>(c2));
        next_প্রতীক_Pair:
            অনুবর্ত;
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

    U_ASSERT(fSafeTable == nullptr);
    LocalPointer<UVector> lpSafeTable(
        নূতন UVector(uprv_deleteUObject, uhash_compareUnicodeString, অঙ্ক_Char_Classes + 2, status), status);
    যদি (U_FAILURE(status)) {
        প্রত্যাগম;
    }
    fSafeTable = lpSafeTable.orphan();
    প্রতি (int32_t row=0; row<অঙ্ক_Char_Classes + 2; ++row) {
        LocalPointer<UnicodeString> lpString(নূতন UnicodeString(অঙ্ক_Char_Classes, 0, অঙ্ক_Char_Classes+4), status);
        fSafeTable->adoptElement(lpString.orphan(), status);
    }
    যদি (U_FAILURE(status)) {
        প্রত্যাগম;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &startState = *static_cast<UnicodeString *>(fSafeTable->elementAt(1));
    প্রতি (int32_t charClass=0; charClass < অঙ্ক_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        startState.setCharAt(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    প্রতি (int32_t row=2; row<অঙ্ক_Char_Classes+2; ++row) {
        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(row));
        rowState = startState;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    প্রতি (int32_t pairIdx=0; pairIdx<safePairs.length(); pairIdx+=2) {
        int32_t c1 = safePairs.charAt(pairIdx);
        int32_t c2 = safePairs.charAt(pairIdx + 1);

        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(c2 + 2));
        rowState.setCharAt(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair states = {1, 0};
    যাবত্ (findDuplicateSafeState(&states)) {
        // printf("Removing duplicate safe states (%d, %d)\n", states.first, states.second);
        removeSafeState(states);
    }
}


//-----------------------------------------------------------------------------
//
//   getSafeTableSize()    Calculate the size of the runtime form of this
//                         safe state table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::getSafeTableSize() ধ্রুব {
    int32_t    size = 0;
    int32_t    অঙ্ক_Rows;
    int32_t    অঙ্ক_Cols;
    int32_t    rowSize;

    যদি (fSafeTable == nullptr) {
        প্রত্যাগম 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    অঙ্ক_Rows = fSafeTable->size();
    অঙ্ক_Cols = fRB->fSetBuilder->get_অঙ্ক_Char_Categories();

    যদি (use8BitsForSafeTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + পরিমাণ(int8_t)*অঙ্ক_Cols;
    } অন্যথা {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + পরিমাণ(int16_t)*অঙ্ক_Cols;
    }
    size   += অঙ্ক_Rows * rowSize;
    প্রত্যাগম size;
}

তার্কিক RBBITableBuilder::use8BitsForSafeTable() ধ্রুব {
    প্রত্যাগম fSafeTable->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
শূন্য RBBITableBuilder::exportSafeTable(শূন্য *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    পূর্ণাঙ্ক                col;

    যদি (U_FAILURE(*fStatus) || fSafeTable == nullptr) {
        প্রত্যাগম;
    }

    int32_t cat_গুনতি = fRB->fSetBuilder->get_অঙ্ক_Char_Categories();
    যদি (cat_গুনতি > 0x7fff ||
            fSafeTable->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        প্রত্যাগম;
    }

    table->f_অঙ্ক_States = fSafeTable->size();
    table->fFlags     = 0;
    যদি (use8BitsForSafeTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + পরিমাণ(uint8_t) * cat_গুনতি;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } অন্যথা {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + পরিমাণ(int16_t) * cat_গুনতি;
    }

    প্রতি (state=0; state<table->f_অঙ্ক_States; state++) {
        UnicodeString* rowString = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        যদি (use8BitsForSafeTable()) {
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->fTagsIdx    = 0;
            প্রতি (col=0; col<cat_গুনতি; col++) {
                U_ASSERT(rowString->charAt(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = static_cast<uint8_t>(rowString->charAt(col));
            }
        } অন্যথা {
            row->r16.fAccepting = 0;
            row->r16.fLookAhead = 0;
            row->r16.fTagsIdx    = 0;
            প্রতি (col=0; col<cat_গুনতি; col++) {
                row->r16.fNextState[col] = rowString->charAt(col);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//   printSet    Debug function.   Print the contents of a UVector
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
শূন্য RBBITableBuilder::printSet(UVector *s) {
    int32_t  i;
    প্রতি (i=0; i<s->size(); i++) {
        ধ্রুব RBBINode *v = static_cast<ধ্রুব RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==nullptr? -1 : v->f_Serial_অঙ্ক);
    }
    RBBIDebugPrintf("\n");
}
#endif


//-----------------------------------------------------------------------------
//
//   printStates    Debug Function.  Dump the fully constructed state transition table.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
শূন্য RBBITableBuilder::printStates() {
    পূর্ণাঙ্ক     c;    // input "character"
    পূর্ণাঙ্ক     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    প্রতি (c=0; c<fRB->fSetBuilder->get_অঙ্ক_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    প্রতি (c=0; c<fRB->fSetBuilder->get_অঙ্ক_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    প্রতি (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->fTagsIdx);
        প্রতি (c=0; c<fRB->fSetBuilder->get_অঙ্ক_Char_Categories(); c++) {
            RBBIDebugPrintf(" %3d", sd->fDtran->elementAti(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#endif


//-----------------------------------------------------------------------------
//
//   printSafeTable    Debug Function.  Dump the fully constructed safe table.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
শূন্য RBBITableBuilder::printReverseTable() {
    পূর্ণাঙ্ক     c;    // input "character"
    পূর্ণাঙ্ক     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    যদি (fSafeTable == nullptr) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        প্রত্যাগম;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    প্রতি (c=0; c<fRB->fSetBuilder->get_অঙ্ক_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    প্রতি (c=0; c<fRB->fSetBuilder->get_অঙ্ক_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    প্রতি (n=0; n<fSafeTable->size(); n++) {
        UnicodeString *rowString = (UnicodeString *)fSafeTable->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        প্রতি (c=0; c<fRB->fSetBuilder->get_অঙ্ক_Char_Categories(); c++) {
            RBBIDebugPrintf(" %2d", rowString->charAt(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#endif



//-----------------------------------------------------------------------------
//
//   printRuleStatusTable    Debug Function.  Dump the common rule status table
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
শূন্য RBBITableBuilder::printRuleStatusTable() {
    int32_t  thisRecord = 0;
    int32_t  nextRecord = 0;
    পূর্ণাঙ্ক      i;
    UVector  *tbl = fRB->fRuleStatusVals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    যাবত্ (nextRecord < tbl->size()) {
        thisRecord = nextRecord;
        nextRecord = thisRecord + tbl->elementAti(thisRecord) + 1;
        RBBIDebugPrintf("%4d   ", thisRecord);
        প্রতি (i=thisRecord+1; i<nextRecord; i++) {
            RBBIDebugPrintf("  %5d", tbl->elementAti(i));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#endif


//-----------------------------------------------------------------------------
//
//   RBBIStateDescriptor     Methods.  This is a very struct-like class
//                           Most access is directly to the fields.
//
//-----------------------------------------------------------------------------

RBBIStateDescriptor::RBBIStateDescriptor(পূর্ণাঙ্ক last_প্রবেশ_প্রতীক, UErrorCode *fStatus) {
    fMarked    = অসত্য;
    fAccepting = 0;
    fLookAhead = 0;
    fTagsIdx   = 0;
    fTagVals   = nullptr;
    fPositions = nullptr;
    fDtran     = nullptr;

    fDtran     = নূতন UVector32(last_প্রবেশ_প্রতীক+1, *fStatus);
    যদি (U_FAILURE(*fStatus)) {
        প্রত্যাগম;
    }
    যদি (fDtran == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    fDtran->setSize(last_প্রবেশ_প্রতীক+1);    // fDtran needs to be pre-sized.
                                           //   It is indexed by input symbols, and will
                                           //   hold  the next state number for each
                                           //   symbol.
}


RBBIStateDescriptor::~RBBIStateDescriptor() {
    delete       fPositions;
    delete       fDtran;
    delete       fTagVals;
    fPositions = nullptr;
    fDtran     = nullptr;
    fTagVals   = nullptr;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
