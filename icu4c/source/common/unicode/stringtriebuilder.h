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

#که_نه_تعریف_شوی __STRINGTRIEBUILDER_H__
#تعریف_کړه __STRINGTRIEBUILDER_H__

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
#که_نه_تعریف_شوی U_HIDE_دننی_API
    /** @internal */
    ساكن int32_t hash_غوټه(ثابت فراغ *غوټه);
    /** @internal */
    ساكن UBool equalNodes(ثابت فراغ *کیڼ_2, ثابت فراغ *ښی_2);
#د_که_پای  /* U_HIDE_INTERNAL_API */

محمي:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    مجازي ~StringTrieBuilder();

#که_نه_تعریف_شوی U_HIDE_دننی_API
    /** @internal */
    فراغ createCompactBuilder(int32_t اندازه_Guess, UError_کوډ &error_کوډ);
    /** @internal */
    فراغ deleteCompactBuilder();

    /** @internal */
    فراغ build(UStringTrieBuildOption buildOption, int32_t elements_اوږدتیا, UError_کوډ &error_کوډ);

    /** @internal */
    int32_t write_غوټه(int32_t پیل, int32_t حد, int32_t unit_شاخص);
    /** @internal */
    int32_t write_Branch_Sub_غوټه(int32_t پیل, int32_t حد, int32_t unit_شاخص, int32_t اوږدتیا);
#د_که_پای  /* U_HIDE_INTERNAL_API */

    class غوټه_2;

#که_نه_تعریف_شوی U_HIDE_دننی_API
    /** @internal */
    غوټه_2* make_غوټه(int32_t پیل,
                   int32_t حد,
                   int32_t unit_شاخص,
                   UError_کوډ& error_کوډ) U_LIFETIME_BOUND;
    /** @internal */
    غوټه_2* make_Branch_Sub_غوټه(int32_t پیل,
                            int32_t حد,
                            int32_t unit_شاخص,
                            int32_t اوږدتیا,
                            UError_کوډ& error_کوډ) U_LIFETIME_BOUND;
#د_که_پای  /* U_HIDE_INTERNAL_API */

    /** @internal */
    مجازي int32_t get_Element_String_اوږدتیا(int32_t i) ثابت = 0;
    /** @internal */
    مجازي char16_t getElementUnit(int32_t i, int32_t unit_شاخص) ثابت = 0;
    /** @internal */
    مجازي int32_t get_Element_ارزښت(int32_t i) ثابت = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    مجازي int32_t get_حد_د_خطي_سمون(int32_t لومړی, int32_t وروستی, int32_t unit_شاخص) ثابت = 0;

    // Number of different units at unitIndex.
    /** @internal */
    مجازي int32_t شمېر_Element_Units(int32_t پیل, int32_t حد, int32_t unit_شاخص) ثابت = 0;
    /** @internal */
    مجازي int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_شاخص, int32_t شمېر) ثابت = 0;
    /** @internal */
    مجازي int32_t شاخص_د_Element_With_بل_Unit(int32_t i, int32_t unit_شاخص, char16_t unit) ثابت = 0;

    /** @internal */
    مجازي UBool سمون_Nodes_Can_Have_ارزښتونه() ثابت = 0;

    /** @internal */
    مجازي int32_t get_Max_Branch_خطي_Sub_غوټه_اوږدتیا() ثابت = 0;
    /** @internal */
    مجازي int32_t get_Min_خطي_سمون() ثابت = 0;
    /** @internal */
    مجازي int32_t get_Max_خطي_سمون_اوږدتیا() ثابت = 0;

#که_نه_تعریف_شوی U_HIDE_دننی_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    ساكن ثابت int32_t k_Max_Branch_خطي_Sub_غوټه_اوږدتیا=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    ساكن ثابت int32_t k_Max_وېشل_Branch_پوړونه=14;

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
    غوټه_2* ثبتوونکی_غوټه(غوټه_2* new_غوټه, UError_کوډ& error_کوډ) U_LIFETIME_BOUND;
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
    غوټه_2* ثبتوونکی_Final_ارزښت(int32_t ارزښت_2, UError_کوډ& error_کوډ) U_LIFETIME_BOUND;
