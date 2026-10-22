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

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API

#अन्तर्भाव "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
संरचना UHashtable;
प्रकारनिर्देश संरचना UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
गणना UStringTrieBuildOption {
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
class U_COMMON_API StringTrieBuilder : सार्वजनिक UObject {
सार्वजनिक:
#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    स्थिर int32_t hash_गाँठो(अचर शून्य *गाँठो);
    /** @internal */
    स्थिर UBool equalNodes(अचर शून्य *बायाँ, अचर शून्य *दायाँ);
#endif  /* U_HIDE_INTERNAL_API */

रक्षित:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    आभास ~StringTrieBuilder();

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    शून्य createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode);
    /** @internal */
    शून्य deleteCompactBuilder();

    /** @internal */
    शून्य build(UStringTrieBuildOption buildOption, int32_t elementsLength, UErrorCode &errorCode);

    /** @internal */
    int32_t write_गाँठो(int32_t start, int32_t limit, int32_t unitIndex);
    /** @internal */
    int32_t write_Branch_Sub_गाँठो(int32_t start, int32_t limit, int32_t unitIndex, int32_t length);
#endif  /* U_HIDE_INTERNAL_API */

    class गाँठो_2;

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    गाँठो_2* make_गाँठो(int32_t start,
                   int32_t limit,
                   int32_t unitIndex,
                   UErrorCode& errorCode) U_LIFETIME_BOUND;
    /** @internal */
    गाँठो_2* make_Branch_Sub_गाँठो(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            int32_t length,
                            UErrorCode& errorCode) U_LIFETIME_BOUND;
#endif  /* U_HIDE_INTERNAL_API */

    /** @internal */
    आभास int32_t getElementStringLength(int32_t i) अचर = 0;
    /** @internal */
    आभास char16_t getElementUnit(int32_t i, int32_t unitIndex) अचर = 0;
    /** @internal */
    आभास int32_t get_Element_मान(int32_t i) अचर = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    आभास int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) अचर = 0;

    // Number of different units at unitIndex.
    /** @internal */
    आभास int32_t गन्ती_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) अचर = 0;
    /** @internal */
    आभास int32_t skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t गन्ती) अचर = 0;
    /** @internal */
    आभास int32_t indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) अचर = 0;

    /** @internal */
    आभास UBool match_Nodes_Can_Have_मानहरू() अचर = 0;

    /** @internal */
    आभास int32_t get_Max_Branch_Linear_Sub_गाँठो_Length() अचर = 0;
    /** @internal */
    आभास int32_t getMinLinearMatch() अचर = 0;
    /** @internal */
    आभास int32_t getMaxLinearMatchLength() अचर = 0;

