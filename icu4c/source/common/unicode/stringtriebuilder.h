// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012,2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  stringtriebuilder.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010dec24
*   created by: Markus W. Scherer
*/

#ifndef __STRINGTRIEBUILDER_H__
#define __STRINGTRIEBUILDER_H__

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି U_SHOW_CPLUSPLUS_API

#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
ସଂରଚନା UHashtable;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
ଗଣନା UStringTrieBuildOption {
    /**
     * Builds a trie quickly.
     * @stable ICU 4.8
     */
    USTRINGTRIE_BUILD_FAST,
    /**
     * Builds a trie more slowly, attempting to generate
     * a shorter but equivalent serialization.
     * This build option also uses more memory.
     *
     * This option can be effective when many integer values are the same
     * and string/byte sequence suffixes can be shared.
     * Runtime speed is not expected to improve.
     * @stable ICU 4.8
     */
    USTRINGTRIE_BUILD_SMALL
};

U_NAMESPACE_BEGIN

/**
 * Base class for string trie builder classes.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API StringTrieBuilder : ସର୍ବସାଧାରଣ UObject {
ସର୍ବସାଧାରଣ:
#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    ସ୍ଥିର int32_t hash_ଗଣ୍ଠି(ଅଚଳ ଶୂନ୍ୟ *ଗଣ୍ଠି);
    /** @internal */
    ସ୍ଥିର UBool equalNodes(ଅଚଳ ଶୂନ୍ୟ *ବାମ, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ);
#endif  /* U_HIDE_INTERNAL_API */

ସୁରକ୍ଷିତ:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    ଆଭାସୀ ~StringTrieBuilder();

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    ଶୂନ୍ୟ createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode);
    /** @internal */
    ଶୂନ୍ୟ deleteCompactBuilder();

    /** @internal */
    ଶୂନ୍ୟ build(UStringTrieBuildOption buildOption, int32_t elementsLength, UErrorCode &errorCode);

    /** @internal */
    int32_t write_ଗଣ୍ଠି(int32_t start, int32_t limit, int32_t unitIndex);
    /** @internal */
    int32_t write_Branch_Sub_ଗଣ୍ଠି(int32_t start, int32_t limit, int32_t unitIndex, int32_t length);
#endif  /* U_HIDE_INTERNAL_API */

    class ଗଣ୍ଠି_2;

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    ଗଣ୍ଠି_2* make_ଗଣ୍ଠି(int32_t start,
                   int32_t limit,
                   int32_t unitIndex,
                   UErrorCode& errorCode) U_LIFETIME_BOUND;
    /** @internal */
    ଗଣ୍ଠି_2* make_Branch_Sub_ଗଣ୍ଠି(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            int32_t length,
                            UErrorCode& errorCode) U_LIFETIME_BOUND;
#endif  /* U_HIDE_INTERNAL_API */

    /** @internal */
    ଆଭାସୀ int32_t getElementStringLength(int32_t i) ଅଚଳ = 0;
    /** @internal */
    ଆଭାସୀ char16_t getElementUnit(int32_t i, int32_t unitIndex) ଅଚଳ = 0;
    /** @internal */
    ଆଭାସୀ int32_t get_Element_ମୂଲ୍ୟ(int32_t i) ଅଚଳ = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    ଆଭାସୀ int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ଅଚଳ = 0;

    // Number of different units at unitIndex.
    /** @internal */
    ଆଭାସୀ int32_t ଗଣନା_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ଅଚଳ = 0;
    /** @internal */
    ଆଭାସୀ int32_t skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t ଗଣନା_2) ଅଚଳ = 0;
    /** @internal */
    ଆଭାସୀ int32_t indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ଅଚଳ = 0;

    /** @internal */
    ଆଭାସୀ UBool match_Nodes_Can_Have_ମୂଲ୍ୟଗୁଡ଼ିକ() ଅଚଳ = 0;

    /** @internal */
    ଆଭାସୀ int32_t get_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length() ଅଚଳ = 0;
    /** @internal */
    ଆଭାସୀ int32_t getMinLinearMatch() ଅଚଳ = 0;
    /** @internal */
    ଆଭାସୀ int32_t getMaxLinearMatchLength() ଅଚଳ = 0;

