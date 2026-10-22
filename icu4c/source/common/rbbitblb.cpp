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

#అన్తర్భావ <functional>

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "rbbitblb.h"
#అన్తర్భావ "rbbirb.h"
#అన్తర్భావ "rbbiscan.h"
#అన్తర్భావ "rbbisetb.h"
#అన్తర్భావ "rbbidata.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "uvectr32.h"
#అన్తర్భావ "cmemory.h"

U_NAMESPACE_BEGIN

నామాకాశ {

// Given the `RBBITableBuilder::fDStates` vector of `RBBIStateDescriptor`s, returns
// true if a state for which `isSink` returns true is reachable from state `source` by following
// transitions without going through any state for which `excludedState` returns true.
// Note that “going through” is distinct from “reaching”, and means entering and then leaving: A
// state can be both a sink and excluded, and can still be reachable.
తార్కిక reachableByTransitions(స్థిరరాశి UVector &స్థితులు, స్థిరరాశి int32_t ఆధారం,
                            స్థిరరాశి std::function<తార్కిక(int32_t)> isSink,
                            స్థిరరాశి std::function<తార్కిక(int32_t)> excludedState, UError_సంకేతం &స్థితి_2) {
    UStack boundary(స్థితి_2);
    {
        UVector32 &transitions_From_ఆధారం =
            *static_cast<RBBIStateDescriptor *>(స్థితులు.elementAt(ఆధారం))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        కొరకు (int32_t చిహ్నం = 0; చిహ్నం < transitions_From_ఆధారం.size(); ++చిహ్నం) {
            స్థిరరాశి int32_t state = transitions_From_ఆధారం.elementAti(చిహ్నం);
            ఒకవేళ (state != 0) {
                boundary.push(state, స్థితి_2);
            }
        }
    }
    స్థిరరాశి స్వయంచాలక సందర్శించిన = prv::make_unique<తార్కిక[]>(స్థితులు.size(), స్థితి_2);
    అయితే (U_SUCCESS(స్థితి_2) && !boundary.empty()) {
        స్థిరరాశి int32_t s = boundary.popi();
        ఒకవేళ (isSink(s)) {
            తిరిగి నిజం;
        }
        ఒకవేళ (excludedState(s) || సందర్శించిన[s]) {
            కొనసాగు;
        }
        సందర్శించిన[s] = నిజం;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(స్థితులు.elementAt(s))->fDtran;
        కొరకు (int32_t చిహ్నం = 0; చిహ్నం < transitions.size(); ++చిహ్నం) {
            స్థిరరాశి int32_t t = transitions.elementAti(చిహ్నం);
            ఒకవేళ (t != 0 && !సందర్శించిన[t]) {
                boundary.push(t, స్థితి_2);
            }
        }
    }
    తిరిగి అబద్ధం;
}

} // namespace

స్థిరరాశి int32_t k_Max_State_For8_బిట్లు_పట్టిక = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **మూలం_కణుపు, UError_సంకేతం &స్థితి_2) :
        fRB(rb),
        f_వృక్షం(*మూలం_కణుపు),
        f_స్థితి(&స్థితి_2),
        fDStates(శూన్యసూచిక),
        f_సురక్షిత_పట్టిక(శూన్యసూచిక) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = కొత్త UVector(స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2) && fDStates == శూన్యసూచిక ) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    పూర్ణాంకం i;
    కొరకు (i=0; i<fDStates->size(); i++) {
        తొలగించు static_cast<RBBIStateDescriptor*>(fDStates->elementAt(i));
    }
    తొలగించు fDStates;
    తొలగించు f_సురక్షిత_పట్టిక;
    తొలగించు f_Look_Ahead_నియమం_Map;
}


//-----------------------------------------------------------------------------
//
//   RBBITableBuilder::buildForwardTable  -  This is the main function for building
//                               the DFA state transition table from the RBBI rules parse tree.
//
//-----------------------------------------------------------------------------
శూన్య  RBBITableBuilder::build_Forward_పట్టిక() {

    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    ఒకవేళ (f_వృక్షం==శూన్యసూచిక) {
        తిరిగి;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_వృక్షం = f_వృక్షం->flattenVariables(*f_స్థితి, 0);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
#నిర్వచితమైతే RBBI_DEBUG
    ఒకవేళ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_వృక్షం(f_వృక్షం, నిజం);
    }
#అయితే_ముగింపు

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = కొత్త RBBINode(RBBINode::opCat, *f_స్థితి);
    // Exit if memory allocation failed.
    ఒకవేళ (cn == శూన్యసూచిక) {
        *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తొలగించు cn;
        తిరిగి;
    }
    cn->f_ఎడమ_బిడ్డ = f_వృక్షం;
    f_వృక్షం->f_మాతృ = cn;
    RBBINode *చివర_Marker_కణుపు = cn->f_కుడి_బిడ్డ = కొత్త RBBINode(RBBINode::చివర_Mark, *f_స్థితి);
    // Delete and exit if memory allocation failed.
    ఒకవేళ (cn->f_కుడి_బిడ్డ == శూన్యసూచిక) {
        *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తొలగించు cn;
        తిరిగి;
    }
    cn->f_కుడి_బిడ్డ->f_మాతృ = cn;
    f_వృక్షం = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_వృక్షం->flattenSets(*f_స్థితి, 0);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
#నిర్వచితమైతే RBBI_DEBUG
    ఒకవేళ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_వృక్షం(f_వృక్షం, నిజం);
    }
#అయితే_ముగింపు


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_వృక్షం);
    calc_మొదటి_Pos(f_వృక్షం);
    calc_చివరి_Pos(f_వృక్షం);
    calcFollowPos(f_వృక్షం);
    ఒకవేళ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_వృక్షం);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    ఒకవేళ (fRB->f_Chain_నియమాలు) {
        calcChainedFollowPos(f_వృక్షం, చివర_Marker_కణుపు);
    }

    //
    // Build the DFA state transition tables.
    //
    build_State_పట్టిక();
    map_Look_Ahead_నియమాలు();
    flag_Accepting_స్థితులు();
    flag_Look_Ahead_స్థితులు();
    flag_Tagged_స్థితులు();

    //
    // Update the global table of rule status {tag} values
    // The rule builder has a global vector of status values that are common
    //    for all tables.  Merge the ones from this table into the global set.
    //
    విలీనం_నియమం_స్థితి_Vals();
}



//-----------------------------------------------------------------------------
//
//   calcNullable.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
శూన్య RBBITableBuilder::calcNullable(RBBINode *n) {
    ఒకవేళ (n == శూన్యసూచిక) {
        తిరిగి;
    }
    ఒకవేళ (n->fType == RBBINode::సమితి_Ref ||
        n->fType == RBBINode::చివర_Mark ) {
        // These are non-empty leaf node types.
        n->fNullable = అబద్ధం;
        తిరిగి;
    }

    ఒకవేళ (n->fType == RBBINode::lookAhead || n->fType == RBBINode::ట్యాగ్) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = నిజం;
        తిరిగి;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_ఎడమ_బిడ్డ);
    calcNullable(n->f_కుడి_బిడ్డ);

    // Apply functions from table 3.40 in Aho
    ఒకవేళ (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_ఎడమ_బిడ్డ->fNullable || n->f_కుడి_బిడ్డ->fNullable;
    }
    లేకపోతే ఒకవేళ (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_ఎడమ_బిడ్డ->fNullable && n->f_కుడి_బిడ్డ->fNullable;
    }
    లేకపోతే ఒకవేళ (n->fType == RBBINode::op_నక్షత్రం || n->fType == RBBINode::opQuestion) {
        n->fNullable = నిజం;
    }
    లేకపోతే {
        n->fNullable = అబద్ధం;
    }
}




