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

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

#ضمن "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
بنية UHashtable;
تعريفنوع بنية UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
تعداد UStringTrieBuildOption {
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
class U_COMMON_API StringTrieBuilder : عام UObject {
عام:
#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    ساكن int32_t hash_گرێ(ثابت فراغ *گرێ);
    /** @internal */
    ساكن UBool equalNodes(ثابت فراغ *چەپ, ثابت فراغ *ڕاست);
#endif  /* U_HIDE_INTERNAL_API */

محمي:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    virtual ~StringTrieBuilder();

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    فراغ createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode);
    /** @internal */
    فراغ deleteCompactBuilder();

    /** @internal */
    فراغ build(UStringTrieBuildOption buildOption, int32_t elementsLength, UErrorCode &errorCode);

    /** @internal */
    int32_t write_گرێ(int32_t start, int32_t limit, int32_t unitIndex);
    /** @internal */
    int32_t write_Branch_Sub_گرێ(int32_t start, int32_t limit, int32_t unitIndex, int32_t length);
#endif  /* U_HIDE_INTERNAL_API */

    class گرێ_2;

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    گرێ_2* make_گرێ(int32_t start,
                   int32_t limit,
                   int32_t unitIndex,
                   UErrorCode& errorCode) U_LIFETIME_BOUND;
    /** @internal */
    گرێ_2* make_Branch_Sub_گرێ(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            int32_t length,
                            UErrorCode& errorCode) U_LIFETIME_BOUND;
#endif  /* U_HIDE_INTERNAL_API */

    /** @internal */
    virtual int32_t getElementStringLength(int32_t i) ثابت = 0;
    /** @internal */
    virtual char16_t getElementUnit(int32_t i, int32_t unitIndex) ثابت = 0;
    /** @internal */
    virtual int32_t get_Element_بەها(int32_t i) ثابت = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    virtual int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ثابت = 0;

    // Number of different units at unitIndex.
    /** @internal */
    virtual int32_t ژماردن_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ثابت = 0;
    /** @internal */
    virtual int32_t skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t ژماردن) ثابت = 0;
    /** @internal */
    virtual int32_t indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ثابت = 0;

    /** @internal */
    virtual UBool match_Nodes_Can_Have_بەهاکان() ثابت = 0;

    /** @internal */
    virtual int32_t get_Max_Branch_Linear_Sub_گرێ_Length() ثابت = 0;
    /** @internal */
    virtual int32_t getMinLinearMatch() ثابت = 0;
    /** @internal */
    virtual int32_t getMaxLinearMatchLength() ثابت = 0;