#ifndef U_HIDE_INTERNAL_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    ସ୍ଥିର ଅଚଳ int32_t k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    ସ୍ଥିର ଅଚଳ int32_t kMaxSplitBranchLevels=14;

    /**
     * Makes sure that there is only one unique node registered that is
     * equivalent to newNode.
     * @param newNode Input node. The builder takes ownership.
     * @param errorCode ICU in/out UErrorCode.
                        Set to U_MEMORY_ALLOCATION_ERROR if it was success but newNode==nullptr.
     * @return newNode if it is the first of its kind, or
     *         an equivalent node if newNode is a duplicate.
     * @internal
     */
    ଗଣ୍ଠି_2* register_ଗଣ୍ଠି(ଗଣ୍ଠି_2* new_ଗଣ୍ଠି, UErrorCode& errorCode) U_LIFETIME_BOUND;
    /**
     * Makes sure that there is only one unique FinalValueNode registered
     * with this value.
     * Avoids creating a node if the value is a duplicate.
     * @param value A final value.
     * @param errorCode ICU in/out UErrorCode.
                        Set to U_MEMORY_ALLOCATION_ERROR if it was success but newNode==nullptr.
     * @return A FinalValueNode with the given value.
     * @internal
     */
    ଗଣ୍ଠି_2* register_Final_ମୂଲ୍ୟ(int32_t ମୂଲ୍ୟ_2, UErrorCode& errorCode) U_LIFETIME_BOUND;
#endif  /* U_HIDE_INTERNAL_API */

    /*
     * C++ note:
     * registerNode() and registerFinalValue() take ownership of their input nodes,
     * and only return owned nodes.
     * If they see a failure UErrorCode, they will delete the input node.
     * If they get a nullptr pointer, they will record a U_MEMORY_ALLOCATION_ERROR.
     * If there is a failure, they return nullptr.
     *
     * nullptr Node pointers can be safely passed into other Nodes because
     * they call the static Node::hashCode() which checks for a nullptr pointer first.
     *
     * Therefore, as long as builder functions register a new node,
     * they need to check for failures only before explicitly dereferencing
     * a Node pointer, or before setting a new UErrorCode.
     */

    // Hash set of nodes, maps from nodes to integer 1.
    /** @internal */
    UHashtable *nodes;

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal
     * \cond
     */
    class ଗଣ୍ଠି_2 : ସର୍ବସାଧାରଣ UObject {
    ସର୍ବସାଧାରଣ:
        ଗଣ୍ଠି_2(int32_t initialHash) : hash(initialHash), offset(0) {}
        ଧାଡ଼ିଭିତରେ int32_t hashCode() ଅଚଳ { ଫେରନ୍ତୁ hash; }
        // Handles node==nullptr.
        ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t hashCode(ଅଚଳ ଗଣ୍ଠି_2 *ଗଣ୍ଠି) { ଫେରନ୍ତୁ ଗଣ୍ଠି==nullptr ? 0 : ଗଣ୍ଠି->hashCode(); }
        // Base class operator==() compares the actual class types.
        ଆଭାସୀ bool operator==(ଅଚଳ ଗଣ୍ଠି_2 &other) ଅଚଳ;
        ଧାଡ଼ିଭିତରେ bool operator!=(ଅଚଳ ଗଣ୍ଠି_2 &other) ଅଚଳ { ଫେରନ୍ତୁ !operator==(other); }
        /**
         * Traverses the Node graph and numbers branch edges, with rightmost edges first.
         * This is to avoid writing a duplicate node twice.
         *
         * Branch nodes in this trie data structure are not symmetric.
         * Most branch edges "jump" to other nodes but the rightmost branch edges
         * just continue without a jump.
         * Therefore, write() must write the rightmost branch edge last
         * (trie units are written backwards), and must write it at that point even if
         * it is a duplicate of a node previously written elsewhere.
         *
         * This function visits and marks right branch edges first.
         * Edges are numbered with increasingly negative values because we share the
         * offset field which gets positive values when nodes are written.
         * A branch edge also remembers the first number for any of its edges.
         *
         * When a further-left branch edge has a number in the range of the rightmost
         * edge's numbers, then it will be written as part of the required right edge
         * and we can avoid writing it first.
         *
         * After root.markRightEdgesFirst(-1) the offsets of all nodes are negative
         * edge numbers.
         *
         * @param edgeNumber The first edge number for this node and its sub-nodes.
         * @return An edge number that is at least the maximum-negative
         *         of the input edge number and the numbers of this node and all of its sub-nodes.
         */
        ଆଭାସୀ int32_t mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା);
        // write() must set the offset to a positive value.
        ଆଭାସୀ ଶୂନ୍ୟ ଲିଖ(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ write_Unless_Inside_ଡାହାଣ_Edge(int32_t first_ଡାହାଣ, int32_t last_ଡାହାଣ,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            ଯଦି(offset<0 && (offset<last_ଡାହାଣ || first_ଡାହାଣ<offset)) {
                ଲିଖ(builder);
            }
        }
        ଧାଡ଼ିଭିତରେ int32_t getOffset() ଅଚଳ { ଫେରନ୍ତୁ offset; }
    ସୁରକ୍ଷିତ:
        int32_t hash;
        int32_t offset;
    };

