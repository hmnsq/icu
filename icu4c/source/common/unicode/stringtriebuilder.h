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

#جے_بنا_تعریف __STRINGTRIEBUILDER_H__
#تعریف_کرو __STRINGTRIEBUILDER_H__

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
#جے_بنا_تعریف U_HIDE_اندرونی_API
    /** @internal */
    ساكن int32_t hash_گرہ(ثابت فراغ *گرہ);
    /** @internal */
    ساكن UBool equalNodes(ثابت فراغ *بایاں, ثابت فراغ *دایاں);
#جے_انت  /* U_HIDE_INTERNAL_API */

محمي:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    مجازی ~StringTrieBuilder();

#جے_بنا_تعریف U_HIDE_اندرونی_API
    /** @internal */
    فراغ createCompactBuilder(int32_t ناپ_Guess, UError_کوڈ &error_کوڈ);
    /** @internal */
    فراغ deleteCompactBuilder();

    /** @internal */
    فراغ build(UStringTrieBuildOption buildOption, int32_t elements_لمائی, UError_کوڈ &error_کوڈ);

    /** @internal */
    int32_t write_گرہ(int32_t شروع, int32_t حد_بندی, int32_t unit_اشاریہ);
    /** @internal */
    int32_t write_Branch_Sub_گرہ(int32_t شروع, int32_t حد_بندی, int32_t unit_اشاریہ, int32_t لمائی);
#جے_انت  /* U_HIDE_INTERNAL_API */

    class گرہ_2;

