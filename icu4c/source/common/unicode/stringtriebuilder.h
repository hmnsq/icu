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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
ಸಂರಚನಾ UHashtable;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
ಗಣನಾ UStringTrieBuildOption {
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
class U_COMMON_API StringTrieBuilder : ಸಾರ್ವಜನಿಕ UObject {
ಸಾರ್ವಜನಿಕ:
#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    ಸ್ಥಿರ int32_t hash_ಗಂಟು(ಧ್ರುವ ಶೂನ್ಯ *ಗಂಟು);
    /** @internal */
    ಸ್ಥಿರ UBool equalNodes(ಧ್ರುವ ಶೂನ್ಯ *ಎಡ, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ);
#endif  /* U_HIDE_INTERNAL_API */

ರಕ್ಷಿತ:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    ಆಭಾಸ ~StringTrieBuilder();

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    ಶೂನ್ಯ createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode);
    /** @internal */
    ಶೂನ್ಯ deleteCompactBuilder();

    /** @internal */
    ಶೂನ್ಯ build(UStringTrieBuildOption buildOption, int32_t elementsLength, UErrorCode &errorCode);

    /** @internal */
    int32_t write_ಗಂಟು(int32_t start, int32_t limit, int32_t unitIndex);
    /** @internal */
    int32_t write_Branch_Sub_ಗಂಟು(int32_t start, int32_t limit, int32_t unitIndex, int32_t length);
#endif  /* U_HIDE_INTERNAL_API */

    class ಗಂಟು_2;

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    ಗಂಟು_2* make_ಗಂಟು(int32_t start,
                   int32_t limit,
                   int32_t unitIndex,
                   UErrorCode& errorCode) U_LIFETIME_BOUND;
    /** @internal */
    ಗಂಟು_2* make_Branch_Sub_ಗಂಟು(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            int32_t length,
                            UErrorCode& errorCode) U_LIFETIME_BOUND;
#endif  /* U_HIDE_INTERNAL_API */

    /** @internal */
    ಆಭಾಸ int32_t getElementStringLength(int32_t i) ಧ್ರುವ = 0;
    /** @internal */
    ಆಭಾಸ char16_t getElementUnit(int32_t i, int32_t unitIndex) ಧ್ರುವ = 0;
    /** @internal */
    ಆಭಾಸ int32_t get_Element_ಮೌಲ್ಯ(int32_t i) ಧ್ರುವ = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    ಆಭಾಸ int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ಧ್ರುವ = 0;

    // Number of different units at unitIndex.
    /** @internal */
    ಆಭಾಸ int32_t ಎಣಿಕೆ_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ಧ್ರುವ = 0;
    /** @internal */
    ಆಭಾಸ int32_t skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t ಎಣಿಕೆ) ಧ್ರುವ = 0;
    /** @internal */
    ಆಭಾಸ int32_t indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ಧ್ರುವ = 0;

    /** @internal */
    ಆಭಾಸ UBool match_Nodes_Can_Have_ಮೌಲ್ಯಗಳು() ಧ್ರುವ = 0;

    /** @internal */
    ಆಭಾಸ int32_t get_Max_Branch_Linear_Sub_ಗಂಟು_Length() ಧ್ರುವ = 0;
    /** @internal */
    ಆಭಾಸ int32_t getMinLinearMatch() ಧ್ರುವ = 0;
    /** @internal */
    ಆಭಾಸ int32_t getMaxLinearMatchLength() ಧ್ರುವ = 0;

#ifndef U_HIDE_INTERNAL_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    ಸ್ಥಿರ ಧ್ರುವ int32_t k_Max_Branch_Linear_Sub_ಗಂಟು_Length=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    ಸ್ಥಿರ ಧ್ರುವ int32_t kMaxSplitBranchLevels=14;

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
    ಗಂಟು_2* register_ಗಂಟು(ಗಂಟು_2* new_ಗಂಟು, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    ಗಂಟು_2* register_Final_ಮೌಲ್ಯ(int32_t ಮೌಲ್ಯ_2, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    class ಗಂಟು_2 : ಸಾರ್ವಜನಿಕ UObject {
    ಸಾರ್ವಜನಿಕ:
        ಗಂಟು_2(int32_t initialHash) : hash(initialHash), offset(0) {}
        ಅನ್ತಃಸ್ಥ int32_t hashCode() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ hash; }
        // Handles node==nullptr.
        ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ int32_t hashCode(ಧ್ರುವ ಗಂಟು_2 *ಗಂಟು) { ಪ್ರತ್ಯಾಗಮ ಗಂಟು==nullptr ? 0 : ಗಂಟು->hashCode(); }
        // Base class operator==() compares the actual class types.
        ಆಭಾಸ bool operator==(ಧ್ರುವ ಗಂಟು_2 &other) ಧ್ರುವ;
        ಅನ್ತಃಸ್ಥ bool operator!=(ಧ್ರುವ ಗಂಟು_2 &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ !operator==(other); }
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
        ಆಭಾಸ int32_t mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ);
        // write() must set the offset to a positive value.
        ಆಭಾಸ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ write_Unless_Inside_ಬಲ_Edge(int32_t first_ಬಲ, int32_t last_ಬಲ,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            ಯದಿ(offset<0 && (offset<last_ಬಲ || first_ಬಲ<offset)) {
                ಲಿಖ(builder);
            }
        }
        ಅನ್ತಃಸ್ಥ int32_t getOffset() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ offset; }
    ರಕ್ಷಿತ:
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
    class Final_ಮೌಲ್ಯ_ಗಂಟು : ಸಾರ್ವಜನಿಕ ಗಂಟು_2 {
    ಸಾರ್ವಜನಿಕ:
        Final_ಮೌಲ್ಯ_ಗಂಟು(int32_t v) : ಗಂಟು_2(0x111111u*37u+v), ಮೌಲ್ಯ_2(v) {}
        ಆಭಾಸ bool operator==(ಧ್ರುವ ಗಂಟು_2 &other) ಧ್ರುವ override;
        ಆಭಾಸ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
    ರಕ್ಷಿತ:
        int32_t ಮೌಲ್ಯ_2;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class ಮೌಲ್ಯ_ಗಂಟು : ಸಾರ್ವಜನಿಕ ಗಂಟು_2 {
    ಸಾರ್ವಜನಿಕ:
        ಮೌಲ್ಯ_ಗಂಟು(int32_t initialHash) : ಗಂಟು_2(initialHash), has_ಮೌಲ್ಯ(false), ಮೌಲ್ಯ_2(0) {}
        ಆಭಾಸ bool operator==(ಧ್ರುವ ಗಂಟು_2 &other) ಧ್ರುವ override;
        ಶೂನ್ಯ set_ಮೌಲ್ಯ(int32_t v) {
            has_ಮೌಲ್ಯ=true;
            ಮೌಲ್ಯ_2=v;
            hash=hash*37u+v;
        }
    ರಕ್ಷಿತ:
        UBool has_ಮೌಲ್ಯ;
        int32_t ಮೌಲ್ಯ_2;
    };

#ifndef U_HIDE_INTERNAL_API
    /** 
     * @internal 
     */
    class Intermediate_ಮೌಲ್ಯ_ಗಂಟು : ಸಾರ್ವಜನಿಕ ಮೌಲ್ಯ_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        Intermediate_ಮೌಲ್ಯ_ಗಂಟು(int32_t v, ಗಂಟು_2 *next_ಗಂಟು)
                : ಮೌಲ್ಯ_ಗಂಟು(0x222222u*37u+hashCode(next_ಗಂಟು)), next(next_ಗಂಟು) { set_ಮೌಲ್ಯ(v); }
        ಆಭಾಸ bool operator==(ಧ್ರುವ ಗಂಟು_2 &other) ಧ್ರುವ override;
        ಆಭಾಸ int32_t mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ) override;
        ಆಭಾಸ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
    ರಕ್ಷಿತ:
        ಗಂಟು_2 *next;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class Linear_Match_ಗಂಟು : ಸಾರ್ವಜನಿಕ ಮೌಲ್ಯ_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        Linear_Match_ಗಂಟು(int32_t len, ಗಂಟು_2 *next_ಗಂಟು)
                : ಮೌಲ್ಯ_ಗಂಟು((0x333333u*37u+len)*37u+hashCode(next_ಗಂಟು)),
                  length(len), next(next_ಗಂಟು) {}
        ಆಭಾಸ bool operator==(ಧ್ರುವ ಗಂಟು_2 &other) ಧ್ರುವ override;
        ಆಭಾಸ int32_t mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ) override;
    ರಕ್ಷಿತ:
        int32_t length;
        ಗಂಟು_2 *next;
    };

#ifndef U_HIDE_INTERNAL_API
    /**
     * @internal 
     */
    class Branch_ಗಂಟು : ಸಾರ್ವಜನಿಕ ಗಂಟು_2 {
    ಸಾರ್ವಜನಿಕ:
        Branch_ಗಂಟು(int32_t initialHash) : ಗಂಟು_2(initialHash) {}
    ರಕ್ಷಿತ:
        int32_t first_Edge_ಸಂಖ್ಯೆ;
    };

    /**
     * @internal 
     */
    class List_Branch_ಗಂಟು : ಸಾರ್ವಜನಿಕ Branch_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        List_Branch_ಗಂಟು() : Branch_ಗಂಟು(0x444444), length(0) {}
        ಆಭಾಸ bool operator==(ಧ್ರುವ ಗಂಟು_2 &other) ಧ್ರುವ override;
        ಆಭಾಸ int32_t mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ) override;
        ಆಭಾಸ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        ಶೂನ್ಯ add(int32_t c, int32_t ಮೌಲ್ಯ_2) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=nullptr;
            ಮೌಲ್ಯಗಳು[length]=ಮೌಲ್ಯ_2;
            ++length;
            hash=(hash*37u+c)*37u+ಮೌಲ್ಯ_2;
        }
        // Adds a unit which leads to another match node.
        ಶೂನ್ಯ add(int32_t c, ಗಂಟು_2 *ಗಂಟು) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=ಗಂಟು;
            ಮೌಲ್ಯಗಳು[length]=0;
            ++length;
            hash=(hash*37u+c)*37u+hashCode(ಗಂಟು);
        }
    ರಕ್ಷಿತ:
        ಗಂಟು_2 *equal[k_Max_Branch_Linear_Sub_ಗಂಟು_Length];  // nullptr means "has final value".
        int32_t length;
        int32_t ಮೌಲ್ಯಗಳು[k_Max_Branch_Linear_Sub_ಗಂಟು_Length];
        char16_t units[k_Max_Branch_Linear_Sub_ಗಂಟು_Length];
    };

    /**
     * @internal 
     */
    class Split_Branch_ಗಂಟು : ಸಾರ್ವಜನಿಕ Branch_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        Split_Branch_ಗಂಟು(char16_t ಮಧ್ಯ_Unit, ಗಂಟು_2 *less_Than_ಗಂಟು, ಗಂಟು_2 *greater_Or_Equal_ಗಂಟು)
                : Branch_ಗಂಟು(((0x555555u*37u+ಮಧ್ಯ_Unit)*37u+
                              hashCode(less_Than_ಗಂಟು))*37u+hashCode(greater_Or_Equal_ಗಂಟು)),
                  unit(ಮಧ್ಯ_Unit), lessThan(less_Than_ಗಂಟು), greaterOrEqual(greater_Or_Equal_ಗಂಟು) {}
        ಆಭಾಸ bool operator==(ಧ್ರುವ ಗಂಟು_2 &other) ಧ್ರುವ override;
        ಆಭಾಸ int32_t mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ) override;
        ಆಭಾಸ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
    ರಕ್ಷಿತ:
        char16_t unit;
        ಗಂಟು_2 *lessThan;
        ಗಂಟು_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_Head_ಗಂಟು : ಸಾರ್ವಜನಿಕ ಮೌಲ್ಯ_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        Branch_Head_ಗಂಟು(int32_t len, ಗಂಟು_2 *sub_ಗಂಟು)
                : ಮೌಲ್ಯ_ಗಂಟು((0x666666u*37u+len)*37u+hashCode(sub_ಗಂಟು)),
                  length(len), next(sub_ಗಂಟು) {}
        ಆಭಾಸ bool operator==(ಧ್ರುವ ಗಂಟು_2 &other) ಧ್ರುವ override;
        ಆಭಾಸ int32_t mark_ಬಲ_Edges_First(int32_t edge_ಸಂಖ್ಯೆ) override;
        ಆಭಾಸ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
    ರಕ್ಷಿತ:
        int32_t length;
        ಗಂಟು_2 *next;  // A branch sub-node.
    };

#endif  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    ಆಭಾಸ ಗಂಟು_2 *create_Linear_Match_ಗಂಟು(int32_t i, int32_t unitIndex, int32_t length,
                                        ಗಂಟು_2 *next_ಗಂಟು) ಧ್ರುವ = 0;

    /** @internal */
    ಆಭಾಸ int32_t ಲಿಖ(int32_t unit) = 0;
    /** @internal */
    ಆಭಾಸ int32_t writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) = 0;
    /** @internal */
    ಆಭಾಸ int32_t write_ಮೌಲ್ಯ_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    ಆಭಾಸ int32_t write_ಮೌಲ್ಯ_And_Type(UBool has_ಮೌಲ್ಯ, int32_t ಮೌಲ್ಯ_2, int32_t ಗಂಟು) = 0;
    /** @internal */
    ಆಭಾಸ int32_t writeDeltaTo(int32_t jumpTarget) = 0;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __STRINGTRIEBUILDER_H__
