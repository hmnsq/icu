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

#समावेश <functional>

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "unicode/unistr.h"
#समावेश "rbbitblb.h"
#समावेश "rbbirb.h"
#समावेश "rbbiscan.h"
#समावेश "rbbisetb.h"
#समावेश "rbbidata.h"
#समावेश "cstring.h"
#समावेश "uassert.h"
#समावेश "uvectr32.h"
#समावेश "cmemory.h"

U_NAMESPACE_BEGIN

namespace {

// Given the `RBBITableBuilder::fDStates` vector of `RBBIStateDescriptor`s, returns
// true if a state for which `isSink` returns true is reachable from state `source` by following
// transitions without going through any state for which `excludedState` returns true.
// Note that “going through” is distinct from “reaching”, and means entering and then leaving: A
// state can be both a sink and excluded, and can still be reachable.
bool reachableByTransitions(स्थिर UVector &states, स्थिर int32_t source,
                            स्थिर std::function<bool(int32_t)> isSink,
                            स्थिर std::function<bool(int32_t)> excludedState, UErrorCode &status) {
    UStack boundary(status);
    {
        UVector32 &transitionsFromSource =
            *static_cast<RBBIStateDescriptor *>(states.elementAt(source))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        क्रम (int32_t प्रतीक = 0; प्रतीक < transitionsFromSource.size(); ++प्रतीक) {
            स्थिर int32_t state = transitionsFromSource.elementAti(प्रतीक);
            अगर (state != 0) {
                boundary.push(state, status);
            }
        }
    }
    स्थिर स्वतः visited = prv::make_unique<bool[]>(states.size(), status);
    जबतक (U_SUCCESS(status) && !boundary.empty()) {
        स्थिर int32_t s = boundary.popi();
        अगर (isSink(s)) {
            वापस true;
        }
        अगर (excludedState(s) || visited[s]) {
            जारी;
        }
        visited[s] = true;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(states.elementAt(s))->fDtran;
        क्रम (int32_t प्रतीक = 0; प्रतीक < transitions.size(); ++प्रतीक) {
            स्थिर int32_t t = transitions.elementAti(प्रतीक);
            अगर (t != 0 && !visited[t]) {
                boundary.push(t, status);
            }
        }
    }
    वापस false;
}

} // namespace

स्थिर int32_t kMaxStateFor8BitsTable = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **मूल_गाँठ, UErrorCode &status) :
        fRB(rb),
        f_वृक्ष(*मूल_गाँठ),
        fStatus(&status),
        fDStates(nullptr),
        fSafeTable(nullptr) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = नया UVector(status);
    अगर (U_SUCCESS(status) && fDStates == nullptr ) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    पूर्णांक i;
    क्रम (i=0; i<fDStates->size(); i++) {
        मिटाओ static_cast<RBBIStateDescriptor*>(fDStates->elementAt(i));
    }
    मिटाओ fDStates;
    मिटाओ fSafeTable;
    मिटाओ fLookAheadRuleMap;
}


