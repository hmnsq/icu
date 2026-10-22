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

#अगर_अघोषित __STRINGTRIEBUILDER_H__
#घोषणा __STRINGTRIEBUILDER_H__

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

#समावेश "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
काष्ठा UHashtable;
प्रकार काष्ठा UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
क्रमागत UStringTrieBuildOption {
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
class U_COMMON_API StringTrieBuilder : खुला UObject {
खुला:
#अगर_अघोषित U_HIDE_INTERNAL_API
    /** @internal */
    अटल int32_t hash_गाँठ(स्थिर व्योम *गाँठ);
    /** @internal */
    अटल UBool equalNodes(स्थिर व्योम *बायाँ, स्थिर व्योम *दायाँ);
#पूर्ण_अगर  /* U_HIDE_INTERNAL_API */

रक्षित:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    भव ~StringTrieBuilder();

#अगर_अघोषित U_HIDE_INTERNAL_API
    /** @internal */
    व्योम createCompactBuilder(int32_t आकार_Guess, UErrorCode &errorCode);
    /** @internal */
    व्योम deleteCompactBuilder();

    /** @internal */
    व्योम build(UStringTrieBuildOption buildOption, int32_t elements_लंबाई, UErrorCode &errorCode);

    /** @internal */
    int32_t write_गाँठ(int32_t start, int32_t limit, int32_t unit_सूचकांक);
    /** @internal */
    int32_t write_Branch_Sub_गाँठ(int32_t start, int32_t limit, int32_t unit_सूचकांक, int32_t लंबाई);
#पूर्ण_अगर  /* U_HIDE_INTERNAL_API */

    class गाँठ_2;

#अगर_अघोषित U_HIDE_INTERNAL_API
    /** @internal */
    गाँठ_2* make_गाँठ(int32_t start,
                   int32_t limit,
                   int32_t unit_सूचकांक,
                   UErrorCode& errorCode) U_LIFETIME_BOUND;
    /** @internal */
    गाँठ_2* make_Branch_Sub_गाँठ(int32_t start,
                            int32_t limit,
                            int32_t unit_सूचकांक,
                            int32_t लंबाई,
                            UErrorCode& errorCode) U_LIFETIME_BOUND;
#पूर्ण_अगर  /* U_HIDE_INTERNAL_API */

    /** @internal */
    भव int32_t get_Element_String_लंबाई(int32_t i) स्थिर = 0;
    /** @internal */
    भव char16_t getElementUnit(int32_t i, int32_t unit_सूचकांक) स्थिर = 0;
    /** @internal */
    भव int32_t get_Element_मूल्य(int32_t i) स्थिर = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    भव int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unit_सूचकांक) स्थिर = 0;

    // Number of different units at unitIndex.
    /** @internal */
    भव int32_t गिनती_Element_Units(int32_t start, int32_t limit, int32_t unit_सूचकांक) स्थिर = 0;
    /** @internal */
    भव int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_सूचकांक, int32_t गिनती) स्थिर = 0;
    /** @internal */
    भव int32_t सूचकांक_Of_Element_With_अग्र_Unit(int32_t i, int32_t unit_सूचकांक, char16_t unit) स्थिर = 0;

    /** @internal */
    भव UBool match_Nodes_Can_Have_मूल्यावली() स्थिर = 0;

    /** @internal */
    भव int32_t get_Max_Branch_Linear_Sub_गाँठ_लंबाई() स्थिर = 0;
    /** @internal */
    भव int32_t getMinLinearMatch() स्थिर = 0;
    /** @internal */
    भव int32_t get_Max_Linear_Match_लंबाई() स्थिर = 0;

