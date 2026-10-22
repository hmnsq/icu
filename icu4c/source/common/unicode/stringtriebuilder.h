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

#جيڪڏهن_وصف_نه_ٿيل __STRINGTRIEBUILDER_H__
#وصف_ڪريو __STRINGTRIEBUILDER_H__

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
#جيڪڏهن_وصف_نه_ٿيل U_HIDE_اندروني_API
    /** @internal */
    ساكن int32_t hash_ڳنڍ(ثابت فراغ *ڳنڍ);
    /** @internal */
    ساكن UBool equalNodes(ثابت فراغ *کاٻو, ثابت فراغ *ساڄو);
#جيڪڏهن_پورو  /* U_HIDE_INTERNAL_API */

محمي:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    مجازي ~StringTrieBuilder();

#جيڪڏهن_وصف_نه_ٿيل U_HIDE_اندروني_API
    /** @internal */
    فراغ createCompactBuilder(int32_t سائيز_Guess, UError_ڪوڊ &error_ڪوڊ);
    /** @internal */
    فراغ deleteCompactBuilder();

    /** @internal */
    فراغ build(UStringTrieBuildOption buildOption, int32_t elements_ڊيگهائي, UError_ڪوڊ &error_ڪوڊ);

    /** @internal */
    int32_t write_ڳنڍ(int32_t شروعات, int32_t حد_بندي, int32_t unit_اشاريو);
    /** @internal */
    int32_t write_Branch_Sub_ڳنڍ(int32_t شروعات, int32_t حد_بندي, int32_t unit_اشاريو, int32_t ڊيگهائي);
#جيڪڏهن_پورو  /* U_HIDE_INTERNAL_API */

    class ڳنڍ_2;

