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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் U_SHOW_CPLUSPLUS_API

#அந்தர்பாவ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
கட்டமைப்பு UHashtable;
வகைவரை கட்டமைப்பு UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
எண்ணிக்கை UStringTrieBuildOption {
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
class U_COMMON_API StringTrieBuilder : பொது UObject {
பொது:
#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    நிலையான int32_t hash_முனை(மாறிலி வெறுமை *முனை);
    /** @internal */
    நிலையான UBool equalNodes(மாறிலி வெறுமை *இடது, மாறிலி வெறுமை *வலது);
#endif  /* U_HIDE_INTERNAL_API */

பாதுகாக்கப்பட்ட:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    மெய்நிகர் ~StringTrieBuilder();

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    வெறுமை createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode);
    /** @internal */
    வெறுமை deleteCompactBuilder();

    /** @internal */
    வெறுமை build(UStringTrieBuildOption buildOption, int32_t elementsLength, UErrorCode &errorCode);

    /** @internal */
    int32_t write_முனை(int32_t start, int32_t limit, int32_t unitIndex);
    /** @internal */
    int32_t write_Branch_Sub_முனை(int32_t start, int32_t limit, int32_t unitIndex, int32_t length);
#endif  /* U_HIDE_INTERNAL_API */

    class முனை_2;

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    முனை_2* make_முனை(int32_t start,
                   int32_t limit,
                   int32_t unitIndex,
                   UErrorCode& errorCode) U_LIFETIME_BOUND;
    /** @internal */
    முனை_2* make_Branch_Sub_முனை(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            int32_t length,
                            UErrorCode& errorCode) U_LIFETIME_BOUND;
#endif  /* U_HIDE_INTERNAL_API */

    /** @internal */
    மெய்நிகர் int32_t getElementStringLength(int32_t i) மாறிலி = 0;
    /** @internal */
    மெய்நிகர் char16_t getElementUnit(int32_t i, int32_t unitIndex) மாறிலி = 0;
    /** @internal */
    மெய்நிகர் int32_t get_Element_மதிப்பு(int32_t i) மாறிலி = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    மெய்நிகர் int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) மாறிலி = 0;

    // Number of different units at unitIndex.
    /** @internal */
    மெய்நிகர் int32_t மொத்தம்_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) மாறிலி = 0;
    /** @internal */
    மெய்நிகர் int32_t skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t மொத்தம்) மாறிலி = 0;
    /** @internal */
    மெய்நிகர் int32_t indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) மாறிலி = 0;

    /** @internal */
    மெய்நிகர் UBool match_Nodes_Can_Have_மதிப்புகள்() மாறிலி = 0;

    /** @internal */
    மெய்நிகர் int32_t get_Max_Branch_Linear_Sub_முனை_Length() மாறிலி = 0;
    /** @internal */
    மெய்நிகர் int32_t getMinLinearMatch() மாறிலி = 0;
    /** @internal */
    மெய்நிகர் int32_t getMaxLinearMatchLength() மாறிலி = 0;