#अगर_अघोषित U_HIDE_INTERNAL_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    अटल स्थिर int32_t k_Max_Branch_Linear_Sub_गाँठ_लंबाई=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    अटल स्थिर int32_t kMaxSplitBranchLevels=14;

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
    गाँठ_2* register_गाँठ(गाँठ_2* new_गाँठ, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    गाँठ_2* register_Final_मूल्य(int32_t मूल्य_2, UErrorCode& errorCode) U_LIFETIME_BOUND;
#पूर्ण_अगर  /* U_HIDE_INTERNAL_API */

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
    class गाँठ_2 : खुला UObject {
    खुला:
        गाँठ_2(int32_t initialHash) : hash(initialHash), offset(0) {}
        अंतरभूत int32_t hashCode() स्थिर { वापस hash; }
        // Handles node==nullptr.
        अटल अंतरभूत int32_t hashCode(स्थिर गाँठ_2 *गाँठ) { वापस गाँठ==nullptr ? 0 : गाँठ->hashCode(); }
        // Base class operator==() compares the actual class types.
        भव bool चालक==(स्थिर गाँठ_2 &other) स्थिर;
        अंतरभूत bool चालक!=(स्थिर गाँठ_2 &other) स्थिर { वापस !चालक==(other); }
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
        भव int32_t mark_दायाँ_Edges_First(int32_t edge_संख्या);
        // write() must set the offset to a positive value.
        भव व्योम ग_लिखो(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        अंतरभूत व्योम write_Unless_Inside_दायाँ_Edge(int32_t first_दायाँ, int32_t last_दायाँ,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            अगर(offset<0 && (offset<last_दायाँ || first_दायाँ<offset)) {
                ग_लिखो(builder);
            }
        }
        अंतरभूत int32_t getOffset() स्थिर { वापस offset; }
    रक्षित:
        int32_t hash;
        int32_t offset;
    };

#अगर_अघोषित U_HIDE_INTERNAL_API
    // This class should not be overridden because
    // registerFinalValue() compares a stack-allocated FinalValueNode
    // (stack-allocated so that we don't unnecessarily create lots of duplicate nodes)
    // with the input node, and the
    // !Node::operator==(other) used inside FinalValueNode::operator==(other)
    // will be false if the typeid's are different.
    /** @internal */
    class Final_मूल्य_गाँठ : खुला गाँठ_2 {
    खुला:
        Final_मूल्य_गाँठ(int32_t v) : गाँठ_2(0x111111u*37u+v), मूल्य_2(v) {}
        भव bool चालक==(स्थिर गाँठ_2 &other) स्थिर override;
        भव व्योम ग_लिखो(StringTrieBuilder &builder) override;
    रक्षित:
        int32_t मूल्य_2;
    };
#पूर्ण_अगर  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class मूल्य_गाँठ : खुला गाँठ_2 {
    खुला:
        मूल्य_गाँठ(int32_t initialHash) : गाँठ_2(initialHash), has_मूल्य(false), मूल्य_2(0) {}
        भव bool चालक==(स्थिर गाँठ_2 &other) स्थिर override;
        व्योम set_मूल्य(int32_t v) {
            has_मूल्य=true;
            मूल्य_2=v;
            hash=hash*37u+v;
        }
    रक्षित:
        UBool has_मूल्य;
        int32_t मूल्य_2;
    };

#अगर_अघोषित U_HIDE_INTERNAL_API
    /** 
     * @internal 
     */
    class Intermediate_मूल्य_गाँठ : खुला मूल्य_गाँठ {
    खुला:
        Intermediate_मूल्य_गाँठ(int32_t v, गाँठ_2 *अग्र_गाँठ)
                : मूल्य_गाँठ(0x222222u*37u+hashCode(अग्र_गाँठ)), अग्र(अग्र_गाँठ) { set_मूल्य(v); }
        भव bool चालक==(स्थिर गाँठ_2 &other) स्थिर override;
        भव int32_t mark_दायाँ_Edges_First(int32_t edge_संख्या) override;
        भव व्योम ग_लिखो(StringTrieBuilder &builder) override;
    रक्षित:
        गाँठ_2 *अग्र;
    };
#पूर्ण_अगर  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class Linear_Match_गाँठ : खुला मूल्य_गाँठ {
    खुला:
        Linear_Match_गाँठ(int32_t len, गाँठ_2 *अग्र_गाँठ)
                : मूल्य_गाँठ((0x333333u*37u+len)*37u+hashCode(अग्र_गाँठ)),
                  लंबाई(len), अग्र(अग्र_गाँठ) {}
        भव bool चालक==(स्थिर गाँठ_2 &other) स्थिर override;
        भव int32_t mark_दायाँ_Edges_First(int32_t edge_संख्या) override;
    रक्षित:
        int32_t लंबाई;
        गाँठ_2 *अग्र;
    };

