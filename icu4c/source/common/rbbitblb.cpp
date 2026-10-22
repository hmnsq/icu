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

#شامل <functional>

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_BREAK_ITERATION

#شامل "unicode/unistr.h"
#شامل "rbbitblb.h"
#شامل "rbbirb.h"
#شامل "rbbiscan.h"
#شامل "rbbisetb.h"
#شامل "rbbidata.h"
#شامل "cstring.h"
#شامل "uassert.h"
#شامل "uvectr32.h"
#شامل "cmemory.h"

U_NAMESPACE_BEGIN

فضاینام {

// Given the `RBBITableBuilder::fDStates` vector of `RBBIStateDescriptor`s, returns
// true if a state for which `isSink` returns true is reachable from state `source` by following
// transitions without going through any state for which `excludedState` returns true.
// Note that “going through” is distinct from “reaching”, and means entering and then leaving: A
// state can be both a sink and excluded, and can still be reachable.
منطقی reachableByTransitions(ثابت UVector &states, ثابت int32_t source,
                            ثابت std::function<منطقی(int32_t)> isSink,
                            ثابت std::function<منطقی(int32_t)> excludedState, UErrorCode &status) {
    UStack boundary(status);
    {
        UVector32 &transitionsFromSource =
            *static_cast<RBBIStateDescriptor *>(states.elementAt(source))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        برای (int32_t نماد = 0; نماد < transitionsFromSource.size(); ++نماد) {
            ثابت int32_t state = transitionsFromSource.elementAti(نماد);
            اگر (state != 0) {
                boundary.push(state, status);
            }
        }
    }
    ثابت auto visited = prv::make_unique<منطقی[]>(states.size(), status);
    وقتی (U_SUCCESS(status) && !boundary.empty()) {
        ثابت int32_t s = boundary.popi();
        اگر (isSink(s)) {
            بازگشت درست;
        }
        اگر (excludedState(s) || visited[s]) {
            ادامه;
        }
        visited[s] = درست;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(states.elementAt(s))->fDtran;
        برای (int32_t نماد = 0; نماد < transitions.size(); ++نماد) {
            ثابت int32_t t = transitions.elementAti(نماد);
            اگر (t != 0 && !visited[t]) {
                boundary.push(t, status);
            }
        }
    }
    بازگشت نادرست;
}

} // namespace

