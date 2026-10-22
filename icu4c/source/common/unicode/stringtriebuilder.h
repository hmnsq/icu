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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ U_SHOW_CPLUSPLUS_API

#അന്തര്ഭാവ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
ഘടന UHashtable;
തരനിർവചനം ഘടന UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
എണ്ണൽ UStringTrieBuildOption {
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
class U_COMMON_API StringTrieBuilder : പൊതു UObject {
പൊതു:
#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    സ്ഥിതം int32_t hash_കണ്ണി(സ്ഥിരം ശൂന്യം *കണ്ണി);
    /** @internal */
    സ്ഥിതം UBool equalNodes(സ്ഥിരം ശൂന്യം *ഇടത്, സ്ഥിരം ശൂന്യം *വലത്);
#endif  /* U_HIDE_INTERNAL_API */

സംരക്ഷിത:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    മായാ ~StringTrieBuilder();

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    ശൂന്യം createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode);
    /** @internal */
    ശൂന്യം deleteCompactBuilder();

    /** @internal */
    ശൂന്യം build(UStringTrieBuildOption buildOption, int32_t elementsLength, UErrorCode &errorCode);

    /** @internal */
    int32_t write_കണ്ണി(int32_t start, int32_t limit, int32_t unitIndex);
    /** @internal */
    int32_t write_Branch_Sub_കണ്ണി(int32_t start, int32_t limit, int32_t unitIndex, int32_t length);
#endif  /* U_HIDE_INTERNAL_API */

    class കണ്ണി_2;

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    കണ്ണി_2* make_കണ്ണി(int32_t start,
                   int32_t limit,
                   int32_t unitIndex,
                   UErrorCode& errorCode) U_LIFETIME_BOUND;
    /** @internal */
    കണ്ണി_2* make_Branch_Sub_കണ്ണി(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            int32_t length,
                            UErrorCode& errorCode) U_LIFETIME_BOUND;
#endif  /* U_HIDE_INTERNAL_API */

    /** @internal */
    മായാ int32_t getElementStringLength(int32_t i) സ്ഥിരം = 0;
    /** @internal */
    മായാ char16_t getElementUnit(int32_t i, int32_t unitIndex) സ്ഥിരം = 0;
    /** @internal */
    മായാ int32_t get_Element_മൂല്യം(int32_t i) സ്ഥിരം = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    മായാ int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) സ്ഥിരം = 0;

    // Number of different units at unitIndex.
    /** @internal */
    മായാ int32_t എണ്ണം_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) സ്ഥിരം = 0;
    /** @internal */
    മായാ int32_t skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t എണ്ണം) സ്ഥിരം = 0;
    /** @internal */
    മായാ int32_t indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) സ്ഥിരം = 0;

    /** @internal */
    മായാ UBool match_Nodes_Can_Have_മൂല്യങ്ങൾ() സ്ഥിരം = 0;

    /** @internal */
    മായാ int32_t get_Max_Branch_Linear_Sub_കണ്ണി_Length() സ്ഥിരം = 0;
    /** @internal */
    മായാ int32_t getMinLinearMatch() സ്ഥിരം = 0;
    /** @internal */
    മായാ int32_t getMaxLinearMatchLength() സ്ഥിരം = 0;

