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
منطقي reachableByTransitions(ثابت UVector &states, ثابت int32_t source,
                            ثابت std::function<منطقي(int32_t)> isSink,
                            ثابت std::function<منطقي(int32_t)> excludedState, UErrorCode &status) {
    UStack boundary(status);
    {
        UVector32 &transitionsFromSource =
            *static_cast<RBBIStateDescriptor *>(states.elementAt(source))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        لكل (int32_t هێما = 0; هێما < transitionsFromSource.size(); ++هێما) {
            ثابت int32_t state = transitionsFromSource.elementAti(هێما);
            إذا (state != 0) {
                boundary.push(state, status);
            }
        }
    }
    ثابت auto visited = prv::make_unique<منطقي[]>(states.size(), status);
    طالما (U_SUCCESS(status) && !boundary.empty()) {
        ثابت int32_t s = boundary.popi();
        إذا (isSink(s)) {
            أرجع true;
        }
        إذا (excludedState(s) || visited[s]) {
            استمر;
        }
        visited[s] = true;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(states.elementAt(s))->fDtran;
        لكل (int32_t هێما = 0; هێما < transitions.size(); ++هێما) {
            ثابت int32_t t = transitions.elementAti(هێما);
            إذا (t != 0 && !visited[t]) {
                boundary.push(t, status);
            }
        }
    }
    أرجع خطأ;
}

} // namespace