ثابت int32_t kMaxStateFor8BitsTable = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **ریشه_گره, UErrorCode &status) :
        fRB(rb),
        f_درخت(*ریشه_گره),
        fStatus(&status),
        fDStates(nullptr),
        fSafeTable(nullptr) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = جدید UVector(status);
    اگر (U_SUCCESS(status) && fDStates == nullptr ) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    صحیح i;
    برای (i=0; i<fDStates->size(); i++) {
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
تهی  RBBITableBuilder::buildForwardTable() {

    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    اگر (f_درخت==nullptr) {
        بازگشت;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_درخت = f_درخت->flattenVariables(*fStatus, 0);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
#ifdef RBBI_DEBUG
    اگر (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_درخت(f_درخت, درست);
    }
#endif

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = جدید RBBINode(RBBINode::opCat, *fStatus);
    // Exit if memory allocation failed.
    اگر (cn == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    اگر (U_FAILURE(*fStatus)) {
        delete cn;
        بازگشت;
    }
    cn->f_چپ_Child = f_درخت;
    f_درخت->fParent = cn;
    RBBINode *end_Marker_گره = cn->f_راست_Child = جدید RBBINode(RBBINode::endMark, *fStatus);
    // Delete and exit if memory allocation failed.
    اگر (cn->f_راست_Child == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    اگر (U_FAILURE(*fStatus)) {
        delete cn;
        بازگشت;
    }
    cn->f_راست_Child->fParent = cn;
    f_درخت = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_درخت->flattenSets(*fStatus, 0);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
#ifdef RBBI_DEBUG
    اگر (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_درخت(f_درخت, درست);
    }
#endif


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_درخت);
    calcFirstPos(f_درخت);
    calcLastPos(f_درخت);
    calcFollowPos(f_درخت);
    اگر (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_درخت);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    اگر (fRB->fChainRules) {
        calcChainedFollowPos(f_درخت, end_Marker_گره);
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
تهی RBBITableBuilder::calcNullable(RBBINode *n) {
    اگر (n == nullptr) {
        بازگشت;
    }
    اگر (n->fType == RBBINode::setRef ||
        n->fType == RBBINode::endMark ) {
        // These are non-empty leaf node types.
        n->fNullable = نادرست;
        بازگشت;
    }

    اگر (n->fType == RBBINode::lookAhead || n->fType == RBBINode::tag) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = درست;
        بازگشت;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_چپ_Child);
    calcNullable(n->f_راست_Child);

    // Apply functions from table 3.40 in Aho
    اگر (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_چپ_Child->fNullable || n->f_راست_Child->fNullable;
    }
    وگرنه اگر (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_چپ_Child->fNullable && n->f_راست_Child->fNullable;
    }
    وگرنه اگر (n->fType == RBBINode::opStar || n->fType == RBBINode::opQuestion) {
        n->fNullable = درست;
    }
    وگرنه {
        n->fNullable = نادرست;
    }
}




//-----------------------------------------------------------------------------
//
//   calcFirstPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
تهی RBBITableBuilder::calcFirstPos(RBBINode *n) {
    اگر (n == nullptr) {
        بازگشت;
    }
    اگر (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fFirstPosSet->addElement(n, *fStatus);
        بازگشت;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calcFirstPos(n->f_چپ_Child);
    calcFirstPos(n->f_راست_Child);

    // Apply functions from table 3.40 in Aho
    اگر (n->fType == RBBINode::opOr) {
        setAdd(n->fFirstPosSet, n->f_چپ_Child->fFirstPosSet);
        setAdd(n->fFirstPosSet, n->f_راست_Child->fFirstPosSet);
    }
    وگرنه اگر (n->fType == RBBINode::opCat) {
        setAdd(n->fFirstPosSet, n->f_چپ_Child->fFirstPosSet);
        اگر (n->f_چپ_Child->fNullable) {
            setAdd(n->fFirstPosSet, n->f_راست_Child->fFirstPosSet);
        }
    }
    وگرنه اگر (n->fType == RBBINode::opStar ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fFirstPosSet, n->f_چپ_Child->fFirstPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
تهی RBBITableBuilder::calcLastPos(RBBINode *n) {
    اگر (n == nullptr) {
        بازگشت;
    }
    اگر (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fLastPosSet->addElement(n, *fStatus);
        بازگشت;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calcLastPos(n->f_چپ_Child);
    calcLastPos(n->f_راست_Child);

    // Apply functions from table 3.40 in Aho
    اگر (n->fType == RBBINode::opOr) {
        setAdd(n->fLastPosSet, n->f_چپ_Child->fLastPosSet);
        setAdd(n->fLastPosSet, n->f_راست_Child->fLastPosSet);
    }
    وگرنه اگر (n->fType == RBBINode::opCat) {
        setAdd(n->fLastPosSet, n->f_راست_Child->fLastPosSet);
        اگر (n->f_راست_Child->fNullable) {
            setAdd(n->fLastPosSet, n->f_چپ_Child->fLastPosSet);
        }
    }
    وگرنه اگر (n->fType == RBBINode::opStar     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fLastPosSet, n->f_چپ_Child->fLastPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
تهی RBBITableBuilder::calcFollowPos(RBBINode *n) {
    اگر (n == nullptr ||
        n->fType == RBBINode::leafChar ||
        n->fType == RBBINode::endMark) {
        بازگشت;
    }

    calcFollowPos(n->f_چپ_Child);
    calcFollowPos(n->f_راست_Child);

    // Aho rule #1
    اگر (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *Last_Pos_Of_چپ_Child = n->f_چپ_Child->fLastPosSet;

        برای (ix = 0; ix < static_cast<uint32_t>(Last_Pos_Of_چپ_Child->size()); ix++) {
            i = static_cast<RBBINode*>(Last_Pos_Of_چپ_Child->elementAt(ix));
            setAdd(i->fFollowPos, n->f_راست_Child->fFirstPosSet);
        }
    }

    // Aho rule #2
    اگر (n->fType == RBBINode::opStar ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        برای (ix = 0; ix < static_cast<uint32_t>(n->fLastPosSet->size()); ix++) {
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
تهی RBBITableBuilder::add_Rule_ریشه_Nodes(UVector *dest, RBBINode *گره) {
    اگر (گره == nullptr || U_FAILURE(*fStatus)) {
        بازگشت;
    }
    U_ASSERT(!dest->hasDeleter());
    اگر (گره->f_Rule_ریشه) {
        dest->addElement(گره, *fStatus);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        بازگشت;
    }
    add_Rule_ریشه_Nodes(dest, گره->f_چپ_Child);
    add_Rule_ریشه_Nodes(dest, گره->f_راست_Child);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
تهی RBBITableBuilder::calcChainedFollowPos(RBBINode *درخت, RBBINode *end_Mark_گره) {

    UVector         leafNodes(*fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }

    // get a list all leaf nodes
    درخت->findNodes(&leafNodes, RBBINode::leafChar, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector rule_ریشه_Nodes(*fStatus);
    add_Rule_ریشه_Nodes(&rule_ریشه_Nodes, درخت);

    UVector matchStartNodes(*fStatus);
    برای (صحیح j=0; j<rule_ریشه_Nodes.size(); ++j) {
        RBBINode *گره = static_cast<RBBINode *>(rule_ریشه_Nodes.elementAt(j));
        اگر (گره->fChainIn) {
            setAdd(&matchStartNodes, گره->fFirstPosSet);
        }
    }
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }

    int32_t  end_گره_Ix;
    int32_t  start_گره_Ix;

    برای (end_گره_Ix=0; end_گره_Ix<leafNodes.size(); end_گره_Ix++) {
        RBBINode* end_گره = static_cast<RBBINode*>(leafNodes.elementAt(end_گره_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        اگر (!end_گره->fFollowPos->contains(end_Mark_گره)) {
            ادامه;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *start_گره;
        برای (start_گره_Ix = 0; start_گره_Ix<matchStartNodes.size(); start_گره_Ix++) {
            start_گره = static_cast<RBBINode*>(matchStartNodes.elementAt(start_گره_Ix));
            اگر (start_گره->fType != RBBINode::leafChar) {
                ادامه;
            }

            اگر (end_گره->fVal == start_گره->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                setAdd(end_گره->fFollowPos, start_گره->fFollowPos);
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
تهی RBBITableBuilder::bofFixup() {

    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
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
    RBBINode  *bof_گره = f_درخت->f_چپ_Child->f_چپ_Child;
    U_ASSERT(bof_گره->fType == RBBINode::leafChar);
    U_ASSERT(bof_گره->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *matchStartNodes = f_درخت->f_چپ_Child->f_راست_Child->fFirstPosSet;

    RBBINode *start_گره;
    صحیح       start_گره_Ix;
    برای (start_گره_Ix = 0; start_گره_Ix<matchStartNodes->size(); start_گره_Ix++) {
        start_گره = static_cast<RBBINode*>(matchStartNodes->elementAt(start_گره_Ix));
        اگر (start_گره->fType != RBBINode::leafChar) {
            ادامه;
        }

        اگر (start_گره->fVal == bof_گره->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            setAdd(bof_گره->fFollowPos, start_گره->fFollowPos);
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
تهی RBBITableBuilder::buildStateTable() {
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = nullptr;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    صحیح      last_درون_داد_نماد = fRB->fSetBuilder->get_رقم_Char_Categories() - 1;
    failState = جدید RBBIStateDescriptor(last_درون_داد_نماد, fStatus);
    اگر (failState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        برو ExitBuildSTdeleteall;
    }
    failState->fPositions = جدید UVector(*fStatus);
    اگر (failState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    اگر (failState->fPositions == nullptr || U_FAILURE(*fStatus)) {
        برو ExitBuildSTdeleteall;
    }
    fDStates->addElement(failState, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        برو ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = جدید RBBIStateDescriptor(last_درون_داد_نماد, fStatus);
    اگر (initialState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    اگر (U_FAILURE(*fStatus)) {
        برو ExitBuildSTdeleteall;
    }
    initialState->fPositions = جدید UVector(*fStatus);
    اگر (initialState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    اگر (U_FAILURE(*fStatus)) {
        برو ExitBuildSTdeleteall;
    }
    setAdd(initialState->fPositions, f_درخت->fFirstPosSet);
    fDStates->addElement(initialState, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        برو ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    برای (;;) {
        RBBIStateDescriptor *T = nullptr;
        int32_t              tx;
        برای (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *temp;
            temp = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            اگر (temp->fMarked == نادرست) {
                T = temp;
                بشکن;
            }
        }
        اگر (T == nullptr) {
            بشکن;
        }

        // mark T;
        T->fMarked = درست;

        // for each input symbol a do begin
        int32_t  a;
        برای (a = 1; a<=last_درون_داد_نماد; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = nullptr;
            RBBINode   *p;
            int32_t     px;
            برای (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                اگر ((p->fType == RBBINode::leafChar) &&  (p->fVal == a)) {
                    اگر (U == nullptr) {
                        U = جدید UVector(*fStatus);
                        اگر (U == nullptr) {
                        	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                        	برو ExitBuildSTdeleteall;
                        }
                    }
                    setAdd(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = نادرست;
            اگر (U != nullptr) {
                U_ASSERT(U->size() > 0);
                صحیح  ix;
                برای (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    اگر (setEquals(U, temp2->fPositions)) {
                        delete U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = درست;
                        بشکن;
                    }
                }

                // Add U as an unmarked state to Dstates
                اگر (!UinDstates)
                {
                    RBBIStateDescriptor *newState = جدید RBBIStateDescriptor(last_درون_داد_نماد, fStatus);
                    اگر (newState == nullptr) {
                    	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                    }
                    اگر (U_FAILURE(*fStatus)) {
                        برو ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->addElement(newState, *fStatus);
                    اگر (U_FAILURE(*fStatus)) {
                        بازگشت;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->setElementAt(ux, a);
            }
        }
    }
    بازگشت;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    delete initialState;
    delete failState;
}


/**
 * mapLookAheadRules
 *
 */
تهی RBBITableBuilder::mapLookAheadRules() {
    fLookAheadRuleMap =  جدید UVector32(fRB->fScanner->رقم_Rules() + 1, *fStatus);
    اگر (fLookAheadRuleMap == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    fLookAheadRuleMap->setSize(fRB->fScanner->رقم_Rules() + 1);

    برای (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t laSlotForState = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        منطقی saw_Look_Ahead_گره = نادرست;
        برای (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *گره = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            اگر (گره->fType != RBBINode::گره_Type::lookAhead) {
                ادامه;
            }
            saw_Look_Ahead_گره = درست;
            int32_t rule_رقم = گره->fVal;     // Set when rule was originally parsed.
            U_ASSERT(rule_رقم < fLookAheadRuleMap->size());
            U_ASSERT(rule_رقم > 0);
            int32_t laSlot = fLookAheadRuleMap->elementAti(rule_رقم);
            اگر (laSlot != 0) {
                اگر (laSlotForState == 0) {
                    laSlotForState = laSlot;
                } وگرنه {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(laSlot == laSlotForState);
                }
            }
        }
        اگر (!saw_Look_Ahead_گره) {
            ادامه;
        }

        اگر (laSlotForState == 0) {
            laSlotForState = ++fLASlotsInUse;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        برای (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *گره = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            اگر (گره->fType != RBBINode::گره_Type::lookAhead) {
                ادامه;
            }
            int32_t rule_رقم = گره->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = fLookAheadRuleMap->elementAti(rule_رقم);
            (تهی)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == laSlotForState);
            fLookAheadRuleMap->setElementAt(laSlotForState, rule_رقم);
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
تهی     RBBITableBuilder::flagAcceptingStates() {
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    UVector     endMarkerNodes(*fStatus);
    RBBINode    *endMarker;
    int32_t     i;
    int32_t     n;

    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }

    f_درخت->findNodes(&endMarkerNodes, RBBINode::endMark, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }

    برای (i=0; i<endMarkerNodes.size(); i++) {
        endMarker = static_cast<RBBINode*>(endMarkerNodes.elementAt(i));
        برای (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            اگر (sd->fPositions->indexOf(endMarker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                اگر (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = fLookAheadRuleMap->elementAti(endMarker->fVal);
                    اگر (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                اگر (sd->fAccepting==ACCEPTING_UNCONDITIONAL && endMarker->fVal != 0) {
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
تهی     RBBITableBuilder::flagLookAheadStates() {
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    UVector     lookAheadNodes(*fStatus);
    RBBINode    *look_Ahead_گره;
    int32_t     i;
    int32_t     n;

    f_درخت->findNodes(&lookAheadNodes, RBBINode::lookAhead, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    برای (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_گره = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_گره->fType == RBBINode::گره_Type::lookAhead);

        برای (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->indexOf(look_Ahead_گره);
            اگر (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_گره == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookaheadSlot = fLookAheadRuleMap->elementAti(look_Ahead_گره->fVal);
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
تهی     RBBITableBuilder::flagTaggedStates() {
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    UVector     tagNodes(*fStatus);
    RBBINode    *tag_گره;
    int32_t     i;
    int32_t     n;

    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    f_درخت->findNodes(&tagNodes, RBBINode::tag, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    برای (i=0; i<tagNodes.size(); i++) {                   // For each tag node t (all of 'em)
        tag_گره = static_cast<RBBINode*>(tagNodes.elementAt(i));

        برای (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            اگر (sd->fPositions->indexOf(tag_گره) >= 0) {  //       if  s include the tag node t
                sortedAdd(&sd->fTagVals, tag_گره->fVal);
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
تهی  RBBITableBuilder::mergeRuleStatusVals() {
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
    صحیح i;
    صحیح n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    اگر (fRB->fRuleStatusVals->size() == 0) {
        fRB->fRuleStatusVals->addElement(1, *fStatus);  // Num of statuses in group
        fRB->fRuleStatusVals->addElement(static_cast<int32_t>(0), *fStatus); // and our single status of zero
    }

    //    For each state
    برای (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_States_Tag_مقادیر = sd->fTagVals;
        اگر (this_States_Tag_مقادیر == nullptr) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->fTagsIdx = 0;
            ادامه;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->fTagsIdx = -1;
        int32_t  thisTagGroupStart = 0;   // indexes into the global rule status vals list
        int32_t  nextTagGroupStart = 0;

        // Loop runs once per group of tags in the global list
        وقتی (nextTagGroupStart < fRB->fRuleStatusVals->size()) {
            thisTagGroupStart = nextTagGroupStart;
            nextTagGroupStart += fRB->fRuleStatusVals->elementAti(thisTagGroupStart) + 1;
            اگر (this_States_Tag_مقادیر->size() != fRB->fRuleStatusVals->elementAti(thisTagGroupStart)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                ادامه;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            برای (i=0; i<this_States_Tag_مقادیر->size(); i++) {
                اگر (this_States_Tag_مقادیر->elementAti(i) !=
                    fRB->fRuleStatusVals->elementAti(thisTagGroupStart + 1 + i) ) {
                    // Mismatch.
                    بشکن;
                }
            }

            اگر (i == this_States_Tag_مقادیر->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->fTagsIdx = thisTagGroupStart;
                بشکن;
            }
        }

        اگر (sd->fTagsIdx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->fTagsIdx = fRB->fRuleStatusVals->size();
            fRB->fRuleStatusVals->addElement(this_States_Tag_مقادیر->size(), *fStatus);
            برای (i=0; i<this_States_Tag_مقادیر->size(); i++) {
                fRB->fRuleStatusVals->addElement(this_States_Tag_مقادیر->elementAti(i), *fStatus);
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

تهی RBBITableBuilder::minimizeStates() {
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }

#اگر UPRV_HAS_SANITIZER
    اگر (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *fStatus = U_REGEX_PATTERN_TOO_BIG;
        بازگشت;
    }
#endif
    ساختار StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t fTagsIdx;
        منطقی operator==(StateType ثابت &other) ثابت {
            بازگشت fAccepting == other.fAccepting &&
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
    ساختار TypeToStates : UMemory {
        TypeToStates(ثابت StateType &type, UErrorCode &status)
            : type(type), states(prv::make_unique<UVector>(status, status)) {}
        StateType type;
        prv::unique_ptr<UVector> states;
    };
    UVector initialPartition(*fStatus);
    initialPartition.setDeleter(
        [](تهی *p) { delete static_cast<TypeToStates *>(p); });
    برای (int32_t i = 0; i < fDStates->size(); ++i) {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ثابت StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.fTagsIdx};
        int32_t j = 0;
        برای (; j < initialPartition.size(); ++j) {
            auto &[type_j, states] =
                *static_cast<TypeToStates *>(initialPartition.elementAt(j));
            اگر (type_j == type) {
                states->addElement(i, *fStatus);
                بشکن;
            }
        }
        اگر (j == initialPartition.size()) {
            auto newEntry = prv::make_unique<TypeToStates>(type, *fStatus, *fStatus);
            اگر (U_FAILURE(*fStatus)) {
                بازگشت;
            }
            newEntry->states->addElement(i, *fStatus);
            initialPartition.adoptElement(newEntry.release(), *fStatus);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    auto partition = prv::make_unique<UVector>(*fStatus, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    partition->setDeleter(
        [](تهی *p) { delete static_cast<UVector*>(p); });
    
    برای (int32_t i = 0; i < initialPartition.size(); ++i) {
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
    auto partition_signature = [&partition, این](int32_t stateIndex) -> prv::unique_ptr<UVector> {
        ثابت RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(stateIndex));
        auto result = prv::make_unique<UVector>(state.fDtran->size(), *fStatus, *fStatus);
        اگر (U_FAILURE(*fStatus)) {
            بازگشت nullptr;
        }
        برای (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t toState = state.fDtran->elementAti(i);
            برای (int32_t partIndex = 0; partIndex < partition->size(); ++partIndex) {
                ثابت UVector &part = *static_cast<UVector *>(partition->elementAt(partIndex));
                اگر (part.contains(toState)) {
                    result->addElement(partIndex, *fStatus);
                    بشکن;
                }
            }
        }
        result->setComparer([](ثابت UElement left, ثابت UElement راست) -> UBool {
            بازگشت left.integer == راست.integer;
        });
        بازگشت result;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    برای (;;) {
        // Π_new.
        auto partitionNew = prv::make_unique<UVector>(*fStatus, *fStatus);
        اگر (U_FAILURE(*fStatus)) {
            بازگشت;
        }
        partitionNew->setDeleter(
            [](تهی *p) { delete static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        منطقی refined = نادرست;
        برای (int32_t i = 0; i < partition->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ثابت UVector &group = *static_cast<UVector*>(partition->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            ساختار SignatureToStates : UMemory {
                SignatureToStates(prv::unique_ptr<UVector> signature, UErrorCode &status)
                    : signature(std::move(signature)), states(prv::make_unique<UVector>(status, status)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> states;
            };
            UVector groupRefinement(*fStatus);
            groupRefinement.setDeleter([](تهی *p) { delete static_cast<SignatureToStates*>(p); });
            برای (int32_t j = 0; j < group.size(); ++j) {
                // Index of a state in the group.
                ثابت int32_t s = group.elementAti(j);
                auto signature = partition_signature(s);
                int32_t k = 0;
                برای (; k < groupRefinement.size(); ++k) {
                    ثابت auto &[subgroupSignature, subgroup] =
                        *static_cast<SignatureToStates*>(groupRefinement.elementAt(k));
                    اگر (*subgroupSignature == *signature) {
                        subgroup->addElement(s, *fStatus);
                        بشکن;
                    }
                }
                اگر (k == groupRefinement.size()) {
                    ثابت auto newEntry =
                        جدید SignatureToStates(std::move(signature), *fStatus);
                    اگر (U_FAILURE(*fStatus)) {
                        بازگشت;
                    }
                    اگر (newEntry == nullptr) {
                      *fStatus = U_MEMORY_ALLOCATION_ERROR;
                      بازگشت;
                    }
                    newEntry->states->addElement(s, *fStatus);
                    groupRefinement.adoptElement(newEntry, *fStatus);
                }
            }
            refined |= groupRefinement.size() > 1;
            برای (int32_t j = 0; j < groupRefinement.size(); ++j) {
                auto &[_, subgroup] = *static_cast<SignatureToStates*>(
                    groupRefinement.elementAt(j));
                partitionNew->adoptElement(subgroup.release(), *fStatus);
            }
        }
        اگر (refined) {
            partition = std::move(partitionNew);
        } وگرنه {
            بشکن;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    برای (صحیح i = 0; i < partition->size(); ++i) {
        UVector *ثابت part_i = static_cast<UVector *>(partition->elementAt(i));
        اگر (part_i->contains(0)) {
            تهی *ثابت part_0 = partition->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ثابت auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 0);
            partition->setElementAt(part_0, i);
            partition->setDeleter(deleter);
        } وگرنه اگر (part_i->contains(1)) {
            تهی *ثابت part_1 = partition->elementAt(1);
            ثابت auto deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 1);
            partition->setElementAt(part_1, i);
            partition->setDeleter(deleter);
        }
    }
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    auto oldStateToPart = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    برای (صحیح i = 0; i < partition->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        برای (صحیح j = 0; j < part.size(); ++j) {
            oldStateToPart[part.elementAti(j)] = i;
        }
    }
    auto ثابت oldStates = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*fStatus, *fStatus).release();
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    برای (صحیح i = 0; i < partition->size(); ++i) {
        ثابت UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        RBBIStateDescriptor *ثابت state =
            static_cast<RBBIStateDescriptor*>(oldStates->elementAt(part.elementAti(0)));
        fDStates->addElement(state, *fStatus);
        برای (صحیح j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->setElementAt(oldStateToPart[state->fDtran->elementAti(j)], j);
        }
        oldStates->setElementAt(nullptr, part.elementAti(0));
    }
    oldStates->setDeleter([](تهی *p) { delete static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
تهی RBBITableBuilder::sortedAdd(UVector **vector, int32_t val) {
    int32_t i;

    اگر (*vector == nullptr) {
        *vector = جدید UVector(*fStatus);
    }
    اگر (*vector == nullptr || U_FAILURE(*fStatus)) {
        بازگشت;
    }
    UVector *vec = *vector;
    int32_t  vSize = vec->size();
    برای (i=0; i<vSize; i++) {
        int32_t valAtI = vec->elementAti(i);
        اگر (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            بازگشت;
        }
        اگر (valAtI > val) {
            بشکن;
        }
    }
    vec->درج_Element_At(val, i, *fStatus);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
تهی RBBITableBuilder::setAdd(UVector *dest, UVector *source) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!source->hasDeleter());
    int32_t destOriginalSize = dest->size();
    int32_t sourceSize       = source->size();
    int32_t di           = 0;
    Maybe_پشته_Array<تهی *, 16> destArray, sourceArray;  // Handle small cases without malloc
    تهی **destPtr, **sourcePtr;
    تهی **destLim, **sourceLim;

    اگر (destOriginalSize > destArray.getCapacity()) {
        اگر (destArray.resize(destOriginalSize) == nullptr) {
            بازگشت;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + destOriginalSize;  // destArray.getArrayLimit()?

    اگر (sourceSize > sourceArray.getCapacity()) {
        اگر (sourceArray.resize(sourceSize) == nullptr) {
            بازگشت;
        }
    }
    sourcePtr = sourceArray.getAlias();
    sourceLim = sourcePtr + sourceSize;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (تهی) dest->toArray(destPtr);
    (تهی) source->toArray(sourcePtr);

    dest->setSize(sourceSize+destOriginalSize, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }

    وقتی (sourcePtr < sourceLim && destPtr < destLim) {
        اگر (*destPtr == *sourcePtr) {
            dest->setElementAt(*sourcePtr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        وگرنه اگر (uprv_memcmp(destPtr, sourcePtr, اندازه(تهی *)) < 0) {
            dest->setElementAt(*destPtr++, di++);
        }
        وگرنه { /* *sourcePtr < *destPtr */
            dest->setElementAt(*sourcePtr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    وقتی (destPtr < destLim) {
        dest->setElementAt(*destPtr++, di++);
    }
    وقتی (sourcePtr < sourceLim) {
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
    بازگشت a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#ifdef RBBI_DEBUG
تهی RBBITableBuilder::printPosSets(RBBINode *n) {
    اگر (n==nullptr) {
        بازگشت;
    }
    printf("\n");
    RBBINode::print_گره_Header();
    RBBINode::print_گره(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    printSet(n->fFirstPosSet);

    RBBIDebugPrintf("         lastpos:   ");
    printSet(n->fLastPosSet);

    RBBIDebugPrintf("         followpos: ");
    printSet(n->fFollowPos);

    printPosSets(n->f_چپ_Child);
    printPosSets(n->f_راست_Child);
}
#endif

//
//    findDuplCharClassFrom()
//
منطقی RBBITableBuilder::findDuplCharClassFrom(IntPair *categories) {
    int32_t رقم_States = fDStates->size();
    int32_t رقم_Cols = fRB->fSetBuilder->get_رقم_Char_Categories();

    برای (; categories->first < رقم_Cols-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        صحیح limitSecond = categories->first < fRB->fSetBuilder->getDictCategoriesStart() ?
            fRB->fSetBuilder->getDictCategoriesStart() : رقم_Cols;
        برای (categories->second=categories->first+1; categories->second < limitSecond; categories->second++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t table_base = 0;
            uint16_t table_dupl = 1;
            برای (int32_t state=0; state<رقم_States; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                table_base = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                table_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->second));
                اگر (table_base != table_dupl) {
                    بشکن;
                }
            }
            اگر (table_base == table_dupl) {
                بازگشت درست;
            }
        }
    }
    بازگشت نادرست;
}


//
//    removeColumn()
//
تهی RBBITableBuilder::removeColumn(int32_t column) {
    int32_t رقم_States = fDStates->size();
    برای (int32_t state=0; state<رقم_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(column < sd->fDtran->size());
        sd->fDtran->removeElementAt(column);
    }
}


منطقی RBBITableBuilder::findDuplicateSafeState(IntPair *states) {
    int32_t رقم_States = fSafeTable->size();

    برای (; states->first<رقم_States-1; states->first++) {
        UnicodeString *firstRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->first));
        برای (states->second=states->first+1; states->second<رقم_States; states->second++) {
            UnicodeString *duplRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->second));
            منطقی rowsMatch = درست;
            int32_t رقم_Cols = firstRow->length();
            برای (int32_t col=0; col < رقم_Cols; ++col) {
                int32_t firstVal = firstRow->charAt(col);
                int32_t duplVal = duplRow->charAt(col);
                اگر (!((firstVal == duplVal) ||
                        ((firstVal == states->first || firstVal == states->second) &&
                        (duplVal  == states->first || duplVal  == states->second)))) {
                    rowsMatch = نادرست;
                    بشکن;
                }
            }
            اگر (rowsMatch) {
                بازگشت درست;
            }
        }
    }
    بازگشت نادرست;
}

تهی RBBITableBuilder::removeSafeState(IntPair duplStates) {
    ثابت int32_t keepState = duplStates.first;
    ثابت int32_t duplState = duplStates.second;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < fSafeTable->size());

    fSafeTable->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t رقم_States = fSafeTable->size();
    برای (int32_t state=0; state<رقم_States; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        int32_t رقم_Cols = sd->length();
        برای (int32_t col=0; col<رقم_Cols; col++) {
            int32_t existingVal = sd->charAt(col);
            int32_t newVal = existingVal;
            اگر (existingVal == duplState) {
                newVal = keepState;
            } وگرنه اگر (existingVal > duplState) {
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
    ثابت int32_t old_State_شمار = fDStates->size();
    minimizeStates();
    بازگشت fDStates->size() - old_State_شمار;
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
تهی RBBITableBuilder::minimizeLookaheads() {
    اگر (fLASlotsInUse == ACCEPTING_UNCONDITIONAL) {
        بازگشت;
    }
    ثابت int32_t lookahead_شمار = fLASlotsInUse - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ثابت int32_t firstLookahead = ACCEPTING_UNCONDITIONAL + 1;
    ثابت auto stateDescriptor = [این](ثابت int32_t state) -> RBBIStateDescriptor & {
        بازگشت *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ثابت auto lookaheadReachability =
        prv::make_unique<منطقی[]>(lookahead_شمار * lookahead_شمار, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    برای (int32_t l = firstLookahead; l <= fLASlotsInUse; ++l) {
        برای (int32_t k = firstLookahead; k <= fLASlotsInUse; ++k) {
            اگر (k != l) {
                برای (int32_t source = 1; source < fDStates->size(); ++source) {
                    اگر (static_cast<int32_t>(stateDescriptor(source).fLookAhead) != l) {
                        ادامه;
                    }
                    اگر (reachableByTransitions(
                            *fDStates, source, /*isSink=*/
                            [&](ثابت int32_t state) {
                                بازگشت static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](ثابت int32_t state) {
                                بازگشت static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *fStatus)) {
                        lookaheadReachability[(l - firstLookahead) * lookahead_شمار + k -
                                              firstLookahead] = درست;
                    }
                }
            }
        }
    }

    ثابت auto colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_شمار, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    برای (int32_t chromatic_عدد = 1;; ++chromatic_عدد) {
        uprv_memset(colours.get(), 0, اندازه(int32_t) * lookahead_شمار);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        برای (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            برای (صحیح source = 0; source < lookahead_شمار; ++source) {
                برای (صحیح sink = 0; sink < lookahead_شمار; ++sink) {
                    اگر (lookaheadReachability[source * lookahead_شمار + sink] &&
                        colours[source] == colours[sink]) {
                        برو nextColouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            برای (صحیح i = 0; i < fDStates->size(); ++i) {
                اگر (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        firstLookahead +
                        colours[stateDescriptor(i).fAccepting - firstLookahead];
                }
                اگر (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - firstLookahead];
                }
            }
            fLASlotsInUse = ACCEPTING_UNCONDITIONAL + chromatic_عدد;
            بازگشت;

          nextColouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            برای (int32_t i = 0; i < lookahead_شمار - 1 && colours[i] == chromatic_عدد;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            اگر (colours[lookahead_شمار - 1] == chromatic_عدد) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                بشکن;
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
    int32_t    رقم_Rows;
    int32_t    رقم_Cols;
    int32_t    rowSize;

    اگر (f_درخت == nullptr) {
        بازگشت 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    رقم_Rows = fDStates->size();
    رقم_Cols = fRB->fSetBuilder->get_رقم_Char_Categories();

    اگر (use8BitsForTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + اندازه(int8_t)*رقم_Cols;
    } وگرنه {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + اندازه(int16_t)*رقم_Cols;
    }
    size   += رقم_Rows * rowSize;
    بازگشت size;
}

منطقی RBBITableBuilder::use8BitsForTable() ثابت {
    بازگشت fDStates->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
تهی RBBITableBuilder::exportTable(تهی *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    صحیح                col;

    اگر (U_FAILURE(*fStatus) || f_درخت == nullptr) {
        بازگشت;
    }

    int32_t cat_شمار = fRB->fSetBuilder->get_رقم_Char_Categories();
    اگر (cat_شمار > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        بازگشت;
    }

    table->f_رقم_States = fDStates->size();
    table->fDictCategoriesStart = fRB->fSetBuilder->getDictCategoriesStart();
    table->fLookAheadResultsSize = fLASlotsInUse == ACCEPTING_UNCONDITIONAL ? 0 : fLASlotsInUse + 1;
    table->fFlags     = 0;
    اگر (use8BitsForTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + اندازه(uint8_t) * cat_شمار;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } وگرنه {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + اندازه(int16_t) * cat_شمار;
    }
    اگر (fRB->fLookAheadHardBreak) {
        table->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    برای (state=0; state<table->f_رقم_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        اگر (use8BitsForTable()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 255);
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->fTagsIdx   = sd->fTagsIdx;
            برای (col=0; col<cat_شمار; col++) {
                U_ASSERT (sd->fDtran->elementAti(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = sd->fDtran->elementAti(col);
            }
        } وگرنه {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 0xffff);
            row->r16.fAccepting = sd->fAccepting;
            row->r16.fLookAhead = sd->fLookAhead;
            row->r16.fTagsIdx   = sd->fTagsIdx;
            برای (col=0; col<cat_شمار; col++) {
                row->r16.fNextState[col] = sd->fDtran->elementAti(col);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
تهی RBBITableBuilder::buildSafeReverseTable(UErrorCode &status) {
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

    int32_t رقم_Char_Classes = fRB->fSetBuilder->get_رقم_Char_Categories();
    int32_t رقم_States = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ثابت auto lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    ثابت auto wantedLookaheadsInPair =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t START_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_SET = -1;
    برای (int32_t c1 = 1; c1 < رقم_Char_Classes; ++c1) {
        اگر (fRB->fSetBuilder->getFirstChar(c1) < 0) {
          ادامه;
        }
        برای (int32_t c2 = 1; c2 < رقم_Char_Classes; ++c2) {
            اگر (fRB->fSetBuilder->getFirstChar(c2) < 0) {
                ادامه;
            }
            int32_t wantedEndState = -1;
            int32_t wantedLastAcceptingPosition;
            منطقی mustCheckConsistentWithStartC2 = نادرست;
            ثابت int32_t text[2] = {c1, c2};
            برای (int32_t injectedState = START_STATE; injectedState < رقم_States; ++injectedState) {
                برای (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= fLASlotsInUse; ++injectedLookahead) {
                    int32_t lastBreak = BEFORE_PAIR;
                    int32_t lastAcceptingPosition = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                    اگر (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ثابت RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        اگر (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            lastAcceptingPosition = 0;
                        } وگرنه اگر (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            ادامه;
                        }
                        اگر (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    برو dfaStep;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    برای (;;) {
                        s = START_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                        i = lastBreak;
                        // Loop Find_Next_Break in L2/26-135.
                        برای (;;) {
                        dfaStep:
                            اگر (i == BEFORE_PAIR || i == 2) {
                                برو inspectFinalState;
                            }
                            ثابت int32_t نماد_Ahead = text[i];
                            ++i;
                            ثابت RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            اگر (sDescriptor->fDtran->elementAti(نماد_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(نماد_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } وگرنه {
                                اگر (lastBreak != BEFORE_PAIR &&
                                    lastAcceptingPosition == lastBreak) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    برو next_نماد_Pair;
                                }
                                lastBreak = lastAcceptingPosition;
                                بشکن;
                            }

                            اگر (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                lastAcceptingPosition = i;
                            } وگرنه اگر (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_SET) {
                                lastBreak = lookaheadPositions[sDescriptor->fAccepting];
                                بشکن;
                            }
                            اگر (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    اگر (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        ادامه;
                    }
                    اگر (wantedEndState < 0) {
                        wantedEndState = s;
                        wantedLastAcceptingPosition = lastAcceptingPosition;
                        برای (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                            اگر (lookaheadPositions[l] != LOOKAHEAD_NOT_SET &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wantedLookaheadsInPair[l] = lookaheadPositions[i];
                            } وگرنه {
                                wantedLookaheadsInPair[l] = LOOKAHEAD_NOT_SET;
                            }
                        }
                    } وگرنه {
                        اگر (wantedEndState != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            برو next_نماد_Pair;
                        }
                        اگر (wantedLastAcceptingPosition != lastAcceptingPosition) {
                            برو next_نماد_Pair;
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
                    برای (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                        اگر (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wantedLookaheadsInPair[l]) {
                            ادامه;
                        }
                        اگر (lookaheadPositions[l] == 2 || wantedLookaheadsInPair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            اگر (wantedEndState != START_STATE) {
                                برو next_نماد_Pair;
                            }
                        }
                        اگر (lookaheadPositions[l] == 1 || wantedLookaheadsInPair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            mustCheckConsistentWithStartC2 = درست;
                        }
                    }
                }
            }
            اگر (mustCheckConsistentWithStartC2) {
                ثابت RBBIStateDescriptor& startDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(START_STATE));
                ثابت auto c2State = startDescriptor.fDtran->elementAti(c2);
                اگر (c2State != wantedEndState) {
                    برو next_نماد_Pair;
                }
                ثابت RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                اگر (wantedLastAcceptingPosition != 2) {
                    برو next_نماد_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                برای (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                    اگر (((wantedLookaheadsInPair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wantedEndState != START_STATE) {
                        برو next_نماد_Pair;
                    }
                }
            }
            safePairs.append(static_cast<char16_t>(c1));
            safePairs.append(static_cast<char16_t>(c2));
        next_نماد_Pair:
            ادامه;
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
        جدید UVector(uprv_deleteUObject, uhash_compareUnicodeString, رقم_Char_Classes + 2, status), status);
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    fSafeTable = lpSafeTable.orphan();
    برای (int32_t row=0; row<رقم_Char_Classes + 2; ++row) {
        LocalPointer<UnicodeString> lpString(جدید UnicodeString(رقم_Char_Classes, 0, رقم_Char_Classes+4), status);
        fSafeTable->adoptElement(lpString.orphan(), status);
    }
    اگر (U_FAILURE(status)) {
        بازگشت;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &startState = *static_cast<UnicodeString *>(fSafeTable->elementAt(1));
    برای (int32_t charClass=0; charClass < رقم_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        startState.setCharAt(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    برای (int32_t row=2; row<رقم_Char_Classes+2; ++row) {
        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(row));
        rowState = startState;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    برای (int32_t pairIdx=0; pairIdx<safePairs.length(); pairIdx+=2) {
        int32_t c1 = safePairs.charAt(pairIdx);
        int32_t c2 = safePairs.charAt(pairIdx + 1);

        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(c2 + 2));
        rowState.setCharAt(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair states = {1, 0};
    وقتی (findDuplicateSafeState(&states)) {
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
    int32_t    رقم_Rows;
    int32_t    رقم_Cols;
    int32_t    rowSize;

    اگر (fSafeTable == nullptr) {
        بازگشت 0;
    }

    size    = offsetof(RBBIStateTable, fTableData);    // The header, with no rows to the table.

    رقم_Rows = fSafeTable->size();
    رقم_Cols = fRB->fSetBuilder->get_رقم_Char_Categories();

    اگر (use8BitsForSafeTable()) {
        rowSize = offsetof(RBBIStateTableRow8, fNextState) + اندازه(int8_t)*رقم_Cols;
    } وگرنه {
        rowSize = offsetof(RBBIStateTableRow16, fNextState) + اندازه(int16_t)*رقم_Cols;
    }
    size   += رقم_Rows * rowSize;
    بازگشت size;
}

منطقی RBBITableBuilder::use8BitsForSafeTable() ثابت {
    بازگشت fSafeTable->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
تهی RBBITableBuilder::exportSafeTable(تهی *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    صحیح                col;

    اگر (U_FAILURE(*fStatus) || fSafeTable == nullptr) {
        بازگشت;
    }

    int32_t cat_شمار = fRB->fSetBuilder->get_رقم_Char_Categories();
    اگر (cat_شمار > 0x7fff ||
            fSafeTable->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        بازگشت;
    }

    table->f_رقم_States = fSafeTable->size();
    table->fFlags     = 0;
    اگر (use8BitsForSafeTable()) {
        table->fRowLen    = offsetof(RBBIStateTableRow8, fNextState) + اندازه(uint8_t) * cat_شمار;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } وگرنه {
        table->fRowLen    = offsetof(RBBIStateTableRow16, fNextState) + اندازه(int16_t) * cat_شمار;
    }

    برای (state=0; state<table->f_رقم_States; state++) {
        UnicodeString* rowString = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->fTableData + state * table->fRowLen);
        اگر (use8BitsForSafeTable()) {
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->fTagsIdx    = 0;
            برای (col=0; col<cat_شمار; col++) {
                U_ASSERT(rowString->charAt(col) <= kMaxStateFor8BitsTable);
                r8->fNextState[col] = static_cast<uint8_t>(rowString->charAt(col));
            }
        } وگرنه {
            row->r16.fAccepting = 0;
            row->r16.fLookAhead = 0;
            row->r16.fTagsIdx    = 0;
            برای (col=0; col<cat_شمار; col++) {
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
تهی RBBITableBuilder::printSet(UVector *s) {
    int32_t  i;
    برای (i=0; i<s->size(); i++) {
        ثابت RBBINode *v = static_cast<ثابت RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==nullptr? -1 : v->f_Serial_رقم);
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
تهی RBBITableBuilder::printStates() {
    صحیح     c;    // input "character"
    صحیح     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    برای (c=0; c<fRB->fSetBuilder->get_رقم_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    برای (c=0; c<fRB->fSetBuilder->get_رقم_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    برای (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->fTagsIdx);
        برای (c=0; c<fRB->fSetBuilder->get_رقم_Char_Categories(); c++) {
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
تهی RBBITableBuilder::printReverseTable() {
    صحیح     c;    // input "character"
    صحیح     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    اگر (fSafeTable == nullptr) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        بازگشت;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    برای (c=0; c<fRB->fSetBuilder->get_رقم_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    برای (c=0; c<fRB->fSetBuilder->get_رقم_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    برای (n=0; n<fSafeTable->size(); n++) {
        UnicodeString *rowString = (UnicodeString *)fSafeTable->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        برای (c=0; c<fRB->fSetBuilder->get_رقم_Char_Categories(); c++) {
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
تهی RBBITableBuilder::printRuleStatusTable() {
    int32_t  thisRecord = 0;
    int32_t  nextRecord = 0;
    صحیح      i;
    UVector  *tbl = fRB->fRuleStatusVals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    وقتی (nextRecord < tbl->size()) {
        thisRecord = nextRecord;
        nextRecord = thisRecord + tbl->elementAti(thisRecord) + 1;
        RBBIDebugPrintf("%4d   ", thisRecord);
        برای (i=thisRecord+1; i<nextRecord; i++) {
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

RBBIStateDescriptor::RBBIStateDescriptor(صحیح last_درون_داد_نماد, UErrorCode *fStatus) {
    fMarked    = نادرست;
    fAccepting = 0;
    fLookAhead = 0;
    fTagsIdx   = 0;
    fTagVals   = nullptr;
    fPositions = nullptr;
    fDtran     = nullptr;

    fDtran     = جدید UVector32(last_درون_داد_نماد+1, *fStatus);
    اگر (U_FAILURE(*fStatus)) {
        بازگشت;
    }
    اگر (fDtran == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    fDtran->setSize(last_درون_داد_نماد+1);    // fDtran needs to be pre-sized.
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