#ifndef U_HIDE_INTERNAL_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    സ്ഥിതം സ്ഥിരം int32_t k_Max_Branch_Linear_Sub_കണ്ണി_Length=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    സ്ഥിതം സ്ഥിരം int32_t kMaxSplitBranchLevels=14;

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
    കണ്ണി_2* register_കണ്ണി(കണ്ണി_2* new_കണ്ണി, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    കണ്ണി_2* register_Final_മൂല്യം(int32_t മൂല്യം_2, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    class കണ്ണി_2 : പൊതു UObject {
    പൊതു:
        കണ്ണി_2(int32_t initialHash) : hash(initialHash), offset(0) {}
        വരിയിൽ int32_t hashCode() സ്ഥിരം { തിരികെ hash; }
        // Handles node==nullptr.
        സ്ഥിതം വരിയിൽ int32_t hashCode(സ്ഥിരം കണ്ണി_2 *കണ്ണി) { തിരികെ കണ്ണി==nullptr ? 0 : കണ്ണി->hashCode(); }
        // Base class operator==() compares the actual class types.
        മായാ bool operator==(സ്ഥിരം കണ്ണി_2 &other) സ്ഥിരം;
        വരിയിൽ bool operator!=(സ്ഥിരം കണ്ണി_2 &other) സ്ഥിരം { തിരികെ !operator==(other); }
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
        മായാ int32_t mark_വലത്_Edges_First(int32_t edge_സംഖ്യ);
        // write() must set the offset to a positive value.
        മായാ ശൂന്യം ലിഖ(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        വരിയിൽ ശൂന്യം write_Unless_Inside_വലത്_Edge(int32_t first_വലത്, int32_t last_വലത്,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            എങ്കിൽ(offset<0 && (offset<last_വലത് || first_വലത്<offset)) {
                ലിഖ(builder);
            }
        }
        വരിയിൽ int32_t getOffset() സ്ഥിരം { തിരികെ offset; }
    സംരക്ഷിത:
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
    class Final_മൂല്യം_കണ്ണി : പൊതു കണ്ണി_2 {
    പൊതു:
        Final_മൂല്യം_കണ്ണി(int32_t v) : കണ്ണി_2(0x111111u*37u+v), മൂല്യം_2(v) {}
        മായാ bool operator==(സ്ഥിരം കണ്ണി_2 &other) സ്ഥിരം override;
        മായാ ശൂന്യം ലിഖ(StringTrieBuilder &builder) override;
    സംരക്ഷിത:
        int32_t മൂല്യം_2;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class മൂല്യം_കണ്ണി : പൊതു കണ്ണി_2 {
    പൊതു:
        മൂല്യം_കണ്ണി(int32_t initialHash) : കണ്ണി_2(initialHash), has_മൂല്യം(false), മൂല്യം_2(0) {}
        മായാ bool operator==(സ്ഥിരം കണ്ണി_2 &other) സ്ഥിരം override;
        ശൂന്യം set_മൂല്യം(int32_t v) {
            has_മൂല്യം=true;
            മൂല്യം_2=v;
            hash=hash*37u+v;
        }
    സംരക്ഷിത:
        UBool has_മൂല്യം;
        int32_t മൂല്യം_2;
    };

#ifndef U_HIDE_INTERNAL_API
    /** 
     * @internal 
     */
    class Intermediate_മൂല്യം_കണ്ണി : പൊതു മൂല്യം_കണ്ണി {
    പൊതു:
        Intermediate_മൂല്യം_കണ്ണി(int32_t v, കണ്ണി_2 *next_കണ്ണി)
                : മൂല്യം_കണ്ണി(0x222222u*37u+hashCode(next_കണ്ണി)), next(next_കണ്ണി) { set_മൂല്യം(v); }
        മായാ bool operator==(സ്ഥിരം കണ്ണി_2 &other) സ്ഥിരം override;
        മായാ int32_t mark_വലത്_Edges_First(int32_t edge_സംഖ്യ) override;
        മായാ ശൂന്യം ലിഖ(StringTrieBuilder &builder) override;
    സംരക്ഷിത:
        കണ്ണി_2 *next;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class Linear_Match_കണ്ണി : പൊതു മൂല്യം_കണ്ണി {
    പൊതു:
        Linear_Match_കണ്ണി(int32_t len, കണ്ണി_2 *next_കണ്ണി)
                : മൂല്യം_കണ്ണി((0x333333u*37u+len)*37u+hashCode(next_കണ്ണി)),
                  length(len), next(next_കണ്ണി) {}
        മായാ bool operator==(സ്ഥിരം കണ്ണി_2 &other) സ്ഥിരം override;
        മായാ int32_t mark_വലത്_Edges_First(int32_t edge_സംഖ്യ) override;
    സംരക്ഷിത:
        int32_t length;
        കണ്ണി_2 *next;
    };

#ifndef U_HIDE_INTERNAL_API
    /**
     * @internal 
     */
    class Branch_കണ്ണി : പൊതു കണ്ണി_2 {
    പൊതു:
        Branch_കണ്ണി(int32_t initialHash) : കണ്ണി_2(initialHash) {}
    സംരക്ഷിത:
        int32_t first_Edge_സംഖ്യ;
    };

    /**
     * @internal 
     */
    class List_Branch_കണ്ണി : പൊതു Branch_കണ്ണി {
    പൊതു:
        List_Branch_കണ്ണി() : Branch_കണ്ണി(0x444444), length(0) {}
        മായാ bool operator==(സ്ഥിരം കണ്ണി_2 &other) സ്ഥിരം override;
        മായാ int32_t mark_വലത്_Edges_First(int32_t edge_സംഖ്യ) override;
        മായാ ശൂന്യം ലിഖ(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        ശൂന്യം add(int32_t c, int32_t മൂല്യം_2) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=nullptr;
            മൂല്യങ്ങൾ[length]=മൂല്യം_2;
            ++length;
            hash=(hash*37u+c)*37u+മൂല്യം_2;
        }
        // Adds a unit which leads to another match node.
        ശൂന്യം add(int32_t c, കണ്ണി_2 *കണ്ണി) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=കണ്ണി;
            മൂല്യങ്ങൾ[length]=0;
            ++length;
            hash=(hash*37u+c)*37u+hashCode(കണ്ണി);
        }
    സംരക്ഷിത:
        കണ്ണി_2 *equal[k_Max_Branch_Linear_Sub_കണ്ണി_Length];  // nullptr means "has final value".
        int32_t length;
        int32_t മൂല്യങ്ങൾ[k_Max_Branch_Linear_Sub_കണ്ണി_Length];
        char16_t units[k_Max_Branch_Linear_Sub_കണ്ണി_Length];
    };

    /**
     * @internal 
     */
    class Split_Branch_കണ്ണി : പൊതു Branch_കണ്ണി {
    പൊതു:
        Split_Branch_കണ്ണി(char16_t നടുവ്_Unit, കണ്ണി_2 *less_Than_കണ്ണി, കണ്ണി_2 *greater_Or_Equal_കണ്ണി)
                : Branch_കണ്ണി(((0x555555u*37u+നടുവ്_Unit)*37u+
                              hashCode(less_Than_കണ്ണി))*37u+hashCode(greater_Or_Equal_കണ്ണി)),
                  unit(നടുവ്_Unit), lessThan(less_Than_കണ്ണി), greaterOrEqual(greater_Or_Equal_കണ്ണി) {}
        മായാ bool operator==(സ്ഥിരം കണ്ണി_2 &other) സ്ഥിരം override;
        മായാ int32_t mark_വലത്_Edges_First(int32_t edge_സംഖ്യ) override;
        മായാ ശൂന്യം ലിഖ(StringTrieBuilder &builder) override;
    സംരക്ഷിത:
        char16_t unit;
        കണ്ണി_2 *lessThan;
        കണ്ണി_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_Head_കണ്ണി : പൊതു മൂല്യം_കണ്ണി {
    പൊതു:
        Branch_Head_കണ്ണി(int32_t len, കണ്ണി_2 *sub_കണ്ണി)
                : മൂല്യം_കണ്ണി((0x666666u*37u+len)*37u+hashCode(sub_കണ്ണി)),
                  length(len), next(sub_കണ്ണി) {}
        മായാ bool operator==(സ്ഥിരം കണ്ണി_2 &other) സ്ഥിരം override;
        മായാ int32_t mark_വലത്_Edges_First(int32_t edge_സംഖ്യ) override;
        മായാ ശൂന്യം ലിഖ(StringTrieBuilder &builder) override;
    സംരക്ഷിത:
        int32_t length;
        കണ്ണി_2 *next;  // A branch sub-node.
    };

#endif  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    മായാ കണ്ണി_2 *create_Linear_Match_കണ്ണി(int32_t i, int32_t unitIndex, int32_t length,
                                        കണ്ണി_2 *next_കണ്ണി) സ്ഥിരം = 0;

    /** @internal */
    മായാ int32_t ലിഖ(int32_t unit) = 0;
    /** @internal */
    മായാ int32_t writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) = 0;
    /** @internal */
    മായാ int32_t write_മൂല്യം_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    മായാ int32_t write_മൂല്യം_And_Type(UBool has_മൂല്യം, int32_t മൂല്യം_2, int32_t കണ്ണി) = 0;
    /** @internal */
    മായാ int32_t writeDeltaTo(int32_t jumpTarget) = 0;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __STRINGTRIEBUILDER_H__