ثابت int32_t kMaxStateFor8BitsTable = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **ڕەگ_گرێ, UErrorCode &status) :
        fRB(rb),
        f_درەخت(*ڕەگ_گرێ),
        fStatus(&status),
        fDStates(nullptr),
        fSafeTable(nullptr) {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = جديد UVector(status);
    إذا (U_SUCCESS(status) && fDStates == nullptr ) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    صحيح i;
    لكل (i=0; i<fDStates->size(); i++) {
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
فراغ  RBBITableBuilder::buildForwardTable() {

    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    إذا (f_درەخت==nullptr) {
        أرجع;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_درەخت = f_درەخت->flattenVariables(*fStatus, 0);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
#ifdef RBBI_DEBUG
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_درەخت(f_درەخت, true);
    }
#endif

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = جديد RBBINode(RBBINode::opCat, *fStatus);
    // Exit if memory allocation failed.
    إذا (cn == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    إذا (U_FAILURE(*fStatus)) {
        delete cn;
        أرجع;
    }
    cn->f_چەپ_Child = f_درەخت;
    f_درەخت->fParent = cn;
    RBBINode *end_Marker_گرێ = cn->f_ڕاست_Child = جديد RBBINode(RBBINode::endMark, *fStatus);
    // Delete and exit if memory allocation failed.
    إذا (cn->f_ڕاست_Child == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    إذا (U_FAILURE(*fStatus)) {
        delete cn;
        أرجع;
    }
    cn->f_ڕاست_Child->fParent = cn;
    f_درەخت = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_درەخت->flattenSets(*fStatus, 0);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
#ifdef RBBI_DEBUG
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_درەخت(f_درەخت, true);
    }
#endif


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_درەخت);
    calcFirstPos(f_درەخت);
    calcLastPos(f_درەخت);
    calcFollowPos(f_درەخت);
    إذا (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_درەخت);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    إذا (fRB->fChainRules) {
        calcChainedFollowPos(f_درەخت, end_Marker_گرێ);
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
فراغ RBBITableBuilder::calcNullable(RBBINode *n) {
    إذا (n == nullptr) {
        أرجع;
    }
    إذا (n->fType == RBBINode::setRef ||
        n->fType == RBBINode::endMark ) {
        // These are non-empty leaf node types.
        n->fNullable = خطأ;
        أرجع;
    }

    إذا (n->fType == RBBINode::lookAhead || n->fType == RBBINode::tag) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = true;
        أرجع;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_چەپ_Child);
    calcNullable(n->f_ڕاست_Child);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_چەپ_Child->fNullable || n->f_ڕاست_Child->fNullable;
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_چەپ_Child->fNullable && n->f_ڕاست_Child->fNullable;
    }
    وإلا إذا (n->fType == RBBINode::opStar || n->fType == RBBINode::opQuestion) {
        n->fNullable = true;
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
فراغ RBBITableBuilder::calcFirstPos(RBBINode *n) {
    إذا (n == nullptr) {
        أرجع;
    }
    إذا (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fFirstPosSet->addElement(n, *fStatus);
        أرجع;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calcFirstPos(n->f_چەپ_Child);
    calcFirstPos(n->f_ڕاست_Child);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        setAdd(n->fFirstPosSet, n->f_چەپ_Child->fFirstPosSet);
        setAdd(n->fFirstPosSet, n->f_ڕاست_Child->fFirstPosSet);
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        setAdd(n->fFirstPosSet, n->f_چەپ_Child->fFirstPosSet);
        إذا (n->f_چەپ_Child->fNullable) {
            setAdd(n->fFirstPosSet, n->f_ڕاست_Child->fFirstPosSet);
        }
    }
    وإلا إذا (n->fType == RBBINode::opStar ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fFirstPosSet, n->f_چەپ_Child->fFirstPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcLastPos(RBBINode *n) {
    إذا (n == nullptr) {
        أرجع;
    }
    إذا (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fLastPosSet->addElement(n, *fStatus);
        أرجع;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calcLastPos(n->f_چەپ_Child);
    calcLastPos(n->f_ڕاست_Child);

    // Apply functions from table 3.40 in Aho
    إذا (n->fType == RBBINode::opOr) {
        setAdd(n->fLastPosSet, n->f_چەپ_Child->fLastPosSet);
        setAdd(n->fLastPosSet, n->f_ڕاست_Child->fLastPosSet);
    }
    وإلا إذا (n->fType == RBBINode::opCat) {
        setAdd(n->fLastPosSet, n->f_ڕاست_Child->fLastPosSet);
        إذا (n->f_ڕاست_Child->fNullable) {
            setAdd(n->fLastPosSet, n->f_چەپ_Child->fLastPosSet);
        }
    }
    وإلا إذا (n->fType == RBBINode::opStar     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fLastPosSet, n->f_چەپ_Child->fLastPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcFollowPos(RBBINode *n) {
    إذا (n == nullptr ||
        n->fType == RBBINode::leafChar ||
        n->fType == RBBINode::endMark) {
        أرجع;
    }

    calcFollowPos(n->f_چەپ_Child);
    calcFollowPos(n->f_ڕاست_Child);

    // Aho rule #1
    إذا (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *Last_Pos_Of_چەپ_Child = n->f_چەپ_Child->fLastPosSet;

        لكل (ix = 0; ix < static_cast<uint32_t>(Last_Pos_Of_چەپ_Child->size()); ix++) {
            i = static_cast<RBBINode*>(Last_Pos_Of_چەپ_Child->elementAt(ix));
            setAdd(i->fFollowPos, n->f_ڕاست_Child->fFirstPosSet);
        }
    }

    // Aho rule #2
    إذا (n->fType == RBBINode::opStar ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        لكل (ix = 0; ix < static_cast<uint32_t>(n->fLastPosSet->size()); ix++) {
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
فراغ RBBITableBuilder::add_Rule_ڕەگ_Nodes(UVector *dest, RBBINode *گرێ) {
    إذا (گرێ == nullptr || U_FAILURE(*fStatus)) {
        أرجع;
    }
    U_ASSERT(!dest->hasDeleter());
    إذا (گرێ->f_Rule_ڕەگ) {
        dest->addElement(گرێ, *fStatus);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        أرجع;
    }
    add_Rule_ڕەگ_Nodes(dest, گرێ->f_چەپ_Child);
    add_Rule_ڕەگ_Nodes(dest, گرێ->f_ڕاست_Child);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::calcChainedFollowPos(RBBINode *درەخت, RBBINode *end_Mark_گرێ) {

    UVector         leafNodes(*fStatus);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }

    // get a list all leaf nodes
    درەخت->findNodes(&leafNodes, RBBINode::leafChar, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector rule_ڕەگ_Nodes(*fStatus);
    add_Rule_ڕەگ_Nodes(&rule_ڕەگ_Nodes, درەخت);

    UVector matchStartNodes(*fStatus);
    لكل (صحيح j=0; j<rule_ڕەگ_Nodes.size(); ++j) {
        RBBINode *گرێ = static_cast<RBBINode *>(rule_ڕەگ_Nodes.elementAt(j));
        إذا (گرێ->fChainIn) {
            setAdd(&matchStartNodes, گرێ->fFirstPosSet);
        }
    }
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }

    int32_t  end_گرێ_Ix;
    int32_t  start_گرێ_Ix;

    لكل (end_گرێ_Ix=0; end_گرێ_Ix<leafNodes.size(); end_گرێ_Ix++) {
        RBBINode* end_گرێ = static_cast<RBBINode*>(leafNodes.elementAt(end_گرێ_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        إذا (!end_گرێ->fFollowPos->contains(end_Mark_گرێ)) {
            استمر;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *start_گرێ;
        لكل (start_گرێ_Ix = 0; start_گرێ_Ix<matchStartNodes.size(); start_گرێ_Ix++) {
            start_گرێ = static_cast<RBBINode*>(matchStartNodes.elementAt(start_گرێ_Ix));
            إذا (start_گرێ->fType != RBBINode::leafChar) {
                استمر;
            }

            إذا (end_گرێ->fVal == start_گرێ->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                setAdd(end_گرێ->fFollowPos, start_گرێ->fFollowPos);
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

    إذا (U_FAILURE(*fStatus)) {
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
    RBBINode  *bof_گرێ = f_درەخت->f_چەپ_Child->f_چەپ_Child;
    U_ASSERT(bof_گرێ->fType == RBBINode::leafChar);
    U_ASSERT(bof_گرێ->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *matchStartNodes = f_درەخت->f_چەپ_Child->f_ڕاست_Child->fFirstPosSet;

    RBBINode *start_گرێ;
    صحيح       start_گرێ_Ix;
    لكل (start_گرێ_Ix = 0; start_گرێ_Ix<matchStartNodes->size(); start_گرێ_Ix++) {
        start_گرێ = static_cast<RBBINode*>(matchStartNodes->elementAt(start_گرێ_Ix));
        إذا (start_گرێ->fType != RBBINode::leafChar) {
            استمر;
        }

        إذا (start_گرێ->fVal == bof_گرێ->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            setAdd(bof_گرێ->fFollowPos, start_گرێ->fFollowPos);
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
فراغ RBBITableBuilder::buildStateTable() {
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = nullptr;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    صحيح      last_تێخستن_هێما = fRB->fSetBuilder->get_ژمارە_Char_Categories() - 1;
    failState = جديد RBBIStateDescriptor(last_تێخستن_هێما, fStatus);
    إذا (failState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        اذهب ExitBuildSTdeleteall;
    }
    failState->fPositions = جديد UVector(*fStatus);
    إذا (failState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    إذا (failState->fPositions == nullptr || U_FAILURE(*fStatus)) {
        اذهب ExitBuildSTdeleteall;
    }
    fDStates->addElement(failState, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        اذهب ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = جديد RBBIStateDescriptor(last_تێخستن_هێما, fStatus);
    إذا (initialState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    إذا (U_FAILURE(*fStatus)) {
        اذهب ExitBuildSTdeleteall;
    }
    initialState->fPositions = جديد UVector(*fStatus);
    إذا (initialState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    إذا (U_FAILURE(*fStatus)) {
        اذهب ExitBuildSTdeleteall;
    }
    setAdd(initialState->fPositions, f_درەخت->fFirstPosSet);
    fDStates->addElement(initialState, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        اذهب ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    لكل (;;) {
        RBBIStateDescriptor *T = nullptr;
        int32_t              tx;
        لكل (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *temp;
            temp = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            إذا (temp->fMarked == خطأ) {
                T = temp;
                اكسر;
            }
        }
        إذا (T == nullptr) {
            اكسر;
        }

        // mark T;
        T->fMarked = true;

        // for each input symbol a do begin
        int32_t  a;
        لكل (a = 1; a<=last_تێخستن_هێما; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = nullptr;
            RBBINode   *p;
            int32_t     px;
            لكل (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                إذا ((p->fType == RBBINode::leafChar) &&  (p->fVal == a)) {
                    إذا (U == nullptr) {
                        U = جديد UVector(*fStatus);
                        إذا (U == nullptr) {
                        	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                        	اذهب ExitBuildSTdeleteall;
                        }
                    }
                    setAdd(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = خطأ;
            إذا (U != nullptr) {
                U_ASSERT(U->size() > 0);
                صحيح  ix;
                لكل (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    إذا (setEquals(U, temp2->fPositions)) {
                        delete U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = true;
                        اكسر;
                    }
                }

                // Add U as an unmarked state to Dstates
                إذا (!UinDstates)
                {
                    RBBIStateDescriptor *newState = جديد RBBIStateDescriptor(last_تێخستن_هێما, fStatus);
                    إذا (newState == nullptr) {
                    	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                    }
                    إذا (U_FAILURE(*fStatus)) {
                        اذهب ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->addElement(newState, *fStatus);
                    إذا (U_FAILURE(*fStatus)) {
                        أرجع;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->setElementAt(ux, a);
            }
        }
    }
    أرجع;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    delete initialState;
    delete failState;
}


/**
 * mapLookAheadRules
 *
 */
فراغ RBBITableBuilder::mapLookAheadRules() {
    fLookAheadRuleMap =  جديد UVector32(fRB->fScanner->ژمارە_Rules() + 1, *fStatus);
    إذا (fLookAheadRuleMap == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    fLookAheadRuleMap->setSize(fRB->fScanner->ژمارە_Rules() + 1);

    لكل (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t laSlotForState = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        منطقي saw_Look_Ahead_گرێ = خطأ;
        لكل (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *گرێ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            إذا (گرێ->fType != RBBINode::گرێ_Type::lookAhead) {
                استمر;
            }
            saw_Look_Ahead_گرێ = true;
            int32_t rule_ژمارە = گرێ->fVal;     // Set when rule was originally parsed.
            U_ASSERT(rule_ژمارە < fLookAheadRuleMap->size());
            U_ASSERT(rule_ژمارە > 0);
            int32_t laSlot = fLookAheadRuleMap->elementAti(rule_ژمارە);
            إذا (laSlot != 0) {
                إذا (laSlotForState == 0) {
                    laSlotForState = laSlot;
                } وإلا {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(laSlot == laSlotForState);
                }
            }
        }
        إذا (!saw_Look_Ahead_گرێ) {
            استمر;
        }

        إذا (laSlotForState == 0) {
            laSlotForState = ++fLASlotsInUse;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        لكل (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *گرێ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            إذا (گرێ->fType != RBBINode::گرێ_Type::lookAhead) {
                استمر;
            }
            int32_t rule_ژمارە = گرێ->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = fLookAheadRuleMap->elementAti(rule_ژمارە);
            (فراغ)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == laSlotForState);
            fLookAheadRuleMap->setElementAt(laSlotForState, rule_ژمارە);
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
فراغ     RBBITableBuilder::flagAcceptingStates() {
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    UVector     endMarkerNodes(*fStatus);
    RBBINode    *endMarker;
    int32_t     i;
    int32_t     n;

    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }

    f_درەخت->findNodes(&endMarkerNodes, RBBINode::endMark, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }

    لكل (i=0; i<endMarkerNodes.size(); i++) {
        endMarker = static_cast<RBBINode*>(endMarkerNodes.elementAt(i));
        لكل (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            إذا (sd->fPositions->indexOf(endMarker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                إذا (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = fLookAheadRuleMap->elementAti(endMarker->fVal);
                    إذا (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                إذا (sd->fAccepting==ACCEPTING_UNCONDITIONAL && endMarker->fVal != 0) {
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
فراغ     RBBITableBuilder::flagLookAheadStates() {
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    UVector     lookAheadNodes(*fStatus);
    RBBINode    *look_Ahead_گرێ;
    int32_t     i;
    int32_t     n;

    f_درەخت->findNodes(&lookAheadNodes, RBBINode::lookAhead, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    لكل (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_گرێ = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_گرێ->fType == RBBINode::گرێ_Type::lookAhead);

        لكل (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->indexOf(look_Ahead_گرێ);
            إذا (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_گرێ == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookaheadSlot = fLookAheadRuleMap->elementAti(look_Ahead_گرێ->fVal);
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
فراغ     RBBITableBuilder::flagTaggedStates() {
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    UVector     tagNodes(*fStatus);
    RBBINode    *tag_گرێ;
    int32_t     i;
    int32_t     n;

    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    f_درەخت->findNodes(&tagNodes, RBBINode::tag, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    لكل (i=0; i<tagNodes.size(); i++) {                   // For each tag node t (all of 'em)
        tag_گرێ = static_cast<RBBINode*>(tagNodes.elementAt(i));

        لكل (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            إذا (sd->fPositions->indexOf(tag_گرێ) >= 0) {  //       if  s include the tag node t
                sortedAdd(&sd->fTagVals, tag_گرێ->fVal);
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
فراغ  RBBITableBuilder::mergeRuleStatusVals() {
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
    إذا (fRB->fRuleStatusVals->size() == 0) {
        fRB->fRuleStatusVals->addElement(1, *fStatus);  // Num of statuses in group
        fRB->fRuleStatusVals->addElement(static_cast<int32_t>(0), *fStatus); // and our single status of zero
    }

    //    For each state
    لكل (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_States_Tag_بەهاکان = sd->fTagVals;
        إذا (this_States_Tag_بەهاکان == nullptr) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->fTagsIdx = 0;
            استمر;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->fTagsIdx = -1;
        int32_t  thisTagGroupStart = 0;   // indexes into the global rule status vals list
        int32_t  nextTagGroupStart = 0;

        // Loop runs once per group of tags in the global list
        طالما (nextTagGroupStart < fRB->fRuleStatusVals->size()) {
            thisTagGroupStart = nextTagGroupStart;
            nextTagGroupStart += fRB->fRuleStatusVals->elementAti(thisTagGroupStart) + 1;
            إذا (this_States_Tag_بەهاکان->size() != fRB->fRuleStatusVals->elementAti(thisTagGroupStart)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                استمر;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            لكل (i=0; i<this_States_Tag_بەهاکان->size(); i++) {
                إذا (this_States_Tag_بەهاکان->elementAti(i) !=
                    fRB->fRuleStatusVals->elementAti(thisTagGroupStart + 1 + i) ) {
                    // Mismatch.
                    اكسر;
                }
            }

            إذا (i == this_States_Tag_بەهاکان->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->fTagsIdx = thisTagGroupStart;
                اكسر;
            }
        }

        إذا (sd->fTagsIdx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->fTagsIdx = fRB->fRuleStatusVals->size();
            fRB->fRuleStatusVals->addElement(this_States_Tag_بەهاکان->size(), *fStatus);
            لكل (i=0; i<this_States_Tag_بەهاکان->size(); i++) {
                fRB->fRuleStatusVals->addElement(this_States_Tag_بەهاکان->elementAti(i), *fStatus);
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

فراغ RBBITableBuilder::minimizeStates() {
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }

#إذا UPRV_HAS_SANITIZER
    إذا (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *fStatus = U_REGEX_PATTERN_TOO_BIG;
        أرجع;
    }
#endif
    بنية StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t fTagsIdx;
        منطقي operator==(StateType ثابت &other) ثابت {
            أرجع fAccepting == other.fAccepting &&
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
    بنية TypeToStates : UMemory {
        TypeToStates(ثابت StateType &type, UErrorCode &status)
            : type(type), states(prv::make_unique<UVector>(status, status)) {}
        StateType type;
        prv::unique_ptr<UVector> states;
    };
    UVector initialPartition(*fStatus);
    initialPartition.setDeleter(
        [](فراغ *p) { delete static_cast<TypeToStates *>(p); });
    لكل (int32_t i = 0; i < fDStates->size(); ++i) {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ثابت StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.fTagsIdx};
        int32_t j = 0;
        لكل (; j < initialPartition.size(); ++j) {
            auto &[type_j, states] =
                *static_cast<TypeToStates *>(initialPartition.elementAt(j));
            إذا (type_j == type) {
                states->addElement(i, *fStatus);
                اكسر;
            }
        }
        إذا (j == initialPartition.size()) {
            auto newEntry = prv::make_unique<TypeToStates>(type, *fStatus, *fStatus);
            إذا (U_FAILURE(*fStatus)) {
                أرجع;
            }
            newEntry->states->addElement(i, *fStatus);
            initialPartition.adoptElement(newEntry.release(), *fStatus);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    auto partition = prv::make_unique<UVector>(*fStatus, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    partition->setDeleter(
        [](فراغ *p) { delete static_cast<UVector*>(p); });
    
    لكل (int32_t i = 0; i < initialPartition.size(); ++i) {
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
    auto partition_signature = [&partition, هذا](int32_t stateIndex) -> prv::unique_ptr<UVector> {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(stateIndex));
        auto result = prv::make_unique<UVector>(state.fDtran->size(), *fStatus, *fStatus);
        إذا (U_FAILURE(*fStatus)) {
            أرجع nullptr;
        }
        لكل (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t toState = state.fDtran->elementAti(i);
            لكل (int32_t partIndex = 0; partIndex < partition->size(); ++partIndex) {
                ثابت UVector &part = *static_cast<UVector *>(partition->elementAt(partIndex));
                إذا (part.contains(toState)) {
                    result->addElement(partIndex, *fStatus);
                    اكسر;
                }
            }
        }
        result->setComparer([](ثابت UElement left, ثابت UElement ڕاست) -> UBool {
            أرجع left.integer == ڕاست.integer;
        });
        أرجع result;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    لكل (;;) {
        // Π_new.
        auto partitionNew = prv::make_unique<UVector>(*fStatus, *fStatus);
        إذا (U_FAILURE(*fStatus)) {
            أرجع;
        }
        partitionNew->setDeleter(
            [](فراغ *p) { delete static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        منطقي refined = خطأ;
        لكل (int32_t i = 0; i < partition->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ثابت UVector &group = *static_cast<UVector*>(partition->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            بنية SignatureToStates : UMemory {
                SignatureToStates(prv::unique_ptr<UVector> signature, UErrorCode &status)
                    : signature(std::move(signature)), states(prv::make_unique<UVector>(status, status)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> states;
            };
            UVector groupRefinement(*fStatus);
            groupRefinement.setDeleter([](فراغ *p) { delete static_cast<SignatureToStates*>(p); });
            لكل (int32_t j = 0; j < group.size(); ++j) {
                // Index of a state in the group.
                ثابت int32_t s = group.elementAti(j);
                auto signature = partition_signature(s);
                int32_t k = 0;
                لكل (; k < groupRefinement.size(); ++k) {
                    ثابت auto &[subgroupSignature, subgroup] =
                        *static_cast<SignatureToStates*>(groupRefinement.elementAt(k));
                    إذا (*subgroupSignature == *signature) {
                        subgroup->addElement(s, *fStatus);
                        اكسر;
                    }
                }
                إذا (k == groupRefinement.size()) {
                    ثابت auto newEntry =
                        جديد SignatureToStates(std::move(signature), *fStatus);
                    إذا (U_FAILURE(*fStatus)) {
                        أرجع;
                    }
                    إذا (newEntry == nullptr) {
                      *fStatus = U_MEMORY_ALLOCATION_ERROR;
                      أرجع;
                    }
                    newEntry->states->addElement(s, *fStatus);
                    groupRefinement.adoptElement(newEntry, *fStatus);
                }
            }
            refined |= groupRefinement.size() > 1;
            لكل (int32_t j = 0; j < groupRefinement.size(); ++j) {
                auto &[_, subgroup] = *static_cast<SignatureToStates*>(
                    groupRefinement.elementAt(j));
                partitionNew->adoptElement(subgroup.release(), *fStatus);
            }
        }
        إذا (refined) {
            partition = std::move(partitionNew);
        } وإلا {
            اكسر;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    لكل (صحيح i = 0; i < partition->size(); ++i) {
        UVector *ثابت part_i = static_cast<UVector *>(partition->elementAt(i));
        إذا (part_i->contains(0)) {
            فراغ *ثابت part_0 = partition->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ثابت auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 0);
            partition->setElementAt(part_0, i);
            partition->setDeleter(deleter);
        } وإلا إذا (part_i->contains(1)) {
            فراغ *ثابت part_1 = partition->elementAt(1);
            ثابت auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 1);
            partition->setElementAt(part_1, i);
            partition->setDeleter(deleter);
        }
    }
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    auto oldStateToPart = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    لكل (صحيح i = 0; i < partition->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        لكل (صحيح j = 0; j < part.size(); ++j) {
            oldStateToPart[part.elementAti(j)] = i;
        }
    }
    auto ثابت oldStates = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*fStatus, *fStatus).release();
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    لكل (صحيح i = 0; i < partition->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        RBBIStateDescriptor *ثابت state =
            static_cast<RBBIStateDescriptor*>(oldStates->elementAt(part.elementAti(0)));
        fDStates->addElement(state, *fStatus);
        لكل (صحيح j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->setElementAt(oldStateToPart[state->fDtran->elementAti(j)], j);
        }
        oldStates->setElementAt(nullptr, part.elementAti(0));
    }
    oldStates->setDeleter([](فراغ *p) { delete static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::sortedAdd(UVector **vector, int32_t val) {
    int32_t i;

    إذا (*vector == nullptr) {
        *vector = جديد UVector(*fStatus);
    }
    إذا (*vector == nullptr || U_FAILURE(*fStatus)) {
        أرجع;
    }
    UVector *vec = *vector;
    int32_t  vSize = vec->size();
    لكل (i=0; i<vSize; i++) {
        int32_t valAtI = vec->elementAti(i);
        إذا (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            أرجع;
        }
        إذا (valAtI > val) {
            اكسر;
        }
    }
    vec->زیادکردن_Element_At(val, i, *fStatus);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::setAdd(UVector *dest, UVector *source) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!source->hasDeleter());
    int32_t destOriginalSize = dest->size();
    int32_t sourceSize       = source->size();
    int32_t di           = 0;
    Maybe_کەڵەکە_Array<فراغ *, 16> destArray, sourceArray;  // Handle small cases without malloc
    فراغ **destPtr, **sourcePtr;
    فراغ **destLim, **sourceLim;

    إذا (destOriginalSize > destArray.getCapacity()) {
        إذا (destArray.resize(destOriginalSize) == nullptr) {
            أرجع;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + destOriginalSize;  // destArray.getArrayLimit()?

    إذا (sourceSize > sourceArray.getCapacity()) {
        إذا (sourceArray.resize(sourceSize) == nullptr) {
            أرجع;
        }
    }
    sourcePtr = sourceArray.getAlias();
    sourceLim = sourcePtr + sourceSize;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (فراغ) dest->toArray(destPtr);
    (فراغ) source->toArray(sourcePtr);

    dest->setSize(sourceSize+destOriginalSize, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }

    طالما (sourcePtr < sourceLim && destPtr < destLim) {
        إذا (*destPtr == *sourcePtr) {
            dest->setElementAt(*sourcePtr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        وإلا إذا (uprv_memcmp(destPtr, sourcePtr, حجم(فراغ *)) < 0) {
            dest->setElementAt(*destPtr++, di++);
        }
        وإلا { /* *sourcePtr < *destPtr */
            dest->setElementAt(*sourcePtr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    طالما (destPtr < destLim) {
        dest->setElementAt(*destPtr++, di++);
    }
    طالما (sourcePtr < sourceLim) {
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
    أرجع a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
فراغ RBBITableBuilder::printPosSets(RBBINode *n) {
    إذا (n==nullptr) {
        أرجع;
    }
    printf("\n");
    RBBINode::print_گرێ_Header();
    RBBINode::print_گرێ(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    printSet(n->fFirstPosSet);

    RBBIDebugPrintf("         lastpos:   ");
    printSet(n->fLastPosSet);

    RBBIDebugPrintf("         followpos: ");
    printSet(n->fFollowPos);

    printPosSets(n->f_چەپ_Child);
    printPosSets(n->f_ڕاست_Child);
}
#endif

//
//    findDuplCharClassFrom()
//
منطقي RBBITableBuilder::findDuplCharClassFrom(IntPair *categories) {
    int32_t ژمارە_States = fDStates->size();
    int32_t ژمارە_Cols = fRB->fSetBuilder->get_ژمارە_Char_Categories();

    لكل (; categories->first < ژمارە_Cols-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        صحيح limitSecond = categories->first < fRB->fSetBuilder->getDictCategoriesStart() ?
            fRB->fSetBuilder->getDictCategoriesStart() : ژمارە_Cols;
        لكل (categories->second=categories->first+1; categories->second < limitSecond; categories->second++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t table_base = 0;
            uint16_t table_dupl = 1;
            لكل (int32_t state=0; state<ژمارە_States; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                table_base = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                table_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->second));
                إذا (table_base != table_dupl) {
                    اكسر;
                }
            }
            إذا (table_base == table_dupl) {
                أرجع true;
            }
        }
    }
    أرجع خطأ;
}


//
//    removeColumn()
//
فراغ RBBITableBuilder::removeColumn(int32_t column) {
    int32_t ژمارە_States = fDStates->size();
    لكل (int32_t state=0; state<ژمارە_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(column < sd->fDtran->size());
        sd->fDtran->removeElementAt(column);
    }
}


منطقي RBBITableBuilder::findDuplicateSafeState(IntPair *states) {
    int32_t ژمارە_States = fSafeTable->size();

    لكل (; states->first<ژمارە_States-1; states->first++) {
        UnicodeString *firstRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->first));
        لكل (states->second=states->first+1; states->second<ژمارە_States; states->second++) {
            UnicodeString *duplRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->second));
            منطقي rowsMatch = true;
            int32_t ژمارە_Cols = firstRow->length();
            لكل (int32_t col=0; col < ژمارە_Cols; ++col) {
                int32_t firstVal = firstRow->charAt(col);
                int32_t duplVal = duplRow->charAt(col);
                إذا (!((firstVal == duplVal) ||
                        ((firstVal == states->first || firstVal == states->second) &&
                        (duplVal  == states->first || duplVal  == states->second)))) {
                    rowsMatch = خطأ;
                    اكسر;
                }
            }
            إذا (rowsMatch) {
                أرجع true;
            }
        }
    }
    أرجع خطأ;
}

فراغ RBBITableBuilder::removeSafeState(IntPair duplStates) {
    ثابت int32_t keepState = duplStates.first;
    ثابت int32_t duplState = duplStates.second;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < fSafeTable->size());

    fSafeTable->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t ژمارە_States = fSafeTable->size();
    لكل (int32_t state=0; state<ژمارە_States; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        int32_t ژمارە_Cols = sd->length();
        لكل (int32_t col=0; col<ژمارە_Cols; col++) {
            int32_t existingVal = sd->charAt(col);
            int32_t newVal = existingVal;
            إذا (existingVal == duplState) {
                newVal = keepState;
            } وإلا إذا (existingVal > duplState) {
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
    ثابت int32_t old_State_ژماردن = fDStates->size();
    minimizeStates();
    أرجع fDStates->size() - old_State_ژماردن;
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
    إذا (fLASlotsInUse == ACCEPTING_UNCONDITIONAL) {
        أرجع;
    }
    ثابت int32_t lookahead_ژماردن = fLASlotsInUse - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ثابت int32_t firstLookahead = ACCEPTING_UNCONDITIONAL + 1;
    ثابت auto stateDescriptor = [هذا](ثابت int32_t state) -> RBBIStateDescriptor & {
        أرجع *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ثابت auto lookaheadReachability =
        prv::make_unique<منطقي[]>(lookahead_ژماردن * lookahead_ژماردن, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    لكل (int32_t l = firstLookahead; l <= fLASlotsInUse; ++l) {
        لكل (int32_t k = firstLookahead; k <= fLASlotsInUse; ++k) {
            إذا (k != l) {
                لكل (int32_t source = 1; source < fDStates->size(); ++source) {
                    إذا (static_cast<int32_t>(stateDescriptor(source).fLookAhead) != l) {
                        استمر;
                    }
                    إذا (reachableByTransitions(
                            *fDStates, source, /*isSink=*/
                            [&](ثابت int32_t state) {
                                أرجع static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](ثابت int32_t state) {
                                أرجع static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *fStatus)) {
                        lookaheadReachability[(l - firstLookahead) * lookahead_ژماردن + k -
                                              firstLookahead] = true;
                    }
                }
            }
        }
    }

    ثابت auto colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_ژماردن, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    لكل (int32_t chromatic_ژمار = 1;; ++chromatic_ژمار) {
        uprv_memset(colours.get(), 0, حجم(int32_t) * lookahead_ژماردن);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        لكل (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            لكل (صحيح source = 0; source < lookahead_ژماردن; ++source) {
                لكل (صحيح sink = 0; sink < lookahead_ژماردن; ++sink) {
                    إذا (lookaheadReachability[source * lookahead_ژماردن + sink] &&
                        colours[source] == colours[sink]) {
                        اذهب nextColouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            لكل (صحيح i = 0; i < fDStates->size(); ++i) {
                إذا (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        firstLookahead +
                        colours[stateDescriptor(i).fAccepting - firstLookahead];
                }
                إذا (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - firstLookahead];
                }
            }
            fLASlotsInUse = ACCEPTING_UNCONDITIONAL + chromatic_ژمار;
            أرجع;

          nextColouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            لكل (int32_t i = 0; i < lookahead_ژماردن - 1 && colours[i] == chromatic_ژمار;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            إذا (colours[lookahead_ژماردن - 1] == chromatic_ژمار) {
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
int32_t  RBBITableBuilder::getTableSize() ثابت {
    int32_t    size = 0;
    int32_t    ژمارە_Rows;
    int32_t    ژمارە_Cols;
    int32_t    rowSize;

    إذا (f_درەخت == nullptr) {
        أرجع 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    ژمارە_Rows = fDStates->size();
    ژمارە_Cols = fRB->fSetBuilder->get_ژمارە_Char_Categories();

    إذا (use8BitsForTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + حجم(int8_t)*ژمارە_Cols;
    } وإلا {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + حجم(int16_t)*ژمارە_Cols;
    }
    size   += ژمارە_Rows * rowSize;
    أرجع size;
}

منطقي RBBITableBuilder::use8BitsForTable() ثابت {
    أرجع fDStates->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::exportTable(فراغ *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    صحيح                col;

    إذا (U_FAILURE(*fStatus) || f_درەخت == nullptr) {
        أرجع;
    }

    int32_t cat_ژماردن = fRB->fSetBuilder->get_ژمارە_Char_Categories();
    إذا (cat_ژماردن > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        أرجع;
    }

    table->f_ژمارە_States = fDStates->size();
    table->fDictCategoriesStart = fRB->fSetBuilder->getDictCategoriesStart();
    table->fLookAheadResultsSize = fLASlotsInUse == ACCEPTING_UNCONDITIONAL ? 0 : fLASlotsInUse + 1;
    table->fFlags     = 0;
    إذا (use8BitsForTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + حجم(uint8_t) * cat_ژماردن;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } وإلا {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + حجم(int16_t) * cat_ژماردن;
    }
    إذا (fRB->fLookAheadHardBreak) {
        table->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    لكل (state=0; state<table->f_ژمارە_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        إذا (use8BitsForTable()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 255);
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->fTagsIdx   = sd->fTagsIdx;
            لكل (col=0; col<cat_ژماردن; col++) {
                U_ASSERT (sd->fDtran->elementAti(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = sd->fDtran->elementAti(col);
            }
        } وإلا {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 0xffff);
            row->r16.fAccepting = sd->fAccepting;
            row->r16.fLookAhead = sd->fLookAhead;
            row->r16.fTagsIdx   = sd->fTagsIdx;
            لكل (col=0; col<cat_ژماردن; col++) {
                row->r16.fNextState[col] = sd->fDtran->elementAti(col);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
فراغ RBBITableBuilder::buildSafeReverseTable(UErrorCode &status) {
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

    int32_t ژمارە_Char_Classes = fRB->fSetBuilder->get_ژمارە_Char_Categories();
    int32_t ژمارە_States = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ثابت auto lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    ثابت auto wantedLookaheadsInPair =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t START_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_SET = -1;
    لكل (int32_t c1 = 1; c1 < ژمارە_Char_Classes; ++c1) {
        إذا (fRB->fSetBuilder->getFirstChar(c1) < 0) {
          استمر;
        }
        لكل (int32_t c2 = 1; c2 < ژمارە_Char_Classes; ++c2) {
            إذا (fRB->fSetBuilder->getFirstChar(c2) < 0) {
                استمر;
            }
            int32_t wantedEndState = -1;
            int32_t wantedLastAcceptingPosition;
            منطقي mustCheckConsistentWithStartC2 = خطأ;
            ثابت int32_t text[2] = {c1, c2};
            لكل (int32_t injectedState = START_STATE; injectedState < ژمارە_States; ++injectedState) {
                لكل (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= fLASlotsInUse; ++injectedLookahead) {
                    int32_t lastBreak = BEFORE_PAIR;
                    int32_t lastAcceptingPosition = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                    إذا (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ثابت RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        إذا (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            lastAcceptingPosition = 0;
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
                    اذهب dfaStep;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    لكل (;;) {
                        s = START_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                        i = lastBreak;
                        // Loop Find_Next_Break in L2/26-135.
                        لكل (;;) {
                        dfaStep:
                            إذا (i == BEFORE_PAIR || i == 2) {
                                اذهب inspectFinalState;
                            }
                            ثابت int32_t هێما_Ahead = text[i];
                            ++i;
                            ثابت RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            إذا (sDescriptor->fDtran->elementAti(هێما_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(هێما_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } وإلا {
                                إذا (lastBreak != BEFORE_PAIR &&
                                    lastAcceptingPosition == lastBreak) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    اذهب next_هێما_Pair;
                                }
                                lastBreak = lastAcceptingPosition;
                                اكسر;
                            }

                            إذا (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                lastAcceptingPosition = i;
                            } وإلا إذا (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_SET) {
                                lastBreak = lookaheadPositions[sDescriptor->fAccepting];
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
                    إذا (wantedEndState < 0) {
                        wantedEndState = s;
                        wantedLastAcceptingPosition = lastAcceptingPosition;
                        لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                            إذا (lookaheadPositions[l] != LOOKAHEAD_NOT_SET &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wantedLookaheadsInPair[l] = lookaheadPositions[i];
                            } وإلا {
                                wantedLookaheadsInPair[l] = LOOKAHEAD_NOT_SET;
                            }
                        }
                    } وإلا {
                        إذا (wantedEndState != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            اذهب next_هێما_Pair;
                        }
                        إذا (wantedLastAcceptingPosition != lastAcceptingPosition) {
                            اذهب next_هێما_Pair;
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
                    لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                        إذا (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wantedLookaheadsInPair[l]) {
                            استمر;
                        }
                        إذا (lookaheadPositions[l] == 2 || wantedLookaheadsInPair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            إذا (wantedEndState != START_STATE) {
                                اذهب next_هێما_Pair;
                            }
                        }
                        إذا (lookaheadPositions[l] == 1 || wantedLookaheadsInPair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            mustCheckConsistentWithStartC2 = true;
                        }
                    }
                }
            }
            إذا (mustCheckConsistentWithStartC2) {
                ثابت RBBIStateDescriptor& startDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(START_STATE));
                ثابت auto c2State = startDescriptor.fDtran->elementAti(c2);
                إذا (c2State != wantedEndState) {
                    اذهب next_هێما_Pair;
                }
                ثابت RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                إذا (wantedLastAcceptingPosition != 2) {
                    اذهب next_هێما_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                لكل (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                    إذا (((wantedLookaheadsInPair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wantedEndState != START_STATE) {
                        اذهب next_هێما_Pair;
                    }
                }
            }
            safePairs.append(static_cast<char16_t>(c1));
            safePairs.append(static_cast<char16_t>(c2));
        next_هێما_Pair:
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

    U_ASSERT(fSafeTable == nullptr);
    LocalPointer<UVector> lpSafeTable(
        جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, ژمارە_Char_Classes + 2, status), status);
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    fSafeTable = lpSafeTable.orphan();
    لكل (int32_t row=0; row<ژمارە_Char_Classes + 2; ++row) {
        LocalPointer<UnicodeString> lpString(جديد UnicodeString(ژمارە_Char_Classes, 0, ژمارە_Char_Classes+4), status);
        fSafeTable->adoptElement(lpString.orphan(), status);
    }
    إذا (U_FAILURE(status)) {
        أرجع;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &startState = *static_cast<UnicodeString *>(fSafeTable->elementAt(1));
    لكل (int32_t charClass=0; charClass < ژمارە_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        startState.setCharAt(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    لكل (int32_t row=2; row<ژمارە_Char_Classes+2; ++row) {
        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(row));
        rowState = startState;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    لكل (int32_t pairIdx=0; pairIdx<safePairs.length(); pairIdx+=2) {
        int32_t c1 = safePairs.charAt(pairIdx);
        int32_t c2 = safePairs.charAt(pairIdx + 1);

        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(c2 + 2));
        rowState.setCharAt(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair states = {1, 0};
    طالما (findDuplicateSafeState(&states)) {
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
int32_t  RBBITableBuilder::getSafeTableSize() ثابت {
    int32_t    size = 0;
    int32_t    ژمارە_Rows;
    int32_t    ژمارە_Cols;
    int32_t    rowSize;

    إذا (fSafeTable == nullptr) {
        أرجع 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    ژمارە_Rows = fSafeTable->size();
    ژمارە_Cols = fRB->fSetBuilder->get_ژمارە_Char_Categories();

    إذا (use8BitsForSafeTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + حجم(int8_t)*ژمارە_Cols;
    } وإلا {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + حجم(int16_t)*ژمارە_Cols;
    }
    size   += ژمارە_Rows * rowSize;
    أرجع size;
}

منطقي RBBITableBuilder::use8BitsForSafeTable() ثابت {
    أرجع fSafeTable->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
فراغ RBBITableBuilder::exportSafeTable(فراغ *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    صحيح                col;

    إذا (U_FAILURE(*fStatus) || fSafeTable == nullptr) {
        أرجع;
    }

    int32_t cat_ژماردن = fRB->fSetBuilder->get_ژمارە_Char_Categories();
    إذا (cat_ژماردن > 0x7fff ||
            fSafeTable->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        أرجع;
    }

    table->f_ژمارە_States = fSafeTable->size();
    table->fFlags     = 0;
    إذا (use8BitsForSafeTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + حجم(uint8_t) * cat_ژماردن;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } وإلا {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + حجم(int16_t) * cat_ژماردن;
    }

    لكل (state=0; state<table->f_ژمارە_States; state++) {
        UnicodeString* rowString = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        إذا (use8BitsForSafeTable()) {
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->fTagsIdx    = 0;
            لكل (col=0; col<cat_ژماردن; col++) {
                U_ASSERT(rowString->charAt(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = static_cast<uint8_t>(rowString->charAt(col));
            }
        } وإلا {
            row->r16.fAccepting = 0;
            row->r16.fLookAhead = 0;
            row->r16.fTagsIdx    = 0;
            لكل (col=0; col<cat_ژماردن; col++) {
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
فراغ RBBITableBuilder::printSet(UVector *s) {
    int32_t  i;
    لكل (i=0; i<s->size(); i++) {
        ثابت RBBINode *v = static_cast<ثابت RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==nullptr? -1 : v->f_Serial_ژمارە);
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
فراغ RBBITableBuilder::printStates() {
    صحيح     c;    // input "character"
    صحيح     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    لكل (c=0; c<fRB->fSetBuilder->get_ژمارە_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    لكل (c=0; c<fRB->fSetBuilder->get_ژمارە_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    لكل (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->fTagsIdx);
        لكل (c=0; c<fRB->fSetBuilder->get_ژمارە_Char_Categories(); c++) {
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
فراغ RBBITableBuilder::printReverseTable() {
    صحيح     c;    // input "character"
    صحيح     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    إذا (fSafeTable == nullptr) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        أرجع;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    لكل (c=0; c<fRB->fSetBuilder->get_ژمارە_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    لكل (c=0; c<fRB->fSetBuilder->get_ژمارە_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    لكل (n=0; n<fSafeTable->size(); n++) {
        UnicodeString *rowString = (UnicodeString *)fSafeTable->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        لكل (c=0; c<fRB->fSetBuilder->get_ژمارە_Char_Categories(); c++) {
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
فراغ RBBITableBuilder::printRuleStatusTable() {
    int32_t  thisRecord = 0;
    int32_t  nextRecord = 0;
    صحيح      i;
    UVector  *tbl = fRB->fRuleStatusVals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    طالما (nextRecord < tbl->size()) {
        thisRecord = nextRecord;
        nextRecord = thisRecord + tbl->elementAti(thisRecord) + 1;
        RBBIDebugPrintf("%4d   ", thisRecord);
        لكل (i=thisRecord+1; i<nextRecord; i++) {
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

RBBIStateDescriptor::RBBIStateDescriptor(صحيح last_تێخستن_هێما, UErrorCode *fStatus) {
    fMarked    = خطأ;
    fAccepting = 0;
    fLookAhead = 0;
    fTagsIdx   = 0;
    fTagVals   = nullptr;
    fPositions = nullptr;
    fDtran     = nullptr;

    fDtran     = جديد UVector32(last_تێخستن_هێما+1, *fStatus);
    إذا (U_FAILURE(*fStatus)) {
        أرجع;
    }
    إذا (fDtran == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    fDtran->setSize(last_تێخستن_هێما+1);    // fDtran needs to be pre-sized.
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