#جے_بنا_تعریف U_HIDE_اندرونی_API
    /** @internal */
    گرہ_2* make_گرہ(int32_t شروع,
                   int32_t حد_بندی,
                   int32_t unit_اشاریہ,
                   UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND;
    /** @internal */
    گرہ_2* make_Branch_Sub_گرہ(int32_t شروع,
                            int32_t حد_بندی,
                            int32_t unit_اشاریہ,
                            int32_t لمائی,
                            UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND;
#جے_انت  /* U_HIDE_INTERNAL_API */

    /** @internal */
    مجازی int32_t get_Element_String_لمائی(int32_t i) ثابت = 0;
    /** @internal */
    مجازی char16_t getElementUnit(int32_t i, int32_t unit_اشاریہ) ثابت = 0;
    /** @internal */
    مجازی int32_t get_Element_قدر(int32_t i) ثابت = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    مجازی int32_t get_حد_بندی_دا_خطی_میل(int32_t پہلا, int32_t آخری, int32_t unit_اشاریہ) ثابت = 0;

    // Number of different units at unitIndex.
    /** @internal */
    مجازی int32_t گنتی_Element_Units(int32_t شروع, int32_t حد_بندی, int32_t unit_اشاریہ) ثابت = 0;
    /** @internal */
    مجازی int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_اشاریہ, int32_t گنتی) ثابت = 0;
    /** @internal */
    مجازی int32_t اشاریہ_دا_Element_With_اگلا_Unit(int32_t i, int32_t unit_اشاریہ, char16_t unit) ثابت = 0;

    /** @internal */
    مجازی UBool میل_Nodes_Can_Have_اقدار() ثابت = 0;

    /** @internal */
    مجازی int32_t get_Max_Branch_خطی_Sub_گرہ_لمائی() ثابت = 0;
    /** @internal */
    مجازی int32_t get_Min_خطی_میل() ثابت = 0;
    /** @internal */
    مجازی int32_t get_Max_خطی_میل_لمائی() ثابت = 0;

#جے_بنا_تعریف U_HIDE_اندرونی_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    ساكن ثابت int32_t k_Max_Branch_خطی_Sub_گرہ_لمائی=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    ساكن ثابت int32_t k_Max_ونڈو_Branch_پدھراں=14;

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
    گرہ_2* رجسٹر_گرہ(گرہ_2* new_گرہ, UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND;
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
    گرہ_2* رجسٹر_Final_قدر(int32_t قدر_2, UError_کوڈ& error_کوڈ) U_LIFETIME_BOUND;
#جے_انت  /* U_HIDE_INTERNAL_API */

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
    class گرہ_2 : عام UObject {
    عام:
        گرہ_2(int32_t initialHash) : hash(initialHash), وتھ(0) {}
        مضمن int32_t hash_کوڈ() ثابت { أرجع hash; }
        // Handles node==nullptr.
        ساكن مضمن int32_t hash_کوڈ(ثابت گرہ_2 *گرہ) { أرجع گرہ==nullptr ? 0 : گرہ->hash_کوڈ(); }
        // Base class operator==() compares the actual class types.
        مجازی bool عامل==(ثابت گرہ_2 &other) ثابت;
        مضمن bool عامل!=(ثابت گرہ_2 &other) ثابت { أرجع !عامل==(other); }
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
        مجازی int32_t mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر);
        // write() must set the offset to a positive value.
        مجازی فراغ اكتب(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        مضمن فراغ write_Unless_Inside_دایاں_کنارا(int32_t پہلا_دایاں, int32_t آخری_دایاں,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            إذا(وتھ<0 && (وتھ<آخری_دایاں || پہلا_دایاں<وتھ)) {
                اكتب(builder);
            }
        }
        مضمن int32_t get_وتھ() ثابت { أرجع وتھ; }
    محمي:
        int32_t hash;
        int32_t وتھ;
    };

#جے_بنا_تعریف U_HIDE_اندرونی_API
    // This class should not be overridden because
    // registerFinalValue() compares a stack-allocated FinalValueNode
    // (stack-allocated so that we don't unnecessarily create lots of duplicate nodes)
    // with the input node, and the
    // !Node::operator==(other) used inside FinalValueNode::operator==(other)
    // will be false if the typeid's are different.
    /** @internal */
    class Final_قدر_گرہ : عام گرہ_2 {
    عام:
        Final_قدر_گرہ(int32_t v) : گرہ_2(0x111111u*37u+v), قدر_2(v) {}
        مجازی bool عامل==(ثابت گرہ_2 &other) ثابت override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t قدر_2;
    };
#جے_انت  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class قدر_گرہ : عام گرہ_2 {
    عام:
        قدر_گرہ(int32_t initialHash) : گرہ_2(initialHash), has_قدر(false), قدر_2(0) {}
        مجازی bool عامل==(ثابت گرہ_2 &other) ثابت override;
        فراغ سیٹ_قدر(int32_t v) {
            has_قدر=true;
            قدر_2=v;
            hash=hash*37u+v;
        }
    محمي:
        UBool has_قدر;
        int32_t قدر_2;
    };

#جے_بنا_تعریف U_HIDE_اندرونی_API
    /** 
     * @internal 
     */
    class Intermediate_قدر_گرہ : عام قدر_گرہ {
    عام:
        Intermediate_قدر_گرہ(int32_t v, گرہ_2 *اگلا_گرہ)
                : قدر_گرہ(0x222222u*37u+hash_کوڈ(اگلا_گرہ)), اگلا(اگلا_گرہ) { سیٹ_قدر(v); }
        مجازی bool عامل==(ثابت گرہ_2 &other) ثابت override;
        مجازی int32_t mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر) override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        گرہ_2 *اگلا;
    };
#جے_انت  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class خطی_میل_گرہ : عام قدر_گرہ {
    عام:
        خطی_میل_گرہ(int32_t len, گرہ_2 *اگلا_گرہ)
                : قدر_گرہ((0x333333u*37u+len)*37u+hash_کوڈ(اگلا_گرہ)),
                  لمائی(len), اگلا(اگلا_گرہ) {}
        مجازی bool عامل==(ثابت گرہ_2 &other) ثابت override;
        مجازی int32_t mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر) override;
    محمي:
        int32_t لمائی;
        گرہ_2 *اگلا;
    };