#د_که_پای  /* U_HIDE_INTERNAL_API */

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
    class غوټه_2 : عام UObject {
    عام:
        غوټه_2(int32_t initialHash) : hash(initialHash), دننه_واټن(0) {}
        مضمن int32_t hash_کوډ() ثابت { أرجع hash; }
        // Handles node==nullptr.
        ساكن مضمن int32_t hash_کوډ(ثابت غوټه_2 *غوټه) { أرجع غوټه==nullptr ? 0 : غوټه->hash_کوډ(); }
        // Base class operator==() compares the actual class types.
        مجازي bool چلوونکی==(ثابت غوټه_2 &other) ثابت;
        مضمن bool چلوونکی!=(ثابت غوټه_2 &other) ثابت { أرجع !چلوونکی==(other); }
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
        مجازي int32_t mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره);
        // write() must set the offset to a positive value.
        مجازي فراغ اكتب(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        مضمن فراغ write_Unless_Inside_ښی_څنډه(int32_t لومړی_ښی, int32_t وروستی_ښی,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            إذا(دننه_واټن<0 && (دننه_واټن<وروستی_ښی || لومړی_ښی<دننه_واټن)) {
                اكتب(builder);
            }
        }
        مضمن int32_t get_دننه_واټن() ثابت { أرجع دننه_واټن; }
    محمي:
        int32_t hash;
        int32_t دننه_واټن;
    };

#که_نه_تعریف_شوی U_HIDE_دننی_API
    // This class should not be overridden because
    // registerFinalValue() compares a stack-allocated FinalValueNode
    // (stack-allocated so that we don't unnecessarily create lots of duplicate nodes)
    // with the input node, and the
    // !Node::operator==(other) used inside FinalValueNode::operator==(other)
    // will be false if the typeid's are different.
    /** @internal */
    class Final_ارزښت_غوټه : عام غوټه_2 {
    عام:
        Final_ارزښت_غوټه(int32_t v) : غوټه_2(0x111111u*37u+v), ارزښت_2(v) {}
        مجازي bool چلوونکی==(ثابت غوټه_2 &other) ثابت override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t ارزښت_2;
    };
#د_که_پای  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class ارزښت_غوټه : عام غوټه_2 {
    عام:
        ارزښت_غوټه(int32_t initialHash) : غوټه_2(initialHash), has_ارزښت(false), ارزښت_2(0) {}
        مجازي bool چلوونکی==(ثابت غوټه_2 &other) ثابت override;
        فراغ سېټ_ارزښت(int32_t v) {
            has_ارزښت=true;
            ارزښت_2=v;
            hash=hash*37u+v;
        }
    محمي:
        UBool has_ارزښت;
        int32_t ارزښت_2;
    };

#که_نه_تعریف_شوی U_HIDE_دننی_API
    /** 
     * @internal 
     */
    class Intermediate_ارزښت_غوټه : عام ارزښت_غوټه {
    عام:
        Intermediate_ارزښت_غوټه(int32_t v, غوټه_2 *بل_غوټه)
                : ارزښت_غوټه(0x222222u*37u+hash_کوډ(بل_غوټه)), بل(بل_غوټه) { سېټ_ارزښت(v); }
        مجازي bool چلوونکی==(ثابت غوټه_2 &other) ثابت override;
        مجازي int32_t mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره) override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        غوټه_2 *بل;
    };
#د_که_پای  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class خطي_سمون_غوټه : عام ارزښت_غوټه {
    عام:
        خطي_سمون_غوټه(int32_t len, غوټه_2 *بل_غوټه)
                : ارزښت_غوټه((0x333333u*37u+len)*37u+hash_کوډ(بل_غوټه)),
                  اوږدتیا(len), بل(بل_غوټه) {}
        مجازي bool چلوونکی==(ثابت غوټه_2 &other) ثابت override;
        مجازي int32_t mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره) override;
    محمي:
        int32_t اوږدتیا;
        غوټه_2 *بل;
    };

