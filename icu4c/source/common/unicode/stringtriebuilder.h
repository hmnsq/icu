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

#اگر_تعریف_نشده __STRINGTRIEBUILDER_H__
#تعریف_کن __STRINGTRIEBUILDER_H__

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
#اگر_تعریف_نشده U_HIDE_داخلی_API
    /** @internal */
    ساكن int32_t hash_گره(ثابت فراغ *گره);
    /** @internal */
    ساكن UBool equalNodes(ثابت فراغ *چپ_2, ثابت فراغ *راست_2);
#پایان_اگر  /* U_HIDE_INTERNAL_API */

محمي:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    مجازی ~StringTrieBuilder();

#اگر_تعریف_نشده U_HIDE_داخلی_API
    /** @internal */
    فراغ createCompactBuilder(int32_t اندازه_Guess, UError_کد &error_کد);
    /** @internal */
    فراغ deleteCompactBuilder();

    /** @internal */
    فراغ build(UStringTrieBuildOption buildOption, int32_t elements_درازی, UError_کد &error_کد);

    /** @internal */
    int32_t write_گره(int32_t شروع, int32_t حد, int32_t unit_شاخص);
    /** @internal */
    int32_t write_Branch_Sub_گره(int32_t شروع, int32_t حد, int32_t unit_شاخص, int32_t درازی);
#پایان_اگر  /* U_HIDE_INTERNAL_API */

    class گره_2;

#اگر_تعریف_نشده U_HIDE_داخلی_API
    /** @internal */
    گره_2* make_گره(int32_t شروع,
                   int32_t حد,
                   int32_t unit_شاخص,
                   UError_کد& error_کد) U_LIFETIME_BOUND;
    /** @internal */
    گره_2* make_Branch_Sub_گره(int32_t شروع,
                            int32_t حد,
                            int32_t unit_شاخص,
                            int32_t درازی,
                            UError_کد& error_کد) U_LIFETIME_BOUND;
#پایان_اگر  /* U_HIDE_INTERNAL_API */

    /** @internal */
    مجازی int32_t get_Element_String_درازی(int32_t i) ثابت = 0;
    /** @internal */
    مجازی char16_t getElementUnit(int32_t i, int32_t unit_شاخص) ثابت = 0;
    /** @internal */
    مجازی int32_t get_Element_مقدار(int32_t i) ثابت = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    مجازی int32_t get_حد_از_خطی_تطابق(int32_t اولی, int32_t آخرین, int32_t unit_شاخص) ثابت = 0;

    // Number of different units at unitIndex.
    /** @internal */
    مجازی int32_t شمار_Element_Units(int32_t شروع, int32_t حد, int32_t unit_شاخص) ثابت = 0;
    /** @internal */
    مجازی int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_شاخص, int32_t شمار) ثابت = 0;
    /** @internal */
    مجازی int32_t شاخص_از_Element_With_بعدی_Unit(int32_t i, int32_t unit_شاخص, char16_t unit) ثابت = 0;

    /** @internal */
    مجازی UBool تطابق_Nodes_Can_Have_مقادیر() ثابت = 0;

    /** @internal */
    مجازی int32_t get_Max_Branch_خطی_Sub_گره_درازی() ثابت = 0;
    /** @internal */
    مجازی int32_t get_Min_خطی_تطابق() ثابت = 0;
    /** @internal */
    مجازی int32_t get_Max_خطی_تطابق_درازی() ثابت = 0;

#اگر_تعریف_نشده U_HIDE_داخلی_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    ساكن ثابت int32_t k_Max_Branch_خطی_Sub_گره_درازی=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    ساكن ثابت int32_t k_Max_تقسیم_Branch_سطوح=14;

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
    گره_2* ثبات_گره(گره_2* new_گره, UError_کد& error_کد) U_LIFETIME_BOUND;
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
    گره_2* ثبات_Final_مقدار(int32_t مقدار_2, UError_کد& error_کد) U_LIFETIME_BOUND;
