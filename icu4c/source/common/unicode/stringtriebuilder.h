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
    ساكن int32_t hash_تۈگۈن(ثابت فراغ *تۈگۈن);
    /** @internal */
    ساكن UBool equalNodes(ثابت فراغ *سول, ثابت فراغ *ئوڭ);
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
    int32_t write_تۈگۈن(int32_t start, int32_t limit, int32_t unitIndex);
    /** @internal */
    int32_t write_Branch_Sub_تۈگۈن(int32_t start, int32_t limit, int32_t unitIndex, int32_t length);
#endif  /* U_HIDE_INTERNAL_API */

    class تۈگۈن_2;

#ifndef U_HIDE_INTERNAL_API
    /** @internal */
    تۈگۈن_2* make_تۈگۈن(int32_t start,
                   int32_t limit,
                   int32_t unitIndex,
                   UErrorCode& errorCode) U_LIFETIME_BOUND;
    /** @internal */
    تۈگۈن_2* make_Branch_Sub_تۈگۈن(int32_t start,
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
    virtual int32_t get_Element_قىممەت(int32_t i) ثابت = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    virtual int32_t getLimitOfLinearMatch(int32_t first, int32_t last, int32_t unitIndex) ثابت = 0;

    // Number of different units at unitIndex.
    /** @internal */
    virtual int32_t سانى_Element_Units(int32_t start, int32_t limit, int32_t unitIndex) ثابت = 0;
    /** @internal */
    virtual int32_t skipElementsBySomeUnits(int32_t i, int32_t unitIndex, int32_t سانى) ثابت = 0;
    /** @internal */
    virtual int32_t indexOfElementWithNextUnit(int32_t i, int32_t unitIndex, char16_t unit) ثابت = 0;

    /** @internal */
    virtual UBool match_Nodes_Can_Have_قىممەتلەر() ثابت = 0;

    /** @internal */
    virtual int32_t get_Max_Branch_Linear_Sub_تۈگۈن_Length() ثابت = 0;
    /** @internal */
    virtual int32_t getMinLinearMatch() ثابت = 0;
    /** @internal */
    virtual int32_t getMaxLinearMatchLength() ثابت = 0;

#ifndef U_HIDE_INTERNAL_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    ساكن ثابت int32_t k_Max_Branch_Linear_Sub_تۈگۈن_Length=5;

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
    تۈگۈن_2* register_تۈگۈن(تۈگۈن_2* new_تۈگۈن, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    تۈگۈن_2* register_Final_قىممەت(int32_t قىممەت_2, UErrorCode& errorCode) U_LIFETIME_BOUND;
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
    class تۈگۈن_2 : عام UObject {
    عام:
        تۈگۈن_2(int32_t initialHash) : hash(initialHash), offset(0) {}
        مضمن int32_t hashCode() ثابت { أرجع hash; }
        // Handles node==nullptr.
        ساكن مضمن int32_t hashCode(ثابت تۈگۈن_2 *تۈگۈن) { أرجع تۈگۈن==nullptr ? 0 : تۈگۈن->hashCode(); }
        // Base class operator==() compares the actual class types.
        virtual bool operator==(ثابت تۈگۈن_2 &other) ثابت;
        مضمن bool operator!=(ثابت تۈگۈن_2 &other) ثابت { أرجع !operator==(other); }
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
        virtual int32_t mark_ئوڭ_Edges_First(int32_t edge_سان);
        // write() must set the offset to a positive value.
        virtual فراغ اكتب(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        مضمن فراغ write_Unless_Inside_ئوڭ_Edge(int32_t first_ئوڭ, int32_t last_ئوڭ,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            إذا(offset<0 && (offset<last_ئوڭ || first_ئوڭ<offset)) {
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
    class Final_قىممەت_تۈگۈن : عام تۈگۈن_2 {
    عام:
        Final_قىممەت_تۈگۈن(int32_t v) : تۈگۈن_2(0x111111u*37u+v), قىممەت_2(v) {}
        virtual bool operator==(ثابت تۈگۈن_2 &other) ثابت override;
        virtual فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t قىممەت_2;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class قىممەت_تۈگۈن : عام تۈگۈن_2 {
    عام:
        قىممەت_تۈگۈن(int32_t initialHash) : تۈگۈن_2(initialHash), has_قىممەت(false), قىممەت_2(0) {}
        virtual bool operator==(ثابت تۈگۈن_2 &other) ثابت override;
        فراغ set_قىممەت(int32_t v) {
            has_قىممەت=true;
            قىممەت_2=v;
            hash=hash*37u+v;
        }
    محمي:
        UBool has_قىممەت;
        int32_t قىممەت_2;
    };

#ifndef U_HIDE_INTERNAL_API
    /** 
     * @internal 
     */
    class Intermediate_قىممەت_تۈگۈن : عام قىممەت_تۈگۈن {
    عام:
        Intermediate_قىممەت_تۈگۈن(int32_t v, تۈگۈن_2 *next_تۈگۈن)
                : قىممەت_تۈگۈن(0x222222u*37u+hashCode(next_تۈگۈن)), next(next_تۈگۈن) { set_قىممەت(v); }
        virtual bool operator==(ثابت تۈگۈن_2 &other) ثابت override;
        virtual int32_t mark_ئوڭ_Edges_First(int32_t edge_سان) override;
        virtual فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        تۈگۈن_2 *next;
    };
#endif  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class Linear_Match_تۈگۈن : عام قىممەت_تۈگۈن {
    عام:
        Linear_Match_تۈگۈن(int32_t len, تۈگۈن_2 *next_تۈگۈن)
                : قىممەت_تۈگۈن((0x333333u*37u+len)*37u+hashCode(next_تۈگۈن)),
                  length(len), next(next_تۈگۈن) {}
        virtual bool operator==(ثابت تۈگۈن_2 &other) ثابت override;
        virtual int32_t mark_ئوڭ_Edges_First(int32_t edge_سان) override;
    محمي:
        int32_t length;
        تۈگۈن_2 *next;
    };

#ifndef U_HIDE_INTERNAL_API
    /**
     * @internal 
     */
    class Branch_تۈگۈن : عام تۈگۈن_2 {
    عام:
        Branch_تۈگۈن(int32_t initialHash) : تۈگۈن_2(initialHash) {}
    محمي:
        int32_t first_Edge_سان;
    };

    /**
     * @internal 
     */
    class List_Branch_تۈگۈن : عام Branch_تۈگۈن {
    عام:
        List_Branch_تۈگۈن() : Branch_تۈگۈن(0x444444), length(0) {}
        virtual bool operator==(ثابت تۈگۈن_2 &other) ثابت override;
        virtual int32_t mark_ئوڭ_Edges_First(int32_t edge_سان) override;
        virtual فراغ اكتب(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        فراغ add(int32_t c, int32_t قىممەت_2) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=nullptr;
            قىممەتلەر[length]=قىممەت_2;
            ++length;
            hash=(hash*37u+c)*37u+قىممەت_2;
        }
        // Adds a unit which leads to another match node.
        فراغ add(int32_t c, تۈگۈن_2 *تۈگۈن) {
            units[length] = static_cast<char16_t>(c);
            equal[length]=تۈگۈن;
            قىممەتلەر[length]=0;
            ++length;
            hash=(hash*37u+c)*37u+hashCode(تۈگۈن);
        }
    محمي:
        تۈگۈن_2 *equal[k_Max_Branch_Linear_Sub_تۈگۈن_Length];  // nullptr means "has final value".
        int32_t length;
        int32_t قىممەتلەر[k_Max_Branch_Linear_Sub_تۈگۈن_Length];
        char16_t units[k_Max_Branch_Linear_Sub_تۈگۈن_Length];
    };

    /**
     * @internal 
     */
    class Split_Branch_تۈگۈن : عام Branch_تۈگۈن {
    عام:
        Split_Branch_تۈگۈن(char16_t ئوتتۇرا_Unit, تۈگۈن_2 *less_Than_تۈگۈن, تۈگۈن_2 *greater_Or_Equal_تۈگۈن)
                : Branch_تۈگۈن(((0x555555u*37u+ئوتتۇرا_Unit)*37u+
                              hashCode(less_Than_تۈگۈن))*37u+hashCode(greater_Or_Equal_تۈگۈن)),
                  unit(ئوتتۇرا_Unit), lessThan(less_Than_تۈگۈن), greaterOrEqual(greater_Or_Equal_تۈگۈن) {}
        virtual bool operator==(ثابت تۈگۈن_2 &other) ثابت override;
        virtual int32_t mark_ئوڭ_Edges_First(int32_t edge_سان) override;
        virtual فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        char16_t unit;
        تۈگۈن_2 *lessThan;
        تۈگۈن_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_Head_تۈگۈن : عام قىممەت_تۈگۈن {
    عام:
        Branch_Head_تۈگۈن(int32_t len, تۈگۈن_2 *sub_تۈگۈن)
                : قىممەت_تۈگۈن((0x666666u*37u+len)*37u+hashCode(sub_تۈگۈن)),
                  length(len), next(sub_تۈگۈن) {}
        virtual bool operator==(ثابت تۈگۈن_2 &other) ثابت override;
        virtual int32_t mark_ئوڭ_Edges_First(int32_t edge_سان) override;
        virtual فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t length;
        تۈگۈن_2 *next;  // A branch sub-node.
    };

#endif  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    virtual تۈگۈن_2 *create_Linear_Match_تۈگۈن(int32_t i, int32_t unitIndex, int32_t length,
                                        تۈگۈن_2 *next_تۈگۈن) ثابت = 0;

    /** @internal */
    virtual int32_t اكتب(int32_t unit) = 0;
    /** @internal */
    virtual int32_t writeElementUnits(int32_t i, int32_t unitIndex, int32_t length) = 0;
    /** @internal */
    virtual int32_t write_قىممەت_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    virtual int32_t write_قىممەت_And_Type(UBool has_قىممەت, int32_t قىممەت_2, int32_t تۈگۈن) = 0;
    /** @internal */
    virtual int32_t writeDeltaTo(int32_t jumpTarget) = 0;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __STRINGTRIEBUILDER_H__