#جے_بنا_تعریف U_HIDE_اندرونی_API
    /**
     * @internal 
     */
    class Branch_گرہ : عام گرہ_2 {
    عام:
        Branch_گرہ(int32_t initialHash) : گرہ_2(initialHash) {}
    محمي:
        int32_t پہلا_کنارا_نمبر;
    };

    /**
     * @internal 
     */
    class List_Branch_گرہ : عام Branch_گرہ {
    عام:
        List_Branch_گرہ() : Branch_گرہ(0x444444), لمائی(0) {}
        مجازی bool عامل==(ثابت گرہ_2 &other) ثابت override;
        مجازی int32_t mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر) override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        فراغ جوڑو(int32_t c, int32_t قدر_2) {
            units[لمائی] = static_cast<char16_t>(c);
            equal[لمائی]=nullptr;
            اقدار[لمائی]=قدر_2;
            ++لمائی;
            hash=(hash*37u+c)*37u+قدر_2;
        }
        // Adds a unit which leads to another match node.
        فراغ جوڑو(int32_t c, گرہ_2 *گرہ) {
            units[لمائی] = static_cast<char16_t>(c);
            equal[لمائی]=گرہ;
            اقدار[لمائی]=0;
            ++لمائی;
            hash=(hash*37u+c)*37u+hash_کوڈ(گرہ);
        }
    محمي:
        گرہ_2 *equal[k_Max_Branch_خطی_Sub_گرہ_لمائی];  // nullptr means "has final value".
        int32_t لمائی;
        int32_t اقدار[k_Max_Branch_خطی_Sub_گرہ_لمائی];
        char16_t units[k_Max_Branch_خطی_Sub_گرہ_لمائی];
    };

    /**
     * @internal 
     */
    class ونڈو_Branch_گرہ : عام Branch_گرہ {
    عام:
        ونڈو_Branch_گرہ(char16_t درمیان_Unit, گرہ_2 *less_Than_گرہ, گرہ_2 *greater_Or_Equal_گرہ)
                : Branch_گرہ(((0x555555u*37u+درمیان_Unit)*37u+
                              hash_کوڈ(less_Than_گرہ))*37u+hash_کوڈ(greater_Or_Equal_گرہ)),
                  unit(درمیان_Unit), lessThan(less_Than_گرہ), greaterOrEqual(greater_Or_Equal_گرہ) {}
        مجازی bool عامل==(ثابت گرہ_2 &other) ثابت override;
        مجازی int32_t mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر) override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        char16_t unit;
        گرہ_2 *lessThan;
        گرہ_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_سر_گرہ : عام قدر_گرہ {
    عام:
        Branch_سر_گرہ(int32_t len, گرہ_2 *sub_گرہ)
                : قدر_گرہ((0x666666u*37u+len)*37u+hash_کوڈ(sub_گرہ)),
                  لمائی(len), اگلا(sub_گرہ) {}
        مجازی bool عامل==(ثابت گرہ_2 &other) ثابت override;
        مجازی int32_t mark_دایاں_کنارے_پہلا(int32_t کنارا_نمبر) override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t لمائی;
        گرہ_2 *اگلا;  // A branch sub-node.
    };

#جے_انت  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    مجازی گرہ_2 *create_خطی_میل_گرہ(int32_t i, int32_t unit_اشاریہ, int32_t لمائی,
                                        گرہ_2 *اگلا_گرہ) ثابت = 0;

    /** @internal */
    مجازی int32_t اكتب(int32_t unit) = 0;
    /** @internal */
    مجازی int32_t writeElementUnits(int32_t i, int32_t unit_اشاریہ, int32_t لمائی) = 0;
    /** @internal */
    مجازی int32_t write_قدر_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    مجازی int32_t write_قدر_And_Type(UBool has_قدر, int32_t قدر_2, int32_t گرہ) = 0;
    /** @internal */
    مجازی int32_t write_Delta_منزل(int32_t jump_نشانہ) = 0;
};

U_NAMESPACE_سرا

#جے_انت /* U_SHOW_CPLUSPLUS_API */

#جے_انت  // __STRINGTRIEBUILDER_H__
