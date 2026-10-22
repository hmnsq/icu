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

#અન્તર્ભાવ "unicode/utypes.h"

#જો U_SHOW_CPLUSPLUS_API

#અન્તર્ભાવ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
સંરચના UHashtable;
પ્રકારવ્યાખ્યા સંરચના UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
ગણના UStringTrieBuildOption {
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
class U_COMMON_API StringTrieBuilder : જાહેર UObject {
જાહેર:
#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    સ્થિર int32_t hash_ગાંઠ(અચલ શૂન્ય *ગાંઠ);
    /** @internal */
    સ્થિર UBool equalNodes(અચલ શૂન્ય *ડાબું, અચલ શૂન્ય *જમણું);
#endif  /* U_HIDE_INTERNAL_API */

સુરક્ષિત:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    આભાસી ~StringTrieBuilder();

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    શૂન્ય createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode);
    /** @internal */
    શૂન્ય deleteCompactBuilder();

    /** @internal */
    શૂન્ય build(UStringTrieBuildOption buildOption, int32_t elementsLength, UErrorCode &errorCode);

    /** @internal */
    int32_t write_ગાંઠ(int32_t start, int32_t limit, int32_t unitIndex);
    /** @internal */
    int32_t write_Branch_Sub_ગાંઠ(int32_t start, int32_t limit, int32_t unitIndex, int32_t length);
#endif  /* U_HIDE_INTERNAL_API */

    class ગાંઠ_2;

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    ગાંઠ_2* make_ગાંઠ(int32_t start,
                   int32_t limit,
                   int32_t unitIndex,
                   UErrorCode& errorCode) U_LIFETIME_BOUND;
    /** @internal */
    ગાંઠ_2* make_Branch_Sub_ગાંઠ(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            int32_t length,
                            UErrorCode& errorCode) U_LIFETIME_BOUND;
#endif  /* U_HIDE_INTERNAL_API */

    /** @internal */
    આભાસી int32_t getElementStringLength(int32_t i) અચલ = 0;
    /** @internal */
    આભાસી char16_t getElementUnit(int32_t i, int32_t unitIndex) અચલ = 0;
    /** @internal */
    આભાસી int32_t get_Element_કિંમત(int32_t i) અચલ = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    આભાસી int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) અચલ = 0;

    // Number of different units at unitIndex.
    /** @internal */
    આભાસી int32_t ગણતરી_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) અચલ = 0;
    /** @internal */
    આભાસી int32_t skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t ગણતરી) અચલ = 0;
    /** @internal */
    આભાસી int32_t indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) અચલ = 0;

    /** @internal */
    આભાસી UBool match_Nodes_Can_Have_કિંમતો() અચલ = 0;

    /** @internal */
    આભાસી int32_t get_Max_Branch_Linear_Sub_ગાંઠ_Length() અચલ = 0;
    /** @internal */
    આભાસી int32_t getMinLinearMatch() અચલ = 0;
    /** @internal */
    આભાસી int32_t getMaxLinearMatchLength() અચલ = 0;