//-----------------------------------------------------------------------------
//
//   RBBITableBuilder::buildForwardTable  -  This is the main function for building
//                               the DFA state transition table from the RBBI rules parse tree.
//
//-----------------------------------------------------------------------------
व्योम  RBBITableBuilder::buildForwardTable() {

    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    अगर (f_वृक्ष==nullptr) {
        वापस;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_वृक्ष = f_वृक्ष->flattenVariables(*fStatus, 0);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
#अगर_घोषित RBBI_DEBUG
    अगर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_वृक्ष(f_वृक्ष, true);
    }
#पूर्ण_अगर

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = नया RBBINode(RBBINode::opCat, *fStatus);
    // Exit if memory allocation failed.
    अगर (cn == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    अगर (U_FAILURE(*fStatus)) {
        मिटाओ cn;
        वापस;
    }
    cn->f_बायाँ_Child = f_वृक्ष;
    f_वृक्ष->fParent = cn;
    RBBINode *end_Marker_गाँठ = cn->f_दायाँ_Child = नया RBBINode(RBBINode::endMark, *fStatus);
    // Delete and exit if memory allocation failed.
    अगर (cn->f_दायाँ_Child == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    अगर (U_FAILURE(*fStatus)) {
        मिटाओ cn;
        वापस;
    }
    cn->f_दायाँ_Child->fParent = cn;
    f_वृक्ष = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_वृक्ष->flattenSets(*fStatus, 0);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
#अगर_घोषित RBBI_DEBUG
    अगर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_वृक्ष(f_वृक्ष, true);
    }
#पूर्ण_अगर


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_वृक्ष);
    calcFirstPos(f_वृक्ष);
    calcLastPos(f_वृक्ष);
    calcFollowPos(f_वृक्ष);
    अगर (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_वृक्ष);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    अगर (fRB->fChainRules) {
        calcChainedFollowPos(f_वृक्ष, end_Marker_गाँठ);
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
व्योम RBBITableBuilder::calcNullable(RBBINode *n) {
    अगर (n == nullptr) {
        वापस;
    }
    अगर (n->fType == RBBINode::setRef ||
        n->fType == RBBINode::endMark ) {
        // These are non-empty leaf node types.
        n->fNullable = false;
        वापस;
    }

    अगर (n->fType == RBBINode::lookAhead || n->fType == RBBINode::tag) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = true;
        वापस;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_बायाँ_Child);
    calcNullable(n->f_दायाँ_Child);

    // Apply functions from table 3.40 in Aho
    अगर (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_बायाँ_Child->fNullable || n->f_दायाँ_Child->fNullable;
    }
    अन्यथा अगर (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_बायाँ_Child->fNullable && n->f_दायाँ_Child->fNullable;
    }
    अन्यथा अगर (n->fType == RBBINode::opStar || n->fType == RBBINode::opQuestion) {
        n->fNullable = true;
    }
    अन्यथा {
        n->fNullable = false;
    }
}




//-----------------------------------------------------------------------------
//
//   calcFirstPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
व्योम RBBITableBuilder::calcFirstPos(RBBINode *n) {
    अगर (n == nullptr) {
        वापस;
    }
    अगर (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fFirstPosSet->addElement(n, *fStatus);
        वापस;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calcFirstPos(n->f_बायाँ_Child);
    calcFirstPos(n->f_दायाँ_Child);

    // Apply functions from table 3.40 in Aho
    अगर (n->fType == RBBINode::opOr) {
        setAdd(n->fFirstPosSet, n->f_बायाँ_Child->fFirstPosSet);
        setAdd(n->fFirstPosSet, n->f_दायाँ_Child->fFirstPosSet);
    }
    अन्यथा अगर (n->fType == RBBINode::opCat) {
        setAdd(n->fFirstPosSet, n->f_बायाँ_Child->fFirstPosSet);
        अगर (n->f_बायाँ_Child->fNullable) {
            setAdd(n->fFirstPosSet, n->f_दायाँ_Child->fFirstPosSet);
        }
    }
    अन्यथा अगर (n->fType == RBBINode::opStar ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fFirstPosSet, n->f_बायाँ_Child->fFirstPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
व्योम RBBITableBuilder::calcLastPos(RBBINode *n) {
    अगर (n == nullptr) {
        वापस;
    }
    अगर (n->fType == RBBINode::leafChar  ||
        n->fType == RBBINode::endMark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::tag) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->fLastPosSet->addElement(n, *fStatus);
        वापस;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calcLastPos(n->f_बायाँ_Child);
    calcLastPos(n->f_दायाँ_Child);

    // Apply functions from table 3.40 in Aho
    अगर (n->fType == RBBINode::opOr) {
        setAdd(n->fLastPosSet, n->f_बायाँ_Child->fLastPosSet);
        setAdd(n->fLastPosSet, n->f_दायाँ_Child->fLastPosSet);
    }
    अन्यथा अगर (n->fType == RBBINode::opCat) {
        setAdd(n->fLastPosSet, n->f_दायाँ_Child->fLastPosSet);
        अगर (n->f_दायाँ_Child->fNullable) {
            setAdd(n->fLastPosSet, n->f_बायाँ_Child->fLastPosSet);
        }
    }
    अन्यथा अगर (n->fType == RBBINode::opStar     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        setAdd(n->fLastPosSet, n->f_बायाँ_Child->fLastPosSet);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
व्योम RBBITableBuilder::calcFollowPos(RBBINode *n) {
    अगर (n == nullptr ||
        n->fType == RBBINode::leafChar ||
        n->fType == RBBINode::endMark) {
        वापस;
    }

    calcFollowPos(n->f_बायाँ_Child);
    calcFollowPos(n->f_दायाँ_Child);

    // Aho rule #1
    अगर (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *Last_Pos_Of_बायाँ_Child = n->f_बायाँ_Child->fLastPosSet;

        क्रम (ix = 0; ix < static_cast<uint32_t>(Last_Pos_Of_बायाँ_Child->size()); ix++) {
            i = static_cast<RBBINode*>(Last_Pos_Of_बायाँ_Child->elementAt(ix));
            setAdd(i->fFollowPos, n->f_दायाँ_Child->fFirstPosSet);
        }
    }

    // Aho rule #2
    अगर (n->fType == RBBINode::opStar ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        क्रम (ix = 0; ix < static_cast<uint32_t>(n->fLastPosSet->size()); ix++) {
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
व्योम RBBITableBuilder::add_Rule_मूल_Nodes(UVector *dest, RBBINode *गाँठ) {
    अगर (गाँठ == nullptr || U_FAILURE(*fStatus)) {
        वापस;
    }
    U_ASSERT(!dest->hasDeleter());
    अगर (गाँठ->f_Rule_मूल) {
        dest->addElement(गाँठ, *fStatus);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        वापस;
    }
    add_Rule_मूल_Nodes(dest, गाँठ->f_बायाँ_Child);
    add_Rule_मूल_Nodes(dest, गाँठ->f_दायाँ_Child);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
व्योम RBBITableBuilder::calcChainedFollowPos(RBBINode *वृक्ष, RBBINode *end_Mark_गाँठ) {

    UVector         leafNodes(*fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }

    // get a list all leaf nodes
    वृक्ष->findNodes(&leafNodes, RBBINode::leafChar, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector rule_मूल_Nodes(*fStatus);
    add_Rule_मूल_Nodes(&rule_मूल_Nodes, वृक्ष);

    UVector matchStartNodes(*fStatus);
    क्रम (पूर्णांक j=0; j<rule_मूल_Nodes.size(); ++j) {
        RBBINode *गाँठ = static_cast<RBBINode *>(rule_मूल_Nodes.elementAt(j));
        अगर (गाँठ->fChainIn) {
            setAdd(&matchStartNodes, गाँठ->fFirstPosSet);
        }
    }
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }

    int32_t  end_गाँठ_Ix;
    int32_t  start_गाँठ_Ix;

    क्रम (end_गाँठ_Ix=0; end_गाँठ_Ix<leafNodes.size(); end_गाँठ_Ix++) {
        RBBINode* end_गाँठ = static_cast<RBBINode*>(leafNodes.elementAt(end_गाँठ_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        अगर (!end_गाँठ->fFollowPos->contains(end_Mark_गाँठ)) {
            जारी;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *start_गाँठ;
        क्रम (start_गाँठ_Ix = 0; start_गाँठ_Ix<matchStartNodes.size(); start_गाँठ_Ix++) {
            start_गाँठ = static_cast<RBBINode*>(matchStartNodes.elementAt(start_गाँठ_Ix));
            अगर (start_गाँठ->fType != RBBINode::leafChar) {
                जारी;
            }

            अगर (end_गाँठ->fVal == start_गाँठ->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                setAdd(end_गाँठ->fFollowPos, start_गाँठ->fFollowPos);
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
व्योम RBBITableBuilder::bofFixup() {

    अगर (U_FAILURE(*fStatus)) {
        वापस;
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
    RBBINode  *bof_गाँठ = f_वृक्ष->f_बायाँ_Child->f_बायाँ_Child;
    U_ASSERT(bof_गाँठ->fType == RBBINode::leafChar);
    U_ASSERT(bof_गाँठ->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *matchStartNodes = f_वृक्ष->f_बायाँ_Child->f_दायाँ_Child->fFirstPosSet;

    RBBINode *start_गाँठ;
    पूर्णांक       start_गाँठ_Ix;
    क्रम (start_गाँठ_Ix = 0; start_गाँठ_Ix<matchStartNodes->size(); start_गाँठ_Ix++) {
        start_गाँठ = static_cast<RBBINode*>(matchStartNodes->elementAt(start_गाँठ_Ix));
        अगर (start_गाँठ->fType != RBBINode::leafChar) {
            जारी;
        }

        अगर (start_गाँठ->fVal == bof_गाँठ->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            setAdd(bof_गाँठ->fFollowPos, start_गाँठ->fFollowPos);
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
व्योम RBBITableBuilder::buildStateTable() {
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = nullptr;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    पूर्णांक      last_निवेश_प्रतीक = fRB->fSetBuilder->get_अंक_Char_Categories() - 1;
    failState = नया RBBIStateDescriptor(last_निवेश_प्रतीक, fStatus);
    अगर (failState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        जाओ ExitBuildSTdeleteall;
    }
    failState->fPositions = नया UVector(*fStatus);
    अगर (failState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    अगर (failState->fPositions == nullptr || U_FAILURE(*fStatus)) {
        जाओ ExitBuildSTdeleteall;
    }
    fDStates->addElement(failState, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        जाओ ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = नया RBBIStateDescriptor(last_निवेश_प्रतीक, fStatus);
    अगर (initialState == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    अगर (U_FAILURE(*fStatus)) {
        जाओ ExitBuildSTdeleteall;
    }
    initialState->fPositions = नया UVector(*fStatus);
    अगर (initialState->fPositions == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    अगर (U_FAILURE(*fStatus)) {
        जाओ ExitBuildSTdeleteall;
    }
    setAdd(initialState->fPositions, f_वृक्ष->fFirstPosSet);
    fDStates->addElement(initialState, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        जाओ ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    क्रम (;;) {
        RBBIStateDescriptor *T = nullptr;
        int32_t              tx;
        क्रम (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *अस्थायी;
            अस्थायी = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            अगर (अस्थायी->fMarked == false) {
                T = अस्थायी;
                अवरोध;
            }
        }
        अगर (T == nullptr) {
            अवरोध;
        }

        // mark T;
        T->fMarked = true;

        // for each input symbol a do begin
        int32_t  a;
        क्रम (a = 1; a<=last_निवेश_प्रतीक; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = nullptr;
            RBBINode   *p;
            int32_t     px;
            क्रम (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                अगर ((p->fType == RBBINode::leafChar) &&  (p->fVal == a)) {
                    अगर (U == nullptr) {
                        U = नया UVector(*fStatus);
                        अगर (U == nullptr) {
                        	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                        	जाओ ExitBuildSTdeleteall;
                        }
                    }
                    setAdd(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = false;
            अगर (U != nullptr) {
                U_ASSERT(U->size() > 0);
                पूर्णांक  ix;
                क्रम (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    अगर (setEquals(U, temp2->fPositions)) {
                        मिटाओ U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = true;
                        अवरोध;
                    }
                }

                // Add U as an unmarked state to Dstates
                अगर (!UinDstates)
                {
                    RBBIStateDescriptor *newState = नया RBBIStateDescriptor(last_निवेश_प्रतीक, fStatus);
                    अगर (newState == nullptr) {
                    	*fStatus = U_MEMORY_ALLOCATION_ERROR;
                    }
                    अगर (U_FAILURE(*fStatus)) {
                        जाओ ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->addElement(newState, *fStatus);
                    अगर (U_FAILURE(*fStatus)) {
                        वापस;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->setElementAt(ux, a);
            }
        }
    }
    वापस;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    मिटाओ initialState;
    मिटाओ failState;
}


/**
 * mapLookAheadRules
 *
 */
व्योम RBBITableBuilder::mapLookAheadRules() {
    fLookAheadRuleMap =  नया UVector32(fRB->fScanner->अंक_Rules() + 1, *fStatus);
    अगर (fLookAheadRuleMap == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
    }
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    fLookAheadRuleMap->set_आकार(fRB->fScanner->अंक_Rules() + 1);

    क्रम (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t laSlotForState = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        bool saw_Look_Ahead_गाँठ = false;
        क्रम (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *गाँठ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            अगर (गाँठ->fType != RBBINode::गाँठ_Type::lookAhead) {
                जारी;
            }
            saw_Look_Ahead_गाँठ = true;
            int32_t rule_अंक = गाँठ->fVal;     // Set when rule was originally parsed.
            U_ASSERT(rule_अंक < fLookAheadRuleMap->size());
            U_ASSERT(rule_अंक > 0);
            int32_t laSlot = fLookAheadRuleMap->elementAti(rule_अंक);
            अगर (laSlot != 0) {
                अगर (laSlotForState == 0) {
                    laSlotForState = laSlot;
                } अन्यथा {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(laSlot == laSlotForState);
                }
            }
        }
        अगर (!saw_Look_Ahead_गाँठ) {
            जारी;
        }

        अगर (laSlotForState == 0) {
            laSlotForState = ++fLASlotsInUse;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        क्रम (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *गाँठ = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            अगर (गाँठ->fType != RBBINode::गाँठ_Type::lookAhead) {
                जारी;
            }
            int32_t rule_अंक = गाँठ->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = fLookAheadRuleMap->elementAti(rule_अंक);
            (व्योम)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == laSlotForState);
            fLookAheadRuleMap->setElementAt(laSlotForState, rule_अंक);
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
व्योम     RBBITableBuilder::flagAcceptingStates() {
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    UVector     endMarkerNodes(*fStatus);
    RBBINode    *endMarker;
    int32_t     i;
    int32_t     n;

    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }

    f_वृक्ष->findNodes(&endMarkerNodes, RBBINode::endMark, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }

    क्रम (i=0; i<endMarkerNodes.size(); i++) {
        endMarker = static_cast<RBBINode*>(endMarkerNodes.elementAt(i));
        क्रम (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            अगर (sd->fPositions->सूचकांक_Of(endMarker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                अगर (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = fLookAheadRuleMap->elementAti(endMarker->fVal);
                    अगर (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                अगर (sd->fAccepting==ACCEPTING_UNCONDITIONAL && endMarker->fVal != 0) {
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
व्योम     RBBITableBuilder::flagLookAheadStates() {
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    UVector     lookAheadNodes(*fStatus);
    RBBINode    *look_Ahead_गाँठ;
    int32_t     i;
    int32_t     n;

    f_वृक्ष->findNodes(&lookAheadNodes, RBBINode::lookAhead, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    क्रम (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_गाँठ = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_गाँठ->fType == RBBINode::गाँठ_Type::lookAhead);

        क्रम (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->सूचकांक_Of(look_Ahead_गाँठ);
            अगर (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_गाँठ == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookaheadSlot = fLookAheadRuleMap->elementAti(look_Ahead_गाँठ->fVal);
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
व्योम     RBBITableBuilder::flagTaggedStates() {
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    UVector     tagNodes(*fStatus);
    RBBINode    *tag_गाँठ;
    int32_t     i;
    int32_t     n;

    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    f_वृक्ष->findNodes(&tagNodes, RBBINode::tag, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    क्रम (i=0; i<tagNodes.size(); i++) {                   // For each tag node t (all of 'em)
        tag_गाँठ = static_cast<RBBINode*>(tagNodes.elementAt(i));

        क्रम (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            अगर (sd->fPositions->सूचकांक_Of(tag_गाँठ) >= 0) {  //       if  s include the tag node t
                sortedAdd(&sd->fTagVals, tag_गाँठ->fVal);
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
व्योम  RBBITableBuilder::mergeRuleStatusVals() {
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
    पूर्णांक i;
    पूर्णांक n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    अगर (fRB->fRuleStatusVals->size() == 0) {
        fRB->fRuleStatusVals->addElement(1, *fStatus);  // Num of statuses in group
        fRB->fRuleStatusVals->addElement(static_cast<int32_t>(0), *fStatus); // and our single status of zero
    }

    //    For each state
    क्रम (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_States_Tag_मूल्यावली = sd->fTagVals;
        अगर (this_States_Tag_मूल्यावली == nullptr) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->fTagsIdx = 0;
            जारी;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->fTagsIdx = -1;
        int32_t  thisTagGroupStart = 0;   // indexes into the global rule status vals list
        int32_t  अग्र_Tag_Group_Start = 0;

        // Loop runs once per group of tags in the global list
        जबतक (अग्र_Tag_Group_Start < fRB->fRuleStatusVals->size()) {
            thisTagGroupStart = अग्र_Tag_Group_Start;
            अग्र_Tag_Group_Start += fRB->fRuleStatusVals->elementAti(thisTagGroupStart) + 1;
            अगर (this_States_Tag_मूल्यावली->size() != fRB->fRuleStatusVals->elementAti(thisTagGroupStart)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                जारी;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            क्रम (i=0; i<this_States_Tag_मूल्यावली->size(); i++) {
                अगर (this_States_Tag_मूल्यावली->elementAti(i) !=
                    fRB->fRuleStatusVals->elementAti(thisTagGroupStart + 1 + i) ) {
                    // Mismatch.
                    अवरोध;
                }
            }

            अगर (i == this_States_Tag_मूल्यावली->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->fTagsIdx = thisTagGroupStart;
                अवरोध;
            }
        }

        अगर (sd->fTagsIdx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->fTagsIdx = fRB->fRuleStatusVals->size();
            fRB->fRuleStatusVals->addElement(this_States_Tag_मूल्यावली->size(), *fStatus);
            क्रम (i=0; i<this_States_Tag_मूल्यावली->size(); i++) {
                fRB->fRuleStatusVals->addElement(this_States_Tag_मूल्यावली->elementAti(i), *fStatus);
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

व्योम RBBITableBuilder::minimizeStates() {
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }

#अगर UPRV_HAS_SANITIZER
    अगर (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *fStatus = U_REGEX_PATTERN_TOO_BIG;
        वापस;
    }
#पूर्ण_अगर
    काष्ठा StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t fTagsIdx;
        bool चालक==(StateType स्थिर &other) स्थिर {
            वापस fAccepting == other.fAccepting &&
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
    काष्ठा TypeToStates : UMemory {
        TypeToStates(स्थिर StateType &type, UErrorCode &status)
            : type(type), states(prv::make_unique<UVector>(status, status)) {}
        StateType type;
        prv::unique_ptr<UVector> states;
    };
    UVector initialPartition(*fStatus);
    initialPartition.setDeleter(
        [](व्योम *p) { मिटाओ static_cast<TypeToStates *>(p); });
    क्रम (int32_t i = 0; i < fDStates->size(); ++i) {
        स्थिर RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        स्थिर StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.fTagsIdx};
        int32_t j = 0;
        क्रम (; j < initialPartition.size(); ++j) {
            स्वतः &[type_j, states] =
                *static_cast<TypeToStates *>(initialPartition.elementAt(j));
            अगर (type_j == type) {
                states->addElement(i, *fStatus);
                अवरोध;
            }
        }
        अगर (j == initialPartition.size()) {
            स्वतः newEntry = prv::make_unique<TypeToStates>(type, *fStatus, *fStatus);
            अगर (U_FAILURE(*fStatus)) {
                वापस;
            }
            newEntry->states->addElement(i, *fStatus);
            initialPartition.adoptElement(newEntry.release(), *fStatus);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    स्वतः partition = prv::make_unique<UVector>(*fStatus, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    partition->setDeleter(
        [](व्योम *p) { मिटाओ static_cast<UVector*>(p); });
    
    क्रम (int32_t i = 0; i < initialPartition.size(); ++i) {
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
    स्वतः partition_signature = [&partition, यह](int32_t state_सूचकांक) -> prv::unique_ptr<UVector> {
        स्थिर RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state_सूचकांक));
        स्वतः परिणाम = prv::make_unique<UVector>(state.fDtran->size(), *fStatus, *fStatus);
        अगर (U_FAILURE(*fStatus)) {
            वापस nullptr;
        }
        क्रम (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t toState = state.fDtran->elementAti(i);
            क्रम (int32_t part_सूचकांक = 0; part_सूचकांक < partition->size(); ++part_सूचकांक) {
                स्थिर UVector &part = *static_cast<UVector *>(partition->elementAt(part_सूचकांक));
                अगर (part.contains(toState)) {
                    परिणाम->addElement(part_सूचकांक, *fStatus);
                    अवरोध;
                }
            }
        }
        परिणाम->setComparer([](स्थिर UElement बाँए, स्थिर UElement दाँए) -> UBool {
            वापस बाँए.integer == दाँए.integer;
        });
        वापस परिणाम;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    क्रम (;;) {
        // Π_new.
        स्वतः partitionNew = prv::make_unique<UVector>(*fStatus, *fStatus);
        अगर (U_FAILURE(*fStatus)) {
            वापस;
        }
        partitionNew->setDeleter(
            [](व्योम *p) { मिटाओ static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        bool refined = false;
        क्रम (int32_t i = 0; i < partition->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            स्थिर UVector &group = *static_cast<UVector*>(partition->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            काष्ठा SignatureToStates : UMemory {
                SignatureToStates(prv::unique_ptr<UVector> signature, UErrorCode &status)
                    : signature(std::move(signature)), states(prv::make_unique<UVector>(status, status)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> states;
            };
            UVector groupRefinement(*fStatus);
            groupRefinement.setDeleter([](व्योम *p) { मिटाओ static_cast<SignatureToStates*>(p); });
            क्रम (int32_t j = 0; j < group.size(); ++j) {
                // Index of a state in the group.
                स्थिर int32_t s = group.elementAti(j);
                स्वतः signature = partition_signature(s);
                int32_t k = 0;
                क्रम (; k < groupRefinement.size(); ++k) {
                    स्थिर स्वतः &[subgroupSignature, subgroup] =
                        *static_cast<SignatureToStates*>(groupRefinement.elementAt(k));
                    अगर (*subgroupSignature == *signature) {
                        subgroup->addElement(s, *fStatus);
                        अवरोध;
                    }
                }
                अगर (k == groupRefinement.size()) {
                    स्थिर स्वतः newEntry =
                        नया SignatureToStates(std::move(signature), *fStatus);
                    अगर (U_FAILURE(*fStatus)) {
                        वापस;
                    }
                    अगर (newEntry == nullptr) {
                      *fStatus = U_MEMORY_ALLOCATION_ERROR;
                      वापस;
                    }
                    newEntry->states->addElement(s, *fStatus);
                    groupRefinement.adoptElement(newEntry, *fStatus);
                }
            }
            refined |= groupRefinement.size() > 1;
            क्रम (int32_t j = 0; j < groupRefinement.size(); ++j) {
                स्वतः &[_, subgroup] = *static_cast<SignatureToStates*>(
                    groupRefinement.elementAt(j));
                partitionNew->adoptElement(subgroup.release(), *fStatus);
            }
        }
        अगर (refined) {
            partition = std::move(partitionNew);
        } अन्यथा {
            अवरोध;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    क्रम (पूर्णांक i = 0; i < partition->size(); ++i) {
        UVector *स्थिर part_i = static_cast<UVector *>(partition->elementAt(i));
        अगर (part_i->contains(0)) {
            व्योम *स्थिर part_0 = partition->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            स्थिर स्वतः deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 0);
            partition->setElementAt(part_0, i);
            partition->setDeleter(deleter);
        } अन्यथा अगर (part_i->contains(1)) {
            व्योम *स्थिर part_1 = partition->elementAt(1);
            स्थिर स्वतः deleter = partition->setDeleter(nullptr);
            partition->setElementAt(part_i, 1);
            partition->setElementAt(part_1, i);
            partition->setDeleter(deleter);
        }
    }
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    स्वतः oldStateToPart = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    क्रम (पूर्णांक i = 0; i < partition->size(); ++i) {
        स्थिर UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        क्रम (पूर्णांक j = 0; j < part.size(); ++j) {
            oldStateToPart[part.elementAti(j)] = i;
        }
    }
    स्वतः स्थिर oldStates = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*fStatus, *fStatus).release();
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    क्रम (पूर्णांक i = 0; i < partition->size(); ++i) {
        स्थिर UVector &part = *static_cast<UVector*>(partition->elementAt(i));
        RBBIStateDescriptor *स्थिर state =
            static_cast<RBBIStateDescriptor*>(oldStates->elementAt(part.elementAti(0)));
        fDStates->addElement(state, *fStatus);
        क्रम (पूर्णांक j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->setElementAt(oldStateToPart[state->fDtran->elementAti(j)], j);
        }
        oldStates->setElementAt(nullptr, part.elementAti(0));
    }
    oldStates->setDeleter([](व्योम *p) { मिटाओ static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
व्योम RBBITableBuilder::sortedAdd(UVector **vector, int32_t val) {
    int32_t i;

    अगर (*vector == nullptr) {
        *vector = नया UVector(*fStatus);
    }
    अगर (*vector == nullptr || U_FAILURE(*fStatus)) {
        वापस;
    }
    UVector *vec = *vector;
    int32_t  v_आकार = vec->size();
    क्रम (i=0; i<v_आकार; i++) {
        int32_t valAtI = vec->elementAti(i);
        अगर (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            वापस;
        }
        अगर (valAtI > val) {
            अवरोध;
        }
    }
    vec->जोड़ो_Element_At(val, i, *fStatus);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
व्योम RBBITableBuilder::setAdd(UVector *dest, UVector *source) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!source->hasDeleter());
    int32_t dest_Original_आकार = dest->size();
    int32_t source_आकार       = source->size();
    int32_t di           = 0;
    Maybe_ढेर_Array<व्योम *, 16> destArray, sourceArray;  // Handle small cases without malloc
    व्योम **destPtr, **sourcePtr;
    व्योम **destLim, **sourceLim;

    अगर (dest_Original_आकार > destArray.getCapacity()) {
        अगर (destArray.resize(dest_Original_आकार) == nullptr) {
            वापस;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + dest_Original_आकार;  // destArray.getArrayLimit()?

    अगर (source_आकार > sourceArray.getCapacity()) {
        अगर (sourceArray.resize(source_आकार) == nullptr) {
            वापस;
        }
    }
    sourcePtr = sourceArray.getAlias();
    sourceLim = sourcePtr + source_आकार;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (व्योम) dest->toArray(destPtr);
    (व्योम) source->toArray(sourcePtr);

    dest->set_आकार(source_आकार+dest_Original_आकार, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }

    जबतक (sourcePtr < sourceLim && destPtr < destLim) {
        अगर (*destPtr == *sourcePtr) {
            dest->setElementAt(*sourcePtr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        अन्यथा अगर (uprv_memcmp(destPtr, sourcePtr, माप(व्योम *)) < 0) {
            dest->setElementAt(*destPtr++, di++);
        }
        अन्यथा { /* *sourcePtr < *destPtr */
            dest->setElementAt(*sourcePtr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    जबतक (destPtr < destLim) {
        dest->setElementAt(*destPtr++, di++);
    }
    जबतक (sourcePtr < sourceLim) {
        dest->setElementAt(*sourcePtr++, di++);
    }

    dest->set_आकार(di, *fStatus);
}



//-----------------------------------------------------------------------------
//
//  setEqual    Set operation on UVector.
//              Compare for equality.
//              Elements must be sorted.
//
//-----------------------------------------------------------------------------
UBool RBBITableBuilder::setEquals(UVector *a, UVector *b) {
    वापस a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG
व्योम RBBITableBuilder::printPosSets(RBBINode *n) {
    अगर (n==nullptr) {
        वापस;
    }
    म_लिखो("\n");
    RBBINode::print_गाँठ_Header();
    RBBINode::print_गाँठ(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    printSet(n->fFirstPosSet);

    RBBIDebugPrintf("         lastpos:   ");
    printSet(n->fLastPosSet);

    RBBIDebugPrintf("         followpos: ");
    printSet(n->fFollowPos);

    printPosSets(n->f_बायाँ_Child);
    printPosSets(n->f_दायाँ_Child);
}
#पूर्ण_अगर

//
//    findDuplCharClassFrom()
//
bool RBBITableBuilder::findDuplCharClassFrom(IntPair *categories) {
    int32_t अंक_States = fDStates->size();
    int32_t अंक_Cols = fRB->fSetBuilder->get_अंक_Char_Categories();

    क्रम (; categories->first < अंक_Cols-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        पूर्णांक limitSecond = categories->first < fRB->fSetBuilder->getDictCategoriesStart() ?
            fRB->fSetBuilder->getDictCategoriesStart() : अंक_Cols;
        क्रम (categories->second=categories->first+1; categories->second < limitSecond; categories->second++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t table_base = 0;
            uint16_t table_dupl = 1;
            क्रम (int32_t state=0; state<अंक_States; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                table_base = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                table_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->second));
                अगर (table_base != table_dupl) {
                    अवरोध;
                }
            }
            अगर (table_base == table_dupl) {
                वापस true;
            }
        }
    }
    वापस false;
}


//
//    removeColumn()
//
व्योम RBBITableBuilder::removeColumn(int32_t column) {
    int32_t अंक_States = fDStates->size();
    क्रम (int32_t state=0; state<अंक_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(column < sd->fDtran->size());
        sd->fDtran->removeElementAt(column);
    }
}


bool RBBITableBuilder::findDuplicateSafeState(IntPair *states) {
    int32_t अंक_States = fSafeTable->size();

    क्रम (; states->first<अंक_States-1; states->first++) {
        UnicodeString *firstRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->first));
        क्रम (states->second=states->first+1; states->second<अंक_States; states->second++) {
            UnicodeString *duplRow = static_cast<UnicodeString *>(fSafeTable->elementAt(states->second));
            bool rowsMatch = true;
            int32_t अंक_Cols = firstRow->length();
            क्रम (int32_t col=0; col < अंक_Cols; ++col) {
                int32_t firstVal = firstRow->charAt(col);
                int32_t duplVal = duplRow->charAt(col);
                अगर (!((firstVal == duplVal) ||
                        ((firstVal == states->first || firstVal == states->second) &&
                        (duplVal  == states->first || duplVal  == states->second)))) {
                    rowsMatch = false;
                    अवरोध;
                }
            }
            अगर (rowsMatch) {
                वापस true;
            }
        }
    }
    वापस false;
}

व्योम RBBITableBuilder::removeSafeState(IntPair duplStates) {
    स्थिर int32_t keepState = duplStates.first;
    स्थिर int32_t duplState = duplStates.second;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < fSafeTable->size());

    fSafeTable->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t अंक_States = fSafeTable->size();
    क्रम (int32_t state=0; state<अंक_States; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        int32_t अंक_Cols = sd->length();
        क्रम (int32_t col=0; col<अंक_Cols; col++) {
            int32_t existingVal = sd->charAt(col);
            int32_t newVal = existingVal;
            अगर (existingVal == duplState) {
                newVal = keepState;
            } अन्यथा अगर (existingVal > duplState) {
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
    स्थिर int32_t old_State_गिनती = fDStates->size();
    minimizeStates();
    वापस fDStates->size() - old_State_गिनती;
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
व्योम RBBITableBuilder::minimizeLookaheads() {
    अगर (fLASlotsInUse == ACCEPTING_UNCONDITIONAL) {
        वापस;
    }
    स्थिर int32_t lookahead_गिनती = fLASlotsInUse - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    स्थिर int32_t firstLookahead = ACCEPTING_UNCONDITIONAL + 1;
    स्थिर स्वतः stateDescriptor = [यह](स्थिर int32_t state) -> RBBIStateDescriptor & {
        वापस *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    स्थिर स्वतः lookaheadReachability =
        prv::make_unique<bool[]>(lookahead_गिनती * lookahead_गिनती, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    क्रम (int32_t l = firstLookahead; l <= fLASlotsInUse; ++l) {
        क्रम (int32_t k = firstLookahead; k <= fLASlotsInUse; ++k) {
            अगर (k != l) {
                क्रम (int32_t source = 1; source < fDStates->size(); ++source) {
                    अगर (static_cast<int32_t>(stateDescriptor(source).fLookAhead) != l) {
                        जारी;
                    }
                    अगर (reachableByTransitions(
                            *fDStates, source, /*isSink=*/
                            [&](स्थिर int32_t state) {
                                वापस static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](स्थिर int32_t state) {
                                वापस static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *fStatus)) {
                        lookaheadReachability[(l - firstLookahead) * lookahead_गिनती + k -
                                              firstLookahead] = true;
                    }
                }
            }
        }
    }

    स्थिर स्वतः colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_गिनती, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    क्रम (int32_t chromatic_संख्या = 1;; ++chromatic_संख्या) {
        uprv_memset(colours.लो(), 0, माप(int32_t) * lookahead_गिनती);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        क्रम (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            क्रम (पूर्णांक source = 0; source < lookahead_गिनती; ++source) {
                क्रम (पूर्णांक sink = 0; sink < lookahead_गिनती; ++sink) {
                    अगर (lookaheadReachability[source * lookahead_गिनती + sink] &&
                        colours[source] == colours[sink]) {
                        जाओ अग्र_Colouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            क्रम (पूर्णांक i = 0; i < fDStates->size(); ++i) {
                अगर (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        firstLookahead +
                        colours[stateDescriptor(i).fAccepting - firstLookahead];
                }
                अगर (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - firstLookahead];
                }
            }
            fLASlotsInUse = ACCEPTING_UNCONDITIONAL + chromatic_संख्या;
            वापस;

          अग्र_Colouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            क्रम (int32_t i = 0; i < lookahead_गिनती - 1 && colours[i] == chromatic_संख्या;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            अगर (colours[lookahead_गिनती - 1] == chromatic_संख्या) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                अवरोध;
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
int32_t  RBBITableBuilder::get_Table_आकार() स्थिर {
    int32_t    size = 0;
    int32_t    अंक_Rows;
    int32_t    अंक_Cols;
    int32_t    row_आकार;

    अगर (f_वृक्ष == nullptr) {
        वापस 0;
    }

    size    = दुरत्व(RBBIStateTable, f_Table_तथ्य);    // The header, with no rows to the table.

    अंक_Rows = fDStates->size();
    अंक_Cols = fRB->fSetBuilder->get_अंक_Char_Categories();

    अगर (use8BitsForTable()) {
        row_आकार = दुरत्व(RBBIStateTableRow8, f_अग्र_State) + माप(int8_t)*अंक_Cols;
    } अन्यथा {
        row_आकार = दुरत्व(RBBIStateTableRow16, f_अग्र_State) + माप(int16_t)*अंक_Cols;
    }
    size   += अंक_Rows * row_आकार;
    वापस size;
}

bool RBBITableBuilder::use8BitsForTable() स्थिर {
    वापस fDStates->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
व्योम RBBITableBuilder::exportTable(व्योम *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    पूर्णांक                col;

    अगर (U_FAILURE(*fStatus) || f_वृक्ष == nullptr) {
        वापस;
    }

    int32_t cat_गिनती = fRB->fSetBuilder->get_अंक_Char_Categories();
    अगर (cat_गिनती > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        वापस;
    }

    table->f_अंक_States = fDStates->size();
    table->fDictCategoriesStart = fRB->fSetBuilder->getDictCategoriesStart();
    table->f_Look_Ahead_Results_आकार = fLASlotsInUse == ACCEPTING_UNCONDITIONAL ? 0 : fLASlotsInUse + 1;
    table->fFlags     = 0;
    अगर (use8BitsForTable()) {
        table->fRowLen    = दुरत्व(RBBIStateTableRow8, f_अग्र_State) + माप(uint8_t) * cat_गिनती;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } अन्यथा {
        table->fRowLen    = दुरत्व(RBBIStateTableRow16, f_अग्र_State) + माप(int16_t) * cat_गिनती;
    }
    अगर (fRB->fLookAheadHardBreak) {
        table->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    क्रम (state=0; state<table->f_अंक_States; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->f_Table_तथ्य + state * table->fRowLen);
        अगर (use8BitsForTable()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 255);
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->fTagsIdx   = sd->fTagsIdx;
            क्रम (col=0; col<cat_गिनती; col++) {
                U_ASSERT (sd->fDtran->elementAti(col) <= kMaxStateFor8BitsTable);
                r8->f_अग्र_State[col] = sd->fDtran->elementAti(col);
            }
        } अन्यथा {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->fTagsIdx && sd->fTagsIdx <= 0xffff);
            row->r16.fAccepting = sd->fAccepting;
            row->r16.fLookAhead = sd->fLookAhead;
            row->r16.fTagsIdx   = sd->fTagsIdx;
            क्रम (col=0; col<cat_गिनती; col++) {
                row->r16.f_अग्र_State[col] = sd->fDtran->elementAti(col);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
व्योम RBBITableBuilder::buildSafeReverseTable(UErrorCode &status) {
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

    int32_t अंक_Char_Classes = fRB->fSetBuilder->get_अंक_Char_Categories();
    int32_t अंक_States = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    स्थिर स्वतः lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    स्थिर स्वतः wantedLookaheadsInPair =
        prv::make_unique_for_overwrite<int8_t[]>(fLASlotsInUse + 1, status);
    अगर (U_FAILURE(status)) {
        वापस;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t START_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_SET = -1;
    क्रम (int32_t c1 = 1; c1 < अंक_Char_Classes; ++c1) {
        अगर (fRB->fSetBuilder->getFirstChar(c1) < 0) {
          जारी;
        }
        क्रम (int32_t c2 = 1; c2 < अंक_Char_Classes; ++c2) {
            अगर (fRB->fSetBuilder->getFirstChar(c2) < 0) {
                जारी;
            }
            int32_t wantedEndState = -1;
            int32_t wantedLastAcceptingPosition;
            bool mustCheckConsistentWithStartC2 = false;
            स्थिर int32_t text[2] = {c1, c2};
            क्रम (int32_t injectedState = START_STATE; injectedState < अंक_States; ++injectedState) {
                क्रम (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= fLASlotsInUse; ++injectedLookahead) {
                    int32_t lastBreak = BEFORE_PAIR;
                    int32_t lastAcceptingPosition = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.लो(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                    अगर (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        स्थिर RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        अगर (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            lastAcceptingPosition = 0;
                        } अन्यथा अगर (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            जारी;
                        }
                        अगर (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    जाओ dfaStep;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    क्रम (;;) {
                        s = START_STATE;
                        uprv_memset(lookaheadPositions.लो(), LOOKAHEAD_NOT_SET, fLASlotsInUse + 1);
                        i = lastBreak;
                        // Loop Find_Next_Break in L2/26-135.
                        क्रम (;;) {
                        dfaStep:
                            अगर (i == BEFORE_PAIR || i == 2) {
                                जाओ inspectFinalState;
                            }
                            स्थिर int32_t प्रतीक_Ahead = text[i];
                            ++i;
                            स्थिर RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            अगर (sDescriptor->fDtran->elementAti(प्रतीक_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(प्रतीक_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } अन्यथा {
                                अगर (lastBreak != BEFORE_PAIR &&
                                    lastAcceptingPosition == lastBreak) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    जाओ अग्र_प्रतीक_Pair;
                                }
                                lastBreak = lastAcceptingPosition;
                                अवरोध;
                            }

                            अगर (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                lastAcceptingPosition = i;
                            } अन्यथा अगर (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_SET) {
                                lastBreak = lookaheadPositions[sDescriptor->fAccepting];
                                अवरोध;
                            }
                            अगर (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    अगर (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        जारी;
                    }
                    अगर (wantedEndState < 0) {
                        wantedEndState = s;
                        wantedLastAcceptingPosition = lastAcceptingPosition;
                        क्रम (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                            अगर (lookaheadPositions[l] != LOOKAHEAD_NOT_SET &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wantedLookaheadsInPair[l] = lookaheadPositions[i];
                            } अन्यथा {
                                wantedLookaheadsInPair[l] = LOOKAHEAD_NOT_SET;
                            }
                        }
                    } अन्यथा {
                        अगर (wantedEndState != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            जाओ अग्र_प्रतीक_Pair;
                        }
                        अगर (wantedLastAcceptingPosition != lastAcceptingPosition) {
                            जाओ अग्र_प्रतीक_Pair;
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
                    क्रम (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                        अगर (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wantedLookaheadsInPair[l]) {
                            जारी;
                        }
                        अगर (lookaheadPositions[l] == 2 || wantedLookaheadsInPair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            अगर (wantedEndState != START_STATE) {
                                जाओ अग्र_प्रतीक_Pair;
                            }
                        }
                        अगर (lookaheadPositions[l] == 1 || wantedLookaheadsInPair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            mustCheckConsistentWithStartC2 = true;
                        }
                    }
                }
            }
            अगर (mustCheckConsistentWithStartC2) {
                स्थिर RBBIStateDescriptor& startDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(START_STATE));
                स्थिर स्वतः c2State = startDescriptor.fDtran->elementAti(c2);
                अगर (c2State != wantedEndState) {
                    जाओ अग्र_प्रतीक_Pair;
                }
                स्थिर RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                अगर (wantedLastAcceptingPosition != 2) {
                    जाओ अग्र_प्रतीक_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                क्रम (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= fLASlotsInUse; ++l) {
                    अगर (((wantedLookaheadsInPair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wantedEndState != START_STATE) {
                        जाओ अग्र_प्रतीक_Pair;
                    }
                }
            }
            safePairs.append(static_cast<char16_t>(c1));
            safePairs.append(static_cast<char16_t>(c2));
        अग्र_प्रतीक_Pair:
            जारी;
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
        नया UVector(uprv_deleteUObject, uhash_compareUnicodeString, अंक_Char_Classes + 2, status), status);
    अगर (U_FAILURE(status)) {
        वापस;
    }
    fSafeTable = lpSafeTable.orphan();
    क्रम (int32_t row=0; row<अंक_Char_Classes + 2; ++row) {
        LocalPointer<UnicodeString> lpString(नया UnicodeString(अंक_Char_Classes, 0, अंक_Char_Classes+4), status);
        fSafeTable->adoptElement(lpString.orphan(), status);
    }
    अगर (U_FAILURE(status)) {
        वापस;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &startState = *static_cast<UnicodeString *>(fSafeTable->elementAt(1));
    क्रम (int32_t charClass=0; charClass < अंक_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        startState.setCharAt(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    क्रम (int32_t row=2; row<अंक_Char_Classes+2; ++row) {
        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(row));
        rowState = startState;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    क्रम (int32_t pairIdx=0; pairIdx<safePairs.length(); pairIdx+=2) {
        int32_t c1 = safePairs.charAt(pairIdx);
        int32_t c2 = safePairs.charAt(pairIdx + 1);

        UnicodeString &rowState = *static_cast<UnicodeString *>(fSafeTable->elementAt(c2 + 2));
        rowState.setCharAt(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair states = {1, 0};
    जबतक (findDuplicateSafeState(&states)) {
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
int32_t  RBBITableBuilder::get_Safe_Table_आकार() स्थिर {
    int32_t    size = 0;
    int32_t    अंक_Rows;
    int32_t    अंक_Cols;
    int32_t    row_आकार;

    अगर (fSafeTable == nullptr) {
        वापस 0;
    }

    size    = दुरत्व(RBBIStateTable, f_Table_तथ्य);    // The header, with no rows to the table.

    अंक_Rows = fSafeTable->size();
    अंक_Cols = fRB->fSetBuilder->get_अंक_Char_Categories();

    अगर (use8BitsForSafeTable()) {
        row_आकार = दुरत्व(RBBIStateTableRow8, f_अग्र_State) + माप(int8_t)*अंक_Cols;
    } अन्यथा {
        row_आकार = दुरत्व(RBBIStateTableRow16, f_अग्र_State) + माप(int16_t)*अंक_Cols;
    }
    size   += अंक_Rows * row_आकार;
    वापस size;
}

bool RBBITableBuilder::use8BitsForSafeTable() स्थिर {
    वापस fSafeTable->size() <= kMaxStateFor8BitsTable;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
व्योम RBBITableBuilder::exportSafeTable(व्योम *where) {
    RBBIStateTable* table = static_cast<RBBIStateTable*>(where);
    uint32_t           state;
    पूर्णांक                col;

    अगर (U_FAILURE(*fStatus) || fSafeTable == nullptr) {
        वापस;
    }

    int32_t cat_गिनती = fRB->fSetBuilder->get_अंक_Char_Categories();
    अगर (cat_गिनती > 0x7fff ||
            fSafeTable->size() > 0x7fff) {
        *fStatus = U_BRK_INTERNAL_ERROR;
        वापस;
    }

    table->f_अंक_States = fSafeTable->size();
    table->fFlags     = 0;
    अगर (use8BitsForSafeTable()) {
        table->fRowLen    = दुरत्व(RBBIStateTableRow8, f_अग्र_State) + माप(uint8_t) * cat_गिनती;
        table->fFlags  |= RBBI_8BITS_ROWS;
    } अन्यथा {
        table->fRowLen    = दुरत्व(RBBIStateTableRow16, f_अग्र_State) + माप(int16_t) * cat_गिनती;
    }

    क्रम (state=0; state<table->f_अंक_States; state++) {
        UnicodeString* rowString = static_cast<UnicodeString*>(fSafeTable->elementAt(state));
        RBBIStateTableRow* row = reinterpret_cast<RBBIStateTableRow*>(table->f_Table_तथ्य + state * table->fRowLen);
        अगर (use8BitsForSafeTable()) {
            RBBIStateTableRow8* r8 = reinterpret_cast<RBBIStateTableRow8*>(row);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->fTagsIdx    = 0;
            क्रम (col=0; col<cat_गिनती; col++) {
                U_ASSERT(rowString->charAt(col) <= kMaxStateFor8BitsTable);
                r8->f_अग्र_State[col] = static_cast<uint8_t>(rowString->charAt(col));
            }
        } अन्यथा {
            row->r16.fAccepting = 0;
            row->r16.fLookAhead = 0;
            row->r16.fTagsIdx    = 0;
            क्रम (col=0; col<cat_गिनती; col++) {
                row->r16.f_अग्र_State[col] = rowString->charAt(col);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//   printSet    Debug function.   Print the contents of a UVector
//
//-----------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG
व्योम RBBITableBuilder::printSet(UVector *s) {
    int32_t  i;
    क्रम (i=0; i<s->size(); i++) {
        स्थिर RBBINode *v = static_cast<स्थिर RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==nullptr? -1 : v->f_Serial_अंक);
    }
    RBBIDebugPrintf("\n");
}
#पूर्ण_अगर


//-----------------------------------------------------------------------------
//
//   printStates    Debug Function.  Dump the fully constructed state transition table.
//
//-----------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG
व्योम RBBITableBuilder::printStates() {
    पूर्णांक     c;    // input "character"
    पूर्णांक     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    क्रम (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    क्रम (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    क्रम (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->fTagsIdx);
        क्रम (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
            RBBIDebugPrintf(" %3d", sd->fDtran->elementAti(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#पूर्ण_अगर


//-----------------------------------------------------------------------------
//
//   printSafeTable    Debug Function.  Dump the fully constructed safe table.
//
//-----------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG
व्योम RBBITableBuilder::printReverseTable() {
    पूर्णांक     c;    // input "character"
    पूर्णांक     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    अगर (fSafeTable == nullptr) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        वापस;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    क्रम (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    क्रम (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    क्रम (n=0; n<fSafeTable->size(); n++) {
        UnicodeString *rowString = (UnicodeString *)fSafeTable->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        क्रम (c=0; c<fRB->fSetBuilder->get_अंक_Char_Categories(); c++) {
            RBBIDebugPrintf(" %2d", rowString->charAt(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#पूर्ण_अगर



//-----------------------------------------------------------------------------
//
//   printRuleStatusTable    Debug Function.  Dump the common rule status table
//
//-----------------------------------------------------------------------------
#अगर_घोषित RBBI_DEBUG
व्योम RBBITableBuilder::printRuleStatusTable() {
    int32_t  thisRecord = 0;
    int32_t  अग्र_Record = 0;
    पूर्णांक      i;
    UVector  *tbl = fRB->fRuleStatusVals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    जबतक (अग्र_Record < tbl->size()) {
        thisRecord = अग्र_Record;
        अग्र_Record = thisRecord + tbl->elementAti(thisRecord) + 1;
        RBBIDebugPrintf("%4d   ", thisRecord);
        क्रम (i=thisRecord+1; i<अग्र_Record; i++) {
            RBBIDebugPrintf("  %5d", tbl->elementAti(i));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#पूर्ण_अगर


//-----------------------------------------------------------------------------
//
//   RBBIStateDescriptor     Methods.  This is a very struct-like class
//                           Most access is directly to the fields.
//
//-----------------------------------------------------------------------------

RBBIStateDescriptor::RBBIStateDescriptor(पूर्णांक last_निवेश_प्रतीक, UErrorCode *fStatus) {
    fMarked    = false;
    fAccepting = 0;
    fLookAhead = 0;
    fTagsIdx   = 0;
    fTagVals   = nullptr;
    fPositions = nullptr;
    fDtran     = nullptr;

    fDtran     = नया UVector32(last_निवेश_प्रतीक+1, *fStatus);
    अगर (U_FAILURE(*fStatus)) {
        वापस;
    }
    अगर (fDtran == nullptr) {
        *fStatus = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    fDtran->set_आकार(last_निवेश_प्रतीक+1);    // fDtran needs to be pre-sized.
                                           //   It is indexed by input symbols, and will
                                           //   hold  the next state number for each
                                           //   symbol.
}


RBBIStateDescriptor::~RBBIStateDescriptor() {
    मिटाओ       fPositions;
    मिटाओ       fDtran;
    मिटाओ       fTagVals;
    fPositions = nullptr;
    fDtran     = nullptr;
    fTagVals   = nullptr;
}

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */
