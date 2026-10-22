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

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি U_SHOW_CPLUSPLUS_API

#অন্তর্ভাৱ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
সংরচনা UHashtable;
প্রকারনির্দেশ সংরচনা UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
গণনা UStringTrieBuildOption {
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
class U_COMMON_API StringTrieBuilder : সার্ৱজনিক UObject {
সার্ৱজনিক:
#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    স্থির int32_t hash_গ্ৰন্থি(ধ্রুৱ শূন্য *গ্ৰন্থি);
    /** @internal */
    স্থির UBool equalNodes(ধ্রুৱ শূন্য *বাম, ধ্রুৱ শূন্য *ডান);
#endif  /* U_HIDE_INTERNAL_API */

রক্ষিত:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    আভাস ~StringTrieBuilder();

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    শূন্য createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode);
    /** @internal */
    শূন্য deleteCompactBuilder();

    /** @internal */
    শূন্য build(UStringTrieBuildOption buildOption, int32_t elementsLength, UErrorCode &errorCode);

    /** @internal */
    int32_t write_গ্ৰন্থি(int32_t start, int32_t limit, int32_t unitIndex);
    /** @internal */
    int32_t write_Branch_Sub_গ্ৰন্থি(int32_t start, int32_t limit, int32_t unitIndex, int32_t length);
#endif  /* U_HIDE_INTERNAL_API */

    class গ্ৰন্থি_2;

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    গ্ৰন্থি_2* make_গ্ৰন্থি(int32_t start,
                   int32_t limit,
                   int32_t unitIndex,
                   UErrorCode& errorCode) U_LIFETIME_BOUND;
    /** @internal */
    গ্ৰন্থি_2* make_Branch_Sub_গ্ৰন্থি(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            int32_t length,
                            UErrorCode& errorCode) U_LIFETIME_BOUND;
#endif  /* U_HIDE_INTERNAL_API */

    /** @internal */
    আভাস int32_t getElementStringLength(int32_t i) ধ্রুৱ = 0;
    /** @internal */
    আভাস char16_t getElementUnit(int32_t i, int32_t unitIndex) ধ্রুৱ = 0;
    /** @internal */
    আভাস int32_t get_Element_মান(int32_t i) ধ্রুৱ = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    আভাস int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ধ্রুৱ = 0;

    // Number of different units at unitIndex.
    /** @internal */
    আভাস int32_t গুনতি_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ধ্রুৱ = 0;
    /** @internal */
    আভাস int32_t skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t গুনতি) ধ্রুৱ = 0;
    /** @internal */
    আভাস int32_t indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ধ্রুৱ = 0;

    /** @internal */
    আভাস UBool match_Nodes_Can_Have_মানসমূহ() ধ্রুৱ = 0;

    /** @internal */
    আভাস int32_t get_Max_Branch_Linear_Sub_গ্ৰন্থি_Length() ধ্রুৱ = 0;
    /** @internal */
    আভাস int32_t getMinLinearMatch() ধ্রুৱ = 0;
    /** @internal */
    আভাস int32_t getMaxLinearMatchLength() ধ্রুৱ = 0;

#ifndef U_HIDE_INTERNAL_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    স্থির ধ্রুৱ int32_t k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    স্থির ধ্রুৱ int32_t kMaxSplitBranchLevels=14;

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
    গ্ৰন্থি_2* register_গ্ৰন্থি(গ্ৰন্থি_2* new_গ্ৰন্থি, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    গ্ৰন্থি_2* register_Final_মান(int32_t মান_2, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    class গ্ৰন্থি_2 : সার্ৱজনিক UObject {
    সার্ৱজনিক:
        গ্ৰন্থি_2(int32_t initialHash) : hash(initialHash), offset(0) {}
        অন্তঃস্থ int32_t hashCode() ধ্রুৱ { ঘূৰাই hash; }
        // Handles node==nullptr.
        স্থির অন্তঃস্থ int32_t hashCode(ধ্রুৱ গ্ৰন্থি_2 *গ্ৰন্থি) { ঘূৰাই গ্ৰন্থি==nullptr ? 0 : গ্ৰন্থি->hashCode(); }
        // Base class operator==() compares the actual class types.
        আভাস bool operator==(ধ্রুৱ গ্ৰন্থি_2 &other) ধ্রুৱ;
        অন্তঃস্থ bool operator!=(ধ্রুৱ গ্ৰন্থি_2 &other) ধ্রুৱ { ঘূৰাই !operator==(other); }
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
        আভাস int32_t mark_ডান_Edges_First(int32_t edge_সংখ্যা);
        // write() must set the offset to a positive value.
        আভাস শূন্য লিখ(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        অন্তঃস্থ শূন্য write_Unless_Inside_ডান_Edge(int32_t first_ডান, int32_t last_ডান,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            যদি(offset<0 && (offset<last_ডান || first_ডান<offset)) {
                লিখ(builder);
            }
        }
        অন্তঃস্থ int32_t getOffset() ধ্রুৱ { ঘূৰাই offset; }
    রক্ষিত:
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
    class Final_মান_গ্ৰন্থি : সার্ৱজনিক গ্ৰন্থি_2 {
    সার্ৱজনিক:
        Final_মান_গ্ৰন্থি(int32_t v) : গ্ৰন্থি_2(0x111111u*37u+v), মান_2(v) {}
        আভাস bool operator==(ধ্রুৱ গ্ৰন্থি_2 &other) ধ্রুৱ override;
        আভাস শূন্য লিখ(StringTrieBuilder &builder) override;
    রক্ষিত:
        int32_t মান_2;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class মান_গ্ৰন্থি : সার্ৱজনিক গ্ৰন্থি_2 {
    সার্ৱজনিক:
        মান_গ্ৰন্থি(int32_t initialHash) : গ্ৰন্থি_2(initialHash), has_মান(false), মান_2(0) {}
        আভাস bool operator==(ধ্রুৱ গ্ৰন্থি_2 &other) ধ্রুৱ override;
        শূন্য set_মান(int32_t v) {
            has_মান=true;
            মান_2=v;
            hash=hash*37u+v;
        }
    রক্ষিত:
        UBool has_মান;
        int32_t মান_2;
    };

#ifndef U_HIDE_INTERNAL_API
    /** 
     * @internal 
     */
    class Intermediate_মান_গ্ৰন্থি : সার্ৱজনিক মান_গ্ৰন্থি {
    সার্ৱজনিক:
        Intermediate_মান_গ্ৰন্থি(int32_t v, গ্ৰন্থি_2 *next_গ্ৰন্থি)
                : মান_গ্ৰন্থি(0x222222u*37u+hashCode(next_গ্ৰন্থি)), next(next_গ্ৰন্থি) { set_মান(v); }
        আভাস bool operator==(ধ্রুৱ গ্ৰন্থি_2 &other) ধ্রুৱ override;
        আভাস int32_t mark_ডান_Edges_First(int32_t edge_সংখ্যা) override;
        আভাস শূন্য লিখ(StringTrieBuilder &builder) override;
    রক্ষিত:
        গ্ৰন্থি_2 *next;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class Linear_Match_গ্ৰন্থি : সার্ৱজনিক মান_গ্ৰন্থি {
    সার্ৱজনিক:
        Linear_Match_গ্ৰন্থি(int32_t len, গ্ৰন্থি_2 *next_গ্ৰন্থি)
                : মান_গ্ৰন্থি((0x333333u*37u+len)*37u+hashCode(next_গ্ৰন্থি)),
                  length(len), next(next_গ্ৰন্থি) {}
        আভাস bool operator==(ধ্রুৱ গ্ৰন্থি_2 &other) ধ্রুৱ override;
        আভাস int32_t mark_ডান_Edges_First(int32_t edge_সংখ্যা) override;
    রক্ষিত:
        int32_t length;
        গ্ৰন্থি_2 *next;
    };

#ifndef U_HIDE_INTERNAL_API
    /**
     * @internal 
     */
    class Branch_গ্ৰন্থি : সার্ৱজনিক গ্ৰন্থি_2 {
    সার্ৱজনিক:
        Branch_গ্ৰন্থি(int32_t initialHash) : গ্ৰন্থি_2(initialHash) {}
    রক্ষিত:
        int32_t first_Edge_সংখ্যা;
    };

    /**
     * @internal 
     */
    class List_Branch_গ্ৰন্থি : সার্ৱজনিক Branch_গ্ৰন্থি {
    সার্ৱজনিক:
        List_Branch_গ্ৰন্থি() : Branch_গ্ৰন্থি(0x444444), length(0) {}
        আভাস bool operator==(ধ্রুৱ গ্ৰন্থি_2 &other) ধ্রুৱ override;
        আভাস int32_t mark_ডান_Edges_First(int32_t edge_সংখ্যা) override;
        আভাস শূন্য লিখ(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        শূন্য add(int32_t c, int32_t মান_2) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=nullptr;
            মানসমূহ[length]=মান_2;
            ++length;
            hash=(hash*37u+c)*37u+মান_2;
        }
        // Adds a unit which leads to another match node.
        শূন্য add(int32_t c, গ্ৰন্থি_2 *গ্ৰন্থি) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=গ্ৰন্থি;
            মানসমূহ[length]=0;
            ++length;
            hash=(hash*37u+c)*37u+hashCode(গ্ৰন্থি);
        }
    রক্ষিত:
        গ্ৰন্থি_2 *equal[k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length];  // nullptr means "has final value".
        int32_t length;
        int32_t মানসমূহ[k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length];
        char16_t units[k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length];
    };

    /**
     * @internal 
     */
    class Split_Branch_গ্ৰন্থি : সার্ৱজনিক Branch_গ্ৰন্থি {
    সার্ৱজনিক:
        Split_Branch_গ্ৰন্থি(char16_t মধ্য_Unit, গ্ৰন্থি_2 *less_Than_গ্ৰন্থি, গ্ৰন্থি_2 *greater_Or_Equal_গ্ৰন্থি)
                : Branch_গ্ৰন্থি(((0x555555u*37u+মধ্য_Unit)*37u+
                              hashCode(less_Than_গ্ৰন্থি))*37u+hashCode(greater_Or_Equal_গ্ৰন্থি)),
                  unit(মধ্য_Unit), lessThan(less_Than_গ্ৰন্থি), greaterOrEqual(greater_Or_Equal_গ্ৰন্থি) {}
        আভাস bool operator==(ধ্রুৱ গ্ৰন্থি_2 &other) ধ্রুৱ override;
        আভাস int32_t mark_ডান_Edges_First(int32_t edge_সংখ্যা) override;
        আভাস শূন্য লিখ(StringTrieBuilder &builder) override;
    রক্ষিত:
        char16_t unit;
        গ্ৰন্থি_2 *lessThan;
        গ্ৰন্থি_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_Head_গ্ৰন্থি : সার্ৱজনিক মান_গ্ৰন্থি {
    সার্ৱজনিক:
        Branch_Head_গ্ৰন্থি(int32_t len, গ্ৰন্থি_2 *sub_গ্ৰন্থি)
                : মান_গ্ৰন্থি((0x666666u*37u+len)*37u+hashCode(sub_গ্ৰন্থি)),
                  length(len), next(sub_গ্ৰন্থি) {}
        আভাস bool operator==(ধ্রুৱ গ্ৰন্থি_2 &other) ধ্রুৱ override;
        আভাস int32_t mark_ডান_Edges_First(int32_t edge_সংখ্যা) override;
        আভাস শূন্য লিখ(StringTrieBuilder &builder) override;
    রক্ষিত:
        int32_t length;
        গ্ৰন্থি_2 *next;  // A branch sub-node.
    };

#endif  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    আভাস গ্ৰন্থি_2 *create_Linear_Match_গ্ৰন্থি(int32_t i, int32_t unitIndex, int32_t length,
                                        গ্ৰন্থি_2 *next_গ্ৰন্থি) ধ্রুৱ = 0;

    /** @internal */
    আভাস int32_t লিখ(int32_t unit) = 0;
    /** @internal */
    আভাস int32_t writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) = 0;
    /** @internal */
    আভাস int32_t write_মান_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    আভাস int32_t write_মান_And_Type(UBool has_মান, int32_t মান_2, int32_t গ্ৰন্থি) = 0;
    /** @internal */
    আভাস int32_t writeDeltaTo(int32_t jumpTarget) = 0;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __STRINGTRIEBUILDER_H__
