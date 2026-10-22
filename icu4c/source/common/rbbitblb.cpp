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

#ಅನ್ತರ್ಭಾವ <functional>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_BREAK_ITERATION

#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "rbbitblb.h"
#ಅನ್ತರ್ಭಾವ "rbbirb.h"
#ಅನ್ತರ್ಭಾವ "rbbiscan.h"
#ಅನ್ತರ್ಭಾವ "rbbisetb.h"
#ಅನ್ತರ್ಭಾವ "rbbidata.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

// Given the `RBBITableBuilder::fDStates` vector of `RBBIStateDescriptor`s, returns
// true if a state for which `isSink` returns true is reachable from state `source` by following
// transitions without going through any state for which `excludedState` returns true.
// Note that “going through” is distinct from “reaching”, and means entering and then leaving: A
// state can be both a sink and excluded, and can still be reachable.
ತಾರ್ಕಿಕ reachableByTransitions(ಸ್ಥಿರಾಂಕ UVector &ಸ್ಥಿತಿಗಳು, ಸ್ಥಿರಾಂಕ int32_t ಮೂಲ,
                            ಸ್ಥಿರಾಂಕ std::function<ತಾರ್ಕಿಕ(int32_t)> isSink,
                            ಸ್ಥಿರಾಂಕ std::function<ತಾರ್ಕಿಕ(int32_t)> excludedState, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    UStack boundary(ಸ್ಥಿತಿ_2);
    {
        UVector32 &transitions_From_ಮೂಲ =
            *static_cast<RBBIStateDescriptor *>(ಸ್ಥಿತಿಗಳು.elementAt(ಮೂಲ))->fDtran;
        // We do not initialize boundary to `{source}`, but instead to the set of states one
        // transition away from `source`; if `source` sets lookahead l and accepts lookahead k, we
        // only need k and l to occupy distinct slots if there is a `source`-to-`source` path.
        ಗಾಗಿ (int32_t ಚಿಹ್ನೆ = 0; ಚಿಹ್ನೆ < transitions_From_ಮೂಲ.size(); ++ಚಿಹ್ನೆ) {
            ಸ್ಥಿರಾಂಕ int32_t state = transitions_From_ಮೂಲ.elementAti(ಚಿಹ್ನೆ);
            ಆದರೆ (state != 0) {
                boundary.push(state, ಸ್ಥಿತಿ_2);
            }
        }
    }
    ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ ಭೇಟಿಯಾದ = prv::make_unique<ತಾರ್ಕಿಕ[]>(ಸ್ಥಿತಿಗಳು.size(), ಸ್ಥಿತಿ_2);
    ಆಗ (U_SUCCESS(ಸ್ಥಿತಿ_2) && !boundary.empty()) {
        ಸ್ಥಿರಾಂಕ int32_t s = boundary.popi();
        ಆದರೆ (isSink(s)) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        ಆದರೆ (excludedState(s) || ಭೇಟಿಯಾದ[s]) {
            ಮುಂದುವರಿ;
        }
        ಭೇಟಿಯಾದ[s] = ನಿಜ;
        UVector32 &transitions = *static_cast<RBBIStateDescriptor *>(ಸ್ಥಿತಿಗಳು.elementAt(s))->fDtran;
        ಗಾಗಿ (int32_t ಚಿಹ್ನೆ = 0; ಚಿಹ್ನೆ < transitions.size(); ++ಚಿಹ್ನೆ) {
            ಸ್ಥಿರಾಂಕ int32_t t = transitions.elementAti(ಚಿಹ್ನೆ);
            ಆದರೆ (t != 0 && !ಭೇಟಿಯಾದ[t]) {
                boundary.push(t, ಸ್ಥಿತಿ_2);
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

} // namespace

ಸ್ಥಿರಾಂಕ int32_t k_Max_State_For8_ಬಿಟ್ಟುಗಳು_ಕೋಷ್ಟಕ = 255;

RBBITableBuilder::RBBITableBuilder(RBBIRuleBuilder *rb, RBBINode **ಬೇರು_ಗಂಟು, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
        fRB(rb),
        f_ವೃಕ್ಷ(*ಬೇರು_ಗಂಟು),
        f_ಸ್ಥಿತಿ(&ಸ್ಥಿತಿ_2),
        fDStates(ಶೂನ್ಯಸೂಚಕ),
        f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ(ಶೂನ್ಯಸೂಚಕ) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    // fDStates is UVector<RBBIStateDescriptor *>
    fDStates = ಹೊಸ UVector(ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && fDStates == ಶೂನ್ಯಸೂಚಕ ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}



RBBITableBuilder::~RBBITableBuilder() {
    ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ (i=0; i<fDStates->size(); i++) {
        ಅಳಿಸು static_cast<RBBIStateDescriptor*>(fDStates->elementAt(i));
    }
    ಅಳಿಸು fDStates;
    ಅಳಿಸು f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ;
    ಅಳಿಸು f_Look_Ahead_ನಿಯಮ_Map;
}


//-----------------------------------------------------------------------------
//
//   RBBITableBuilder::buildForwardTable  -  This is the main function for building
//                               the DFA state transition table from the RBBI rules parse tree.
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ  RBBITableBuilder::build_Forward_ಕೋಷ್ಟಕ() {

    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }

    // If there were no rules, just return.  This situation can easily arise
    //   for the reverse rules.
    ಆದರೆ (f_ವೃಕ್ಷ==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }

    //
    // Walk through the tree, replacing any references to $variables with a copy of the
    //   parse tree for the substitution expression.
    //
    f_ವೃಕ್ಷ = f_ವೃಕ್ಷ->flattenVariables(*f_ಸ್ಥಿತಿ, 0);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    ಆದರೆ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "ftree")) {
        RBBIDebugPuts("\nParse tree after flattening variable references.");
        RBBINode::print_ವೃಕ್ಷ(f_ವೃಕ್ಷ, ನಿಜ);
    }
#ಆದರೆ_ಅಂತ್ಯ

    //
    // Add a unique right-end marker to the expression.
    //   Appears as a cat-node, left child being the original tree,
    //   right child being the end marker.
    //
    RBBINode *cn = ಹೊಸ RBBINode(RBBINode::opCat, *f_ಸ್ಥಿತಿ);
    // Exit if memory allocation failed.
    ಆದರೆ (cn == ಶೂನ್ಯಸೂಚಕ) {
        *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಅಳಿಸು cn;
        ಹಿಂತಿರುಗಿ;
    }
    cn->f_ಎಡ_ಮಗು = f_ವೃಕ್ಷ;
    f_ವೃಕ್ಷ->f_ಪೋಷಕ = cn;
    RBBINode *ಕೊನೆ_Marker_ಗಂಟು = cn->f_ಬಲ_ಮಗು = ಹೊಸ RBBINode(RBBINode::ಕೊನೆ_Mark, *f_ಸ್ಥಿತಿ);
    // Delete and exit if memory allocation failed.
    ಆದರೆ (cn->f_ಬಲ_ಮಗು == ಶೂನ್ಯಸೂಚಕ) {
        *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಅಳಿಸು cn;
        ಹಿಂತಿರುಗಿ;
    }
    cn->f_ಬಲ_ಮಗು->f_ಪೋಷಕ = cn;
    f_ವೃಕ್ಷ = cn;

    //
    //  Replace all references to UnicodeSets with the tree for the equivalent
    //      expression.
    //
    f_ವೃಕ್ಷ->flattenSets(*f_ಸ್ಥಿತಿ, 0);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
    ಆದರೆ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "stree")) {
        RBBIDebugPuts("\nParse tree after flattening Unicode Set references.");
        RBBINode::print_ವೃಕ್ಷ(f_ವೃಕ್ಷ, ನಿಜ);
    }
#ಆದರೆ_ಅಂತ್ಯ


    //
    // calculate the functions nullable, firstpos, lastpos and followpos on
    // nodes in the parse tree.
    //    See the algorithm description in Aho.
    //    Understanding how this works by looking at the code alone will be
    //       nearly impossible.
    //
    calcNullable(f_ವೃಕ್ಷ);
    calc_ಮೊದಲ_Pos(f_ವೃಕ್ಷ);
    calc_ಕೊನೆಯ_Pos(f_ವೃಕ್ಷ);
    calcFollowPos(f_ವೃಕ್ಷ);
    ಆದರೆ (fRB->fDebugEnv && uprv_strstr(fRB->fDebugEnv, "pos")) {
        RBBIDebugPuts("\n");
        printPosSets(f_ವೃಕ್ಷ);
    }

    //
    //  For "chained" rules, modify the followPos sets
    //
    ಆದರೆ (fRB->f_Chain_ನಿಯಮಗಳು) {
        calcChainedFollowPos(f_ವೃಕ್ಷ, ಕೊನೆ_Marker_ಗಂಟು);
    }

    //
    // Build the DFA state transition tables.
    //
    build_State_ಕೋಷ್ಟಕ();
    map_Look_Ahead_ನಿಯಮಗಳು();
    flag_Accepting_ಸ್ಥಿತಿಗಳು();
    flag_Look_Ahead_ಸ್ಥಿತಿಗಳು();
    flag_Tagged_ಸ್ಥಿತಿಗಳು();

    //
    // Update the global table of rule status {tag} values
    // The rule builder has a global vector of status values that are common
    //    for all tables.  Merge the ones from this table into the global set.
    //
    ವಿಲೀನ_ನಿಯಮ_ಸ್ಥಿತಿ_Vals();
}