#پایان_اگر  /* U_HIDE_INTERNAL_API */

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
    class گره_2 : عام UObject {
    عام:
        گره_2(int32_t initialHash) : hash(initialHash), جابه_جایی(0) {}
        مضمن int32_t hash_کد() ثابت { أرجع hash; }
        // Handles node==nullptr.
        ساكن مضمن int32_t hash_کد(ثابت گره_2 *گره) { أرجع گره==nullptr ? 0 : گره->hash_کد(); }
        // Base class operator==() compares the actual class types.
        مجازی bool عملگر==(ثابت گره_2 &other) ثابت;
        مضمن bool عملگر!=(ثابت گره_2 &other) ثابت { أرجع !عملگر==(other); }
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
        مجازی int32_t mark_راست_یالها_اولی(int32_t یال_عدد);
        // write() must set the offset to a positive value.
        مجازی فراغ اكتب(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        مضمن فراغ write_Unless_Inside_راست_یال(int32_t اولی_راست, int32_t آخرین_راست,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            إذا(جابه_جایی<0 && (جابه_جایی<آخرین_راست || اولی_راست<جابه_جایی)) {
                اكتب(builder);
            }
        }
        مضمن int32_t get_جابه_جایی() ثابت { أرجع جابه_جایی; }
    محمي:
        int32_t hash;
        int32_t جابه_جایی;
    };

#اگر_تعریف_نشده U_HIDE_داخلی_API
    // This class should not be overridden because
    // registerFinalValue() compares a stack-allocated FinalValueNode
    // (stack-allocated so that we don't unnecessarily create lots of duplicate nodes)
    // with the input node, and the
    // !Node::operator==(other) used inside FinalValueNode::operator==(other)
    // will be false if the typeid's are different.
    /** @internal */
    class Final_مقدار_گره : عام گره_2 {
    عام:
        Final_مقدار_گره(int32_t v) : گره_2(0x111111u*37u+v), مقدار_2(v) {}
        مجازی bool عملگر==(ثابت گره_2 &other) ثابت override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t مقدار_2;
    };
#پایان_اگر  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class مقدار_گره : عام گره_2 {
    عام:
        مقدار_گره(int32_t initialHash) : گره_2(initialHash), has_مقدار(false), مقدار_2(0) {}
        مجازی bool عملگر==(ثابت گره_2 &other) ثابت override;
        فراغ مجموعه_مقدار(int32_t v) {
            has_مقدار=true;
            مقدار_2=v;
            hash=hash*37u+v;
        }
    محمي:
        UBool has_مقدار;
        int32_t مقدار_2;
    };

#اگر_تعریف_نشده U_HIDE_داخلی_API
    /** 
     * @internal 
     */
    class Intermediate_مقدار_گره : عام مقدار_گره {
    عام:
        Intermediate_مقدار_گره(int32_t v, گره_2 *بعدی_گره)
                : مقدار_گره(0x222222u*37u+hash_کد(بعدی_گره)), بعدی(بعدی_گره) { مجموعه_مقدار(v); }
        مجازی bool عملگر==(ثابت گره_2 &other) ثابت override;
        مجازی int32_t mark_راست_یالها_اولی(int32_t یال_عدد) override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        گره_2 *بعدی;
    };
#پایان_اگر  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class خطی_تطابق_گره : عام مقدار_گره {
    عام:
        خطی_تطابق_گره(int32_t len, گره_2 *بعدی_گره)
                : مقدار_گره((0x333333u*37u+len)*37u+hash_کد(بعدی_گره)),
                  درازی(len), بعدی(بعدی_گره) {}
        مجازی bool عملگر==(ثابت گره_2 &other) ثابت override;
        مجازی int32_t mark_راست_یالها_اولی(int32_t یال_عدد) override;
    محمي:
        int32_t درازی;
        گره_2 *بعدی;
    };