#ifndef U_HIDE_INTERNAL_API
    // This class should not be overridden because
    // registerFinalValue() compares a stack-allocated FinalValueNode
    // (stack-allocated so that we don't unnecessarily create lots of duplicate nodes)
    // with the input node, and the
    // !Node::operator==(other) used inside FinalValueNode::operator==(other)
    // will be false if the typeid's are different.
    /** @internal */
    class Final_ମୂଲ୍ୟ_ଗଣ୍ଠି : ସର୍ବସାଧାରଣ ଗଣ୍ଠି_2 {
    ସର୍ବସାଧାରଣ:
        Final_ମୂଲ୍ୟ_ଗଣ୍ଠି(int32_t v) : ଗଣ୍ଠି_2(0x111111u*37u+v), ମୂଲ୍ୟ_2(v) {}
        ଆଭାସୀ bool operator==(ଅଚଳ ଗଣ୍ଠି_2 &other) ଅଚଳ override;
        ଆଭାସୀ ଶୂନ୍ୟ ଲିଖ(StringTrieBuilder &builder) override;
    ସୁରକ୍ଷିତ:
        int32_t ମୂଲ୍ୟ_2;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class ମୂଲ୍ୟ_ଗଣ୍ଠି : ସର୍ବସାଧାରଣ ଗଣ୍ଠି_2 {
    ସର୍ବସାଧାରଣ:
        ମୂଲ୍ୟ_ଗଣ୍ଠି(int32_t initialHash) : ଗଣ୍ଠି_2(initialHash), has_ମୂଲ୍ୟ(false), ମୂଲ୍ୟ_2(0) {}
        ଆଭାସୀ bool operator==(ଅଚଳ ଗଣ୍ଠି_2 &other) ଅଚଳ override;
        ଶୂନ୍ୟ set_ମୂଲ୍ୟ(int32_t v) {
            has_ମୂଲ୍ୟ=true;
            ମୂଲ୍ୟ_2=v;
            hash=hash*37u+v;
        }
    ସୁରକ୍ଷିତ:
        UBool has_ମୂଲ୍ୟ;
        int32_t ମୂଲ୍ୟ_2;
    };

#ifndef U_HIDE_INTERNAL_API
    /** 
     * @internal 
     */
    class Intermediate_ମୂଲ୍ୟ_ଗଣ୍ଠି : ସର୍ବସାଧାରଣ ମୂଲ୍ୟ_ଗଣ୍ଠି {
    ସର୍ବସାଧାରଣ:
        Intermediate_ମୂଲ୍ୟ_ଗଣ୍ଠି(int32_t v, ଗଣ୍ଠି_2 *next_ଗଣ୍ଠି)
                : ମୂଲ୍ୟ_ଗଣ୍ଠି(0x222222u*37u+hashCode(next_ଗଣ୍ଠି)), next(next_ଗଣ୍ଠି) { set_ମୂଲ୍ୟ(v); }
        ଆଭାସୀ bool operator==(ଅଚଳ ଗଣ୍ଠି_2 &other) ଅଚଳ override;
        ଆଭାସୀ int32_t mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା) override;
        ଆଭାସୀ ଶୂନ୍ୟ ଲିଖ(StringTrieBuilder &builder) override;
    ସୁରକ୍ଷିତ:
        ଗଣ୍ଠି_2 *next;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class Linear_Match_ଗଣ୍ଠି : ସର୍ବସାଧାରଣ ମୂଲ୍ୟ_ଗଣ୍ଠି {
    ସର୍ବସାଧାରଣ:
        Linear_Match_ଗଣ୍ଠି(int32_t len, ଗଣ୍ଠି_2 *next_ଗଣ୍ଠି)
                : ମୂଲ୍ୟ_ଗଣ୍ଠି((0x333333u*37u+len)*37u+hashCode(next_ଗଣ୍ଠି)),
                  length(len), next(next_ଗଣ୍ଠି) {}
        ଆଭାସୀ bool operator==(ଅଚଳ ଗଣ୍ଠି_2 &other) ଅଚଳ override;
        ଆଭାସୀ int32_t mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା) override;
    ସୁରକ୍ଷିତ:
        int32_t length;
        ଗଣ୍ଠି_2 *next;
    };