#अगर_अघोषित U_HIDE_INTERNAL_API
    /**
     * @internal 
     */
    class Branch_गाँठ : खुला गाँठ_2 {
    खुला:
        Branch_गाँठ(int32_t initialHash) : गाँठ_2(initialHash) {}
    रक्षित:
        int32_t first_Edge_संख्या;
    };

    /**
     * @internal 
     */
    class List_Branch_गाँठ : खुला Branch_गाँठ {
    खुला:
        List_Branch_गाँठ() : Branch_गाँठ(0x444444), लंबाई(0) {}
        भव bool चालक==(स्थिर गाँठ_2 &other) स्थिर override;
        भव int32_t mark_दायाँ_Edges_First(int32_t edge_संख्या) override;
        भव व्योम ग_लिखो(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        व्योम add(int32_t c, int32_t मूल्य_2) {
            units[लंबाई] = static_cast<char16_t>(c);
            equal[लंबाई]=nullptr;
            मूल्यावली[लंबाई]=मूल्य_2;
            ++लंबाई;
            hash=(hash*37u+c)*37u+मूल्य_2;
        }
        // Adds a unit which leads to another match node.
        व्योम add(int32_t c, गाँठ_2 *गाँठ) {
            units[लंबाई] = static_cast<char16_t>(c);
            equal[लंबाई]=गाँठ;
            मूल्यावली[लंबाई]=0;
            ++लंबाई;
            hash=(hash*37u+c)*37u+hashCode(गाँठ);
        }
    रक्षित:
        गाँठ_2 *equal[k_Max_Branch_Linear_Sub_गाँठ_लंबाई];  // nullptr means "has final value".
        int32_t लंबाई;
        int32_t मूल्यावली[k_Max_Branch_Linear_Sub_गाँठ_लंबाई];
        char16_t units[k_Max_Branch_Linear_Sub_गाँठ_लंबाई];
    };

    /**
     * @internal 
     */
    class Split_Branch_गाँठ : खुला Branch_गाँठ {
    खुला:
        Split_Branch_गाँठ(char16_t मध्य_Unit, गाँठ_2 *less_Than_गाँठ, गाँठ_2 *greater_Or_Equal_गाँठ)
                : Branch_गाँठ(((0x555555u*37u+मध्य_Unit)*37u+
                              hashCode(less_Than_गाँठ))*37u+hashCode(greater_Or_Equal_गाँठ)),
                  unit(मध्य_Unit), lessThan(less_Than_गाँठ), greaterOrEqual(greater_Or_Equal_गाँठ) {}
        भव bool चालक==(स्थिर गाँठ_2 &other) स्थिर override;
        भव int32_t mark_दायाँ_Edges_First(int32_t edge_संख्या) override;
        भव व्योम ग_लिखो(StringTrieBuilder &builder) override;
    रक्षित:
        char16_t unit;
        गाँठ_2 *lessThan;
        गाँठ_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_शीर्ष_गाँठ : खुला मूल्य_गाँठ {
    खुला:
        Branch_शीर्ष_गाँठ(int32_t len, गाँठ_2 *sub_गाँठ)
                : मूल्य_गाँठ((0x666666u*37u+len)*37u+hashCode(sub_गाँठ)),
                  लंबाई(len), अग्र(sub_गाँठ) {}
        भव bool चालक==(स्थिर गाँठ_2 &other) स्थिर override;
        भव int32_t mark_दायाँ_Edges_First(int32_t edge_संख्या) override;
        भव व्योम ग_लिखो(StringTrieBuilder &builder) override;
    रक्षित:
        int32_t लंबाई;
        गाँठ_2 *अग्र;  // A branch sub-node.
    };

#पूर्ण_अगर  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    भव गाँठ_2 *create_Linear_Match_गाँठ(int32_t i, int32_t unit_सूचकांक, int32_t लंबाई,
                                        गाँठ_2 *अग्र_गाँठ) स्थिर = 0;

    /** @internal */
    भव int32_t ग_लिखो(int32_t unit) = 0;
    /** @internal */
    भव int32_t writeElementUnits(int32_t i, int32_t unit_सूचकांक, int32_t लंबाई) = 0;
    /** @internal */
    भव int32_t write_मूल्य_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    भव int32_t write_मूल्य_And_Type(UBool has_मूल्य, int32_t मूल्य_2, int32_t गाँठ) = 0;
    /** @internal */
    भव int32_t writeDeltaTo(int32_t jumpTarget) = 0;
};

U_NAMESPACE_END

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */

#पूर्ण_अगर  // __STRINGTRIEBUILDER_H__