//-----------------------------------------------------------------------------
//
//   calcNullable.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::calcNullable(RBBINode *n) {
    ಆದರೆ (n == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (n->fType == RBBINode::ಗಣ_Ref ||
        n->fType == RBBINode::ಕೊನೆ_Mark ) {
        // These are non-empty leaf node types.
        n->fNullable = ಸುಳ್ಳು;
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (n->fType == RBBINode::lookAhead || n->fType == RBBINode::ಟ್ಯಾಗ್) {
        // Lookahead marker node.  It's a leaf, so no recursion on children.
        // It's nullable because it does not match any literal text from the input stream.
        n->fNullable = ನಿಜ;
        ಹಿಂತಿರುಗಿ;
    }


    // The node is not a leaf.
    //  Calculate nullable on its children.
    calcNullable(n->f_ಎಡ_ಮಗು);
    calcNullable(n->f_ಬಲ_ಮಗು);

    // Apply functions from table 3.40 in Aho
    ಆದರೆ (n->fType == RBBINode::opOr) {
        n->fNullable = n->f_ಎಡ_ಮಗು->fNullable || n->f_ಬಲ_ಮಗು->fNullable;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n->fType == RBBINode::opCat) {
        n->fNullable = n->f_ಎಡ_ಮಗು->fNullable && n->f_ಬಲ_ಮಗು->fNullable;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n->fType == RBBINode::op_ನಕ್ಷತ್ರ || n->fType == RBBINode::opQuestion) {
        n->fNullable = ನಿಜ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        n->fNullable = ಸುಳ್ಳು;
    }
}




//-----------------------------------------------------------------------------
//
//   calcFirstPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::calc_ಮೊದಲ_Pos(RBBINode *n) {
    ಆದರೆ (n == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (n->fType == RBBINode::ಎಲೆ_Char  ||
        n->fType == RBBINode::ಕೊನೆ_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::ಟ್ಯಾಗ್) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_ಮೊದಲ_Pos_ಗಣ->ಕೂಡಿಸು_Element(n, *f_ಸ್ಥಿತಿ);
        ಹಿಂತಿರುಗಿ;
    }

    // The node is not a leaf.
    //  Calculate firstPos on its children.
    calc_ಮೊದಲ_Pos(n->f_ಎಡ_ಮಗು);
    calc_ಮೊದಲ_Pos(n->f_ಬಲ_ಮಗು);

    // Apply functions from table 3.40 in Aho
    ಆದರೆ (n->fType == RBBINode::opOr) {
        ಗಣ_ಕೂಡಿಸು_2(n->f_ಮೊದಲ_Pos_ಗಣ, n->f_ಎಡ_ಮಗು->f_ಮೊದಲ_Pos_ಗಣ);
        ಗಣ_ಕೂಡಿಸು_2(n->f_ಮೊದಲ_Pos_ಗಣ, n->f_ಬಲ_ಮಗು->f_ಮೊದಲ_Pos_ಗಣ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n->fType == RBBINode::opCat) {
        ಗಣ_ಕೂಡಿಸು_2(n->f_ಮೊದಲ_Pos_ಗಣ, n->f_ಎಡ_ಮಗು->f_ಮೊದಲ_Pos_ಗಣ);
        ಆದರೆ (n->f_ಎಡ_ಮಗು->fNullable) {
            ಗಣ_ಕೂಡಿಸು_2(n->f_ಮೊದಲ_Pos_ಗಣ, n->f_ಬಲ_ಮಗು->f_ಮೊದಲ_Pos_ಗಣ);
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n->fType == RBBINode::op_ನಕ್ಷತ್ರ ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        ಗಣ_ಕೂಡಿಸು_2(n->f_ಮೊದಲ_Pos_ಗಣ, n->f_ಎಡ_ಮಗು->f_ಮೊದಲ_Pos_ಗಣ);
    }
}



//-----------------------------------------------------------------------------
//
//   calcLastPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::calc_ಕೊನೆಯ_Pos(RBBINode *n) {
    ಆದರೆ (n == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (n->fType == RBBINode::ಎಲೆ_Char  ||
        n->fType == RBBINode::ಕೊನೆ_Mark   ||
        n->fType == RBBINode::lookAhead ||
        n->fType == RBBINode::ಟ್ಯಾಗ್) {
        // These are non-empty leaf node types.
        // Note: In order to maintain the sort invariant on the set,
        // this function should only be called on a node whose set is
        // empty to start with.
        n->f_ಕೊನೆಯ_Pos_ಗಣ->ಕೂಡಿಸು_Element(n, *f_ಸ್ಥಿತಿ);
        ಹಿಂತಿರುಗಿ;
    }

    // The node is not a leaf.
    //  Calculate lastPos on its children.
    calc_ಕೊನೆಯ_Pos(n->f_ಎಡ_ಮಗು);
    calc_ಕೊನೆಯ_Pos(n->f_ಬಲ_ಮಗು);

    // Apply functions from table 3.40 in Aho
    ಆದರೆ (n->fType == RBBINode::opOr) {
        ಗಣ_ಕೂಡಿಸು_2(n->f_ಕೊನೆಯ_Pos_ಗಣ, n->f_ಎಡ_ಮಗು->f_ಕೊನೆಯ_Pos_ಗಣ);
        ಗಣ_ಕೂಡಿಸು_2(n->f_ಕೊನೆಯ_Pos_ಗಣ, n->f_ಬಲ_ಮಗು->f_ಕೊನೆಯ_Pos_ಗಣ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n->fType == RBBINode::opCat) {
        ಗಣ_ಕೂಡಿಸು_2(n->f_ಕೊನೆಯ_Pos_ಗಣ, n->f_ಬಲ_ಮಗು->f_ಕೊನೆಯ_Pos_ಗಣ);
        ಆದರೆ (n->f_ಬಲ_ಮಗು->fNullable) {
            ಗಣ_ಕೂಡಿಸು_2(n->f_ಕೊನೆಯ_Pos_ಗಣ, n->f_ಎಡ_ಮಗು->f_ಕೊನೆಯ_Pos_ಗಣ);
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n->fType == RBBINode::op_ನಕ್ಷತ್ರ     ||
             n->fType == RBBINode::opQuestion ||
             n->fType == RBBINode::opPlus) {
        ಗಣ_ಕೂಡಿಸು_2(n->f_ಕೊನೆಯ_Pos_ಗಣ, n->f_ಎಡ_ಮಗು->f_ಕೊನೆಯ_Pos_ಗಣ);
    }
}



//-----------------------------------------------------------------------------
//
//   calcFollowPos.    Impossible to explain succinctly.  See Aho, section 3.9
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::calcFollowPos(RBBINode *n) {
    ಆದರೆ (n == ಶೂನ್ಯಸೂಚಕ ||
        n->fType == RBBINode::ಎಲೆ_Char ||
        n->fType == RBBINode::ಕೊನೆ_Mark) {
        ಹಿಂತಿರುಗಿ;
    }

    calcFollowPos(n->f_ಎಡ_ಮಗು);
    calcFollowPos(n->f_ಬಲ_ಮಗು);

    // Aho rule #1
    ಆದರೆ (n->fType == RBBINode::opCat) {
        RBBINode *i;   // is 'i' in Aho's description
        uint32_t     ix;

        UVector *ಕೊನೆಯ_Pos_ದ_ಎಡ_ಮಗು = n->f_ಎಡ_ಮಗು->f_ಕೊನೆಯ_Pos_ಗಣ;

        ಗಾಗಿ (ix = 0; ix < static_cast<uint32_t>(ಕೊನೆಯ_Pos_ದ_ಎಡ_ಮಗು->size()); ix++) {
            i = static_cast<RBBINode*>(ಕೊನೆಯ_Pos_ದ_ಎಡ_ಮಗು->elementAt(ix));
            ಗಣ_ಕೂಡಿಸು_2(i->fFollowPos, n->f_ಬಲ_ಮಗು->f_ಮೊದಲ_Pos_ಗಣ);
        }
    }

    // Aho rule #2
    ಆದರೆ (n->fType == RBBINode::op_ನಕ್ಷತ್ರ ||
        n->fType == RBBINode::opPlus) {
        RBBINode   *i;  // again, n and i are the names from Aho's description.
        uint32_t    ix;

        ಗಾಗಿ (ix = 0; ix < static_cast<uint32_t>(n->f_ಕೊನೆಯ_Pos_ಗಣ->size()); ix++) {
            i = static_cast<RBBINode*>(n->f_ಕೊನೆಯ_Pos_ಗಣ->elementAt(ix));
            ಗಣ_ಕೂಡಿಸು_2(i->fFollowPos, n->f_ಮೊದಲ_Pos_ಗಣ);
        }
    }



}

//-----------------------------------------------------------------------------
//
//    addRuleRootNodes    Recursively walk a parse tree, adding all nodes flagged
//                        as roots of a rule to a destination vector.
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::ಕೂಡಿಸು_ನಿಯಮ_ಬೇರು_Nodes(UVector *dest, RBBINode *ಗಂಟು) {
    ಆದರೆ (ಗಂಟು == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    U_ASSERT(!dest->hasDeleter());
    ಆದರೆ (ಗಂಟು->f_ನಿಯಮ_ಬೇರು) {
        dest->ಕೂಡಿಸು_Element(ಗಂಟು, *f_ಸ್ಥಿತಿ);
        // Note: rules cannot nest. If we found a rule start node,
        //       no child node can also be a start node.
        ಹಿಂತಿರುಗಿ;
    }
    ಕೂಡಿಸು_ನಿಯಮ_ಬೇರು_Nodes(dest, ಗಂಟು->f_ಎಡ_ಮಗು);
    ಕೂಡಿಸು_ನಿಯಮ_ಬೇರು_Nodes(dest, ಗಂಟು->f_ಬಲ_ಮಗು);
}

//-----------------------------------------------------------------------------
//
//   calcChainedFollowPos.    Modify the previously calculated followPos sets
//                            to implement rule chaining.  NOT described by Aho
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::calcChainedFollowPos(RBBINode *ವೃಕ್ಷ, RBBINode *ಕೊನೆ_Mark_ಗಂಟು) {

    UVector         ಎಲೆ_Nodes(*f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }

    // get a list all leaf nodes
    ವೃಕ್ಷ->ಕಂಡುಹಿಡಿ_Nodes(&ಎಲೆ_Nodes, RBBINode::ಎಲೆ_Char, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }

    // Collect all leaf nodes that can start matches for rules
    // with inbound chaining enabled, which is the union of the 
    // firstPosition sets from each of the rule root nodes.
    
    UVector ನಿಯಮ_ಬೇರು_Nodes(*f_ಸ್ಥಿತಿ);
    ಕೂಡಿಸು_ನಿಯಮ_ಬೇರು_Nodes(&ನಿಯಮ_ಬೇರು_Nodes, ವೃಕ್ಷ);

    UVector ಹೊಂದಿಕೆ_ಆರಂಭ_Nodes(*f_ಸ್ಥಿತಿ);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j=0; j<ನಿಯಮ_ಬೇರು_Nodes.size(); ++j) {
        RBBINode *ಗಂಟು = static_cast<RBBINode *>(ನಿಯಮ_ಬೇರು_Nodes.elementAt(j));
        ಆದರೆ (ಗಂಟು->f_Chain_ಒಳ) {
            ಗಣ_ಕೂಡಿಸು_2(&ಹೊಂದಿಕೆ_ಆರಂಭ_Nodes, ಗಂಟು->f_ಮೊದಲ_Pos_ಗಣ);
        }
    }
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }

    int32_t  ಕೊನೆ_ಗಂಟು_Ix;
    int32_t  ಆರಂಭ_ಗಂಟು_Ix;

    ಗಾಗಿ (ಕೊನೆ_ಗಂಟು_Ix=0; ಕೊನೆ_ಗಂಟು_Ix<ಎಲೆ_Nodes.size(); ಕೊನೆ_ಗಂಟು_Ix++) {
        RBBINode* ಕೊನೆ_ಗಂಟು = static_cast<RBBINode*>(ಎಲೆ_Nodes.elementAt(ಕೊನೆ_ಗಂಟು_Ix));

        // Identify leaf nodes that correspond to overall rule match positions.
        // These include the endMarkNode in their followPos sets.
        //
        // Note: do not consider other end marker nodes, those that are added to
        //       look-ahead rules. These can't chain; a match immediately stops
        //       further matching. This leaves exactly one end marker node, the one
        //       at the end of the complete tree.

        ಆದರೆ (!ಕೊನೆ_ಗಂಟು->fFollowPos->ಒಳಗೊಂಡಿದೆ(ಕೊನೆ_Mark_ಗಂಟು)) {
            ಮುಂದುವರಿ;
        }

        // We've got a node that can end a match.

        // Now iterate over the nodes that can start a match, looking for ones
        //   with the same char class as our ending node.
        RBBINode *ಆರಂಭ_ಗಂಟು;
        ಗಾಗಿ (ಆರಂಭ_ಗಂಟು_Ix = 0; ಆರಂಭ_ಗಂಟು_Ix<ಹೊಂದಿಕೆ_ಆರಂಭ_Nodes.size(); ಆರಂಭ_ಗಂಟು_Ix++) {
            ಆರಂಭ_ಗಂಟು = static_cast<RBBINode*>(ಹೊಂದಿಕೆ_ಆರಂಭ_Nodes.elementAt(ಆರಂಭ_ಗಂಟು_Ix));
            ಆದರೆ (ಆರಂಭ_ಗಂಟು->fType != RBBINode::ಎಲೆ_Char) {
                ಮುಂದುವರಿ;
            }

            ಆದರೆ (ಕೊನೆ_ಗಂಟು->fVal == ಆರಂಭ_ಗಂಟು->fVal) {
                // The end val (character class) of one possible match is the
                //   same as the start of another.

                // Add all nodes from the followPos of the start node to the
                //  followPos set of the end node, which will have the effect of
                //  letting matches transition from a match state at endNode
                //  to the second char of a match starting with startNode.
                ಗಣ_ಕೂಡಿಸು_2(ಕೊನೆ_ಗಂಟು->fFollowPos, ಆರಂಭ_ಗಂಟು->fFollowPos);
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
ಶೂನ್ಯ RBBITableBuilder::bofFixup() {

    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
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
    RBBINode  *bof_ಗಂಟು = f_ವೃಕ್ಷ->f_ಎಡ_ಮಗು->f_ಎಡ_ಮಗು;
    U_ASSERT(bof_ಗಂಟು->fType == RBBINode::ಎಲೆ_Char);
    U_ASSERT(bof_ಗಂಟು->fVal == 2);

    // Get all nodes that can be the start a match of the user-written rules
    //  (excluding the fake bofNode)
    //  We want the nodes that can start a match in the
    //     part labeled "rest of tree"
    // 
    UVector *ಹೊಂದಿಕೆ_ಆರಂಭ_Nodes = f_ವೃಕ್ಷ->f_ಎಡ_ಮಗು->f_ಬಲ_ಮಗು->f_ಮೊದಲ_Pos_ಗಣ;

    RBBINode *ಆರಂಭ_ಗಂಟು;
    ಪೂರ್ಣಾಂಕ       ಆರಂಭ_ಗಂಟು_Ix;
    ಗಾಗಿ (ಆರಂಭ_ಗಂಟು_Ix = 0; ಆರಂಭ_ಗಂಟು_Ix<ಹೊಂದಿಕೆ_ಆರಂಭ_Nodes->size(); ಆರಂಭ_ಗಂಟು_Ix++) {
        ಆರಂಭ_ಗಂಟು = static_cast<RBBINode*>(ಹೊಂದಿಕೆ_ಆರಂಭ_Nodes->elementAt(ಆರಂಭ_ಗಂಟು_Ix));
        ಆದರೆ (ಆರಂಭ_ಗಂಟು->fType != RBBINode::ಎಲೆ_Char) {
            ಮುಂದುವರಿ;
        }

        ಆದರೆ (ಆರಂಭ_ಗಂಟು->fVal == bof_ಗಂಟು->fVal) {
            //  We found a leaf node corresponding to a {bof} that was
            //    explicitly written into a rule.
            //  Add everything from the followPos set of this node to the
            //    followPos set of the fake bofNode at the start of the tree.
            //  
            ಗಣ_ಕೂಡಿಸು_2(bof_ಗಂಟು->fFollowPos, ಆರಂಭ_ಗಂಟು->fFollowPos);
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
ಶೂನ್ಯ RBBITableBuilder::build_State_ಕೋಷ್ಟಕ() {
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    RBBIStateDescriptor *failState;
    // Set it to nullptr to avoid uninitialized warning
    RBBIStateDescriptor *initialState = ಶೂನ್ಯಸೂಚಕ;
    //
    // Add a dummy state 0 - the stop state.  Not from Aho.
    ಪೂರ್ಣಾಂಕ      ಕೊನೆಯ_ಒಳಹರಿವು_ಚಿಹ್ನೆ = fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories() - 1;
    failState = ಹೊಸ RBBIStateDescriptor(ಕೊನೆಯ_ಒಳಹರಿವು_ಚಿಹ್ನೆ, f_ಸ್ಥಿತಿ);
    ಆದರೆ (failState == ಶೂನ್ಯಸೂಚಕ) {
        *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹೋಗು ExitBuildSTdeleteall;
    }
    failState->fPositions = ಹೊಸ UVector(*f_ಸ್ಥಿತಿ);
    ಆದರೆ (failState->fPositions == ಶೂನ್ಯಸೂಚಕ) {
        *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಆದರೆ (failState->fPositions == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹೋಗು ExitBuildSTdeleteall;
    }
    fDStates->ಕೂಡಿಸು_Element(failState, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹೋಗು ExitBuildSTdeleteall;
    }

    // initially, the only unmarked state in Dstates is firstpos(root),
    //       where toot is the root of the syntax tree for (r)#;
    initialState = ಹೊಸ RBBIStateDescriptor(ಕೊನೆಯ_ಒಳಹರಿವು_ಚಿಹ್ನೆ, f_ಸ್ಥಿತಿ);
    ಆದರೆ (initialState == ಶೂನ್ಯಸೂಚಕ) {
        *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹೋಗು ExitBuildSTdeleteall;
    }
    initialState->fPositions = ಹೊಸ UVector(*f_ಸ್ಥಿತಿ);
    ಆದರೆ (initialState->fPositions == ಶೂನ್ಯಸೂಚಕ) {
        *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹೋಗು ExitBuildSTdeleteall;
    }
    ಗಣ_ಕೂಡಿಸು_2(initialState->fPositions, f_ವೃಕ್ಷ->f_ಮೊದಲ_Pos_ಗಣ);
    fDStates->ಕೂಡಿಸು_Element(initialState, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹೋಗು ExitBuildSTdeleteall;
    }

    // while there is an unmarked state T in Dstates do begin
    ಗಾಗಿ (;;) {
        RBBIStateDescriptor *T = ಶೂನ್ಯಸೂಚಕ;
        int32_t              tx;
        ಗಾಗಿ (tx=1; tx<fDStates->size(); tx++) {
            RBBIStateDescriptor *ತಾತ್ಕಾಲಿಕ;
            ತಾತ್ಕಾಲಿಕ = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(tx));
            ಆದರೆ (ತಾತ್ಕಾಲಿಕ->fMarked == ಸುಳ್ಳು) {
                T = ತಾತ್ಕಾಲಿಕ;
                ನಿಲ್ಲಿಸು;
            }
        }
        ಆದರೆ (T == ಶೂನ್ಯಸೂಚಕ) {
            ನಿಲ್ಲಿಸು;
        }

        // mark T;
        T->fMarked = ನಿಜ;

        // for each input symbol a do begin
        int32_t  a;
        ಗಾಗಿ (a = 1; a<=ಕೊನೆಯ_ಒಳಹರಿವು_ಚಿಹ್ನೆ; a++) {
            // let U be the set of positions that are in followpos(p)
            //    for some position p in T
            //    such that the symbol at position p is a;
            UVector    *U = ಶೂನ್ಯಸೂಚಕ;
            RBBINode   *p;
            int32_t     px;
            ಗಾಗಿ (px=0; px<T->fPositions->size(); px++) {
                p = static_cast<RBBINode*>(T->fPositions->elementAt(px));
                ಆದರೆ ((p->fType == RBBINode::ಎಲೆ_Char) &&  (p->fVal == a)) {
                    ಆದರೆ (U == ಶೂನ್ಯಸೂಚಕ) {
                        U = ಹೊಸ UVector(*f_ಸ್ಥಿತಿ);
                        ಆದರೆ (U == ಶೂನ್ಯಸೂಚಕ) {
                        	*f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                        	ಹೋಗು ExitBuildSTdeleteall;
                        }
                    }
                    ಗಣ_ಕೂಡಿಸು_2(U, p->fFollowPos);
                }
            }

            // if U is not empty and not in DStates then
            int32_t  ux = 0;
            UBool    UinDstates = ಸುಳ್ಳು;
            ಆದರೆ (U != ಶೂನ್ಯಸೂಚಕ) {
                U_ASSERT(U->size() > 0);
                ಪೂರ್ಣಾಂಕ  ix;
                ಗಾಗಿ (ix=0; ix<fDStates->size(); ix++) {
                    RBBIStateDescriptor *temp2;
                    temp2 = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(ix));
                    ಆದರೆ (ಗಣ_Equals(U, temp2->fPositions)) {
                        ಅಳಿಸು U;
                        U  = temp2->fPositions;
                        ux = ix;
                        UinDstates = ನಿಜ;
                        ನಿಲ್ಲಿಸು;
                    }
                }

                // Add U as an unmarked state to Dstates
                ಆದರೆ (!UinDstates)
                {
                    RBBIStateDescriptor *newState = ಹೊಸ RBBIStateDescriptor(ಕೊನೆಯ_ಒಳಹರಿವು_ಚಿಹ್ನೆ, f_ಸ್ಥಿತಿ);
                    ಆದರೆ (newState == ಶೂನ್ಯಸೂಚಕ) {
                    	*f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    }
                    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
                        ಹೋಗು ExitBuildSTdeleteall;
                    }
                    newState->fPositions = U;
                    fDStates->ಕೂಡಿಸು_Element(newState, *f_ಸ್ಥಿತಿ);
                    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
                        ಹಿಂತಿರುಗಿ;
                    }
                    ux = fDStates->size()-1;
                }

                // Dtran[T, a] := U;
                T->fDtran->ಗಣ_Element_At(ux, a);
            }
        }
    }
    ಹಿಂತಿರುಗಿ;
    // delete local pointers only if error occurred.
ExitBuildSTdeleteall:
    ಅಳಿಸು initialState;
    ಅಳಿಸು failState;
}


/**
 * mapLookAheadRules
 *
 */
ಶೂನ್ಯ RBBITableBuilder::map_Look_Ahead_ನಿಯಮಗಳು() {
    f_Look_Ahead_ನಿಯಮ_Map =  ಹೊಸ UVector32(fRB->fScanner->ಅಂಕೆ_ನಿಯಮಗಳು() + 1, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (f_Look_Ahead_ನಿಯಮ_Map == ಶೂನ್ಯಸೂಚಕ) {
        *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    f_Look_Ahead_ನಿಯಮ_Map->ಗಣ_ಗಾತ್ರ(fRB->fScanner->ಅಂಕೆ_ನಿಯಮಗಳು() + 1);

    ಗಾಗಿ (int32_t n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        int32_t la_ಸ್ಥಾನ_For_State = 0;

        // Establish the look-ahead slot for this state, if the state covers
        // any look-ahead nodes - corresponding to the '/' in look-ahead rules.

        // If any of the look-ahead nodes already have a slot assigned, use it,
        // otherwise assign a new one.

        ತಾರ್ಕಿಕ saw_Look_Ahead_ಗಂಟು = ಸುಳ್ಳು;
        ಗಾಗಿ (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *ಗಂಟು = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            ಆದರೆ (ಗಂಟು->fType != RBBINode::ಗಂಟು_Type::lookAhead) {
                ಮುಂದುವರಿ;
            }
            saw_Look_Ahead_ಗಂಟು = ನಿಜ;
            int32_t ನಿಯಮ_ಅಂಕೆ = ಗಂಟು->fVal;     // Set when rule was originally parsed.
            U_ASSERT(ನಿಯಮ_ಅಂಕೆ < f_Look_Ahead_ನಿಯಮ_Map->size());
            U_ASSERT(ನಿಯಮ_ಅಂಕೆ > 0);
            int32_t la_ಸ್ಥಾನ = f_Look_Ahead_ನಿಯಮ_Map->elementAti(ನಿಯಮ_ಅಂಕೆ);
            ಆದರೆ (la_ಸ್ಥಾನ != 0) {
                ಆದರೆ (la_ಸ್ಥಾನ_For_State == 0) {
                    la_ಸ್ಥಾನ_For_State = la_ಸ್ಥಾನ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // TODO: figure out if this can fail, change to setting an error code if so.
                    U_ASSERT(la_ಸ್ಥಾನ == la_ಸ್ಥಾನ_For_State);
                }
            }
        }
        ಆದರೆ (!saw_Look_Ahead_ಗಂಟು) {
            ಮುಂದುವರಿ;
        }

        ಆದರೆ (la_ಸ್ಥಾನ_For_State == 0) {
            la_ಸ್ಥಾನ_For_State = ++f_LASlots_ಒಳ_Use;
        }

        // For each look ahead node covered by this state,
        // set the mapping from the node's rule number to the look ahead slot.
        // There can be multiple nodes/rule numbers going to the same la slot.

        ಗಾಗಿ (int32_t ipos=0; ipos<sd->fPositions->size(); ++ipos) {
            RBBINode *ಗಂಟು = static_cast<RBBINode *>(sd->fPositions->elementAt(ipos));
            ಆದರೆ (ಗಂಟು->fType != RBBINode::ಗಂಟು_Type::lookAhead) {
                ಮುಂದುವರಿ;
            }
            int32_t ನಿಯಮ_ಅಂಕೆ = ಗಂಟು->fVal;     // Set when rule was originally parsed.
            int32_t existingVal = f_Look_Ahead_ನಿಯಮ_Map->elementAti(ನಿಯಮ_ಅಂಕೆ);
            (ಶೂನ್ಯ)existingVal;
            U_ASSERT(existingVal == 0 || existingVal == la_ಸ್ಥಾನ_For_State);
            f_Look_Ahead_ನಿಯಮ_Map->ಗಣ_Element_At(la_ಸ್ಥಾನ_For_State, ನಿಯಮ_ಅಂಕೆ);
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
ಶೂನ್ಯ     RBBITableBuilder::flag_Accepting_ಸ್ಥಿತಿಗಳು() {
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    UVector     ಕೊನೆ_Marker_Nodes(*f_ಸ್ಥಿತಿ);
    RBBINode    *ಕೊನೆ_Marker;
    int32_t     i;
    int32_t     n;

    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }

    f_ವೃಕ್ಷ->ಕಂಡುಹಿಡಿ_Nodes(&ಕೊನೆ_Marker_Nodes, RBBINode::ಕೊನೆ_Mark, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (i=0; i<ಕೊನೆ_Marker_Nodes.size(); i++) {
        ಕೊನೆ_Marker = static_cast<RBBINode*>(ಕೊನೆ_Marker_Nodes.elementAt(i));
        ಗಾಗಿ (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            ಆದರೆ (sd->fPositions->ಸೂಚ್ಯಂಕ_ದ(ಕೊನೆ_Marker) >= 0) {
                // Any non-zero value for fAccepting means this is an accepting node.
                // The value is what will be returned to the user as the break status.
                // If no other value was specified, force it to ACCEPTING_UNCONDITIONAL (1).

                ಆದರೆ (sd->fAccepting==0) {
                    // State hasn't been marked as accepting yet.  Do it now.
                    sd->fAccepting = f_Look_Ahead_ನಿಯಮ_Map->elementAti(ಕೊನೆ_Marker->fVal);
                    ಆದರೆ (sd->fAccepting == 0) {
                        sd->fAccepting = ACCEPTING_UNCONDITIONAL;
                    }
                }
                ಆದರೆ (sd->fAccepting==ACCEPTING_UNCONDITIONAL && ಕೊನೆ_Marker->fVal != 0) {
                    // Both lookahead and non-lookahead accepting for this state.
                    // Favor the look-ahead, because a look-ahead match needs to
                    // immediately stop the run-time engine. First match, not longest.
                    sd->fAccepting = f_Look_Ahead_ನಿಯಮ_Map->elementAti(ಕೊನೆ_Marker->fVal);
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
ಶೂನ್ಯ     RBBITableBuilder::flag_Look_Ahead_ಸ್ಥಿತಿಗಳು() {
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    UVector     lookAheadNodes(*f_ಸ್ಥಿತಿ);
    RBBINode    *look_Ahead_ಗಂಟು;
    int32_t     i;
    int32_t     n;

    f_ವೃಕ್ಷ->ಕಂಡುಹಿಡಿ_Nodes(&lookAheadNodes, RBBINode::lookAhead, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (i=0; i<lookAheadNodes.size(); i++) {
        look_Ahead_ಗಂಟು = static_cast<RBBINode*>(lookAheadNodes.elementAt(i));
        U_ASSERT(look_Ahead_ಗಂಟು->fType == RBBINode::ಗಂಟು_Type::lookAhead);

        ಗಾಗಿ (n=0; n<fDStates->size(); n++) {
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            int32_t positionsIdx = sd->fPositions->ಸೂಚ್ಯಂಕ_ದ(look_Ahead_ಗಂಟು);
            ಆದರೆ (positionsIdx >= 0) {
                U_ASSERT(look_Ahead_ಗಂಟು == sd->fPositions->elementAt(positionsIdx));
                uint32_t lookahead_ಸ್ಥಾನ = f_Look_Ahead_ನಿಯಮ_Map->elementAti(look_Ahead_ಗಂಟು->fVal);
                U_ASSERT(sd->fLookAhead == 0 || sd->fLookAhead == lookahead_ಸ್ಥಾನ);
                // if (sd->fLookAhead != 0 && sd->fLookAhead != lookaheadSlot) {
                //     printf("%s:%d Bingo. sd->fLookAhead:%d   lookaheadSlot:%d\n",
                //            __FILE__, __LINE__, sd->fLookAhead, lookaheadSlot);
                // }
                sd->fLookAhead = lookahead_ಸ್ಥಾನ;
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//    flagTaggedStates
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ     RBBITableBuilder::flag_Tagged_ಸ್ಥಿತಿಗಳು() {
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    UVector     ಟ್ಯಾಗ್_Nodes(*f_ಸ್ಥಿತಿ);
    RBBINode    *ಟ್ಯಾಗ್_ಗಂಟು;
    int32_t     i;
    int32_t     n;

    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    f_ವೃಕ್ಷ->ಕಂಡುಹಿಡಿ_Nodes(&ಟ್ಯಾಗ್_Nodes, RBBINode::ಟ್ಯಾಗ್, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (i=0; i<ಟ್ಯಾಗ್_Nodes.size(); i++) {                   // For each tag node t (all of 'em)
        ಟ್ಯಾಗ್_ಗಂಟು = static_cast<RBBINode*>(ಟ್ಯಾಗ್_Nodes.elementAt(i));

        ಗಾಗಿ (n=0; n<fDStates->size(); n++) {              //    For each state  s (row in the state table)
            RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
            ಆದರೆ (sd->fPositions->ಸೂಚ್ಯಂಕ_ದ(ಟ್ಯಾಗ್_ಗಂಟು) >= 0) {  //       if  s include the tag node t
                sorted_ಕೂಡಿಸು(&sd->f_ಟ್ಯಾಗ್_Vals, ಟ್ಯಾಗ್_ಗಂಟು->fVal);
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
ಶೂನ್ಯ  RBBITableBuilder::ವಿಲೀನ_ನಿಯಮ_ಸ್ಥಿತಿ_Vals() {
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
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ n;

    // Pre-set a single tag of {0} into the table.
    //   We will need this as a default, for rule sets with no explicit tagging.
    ಆದರೆ (fRB->f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->size() == 0) {
        fRB->f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->ಕೂಡಿಸು_Element(1, *f_ಸ್ಥಿತಿ);  // Num of statuses in group
        fRB->f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->ಕೂಡಿಸು_Element(static_cast<int32_t>(0), *f_ಸ್ಥಿತಿ); // and our single status of zero
    }

    //    For each state
    ಗಾಗಿ (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(n));
        UVector *this_ಸ್ಥಿತಿಗಳು_ಟ್ಯಾಗ್_ಮೌಲ್ಯಗಳು = sd->f_ಟ್ಯಾಗ್_Vals;
        ಆದರೆ (this_ಸ್ಥಿತಿಗಳು_ಟ್ಯಾಗ್_ಮೌಲ್ಯಗಳು == ಶೂನ್ಯಸೂಚಕ) {
            // No tag values are explicitly associated with this state.
            //   Set the default tag value.
            sd->f_ಟ್ಯಾಗುಗಳು_Idx = 0;
            ಮುಂದುವರಿ;
        }

        // There are tag(s) associated with this state.
        //   fTagsIdx will be the index into the global tag list for this state's tag values.
        //   Initial value of -1 flags that we haven't got it set yet.
        sd->f_ಟ್ಯಾಗುಗಳು_Idx = -1;
        int32_t  this_ಟ್ಯಾಗ್_ಗುಂಪು_ಆರಂಭ = 0;   // indexes into the global rule status vals list
        int32_t  ಮುಂದಿನ_ಟ್ಯಾಗ್_ಗುಂಪು_ಆರಂಭ = 0;

        // Loop runs once per group of tags in the global list
        ಆಗ (ಮುಂದಿನ_ಟ್ಯಾಗ್_ಗುಂಪು_ಆರಂಭ < fRB->f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->size()) {
            this_ಟ್ಯಾಗ್_ಗುಂಪು_ಆರಂಭ = ಮುಂದಿನ_ಟ್ಯಾಗ್_ಗುಂಪು_ಆರಂಭ;
            ಮುಂದಿನ_ಟ್ಯಾಗ್_ಗುಂಪು_ಆರಂಭ += fRB->f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->elementAti(this_ಟ್ಯಾಗ್_ಗುಂಪು_ಆರಂಭ) + 1;
            ಆದರೆ (this_ಸ್ಥಿತಿಗಳು_ಟ್ಯಾಗ್_ಮೌಲ್ಯಗಳು->size() != fRB->f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->elementAti(this_ಟ್ಯಾಗ್_ಗುಂಪು_ಆರಂಭ)) {
                // The number of tags for this state is different from
                //    the number of tags in this group from the global list.
                //    Continue with the next group from the global list.
                ಮುಂದುವರಿ;
            }
            // The lengths match, go ahead and compare the actual tag values
            //    between this state and the group from the global list.
            ಗಾಗಿ (i=0; i<this_ಸ್ಥಿತಿಗಳು_ಟ್ಯಾಗ್_ಮೌಲ್ಯಗಳು->size(); i++) {
                ಆದರೆ (this_ಸ್ಥಿತಿಗಳು_ಟ್ಯಾಗ್_ಮೌಲ್ಯಗಳು->elementAti(i) !=
                    fRB->f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->elementAti(this_ಟ್ಯಾಗ್_ಗುಂಪು_ಆರಂಭ + 1 + i) ) {
                    // Mismatch.
                    ನಿಲ್ಲಿಸು;
                }
            }

            ಆದರೆ (i == this_ಸ್ಥಿತಿಗಳು_ಟ್ಯಾಗ್_ಮೌಲ್ಯಗಳು->size()) {
                // We found a set of tag values in the global list that match
                //   those for this state.  Use them.
                sd->f_ಟ್ಯಾಗುಗಳು_Idx = this_ಟ್ಯಾಗ್_ಗುಂಪು_ಆರಂಭ;
                ನಿಲ್ಲಿಸು;
            }
        }

        ಆದರೆ (sd->f_ಟ್ಯಾಗುಗಳು_Idx == -1) {
            // No suitable entry in the global tag list already.  Add one
            sd->f_ಟ್ಯಾಗುಗಳು_Idx = fRB->f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->size();
            fRB->f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->ಕೂಡಿಸು_Element(this_ಸ್ಥಿತಿಗಳು_ಟ್ಯಾಗ್_ಮೌಲ್ಯಗಳು->size(), *f_ಸ್ಥಿತಿ);
            ಗಾಗಿ (i=0; i<this_ಸ್ಥಿತಿಗಳು_ಟ್ಯಾಗ್_ಮೌಲ್ಯಗಳು->size(); i++) {
                fRB->f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals->ಕೂಡಿಸು_Element(this_ಸ್ಥಿತಿಗಳು_ಟ್ಯಾಗ್_ಮೌಲ್ಯಗಳು->elementAti(i), *f_ಸ್ಥಿತಿ);
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

ಶೂನ್ಯ RBBITableBuilder::minimize_ಸ್ಥಿತಿಗಳು() {
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }

#ಆದರೆ UPRV_HAS_SANITIZER
    ಆದರೆ (fDStates->size() > 512) {
        // This algorithm is sluggish on large state machines, and even more sluggish with
        // sanitizers, which leads the fuzzer into the weeds, see
        // https://github.com/unicode-org/icu/pull/3948#issuecomment-4335461360.
        *f_ಸ್ಥಿತಿ = U_REGEX_ನಮೂನೆ_TOO_BIG;
        ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ರಚನೆ StateType {
        uint32_t fAccepting;
        uint32_t fLookAhead;
        int32_t f_ಟ್ಯಾಗುಗಳು_Idx;
        ತಾರ್ಕಿಕ ಕ್ರಿಯಾಕಾರಕ==(StateType ಸ್ಥಿರಾಂಕ &other) ಸ್ಥಿರಾಂಕ {
            ಹಿಂತಿರುಗಿ fAccepting == other.fAccepting &&
                   fLookAhead == other.fLookAhead &&
                   f_ಟ್ಯಾಗುಗಳು_Idx == other.f_ಟ್ಯಾಗುಗಳು_Idx;
        }
    };
    // We wrap `UVector`s in `unique_ptr``s throughout so we can move them,
    // including into `UVector`s (by releasing them from the `unique_ptr`
    // and having the enclosing `UVector` adopt them). 
    // Group the states by types (but we have no maps so this is verbose).
    // If there are no lookaheads and no tags, there are only two types
    // (accepting and non-accepting) in which case this is exactly step 1
    // of Algorithm 3.6.
    ರಚನೆ Type_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು : UMemory {
        Type_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು(ಸ್ಥಿರಾಂಕ StateType &type, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
            : type(type), ಸ್ಥಿತಿಗಳು(prv::make_unique<UVector>(ಸ್ಥಿತಿ_2, ಸ್ಥಿತಿ_2)) {}
        StateType type;
        prv::unique_ptr<UVector> ಸ್ಥಿತಿಗಳು;
    };
    UVector initial_ವಿಭಜನೆ(*f_ಸ್ಥಿತಿ);
    initial_ವಿಭಜನೆ.ಗಣ_Deleter(
        [](ಶೂನ್ಯ *p) { ಅಳಿಸು static_cast<Type_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು *>(p); });
    ಗಾಗಿ (int32_t i = 0; i < fDStates->size(); ++i) {
        ಸ್ಥಿರಾಂಕ RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(i));
        ಸ್ಥಿರಾಂಕ StateType type{state.fAccepting,
                             state.fLookAhead,
                             state.f_ಟ್ಯಾಗುಗಳು_Idx};
        int32_t j = 0;
        ಗಾಗಿ (; j < initial_ವಿಭಜನೆ.size(); ++j) {
            ಸ್ವಯಂಚಾಲಿತ &[type_j, ಸ್ಥಿತಿಗಳು] =
                *static_cast<Type_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು *>(initial_ವಿಭಜನೆ.elementAt(j));
            ಆದರೆ (type_j == type) {
                ಸ್ಥಿತಿಗಳು->ಕೂಡಿಸು_Element(i, *f_ಸ್ಥಿತಿ);
                ನಿಲ್ಲಿಸು;
            }
        }
        ಆದರೆ (j == initial_ವಿಭಜನೆ.size()) {
            ಸ್ವಯಂಚಾಲಿತ new_ನಮೂದು = prv::make_unique<Type_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು>(type, *f_ಸ್ಥಿತಿ, *f_ಸ್ಥಿತಿ);
            ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
                ಹಿಂತಿರುಗಿ;
            }
            new_ನಮೂದು->ಸ್ಥಿತಿಗಳು->ಕೂಡಿಸು_Element(i, *f_ಸ್ಥಿತಿ);
            initial_ವಿಭಜನೆ.adoptElement(new_ನಮೂದು.release(), *f_ಸ್ಥಿತಿ);
        }
    }
    // The partition Π from Algorithm 3.6.
    // (We could call it Π, but some member companies that integrate the ICU
    // code base prohibit non-ASCII identifiers…).
    ಸ್ವಯಂಚಾಲಿತ ವಿಭಜನೆ = prv::make_unique<UVector>(*f_ಸ್ಥಿತಿ, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ವಿಭಜನೆ->ಗಣ_Deleter(
        [](ಶೂನ್ಯ *p) { ಅಳಿಸು static_cast<UVector*>(p); });
    
    ಗಾಗಿ (int32_t i = 0; i < initial_ವಿಭಜನೆ.size(); ++i) {
        ವಿಭಜನೆ->adoptElement(
            static_cast<Type_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು*>(initial_ವಿಭಜನೆ.elementAt(i))->ಸ್ಥಿತಿಗಳು.release(),
            *f_ಸ್ಥಿತಿ);
    }
    // Given the index of a state 𝑠 in `fDStates`, returns a UVector of integers
    // σ(𝑠) such that 𝑠.fDtran[i] (the index of the state reached by the
    // transition from 𝑠 on the character class with index i) is in Π[σ(𝑠)[i]].
    // We then have σ(𝑠) == σ(𝑡) if and only if “for all input symbols 𝑎, states
    // 𝑠 and 𝑡 have transitions on 𝑎 to states in the same group of Π” (which is
    // what defines the refinement of 𝐺 in Fig. 3.45).
    ಸ್ವಯಂಚಾಲಿತ ವಿಭಜನೆ_signature = [&ವಿಭಜನೆ, ಇದು](int32_t state_ಸೂಚ್ಯಂಕ) -> prv::unique_ptr<UVector> {
        ಸ್ಥಿರಾಂಕ RBBIStateDescriptor &state =
            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state_ಸೂಚ್ಯಂಕ));
        ಸ್ವಯಂಚಾಲಿತ ಫಲಿತಾಂಶ = prv::make_unique<UVector>(state.fDtran->size(), *f_ಸ್ಥಿತಿ, *f_ಸ್ಥಿತಿ);
        ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ಗಾಗಿ (int32_t i = 0; i < state.fDtran->size(); ++i) {
            int32_t ಗಮ್ಯ_State = state.fDtran->elementAti(i);
            ಗಾಗಿ (int32_t part_ಸೂಚ್ಯಂಕ = 0; part_ಸೂಚ್ಯಂಕ < ವಿಭಜನೆ->size(); ++part_ಸೂಚ್ಯಂಕ) {
                ಸ್ಥಿರಾಂಕ UVector &part = *static_cast<UVector *>(ವಿಭಜನೆ->elementAt(part_ಸೂಚ್ಯಂಕ));
                ಆದರೆ (part.ಒಳಗೊಂಡಿದೆ(ಗಮ್ಯ_State)) {
                    ಫಲಿತಾಂಶ->ಕೂಡಿಸು_Element(part_ಸೂಚ್ಯಂಕ, *f_ಸ್ಥಿತಿ);
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        ಫಲಿತಾಂಶ->ಗಣ_Comparer([](ಸ್ಥಿರಾಂಕ UElement ಎಡ, ಸ್ಥಿರಾಂಕ UElement ಬಲ) -> UBool {
            ಹಿಂತಿರುಗಿ ಎಡ.integer == ಬಲ.integer;
        });
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    };
    // The loop between steps 2. and 3. of Algorithm 3.6.
    ಗಾಗಿ (;;) {
        // Π_new.
        ಸ್ವಯಂಚಾಲಿತ ವಿಭಜನೆ_New = prv::make_unique<UVector>(*f_ಸ್ಥಿತಿ, *f_ಸ್ಥಿತಿ);
        ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
            ಹಿಂತಿರುಗಿ;
        }
        ವಿಭಜನೆ_New->ಗಣ_Deleter(
            [](ಶೂನ್ಯ *p) { ಅಳಿಸು static_cast<UVector*>(p); });
        // The procedure from Figure 3.45, Construction of Π_new.
        ತಾರ್ಕಿಕ refined = ಸುಳ್ಳು;
        ಗಾಗಿ (int32_t i = 0; i < ವಿಭಜನೆ->size(); ++i) {
            // Group 𝐺 in Π (=`partition`).
            ಸ್ಥಿರಾಂಕ UVector &ಗುಂಪು = *static_cast<UVector*>(ವಿಭಜನೆ->elementAt(i));
            // Partition 𝐺 based on the signature, see above.
            ರಚನೆ Signature_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು : UMemory {
                Signature_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು(prv::unique_ptr<UVector> signature, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
                    : signature(std::move(signature)), ಸ್ಥಿತಿಗಳು(prv::make_unique<UVector>(ಸ್ಥಿತಿ_2, ಸ್ಥಿತಿ_2)) {
                }
                prv::unique_ptr<UVector> signature;
                prv::unique_ptr<UVector> ಸ್ಥಿತಿಗಳು;
            };
            UVector ಗುಂಪು_Refinement(*f_ಸ್ಥಿತಿ);
            ಗುಂಪು_Refinement.ಗಣ_Deleter([](ಶೂನ್ಯ *p) { ಅಳಿಸು static_cast<Signature_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು*>(p); });
            ಗಾಗಿ (int32_t j = 0; j < ಗುಂಪು.size(); ++j) {
                // Index of a state in the group.
                ಸ್ಥಿರಾಂಕ int32_t s = ಗುಂಪು.elementAti(j);
                ಸ್ವಯಂಚಾಲಿತ signature = ವಿಭಜನೆ_signature(s);
                int32_t k = 0;
                ಗಾಗಿ (; k < ಗುಂಪು_Refinement.size(); ++k) {
                    ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &[subgroupSignature, subgroup] =
                        *static_cast<Signature_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು*>(ಗುಂಪು_Refinement.elementAt(k));
                    ಆದರೆ (*subgroupSignature == *signature) {
                        subgroup->ಕೂಡಿಸು_Element(s, *f_ಸ್ಥಿತಿ);
                        ನಿಲ್ಲಿಸು;
                    }
                }
                ಆದರೆ (k == ಗುಂಪು_Refinement.size()) {
                    ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ new_ನಮೂದು =
                        ಹೊಸ Signature_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು(std::move(signature), *f_ಸ್ಥಿತಿ);
                    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
                        ಹಿಂತಿರುಗಿ;
                    }
                    ಆದರೆ (new_ನಮೂದು == ಶೂನ್ಯಸೂಚಕ) {
                      *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                      ಹಿಂತಿರುಗಿ;
                    }
                    new_ನಮೂದು->ಸ್ಥಿತಿಗಳು->ಕೂಡಿಸು_Element(s, *f_ಸ್ಥಿತಿ);
                    ಗುಂಪು_Refinement.adoptElement(new_ನಮೂದು, *f_ಸ್ಥಿತಿ);
                }
            }
            refined |= ಗುಂಪು_Refinement.size() > 1;
            ಗಾಗಿ (int32_t j = 0; j < ಗುಂಪು_Refinement.size(); ++j) {
                ಸ್ವಯಂಚಾಲಿತ &[_, subgroup] = *static_cast<Signature_ಗಮ್ಯ_ಸ್ಥಿತಿಗಳು*>(
                    ಗುಂಪು_Refinement.elementAt(j));
                ವಿಭಜನೆ_New->adoptElement(subgroup.release(), *f_ಸ್ಥಿತಿ);
            }
        }
        ಆದರೆ (refined) {
            ವಿಭಜನೆ = std::move(ವಿಭಜನೆ_New);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ನಿಲ್ಲಿಸು;
        }
    }
    // We will use the indices in `partition` as the new state indices.  Make
    // sure that the start (1) and stop (0) states remain in their correct
    // places; everything else can merrily be scrambled.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ವಿಭಜನೆ->size(); ++i) {
        UVector *ಸ್ಥಿರಾಂಕ part_i = static_cast<UVector *>(ವಿಭಜನೆ->elementAt(i));
        ಆದರೆ (part_i->ಒಳಗೊಂಡಿದೆ(0)) {
            ಶೂನ್ಯ *ಸ್ಥಿರಾಂಕ part_0 = ವಿಭಜನೆ->elementAt(0);
            // No swap on UVector, so we briefly remove the deleter.
            ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ deleter = ವಿಭಜನೆ->ಗಣ_Deleter(ಶೂನ್ಯಸೂಚಕ);
            ವಿಭಜನೆ->ಗಣ_Element_At(part_i, 0);
            ವಿಭಜನೆ->ಗಣ_Element_At(part_0, i);
            ವಿಭಜನೆ->ಗಣ_Deleter(deleter);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part_i->ಒಳಗೊಂಡಿದೆ(1)) {
            ಶೂನ್ಯ *ಸ್ಥಿರಾಂಕ part_1 = ವಿಭಜನೆ->elementAt(1);
            ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ deleter = ವಿಭಜನೆ->ಗಣ_Deleter(ಶೂನ್ಯಸೂಚಕ);
            ವಿಭಜನೆ->ಗಣ_Element_At(part_i, 1);
            ವಿಭಜನೆ->ಗಣ_Element_At(part_1, i);
            ವಿಭಜನೆ->ಗಣ_Deleter(deleter);
        }
    }
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ವಯಂಚಾಲಿತ old_State_ಗಮ್ಯ_Part = prv::make_unique_for_overwrite<uint16_t[]>(fDStates->size(), *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ವಿಭಜನೆ->size(); ++i) {
        ಸ್ಥಿರಾಂಕ UVector &part = *static_cast<UVector*>(ವಿಭಜನೆ->elementAt(i));
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < part.size(); ++j) {
            old_State_ಗಮ್ಯ_Part[part.elementAti(j)] = i;
        }
    }
    ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ old_ಸ್ಥಿತಿಗಳು = prv::unique_ptr<UVector>(fDStates);
    fDStates = prv::make_unique<UVector>(*f_ಸ್ಥಿತಿ, *f_ಸ್ಥಿತಿ).release();
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ವಿಭಜನೆ->size(); ++i) {
        ಸ್ಥಿರಾಂಕ UVector &part = *static_cast<UVector*>(ವಿಭಜನೆ->elementAt(i));
        RBBIStateDescriptor *ಸ್ಥಿರಾಂಕ state =
            static_cast<RBBIStateDescriptor*>(old_ಸ್ಥಿತಿಗಳು->elementAt(part.elementAti(0)));
        fDStates->ಕೂಡಿಸು_Element(state, *f_ಸ್ಥಿತಿ);
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < state->fDtran->size(); ++j) {
            state->fDtran->ಗಣ_Element_At(old_State_ಗಮ್ಯ_Part[state->fDtran->elementAti(j)], j);
        }
        old_ಸ್ಥಿತಿಗಳು->ಗಣ_Element_At(ಶೂನ್ಯಸೂಚಕ, part.elementAti(0));
    }
    old_ಸ್ಥಿತಿಗಳು->ಗಣ_Deleter([](ಶೂನ್ಯ *p) { ಅಳಿಸು static_cast<RBBIStateDescriptor*>(p); });
}







//-----------------------------------------------------------------------------
//
//  sortedAdd  Add a value to a vector of sorted values (ints).
//             Do not replicate entries; if the value is already there, do not
//                add a second one.
//             Lazily create the vector if it does not already exist.
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::sorted_ಕೂಡಿಸು(UVector **vector, int32_t val) {
    int32_t i;

    ಆದರೆ (*vector == ಶೂನ್ಯಸೂಚಕ) {
        *vector = ಹೊಸ UVector(*f_ಸ್ಥಿತಿ);
    }
    ಆದರೆ (*vector == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    UVector *vec = *vector;
    int32_t  v_ಗಾತ್ರ = vec->size();
    ಗಾಗಿ (i=0; i<v_ಗಾತ್ರ; i++) {
        int32_t valAtI = vec->elementAti(i);
        ಆದರೆ (valAtI == val) {
            // The value is already in the vector.  Don't add it again.
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (valAtI > val) {
            ನಿಲ್ಲಿಸು;
        }
    }
    vec->ಸೇರಿಸು_Element_At(val, i, *f_ಸ್ಥಿತಿ);
}



//-----------------------------------------------------------------------------
//
//  setAdd     Set operation on UVector
//             dest = dest union source
//             Elements may only appear once and must be sorted.
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::ಗಣ_ಕೂಡಿಸು_2(UVector *dest, UVector *ಮೂಲ) {
    U_ASSERT(!dest->hasDeleter());
    U_ASSERT(!ಮೂಲ->hasDeleter());
    int32_t dest_Original_ಗಾತ್ರ = dest->size();
    int32_t ಮೂಲ_ಗಾತ್ರ       = ಮೂಲ->size();
    int32_t di           = 0;
    Maybe_ರಾಶಿ_Array<ಶೂನ್ಯ *, 16> destArray, ಮೂಲ_Array;  // Handle small cases without malloc
    ಶೂನ್ಯ **destPtr, **ಮೂಲ_Ptr;
    ಶೂನ್ಯ **destLim, **ಮೂಲ_Lim;

    ಆದರೆ (dest_Original_ಗಾತ್ರ > destArray.get_ಸಾಮರ್ಥ್ಯ()) {
        ಆದರೆ (destArray.resize(dest_Original_ಗಾತ್ರ) == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ;
        }
    }
    destPtr = destArray.getAlias();
    destLim = destPtr + dest_Original_ಗಾತ್ರ;  // destArray.getArrayLimit()?

    ಆದರೆ (ಮೂಲ_ಗಾತ್ರ > ಮೂಲ_Array.get_ಸಾಮರ್ಥ್ಯ()) {
        ಆದರೆ (ಮೂಲ_Array.resize(ಮೂಲ_ಗಾತ್ರ) == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ;
        }
    }
    ಮೂಲ_Ptr = ಮೂಲ_Array.getAlias();
    ಮೂಲ_Lim = ಮೂಲ_Ptr + ಮೂಲ_ಗಾತ್ರ;  // sourceArray.getArrayLimit()?

    // Avoid multiple "get element" calls by getting the contents into arrays
    (ಶೂನ್ಯ) dest->ಗಮ್ಯ_Array(destPtr);
    (ಶೂನ್ಯ) ಮೂಲ->ಗಮ್ಯ_Array(ಮೂಲ_Ptr);

    dest->ಗಣ_ಗಾತ್ರ(ಮೂಲ_ಗಾತ್ರ+dest_Original_ಗಾತ್ರ, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆಗ (ಮೂಲ_Ptr < ಮೂಲ_Lim && destPtr < destLim) {
        ಆದರೆ (*destPtr == *ಮೂಲ_Ptr) {
            dest->ಗಣ_Element_At(*ಮೂಲ_Ptr++, di++);
            destPtr++;
        }
        // This check is required for machines with segmented memory, like i5/OS.
        // Direct pointer comparison is not recommended.
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_memcmp(destPtr, ಮೂಲ_Ptr, ಗಾತ್ರ(ಶೂನ್ಯ *)) < 0) {
            dest->ಗಣ_Element_At(*destPtr++, di++);
        }
        ಇಲ್ಲದಿದ್ದರೆ { /* *sourcePtr < *destPtr */
            dest->ಗಣ_Element_At(*ಮೂಲ_Ptr++, di++);
        }
    }

    // At most one of these two cleanup loops will execute
    ಆಗ (destPtr < destLim) {
        dest->ಗಣ_Element_At(*destPtr++, di++);
    }
    ಆಗ (ಮೂಲ_Ptr < ಮೂಲ_Lim) {
        dest->ಗಣ_Element_At(*ಮೂಲ_Ptr++, di++);
    }

    dest->ಗಣ_ಗಾತ್ರ(di, *f_ಸ್ಥಿತಿ);
}



//-----------------------------------------------------------------------------
//
//  setEqual    Set operation on UVector.
//              Compare for equality.
//              Elements must be sorted.
//
//-----------------------------------------------------------------------------
UBool RBBITableBuilder::ಗಣ_Equals(UVector *a, UVector *b) {
    ಹಿಂತಿರುಗಿ a->equals(*b);
}


//-----------------------------------------------------------------------------
//
//  printPosSets   Debug function.  Dump Nullable, firstpos, lastpos and followpos
//                 for each node in the tree.
//
//-----------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಶೂನ್ಯ RBBITableBuilder::printPosSets(RBBINode *n) {
    ಆದರೆ (n==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }
    ಬರೆ("\n");
    RBBINode::print_ಗಂಟು_ಶೀರ್ಷಿಕೆ();
    RBBINode::print_ಗಂಟು(n);
    RBBIDebugPrintf("         Nullable:  %s\n", n->fNullable?"true":"false");

    RBBIDebugPrintf("         firstpos:  ");
    print_ಗಣ(n->f_ಮೊದಲ_Pos_ಗಣ);

    RBBIDebugPrintf("         lastpos:   ");
    print_ಗಣ(n->f_ಕೊನೆಯ_Pos_ಗಣ);

    RBBIDebugPrintf("         followpos: ");
    print_ಗಣ(n->fFollowPos);

    printPosSets(n->f_ಎಡ_ಮಗು);
    printPosSets(n->f_ಬಲ_ಮಗು);
}
#ಆದರೆ_ಅಂತ್ಯ

//
//    findDuplCharClassFrom()
//
ತಾರ್ಕಿಕ RBBITableBuilder::ಕಂಡುಹಿಡಿ_Dupl_Char_Class_From(IntPair *categories) {
    int32_t ಅಂಕೆ_ಸ್ಥಿತಿಗಳು = fDStates->size();
    int32_t ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories();

    ಗಾಗಿ (; categories->first < ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ-1; categories->first++) {
        // Note: dictionary & non-dictionary columns cannot be merged.
        //       The limitSecond value prevents considering mixed pairs.
        //       Dictionary categories are >= DictCategoriesStart.
        //       Non dict categories are   <  DictCategoriesStart.
        ಪೂರ್ಣಾಂಕ ಎಲ್ಲೆ_ಎರಡನೇ = categories->first < fRB->f_ಗಣ_Builder->get_Dict_Categories_ಆರಂಭ() ?
            fRB->f_ಗಣ_Builder->get_Dict_Categories_ಆರಂಭ() : ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
        ಗಾಗಿ (categories->ಎರಡನೇ=categories->first+1; categories->ಎರಡನೇ < ಎಲ್ಲೆ_ಎರಡನೇ; categories->ಎರಡನೇ++) {
            // Initialized to different values to prevent returning true if numStates = 0 (implies no duplicates).
            uint16_t ಕೋಷ್ಟಕ_ತಳ = 0;
            uint16_t ಕೋಷ್ಟಕ_dupl = 1;
            ಗಾಗಿ (int32_t state=0; state<ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; state++) {
                RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
                ಕೋಷ್ಟಕ_ತಳ = static_cast<uint16_t>(sd->fDtran->elementAti(categories->first));
                ಕೋಷ್ಟಕ_dupl = static_cast<uint16_t>(sd->fDtran->elementAti(categories->ಎರಡನೇ));
                ಆದರೆ (ಕೋಷ್ಟಕ_ತಳ != ಕೋಷ್ಟಕ_dupl) {
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಆದರೆ (ಕೋಷ್ಟಕ_ತಳ == ಕೋಷ್ಟಕ_dupl) {
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}


//
//    removeColumn()
//
ಶೂನ್ಯ RBBITableBuilder::remove_ಸ್ತಂಭ(int32_t ಸ್ತಂಭ) {
    int32_t ಅಂಕೆ_ಸ್ಥಿತಿಗಳು = fDStates->size();
    ಗಾಗಿ (int32_t state=0; state<ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        U_ASSERT(ಸ್ತಂಭ < sd->fDtran->size());
        sd->fDtran->removeElementAt(ಸ್ತಂಭ);
    }
}


ತಾರ್ಕಿಕ RBBITableBuilder::ಕಂಡುಹಿಡಿ_Duplicate_ಸುರಕ್ಷಿತ_State(IntPair *ಸ್ಥಿತಿಗಳು) {
    int32_t ಅಂಕೆ_ಸ್ಥಿತಿಗಳು = f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->size();

    ಗಾಗಿ (; ಸ್ಥಿತಿಗಳು->first<ಅಂಕೆ_ಸ್ಥಿತಿಗಳು-1; ಸ್ಥಿತಿಗಳು->first++) {
        UnicodeString *ಮೊದಲ_ಅಡ್ಡಸಾಲು = static_cast<UnicodeString *>(f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->elementAt(ಸ್ಥಿತಿಗಳು->first));
        ಗಾಗಿ (ಸ್ಥಿತಿಗಳು->ಎರಡನೇ=ಸ್ಥಿತಿಗಳು->first+1; ಸ್ಥಿತಿಗಳು->ಎರಡನೇ<ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; ಸ್ಥಿತಿಗಳು->ಎರಡನೇ++) {
            UnicodeString *dupl_ಅಡ್ಡಸಾಲು = static_cast<UnicodeString *>(f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->elementAt(ಸ್ಥಿತಿಗಳು->ಎರಡನೇ));
            ತಾರ್ಕಿಕ ಅಡ್ಡಸಾಲುಗಳು_ಹೊಂದಿಕೆ = ನಿಜ;
            int32_t ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಮೊದಲ_ಅಡ್ಡಸಾಲು->length();
            ಗಾಗಿ (int32_t ಲಂಬಸಾಲು=0; ಲಂಬಸಾಲು < ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ++ಲಂಬಸಾಲು) {
                int32_t ಮೊದಲ_Val = ಮೊದಲ_ಅಡ್ಡಸಾಲು->charAt(ಲಂಬಸಾಲು);
                int32_t duplVal = dupl_ಅಡ್ಡಸಾಲು->charAt(ಲಂಬಸಾಲು);
                ಆದರೆ (!((ಮೊದಲ_Val == duplVal) ||
                        ((ಮೊದಲ_Val == ಸ್ಥಿತಿಗಳು->first || ಮೊದಲ_Val == ಸ್ಥಿತಿಗಳು->ಎರಡನೇ) &&
                        (duplVal  == ಸ್ಥಿತಿಗಳು->first || duplVal  == ಸ್ಥಿತಿಗಳು->ಎರಡನೇ)))) {
                    ಅಡ್ಡಸಾಲುಗಳು_ಹೊಂದಿಕೆ = ಸುಳ್ಳು;
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು_ಹೊಂದಿಕೆ) {
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ RBBITableBuilder::remove_ಸುರಕ್ಷಿತ_State(IntPair dupl_ಸ್ಥಿತಿಗಳು) {
    ಸ್ಥಿರಾಂಕ int32_t keepState = dupl_ಸ್ಥಿತಿಗಳು.first;
    ಸ್ಥಿರಾಂಕ int32_t duplState = dupl_ಸ್ಥಿತಿಗಳು.ಎರಡನೇ;
    U_ASSERT(keepState < duplState);
    U_ASSERT(duplState < f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->size());

    f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->removeElementAt(duplState);   // Note that fSafeTable has a deleter function
                                              // and will auto-delete the removed element.
    int32_t ಅಂಕೆ_ಸ್ಥಿತಿಗಳು = f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->size();
    ಗಾಗಿ (int32_t state=0; state<ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; ++state) {
        UnicodeString* sd = static_cast<UnicodeString*>(f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->elementAt(state));
        int32_t ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = sd->length();
        ಗಾಗಿ (int32_t ಲಂಬಸಾಲು=0; ಲಂಬಸಾಲು<ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ಲಂಬಸಾಲು++) {
            int32_t existingVal = sd->charAt(ಲಂಬಸಾಲು);
            int32_t newVal = existingVal;
            ಆದರೆ (existingVal == duplState) {
                newVal = keepState;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (existingVal > duplState) {
                newVal = existingVal - 1;
            }
            sd->ಗಣ_Char_At(ಲಂಬಸಾಲು, static_cast<char16_t>(newVal));
        }
    }
}


/*
 * RemoveDuplicateStates
 */
int32_t RBBITableBuilder::remove_Duplicate_ಸ್ಥಿತಿಗಳು() {
    ಸ್ಥಿರಾಂಕ int32_t old_State_ಎಣಿಕೆ = fDStates->size();
    minimize_ಸ್ಥಿತಿಗಳು();
    ಹಿಂತಿರುಗಿ fDStates->size() - old_State_ಎಣಿಕೆ;
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
ಶೂನ್ಯ RBBITableBuilder::minimizeLookaheads() {
    ಆದರೆ (f_LASlots_ಒಳ_Use == ACCEPTING_UNCONDITIONAL) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಥಿರಾಂಕ int32_t lookahead_ಎಣಿಕೆ = f_LASlots_ಒಳ_Use - ACCEPTING_UNCONDITIONAL;
    // The last lookahead is fLASlotsInUse.
    ಸ್ಥಿರಾಂಕ int32_t ಮೊದಲ_Lookahead = ACCEPTING_UNCONDITIONAL + 1;
    ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ stateDescriptor = [ಇದು](ಸ್ಥಿರಾಂಕ int32_t state) -> RBBIStateDescriptor & {
        ಹಿಂತಿರುಗಿ *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(state));
    };

    ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ lookaheadReachability =
        prv::make_unique<ತಾರ್ಕಿಕ[]>(lookahead_ಎಣಿಕೆ * lookahead_ಎಣಿಕೆ, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (int32_t l = ಮೊದಲ_Lookahead; l <= f_LASlots_ಒಳ_Use; ++l) {
        ಗಾಗಿ (int32_t k = ಮೊದಲ_Lookahead; k <= f_LASlots_ಒಳ_Use; ++k) {
            ಆದರೆ (k != l) {
                ಗಾಗಿ (int32_t ಮೂಲ = 1; ಮೂಲ < fDStates->size(); ++ಮೂಲ) {
                    ಆದರೆ (static_cast<int32_t>(stateDescriptor(ಮೂಲ).fLookAhead) != l) {
                        ಮುಂದುವರಿ;
                    }
                    ಆದರೆ (reachableByTransitions(
                            *fDStates, ಮೂಲ, /*isSink=*/
                            [&](ಸ್ಥಿರಾಂಕ int32_t state) {
                                ಹಿಂತಿರುಗಿ static_cast<int32_t>(stateDescriptor(state).fAccepting) == k;
                            },
                            /*excludedState=*/
                            [&](ಸ್ಥಿರಾಂಕ int32_t state) {
                                ಹಿಂತಿರುಗಿ static_cast<int32_t>(stateDescriptor(state).fLookAhead) == k;
                            },
                            *f_ಸ್ಥಿತಿ)) {
                        lookaheadReachability[(l - ಮೊದಲ_Lookahead) * lookahead_ಎಣಿಕೆ + k -
                                              ಮೊದಲ_Lookahead] = ನಿಜ;
                    }
                }
            }
        }
    }

    ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ colours = prv::make_unique_for_overwrite<int32_t[]>(lookahead_ಎಣಿಕೆ, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    // Brute-force colouring: the number of lookaheads is in practice small, and the chromatic
    // number is in practice 1, so this usually terminates in one iteration of the first two loops.
    // Of course the worst case is exponential, but exponentials are everywhere in regular
    // expressions anyway.
    // The outer loop terminates after at most `lookaheadCount` iterations
    // (when the chromatic number equals the number of lookaheads being coloured).
    ಗಾಗಿ (int32_t chromatic_ಸಂಖ್ಯೆ = 1;; ++chromatic_ಸಂಖ್ಯೆ) {
        uprv_memset(colours.get(), 0, ಗಾತ್ರ(int32_t) * lookahead_ಎಣಿಕೆ);
        // This loop terminates after at most chromaticNumber ** lookaheadCount iterations.
        ಗಾಗಿ (;;) {
            // Here `source` and `sink` correspond to lookaheads, not states.
            ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಮೂಲ = 0; ಮೂಲ < lookahead_ಎಣಿಕೆ; ++ಮೂಲ) {
                ಗಾಗಿ (ಪೂರ್ಣಾಂಕ sink = 0; sink < lookahead_ಎಣಿಕೆ; ++sink) {
                    ಆದರೆ (lookaheadReachability[ಮೂಲ * lookahead_ಎಣಿಕೆ + sink] &&
                        colours[ಮೂಲ] == colours[sink]) {
                        ಹೋಗು ಮುಂದಿನ_Colouring;
                    }
                }
            }

            // We have found a valid colouring of the graph of lookaheads.  Assign lookahead slots
            // accordingly.
            ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < fDStates->size(); ++i) {
                ಆದರೆ (stateDescriptor(i).fAccepting > ACCEPTING_UNCONDITIONAL) {
                    stateDescriptor(i).fAccepting =
                        ಮೊದಲ_Lookahead +
                        colours[stateDescriptor(i).fAccepting - ಮೊದಲ_Lookahead];
                }
                ಆದರೆ (stateDescriptor(i).fLookAhead != 0) {
                    stateDescriptor(i).fLookAhead =
                        ACCEPTING_UNCONDITIONAL + 1 +
                        colours[stateDescriptor(i).fLookAhead - ಮೊದಲ_Lookahead];
                }
            }
            f_LASlots_ಒಳ_Use = ACCEPTING_UNCONDITIONAL + chromatic_ಸಂಖ್ಯೆ;
            ಹಿಂತಿರುಗಿ;

          ಮುಂದಿನ_Colouring:
            // Increment the colour of the first lookahead, and then carry if we reach
            // `chromaticNumber` (which is one more than the greatest colour).
            ++colours[0];
            ಗಾಗಿ (int32_t i = 0; i < lookahead_ಎಣಿಕೆ - 1 && colours[i] == chromatic_ಸಂಖ್ಯೆ;
                 ++i) {
                colours[i] = 0;
                ++colours[i + 1];
            }
            ಆದರೆ (colours[lookahead_ಎಣಿಕೆ - 1] == chromatic_ಸಂಖ್ಯೆ) {
                // We tried all assignments of colours in {0, …, chromaticNumber - 1},
                // we need more colours.
                ನಿಲ್ಲಿಸು;
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
int32_t  RBBITableBuilder::get_ಕೋಷ್ಟಕ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ {
    int32_t    size = 0;
    int32_t    ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು;
    int32_t    ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    int32_t    ಅಡ್ಡಸಾಲು_ಗಾತ್ರ;

    ಆದರೆ (f_ವೃಕ್ಷ == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0;
    }

    size    = offsetof(RBBIState_ಕೋಷ್ಟಕ, f_ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ);    // The header, with no rows to the table.

    ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು = fDStates->size();
    ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories();

    ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು_For_ಕೋಷ್ಟಕ()) {
        ಅಡ್ಡಸಾಲು_ಗಾತ್ರ = offsetof(RBBIState_ಕೋಷ್ಟಕ_Row8, f_ಮುಂದಿನ_State) + ಗಾತ್ರ(int8_t)*ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಅಡ್ಡಸಾಲು_ಗಾತ್ರ = offsetof(RBBIState_ಕೋಷ್ಟಕ_Row16, f_ಮುಂದಿನ_State) + ಗಾತ್ರ(int16_t)*ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    }
    size   += ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು * ಅಡ್ಡಸಾಲು_ಗಾತ್ರ;
    ಹಿಂತಿರುಗಿ size;
}

ತಾರ್ಕಿಕ RBBITableBuilder::use8_ಬಿಟ್ಟುಗಳು_For_ಕೋಷ್ಟಕ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ fDStates->size() <= k_Max_State_For8_ಬಿಟ್ಟುಗಳು_ಕೋಷ್ಟಕ;
}

//-----------------------------------------------------------------------------
//
//   exportTable()    export the state transition table in the format required
//                    by the runtime engine.  getTableSize() bytes of memory
//                    must be available at the output address "where".
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::export_ಕೋಷ್ಟಕ(ಶೂನ್ಯ *where) {
    RBBIState_ಕೋಷ್ಟಕ* ಕೋಷ್ಟಕ = static_cast<RBBIState_ಕೋಷ್ಟಕ*>(where);
    uint32_t           state;
    ಪೂರ್ಣಾಂಕ                ಲಂಬಸಾಲು;

    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ) || f_ವೃಕ್ಷ == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }

    int32_t cat_ಎಣಿಕೆ = fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories();
    ಆದರೆ (cat_ಎಣಿಕೆ > 0x7fff ||
        fDStates->size() > 0x7fff) {
        *f_ಸ್ಥಿತಿ = U_BRK_ಆಂತರಿಕ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಕೋಷ್ಟಕ->f_ಅಂಕೆ_ಸ್ಥಿತಿಗಳು = fDStates->size();
    ಕೋಷ್ಟಕ->f_Dict_Categories_ಆರಂಭ = fRB->f_ಗಣ_Builder->get_Dict_Categories_ಆರಂಭ();
    ಕೋಷ್ಟಕ->f_Look_Ahead_Results_ಗಾತ್ರ = f_LASlots_ಒಳ_Use == ACCEPTING_UNCONDITIONAL ? 0 : f_LASlots_ಒಳ_Use + 1;
    ಕೋಷ್ಟಕ->fFlags     = 0;
    ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು_For_ಕೋಷ್ಟಕ()) {
        ಕೋಷ್ಟಕ->f_ಅಡ್ಡಸಾಲು_Len    = offsetof(RBBIState_ಕೋಷ್ಟಕ_Row8, f_ಮುಂದಿನ_State) + ಗಾತ್ರ(uint8_t) * cat_ಎಣಿಕೆ;
        ಕೋಷ್ಟಕ->fFlags  |= RBBI_8_ಬಿಟ್ಟುಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಕೋಷ್ಟಕ->f_ಅಡ್ಡಸಾಲು_Len    = offsetof(RBBIState_ಕೋಷ್ಟಕ_Row16, f_ಮುಂದಿನ_State) + ಗಾತ್ರ(int16_t) * cat_ಎಣಿಕೆ;
    }
    ಆದರೆ (fRB->fLookAheadHardBreak) {
        ಕೋಷ್ಟಕ->fFlags  |= RBBI_LOOKAHEAD_HARD_BREAK;
    }

    ಗಾಗಿ (state=0; state<ಕೋಷ್ಟಕ->f_ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; state++) {
        RBBIStateDescriptor* sd = static_cast<RBBIStateDescriptor*>(fDStates->elementAt(state));
        RBBIState_ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು* ಅಡ್ಡಸಾಲು = reinterpret_cast<RBBIState_ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು*>(ಕೋಷ್ಟಕ->f_ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ + state * ಕೋಷ್ಟಕ->f_ಅಡ್ಡಸಾಲು_Len);
        ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು_For_ಕೋಷ್ಟಕ()) {
            U_ASSERT (sd->fAccepting <= 255);
            U_ASSERT (sd->fLookAhead <= 255);
            U_ASSERT (0 <= sd->f_ಟ್ಯಾಗುಗಳು_Idx && sd->f_ಟ್ಯಾಗುಗಳು_Idx <= 255);
            RBBIState_ಕೋಷ್ಟಕ_Row8* r8 = reinterpret_cast<RBBIState_ಕೋಷ್ಟಕ_Row8*>(ಅಡ್ಡಸಾಲು);
            r8->fAccepting = sd->fAccepting;
            r8->fLookAhead = sd->fLookAhead;
            r8->f_ಟ್ಯಾಗುಗಳು_Idx   = sd->f_ಟ್ಯಾಗುಗಳು_Idx;
            ಗಾಗಿ (ಲಂಬಸಾಲು=0; ಲಂಬಸಾಲು<cat_ಎಣಿಕೆ; ಲಂಬಸಾಲು++) {
                U_ASSERT (sd->fDtran->elementAti(ಲಂಬಸಾಲು) <= k_Max_State_For8_ಬಿಟ್ಟುಗಳು_ಕೋಷ್ಟಕ);
                r8->f_ಮುಂದಿನ_State[ಲಂಬಸಾಲು] = sd->fDtran->elementAti(ಲಂಬಸಾಲು);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            U_ASSERT (sd->fAccepting <= 0xffff);
            U_ASSERT (sd->fLookAhead <= 0xffff);
            U_ASSERT (0 <= sd->f_ಟ್ಯಾಗುಗಳು_Idx && sd->f_ಟ್ಯಾಗುಗಳು_Idx <= 0xffff);
            ಅಡ್ಡಸಾಲು->r16.fAccepting = sd->fAccepting;
            ಅಡ್ಡಸಾಲು->r16.fLookAhead = sd->fLookAhead;
            ಅಡ್ಡಸಾಲು->r16.f_ಟ್ಯಾಗುಗಳು_Idx   = sd->f_ಟ್ಯಾಗುಗಳು_Idx;
            ಗಾಗಿ (ಲಂಬಸಾಲು=0; ಲಂಬಸಾಲು<cat_ಎಣಿಕೆ; ಲಂಬಸಾಲು++) {
                ಅಡ್ಡಸಾಲು->r16.f_ಮುಂದಿನ_State[ಲಂಬಸಾಲು] = sd->fDtran->elementAti(ಲಂಬಸಾಲು);
            }
        }
    }
}


/**
 *   Synthesize a safe state table from the main state table.
 */
ಶೂನ್ಯ RBBITableBuilder::build_ಸುರಕ್ಷಿತ_ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
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
    UnicodeString ಸುರಕ್ಷಿತ_ಜೋಡಿಗಳು;

    int32_t ಅಂಕೆ_Char_Classes = fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories();
    int32_t ಅಂಕೆ_ಸ್ಥಿತಿಗಳು = fDStates->size();
    // The elements at indices 0 and ACCEPTING_UNCONDITIONAL (1) are never used.
    ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ lookaheadPositions =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_ಒಳ_Use + 1, ಸ್ಥಿತಿ_2);
    ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ wanted_Lookaheads_ಒಳ_Pair =
        prv::make_unique_for_overwrite<int8_t[]>(f_LASlots_ಒಳ_Use + 1, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    // Same as in rbbi.cpp.
    // TODO(egg): Maybe this should move to rbbidata.h like ACCEPTING_UNCONDITIONAL?
    constexpr int32_t ಆರಂಭ_STATE = 1;
    // Index -2 is used to signify a text position before the pair (c1, c2).
    constexpr int32_t BEFORE_PAIR = -2;
    // In lookaheadPositions, index -1 is used to signify that the lookahead has not been set.
    constexpr int8_t LOOKAHEAD_NOT_ಗಣ = -1;
    ಗಾಗಿ (int32_t c1 = 1; c1 < ಅಂಕೆ_Char_Classes; ++c1) {
        ಆದರೆ (fRB->f_ಗಣ_Builder->get_ಮೊದಲ_Char(c1) < 0) {
          ಮುಂದುವರಿ;
        }
        ಗಾಗಿ (int32_t c2 = 1; c2 < ಅಂಕೆ_Char_Classes; ++c2) {
            ಆದರೆ (fRB->f_ಗಣ_Builder->get_ಮೊದಲ_Char(c2) < 0) {
                ಮುಂದುವರಿ;
            }
            int32_t wanted_ಕೊನೆ_State = -1;
            int32_t wanted_ಕೊನೆಯ_Accepting_Position;
            ತಾರ್ಕಿಕ must_Check_Consistent_With_ಆರಂಭ_C2 = ಸುಳ್ಳು;
            ಸ್ಥಿರಾಂಕ int32_t ಪಠ್ಯ[2] = {c1, c2};
            ಗಾಗಿ (int32_t injectedState = ಆರಂಭ_STATE; injectedState < ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; ++injectedState) {
                ಗಾಗಿ (int32_t injectedLookahead = ACCEPTING_UNCONDITIONAL;
                     injectedLookahead <= f_LASlots_ಒಳ_Use; ++injectedLookahead) {
                    int32_t ಕೊನೆಯ_Break = BEFORE_PAIR;
                    int32_t ಕೊನೆಯ_Accepting_Position = BEFORE_PAIR;
                    uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_ಗಣ, f_LASlots_ಒಳ_Use + 1);
                    ಆದರೆ (injectedLookahead != ACCEPTING_UNCONDITIONAL) {
                        lookaheadPositions[injectedLookahead] = BEFORE_PAIR;
                    }
                    {
                        ಸ್ಥಿರಾಂಕ RBBIStateDescriptor &injectedStateDescriptor =
                            *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(injectedState));
                        ಆದರೆ (injectedStateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL) {
                            ಕೊನೆಯ_Accepting_Position = 0;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (static_cast<int32_t>(injectedStateDescriptor.fAccepting) ==
                                   injectedLookahead) {
                            ಮುಂದುವರಿ;
                        }
                        ಆದರೆ (injectedStateDescriptor.fLookAhead != 0) {
                            lookaheadPositions[injectedStateDescriptor.fLookAhead] = 0;
                        }
                    }
                    int32_t s = injectedState;
                    int32_t i = 0;
                    ಹೋಗು dfa_ಹೆಜ್ಜೆ;
                    // Loop Run_DFA in L2/26-135, without the termination condition (we do not need
                    // to simulate the end of text).
                    ಗಾಗಿ (;;) {
                        s = ಆರಂಭ_STATE;
                        uprv_memset(lookaheadPositions.get(), LOOKAHEAD_NOT_ಗಣ, f_LASlots_ಒಳ_Use + 1);
                        i = ಕೊನೆಯ_Break;
                        // Loop Find_Next_Break in L2/26-135.
                        ಗಾಗಿ (;;) {
                        dfa_ಹೆಜ್ಜೆ:
                            ಆದರೆ (i == BEFORE_PAIR || i == 2) {
                                ಹೋಗು inspectFinalState;
                            }
                            ಸ್ಥಿರಾಂಕ int32_t ಚಿಹ್ನೆ_Ahead = ಪಠ್ಯ[i];
                            ++i;
                            ಸ್ಥಿರಾಂಕ RBBIStateDescriptor *sDescriptor =
                                static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            ಆದರೆ (sDescriptor->fDtran->elementAti(ಚಿಹ್ನೆ_Ahead) != 0) {
                                s = sDescriptor->fDtran->elementAti(ಚಿಹ್ನೆ_Ahead);
                                sDescriptor =
                                    static_cast<RBBIStateDescriptor *>(fDStates->elementAt(s));
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ (ಕೊನೆಯ_Break != BEFORE_PAIR &&
                                    ಕೊನೆಯ_Accepting_Position == ಕೊನೆಯ_Break) {
                                    // The pair (c1, c2) puts us in an infinite loop from the
                                    // current configuration, presumably because some one-character
                                    // strings are not in the language recognized by the rules).
                                    // This should never happen for a segmentation algorithm.  The
                                    // RBBI implementation forcefully advances the iterator in that
                                    // case; let’s just say the pair is unsafe and move on.
                                    ಹೋಗು ಮುಂದಿನ_ಚಿಹ್ನೆ_Pair;
                                }
                                ಕೊನೆಯ_Break = ಕೊನೆಯ_Accepting_Position;
                                ನಿಲ್ಲಿಸು;
                            }

                            ಆದರೆ (sDescriptor->fAccepting == ACCEPTING_UNCONDITIONAL) {
                                ಕೊನೆಯ_Accepting_Position = i;
                            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sDescriptor->fAccepting > ACCEPTING_UNCONDITIONAL &&
                                       lookaheadPositions[sDescriptor->fAccepting] !=
                                           LOOKAHEAD_NOT_ಗಣ) {
                                ಕೊನೆಯ_Break = lookaheadPositions[sDescriptor->fAccepting];
                                ನಿಲ್ಲಿಸು;
                            }
                            ಆದರೆ (sDescriptor->fLookAhead != 0) {
                                lookaheadPositions[sDescriptor->fLookAhead] = i;
                            }
                        }
                    }
                inspectFinalState:
                    ಆದರೆ (i == BEFORE_PAIR) {
                        // Starting on `injectedState` with `injectedLookahead` set, with the pair
                        // (c1, c2) ahead, the state machine finds a break before the pair; it will
                        // thus come back to the pair in a different configuration, which is covered
                        // by some other iteration of the loop over `injectedState` and
                        // `injectedLookahead`.
                        ಮುಂದುವರಿ;
                    }
                    ಆದರೆ (wanted_ಕೊನೆ_State < 0) {
                        wanted_ಕೊನೆ_State = s;
                        wanted_ಕೊನೆಯ_Accepting_Position = ಕೊನೆಯ_Accepting_Position;
                        ಗಾಗಿ (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_ಒಳ_Use; ++l) {
                            ಆದರೆ (lookaheadPositions[l] != LOOKAHEAD_NOT_ಗಣ &&
                                lookaheadPositions[l] != BEFORE_PAIR &&
                                lookaheadPositions[l] != 0) {
                                wanted_Lookaheads_ಒಳ_Pair[l] = lookaheadPositions[i];
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                wanted_Lookaheads_ಒಳ_Pair[l] = LOOKAHEAD_NOT_ಗಣ;
                            }
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ (wanted_ಕೊನೆ_State != s) {
                            // We can get out of the pair on different states depending on the
                            // initial configuration.
                            ಹೋಗು ಮುಂದಿನ_ಚಿಹ್ನೆ_Pair;
                        }
                        ಆದರೆ (wanted_ಕೊನೆಯ_Accepting_Position != ಕೊನೆಯ_Accepting_Position) {
                            ಹೋಗು ಮುಂದಿನ_ಚಿಹ್ನೆ_Pair;
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
                    ಗಾಗಿ (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_ಒಳ_Use; ++l) {
                        ಆದರೆ (lookaheadPositions[l] == BEFORE_PAIR || lookaheadPositions[l] == 0 ||
                            lookaheadPositions[l] == wanted_Lookaheads_ಒಳ_Pair[l]) {
                            ಮುಂದುವರಿ;
                        }
                        ಆದರೆ (lookaheadPositions[l] == 2 || wanted_Lookaheads_ಒಳ_Pair[l] == 2) {
                            // A lookahead after the pair means we will come back to the end of the
                            // pair on the start state if it matches, so in order for the pair to be
                            // safe if that lookahead is not consistently set, we must have left it
                            // on the start state.
                            ಆದರೆ (wanted_ಕೊನೆ_State != ಆರಂಭ_STATE) {
                                ಹೋಗು ಮುಂದಿನ_ಚಿಹ್ನೆ_Pair;
                            }
                        }
                        ಆದರೆ (lookaheadPositions[l] == 1 || wanted_Lookaheads_ಒಳ_Pair[l] == 1) {
                            // If a lookahead in the middle of the pair matches, we will come back
                            // to c2 on the start state.
                            must_Check_Consistent_With_ಆರಂಭ_C2 = ನಿಜ;
                        }
                    }
                }
            }
            ಆದರೆ (must_Check_Consistent_With_ಆರಂಭ_C2) {
                ಸ್ಥಿರಾಂಕ RBBIStateDescriptor& ಆರಂಭ_Descriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(ಆರಂಭ_STATE));
                ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ c2State = ಆರಂಭ_Descriptor.fDtran->elementAti(c2);
                ಆದರೆ (c2State != wanted_ಕೊನೆ_State) {
                    ಹೋಗು ಮುಂದಿನ_ಚಿಹ್ನೆ_Pair;
                }
                ಸ್ಥಿರಾಂಕ RBBIStateDescriptor& c2StateDescriptor =
                    *static_cast<RBBIStateDescriptor *>(fDStates->elementAt(c2State));
                // In a segmentation algorithm, any single symbol transitions to an
                // accepting state from the start state (a single character is a
                // possible segment), so we know c2State is accepting.
                U_ASSERT(c2StateDescriptor.fAccepting == ACCEPTING_UNCONDITIONAL);
                ಆದರೆ (wanted_ಕೊನೆಯ_Accepting_Position != 2) {
                    ಹೋಗು ಮುಂದಿನ_ಚಿಹ್ನೆ_Pair;
                }
                // If that in turn sets a lookahead, we have a lookahead after the
                // pair.  If we consistently have that the pair is safe.  Otherwise, if we match
                // that lookahead, we end up on the start state.
                ಗಾಗಿ (int32_t l = ACCEPTING_UNCONDITIONAL + 1; l <= f_LASlots_ಒಳ_Use; ++l) {
                    ಆದರೆ (((wanted_Lookaheads_ಒಳ_Pair[l] == 2) !=
                         (static_cast<int32_t>(c2StateDescriptor.fLookAhead) == l)) &&
                        wanted_ಕೊನೆ_State != ಆರಂಭ_STATE) {
                        ಹೋಗು ಮುಂದಿನ_ಚಿಹ್ನೆ_Pair;
                    }
                }
            }
            ಸುರಕ್ಷಿತ_ಜೋಡಿಗಳು.append(static_cast<char16_t>(c1));
            ಸುರಕ್ಷಿತ_ಜೋಡಿಗಳು.append(static_cast<char16_t>(c2));
        ಮುಂದಿನ_ಚಿಹ್ನೆ_Pair:
            ಮುಂದುವರಿ;
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

    U_ASSERT(f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ == ಶೂನ್ಯಸೂಚಕ);
    Local_ಸೂಚಕ<UVector> lp_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ(
        ಹೊಸ UVector(uprv_deleteUObject, uhash_compareUnicodeString, ಅಂಕೆ_Char_Classes + 2, ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ = lp_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ.orphan();
    ಗಾಗಿ (int32_t ಅಡ್ಡಸಾಲು=0; ಅಡ್ಡಸಾಲು<ಅಂಕೆ_Char_Classes + 2; ++ಅಡ್ಡಸಾಲು) {
        Local_ಸೂಚಕ<UnicodeString> lpString(ಹೊಸ UnicodeString(ಅಂಕೆ_Char_Classes, 0, ಅಂಕೆ_Char_Classes+4), ಸ್ಥಿತಿ_2);
        f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->adoptElement(lpString.orphan(), ಸ್ಥಿತಿ_2);
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }

    // From the start state, each input char class transitions to the state for that input.
    UnicodeString &ಆರಂಭ_State = *static_cast<UnicodeString *>(f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->elementAt(1));
    ಗಾಗಿ (int32_t charClass=0; charClass < ಅಂಕೆ_Char_Classes; ++charClass) {
        // Note: +2 for the start & stop state.
        ಆರಂಭ_State.ಗಣ_Char_At(charClass, static_cast<char16_t>(charClass+2));
    }

    // Initially make every other state table row look like the start state row,
    ಗಾಗಿ (int32_t ಅಡ್ಡಸಾಲು=2; ಅಡ್ಡಸಾಲು<ಅಂಕೆ_Char_Classes+2; ++ಅಡ್ಡಸಾಲು) {
        UnicodeString &ಅಡ್ಡಸಾಲು_State = *static_cast<UnicodeString *>(f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->elementAt(ಅಡ್ಡಸಾಲು));
        ಅಡ್ಡಸಾಲು_State = ಆರಂಭ_State;   // UnicodeString assignment, copies contents.
    }

    // Run through the safe pairs, set the next state to zero when pair has been seen.
    // Zero being the stop state, meaning we found a safe point.
    ಗಾಗಿ (int32_t pairIdx=0; pairIdx<ಸುರಕ್ಷಿತ_ಜೋಡಿಗಳು.length(); pairIdx+=2) {
        int32_t c1 = ಸುರಕ್ಷಿತ_ಜೋಡಿಗಳು.charAt(pairIdx);
        int32_t c2 = ಸುರಕ್ಷಿತ_ಜೋಡಿಗಳು.charAt(pairIdx + 1);

        UnicodeString &ಅಡ್ಡಸಾಲು_State = *static_cast<UnicodeString *>(f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->elementAt(c2 + 2));
        ಅಡ್ಡಸಾಲು_State.ಗಣ_Char_At(c1, 0);
    }

    // Remove duplicate or redundant rows from the table.
    IntPair ಸ್ಥಿತಿಗಳು = {1, 0};
    ಆಗ (ಕಂಡುಹಿಡಿ_Duplicate_ಸುರಕ್ಷಿತ_State(&ಸ್ಥಿತಿಗಳು)) {
        // printf("Removing duplicate safe states (%d, %d)\n", states.first, states.second);
        remove_ಸುರಕ್ಷಿತ_State(ಸ್ಥಿತಿಗಳು);
    }
}


//-----------------------------------------------------------------------------
//
//   getSafeTableSize()    Calculate the size of the runtime form of this
//                         safe state table.
//
//-----------------------------------------------------------------------------
int32_t  RBBITableBuilder::get_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ {
    int32_t    size = 0;
    int32_t    ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು;
    int32_t    ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    int32_t    ಅಡ್ಡಸಾಲು_ಗಾತ್ರ;

    ಆದರೆ (f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0;
    }

    size    = offsetof(RBBIState_ಕೋಷ್ಟಕ, f_ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ);    // The header, with no rows to the table.

    ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು = f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->size();
    ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories();

    ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು_For_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ()) {
        ಅಡ್ಡಸಾಲು_ಗಾತ್ರ = offsetof(RBBIState_ಕೋಷ್ಟಕ_Row8, f_ಮುಂದಿನ_State) + ಗಾತ್ರ(int8_t)*ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಅಡ್ಡಸಾಲು_ಗಾತ್ರ = offsetof(RBBIState_ಕೋಷ್ಟಕ_Row16, f_ಮುಂದಿನ_State) + ಗಾತ್ರ(int16_t)*ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    }
    size   += ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು * ಅಡ್ಡಸಾಲು_ಗಾತ್ರ;
    ಹಿಂತಿರುಗಿ size;
}

ತಾರ್ಕಿಕ RBBITableBuilder::use8_ಬಿಟ್ಟುಗಳು_For_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->size() <= k_Max_State_For8_ಬಿಟ್ಟುಗಳು_ಕೋಷ್ಟಕ;
}

//-----------------------------------------------------------------------------
//
//   exportSafeTable()   export the state transition table in the format required
//                       by the runtime engine.  getTableSize() bytes of memory
//                       must be available at the output address "where".
//
//-----------------------------------------------------------------------------
ಶೂನ್ಯ RBBITableBuilder::export_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ(ಶೂನ್ಯ *where) {
    RBBIState_ಕೋಷ್ಟಕ* ಕೋಷ್ಟಕ = static_cast<RBBIState_ಕೋಷ್ಟಕ*>(where);
    uint32_t           state;
    ಪೂರ್ಣಾಂಕ                ಲಂಬಸಾಲು;

    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ) || f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }

    int32_t cat_ಎಣಿಕೆ = fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories();
    ಆದರೆ (cat_ಎಣಿಕೆ > 0x7fff ||
            f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->size() > 0x7fff) {
        *f_ಸ್ಥಿತಿ = U_BRK_ಆಂತರಿಕ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಕೋಷ್ಟಕ->f_ಅಂಕೆ_ಸ್ಥಿತಿಗಳು = f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->size();
    ಕೋಷ್ಟಕ->fFlags     = 0;
    ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು_For_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ()) {
        ಕೋಷ್ಟಕ->f_ಅಡ್ಡಸಾಲು_Len    = offsetof(RBBIState_ಕೋಷ್ಟಕ_Row8, f_ಮುಂದಿನ_State) + ಗಾತ್ರ(uint8_t) * cat_ಎಣಿಕೆ;
        ಕೋಷ್ಟಕ->fFlags  |= RBBI_8_ಬಿಟ್ಟುಗಳು_ಅಡ್ಡಸಾಲುಗಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಕೋಷ್ಟಕ->f_ಅಡ್ಡಸಾಲು_Len    = offsetof(RBBIState_ಕೋಷ್ಟಕ_Row16, f_ಮುಂದಿನ_State) + ಗಾತ್ರ(int16_t) * cat_ಎಣಿಕೆ;
    }

    ಗಾಗಿ (state=0; state<ಕೋಷ್ಟಕ->f_ಅಂಕೆ_ಸ್ಥಿತಿಗಳು; state++) {
        UnicodeString* ಅಡ್ಡಸಾಲು_String = static_cast<UnicodeString*>(f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->elementAt(state));
        RBBIState_ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು* ಅಡ್ಡಸಾಲು = reinterpret_cast<RBBIState_ಕೋಷ್ಟಕ_ಅಡ್ಡಸಾಲು*>(ಕೋಷ್ಟಕ->f_ಕೋಷ್ಟಕ_ದತ್ತಾಂಶ + state * ಕೋಷ್ಟಕ->f_ಅಡ್ಡಸಾಲು_Len);
        ಆದರೆ (use8_ಬಿಟ್ಟುಗಳು_For_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ()) {
            RBBIState_ಕೋಷ್ಟಕ_Row8* r8 = reinterpret_cast<RBBIState_ಕೋಷ್ಟಕ_Row8*>(ಅಡ್ಡಸಾಲು);
            r8->fAccepting = 0;
            r8->fLookAhead = 0;
            r8->f_ಟ್ಯಾಗುಗಳು_Idx    = 0;
            ಗಾಗಿ (ಲಂಬಸಾಲು=0; ಲಂಬಸಾಲು<cat_ಎಣಿಕೆ; ಲಂಬಸಾಲು++) {
                U_ASSERT(ಅಡ್ಡಸಾಲು_String->charAt(ಲಂಬಸಾಲು) <= k_Max_State_For8_ಬಿಟ್ಟುಗಳು_ಕೋಷ್ಟಕ);
                r8->f_ಮುಂದಿನ_State[ಲಂಬಸಾಲು] = static_cast<uint8_t>(ಅಡ್ಡಸಾಲು_String->charAt(ಲಂಬಸಾಲು));
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಅಡ್ಡಸಾಲು->r16.fAccepting = 0;
            ಅಡ್ಡಸಾಲು->r16.fLookAhead = 0;
            ಅಡ್ಡಸಾಲು->r16.f_ಟ್ಯಾಗುಗಳು_Idx    = 0;
            ಗಾಗಿ (ಲಂಬಸಾಲು=0; ಲಂಬಸಾಲು<cat_ಎಣಿಕೆ; ಲಂಬಸಾಲು++) {
                ಅಡ್ಡಸಾಲು->r16.f_ಮುಂದಿನ_State[ಲಂಬಸಾಲು] = ಅಡ್ಡಸಾಲು_String->charAt(ಲಂಬಸಾಲು);
            }
        }
    }
}




//-----------------------------------------------------------------------------
//
//   printSet    Debug function.   Print the contents of a UVector
//
//-----------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಶೂನ್ಯ RBBITableBuilder::print_ಗಣ(UVector *s) {
    int32_t  i;
    ಗಾಗಿ (i=0; i<s->size(); i++) {
        ಸ್ಥಿರಾಂಕ RBBINode *v = static_cast<ಸ್ಥಿರಾಂಕ RBBINode *>(s->elementAt(i));
        RBBIDebugPrintf("%5d", v==ಶೂನ್ಯಸೂಚಕ? -1 : v->f_Serial_ಅಂಕೆ);
    }
    RBBIDebugPrintf("\n");
}
#ಆದರೆ_ಅಂತ್ಯ


//-----------------------------------------------------------------------------
//
//   printStates    Debug Function.  Dump the fully constructed state transition table.
//
//-----------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಶೂನ್ಯ RBBITableBuilder::print_ಸ್ಥಿತಿಗಳು() {
    ಪೂರ್ಣಾಂಕ     c;    // input "character"
    ಪೂರ್ಣಾಂಕ     n;    // state number

    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    ಗಾಗಿ (c=0; c<fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories(); c++) {
        RBBIDebugPrintf(" %3d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    ಗಾಗಿ (c=0; c<fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories(); c++) {
        RBBIDebugPrintf("----");
    }
    RBBIDebugPrintf("\n");

    ಗಾಗಿ (n=0; n<fDStates->size(); n++) {
        RBBIStateDescriptor *sd = (RBBIStateDescriptor *)fDStates->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", sd->fAccepting, sd->fLookAhead, sd->f_ಟ್ಯಾಗುಗಳು_Idx);
        ಗಾಗಿ (c=0; c<fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories(); c++) {
            RBBIDebugPrintf(" %3d", sd->fDtran->elementAti(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#ಆದರೆ_ಅಂತ್ಯ


//-----------------------------------------------------------------------------
//
//   printSafeTable    Debug Function.  Dump the fully constructed safe table.
//
//-----------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಶೂನ್ಯ RBBITableBuilder::print_ಹಿಮ್ಮುಖ_ಕೋಷ್ಟಕ() {
    ಪೂರ್ಣಾಂಕ     c;    // input "character"
    ಪೂರ್ಣಾಂಕ     n;    // state number

    RBBIDebugPrintf("    Safe Reverse Table \n");
    ಆದರೆ (f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ == ಶೂನ್ಯಸೂಚಕ) {
        RBBIDebugPrintf("   --- nullptr ---\n");
        ಹಿಂತಿರುಗಿ;
    }
    RBBIDebugPrintf("state |           i n p u t     s y m b o l s \n");
    RBBIDebugPrintf("      | Acc  LA    Tag");
    ಗಾಗಿ (c=0; c<fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories(); c++) {
        RBBIDebugPrintf(" %2d", c);
    }
    RBBIDebugPrintf("\n");
    RBBIDebugPrintf("      |---------------");
    ಗಾಗಿ (c=0; c<fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories(); c++) {
        RBBIDebugPrintf("---");
    }
    RBBIDebugPrintf("\n");

    ಗಾಗಿ (n=0; n<f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->size(); n++) {
        UnicodeString *ಅಡ್ಡಸಾಲು_String = (UnicodeString *)f_ಸುರಕ್ಷಿತ_ಕೋಷ್ಟಕ->elementAt(n);
        RBBIDebugPrintf("  %3d | " , n);
        RBBIDebugPrintf("%3d %3d %5d ", 0, 0, 0);  // Accepting, LookAhead, Tags
        ಗಾಗಿ (c=0; c<fRB->f_ಗಣ_Builder->get_ಅಂಕೆ_Char_Categories(); c++) {
            RBBIDebugPrintf(" %2d", ಅಡ್ಡಸಾಲು_String->charAt(c));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#ಆದರೆ_ಅಂತ್ಯ



//-----------------------------------------------------------------------------
//
//   printRuleStatusTable    Debug Function.  Dump the common rule status table
//
//-----------------------------------------------------------------------------
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ RBBI_DEBUG
ಶೂನ್ಯ RBBITableBuilder::print_ನಿಯಮ_ಸ್ಥಿತಿ_ಕೋಷ್ಟಕ() {
    int32_t  this_ದಾಖಲೆ = 0;
    int32_t  ಮುಂದಿನ_ದಾಖಲೆ = 0;
    ಪೂರ್ಣಾಂಕ      i;
    UVector  *tbl = fRB->f_ನಿಯಮ_ಸ್ಥಿತಿ_Vals;

    RBBIDebugPrintf("index |  tags \n");
    RBBIDebugPrintf("-------------------\n");

    ಆಗ (ಮುಂದಿನ_ದಾಖಲೆ < tbl->size()) {
        this_ದಾಖಲೆ = ಮುಂದಿನ_ದಾಖಲೆ;
        ಮುಂದಿನ_ದಾಖಲೆ = this_ದಾಖಲೆ + tbl->elementAti(this_ದಾಖಲೆ) + 1;
        RBBIDebugPrintf("%4d   ", this_ದಾಖಲೆ);
        ಗಾಗಿ (i=this_ದಾಖಲೆ+1; i<ಮುಂದಿನ_ದಾಖಲೆ; i++) {
            RBBIDebugPrintf("  %5d", tbl->elementAti(i));
        }
        RBBIDebugPrintf("\n");
    }
    RBBIDebugPrintf("\n\n");
}
#ಆದರೆ_ಅಂತ್ಯ


//-----------------------------------------------------------------------------
//
//   RBBIStateDescriptor     Methods.  This is a very struct-like class
//                           Most access is directly to the fields.
//
//-----------------------------------------------------------------------------

RBBIStateDescriptor::RBBIStateDescriptor(ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_ಒಳಹರಿವು_ಚಿಹ್ನೆ, UError_ಸಂಕೇತ *f_ಸ್ಥಿತಿ) {
    fMarked    = ಸುಳ್ಳು;
    fAccepting = 0;
    fLookAhead = 0;
    f_ಟ್ಯಾಗುಗಳು_Idx   = 0;
    f_ಟ್ಯಾಗ್_Vals   = ಶೂನ್ಯಸೂಚಕ;
    fPositions = ಶೂನ್ಯಸೂಚಕ;
    fDtran     = ಶೂನ್ಯಸೂಚಕ;

    fDtran     = ಹೊಸ UVector32(ಕೊನೆಯ_ಒಳಹರಿವು_ಚಿಹ್ನೆ+1, *f_ಸ್ಥಿತಿ);
    ಆದರೆ (U_FAILURE(*f_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (fDtran == ಶೂನ್ಯಸೂಚಕ) {
        *f_ಸ್ಥಿತಿ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    fDtran->ಗಣ_ಗಾತ್ರ(ಕೊನೆಯ_ಒಳಹರಿವು_ಚಿಹ್ನೆ+1);    // fDtran needs to be pre-sized.
                                           //   It is indexed by input symbols, and will
                                           //   hold  the next state number for each
                                           //   symbol.
}


RBBIStateDescriptor::~RBBIStateDescriptor() {
    ಅಳಿಸು       fPositions;
    ಅಳಿಸು       fDtran;
    ಅಳಿಸು       f_ಟ್ಯಾಗ್_Vals;
    fPositions = ಶೂನ್ಯಸೂಚಕ;
    fDtran     = ಶೂನ್ಯಸೂಚಕ;
    f_ಟ್ಯಾಗ್_Vals   = ಶೂನ್ಯಸೂಚಕ;
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_BREAK_ITERATION */