#ifndef U_HIDE_INTERNAL_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    ساكن ثابت int32_t k_Max_Branch_Linear_Sub_گرێ_Length=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    ساكن ثابت int32_t kMaxSplitBranchLevels=14;

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
    گرێ_2* register_گرێ(گرێ_2* new_گرێ, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    گرێ_2* register_Final_بەها(int32_t بەها_2, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    class گرێ_2 : عام UObject {
    عام:
        گرێ_2(int32_t initialHash) : hash(initialHash), offset(0) {}
        مضمن int32_t hashCode() ثابت { أرجع hash; }
        // Handles node==nullptr.
        ساكن مضمن int32_t hashCode(ثابت گرێ_2 *گرێ) { أرجع گرێ==nullptr ? 0 : گرێ->hashCode(); }
        // Base class operator==() compares the actual class types.
        virtual bool operator==(ثابت گرێ_2 &other) ثابت;
        مضمن bool operator!=(ثابت گرێ_2 &other) ثابت { أرجع !operator==(other); }
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
        virtual int32_t mark_ڕاست_Edges_First(int32_t edge_ژمار);
        // write() must set the offset to a positive value.
        virtual فراغ اكتب(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        مضمن فراغ write_Unless_Inside_ڕاست_Edge(int32_t first_ڕاست, int32_t last_ڕاست,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            إذا(offset<0 && (offset<last_ڕاست || first_ڕاست<offset)) {
                اكتب(builder);
            }
        }
        مضمن int32_t getOffset() ثابت { أرجع offset; }
    محمي:
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
    class Final_بەها_گرێ : عام گرێ_2 {
    عام:
        Final_بەها_گرێ(int32_t v) : گرێ_2(0x111111u*37u+v), بەها_2(v) {}
        virtual bool operator==(ثابت گرێ_2 &other) ثابت override;
        virtual فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t بەها_2;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class بەها_گرێ : عام گرێ_2 {
    عام:
        بەها_گرێ(int32_t initialHash) : گرێ_2(initialHash), has_بەها(false), بەها_2(0) {}
        virtual bool operator==(ثابت گرێ_2 &other) ثابت override;
        فراغ set_بەها(int32_t v) {
            has_بەها=true;
            بەها_2=v;
            hash=hash*37u+v;
        }
    محمي:
        UBool has_بەها;
        int32_t بەها_2;
    };

#ifndef U_HIDE_INTERNAL_API
    /** 
     * @internal 
     */
    class Intermediate_بەها_گرێ : عام بەها_گرێ {
    عام:
        Intermediate_بەها_گرێ(int32_t v, گرێ_2 *next_گرێ)
                : بەها_گرێ(0x222222u*37u+hashCode(next_گرێ)), next(next_گرێ) { set_بەها(v); }
        virtual bool operator==(ثابت گرێ_2 &other) ثابت override;
        virtual int32_t mark_ڕاست_Edges_First(int32_t edge_ژمار) override;
        virtual فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        گرێ_2 *next;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class Linear_Match_گرێ : عام بەها_گرێ {
    عام:
        Linear_Match_گرێ(int32_t len, گرێ_2 *next_گرێ)
                : بەها_گرێ((0x333333u*37u+len)*37u+hashCode(next_گرێ)),
                  length(len), next(next_گرێ) {}
        virtual bool operator==(ثابت گرێ_2 &other) ثابت override;
        virtual int32_t mark_ڕاست_Edges_First(int32_t edge_ژمار) override;
    محمي:
        int32_t length;
        گرێ_2 *next;
    };

#ifndef U_HIDE_INTERNAL_API
    /**
     * @internal 
     */
    class Branch_گرێ : عام گرێ_2 {
    عام:
        Branch_گرێ(int32_t initialHash) : گرێ_2(initialHash) {}
    محمي:
        int32_t first_Edge_ژمار;
    };

    /**
     * @internal 
     */
    class List_Branch_گرێ : عام Branch_گرێ {
    عام:
        List_Branch_گرێ() : Branch_گرێ(0x444444), length(0) {}
        virtual bool operator==(ثابت گرێ_2 &other) ثابت override;
        virtual int32_t mark_ڕاست_Edges_First(int32_t edge_ژمار) override;
        virtual فراغ اكتب(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        فراغ add(int32_t c, int32_t بەها_2) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=nullptr;
            بەهاکان[length]=بەها_2;
            ++length;
            hash=(hash*37u+c)*37u+بەها_2;
        }
        // Adds a unit which leads to another match node.
        فراغ add(int32_t c, گرێ_2 *گرێ) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=گرێ;
            بەهاکان[length]=0;
            ++length;
            hash=(hash*37u+c)*37u+hashCode(گرێ);
        }
    محمي:
        گرێ_2 *equal[k_Max_Branch_Linear_Sub_گرێ_Length];  // nullptr means "has final value".
        int32_t length;
        int32_t بەهاکان[k_Max_Branch_Linear_Sub_گرێ_Length];
        char16_t units[k_Max_Branch_Linear_Sub_گرێ_Length];
    };

    /**
     * @internal 
     */
    class Split_Branch_گرێ : عام Branch_گرێ {
    عام:
        Split_Branch_گرێ(char16_t ناوەڕاست_Unit, گرێ_2 *less_Than_گرێ, گرێ_2 *greater_Or_Equal_گرێ)
                : Branch_گرێ(((0x555555u*37u+ناوەڕاست_Unit)*37u+
                              hashCode(less_Than_گرێ))*37u+hashCode(greater_Or_Equal_گرێ)),
                  unit(ناوەڕاست_Unit), lessThan(less_Than_گرێ), greaterOrEqual(greater_Or_Equal_گرێ) {}
        virtual bool operator==(ثابت گرێ_2 &other) ثابت override;
        virtual int32_t mark_ڕاست_Edges_First(int32_t edge_ژمار) override;
        virtual فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        char16_t unit;
        گرێ_2 *lessThan;
        گرێ_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_Head_گرێ : عام بەها_گرێ {
    عام:
        Branch_Head_گرێ(int32_t len, گرێ_2 *sub_گرێ)
                : بەها_گرێ((0x666666u*37u+len)*37u+hashCode(sub_گرێ)),
                  length(len), next(sub_گرێ) {}
        virtual bool operator==(ثابت گرێ_2 &other) ثابت override;
        virtual int32_t mark_ڕاست_Edges_First(int32_t edge_ژمار) override;
        virtual فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t length;
        گرێ_2 *next;  // A branch sub-node.
    };

#endif  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    virtual گرێ_2 *create_Linear_Match_گرێ(int32_t i, int32_t unitIndex, int32_t length,
                                        گرێ_2 *next_گرێ) ثابت = 0;

    /** @internal */
    virtual int32_t اكتب(int32_t unit) = 0;
    /** @internal */
    virtual int32_t writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) = 0;
    /** @internal */
    virtual int32_t write_بەها_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    virtual int32_t write_بەها_And_Type(UBool has_بەها, int32_t بەها_2, int32_t گرێ) = 0;
    /** @internal */
    virtual int32_t writeDeltaTo(int32_t jumpTarget) = 0;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __STRINGTRIEBUILDER_H__