#که_نه_تعریف_شوی U_HIDE_دننی_API
    /**
     * @internal 
     */
    class Branch_غوټه : عام غوټه_2 {
    عام:
        Branch_غوټه(int32_t initialHash) : غوټه_2(initialHash) {}
    محمي:
        int32_t لومړی_څنډه_شمېره;
    };

    /**
     * @internal 
     */
    class List_Branch_غوټه : عام Branch_غوټه {
    عام:
        List_Branch_غوټه() : Branch_غوټه(0x444444), اوږدتیا(0) {}
        مجازي bool چلوونکی==(ثابت غوټه_2 &other) ثابت override;
        مجازي int32_t mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره) override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        فراغ جمع_کړه(int32_t c, int32_t ارزښت_2) {
            units[اوږدتیا] = static_cast<char16_t>(c);
            equal[اوږدتیا]=nullptr;
            ارزښتونه[اوږدتیا]=ارزښت_2;
            ++اوږدتیا;
            hash=(hash*37u+c)*37u+ارزښت_2;
        }
        // Adds a unit which leads to another match node.
        فراغ جمع_کړه(int32_t c, غوټه_2 *غوټه) {
            units[اوږدتیا] = static_cast<char16_t>(c);
            equal[اوږدتیا]=غوټه;
            ارزښتونه[اوږدتیا]=0;
            ++اوږدتیا;
            hash=(hash*37u+c)*37u+hash_کوډ(غوټه);
        }
    محمي:
        غوټه_2 *equal[k_Max_Branch_خطي_Sub_غوټه_اوږدتیا];  // nullptr means "has final value".
        int32_t اوږدتیا;
        int32_t ارزښتونه[k_Max_Branch_خطي_Sub_غوټه_اوږدتیا];
        char16_t units[k_Max_Branch_خطي_Sub_غوټه_اوږدتیا];
    };

    /**
     * @internal 
     */
    class وېشل_Branch_غوټه : عام Branch_غوټه {
    عام:
        وېشل_Branch_غوټه(char16_t منځ_Unit, غوټه_2 *less_Than_غوټه, غوټه_2 *greater_Or_Equal_غوټه)
                : Branch_غوټه(((0x555555u*37u+منځ_Unit)*37u+
                              hash_کوډ(less_Than_غوټه))*37u+hash_کوډ(greater_Or_Equal_غوټه)),
                  unit(منځ_Unit), lessThan(less_Than_غوټه), greaterOrEqual(greater_Or_Equal_غوټه) {}
        مجازي bool چلوونکی==(ثابت غوټه_2 &other) ثابت override;
        مجازي int32_t mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره) override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        char16_t unit;
        غوټه_2 *lessThan;
        غوټه_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_سر_غوټه : عام ارزښت_غوټه {
    عام:
        Branch_سر_غوټه(int32_t len, غوټه_2 *sub_غوټه)
                : ارزښت_غوټه((0x666666u*37u+len)*37u+hash_کوډ(sub_غوټه)),
                  اوږدتیا(len), بل(sub_غوټه) {}
        مجازي bool چلوونکی==(ثابت غوټه_2 &other) ثابت override;
        مجازي int32_t mark_ښی_څنډې_لومړی(int32_t څنډه_شمېره) override;
        مجازي فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t اوږدتیا;
        غوټه_2 *بل;  // A branch sub-node.
    };

#د_که_پای  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    مجازي غوټه_2 *create_خطي_سمون_غوټه(int32_t i, int32_t unit_شاخص, int32_t اوږدتیا,
                                        غوټه_2 *بل_غوټه) ثابت = 0;

    /** @internal */
    مجازي int32_t اكتب(int32_t unit) = 0;
    /** @internal */
    مجازي int32_t writeElementUnits(int32_t i, int32_t unit_شاخص, int32_t اوږدتیا) = 0;
    /** @internal */
    مجازي int32_t write_ارزښت_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    مجازي int32_t write_ارزښت_And_Type(UBool has_ارزښت, int32_t ارزښت_2, int32_t غوټه) = 0;
    /** @internal */
    مجازي int32_t write_Delta_منزل(int32_t jump_موخه) = 0;
};

U_NAMESPACE_پای_ټکی

#د_که_پای /* U_SHOW_CPLUSPLUS_API */

#د_که_پای  // __STRINGTRIEBUILDER_H__