#ifndef U_HIDE_INTERNAL_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    સ્થિર અચલ int32_t k_Max_Branch_Linear_Sub_ગાંઠ_Length=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    સ્થિર અચલ int32_t kMaxSplitBranchLevels=14;

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
    ગાંઠ_2* register_ગાંઠ(ગાંઠ_2* new_ગાંઠ, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    ગાંઠ_2* register_Final_કિંમત(int32_t કિંમત_2, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    class ગાંઠ_2 : જાહેર UObject {
    જાહેર:
        ગાંઠ_2(int32_t initialHash) : hash(initialHash), offset(0) {}
        અંતઃરેખ int32_t hashCode() અચલ { પાછા hash; }
        // Handles node==nullptr.
        સ્થિર અંતઃરેખ int32_t hashCode(અચલ ગાંઠ_2 *ગાંઠ) { પાછા ગાંઠ==nullptr ? 0 : ગાંઠ->hashCode(); }
        // Base class operator==() compares the actual class types.
        આભાસી bool operator==(અચલ ગાંઠ_2 &other) અચલ;
        અંતઃરેખ bool operator!=(અચલ ગાંઠ_2 &other) અચલ { પાછા !operator==(other); }
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
        આભાસી int32_t mark_જમણું_Edges_First(int32_t edge_સંખ્યા);
        // write() must set the offset to a positive value.
        આભાસી શૂન્ય લિખ(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        અંતઃરેખ શૂન્ય write_Unless_Inside_જમણું_Edge(int32_t first_જમણું, int32_t last_જમણું,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            જો(offset<0 && (offset<last_જમણું || first_જમણું<offset)) {
                લિખ(builder);
            }
        }
        અંતઃરેખ int32_t getOffset() અચલ { પાછા offset; }
    સુરક્ષિત:
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
    class Final_કિંમત_ગાંઠ : જાહેર ગાંઠ_2 {
    જાહેર:
        Final_કિંમત_ગાંઠ(int32_t v) : ગાંઠ_2(0x111111u*37u+v), કિંમત_2(v) {}
        આભાસી bool operator==(અચલ ગાંઠ_2 &other) અચલ override;
        આભાસી શૂન્ય લિખ(StringTrieBuilder &builder) override;
    સુરક્ષિત:
        int32_t કિંમત_2;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class કિંમત_ગાંઠ : જાહેર ગાંઠ_2 {
    જાહેર:
        કિંમત_ગાંઠ(int32_t initialHash) : ગાંઠ_2(initialHash), has_કિંમત(false), કિંમત_2(0) {}
        આભાસી bool operator==(અચલ ગાંઠ_2 &other) અચલ override;
        શૂન્ય set_કિંમત(int32_t v) {
            has_કિંમત=true;
            કિંમત_2=v;
            hash=hash*37u+v;
        }
    સુરક્ષિત:
        UBool has_કિંમત;
        int32_t કિંમત_2;
    };

#ifndef U_HIDE_INTERNAL_API
    /** 
     * @internal 
     */
    class Intermediate_કિંમત_ગાંઠ : જાહેર કિંમત_ગાંઠ {
    જાહેર:
        Intermediate_કિંમત_ગાંઠ(int32_t v, ગાંઠ_2 *next_ગાંઠ)
                : કિંમત_ગાંઠ(0x222222u*37u+hashCode(next_ગાંઠ)), next(next_ગાંઠ) { set_કિંમત(v); }
        આભાસી bool operator==(અચલ ગાંઠ_2 &other) અચલ override;
        આભાસી int32_t mark_જમણું_Edges_First(int32_t edge_સંખ્યા) override;
        આભાસી શૂન્ય લિખ(StringTrieBuilder &builder) override;
    સુરક્ષિત:
        ગાંઠ_2 *next;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class Linear_Match_ગાંઠ : જાહેર કિંમત_ગાંઠ {
    જાહેર:
        Linear_Match_ગાંઠ(int32_t len, ગાંઠ_2 *next_ગાંઠ)
                : કિંમત_ગાંઠ((0x333333u*37u+len)*37u+hashCode(next_ગાંઠ)),
                  length(len), next(next_ગાંઠ) {}
        આભાસી bool operator==(અચલ ગાંઠ_2 &other) અચલ override;
        આભાસી int32_t mark_જમણું_Edges_First(int32_t edge_સંખ્યા) override;
    સુરક્ષિત:
        int32_t length;
        ગાંઠ_2 *next;
    };

#ifndef U_HIDE_INTERNAL_API
    /**
     * @internal 
     */
    class Branch_ગાંઠ : જાહેર ગાંઠ_2 {
    જાહેર:
        Branch_ગાંઠ(int32_t initialHash) : ગાંઠ_2(initialHash) {}
    સુરક્ષિત:
        int32_t first_Edge_સંખ્યા;
    };

    /**
     * @internal 
     */
    class List_Branch_ગાંઠ : જાહેર Branch_ગાંઠ {
    જાહેર:
        List_Branch_ગાંઠ() : Branch_ગાંઠ(0x444444), length(0) {}
        આભાસી bool operator==(અચલ ગાંઠ_2 &other) અચલ override;
        આભાસી int32_t mark_જમણું_Edges_First(int32_t edge_સંખ્યા) override;
        આભાસી શૂન્ય લિખ(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        શૂન્ય add(int32_t c, int32_t કિંમત_2) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=nullptr;
            કિંમતો[length]=કિંમત_2;
            ++length;
            hash=(hash*37u+c)*37u+કિંમત_2;
        }
        // Adds a unit which leads to another match node.
        શૂન્ય add(int32_t c, ગાંઠ_2 *ગાંઠ) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=ગાંઠ;
            કિંમતો[length]=0;
            ++length;
            hash=(hash*37u+c)*37u+hashCode(ગાંઠ);
        }
    સુરક્ષિત:
        ગાંઠ_2 *equal[k_Max_Branch_Linear_Sub_ગાંઠ_Length];  // nullptr means "has final value".
        int32_t length;
        int32_t કિંમતો[k_Max_Branch_Linear_Sub_ગાંઠ_Length];
        char16_t units[k_Max_Branch_Linear_Sub_ગાંઠ_Length];
    };

    /**
     * @internal 
     */
    class Split_Branch_ગાંઠ : જાહેર Branch_ગાંઠ {
    જાહેર:
        Split_Branch_ગાંઠ(char16_t મધ્ય_Unit, ગાંઠ_2 *less_Than_ગાંઠ, ગાંઠ_2 *greater_Or_Equal_ગાંઠ)
                : Branch_ગાંઠ(((0x555555u*37u+મધ્ય_Unit)*37u+
                              hashCode(less_Than_ગાંઠ))*37u+hashCode(greater_Or_Equal_ગાંઠ)),
                  unit(મધ્ય_Unit), lessThan(less_Than_ગાંઠ), greaterOrEqual(greater_Or_Equal_ગાંઠ) {}
        આભાસી bool operator==(અચલ ગાંઠ_2 &other) અચલ override;
        આભાસી int32_t mark_જમણું_Edges_First(int32_t edge_સંખ્યા) override;
        આભાસી શૂન્ય લિખ(StringTrieBuilder &builder) override;
    સુરક્ષિત:
        char16_t unit;
        ગાંઠ_2 *lessThan;
        ગાંઠ_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_Head_ગાંઠ : જાહેર કિંમત_ગાંઠ {
    જાહેર:
        Branch_Head_ગાંઠ(int32_t len, ગાંઠ_2 *sub_ગાંઠ)
                : કિંમત_ગાંઠ((0x666666u*37u+len)*37u+hashCode(sub_ગાંઠ)),
                  length(len), next(sub_ગાંઠ) {}
        આભાસી bool operator==(અચલ ગાંઠ_2 &other) અચલ override;
        આભાસી int32_t mark_જમણું_Edges_First(int32_t edge_સંખ્યા) override;
        આભાસી શૂન્ય લિખ(StringTrieBuilder &builder) override;
    સુરક્ષિત:
        int32_t length;
        ગાંઠ_2 *next;  // A branch sub-node.
    };

#endif  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    આભાસી ગાંઠ_2 *create_Linear_Match_ગાંઠ(int32_t i, int32_t unitIndex, int32_t length,
                                        ગાંઠ_2 *next_ગાંઠ) અચલ = 0;

    /** @internal */
    આભાસી int32_t લિખ(int32_t unit) = 0;
    /** @internal */
    આભાસી int32_t writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) = 0;
    /** @internal */
    આભાસી int32_t write_કિંમત_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    આભાસી int32_t write_કિંમત_And_Type(UBool has_કિંમત, int32_t કિંમત_2, int32_t ગાંઠ) = 0;
    /** @internal */
    આભાસી int32_t writeDeltaTo(int32_t jumpTarget) = 0;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __STRINGTRIEBUILDER_H__