#اگر_تعریف_نشده U_HIDE_داخلی_API
    /**
     * @internal 
     */
    class Branch_گره : عام گره_2 {
    عام:
        Branch_گره(int32_t initialHash) : گره_2(initialHash) {}
    محمي:
        int32_t اولی_یال_عدد;
    };

    /**
     * @internal 
     */
    class List_Branch_گره : عام Branch_گره {
    عام:
        List_Branch_گره() : Branch_گره(0x444444), درازی(0) {}
        مجازی bool عملگر==(ثابت گره_2 &other) ثابت override;
        مجازی int32_t mark_راست_یالها_اولی(int32_t یال_عدد) override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        فراغ جمع_کن(int32_t c, int32_t مقدار_2) {
            units[درازی] = static_cast<char16_t>(c);
            equal[درازی]=nullptr;
            مقادیر[درازی]=مقدار_2;
            ++درازی;
            hash=(hash*37u+c)*37u+مقدار_2;
        }
        // Adds a unit which leads to another match node.
        فراغ جمع_کن(int32_t c, گره_2 *گره) {
            units[درازی] = static_cast<char16_t>(c);
            equal[درازی]=گره;
            مقادیر[درازی]=0;
            ++درازی;
            hash=(hash*37u+c)*37u+hash_کد(گره);
        }
    محمي:
        گره_2 *equal[k_Max_Branch_خطی_Sub_گره_درازی];  // nullptr means "has final value".
        int32_t درازی;
        int32_t مقادیر[k_Max_Branch_خطی_Sub_گره_درازی];
        char16_t units[k_Max_Branch_خطی_Sub_گره_درازی];
    };

    /**
     * @internal 
     */
    class تقسیم_Branch_گره : عام Branch_گره {
    عام:
        تقسیم_Branch_گره(char16_t میانه_Unit, گره_2 *less_Than_گره, گره_2 *greater_Or_Equal_گره)
                : Branch_گره(((0x555555u*37u+میانه_Unit)*37u+
                              hash_کد(less_Than_گره))*37u+hash_کد(greater_Or_Equal_گره)),
                  unit(میانه_Unit), lessThan(less_Than_گره), greaterOrEqual(greater_Or_Equal_گره) {}
        مجازی bool عملگر==(ثابت گره_2 &other) ثابت override;
        مجازی int32_t mark_راست_یالها_اولی(int32_t یال_عدد) override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        char16_t unit;
        گره_2 *lessThan;
        گره_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_سر_گره : عام مقدار_گره {
    عام:
        Branch_سر_گره(int32_t len, گره_2 *sub_گره)
                : مقدار_گره((0x666666u*37u+len)*37u+hash_کد(sub_گره)),
                  درازی(len), بعدی(sub_گره) {}
        مجازی bool عملگر==(ثابت گره_2 &other) ثابت override;
        مجازی int32_t mark_راست_یالها_اولی(int32_t یال_عدد) override;
        مجازی فراغ اكتب(StringTrieBuilder &builder) override;
    محمي:
        int32_t درازی;
        گره_2 *بعدی;  // A branch sub-node.
    };

#پایان_اگر  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    مجازی گره_2 *create_خطی_تطابق_گره(int32_t i, int32_t unit_شاخص, int32_t درازی,
                                        گره_2 *بعدی_گره) ثابت = 0;

    /** @internal */
    مجازی int32_t اكتب(int32_t unit) = 0;
    /** @internal */
    مجازی int32_t writeElementUnits(int32_t i, int32_t unit_شاخص, int32_t درازی) = 0;
    /** @internal */
    مجازی int32_t write_مقدار_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    مجازی int32_t write_مقدار_And_Type(UBool has_مقدار, int32_t مقدار_2, int32_t گره) = 0;
    /** @internal */
    مجازی int32_t write_Delta_مقصد(int32_t jump_هدف) = 0;
};

U_NAMESPACE_انتها

#پایان_اگر /* U_SHOW_CPLUSPLUS_API */

#پایان_اگر  // __STRINGTRIEBUILDER_H__