#ifndef U_HIDE_INTERNAL_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    स्थिर अचर int32_t k_Max_Branch_Linear_Sub_गाँठो_Length=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    स्थिर अचर int32_t kMaxSplitBranchLevels=14;

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
    गाँठो_2* register_गाँठो(गाँठो_2* new_गाँठो, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    गाँठो_2* register_Final_मान(int32_t मान_2, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    class गाँठो_2 : सार्वजनिक UObject {
    सार्वजनिक:
        गाँठो_2(int32_t initialHash) : hash(initialHash), offset(0) {}
        अन्तःस्थ int32_t hashCode() अचर { फर्काऊ hash; }
        // Handles node==nullptr.
        स्थिर अन्तःस्थ int32_t hashCode(अचर गाँठो_2 *गाँठो) { फर्काऊ गाँठो==nullptr ? 0 : गाँठो->hashCode(); }
        // Base class operator==() compares the actual class types.
        आभास bool चालक==(अचर गाँठो_2 &other) अचर;
        अन्तःस्थ bool चालक!=(अचर गाँठो_2 &other) अचर { फर्काऊ !चालक==(other); }
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
        आभास int32_t mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या);
        // write() must set the offset to a positive value.
        आभास शून्य लिख(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        अन्तःस्थ शून्य write_Unless_Inside_दायाँ_Edge(int32_t first_दायाँ, int32_t last_दायाँ,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            यदि(offset<0 && (offset<last_दायाँ || first_दायाँ<offset)) {
                लिख(builder);
            }
        }
        अन्तःस्थ int32_t getOffset() अचर { फर्काऊ offset; }
    रक्षित:
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
    class Final_मान_गाँठो : सार्वजनिक गाँठो_2 {
    सार्वजनिक:
        Final_मान_गाँठो(int32_t v) : गाँठो_2(0x111111u*37u+v), मान_2(v) {}
        आभास bool चालक==(अचर गाँठो_2 &other) अचर override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
    रक्षित:
        int32_t मान_2;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class मान_गाँठो : सार्वजनिक गाँठो_2 {
    सार्वजनिक:
        मान_गाँठो(int32_t initialHash) : गाँठो_2(initialHash), has_मान(false), मान_2(0) {}
        आभास bool चालक==(अचर गाँठो_2 &other) अचर override;
        शून्य set_मान(int32_t v) {
            has_मान=true;
            मान_2=v;
            hash=hash*37u+v;
        }
    रक्षित:
        UBool has_मान;
        int32_t मान_2;
    };

#ifndef U_HIDE_INTERNAL_API
    /** 
     * @internal 
     */
    class Intermediate_मान_गाँठो : सार्वजनिक मान_गाँठो {
    सार्वजनिक:
        Intermediate_मान_गाँठो(int32_t v, गाँठो_2 *next_गाँठो)
                : मान_गाँठो(0x222222u*37u+hashCode(next_गाँठो)), next(next_गाँठो) { set_मान(v); }
        आभास bool चालक==(अचर गाँठो_2 &other) अचर override;
        आभास int32_t mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या) override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
    रक्षित:
        गाँठो_2 *next;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class Linear_Match_गाँठो : सार्वजनिक मान_गाँठो {
    सार्वजनिक:
        Linear_Match_गाँठो(int32_t len, गाँठो_2 *next_गाँठो)
                : मान_गाँठो((0x333333u*37u+len)*37u+hashCode(next_गाँठो)),
                  length(len), next(next_गाँठो) {}
        आभास bool चालक==(अचर गाँठो_2 &other) अचर override;
        आभास int32_t mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या) override;
    रक्षित:
        int32_t length;
        गाँठो_2 *next;
    };

#ifndef U_HIDE_INTERNAL_API
    /**
     * @internal 
     */
    class Branch_गाँठो : सार्वजनिक गाँठो_2 {
    सार्वजनिक:
        Branch_गाँठो(int32_t initialHash) : गाँठो_2(initialHash) {}
    रक्षित:
        int32_t first_Edge_सङ्ख्या;
    };

    /**
     * @internal 
     */
    class List_Branch_गाँठो : सार्वजनिक Branch_गाँठो {
    सार्वजनिक:
        List_Branch_गाँठो() : Branch_गाँठो(0x444444), length(0) {}
        आभास bool चालक==(अचर गाँठो_2 &other) अचर override;
        आभास int32_t mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या) override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        शून्य add(int32_t c, int32_t मान_2) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=nullptr;
            मानहरू[length]=मान_2;
            ++length;
            hash=(hash*37u+c)*37u+मान_2;
        }
        // Adds a unit which leads to another match node.
        शून्य add(int32_t c, गाँठो_2 *गाँठो) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=गाँठो;
            मानहरू[length]=0;
            ++length;
            hash=(hash*37u+c)*37u+hashCode(गाँठो);
        }
    रक्षित:
        गाँठो_2 *equal[k_Max_Branch_Linear_Sub_गाँठो_Length];  // nullptr means "has final value".
        int32_t length;
        int32_t मानहरू[k_Max_Branch_Linear_Sub_गाँठो_Length];
        char16_t units[k_Max_Branch_Linear_Sub_गाँठो_Length];
    };

    /**
     * @internal 
     */
    class Split_Branch_गाँठो : सार्वजनिक Branch_गाँठो {
    सार्वजनिक:
        Split_Branch_गाँठो(char16_t बीच_Unit, गाँठो_2 *less_Than_गाँठो, गाँठो_2 *greater_Or_Equal_गाँठो)
                : Branch_गाँठो(((0x555555u*37u+बीच_Unit)*37u+
                              hashCode(less_Than_गाँठो))*37u+hashCode(greater_Or_Equal_गाँठो)),
                  unit(बीच_Unit), lessThan(less_Than_गाँठो), greaterOrEqual(greater_Or_Equal_गाँठो) {}
        आभास bool चालक==(अचर गाँठो_2 &other) अचर override;
        आभास int32_t mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या) override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
    रक्षित:
        char16_t unit;
        गाँठो_2 *lessThan;
        गाँठो_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_Head_गाँठो : सार्वजनिक मान_गाँठो {
    सार्वजनिक:
        Branch_Head_गाँठो(int32_t len, गाँठो_2 *sub_गाँठो)
                : मान_गाँठो((0x666666u*37u+len)*37u+hashCode(sub_गाँठो)),
                  length(len), next(sub_गाँठो) {}
        आभास bool चालक==(अचर गाँठो_2 &other) अचर override;
        आभास int32_t mark_दायाँ_Edges_First(int32_t edge_सङ्ख्या) override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
    रक्षित:
        int32_t length;
        गाँठो_2 *next;  // A branch sub-node.
    };

#endif  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    आभास गाँठो_2 *create_Linear_Match_गाँठो(int32_t i, int32_t unitIndex, int32_t length,
                                        गाँठो_2 *next_गाँठो) अचर = 0;

    /** @internal */
    आभास int32_t लिख(int32_t unit) = 0;
    /** @internal */
    आभास int32_t writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) = 0;
    /** @internal */
    आभास int32_t write_मान_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    आभास int32_t write_मान_And_Type(UBool has_मान, int32_t मान_2, int32_t गाँठो) = 0;
    /** @internal */
    आभास int32_t writeDeltaTo(int32_t jumpTarget) = 0;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __STRINGTRIEBUILDER_H__