#جيڪڏهن_وصف_نه_ٿيل U_HIDE_اندروني_API
    /** @internal */
    ڳنڍ_2* make_ڳنڍ(int32_t شروعات,
                   int32_t حد_بندي,
                   int32_t unit_اشاريو,
                   UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND;
    /** @internal */
    ڳنڍ_2* make_Branch_Sub_ڳنڍ(int32_t شروعات,
                            int32_t حد_بندي,
                            int32_t unit_اشاريو,
                            int32_t ڊيگهائي,
                            UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND;
#جيڪڏهن_پورو  /* U_HIDE_INTERNAL_API */

    /** @internal */
    مجازي int32_t get_Element_String_ڊيگهائي(int32_t i) ثابت = 0;
    /** @internal */
    مجازي char16_t getElementUnit(int32_t i, int32_t unit_اشاريو) ثابت = 0;
    /** @internal */
    مجازي int32_t get_Element_قدر(int32_t i) ثابت = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    مجازي int32_t get_حد_بندي_جو_لڪيري_ميلاپ(int32_t پهريون, int32_t آخري, int32_t unit_اشاريو) ثابت = 0;

    // Number of different units at unitIndex.
    /** @internal */
    مجازي int32_t ڳڻپ_Element_Units(int32_t شروعات, int32_t حد_بندي, int32_t unit_اشاريو) ثابت = 0;
    /** @internal */
    مجازي int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_اشاريو, int32_t ڳڻپ) ثابت = 0;
    /** @internal */
    مجازي int32_t اشاريو_جو_Element_With_ايندڙ_Unit(int32_t i, int32_t unit_اشاريو, char16_t unit) ثابت = 0;

    /** @internal */
    مجازي UBool ميلاپ_Nodes_Can_Have_قدرون() ثابت = 0;

    /** @internal */
    مجازي int32_t get_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي() ثابت = 0;
    /** @internal */
    مجازي int32_t get_Min_لڪيري_ميلاپ() ثابت = 0;
    /** @internal */
    مجازي int32_t get_Max_لڪيري_ميلاپ_ڊيگهائي() ثابت = 0;

#جيڪڏهن_وصف_نه_ٿيل U_HIDE_اندروني_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    ساكن ثابت int32_t k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    ساكن ثابت int32_t k_Max_ورهايو_Branch_سطحون=14;

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
    ڳنڍ_2* رجسٽر_ڳنڍ(ڳنڍ_2* new_ڳنڍ, UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND;
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
    ڳنڍ_2* رجسٽر_Final_قدر(int32_t قدر_2, UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND;
#جيڪڏهن_پورو  /* U_HIDE_INTERNAL_API */

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
    class ڳنڍ_2 : عام UObject {
    عام:
        ڳنڍ_2(int32_t initialHash) : hash(initialHash), وٿي(0) {}
        مضمن int32_t hash_ڪوڊ() ثابت { أرجع hash; }
        // Handles node==nullptr.
        ساكن مضمن int32_t hash_ڪوڊ(ثابت ڳنڍ_2 *ڳنڍ) { أرجع ڳنڍ==nullptr ? 0 : ڳنڍ->hash_ڪوڊ(); }
        // Base class operator==() compares the actual class types.
        مجازي bool عامل==(ثابت ڳنڍ_2 &other) ثابت;
        مضمن bool عامل!=(ثابت ڳنڍ_2 &other) ثابت { أرجع !عامل==(other); }
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
        مجازي int32_t mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد);
        // write() must set the offset to a positive value.
        مجازي فراغ اكتب(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        مضمن فراغ write_Unless_Inside_ساڄو_ڪنارو(int32_t پهريون_ساڄو, int32_t آخري_ساڄو,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            إذا(وٿي<0 && (وٿي<آخري_ساڄو || پهريون_ساڄو<وٿي)) {
                اكتب(builder);
            }
        }
        مضمن int32_t get_وٿي() ثابت { أرجع وٿي; }
    محمي:
        int32_t hash;
        int32_t وٿي;
    };

#جيڪڏهن_وصف_نه_ٿيل U_HIDE_اندروني_API
    // This class should not be overridden because
    // registerFinalValue() compares a stack-allocated FinalValueNode
    // (stack-allocated so that we don't unnecessarily create lots of duplicate nodes)
    // with the input node, and the
    // !Node::operator==(other) used inside FinalValueNode::operator==(other)
    // will be false if the typeid's are different.
    /** @internal */
    class Final_قدر_ڳنڍ : عام ڳنڍ_2 {
    عام:
        Final_قدر_ڳنڍ(int32_t v) : ڳنڍ_2(0x111111u*37u+v), قدر_2(v) {}
        مجازي bool عامل==(ثابت ڳنڍ_2 &other) ثابت override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t قدر_2;
    };
#جيڪڏهن_پورو  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class قدر_ڳنڍ : عام ڳنڍ_2 {
    عام:
        قدر_ڳنڍ(int32_t initialHash) : ڳنڍ_2(initialHash), has_قدر(false), قدر_2(0) {}
        مجازي bool عامل==(ثابت ڳنڍ_2 &other) ثابت override;
        فراغ سيٽ_قدر(int32_t v) {
            has_قدر=true;
            قدر_2=v;
            hash=hash*37u+v;
        }
    محمي:
        UBool has_قدر;
        int32_t قدر_2;
    };

#جيڪڏهن_وصف_نه_ٿيل U_HIDE_اندروني_API
    /** 
     * @internal 
     */
    class Intermediate_قدر_ڳنڍ : عام قدر_ڳنڍ {
    عام:
        Intermediate_قدر_ڳنڍ(int32_t v, ڳنڍ_2 *ايندڙ_ڳنڍ)
                : قدر_ڳنڍ(0x222222u*37u+hash_ڪوڊ(ايندڙ_ڳنڍ)), ايندڙ(ايندڙ_ڳنڍ) { سيٽ_قدر(v); }
        مجازي bool عامل==(ثابت ڳنڍ_2 &other) ثابت override;
        مجازي int32_t mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد) override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        ڳنڍ_2 *ايندڙ;
    };
#جيڪڏهن_پورو  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class لڪيري_ميلاپ_ڳنڍ : عام قدر_ڳنڍ {
    عام:
        لڪيري_ميلاپ_ڳنڍ(int32_t len, ڳنڍ_2 *ايندڙ_ڳنڍ)
                : قدر_ڳنڍ((0x333333u*37u+len)*37u+hash_ڪوڊ(ايندڙ_ڳنڍ)),
                  ڊيگهائي(len), ايندڙ(ايندڙ_ڳنڍ) {}
        مجازي bool عامل==(ثابت ڳنڍ_2 &other) ثابت override;
        مجازي int32_t mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد) override;
    محمي:
        int32_t ڊيگهائي;
        ڳنڍ_2 *ايندڙ;
    };

#جيڪڏهن_وصف_نه_ٿيل U_HIDE_اندروني_API
    /**
     * @internal 
     */
    class Branch_ڳنڍ : عام ڳنڍ_2 {
    عام:
        Branch_ڳنڍ(int32_t initialHash) : ڳنڍ_2(initialHash) {}
    محمي:
        int32_t پهريون_ڪنارو_عدد;
    };

    /**
     * @internal 
     */
    class List_Branch_ڳنڍ : عام Branch_ڳنڍ {
    عام:
        List_Branch_ڳنڍ() : Branch_ڳنڍ(0x444444), ڊيگهائي(0) {}
        مجازي bool عامل==(ثابت ڳنڍ_2 &other) ثابت override;
        مجازي int32_t mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد) override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        فراغ جوڙيو(int32_t c, int32_t قدر_2) {
            units[ڊيگهائي] = static_cast<char16_t>(c);
            equal[ڊيگهائي]=nullptr;
            قدرون[ڊيگهائي]=قدر_2;
            ++ڊيگهائي;
            hash=(hash*37u+c)*37u+قدر_2;
        }
        // Adds a unit which leads to another match node.
        فراغ جوڙيو(int32_t c, ڳنڍ_2 *ڳنڍ) {
            units[ڊيگهائي] = static_cast<char16_t>(c);
            equal[ڊيگهائي]=ڳنڍ;
            قدرون[ڊيگهائي]=0;
            ++ڊيگهائي;
            hash=(hash*37u+c)*37u+hash_ڪوڊ(ڳنڍ);
        }
    محمي:
        ڳنڍ_2 *equal[k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي];  // nullptr means "has final value".
        int32_t ڊيگهائي;
        int32_t قدرون[k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي];
        char16_t units[k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي];
    };

    /**
     * @internal 
     */
    class ورهايو_Branch_ڳنڍ : عام Branch_ڳنڍ {
    عام:
        ورهايو_Branch_ڳنڍ(char16_t وچ_Unit, ڳنڍ_2 *less_Than_ڳنڍ, ڳنڍ_2 *greater_Or_Equal_ڳنڍ)
                : Branch_ڳنڍ(((0x555555u*37u+وچ_Unit)*37u+
                              hash_ڪوڊ(less_Than_ڳنڍ))*37u+hash_ڪوڊ(greater_Or_Equal_ڳنڍ)),
                  unit(وچ_Unit), lessThan(less_Than_ڳنڍ), greaterOrEqual(greater_Or_Equal_ڳنڍ) {}
        مجازي bool عامل==(ثابت ڳنڍ_2 &other) ثابت override;
        مجازي int32_t mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد) override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        char16_t unit;
        ڳنڍ_2 *lessThan;
        ڳنڍ_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_سر_ڳنڍ : عام قدر_ڳنڍ {
    عام:
        Branch_سر_ڳنڍ(int32_t len, ڳنڍ_2 *sub_ڳنڍ)
                : قدر_ڳنڍ((0x666666u*37u+len)*37u+hash_ڪوڊ(sub_ڳنڍ)),
                  ڊيگهائي(len), ايندڙ(sub_ڳنڍ) {}
        مجازي bool عامل==(ثابت ڳنڍ_2 &other) ثابت override;
        مجازي int32_t mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد) override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t ڊيگهائي;
        ڳنڍ_2 *ايندڙ;  // A branch sub-node.
    };

#جيڪڏهن_پورو  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    مجازي ڳنڍ_2 *create_لڪيري_ميلاپ_ڳنڍ(int32_t i, int32_t unit_اشاريو, int32_t ڊيگهائي,
                                        ڳنڍ_2 *ايندڙ_ڳنڍ) ثابت = 0;

    /** @internal */
    مجازي int32_t اكتب(int32_t unit) = 0;
    /** @internal */
    مجازي int32_t writeElementUnits(int32_t i, int32_t unit_اشاريو, int32_t ڊيگهائي) = 0;
    /** @internal */
    مجازي int32_t write_قدر_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    مجازي int32_t write_قدر_And_Type(UBool has_قدر, int32_t قدر_2, int32_t ڳنڍ) = 0;
    /** @internal */
    مجازي int32_t write_Delta_منزل(int32_t jump_هدف) = 0;
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* U_SHOW_CPLUSPLUS_API */

#جيڪڏهن_پورو  // __STRINGTRIEBUILDER_H__