#ifndef U_HIDE_INTERNAL_API
    /**
     * @internal 
     */
    class Branch_ଗଣ୍ଠି : ସର୍ବସାଧାରଣ ଗଣ୍ଠି_2 {
    ସର୍ବସାଧାରଣ:
        Branch_ଗଣ୍ଠି(int32_t initialHash) : ଗଣ୍ଠି_2(initialHash) {}
    ସୁରକ୍ଷିତ:
        int32_t first_Edge_ସଂଖ୍ୟା;
    };

    /**
     * @internal 
     */
    class List_Branch_ଗଣ୍ଠି : ସର୍ବସାଧାରଣ Branch_ଗଣ୍ଠି {
    ସର୍ବସାଧାରଣ:
        List_Branch_ଗଣ୍ଠି() : Branch_ଗଣ୍ଠି(0x444444), length(0) {}
        ଆଭାସୀ bool operator==(ଅଚଳ ଗଣ୍ଠି_2 &other) ଅଚଳ override;
        ଆଭାସୀ int32_t mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା) override;
        ଆଭାସୀ ଶୂନ୍ୟ ଲିଖ(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        ଶୂନ୍ୟ add(int32_t c, int32_t ମୂଲ୍ୟ_2) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=nullptr;
            ମୂଲ୍ୟଗୁଡ଼ିକ[length]=ମୂଲ୍ୟ_2;
            ++length;
            hash=(hash*37u+c)*37u+ମୂଲ୍ୟ_2;
        }
        // Adds a unit which leads to another match node.
        ଶୂନ୍ୟ add(int32_t c, ଗଣ୍ଠି_2 *ଗଣ୍ଠି) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=ଗଣ୍ଠି;
            ମୂଲ୍ୟଗୁଡ଼ିକ[length]=0;
            ++length;
            hash=(hash*37u+c)*37u+hashCode(ଗଣ୍ଠି);
        }
    ସୁରକ୍ଷିତ:
        ଗଣ୍ଠି_2 *equal[k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length];  // nullptr means "has final value".
        int32_t length;
        int32_t ମୂଲ୍ୟଗୁଡ଼ିକ[k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length];
        char16_t units[k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length];
    };

    /**
     * @internal 
     */
    class Split_Branch_ଗଣ୍ଠି : ସର୍ବସାଧାରଣ Branch_ଗଣ୍ଠି {
    ସର୍ବସାଧାରଣ:
        Split_Branch_ଗଣ୍ଠି(char16_t ମଧ୍ୟ_Unit, ଗଣ୍ଠି_2 *less_Than_ଗଣ୍ଠି, ଗଣ୍ଠି_2 *greater_Or_Equal_ଗଣ୍ଠି)
                : Branch_ଗଣ୍ଠି(((0x555555u*37u+ମଧ୍ୟ_Unit)*37u+
                              hashCode(less_Than_ଗଣ୍ଠି))*37u+hashCode(greater_Or_Equal_ଗଣ୍ଠି)),
                  unit(ମଧ୍ୟ_Unit), lessThan(less_Than_ଗଣ୍ଠି), greaterOrEqual(greater_Or_Equal_ଗଣ୍ଠି) {}
        ଆଭାସୀ bool operator==(ଅଚଳ ଗଣ୍ଠି_2 &other) ଅଚଳ override;
        ଆଭାସୀ int32_t mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା) override;
        ଆଭାସୀ ଶୂନ୍ୟ ଲିଖ(StringTrieBuilder &builder) override;
    ସୁରକ୍ଷିତ:
        char16_t unit;
        ଗଣ୍ଠି_2 *lessThan;
        ଗଣ୍ଠି_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_Head_ଗଣ୍ଠି : ସର୍ବସାଧାରଣ ମୂଲ୍ୟ_ଗଣ୍ଠି {
    ସର୍ବସାଧାରଣ:
        Branch_Head_ଗଣ୍ଠି(int32_t len, ଗଣ୍ଠି_2 *sub_ଗଣ୍ଠି)
                : ମୂଲ୍ୟ_ଗଣ୍ଠି((0x666666u*37u+len)*37u+hashCode(sub_ଗଣ୍ଠି)),
                  length(len), next(sub_ଗଣ୍ଠି) {}
        ଆଭାସୀ bool operator==(ଅଚଳ ଗଣ୍ଠି_2 &other) ଅଚଳ override;
        ଆଭାସୀ int32_t mark_ଡାହାଣ_Edges_First(int32_t edge_ସଂଖ୍ୟା) override;
        ଆଭାସୀ ଶୂନ୍ୟ ଲିଖ(StringTrieBuilder &builder) override;
    ସୁରକ୍ଷିତ:
        int32_t length;
        ଗଣ୍ଠି_2 *next;  // A branch sub-node.
    };

#endif  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    ଆଭାସୀ ଗଣ୍ଠି_2 *create_Linear_Match_ଗଣ୍ଠି(int32_t i, int32_t unitIndex, int32_t length,
                                        ଗଣ୍ଠି_2 *next_ଗଣ୍ଠି) ଅଚଳ = 0;

    /** @internal */
    ଆଭାସୀ int32_t ଲିଖ(int32_t unit) = 0;
    /** @internal */
    ଆଭାସୀ int32_t writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) = 0;
    /** @internal */
    ଆଭାସୀ int32_t write_ମୂଲ୍ୟ_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    ଆଭାସୀ int32_t write_ମୂଲ୍ୟ_And_Type(UBool has_ମୂଲ୍ୟ, int32_t ମୂଲ୍ୟ_2, int32_t ଗଣ୍ଠି) = 0;
    /** @internal */
    ଆଭାସୀ int32_t writeDeltaTo(int32_t jumpTarget) = 0;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __STRINGTRIEBUILDER_H__