#ifndef U_HIDE_INTERNAL_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    நிலையான மாறிலி int32_t k_Max_Branch_Linear_Sub_முனை_Length=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    நிலையான மாறிலி int32_t kMaxSplitBranchLevels=14;

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
    முனை_2* register_முனை(முனை_2* new_முனை, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    முனை_2* register_Final_மதிப்பு(int32_t மதிப்பு_2, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    class முனை_2 : பொது UObject {
    பொது:
        முனை_2(int32_t initialHash) : hash(initialHash), offset(0) {}
        உள்வரி int32_t hashCode() மாறிலி { திரும்பு hash; }
        // Handles node==nullptr.
        நிலையான உள்வரி int32_t hashCode(மாறிலி முனை_2 *முனை) { திரும்பு முனை==nullptr ? 0 : முனை->hashCode(); }
        // Base class operator==() compares the actual class types.
        மெய்நிகர் bool operator==(மாறிலி முனை_2 &other) மாறிலி;
        உள்வரி bool operator!=(மாறிலி முனை_2 &other) மாறிலி { திரும்பு !operator==(other); }
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
        மெய்நிகர் int32_t mark_வலது_Edges_First(int32_t edge_எண்);
        // write() must set the offset to a positive value.
        மெய்நிகர் வெறுமை லிக(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        உள்வரி வெறுமை write_Unless_Inside_வலது_Edge(int32_t first_வலது, int32_t last_வலது,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            எனில்(offset<0 && (offset<last_வலது || first_வலது<offset)) {
                லிக(builder);
            }
        }
        உள்வரி int32_t getOffset() மாறிலி { திரும்பு offset; }
    பாதுகாக்கப்பட்ட:
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
    class Final_மதிப்பு_முனை : பொது முனை_2 {
    பொது:
        Final_மதிப்பு_முனை(int32_t v) : முனை_2(0x111111u*37u+v), மதிப்பு_2(v) {}
        மெய்நிகர் bool operator==(மாறிலி முனை_2 &other) மாறிலி override;
        மெய்நிகர் வெறுமை லிக(StringTrieBuilder &builder) override;
    பாதுகாக்கப்பட்ட:
        int32_t மதிப்பு_2;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class மதிப்பு_முனை : பொது முனை_2 {
    பொது:
        மதிப்பு_முனை(int32_t initialHash) : முனை_2(initialHash), has_மதிப்பு(false), மதிப்பு_2(0) {}
        மெய்நிகர் bool operator==(மாறிலி முனை_2 &other) மாறிலி override;
        வெறுமை set_மதிப்பு(int32_t v) {
            has_மதிப்பு=true;
            மதிப்பு_2=v;
            hash=hash*37u+v;
        }
    பாதுகாக்கப்பட்ட:
        UBool has_மதிப்பு;
        int32_t மதிப்பு_2;
    };

#ifndef U_HIDE_INTERNAL_API
    /** 
     * @internal 
     */
    class Intermediate_மதிப்பு_முனை : பொது மதிப்பு_முனை {
    பொது:
        Intermediate_மதிப்பு_முனை(int32_t v, முனை_2 *next_முனை)
                : மதிப்பு_முனை(0x222222u*37u+hashCode(next_முனை)), next(next_முனை) { set_மதிப்பு(v); }
        மெய்நிகர் bool operator==(மாறிலி முனை_2 &other) மாறிலி override;
        மெய்நிகர் int32_t mark_வலது_Edges_First(int32_t edge_எண்) override;
        மெய்நிகர் வெறுமை லிக(StringTrieBuilder &builder) override;
    பாதுகாக்கப்பட்ட:
        முனை_2 *next;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class Linear_Match_முனை : பொது மதிப்பு_முனை {
    பொது:
        Linear_Match_முனை(int32_t len, முனை_2 *next_முனை)
                : மதிப்பு_முனை((0x333333u*37u+len)*37u+hashCode(next_முனை)),
                  length(len), next(next_முனை) {}
        மெய்நிகர் bool operator==(மாறிலி முனை_2 &other) மாறிலி override;
        மெய்நிகர் int32_t mark_வலது_Edges_First(int32_t edge_எண்) override;
    பாதுகாக்கப்பட்ட:
        int32_t length;
        முனை_2 *next;
    };

#ifndef U_HIDE_INTERNAL_API
    /**
     * @internal 
     */
    class Branch_முனை : பொது முனை_2 {
    பொது:
        Branch_முனை(int32_t initialHash) : முனை_2(initialHash) {}
    பாதுகாக்கப்பட்ட:
        int32_t first_Edge_எண்;
    };

    /**
     * @internal 
     */
    class List_Branch_முனை : பொது Branch_முனை {
    பொது:
        List_Branch_முனை() : Branch_முனை(0x444444), length(0) {}
        மெய்நிகர் bool operator==(மாறிலி முனை_2 &other) மாறிலி override;
        மெய்நிகர் int32_t mark_வலது_Edges_First(int32_t edge_எண்) override;
        மெய்நிகர் வெறுமை லிக(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        வெறுமை add(int32_t c, int32_t மதிப்பு_2) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=nullptr;
            மதிப்புகள்[length]=மதிப்பு_2;
            ++length;
            hash=(hash*37u+c)*37u+மதிப்பு_2;
        }
        // Adds a unit which leads to another match node.
        வெறுமை add(int32_t c, முனை_2 *முனை) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=முனை;
            மதிப்புகள்[length]=0;
            ++length;
            hash=(hash*37u+c)*37u+hashCode(முனை);
        }
    பாதுகாக்கப்பட்ட:
        முனை_2 *equal[k_Max_Branch_Linear_Sub_முனை_Length];  // nullptr means "has final value".
        int32_t length;
        int32_t மதிப்புகள்[k_Max_Branch_Linear_Sub_முனை_Length];
        char16_t units[k_Max_Branch_Linear_Sub_முனை_Length];
    };

    /**
     * @internal 
     */
    class Split_Branch_முனை : பொது Branch_முனை {
    பொது:
        Split_Branch_முனை(char16_t நடு_Unit, முனை_2 *less_Than_முனை, முனை_2 *greater_Or_Equal_முனை)
                : Branch_முனை(((0x555555u*37u+நடு_Unit)*37u+
                              hashCode(less_Than_முனை))*37u+hashCode(greater_Or_Equal_முனை)),
                  unit(நடு_Unit), lessThan(less_Than_முனை), greaterOrEqual(greater_Or_Equal_முனை) {}
        மெய்நிகர் bool operator==(மாறிலி முனை_2 &other) மாறிலி override;
        மெய்நிகர் int32_t mark_வலது_Edges_First(int32_t edge_எண்) override;
        மெய்நிகர் வெறுமை லிக(StringTrieBuilder &builder) override;
    பாதுகாக்கப்பட்ட:
        char16_t unit;
        முனை_2 *lessThan;
        முனை_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_Head_முனை : பொது மதிப்பு_முனை {
    பொது:
        Branch_Head_முனை(int32_t len, முனை_2 *sub_முனை)
                : மதிப்பு_முனை((0x666666u*37u+len)*37u+hashCode(sub_முனை)),
                  length(len), next(sub_முனை) {}
        மெய்நிகர் bool operator==(மாறிலி முனை_2 &other) மாறிலி override;
        மெய்நிகர் int32_t mark_வலது_Edges_First(int32_t edge_எண்) override;
        மெய்நிகர் வெறுமை லிக(StringTrieBuilder &builder) override;
    பாதுகாக்கப்பட்ட:
        int32_t length;
        முனை_2 *next;  // A branch sub-node.
    };

#endif  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    மெய்நிகர் முனை_2 *create_Linear_Match_முனை(int32_t i, int32_t unitIndex, int32_t length,
                                        முனை_2 *next_முனை) மாறிலி = 0;

    /** @internal */
    மெய்நிகர் int32_t லிக(int32_t unit) = 0;
    /** @internal */
    மெய்நிகர் int32_t writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) = 0;
    /** @internal */
    மெய்நிகர் int32_t write_மதிப்பு_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    மெய்நிகர் int32_t write_மதிப்பு_And_Type(UBool has_மதிப்பு, int32_t மதிப்பு_2, int32_t முனை) = 0;
    /** @internal */
    மெய்நிகர் int32_t writeDeltaTo(int32_t jumpTarget) = 0;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __STRINGTRIEBUILDER_H__