//-----------------------------------------------------------------------------
//
//   calcFirstPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
శూన్య RBBITableBuilder::calc_మొదటి_Pos(RBBINode *n) {
    ఒకవేళ (n == శూన్యసూచిక) {
        తిరిగి;
    }
    ఒకవేళ (n->fType == RBBINode::ఆకు_Char  ||
        n->fType == RBBINode::చివర_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::ట్యాగ్) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_మొదటి_Pos_సమితి->కూడు_Element(n, *f_స్థితి);
        తిరిగి;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calc_మొదటి_Pos(n->f_ఎడమ_బిడ్డ);
    calc_మొదటి_Pos(n->f_కుడి_బిడ్డ);

    // Apply functions from table 3.40 in Aho
    ఒకవేళ (n->fType == RBBINode::opOr) {
        సమితి_కూడు_2(n->f_మొదటి_Pos_సమితి, n->f_ఎడమ_బిడ్డ->f_మొదటి_Pos_సమితి);
        సమితి_కూడు_2(n->f_మొదటి_Pos_సమితి, n->f_కుడి_బిడ్డ->f_మొదటి_Pos_సమితి);
    }
    లేకపోతే ఒకవేళ (n->fType == RBBINode::opCat) {
        సమితి_కూడు_2(n->f_మొదటి_Pos_సమితి, n->f_ఎడమ_బిడ్డ->f_మొదటి_Pos_సమితి);
        ఒకవేళ (n->f_ఎడమ_బిడ్డ->fNullable) {
            సమితి_కూడు_2(n->f_మొదటి_Pos_సమితి, n->f_కుడి_బిడ్డ->f_మొదటి_Pos_సమితి);
        }
    }
    లేకపోతే ఒకవేళ (n->fType == RBBINode::op_నక్షత్రం ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        సమితి_కూడు_2(n->f_మొదటి_Pos_సమితి, n->f_ఎడమ_బిడ్డ->f_మొదటి_Pos_సమితి);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
శూన్య RBBITableBuilder::calc_చివరి_Pos(RBBINode *n) {
    ఒకవేళ (n == శూన్యసూచిక) {
        తిరిగి;
    }
    ఒకవేళ (n->fType == RBBINode::ఆకు_Char  ||
        n->fType == RBBINode::చివర_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::ట్యాగ్) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_చివరి_Pos_సమితి->కూడు_Element(n, *f_స్థితి);
        తిరిగి;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calc_చివరి_Pos(n->f_ఎడమ_బిడ్డ);
    calc_చివరి_Pos(n->f_కుడి_బిడ్డ);

    // Apply functions from table 3.40 in Aho
    ఒకవేళ (n->fType == RBBINode::opOr) {
        సమితి_కూడు_2(n->f_చివరి_Pos_సమితి, n->f_ఎడమ_బిడ్డ->f_చివరి_Pos_సమితి);
        సమితి_కూడు_2(n->f_చివరి_Pos_సమితి, n->f_కుడి_బిడ్డ->f_చివరి_Pos_సమితి);
    }
    లేకపోతే ఒకవేళ (n->fType == RBBINode::opCat) {
        సమితి_కూడు_2(n->f_చివరి_Pos_సమితి, n->f_కుడి_బిడ్డ->f_చివరి_Pos_సమితి);
        ఒకవేళ (n->f_కుడి_బిడ్డ->fNullable) {
            సమితి_కూడు_2(n->f_చివరి_Pos_సమితి, n->f_ఎడమ_బిడ్డ->f_చివరి_Pos_సమితి);
        }
    }
    లేకపోతే ఒకవేళ (n->fType == RBBINode::op_నక్షత్రం     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        సమితి_కూడు_2(n->f_చివరి_Pos_సమితి, n->f_ఎడమ_బిడ్డ->f_చివరి_Pos_సమితి);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
శూన్య RBBITableBuilder::calcFollowPos(RBBINode *n) {
    ఒకవేళ (n == శూన్యసూచిక ||
        n->fType == RBBINode::ఆకు_Char ||
        n->fType == RBBINode::చివర_Mark) {
        తిరిగి;
    }

    calcFollowPos(n->f_ఎడమ_బిడ్డ);
    calcFollowPos(n->f_కుడి_బిడ్డ);

    // Aho rule #1
    ఒకవేళ (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *చివరి_Pos_యొక్క_ఎడమ_బిడ్డ = n->f_ఎడమ_బిడ్డ->f_చివరి_Pos_సమితి;

        కొరకు (ix = 0; ix < static_cast<uint32_t>(చివరి_Pos_యొక్క_ఎడమ_బిడ్డ->size()); ix++) {
            i = static_cast<RBBINode*>(చివరి_Pos_యొక్క_ఎడమ_బిడ్డ->elementAt(ix));
            సమితి_కూడు_2(i->fFollowPos, n->f_కుడి_బిడ్డ->f_మొదటి_Pos_సమితి);
        }
    }

    // Aho rule #2
    ఒకవేళ (n->fType == RBBINode::op_నక్షత్రం ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        కొరకు (ix = 0; ix < static_cast<uint32_t>(n->f_చివరి_Pos_సమితి->size()); ix++) {
            i = static_cast<RBBINode*>(n->f_చివరి_Pos_సమితి->elementAt(ix));
            సమితి_కూడు_2(i->fFollowPos, n->f_మొదటి_Pos_సమితి);
        }
    }



}

//-----------------------------------------------------------------------------
//
//    addRuleRootNodes    Recursively walk a parse tree, adding all nodes flagged
//                        as roots of a rule to a destination vector.
//
//-----------------------------------------------------------------------------
శూన్య RBBITableBuilder::కూడు_నియమం_మూలం_Nodes(UVector *dest, RBBINode *కణుపు) {
    ఒకవేళ (కణుపు == శూన్యసూచిక || U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    U_ASSERT(!dest->hasDeleter());
    ఒకవేళ (కణుపు->f_నియమం_మూలం) {
        dest->కూడు_Element(కణుపు, *f_స్థితి);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        తిరిగి;
    }
    కూడు_నియమం_మూలం_Nodes(dest, కణుపు->f_ఎడమ_బిడ్డ);
    కూడు_నియమం_మూలం_Nodes(dest, కణుపు->f_కుడి_బిడ్డ);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
శూన్య RBBITableBuilder::calcChainedFollowPos(RBBINode *వృక్షం, RBBINode *చివర_Mark_కణుపు) {

    UVector         ఆకు_Nodes(*f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }

    // get a list all leaf nodes
    వృక్షం->కనుగొను_Nodes(&ఆకు_Nodes, RBBINode::ఆకు_Char, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector నియమం_మూలం_Nodes(*f_స్థితి);
    కూడు_నియమం_మూలం_Nodes(&నియమం_మూలం_Nodes, వృక్షం);

    UVector సరిపోలిక_ప్రారంభం_Nodes(*f_స్థితి);
    కొరకు (పూర్ణాంకం j=0; j<నియమం_మూలం_Nodes.size(); ++j) {
        RBBINode *కణుపు = static_cast<RBBINode *>(నియమం_మూలం_Nodes.elementAt(j));
        ఒకవేళ (కణుపు->f_Chain_లోపల) {
            సమితి_కూడు_2(&సరిపోలిక_ప్రారంభం_Nodes, కణుపు->f_మొదటి_Pos_సమితి);
        }
    }
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }

    int32_t  చివర_కణుపు_Ix;
    int32_t  ప్రారంభం_కణుపు_Ix;

    కొరకు (చివర_కణుపు_Ix=0; చివర_కణుపు_Ix<ఆకు_Nodes.size(); చివర_కణుపు_Ix++) {
        RBBINode* చివర_కణుపు = static_cast<RBBINode*>(ఆకు_Nodes.elementAt(చివర_కణుపు_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        ఒకవేళ (!చివర_కణుపు->fFollowPos->కలిగియుంది(చివర_Mark_కణుపు)) {
            కొనసాగు;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *ప్రారంభం_కణుపు;
        కొరకు (ప్రారంభం_కణుపు_Ix = 0; ప్రారంభం_కణుపు_Ix<సరిపోలిక_ప్రారంభం_Nodes.size(); ప్రారంభం_కణుపు_Ix++) {
            ప్రారంభం_కణుపు = static_cast<RBBINode*>(సరిపోలిక_ప్రారంభం_Nodes.elementAt(ప్రారంభం_కణుపు_Ix));
            ఒకవేళ (ప్రారంభం_కణుపు->fType != RBBINode::ఆకు_Char) {
                కొనసాగు;
            }

            ఒకవేళ (చివర_కణుపు->fVal == ప్రారంభం_కణుపు->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                సమితి_కూడు_2(చివర_కణుపు->fFollowPos, ప్రారంభం_కణుపు->fFollowPos);
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
శూన్య RBBITableBuilder::bofFixup() {

    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
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
    RBBINode  *bof_కణుపు = f_వృక్షం->f_ఎడమ_బిడ్డ->f_ఎడమ_బిడ్డ;
    U_ASSERT(bof_కణుపు->fType == RBBINode::ఆకు_Char);
    U_ASSERT(bof_కణుపు->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *సరిపోలిక_ప్రారంభం_Nodes = f_వృక్షం->f_ఎడమ_బిడ్డ->f_కుడి_బిడ్డ->f_మొదటి_Pos_సమితి;

    RBBINode *ప్రారంభం_కణుపు;
    పూర్ణాంకం       ప్రారంభం_కణుపు_Ix;
    కొరకు (ప్రారంభం_కణుపు_Ix = 0; ప్రారంభం_కణుపు_Ix<సరిపోలిక_ప్రారంభం_Nodes->size(); ప్రారంభం_కణుపు_Ix++) {
        ప్రారంభం_కణుపు = static_cast<RBBINode*>(సరిపోలిక_ప్రారంభం_Nodes->elementAt(ప్రారంభం_కణుపు_Ix));
        ఒకవేళ (ప్రారంభం_కణుపు->fType != RBBINode::ఆకు_Char) {
            కొనసాగు;
        }

        ఒకవేళ (ప్రారంభం_కణుపు->fVal == bof_కణుపు->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            సమితి_కూడు_2(bof_కణుపు->fFollowPos, ప్రారంభం_కణుపు->fFollowPos);
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
శూన్య RBBITableBuilder::build_State_పట్టిక() {
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = శూన్యసూచిక;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    పూర్ణాంకం      చివరి_ఆదానం_చిహ్నం = fRB->f_సమితి_Builder->get_అంకె_Char_Categories() - 1;
    failState = కొత్త RBBIStateDescriptor(చివరి_ఆదానం_చిహ్నం, f_స్థితి);
    ఒకవేళ (failState == శూన్యసూచిక) {
        *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        వెళ్ళు ExitBuildSTdeleteall;
    }
    failState->fPositions = కొత్త UVector(*f_స్థితి);
    ఒకవేళ (failState->fPositions == శూన్యసూచిక) {
        *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    ఒకవేళ (failState->fPositions == శూన్యసూచిక || U_FAILURE(*f_స్థితి)) {
        వెళ్ళు ExitBuildSTdeleteall;
    }
    fDStates->కూడు_Element(failState, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        వెళ్ళు ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = కొత్త RBBIStateDescriptor(చివరి_ఆదానం_చిహ్నం, f_స్థితి);
    ఒకవేళ (initialState == శూన్యసూచిక) {
        *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        వెళ్ళు ExitBuildSTdeleteall;
    }
    initialState->fPositions = కొత్త UVector(*f_స్థితి);
    ఒకవేళ (initialState->fPositions == శూన్యసూచిక) {
        *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        వెళ్ళు ExitBuildSTdeleteall;
    }
    సమితి_కూడు_2(initialState->fPositions, f_వృక్షం->f_మొదటి_Pos_సమితి);
    fDStates->కూడు_Element(initialState, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        వెళ్ళు ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    కొరకు (;;) {
        RBBIStateDescriptor *T = శూన్యసూచిక;
        int32_t              tx;
        కొరకు (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *తాత్కాలిక;
            తాత్కాలిక = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            ఒకవేళ (తాత్కాలిక->fMarked == అబద్ధం) {
                T = తాత్కాలిక;
                ఆపు;
            }
        }
        ఒకవేళ (T == శూన్యసూచిక) {
            ఆపు;
        }

        // mark T;
        T->fMarked = నిజం;

        // for each input symbol a do begin
        int32_t  a;
        కొరకు (a = 1; a<=చివరి_ఆదానం_చిహ్నం; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = శూన్యసూచిక;
            RBBINode   *p;
            int32_t     px;
            కొరకు (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                ఒకవేళ ((p->fType == RBBINode::ఆకు_Char) &&  (p->fVal == a)) {
                    ఒకవేళ (U == శూన్యసూచిక) {
                        U = కొత్త UVector(*f_స్థితి);
                        ఒకవేళ (U == శూన్యసూచిక) {
                        	*f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                        	వెళ్ళు ExitBuildSTdeleteall;
                        }
                    }
                    సమితి_కూడు_2(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = అబద్ధం;
            ఒకవేళ (U != శూన్యసూచిక) {
                U_ASSERT(U->size() > 0);
                పూర్ణాంకం  ix;
                కొరకు (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    ఒకవేళ (సమితి_Equals(U, temp2->fPositions)) {
                        తొలగించు U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = నిజం;
                        ఆపు;
                    }
                }

                // Add U as an unmarked state to Dstates
                ఒకవేళ (!UinDstates)
                {
                    RBBIStateDescriptor *newState = కొత్త RBBIStateDescriptor(చివరి_ఆదానం_చిహ్నం, f_స్థితి);
                    ఒకవేళ (newState == శూన్యసూచిక) {
                    	*f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    }
                    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
                        వెళ్ళు ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->కూడు_Element(newState, *f_స్థితి);
                    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
                        తిరిగి;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->సమితి_Element_At(ux, a);
            }
        }
    }
    తిరిగి;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    తొలగించు initialState;
    తొలగించు failState;
}


/**
 * mapLookAheadRules
 *
 */
శూన్య RBBITableBuilder::map_Look_Ahead_నియమాలు() {
    f_Look_Ahead_నియమం_Map =  కొత్త UVector32(fRB->fScanner->అంకె_నియమాలు() + 1, *f_స్థితి);
    ఒకవేళ (f_Look_Ahead_నియమం_Map == శూన్యసూచిక) {
        *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    f_Look_Ahead_నియమం_Map->సమితి_కొలత(fRB->fScanner->అంకె_నియమాలు() + 1);

    కొరకు (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t la_స్థానం_For_State = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        తార్కిక saw_Look_Ahead_కణుపు = అబద్ధం;
        కొరకు (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *కణుపు = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            ఒకవేళ (కణుపు->fType != RBBINode::కణుపు_Type::lookAhead) {
                కొనసాగు;
            }
            saw_Look_Ahead_కణుపు = నిజం;
            int32_t నియమం_అంకె = కణుపు->fVal;     // Set when rule was originally parsed.
            U_ASSERT(నియమం_అంకె < f_Look_Ahead_నియమం_Map->size());
            U_ASSERT(నియమం_అంకె > 0);
            int32_t la_స్థానం = f_Look_Ahead_నియమం_Map->elementAti(నియమం_అంకె);
            ఒకవేళ (la_స్థానం != 0) {
                ఒకవేళ (la_స్థానం_For_State == 0) {
                    la_స్థానం_For_State = la_స్థానం;
                } లేకపోతే {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(la_స్థానం == la_స్థానం_For_State);
                }
            }
        }
        ఒకవేళ (!saw_Look_Ahead_కణుపు) {
            కొనసాగు;
        }

        ఒకవేళ (la_స్థానం_For_State == 0) {
            la_స్థానం_For_State = ++f_LASlots_లోపల_Use;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        కొరకు (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *కణుపు = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            ఒకవేళ (కణుపు->fType != RBBINode::కణుపు_Type::lookAhead) {
                కొనసాగు;
            }
            int32_t నియమం_అంకె = కణుపు->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = f_Look_Ahead_నియమం_Map->elementAti(నియమం_అంకె);
            (శూన్య)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == la_స్థానం_For_State);
            f_Look_Ahead_నియమం_Map->సమితి_Element_At(la_స్థానం_For_State, నియమం_అంకె);
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
శూన్య     RBBITableBuilder::flag_Accepting_స్థితులు() {
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    UVector     చివర_Marker_Nodes(*f_స్థితి);
    RBBINode    *చివర_Marker;
    int32_t     i;
    int32_t     n;

    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }

    f_వృక్షం->కనుగొను_Nodes(&చివర_Marker_Nodes, RBBINode::చివర_Mark, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }

    కొరకు (i=0; i<చివర_Marker_Nodes.size(); i++) {
        చివర_Marker = static_cast<RBBINode*>(చివర_Marker_Nodes.elementAt(i));
        కొరకు (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            ఒకవేళ (sd->fPositions->సూచీ_యొక్క(చివర_Marker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                ఒకవేళ (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = f_Look_Ahead_నియమం_Map->elementAti(చివర_Marker->fVal);
                    ఒకవేళ (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                ఒకవేళ (sd->fAccepting==ACCEPTING_UNCONDITIONAL && చివర_Marker->fVal != 0) {
                    // Both lookahead and non-lookahead accepting for this state.
                    // Favor the look-ahead, because a look-ahead match needs to
                    // immediately stop the run-time engine. First match, not longest.
                    sd->fAccepting = f_Look_Ahead_నియమం_Map->elementAti(చివర_Marker->fVal);
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
శూన్య     RBBITableBuilder::flag_Look_Ahead_స్థితులు() {
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    UVector     lookAheadNodes(*f_స్థితి);
    RBBINode    *look_Ahead_కణుపు;
    int32_t     i;
    int32_t     n;

    f_వృక్షం->కనుగొను_Nodes(&lookAheadNodes, RBBINode::lookAhead, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    కొరకు (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_కణుపు = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_కణుపు->fType == RBBINode::కణుపు_Type::lookAhead);

        కొరకు (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->సూచీ_యొక్క(look_Ahead_కణుపు);
            ఒకవేళ (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_కణుపు == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookahead_స్థానం = f_Look_Ahead_నియమం_Map->elementAti(look_Ahead_కణుపు->fVal);
                U_ASSERT(sd->fLookAhead == 0 || sd->fLookAhead == lookahead_స్థానం);
                // if (sd->fLookAhead != 0 && sd->fLookAhead != lookaheadSlot) {
                //     printf("%s:%d Bingo. sd->fLookAhead:%d   lookaheadSlot:%d\n",
                //            __FILE__, __LINE__, sd->fLookAhead, lookaheadSlot);
                // }
                sd->fLookAhead = lookahead_స్థానం;
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//    flagTaggedStates
//
//-----------------------------------------------------------------------------
శూన్య     RBBITableBuilder::flag_Tagged_స్థితులు() {
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    UVector     ట్యాగ్_Nodes(*f_స్థితి);
    RBBINode    *ట్యాగ్_కణుపు;
    int32_t     i;
    int32_t     n;

    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    f_వృక్షం->కనుగొను_Nodes(&ట్యాగ్_Nodes, RBBINode::ట్యాగ్, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    కొరకు (i=0; i<ట్యాగ్_Nodes.size(); i++) {                   // For each tag node t (all of 'em)
        ట్యాగ్_కణుపు = static_cast<RBBINode*>(ట్యాగ్_Nodes.elementAt(i));

        కొరకు (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            ఒకవేళ (sd->fPositions->సూచీ_యొక్క(ట్యాగ్_కణుపు) >= 0) {  //       if  s include the tag node t
                sorted_కూడు(&sd->f_ట్యాగ్_Vals, ట్యాగ్_కణుపు->fVal);
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
శూన్య  RBBITableBuilder::విలీనం_నియమం_స్థితి_Vals() {
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
    పూర్ణాంకం i;
    పూర్ణాంకం n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    ఒకవేళ (fRB->f_నియమం_స్థితి_Vals->size() == 0) {
        fRB->f_నియమం_స్థితి_Vals->కూడు_Element(1, *f_స్థితి);  // Num of statuses in group
        fRB->f_నియమం_స్థితి_Vals->కూడు_Element(static_cast<int32_t>(0), *f_స్థితి); // and our single status of zero
    }

    //    For each state
    కొరకు (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_స్థితులు_ట్యాగ్_విలువలు = sd->f_ట్యాగ్_Vals;
        ఒకవేళ (this_స్థితులు_ట్యాగ్_విలువలు == శూన్యసూచిక) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->f_ట్యాగులు_Idx = 0;
            కొనసాగు;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->f_ట్యాగులు_Idx = -1;
        int32_t  this_ట్యాగ్_సమూహం_ప్రారంభం = 0;   // indexes into the global rule status vals list
        int32_t  తదుపరి_ట్యాగ్_సమూహం_ప్రారంభం = 0;

        // Loop runs once per group of tags in the global list
        అయితే (తదుపరి_ట్యాగ్_సమూహం_ప్రారంభం < fRB->f_నియమం_స్థితి_Vals->size()) {
            this_ట్యాగ్_సమూహం_ప్రారంభం = తదుపరి_ట్యాగ్_సమూహం_ప్రారంభం;
            తదుపరి_ట్యాగ్_సమూహం_ప్రారంభం += fRB->f_నియమం_స్థితి_Vals->elementAti(this_ట్యాగ్_సమూహం_ప్రారంభం) + 1;
            ఒకవేళ (this_స్థితులు_ట్యాగ్_విలువలు->size() != fRB->f_నియమం_స్థితి_Vals->elementAti(this_ట్యాగ్_సమూహం_ప్రారంభం)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                కొనసాగు;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            కొరకు (i=0; i<this_స్థితులు_ట్యాగ్_విలువలు->size(); i++) {
                ఒకవేళ (this_స్థితులు_ట్యాగ్_విలువలు->elementAti(i) !=
                    fRB->f_నియమం_స్థితి_Vals->elementAti(this_ట్యాగ్_సమూహం_ప్రారంభం + 1 + i) ) {
                    // Mismatch.
                    ఆపు;
                }
            }

            ఒకవేళ (i == this_స్థితులు_ట్యాగ్_విలువలు->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->f_ట్యాగులు_Idx = this_ట్యాగ్_సమూహం_ప్రారంభం;
                ఆపు;
            }
        }

        ఒకవేళ (sd->f_ట్యాగులు_Idx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->f_ట్యాగులు_Idx = fRB->f_నియమం_స్థితి_Vals->size();
            fRB->f_నియమం_స్థితి_Vals->కూడు_Element(this_స్థితులు_ట్యాగ్_విలువలు->size(), *f_స్థితి);
            కొరకు (i=0; i<this_స్థితులు_ట్యాగ్_విలువలు->size(); i++) {
                fRB->f_నియమం_స్థితి_Vals->కూడు_Element(this_స్థితులు_ట్యాగ్_విలువలు->elementAti(i), *f_స్థితి);
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

శూన్య RBBITableBuilder::minimize_స్థితులు() {
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }

#ఒకవేళ UPRV_HAS_SANITIZER
    ఒకవేళ (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *f_స్థితి = U_REGEX_నమూనా_TOO_BIG;
        తిరిగి;
    }
#అయితే_ముగింపు
    నిర్మాణం StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t f_ట్యాగులు_Idx;
        తార్కిక క్రియాకారకం==(StateType స్థిరరాశి &other) స్థిరరాశి {
            తిరిగి fAccepting == other.fAccepting &&
                   fLookAhead == other.fLookAhead &&
                   f_ట్యాగులు_Idx == other.f_ట్యాగులు_Idx;
        }
    };
    // We wrap `UVector`s in `unique_ptr``s throughout so we can move them,
    // including into `UVector`s (by releasing them from the `unique_ptr`
    // and having the enclosing `UVector` adopt them). 
    // Group the states by types (but we have no maps so this is verbose).
    // If there are no lookaheads and no tags, there are only two types
    // (accepting and non-accepting) in which case this is exactly step 1
    // of Algorithm 3.6.
    నిర్మాణం Type_గమ్యం_స్థితులు : UMemory {
        Type_గమ్యం_స్థితులు(స్థిరరాశి StateType &type, UError_సంకేతం &స్థితి_2)
            : type(type), స్థితులు(prv::make_unique<UVector>(స్థితి_2, స్థితి_2)) {}
        StateType type;
        prv::unique_ptr<UVector> స్థితులు;
    };
    UVector initial_విభజన(*f_స్థితి);
    initial_విభజన.సమితి_Deleter(
        [](శూన్య *p) { తొలగించు static_cast<Type_గమ్యం_స్థితులు *>(p); });
    కొరకు (int32_t i = 0; i < fDStates->size(); ++i) {
        స్థిరరాశి RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        స్థిరరాశి StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.f_ట్యాగులు_Idx};
        int32_t j = 0;
        కొరకు (; j < initial_విభజన.size(); ++j) {
            స్వయంచాలక &[type_j, స్థితులు] =
                *static_cast<Type_గమ్యం_స్థితులు *>(initial_విభజన.elementAt(j));
            ఒకవేళ (type_j == type) {
                స్థితులు->కూడు_Element(i, *f_స్థితి);
                ఆపు;
            }
        }
        ఒకవేళ (j == initial_విభజన.size()) {
            స్వయంచాలక new_నమోదు = prv::make_unique<Type_గమ్యం_స్థితులు>(type, *f_స్థితి, *f_స్థితి);
            ఒకవేళ (U_FAILURE(*f_స్థితి)) {
                తిరిగి;
            }
            new_నమోదు->స్థితులు->కూడు_Element(i, *f_స్థితి);
            initial_విభజన.adoptElement(new_నమోదు.release(), *f_స్థితి);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    స్వయంచాలక విభజన = prv::make_unique<UVector>(*f_స్థితి, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    విభజన->సమితి_Deleter(
        [](శూన్య *p) { తొలగించు static_cast<UVector*>(p); });
    
    కొరకు (int32_t i = 0; i < initial_విభజన.size(); ++i) {
        విభజన->adoptElement(
            static_cast<Type_గమ్యం_స్థితులు*>(initial_విభజన.elementAt(i))->స్థితులు.release(),
            *f_స్థితి);
    }
    // Given the index of a state 𝑠 in `fDStates`, returns a UVector of integers
    // σ(𝑠) such that 𝑠.fDtran[i] (the index of the state reached by the
    // transition from 𝑠 on the character class with index i) is in Π[σ(𝑠)[i]].
    // We then have σ(𝑠) == σ(𝑡) if and only if “for all input symbols 𝑎, states
    // 𝑠 and 𝑡 have transitions on 𝑎 to states in the same group of Π” (which is
    // what defines the refinement of 𝐺 in Fig. 3.45).
    స్వయంచాలక విభజన_signature = [&విభజన, ఇది](int32_t state_సూచీ) -> prv::unique_ptr<UVector> {
        స్థిరరాశి RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state_సూచీ));
        స్వయంచాలక ఫలితం = prv::make_unique<UVector>(state.fDtran->size(), *f_స్థితి, *f_స్థితి);
        ఒకవేళ (U_FAILURE(*f_స్థితి)) {
            తిరిగి శూన్యసూచిక;
        }
        కొరకు (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t గమ్యం_State = state.fDtran->elementAti(i);
            కొరకు (int32_t part_సూచీ = 0; part_సూచీ < విభజన->size(); ++part_సూచీ) {
                స్థిరరాశి UVector &part = *static_cast<UVector *>(విభజన->elementAt(part_సూచీ));
                ఒకవేళ (part.కలిగియుంది(గమ్యం_State)) {
                    ఫలితం->కూడు_Element(part_సూచీ, *f_స్థితి);
                    ఆపు;
                }
            }
        }
        ఫలితం->సమితి_Comparer([](స్థిరరాశి UElement ఎడమ, స్థిరరాశి UElement కుడి) -> UBool {
            తిరిగి ఎడమ.integer == కుడి.integer;
        });
        తిరిగి ఫలితం;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    కొరకు (;;) {
        // Π_new.
        స్వయంచాలక విభజన_New = prv::make_unique<UVector>(*f_స్థితి, *f_స్థితి);
        ఒకవేళ (U_FAILURE(*f_స్థితి)) {
            తిరిగి;
        }
        విభజన_New->సమితి_Deleter(
            [](శూన్య *p) { తొలగించు static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        తార్కిక refined = అబద్ధం;
        కొరకు (int32_t i = 0; i < విభజన->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            స్థిరరాశి UVector &సమూహం = *static_cast<UVector*>(విభజన->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            నిర్మాణం Signature_గమ్యం_స్థితులు : UMemory {
                Signature_గమ్యం_స్థితులు(prv::unique_ptr<UVector> signature, UError_సంకేతం &స్థితి_2)
                    : signature(std::move(signature)), స్థితులు(prv::make_unique<UVector>(స్థితి_2, స్థితి_2)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> స్థితులు;
            };
            UVector సమూహం_Refinement(*f_స్థితి);
            సమూహం_Refinement.సమితి_Deleter([](శూన్య *p) { తొలగించు static_cast<Signature_గమ్యం_స్థితులు*>(p); });
            కొరకు (int32_t j = 0; j < సమూహం.size(); ++j) {
                // Index of a state in the group.
                స్థిరరాశి int32_t s = సమూహం.elementAti(j);
                స్వయంచాలక signature = విభజన_signature(s);
                int32_t k = 0;
                కొరకు (; k < సమూహం_Refinement.size(); ++k) {
                    స్థిరరాశి స్వయంచాలక &[subgroupSignature, subgroup] =
                        *static_cast<Signature_గమ్యం_స్థితులు*>(సమూహం_Refinement.elementAt(k));
                    ఒకవేళ (*subgroupSignature == *signature) {
                        subgroup->కూడు_Element(s, *f_స్థితి);
                        ఆపు;
                    }
                }
                ఒకవేళ (k == సమూహం_Refinement.size()) {
                    స్థిరరాశి స్వయంచాలక new_నమోదు =
                        కొత్త Signature_గమ్యం_స్థితులు(std::move(signature), *f_స్థితి);
                    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
                        తిరిగి;
                    }
                    ఒకవేళ (new_నమోదు == శూన్యసూచిక) {
                      *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                      తిరిగి;
                    }
                    new_నమోదు->స్థితులు->కూడు_Element(s, *f_స్థితి);
                    సమూహం_Refinement.adoptElement(new_నమోదు, *f_స్థితి);
                }
            }
            refined |= సమూహం_Refinement.size() > 1;
            కొరకు (int32_t j = 0; j < సమూహం_Refinement.size(); ++j) {
                స్వయంచాలక &[_, subgroup] = *static_cast<Signature_గమ్యం_స్థితులు*>(
                    సమూహం_Refinement.elementAt(j));
                విభజన_New->adoptElement(subgroup.release(), *f_స్థితి);
            }
        }
        ఒకవేళ (refined) {
            విభజన = std::move(విభజన_New);
        } లేకపోతే {
            ఆపు;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    కొరకు (పూర్ణాంకం i = 0; i < విభజన->size(); ++i) {
        UVector *స్థిరరాశి part_i = static_cast<UVector *>(విభజన->elementAt(i));
        ఒకవేళ (part_i->కలిగియుంది(0)) {
            శూన్య *స్థిరరాశి part_0 = విభజన->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            స్థిరరాశి స్వయంచాలక deleter = విభజన->సమితి_Deleter(శూన్యసూచిక);
            విభజన->సమితి_Element_At(part_i, 0);
            విభజన->సమితి_Element_At(part_0, i);
            విభజన->సమితి_Deleter(deleter);
        } లేకపోతే ఒకవేళ (part_i->కలిగియుంది(1)) {
            శూన్య *స్థిరరాశి part_1 = విభజన->elementAt(1);
            స్థిరరాశి స్వయంచాలక deleter = విభజన->సమితి_Deleter(శూన్యసూచిక);
            విభజన->సమితి_Element_At(part_i, 1);
            విభజన->సమితి_Element_At(part_1, i);
            విభజన->సమితి_Deleter(deleter);
        }
    }
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    స్వయంచాలక old_State_గమ్యం_Part = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    కొరకు (పూర్ణాంకం i = 0; i < విభజన->size(); ++i) {
        స్థిరరాశి UVector &part = *static_cast<UVector*>(విభజన->elementAt(i));
        కొరకు (పూర్ణాంకం j = 0; j < part.size(); ++j) {
            old_State_గమ్యం_Part[part.elementAti(j)] = i;
        }
    }
    స్వయంచాలక స్థిరరాశి old_స్థితులు = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*f_స్థితి, *f_స్థితి).release();
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    కొరకు (పూర్ణాంకం i = 0; i < విభజన->size(); ++i) {
        స్థిరరాశి UVector &part = *static_cast<UVector*>(విభజన->elementAt(i));
        RBBIStateDescriptor *స్థిరరాశి state =
            static_cast<RBBIStateDescriptor*>(old_స్థితులు->elementAt(part.elementAti(0)));
        fDStates->కూడు_Element(state, *f_స్థితి);
        కొరకు (పూర్ణాంకం j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->సమితి_Element_At(old_State_గమ్యం_Part[state->fDtran->elementAti(j)], j);
        }
        old_స్థితులు->సమితి_Element_At(శూన్యసూచిక, part.elementAti(0));
    }
    old_స్థితులు->సమితి_Deleter([](శూన్య *p) { తొలగించు static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
శూన్య RBBITableBuilder::sorted_కూడు(UVector **vector, int32_t val) {
    int32_t i;

    ఒకవేళ (*vector == శూన్యసూచిక) {
        *vector = కొత్త UVector(*f_స్థితి);
    }
    ఒకవేళ (*vector == శూన్యసూచిక || U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    UVector *vec = *vector;
    int32_t  v_కొలత = vec->size();
    కొరకు (i=0; i<v_కొలత; i++) {
        int32_t valAtI = vec->elementAti(i);
        ఒకవేళ (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            తిరిగి;
        }
        ఒకవేళ (valAtI > val) {
            ఆపు;
        }
    }
    vec->చొప్పించు_Element_At(val, i, *f_స్థితి);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
శూన్య RBBITableBuilder::సమితి_కూడు_2(UVector *dest, UVector *ఆధారం) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!ఆధారం->hasDeleter());
    int32_t dest_Original_కొలత = dest->size();
    int32_t ఆధారం_కొలత       = ఆధారం->size();
    int32_t di           = 0;
    Maybe_దొంతర_Array<శూన్య *, 16> destArray, ఆధారం_Array;  // Handle small cases without malloc
    శూన్య **destPtr, **ఆధారం_Ptr;
    శూన్య **destLim, **ఆధారం_Lim;

    ఒకవేళ (dest_Original_కొలత > destArray.get_సామర్థ్యం()) {
        ఒకవేళ (destArray.resize(dest_Original_కొలత) == శూన్యసూచిక) {
            తిరిగి;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + dest_Original_కొలత;  // destArray.getArrayLimit()?

    ఒకవేళ (ఆధారం_కొలత > ఆధారం_Array.get_సామర్థ్యం()) {
        ఒకవేళ (ఆధారం_Array.resize(ఆధారం_కొలత) == శూన్యసూచిక) {
            తిరిగి;
        }
    }
    ఆధారం_Ptr = ఆధారం_Array.getAlias();
    ఆధారం_Lim = ఆధారం_Ptr + ఆధారం_కొలత;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (శూన్య) dest->గమ్యం_Array(destPtr);
    (శూన్య) ఆధారం->గమ్యం_Array(ఆధారం_Ptr);

    dest->సమితి_కొలత(ఆధారం_కొలత+dest_Original_కొలత, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }

    అయితే (ఆధారం_Ptr < ఆధారం_Lim && destPtr < destLim) {
        ఒకవేళ (*destPtr == *ఆధారం_Ptr) {
            dest->సమితి_Element_At(*ఆధారం_Ptr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        లేకపోతే ఒకవేళ (uprv_memcmp(destPtr, ఆధారం_Ptr, పరిమాణం(శూన్య *)) < 0) {
            dest->సమితి_Element_At(*destPtr++, di++);
        }
        లేకపోతే { /* *sourcePtr < *destPtr */
            dest->సమితి_Element_At(*ఆధారం_Ptr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    అయితే (destPtr < destLim) {
        dest->సమితి_Element_At(*destPtr++, di++);
    }
    అయితే (ఆధారం_Ptr < ఆధారం_Lim) {
        dest->సమితి_Element_At(*ఆధారం_Ptr++, di++);
    }

    dest->సమితి_కొలత(di, *f_స్థితి);
}



//-----------------------------------------------------------------------------
//
//  setEqual    Set operation on UVector.
//              Compare for equality.
//              Elements must be sorted.
//
//-----------------------------------------------------------------------------
UBool RBBITableBuilder::సమితి_Equals(UVector *a, UVector *b) {
    తిరిగి a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG
శూన్య RBBITableBuilder::printPosSets(RBBINode *n) {
    ఒకవేళ (n==శూన్యసూచిక) {
        తిరిగి;
    }
    రాయి("\n");
    RBBINode::print_కణుపు_శీర్షిక();
    RBBINode::print_కణుపు(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    print_సమితి(n->f_మొదటి_Pos_సమితి);

    RBBIDebugPrintf("         lastpos:   ");
    print_సమితి(n->f_చివరి_Pos_సమితి);

    RBBIDebugPrintf("         followpos: ");
    print_సమితి(n->fFollowPos);

    printPosSets(n->f_ఎడమ_బిడ్డ);
    printPosSets(n->f_కుడి_బిడ్డ);
}
#అయితే_ముగింపు

//
//    findDuplCharClassFrom()
//
తార్కిక RBBITableBuilder::కనుగొను_Dupl_Char_Class_From(IntPair *categories) {
    int32_t అంకె_స్థితులు = fDStates->size();
    int32_t అంకె_నిలువుల_సంఖ్య = fRB->f_సమితి_Builder->get_అంకె_Char_Categories();

    కొరకు (; categories->first < అంకె_నిలువుల_సంఖ్య-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        పూర్ణాంకం హద్దు_రెండవ = categories->first < fRB->f_సమితి_Builder->get_Dict_Categories_ప్రారంభం() ?
            fRB->f_సమితి_Builder->get_Dict_Categories_ప్రారంభం() : అంకె_నిలువుల_సంఖ్య;
        కొరకు (categories->రెండవ=categories->first+1; categories->రెండవ < హద్దు_రెండవ; categories->రెండవ++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t పట్టిక_పునాది = 0;
            uint16_t పట్టిక_dupl = 1;
            కొరకు (int32_t state=0; state<అంకె_స్థితులు; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                పట్టిక_పునాది = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                పట్టిక_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->రెండవ));
                ఒకవేళ (పట్టిక_పునాది != పట్టిక_dupl) {
                    ఆపు;
                }
            }
            ఒకవేళ (పట్టిక_పునాది == పట్టిక_dupl) {
                తిరిగి నిజం;
            }
        }
    }
    తిరిగి అబద్ధం;
}


//
//    removeColumn()
//
శూన్య RBBITableBuilder::remove_స్తంభం(int32_t స్తంభం) {
    int32_t అంకె_స్థితులు = fDStates->size();
    కొరకు (int32_t state=0; state<అంకె_స్థితులు; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(స్తంభం < sd->fDtran->size());
        sd->fDtran->removeElementAt(స్తంభం);
    }
}


తార్కిక RBBITableBuilder::కనుగొను_Duplicate_సురక్షిత_State(IntPair *స్థితులు) {
    int32_t అంకె_స్థితులు = f_సురక్షిత_పట్టిక->size();

    కొరకు (; స్థితులు->first<అంకె_స్థితులు-1; స్థితులు->first++) {
        UnicodeString *మొదటి_అడ్డువరుస = static_cast<UnicodeString *>(f_సురక్షిత_పట్టిక->elementAt(స్థితులు->first));
        కొరకు (స్థితులు->రెండవ=స్థితులు->first+1; స్థితులు->రెండవ<అంకె_స్థితులు; స్థితులు->రెండవ++) {
            UnicodeString *dupl_అడ్డువరుస = static_cast<UnicodeString *>(f_సురక్షిత_పట్టిక->elementAt(స్థితులు->రెండవ));
            తార్కిక అడ్డువరుసలు_సరిపోలిక = నిజం;
            int32_t అంకె_నిలువుల_సంఖ్య = మొదటి_అడ్డువరుస->length();
            కొరకు (int32_t నిలువు=0; నిలువు < అంకె_నిలువుల_సంఖ్య; ++నిలువు) {
                int32_t మొదటి_Val = మొదటి_అడ్డువరుస->charAt(నిలువు);
                int32_t duplVal = dupl_అడ్డువరుస->charAt(నిలువు);
                ఒకవేళ (!((మొదటి_Val == duplVal) ||
                        ((మొదటి_Val == స్థితులు->first || మొదటి_Val == స్థితులు->రెండవ) &&
                        (duplVal  == స్థితులు->first || duplVal  == స్థితులు->రెండవ)))) {
                    అడ్డువరుసలు_సరిపోలిక = అబద్ధం;
                    ఆపు;
                }
            }
            ఒకవేళ (అడ్డువరుసలు_సరిపోలిక) {
                తిరిగి నిజం;
            }
        }
    }
    తిరిగి అబద్ధం;
}

శూన్య RBBITableBuilder::remove_సురక్షిత_State(IntPair dupl_స్థితులు) {
    స్థిరరాశి int32_t keepState = dupl_స్థితులు.first;
    స్థిరరాశి int32_t duplState = dupl_స్థితులు.రెండవ;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < f_సురక్షిత_పట్టిక->size());

    f_సురక్షిత_పట్టిక->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t అంకె_స్థితులు = f_సురక్షిత_పట్టిక->size();
    కొరకు (int32_t state=0; state<అంకె_స్థితులు; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(f_సురక్షిత_పట్టిక->elementAt(state));
        int32_t అంకె_నిలువుల_సంఖ్య = sd->length();
        కొరకు (int32_t నిలువు=0; నిలువు<అంకె_నిలువుల_సంఖ్య; నిలువు++) {
            int32_t existingVal = sd->charAt(నిలువు);
            int32_t newVal = existingVal;
            ఒకవేళ (existingVal == duplState) {
                newVal = keepState;
            } లేకపోతే ఒకవేళ (existingVal > duplState) {
                newVal = existingVal - 1;
            }
            sd->సమితి_Char_At(నిలువు, static_cast<char16_t>(newVal));
        }
    }
}


/*
 * RemoveDuplicateStates
 */
int32_t RBBITableBuilder::remove_Duplicate_స్థితులు() {
    స్థిరరాశి int32_t old_State_లెక్క = fDStates->size();
    minimize_స్థితులు();
    తిరిగి fDStates->size() - old_State_లెక్క;
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
శూన్య RBBITableBuilder::minimizeLookaheads() {
    ఒకవేళ (f_LASlots_లోపల_Use == ACCEPTING_UNCONDITIONAL) {
        తిరిగి;
    }
    స్థిరరాశి int32_t lookahead_లెక్క = f_LASlots_లోపల_Use - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    స్థిరరాశి int32_t మొదటి_Lookahead = ACCEPTING_UNCONDITIONAL + 1;
    స్థిరరాశి స్వయంచాలక stateDescriptor = [ఇది](స్థిరరాశి int32_t state) -> RBBIStateDescriptor & {
        తిరిగి *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    స్థిరరాశి స్వయంచాలక lookaheadReachability =
        prv::make_unique<తార్కిక[]>(lookahead_లెక్క * lookahead_లెక్క, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    కొరకు (int32_t l = మొదటి_Lookahead; l <= f_LASlots_లోపల_Use; ++l) {
        కొరకు (int32_t k = మొదటి_Lookahead; k <= f_LASlots_లోపల_Use; ++k) {
            ఒకవేళ (k != l) {
                కొరకు (int32_t ఆధారం = 1; ఆధారం < fDStates->size(); ++ఆధారం) {
                    ఒకవేళ (static_cast<int32_t>(stateDescriptor(ఆధారం).fLookAhead) != l) {
                        కొనసాగు;
                    }
                    ఒకవేళ (reachableByTransitions(
                            *fDStates, ఆధారం, /*isSink=*/
                            [&](స్థిరరాశి int32_t state) {
                                తిరిగి static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](స్థిరరాశి int32_t state) {
                                తిరిగి static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *f_స్థితి)) {
                        lookaheadReachability[(l - మొదటి_Lookahead) * lookahead_లెక్క + k -
                                              మొదటి_Lookahead] = నిజం;
                    }
                }
            }
        }
    }

    స్థిరరాశి స్వయంచాలక colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_లెక్క, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    కొరకు (int32_t chromatic_సంఖ్య = 1;; ++chromatic_సంఖ్య) {
        uprv_memset(colours.get(), 0, పరిమాణం(int32_t) * lookahead_లెక్క);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        కొరకు (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            కొరకు (పూర్ణాంకం ఆధారం = 0; ఆధారం < lookahead_లెక్క; ++ఆధారం) {
                కొరకు (పూర్ణాంకం sink = 0; sink < lookahead_లెక్క; ++sink) {
                    ఒకవేళ (lookaheadReachability[ఆధారం * lookahead_లెక్క + sink] &&
                        colours[ఆధారం] == colours[sink]) {
                        వెళ్ళు తదుపరి_Colouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            కొరకు (పూర్ణాంకం i = 0; i < fDStates->size(); ++i) {
                ఒకవేళ (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        మొదటి_Lookahead +
                        colours[stateDescriptor(i).fAccepting - మొదటి_Lookahead];
                }
                ఒకవేళ (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - మొదటి_Lookahead];
                }
            }
            f_LASlots_లోపల_Use = ACCEPTING_UNCONDITIONAL + chromatic_సంఖ్య;
            తిరిగి;

          తదుపరి_Colouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            కొరకు (int32_t i = 0; i < lookahead_లెక్క - 1 && colours[i] == chromatic_సంఖ్య;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            ఒకవేళ (colours[lookahead_లెక్క - 1] == chromatic_సంఖ్య) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                ఆపు;
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
int32_t  RBBITableBuilder::get_పట్టిక_కొలత() స్థిరరాశి {
    int32_t    size = 0;
    int32_t    అంకె_అడ్డువరుసలు;
    int32_t    అంకె_నిలువుల_సంఖ్య;
    int32_t    అడ్డువరుస_కొలత;

    ఒకవేళ (f_వృక్షం == శూన్యసూచిక) {
        తిరిగి 0;
    }

    size    = offsetof(RBBIState_పట్టిక, f_పట్టిక_దత్తాంశం);    // The header, with no rows to the table.

    అంకె_అడ్డువరుసలు = fDStates->size();
    అంకె_నిలువుల_సంఖ్య = fRB->f_సమితి_Builder->get_అంకె_Char_Categories();

    ఒకవేళ (use8_బిట్లు_For_పట్టిక()) {
        అడ్డువరుస_కొలత = offsetof(RBBIState_పట్టిక_Row8, f_తదుపరి_State) + పరిమాణం(int8_t)*అంకె_నిలువుల_సంఖ్య;
    } లేకపోతే {
        అడ్డువరుస_కొలత = offsetof(RBBIState_పట్టిక_Row16, f_తదుపరి_State) + పరిమాణం(int16_t)*అంకె_నిలువుల_సంఖ్య;
    }
    size   += అంకె_అడ్డువరుసలు * అడ్డువరుస_కొలత;
    తిరిగి size;
}

తార్కిక RBBITableBuilder::use8_బిట్లు_For_పట్టిక() స్థిరరాశి {
    తిరిగి fDStates->size() <= k_Max_State_For8_బిట్లు_పట్టిక;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
శూన్య RBBITableBuilder::export_పట్టిక(శూన్య *where) {
    RBBIState_పట్టిక* పట్టిక = static_cast<RBBIState_పట్టిక*>(where);
    uint32_t           state;
    పూర్ణాంకం                నిలువు;

    ఒకవేళ (U_FAILURE(*f_స్థితి) || f_వృక్షం == శూన్యసూచిక) {
        తిరిగి;
    }

    int32_t cat_లెక్క = fRB->f_సమితి_Builder->get_అంకె_Char_Categories();
    ఒకవేళ (cat_లెక్క > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *f_స్థితి = U_BRK_అంతర్గత_ERROR;
        తిరిగి;
    }

    పట్టిక->f_అంకె_స్థితులు = fDStates->size();
    పట్టిక->f_Dict_Categories_ప్రారంభం = fRB->f_సమితి_Builder->get_Dict_Categories_ప్రారంభం();
    పట్టిక->f_Look_Ahead_Results_కొలత = f_LASlots_లోపల_Use == ACCEPTING_UNCONDITIONAL ? 0 : f_LASlots_లోపల_Use + 1;
    పట్టిక->fFlags     = 0;
    ఒకవేళ (use8_బిట్లు_For_పట్టిక()) {
        పట్టిక->f_అడ్డువరుస_Len    = offsetof(RBBIState_పట్టిక_Row8, f_తదుపరి_State) + పరిమాణం(uint8_t) * cat_లెక్క;
        పట్టిక->fFlags  |= RBBI_8_బిట్లు_అడ్డువరుసలు;
    } లేకపోతే {
        పట్టిక->f_అడ్డువరుస_Len    = offsetof(RBBIState_పట్టిక_Row16, f_తదుపరి_State) + పరిమాణం(int16_t) * cat_లెక్క;
    }
    ఒకవేళ (fRB->fLookAheadHardBreak) {
        పట్టిక->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    కొరకు (state=0; state<పట్టిక->f_అంకె_స్థితులు; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIState_పట్టిక_అడ్డువరుస* అడ్డువరుస = reinterpret_cast<RBBIState_పట్టిక_అడ్డువరుస*>(పట్టిక->f_పట్టిక_దత్తాంశం + state * పట్టిక->f_అడ్డువరుస_Len);
        ఒకవేళ (use8_బిట్లు_For_పట్టిక()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->f_ట్యాగులు_Idx && sd->f_ట్యాగులు_Idx <= 255);
            RBBIState_పట్టిక_Row8* r8 = reinterpret_cast<RBBIState_పట్టిక_Row8*>(అడ్డువరుస);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->f_ట్యాగులు_Idx   = sd->f_ట్యాగులు_Idx;
            కొరకు (నిలువు=0; నిలువు<cat_లెక్క; నిలువు++) {
                U_ASSERT (sd->fDtran->elementAti(నిలువు) <= k_Max_State_For8_బిట్లు_పట్టిక);
                r8->f_తదుపరి_State[నిలువు] = sd->fDtran->elementAti(నిలువు);
            }
        } లేకపోతే {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->f_ట్యాగులు_Idx && sd->f_ట్యాగులు_Idx <= 0xffff);
            అడ్డువరుస->r16.fAccepting = sd->fAccepting;
            అడ్డువరుస->r16.fLookAhead = sd->fLookAhead;
            అడ్డువరుస->r16.f_ట్యాగులు_Idx   = sd->f_ట్యాగులు_Idx;
            కొరకు (నిలువు=0; నిలువు<cat_లెక్క; నిలువు++) {
                అడ్డువరుస->r16.f_తదుపరి_State[నిలువు] = sd->fDtran->elementAti(నిలువు);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
శూన్య RBBITableBuilder::build_సురక్షిత_తిరగేయి_పట్టిక(UError_సంకేతం &స్థితి_2) {
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
    UnicodeString సురక్షిత_జతలు;

    int32_t అంకె_Char_Classes = fRB->f_సమితి_Builder->get_అంకె_Char_Categories();
    int32_t అంకె_స్థితులు = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    స్థిరరాశి స్వయంచాలక lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_లోపల_Use + 1, స్థితి_2);
    స్థిరరాశి స్వయంచాలక wanted_Lookaheads_లోపల_Pair =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_లోపల_Use + 1, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t ప్రారంభం_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_సమితి = -1;
    కొరకు (int32_t c1 = 1; c1 < అంకె_Char_Classes; ++c1) {
        ఒకవేళ (fRB->f_సమితి_Builder->get_మొదటి_Char(c1) < 0) {
          కొనసాగు;
        }
        కొరకు (int32_t c2 = 1; c2 < అంకె_Char_Classes; ++c2) {
            ఒకవేళ (fRB->f_సమితి_Builder->get_మొదటి_Char(c2) < 0) {
                కొనసాగు;
            }
            int32_t wanted_చివర_State = -1;
            int32_t wanted_చివరి_Accepting_Position;
            తార్కిక must_Check_Consistent_With_ప్రారంభం_C2 = అబద్ధం;
            స్థిరరాశి int32_t పాఠ్యం[2] = {c1, c2};
            కొరకు (int32_t injectedState = ప్రారంభం_STATE; injectedState < అంకె_స్థితులు; ++injectedState) {
                కొరకు (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= f_LASlots_లోపల_Use; ++injectedLookahead) {
                    int32_t చివరి_Break = BEFORE_PAIR;
                    int32_t చివరి_Accepting_Position = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_సమితి, f_LASlots_లోపల_Use + 1);
                    ఒకవేళ (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        స్థిరరాశి RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        ఒకవేళ (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            చివరి_Accepting_Position = 0;
                        } లేకపోతే ఒకవేళ (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            కొనసాగు;
                        }
                        ఒకవేళ (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    వెళ్ళు dfa_అడుగు;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    కొరకు (;;) {
                        s = ప్రారంభం_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_సమితి, f_LASlots_లోపల_Use + 1);
                        i = చివరి_Break;
                        // Loop Find_Next_Break in L2/26-135.
                        కొరకు (;;) {
                        dfa_అడుగు:
                            ఒకవేళ (i == BEFORE_PAIR || i == 2) {
                                వెళ్ళు inspectFinalState;
                            }
                            స్థిరరాశి int32_t చిహ్నం_Ahead = పాఠ్యం[i];
                            ++i;
                            స్థిరరాశి RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            ఒకవేళ (sDescriptor->fDtran->elementAti(చిహ్నం_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(చిహ్నం_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } లేకపోతే {
                                ఒకవేళ (చివరి_Break != BEFORE_PAIR &&
                                    చివరి_Accepting_Position == చివరి_Break) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    వెళ్ళు తదుపరి_చిహ్నం_Pair;
                                }
                                చివరి_Break = చివరి_Accepting_Position;
                                ఆపు;
                            }

                            ఒకవేళ (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                చివరి_Accepting_Position = i;
                            } లేకపోతే ఒకవేళ (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_సమితి) {
                                చివరి_Break = lookaheadPositions[sDescriptor->fAccepting];
                                ఆపు;
                            }
                            ఒకవేళ (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    ఒకవేళ (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        కొనసాగు;
                    }
                    ఒకవేళ (wanted_చివర_State < 0) {
                        wanted_చివర_State = s;
                        wanted_చివరి_Accepting_Position = చివరి_Accepting_Position;
                        కొరకు (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_లోపల_Use; ++l) {
                            ఒకవేళ (lookaheadPositions[l] != LOOKAHEAD_NOT_సమితి &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wanted_Lookaheads_లోపల_Pair[l] = lookaheadPositions[i];
                            } లేకపోతే {
                                wanted_Lookaheads_లోపల_Pair[l] = LOOKAHEAD_NOT_సమితి;
                            }
                        }
                    } లేకపోతే {
                        ఒకవేళ (wanted_చివర_State != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            వెళ్ళు తదుపరి_చిహ్నం_Pair;
                        }
                        ఒకవేళ (wanted_చివరి_Accepting_Position != చివరి_Accepting_Position) {
                            వెళ్ళు తదుపరి_చిహ్నం_Pair;
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
                    కొరకు (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_లోపల_Use; ++l) {
                        ఒకవేళ (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wanted_Lookaheads_లోపల_Pair[l]) {
                            కొనసాగు;
                        }
                        ఒకవేళ (lookaheadPositions[l] == 2 || wanted_Lookaheads_లోపల_Pair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            ఒకవేళ (wanted_చివర_State != ప్రారంభం_STATE) {
                                వెళ్ళు తదుపరి_చిహ్నం_Pair;
                            }
                        }
                        ఒకవేళ (lookaheadPositions[l] == 1 || wanted_Lookaheads_లోపల_Pair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            must_Check_Consistent_With_ప్రారంభం_C2 = నిజం;
                        }
                    }
                }
            }
            ఒకవేళ (must_Check_Consistent_With_ప్రారంభం_C2) {
                స్థిరరాశి RBBIStateDescriptor& ప్రారంభం_Descriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(ప్రారంభం_STATE));
                స్థిరరాశి స్వయంచాలక c2State = ప్రారంభం_Descriptor.fDtran->elementAti(c2);
                ఒకవేళ (c2State != wanted_చివర_State) {
                    వెళ్ళు తదుపరి_చిహ్నం_Pair;
                }
                స్థిరరాశి RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                ఒకవేళ (wanted_చివరి_Accepting_Position != 2) {
                    వెళ్ళు తదుపరి_చిహ్నం_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                కొరకు (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_లోపల_Use; ++l) {
                    ఒకవేళ (((wanted_Lookaheads_లోపల_Pair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wanted_చివర_State != ప్రారంభం_STATE) {
                        వెళ్ళు తదుపరి_చిహ్నం_Pair;
                    }
                }
            }
            సురక్షిత_జతలు.append(static_cast<char16_t>(c1));
            సురక్షిత_జతలు.append(static_cast<char16_t>(c2));
        తదుపరి_చిహ్నం_Pair:
            కొనసాగు;
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

    U_ASSERT(f_సురక్షిత_పట్టిక == శూన్యసూచిక);
    Local_సూచిక<UVector> lp_సురక్షిత_పట్టిక(
        కొత్త UVector(uprv_deleteUObject, uhash_compareUnicodeString, అంకె_Char_Classes + 2, స్థితి_2), స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    f_సురక్షిత_పట్టిక = lp_సురక్షిత_పట్టిక.orphan();
    కొరకు (int32_t అడ్డువరుస=0; అడ్డువరుస<అంకె_Char_Classes + 2; ++అడ్డువరుస) {
        Local_సూచిక<UnicodeString> lpString(కొత్త UnicodeString(అంకె_Char_Classes, 0, అంకె_Char_Classes+4), స్థితి_2);
        f_సురక్షిత_పట్టిక->adoptElement(lpString.orphan(), స్థితి_2);
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &ప్రారంభం_State = *static_cast<UnicodeString *>(f_సురక్షిత_పట్టిక->elementAt(1));
    కొరకు (int32_t charClass=0; charClass < అంకె_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        ప్రారంభం_State.సమితి_Char_At(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    కొరకు (int32_t అడ్డువరుస=2; అడ్డువరుస<అంకె_Char_Classes+2; ++అడ్డువరుస) {
        UnicodeString &అడ్డువరుస_State = *static_cast<UnicodeString *>(f_సురక్షిత_పట్టిక->elementAt(అడ్డువరుస));
        అడ్డువరుస_State = ప్రారంభం_State;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    కొరకు (int32_t pairIdx=0; pairIdx<సురక్షిత_జతలు.length(); pairIdx+=2) {
        int32_t c1 = సురక్షిత_జతలు.charAt(pairIdx);
        int32_t c2 = సురక్షిత_జతలు.charAt(pairIdx + 1);

        UnicodeString &అడ్డువరుస_State = *static_cast<UnicodeString *>(f_సురక్షిత_పట్టిక->elementAt(c2 + 2));
        అడ్డువరుస_State.సమితి_Char_At(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair స్థితులు = {1, 0};
    అయితే (కనుగొను_Duplicate_సురక్షిత_State(&స్థితులు)) {
        // printf("Removing duplicate safe states (%d, %d)\n", states.first, states.second);
        remove_సురక్షిత_State(స్థితులు);
    }
}


//-----------------------------------------------------------------------------
//
//   getSafeTableSize()    Calculate the size of the runtime form of this
//                         safe state table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::get_సురక్షిత_పట్టిక_కొలత() స్థిరరాశి {
    int32_t    size = 0;
    int32_t    అంకె_అడ్డువరుసలు;
    int32_t    అంకె_నిలువుల_సంఖ్య;
    int32_t    అడ్డువరుస_కొలత;

    ఒకవేళ (f_సురక్షిత_పట్టిక == శూన్యసూచిక) {
        తిరిగి 0;
    }

    size    = offsetof(RBBIState_పట్టిక, f_పట్టిక_దత్తాంశం);    // The header, with no rows to the table.

    అంకె_అడ్డువరుసలు = f_సురక్షిత_పట్టిక->size();
    అంకె_నిలువుల_సంఖ్య = fRB->f_సమితి_Builder->get_అంకె_Char_Categories();

    ఒకవేళ (use8_బిట్లు_For_సురక్షిత_పట్టిక()) {
        అడ్డువరుస_కొలత = offsetof(RBBIState_పట్టిక_Row8, f_తదుపరి_State) + పరిమాణం(int8_t)*అంకె_నిలువుల_సంఖ్య;
    } లేకపోతే {
        అడ్డువరుస_కొలత = offsetof(RBBIState_పట్టిక_Row16, f_తదుపరి_State) + పరిమాణం(int16_t)*అంకె_నిలువుల_సంఖ్య;
    }
    size   += అంకె_అడ్డువరుసలు * అడ్డువరుస_కొలత;
    తిరిగి size;
}

తార్కిక RBBITableBuilder::use8_బిట్లు_For_సురక్షిత_పట్టిక() స్థిరరాశి {
    తిరిగి f_సురక్షిత_పట్టిక->size() <= k_Max_State_For8_బిట్లు_పట్టిక;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
శూన్య RBBITableBuilder::export_సురక్షిత_పట్టిక(శూన్య *where) {
    RBBIState_పట్టిక* పట్టిక = static_cast<RBBIState_పట్టిక*>(where);
    uint32_t           state;
    పూర్ణాంకం                నిలువు;

    ఒకవేళ (U_FAILURE(*f_స్థితి) || f_సురక్షిత_పట్టిక == శూన్యసూచిక) {
        తిరిగి;
    }

    int32_t cat_లెక్క = fRB->f_సమితి_Builder->get_అంకె_Char_Categories();
    ఒకవేళ (cat_లెక్క > 0x7fff ||
            f_సురక్షిత_పట్టిక->size() > 0x7fff) {
        *f_స్థితి = U_BRK_అంతర్గత_ERROR;
        తిరిగి;
    }

    పట్టిక->f_అంకె_స్థితులు = f_సురక్షిత_పట్టిక->size();
    పట్టిక->fFlags     = 0;
    ఒకవేళ (use8_బిట్లు_For_సురక్షిత_పట్టిక()) {
        పట్టిక->f_అడ్డువరుస_Len    = offsetof(RBBIState_పట్టిక_Row8, f_తదుపరి_State) + పరిమాణం(uint8_t) * cat_లెక్క;
        పట్టిక->fFlags  |= RBBI_8_బిట్లు_అడ్డువరుసలు;
    } లేకపోతే {
        పట్టిక->f_అడ్డువరుస_Len    = offsetof(RBBIState_పట్టిక_Row16, f_తదుపరి_State) + పరిమాణం(int16_t) * cat_లెక్క;
    }

    కొరకు (state=0; state<పట్టిక->f_అంకె_స్థితులు; state++) {
        UnicodeString* అడ్డువరుస_String = static_cast<UnicodeString*>(f_సురక్షిత_పట్టిక->elementAt(state));
        RBBIState_పట్టిక_అడ్డువరుస* అడ్డువరుస = reinterpret_cast<RBBIState_పట్టిక_అడ్డువరుస*>(పట్టిక->f_పట్టిక_దత్తాంశం + state * పట్టిక->f_అడ్డువరుస_Len);
        ఒకవేళ (use8_బిట్లు_For_సురక్షిత_పట్టిక()) {
            RBBIState_పట్టిక_Row8* r8 = reinterpret_cast<RBBIState_పట్టిక_Row8*>(అడ్డువరుస);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->f_ట్యాగులు_Idx    = 0;
            కొరకు (నిలువు=0; నిలువు<cat_లెక్క; నిలువు++) {
                U_ASSERT(అడ్డువరుస_String->charAt(నిలువు) <= k_Max_State_For8_బిట్లు_పట్టిక);
                r8->f_తదుపరి_State[నిలువు] = static_cast<uint8_t>(అడ్డువరుస_String->charAt(నిలువు));
            }
        } లేకపోతే {
            అడ్డువరుస->r16.fAccepting = 0;
            అడ్డువరుస->r16.fLookAhead = 0;
            అడ్డువరుస->r16.f_ట్యాగులు_Idx    = 0;
            కొరకు (నిలువు=0; నిలువు<cat_లెక్క; నిలువు++) {
                అడ్డువరుస->r16.f_తదుపరి_State[నిలువు] = అడ్డువరుస_String->charAt(నిలువు);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//   printSet    Debug function.   Print the contents of a UVector
//
//-----------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG
శూన్య RBBITableBuilder::print_సమితి(UVector *s) {
    int32_t  i;
    కొరకు (i=0; i<s->size(); i++) {
        స్థిరరాశి RBBINode *v = static_cast<స్థిరరాశి RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==శూన్యసూచిక? -1 : v->f_Serial_అంకె);
    }
    RBBIDebugPrintf("\n");
}
#అయితే_ముగింపు


//-----------------------------------------------------------------------------
//
//   printStates    Debug Function.  Dump the fully constructed state transition table.
//
//-----------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG
శూన్య RBBITableBuilder::print_స్థితులు() {
    పూర్ణాంకం     c;    // input "character"
    పూర్ణాంకం     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    కొరకు (c=0; c<fRB->f_సమితి_Builder->get_అంకె_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    కొరకు (c=0; c<fRB->f_సమితి_Builder->get_అంకె_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    కొరకు (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->f_ట్యాగులు_Idx);
        కొరకు (c=0; c<fRB->f_సమితి_Builder->get_అంకె_Char_Categories(); c++) {
            RBBIDebugPrintf(" %3d", sd->fDtran->elementAti(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#అయితే_ముగింపు


//-----------------------------------------------------------------------------
//
//   printSafeTable    Debug Function.  Dump the fully constructed safe table.
//
//-----------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG
శూన్య RBBITableBuilder::print_తిరగేయి_పట్టిక() {
    పూర్ణాంకం     c;    // input "character"
    పూర్ణాంకం     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    ఒకవేళ (f_సురక్షిత_పట్టిక == శూన్యసూచిక) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        తిరిగి;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    కొరకు (c=0; c<fRB->f_సమితి_Builder->get_అంకె_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    కొరకు (c=0; c<fRB->f_సమితి_Builder->get_అంకె_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    కొరకు (n=0; n<f_సురక్షిత_పట్టిక->size(); n++) {
        UnicodeString *అడ్డువరుస_String = (UnicodeString *)f_సురక్షిత_పట్టిక->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        కొరకు (c=0; c<fRB->f_సమితి_Builder->get_అంకె_Char_Categories(); c++) {
            RBBIDebugPrintf(" %2d", అడ్డువరుస_String->charAt(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#అయితే_ముగింపు



//-----------------------------------------------------------------------------
//
//   printRuleStatusTable    Debug Function.  Dump the common rule status table
//
//-----------------------------------------------------------------------------
#నిర్వచితమైతే RBBI_DEBUG
శూన్య RBBITableBuilder::print_నియమం_స్థితి_పట్టిక() {
    int32_t  this_రికార్డు = 0;
    int32_t  తదుపరి_రికార్డు = 0;
    పూర్ణాంకం      i;
    UVector  *tbl = fRB->f_నియమం_స్థితి_Vals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    అయితే (తదుపరి_రికార్డు < tbl->size()) {
        this_రికార్డు = తదుపరి_రికార్డు;
        తదుపరి_రికార్డు = this_రికార్డు + tbl->elementAti(this_రికార్డు) + 1;
        RBBIDebugPrintf("%4d   ", this_రికార్డు);
        కొరకు (i=this_రికార్డు+1; i<తదుపరి_రికార్డు; i++) {
            RBBIDebugPrintf("  %5d", tbl->elementAti(i));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#అయితే_ముగింపు


//-----------------------------------------------------------------------------
//
//   RBBIStateDescriptor     Methods.  This is a very struct-like class
//                           Most access is directly to the fields.
//
//-----------------------------------------------------------------------------

RBBIStateDescriptor::RBBIStateDescriptor(పూర్ణాంకం చివరి_ఆదానం_చిహ్నం, UError_సంకేతం *f_స్థితి) {
    fMarked    = అబద్ధం;
    fAccepting = 0;
    fLookAhead = 0;
    f_ట్యాగులు_Idx   = 0;
    f_ట్యాగ్_Vals   = శూన్యసూచిక;
    fPositions = శూన్యసూచిక;
    fDtran     = శూన్యసూచిక;

    fDtran     = కొత్త UVector32(చివరి_ఆదానం_చిహ్నం+1, *f_స్థితి);
    ఒకవేళ (U_FAILURE(*f_స్థితి)) {
        తిరిగి;
    }
    ఒకవేళ (fDtran == శూన్యసూచిక) {
        *f_స్థితి = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    fDtran->సమితి_కొలత(చివరి_ఆదానం_చిహ్నం+1);    // fDtran needs to be pre-sized.
                                           //   It is indexed by input symbols, and will
                                           //   hold  the next state number for each
                                           //   symbol.
}


RBBIStateDescriptor::~RBBIStateDescriptor() {
    తొలగించు       fPositions;
    తొలగించు       fDtran;
    తొలగించు       f_ట్యాగ్_Vals;
    fPositions = శూన్యసూచిక;
    fDtran     = శూన్యసూచిక;
    f_ట్యాగ్_Vals   = శూన్యసూచిక;
}

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */
