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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __STRINGTRIEBUILDER_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __STRINGTRIEBUILDER_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
ರಚನೆ UHashtable;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
ಎಣಿಕೆ UStringTrieBuildOption {
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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_ಆಂತರಿಕ_API
    /** @internal */
    ಸ್ಥಿರ int32_t hash_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಗಂಟು);
    /** @internal */
    ಸ್ಥಿರ UBool equalNodes(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ_2, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ_2);
#ಆದರೆ_ಅಂತ್ಯ  /* U_HIDE_INTERNAL_API */

ಸಂರಕ್ಷಿತ:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    ವಾಸ್ತವಿಕ ~StringTrieBuilder();

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_ಆಂತರಿಕ_API
    /** @internal */
    ಶೂನ್ಯ createCompactBuilder(int32_t ಗಾತ್ರ_Guess, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    /** @internal */
    ಶೂನ್ಯ deleteCompactBuilder();

    /** @internal */
    ಶೂನ್ಯ build(UStringTrieBuildOption buildOption, int32_t elements_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    /** @internal */
    int32_t write_ಗಂಟು(int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, int32_t unit_ಸೂಚ್ಯಂಕ);
    /** @internal */
    int32_t write_Branch_Sub_ಗಂಟು(int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, int32_t unit_ಸೂಚ್ಯಂಕ, int32_t ಉದ್ದಳತೆ);
#ಆದರೆ_ಅಂತ್ಯ  /* U_HIDE_INTERNAL_API */

    class ಗಂಟು_2;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_ಆಂತರಿಕ_API
    /** @internal */
    ಗಂಟು_2* make_ಗಂಟು(int32_t ಆರಂಭ,
                   int32_t ಎಲ್ಲೆ,
                   int32_t unit_ಸೂಚ್ಯಂಕ,
                   UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND;
    /** @internal */
    ಗಂಟು_2* make_Branch_Sub_ಗಂಟು(int32_t ಆರಂಭ,
                            int32_t ಎಲ್ಲೆ,
                            int32_t unit_ಸೂಚ್ಯಂಕ,
                            int32_t ಉದ್ದಳತೆ,
                            UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND;
#ಆದರೆ_ಅಂತ್ಯ  /* U_HIDE_INTERNAL_API */

    /** @internal */
    ವಾಸ್ತವಿಕ int32_t get_Element_String_ಉದ್ದಳತೆ(int32_t i) ಸ್ಥಿರಾಂಕ = 0;
    /** @internal */
    ವಾಸ್ತವಿಕ char16_t getElementUnit(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ = 0;
    /** @internal */
    ವಾಸ್ತವಿಕ int32_t get_Element_ಮೌಲ್ಯ(int32_t i) ಸ್ಥಿರಾಂಕ = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    ವಾಸ್ತವಿಕ int32_t get_ಎಲ್ಲೆ_ದ_ರೇಖೀಯ_ಹೊಂದಿಕೆ(int32_t ಮೊದಲ, int32_t ಕೊನೆಯ, int32_t unit_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ = 0;

    // Number of different units at unitIndex.
    /** @internal */
    ವಾಸ್ತವಿಕ int32_t ಎಣಿಕೆ_Element_Units(int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, int32_t unit_ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ = 0;
    /** @internal */
    ವಾಸ್ತವಿಕ int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, int32_t ಎಣಿಕೆ_2) ಸ್ಥಿರಾಂಕ = 0;
    /** @internal */
    ವಾಸ್ತವಿಕ int32_t ಸೂಚ್ಯಂಕ_ದ_Element_With_ಮುಂದಿನ_Unit(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, char16_t unit) ಸ್ಥಿರಾಂಕ = 0;

    /** @internal */
    ವಾಸ್ತವಿಕ UBool ಹೊಂದಿಕೆ_Nodes_Can_Have_ಮೌಲ್ಯಗಳು() ಸ್ಥಿರಾಂಕ = 0;

    /** @internal */
    ವಾಸ್ತವಿಕ int32_t get_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ = 0;
    /** @internal */
    ವಾಸ್ತವಿಕ int32_t get_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ() ಸ್ಥಿರಾಂಕ = 0;
    /** @internal */
    ವಾಸ್ತವಿಕ int32_t get_Max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_ಆಂತರಿಕ_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Max_ವಿಭಜಿಸು_Branch_ಮಟ್ಟಗಳು=14;

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
    ಗಂಟು_2* ರಿಜಿಸ್ಟರ್_ಗಂಟು(ಗಂಟು_2* new_ಗಂಟು, UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND;
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
    ಗಂಟು_2* ರಿಜಿಸ್ಟರ್_Final_ಮೌಲ್ಯ(int32_t ಮೌಲ್ಯ_2, UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND;
#ಆದರೆ_ಅಂತ್ಯ  /* U_HIDE_INTERNAL_API */

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
        ಗಂಟು_2(int32_t initialHash) : hash(initialHash), ಸ್ಥಾನಾಂತರ(0) {}
        ಸಾಲೊಳಗೆ int32_t hash_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ hash; }
        // Handles node==nullptr.
        ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t hash_ಸಂಕೇತ(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 *ಗಂಟು) { ಹಿಂತಿರುಗಿ ಗಂಟು==nullptr ? 0 : ಗಂಟು->hash_ಸಂಕೇತ(); }
        // Base class operator==() compares the actual class types.
        ವಾಸ್ತವಿಕ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 &other) ಸ್ಥಿರಾಂಕ;
        ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other); }
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
        ವಾಸ್ತವಿಕ int32_t mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ);
        // write() must set the offset to a positive value.
        ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        ಸಾಲೊಳಗೆ ಶೂನ್ಯ write_Unless_Inside_ಬಲ_ಅಂಚು(int32_t ಮೊದಲ_ಬಲ, int32_t ಕೊನೆಯ_ಬಲ,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            ಆದರೆ(ಸ್ಥಾನಾಂತರ<0 && (ಸ್ಥಾನಾಂತರ<ಕೊನೆಯ_ಬಲ || ಮೊದಲ_ಬಲ<ಸ್ಥಾನಾಂತರ)) {
                ಲಿಖ(builder);
            }
        }
        ಸಾಲೊಳಗೆ int32_t get_ಸ್ಥಾನಾಂತರ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ; }
    ಸಂರಕ್ಷಿತ:
        int32_t hash;
        int32_t ಸ್ಥಾನಾಂತರ;
    };

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_ಆಂತರಿಕ_API
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
        ವಾಸ್ತವಿಕ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 &other) ಸ್ಥಿರಾಂಕ override;
        ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
    ಸಂರಕ್ಷಿತ:
        int32_t ಮೌಲ್ಯ_2;
    };
#ಆದರೆ_ಅಂತ್ಯ  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class ಮೌಲ್ಯ_ಗಂಟು : ಸಾರ್ವಜನಿಕ ಗಂಟು_2 {
    ಸಾರ್ವಜನಿಕ:
        ಮೌಲ್ಯ_ಗಂಟು(int32_t initialHash) : ಗಂಟು_2(initialHash), has_ಮೌಲ್ಯ(false), ಮೌಲ್ಯ_2(0) {}
        ವಾಸ್ತವಿಕ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 &other) ಸ್ಥಿರಾಂಕ override;
        ಶೂನ್ಯ ಗಣ_ಮೌಲ್ಯ(int32_t v) {
            has_ಮೌಲ್ಯ=true;
            ಮೌಲ್ಯ_2=v;
            hash=hash*37u+v;
        }
    ಸಂರಕ್ಷಿತ:
        UBool has_ಮೌಲ್ಯ;
        int32_t ಮೌಲ್ಯ_2;
    };

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_ಆಂತರಿಕ_API
    /** 
     * @internal 
     */
    class Intermediate_ಮೌಲ್ಯ_ಗಂಟು : ಸಾರ್ವಜನಿಕ ಮೌಲ್ಯ_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        Intermediate_ಮೌಲ್ಯ_ಗಂಟು(int32_t v, ಗಂಟು_2 *ಮುಂದಿನ_ಗಂಟು)
                : ಮೌಲ್ಯ_ಗಂಟು(0x222222u*37u+hash_ಸಂಕೇತ(ಮುಂದಿನ_ಗಂಟು)), ಮುಂದಿನ(ಮುಂದಿನ_ಗಂಟು) { ಗಣ_ಮೌಲ್ಯ(v); }
        ವಾಸ್ತವಿಕ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 &other) ಸ್ಥಿರಾಂಕ override;
        ವಾಸ್ತವಿಕ int32_t mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ) override;
        ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
    ಸಂರಕ್ಷಿತ:
        ಗಂಟು_2 *ಮುಂದಿನ;
    };
#ಆದರೆ_ಅಂತ್ಯ  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು : ಸಾರ್ವಜನಿಕ ಮೌಲ್ಯ_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು(int32_t len, ಗಂಟು_2 *ಮುಂದಿನ_ಗಂಟು)
                : ಮೌಲ್ಯ_ಗಂಟು((0x333333u*37u+len)*37u+hash_ಸಂಕೇತ(ಮುಂದಿನ_ಗಂಟು)),
                  ಉದ್ದಳತೆ(len), ಮುಂದಿನ(ಮುಂದಿನ_ಗಂಟು) {}
        ವಾಸ್ತವಿಕ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 &other) ಸ್ಥಿರಾಂಕ override;
        ವಾಸ್ತವಿಕ int32_t mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ) override;
    ಸಂರಕ್ಷಿತ:
        int32_t ಉದ್ದಳತೆ;
        ಗಂಟು_2 *ಮುಂದಿನ;
    };

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_ಆಂತರಿಕ_API
    /**
     * @internal 
     */
    class Branch_ಗಂಟು : ಸಾರ್ವಜನಿಕ ಗಂಟು_2 {
    ಸಾರ್ವಜನಿಕ:
        Branch_ಗಂಟು(int32_t initialHash) : ಗಂಟು_2(initialHash) {}
    ಸಂರಕ್ಷಿತ:
        int32_t ಮೊದಲ_ಅಂಚು_ಸಂಖ್ಯೆ;
    };

    /**
     * @internal 
     */
    class List_Branch_ಗಂಟು : ಸಾರ್ವಜನಿಕ Branch_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        List_Branch_ಗಂಟು() : Branch_ಗಂಟು(0x444444), ಉದ್ದಳತೆ(0) {}
        ವಾಸ್ತವಿಕ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 &other) ಸ್ಥಿರಾಂಕ override;
        ವಾಸ್ತವಿಕ int32_t mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ) override;
        ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        ಶೂನ್ಯ ಕೂಡಿಸು(int32_t c, int32_t ಮೌಲ್ಯ_2) {
            units[ಉದ್ದಳತೆ] = static_cast<char16_t>(c);
            equal[ಉದ್ದಳತೆ]=nullptr;
            ಮೌಲ್ಯಗಳು[ಉದ್ದಳತೆ]=ಮೌಲ್ಯ_2;
            ++ಉದ್ದಳತೆ;
            hash=(hash*37u+c)*37u+ಮೌಲ್ಯ_2;
        }
        // Adds a unit which leads to another match node.
        ಶೂನ್ಯ ಕೂಡಿಸು(int32_t c, ಗಂಟು_2 *ಗಂಟು) {
            units[ಉದ್ದಳತೆ] = static_cast<char16_t>(c);
            equal[ಉದ್ದಳತೆ]=ಗಂಟು;
            ಮೌಲ್ಯಗಳು[ಉದ್ದಳತೆ]=0;
            ++ಉದ್ದಳತೆ;
            hash=(hash*37u+c)*37u+hash_ಸಂಕೇತ(ಗಂಟು);
        }
    ಸಂರಕ್ಷಿತ:
        ಗಂಟು_2 *equal[k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ];  // nullptr means "has final value".
        int32_t ಉದ್ದಳತೆ;
        int32_t ಮೌಲ್ಯಗಳು[k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ];
        char16_t units[k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ];
    };

    /**
     * @internal 
     */
    class ವಿಭಜಿಸು_Branch_ಗಂಟು : ಸಾರ್ವಜನಿಕ Branch_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        ವಿಭಜಿಸು_Branch_ಗಂಟು(char16_t ಮಧ್ಯ_Unit, ಗಂಟು_2 *less_Than_ಗಂಟು, ಗಂಟು_2 *greater_Or_Equal_ಗಂಟು)
                : Branch_ಗಂಟು(((0x555555u*37u+ಮಧ್ಯ_Unit)*37u+
                              hash_ಸಂಕೇತ(less_Than_ಗಂಟು))*37u+hash_ಸಂಕೇತ(greater_Or_Equal_ಗಂಟು)),
                  unit(ಮಧ್ಯ_Unit), lessThan(less_Than_ಗಂಟು), greaterOrEqual(greater_Or_Equal_ಗಂಟು) {}
        ವಾಸ್ತವಿಕ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 &other) ಸ್ಥಿರಾಂಕ override;
        ವಾಸ್ತವಿಕ int32_t mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ) override;
        ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
    ಸಂರಕ್ಷಿತ:
        char16_t unit;
        ಗಂಟು_2 *lessThan;
        ಗಂಟು_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_ತಲೆ_ಗಂಟು : ಸಾರ್ವಜನಿಕ ಮೌಲ್ಯ_ಗಂಟು {
    ಸಾರ್ವಜನಿಕ:
        Branch_ತಲೆ_ಗಂಟು(int32_t len, ಗಂಟು_2 *sub_ಗಂಟು)
                : ಮೌಲ್ಯ_ಗಂಟು((0x666666u*37u+len)*37u+hash_ಸಂಕೇತ(sub_ಗಂಟು)),
                  ಉದ್ದಳತೆ(len), ಮುಂದಿನ(sub_ಗಂಟು) {}
        ವಾಸ್ತವಿಕ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಗಂಟು_2 &other) ಸ್ಥಿರಾಂಕ override;
        ವಾಸ್ತವಿಕ int32_t mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ) override;
        ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಲಿಖ(StringTrieBuilder &builder) override;
    ಸಂರಕ್ಷಿತ:
        int32_t ಉದ್ದಳತೆ;
        ಗಂಟು_2 *ಮುಂದಿನ;  // A branch sub-node.
    };

#ಆದರೆ_ಅಂತ್ಯ  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    ವಾಸ್ತವಿಕ ಗಂಟು_2 *create_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, int32_t ಉದ್ದಳತೆ,
                                        ಗಂಟು_2 *ಮುಂದಿನ_ಗಂಟು) ಸ್ಥಿರಾಂಕ = 0;

    /** @internal */
    ವಾಸ್ತವಿಕ int32_t ಲಿಖ(int32_t unit) = 0;
    /** @internal */
    ವಾಸ್ತವಿಕ int32_t writeElementUnits(int32_t i, int32_t unit_ಸೂಚ್ಯಂಕ, int32_t ಉದ್ದಳತೆ) = 0;
    /** @internal */
    ವಾಸ್ತವಿಕ int32_t write_ಮೌಲ್ಯ_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    ವಾಸ್ತವಿಕ int32_t write_ಮೌಲ್ಯ_And_Type(UBool has_ಮೌಲ್ಯ, int32_t ಮೌಲ್ಯ_2, int32_t ಗಂಟು) = 0;
    /** @internal */
    ವಾಸ್ತವಿಕ int32_t write_Delta_ಗಮ್ಯ(int32_t jump_ಗುರಿ) = 0;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* U_SHOW_CPLUSPLUS_API */

#ಆದರೆ_ಅಂತ್ಯ  // __STRINGTRIEBUILDER_H__
