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

#నిర్వచితం_కాకపోతే __STRINGTRIEBUILDER_H__
#నిర్వచించు __STRINGTRIEBUILDER_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
నిర్మాణం UHashtable;
రకనిర్వచనం నిర్మాణం UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
గణన UStringTrieBuildOption {
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
class U_COMMON_API StringTrieBuilder : బహిరంగ UObject {
బహిరంగ:
#నిర్వచితం_కాకపోతే U_HIDE_అంతర్గత_API
    /** @internal */
    స్థిర int32_t hash_కణుపు(స్థిరరాశి శూన్య *కణుపు);
    /** @internal */
    స్థిర UBool equalNodes(స్థిరరాశి శూన్య *ఎడమ_2, స్థిరరాశి శూన్య *కుడి_2);
#అయితే_ముగింపు  /* U_HIDE_INTERNAL_API */

రక్షిత:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    మాయా ~StringTrieBuilder();

#నిర్వచితం_కాకపోతే U_HIDE_అంతర్గత_API
    /** @internal */
    శూన్య createCompactBuilder(int32_t కొలత_Guess, UError_సంకేతం &error_సంకేతం);
    /** @internal */
    శూన్య deleteCompactBuilder();

    /** @internal */
    శూన్య build(UStringTrieBuildOption buildOption, int32_t elements_నిడివి, UError_సంకేతం &error_సంకేతం);

    /** @internal */
    int32_t write_కణుపు(int32_t ప్రారంభం, int32_t హద్దు, int32_t unit_సూచీ);
    /** @internal */
    int32_t write_Branch_Sub_కణుపు(int32_t ప్రారంభం, int32_t హద్దు, int32_t unit_సూచీ, int32_t నిడివి);
#అయితే_ముగింపు  /* U_HIDE_INTERNAL_API */

    class కణుపు_2;

#నిర్వచితం_కాకపోతే U_HIDE_అంతర్గత_API
    /** @internal */
    కణుపు_2* make_కణుపు(int32_t ప్రారంభం,
                   int32_t హద్దు,
                   int32_t unit_సూచీ,
                   UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND;
    /** @internal */
    కణుపు_2* make_Branch_Sub_కణుపు(int32_t ప్రారంభం,
                            int32_t హద్దు,
                            int32_t unit_సూచీ,
                            int32_t నిడివి,
                            UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND;
#అయితే_ముగింపు  /* U_HIDE_INTERNAL_API */

    /** @internal */
    మాయా int32_t get_Element_String_నిడివి(int32_t i) స్థిరరాశి = 0;
    /** @internal */
    మాయా char16_t getElementUnit(int32_t i, int32_t unit_సూచీ) స్థిరరాశి = 0;
    /** @internal */
    మాయా int32_t get_Element_విలువ(int32_t i) స్థిరరాశి = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    మాయా int32_t get_హద్దు_యొక్క_రేఖీయ_సరిపోలిక(int32_t మొదటి, int32_t చివరి, int32_t unit_సూచీ) స్థిరరాశి = 0;

    // Number of different units at unitIndex.
    /** @internal */
    మాయా int32_t లెక్క_Element_Units(int32_t ప్రారంభం, int32_t హద్దు, int32_t unit_సూచీ) స్థిరరాశి = 0;
    /** @internal */
    మాయా int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_సూచీ, int32_t లెక్క) స్థిరరాశి = 0;
    /** @internal */
    మాయా int32_t సూచీ_యొక్క_Element_With_తదుపరి_Unit(int32_t i, int32_t unit_సూచీ, char16_t unit) స్థిరరాశి = 0;

    /** @internal */
    మాయా UBool సరిపోలిక_Nodes_Can_Have_విలువలు() స్థిరరాశి = 0;

    /** @internal */
    మాయా int32_t get_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి() స్థిరరాశి = 0;
    /** @internal */
    మాయా int32_t get_Min_రేఖీయ_సరిపోలిక() స్థిరరాశి = 0;
    /** @internal */
    మాయా int32_t get_Max_రేఖీయ_సరిపోలిక_నిడివి() స్థిరరాశి = 0;

#నిర్వచితం_కాకపోతే U_HIDE_అంతర్గత_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    స్థిర స్థిరరాశి int32_t k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    స్థిర స్థిరరాశి int32_t k_Max_విభజించు_Branch_స్థాయిలు=14;

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
    కణుపు_2* రిజిస్టరు_కణుపు(కణుపు_2* new_కణుపు, UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND;
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
    కణుపు_2* రిజిస్టరు_Final_విలువ(int32_t విలువ_2, UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND;
#అయితే_ముగింపు  /* U_HIDE_INTERNAL_API */

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
    class కణుపు_2 : బహిరంగ UObject {
    బహిరంగ:
        కణుపు_2(int32_t initialHash) : hash(initialHash), స్థానభ్రంశం(0) {}
        అంతఃవరుస int32_t hash_సంకేతం() స్థిరరాశి { తిరిగి hash; }
        // Handles node==nullptr.
        స్థిర అంతఃవరుస int32_t hash_సంకేతం(స్థిరరాశి కణుపు_2 *కణుపు) { తిరిగి కణుపు==nullptr ? 0 : కణుపు->hash_సంకేతం(); }
        // Base class operator==() compares the actual class types.
        మాయా bool క్రియాకారకం==(స్థిరరాశి కణుపు_2 &other) స్థిరరాశి;
        అంతఃవరుస bool క్రియాకారకం!=(స్థిరరాశి కణుపు_2 &other) స్థిరరాశి { తిరిగి !క్రియాకారకం==(other); }
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
        మాయా int32_t mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య);
        // write() must set the offset to a positive value.
        మాయా శూన్య లిఖ(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        అంతఃవరుస శూన్య write_Unless_Inside_కుడి_అంచు(int32_t మొదటి_కుడి, int32_t చివరి_కుడి,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            ఒకవేళ(స్థానభ్రంశం<0 && (స్థానభ్రంశం<చివరి_కుడి || మొదటి_కుడి<స్థానభ్రంశం)) {
                లిఖ(builder);
            }
        }
        అంతఃవరుస int32_t get_స్థానభ్రంశం() స్థిరరాశి { తిరిగి స్థానభ్రంశం; }
    రక్షిత:
        int32_t hash;
        int32_t స్థానభ్రంశం;
    };

#నిర్వచితం_కాకపోతే U_HIDE_అంతర్గత_API
    // This class should not be overridden because
    // registerFinalValue() compares a stack-allocated FinalValueNode
    // (stack-allocated so that we don't unnecessarily create lots of duplicate nodes)
    // with the input node, and the
    // !Node::operator==(other) used inside FinalValueNode::operator==(other)
    // will be false if the typeid's are different.
    /** @internal */
    class Final_విలువ_కణుపు : బహిరంగ కణుపు_2 {
    బహిరంగ:
        Final_విలువ_కణుపు(int32_t v) : కణుపు_2(0x111111u*37u+v), విలువ_2(v) {}
        మాయా bool క్రియాకారకం==(స్థిరరాశి కణుపు_2 &other) స్థిరరాశి override;
        మాయా శూన్య లిఖ(StringTrieBuilder &builder) override;
    రక్షిత:
        int32_t విలువ_2;
    };
#అయితే_ముగింపు  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class విలువ_కణుపు : బహిరంగ కణుపు_2 {
    బహిరంగ:
        విలువ_కణుపు(int32_t initialHash) : కణుపు_2(initialHash), has_విలువ(false), విలువ_2(0) {}
        మాయా bool క్రియాకారకం==(స్థిరరాశి కణుపు_2 &other) స్థిరరాశి override;
        శూన్య సమితి_విలువ(int32_t v) {
            has_విలువ=true;
            విలువ_2=v;
            hash=hash*37u+v;
        }
    రక్షిత:
        UBool has_విలువ;
        int32_t విలువ_2;
    };

#నిర్వచితం_కాకపోతే U_HIDE_అంతర్గత_API
    /** 
     * @internal 
     */
    class Intermediate_విలువ_కణుపు : బహిరంగ విలువ_కణుపు {
    బహిరంగ:
        Intermediate_విలువ_కణుపు(int32_t v, కణుపు_2 *తదుపరి_కణుపు)
                : విలువ_కణుపు(0x222222u*37u+hash_సంకేతం(తదుపరి_కణుపు)), తదుపరి(తదుపరి_కణుపు) { సమితి_విలువ(v); }
        మాయా bool క్రియాకారకం==(స్థిరరాశి కణుపు_2 &other) స్థిరరాశి override;
        మాయా int32_t mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య) override;
        మాయా శూన్య లిఖ(StringTrieBuilder &builder) override;
    రక్షిత:
        కణుపు_2 *తదుపరి;
    };
#అయితే_ముగింపు  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class రేఖీయ_సరిపోలిక_కణుపు : బహిరంగ విలువ_కణుపు {
    బహిరంగ:
        రేఖీయ_సరిపోలిక_కణుపు(int32_t len, కణుపు_2 *తదుపరి_కణుపు)
                : విలువ_కణుపు((0x333333u*37u+len)*37u+hash_సంకేతం(తదుపరి_కణుపు)),
                  నిడివి(len), తదుపరి(తదుపరి_కణుపు) {}
        మాయా bool క్రియాకారకం==(స్థిరరాశి కణుపు_2 &other) స్థిరరాశి override;
        మాయా int32_t mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య) override;
    రక్షిత:
        int32_t నిడివి;
        కణుపు_2 *తదుపరి;
    };

#నిర్వచితం_కాకపోతే U_HIDE_అంతర్గత_API
    /**
     * @internal 
     */
    class Branch_కణుపు : బహిరంగ కణుపు_2 {
    బహిరంగ:
        Branch_కణుపు(int32_t initialHash) : కణుపు_2(initialHash) {}
    రక్షిత:
        int32_t మొదటి_అంచు_సంఖ్య;
    };

    /**
     * @internal 
     */
    class List_Branch_కణుపు : బహిరంగ Branch_కణుపు {
    బహిరంగ:
        List_Branch_కణుపు() : Branch_కణుపు(0x444444), నిడివి(0) {}
        మాయా bool క్రియాకారకం==(స్థిరరాశి కణుపు_2 &other) స్థిరరాశి override;
        మాయా int32_t mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య) override;
        మాయా శూన్య లిఖ(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        శూన్య కూడు(int32_t c, int32_t విలువ_2) {
            units[నిడివి] = static_cast<char16_t>(c);
            equal[నిడివి]=nullptr;
            విలువలు[నిడివి]=విలువ_2;
            ++నిడివి;
            hash=(hash*37u+c)*37u+విలువ_2;
        }
        // Adds a unit which leads to another match node.
        శూన్య కూడు(int32_t c, కణుపు_2 *కణుపు) {
            units[నిడివి] = static_cast<char16_t>(c);
            equal[నిడివి]=కణుపు;
            విలువలు[నిడివి]=0;
            ++నిడివి;
            hash=(hash*37u+c)*37u+hash_సంకేతం(కణుపు);
        }
    రక్షిత:
        కణుపు_2 *equal[k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి];  // nullptr means "has final value".
        int32_t నిడివి;
        int32_t విలువలు[k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి];
        char16_t units[k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి];
    };

    /**
     * @internal 
     */
    class విభజించు_Branch_కణుపు : బహిరంగ Branch_కణుపు {
    బహిరంగ:
        విభజించు_Branch_కణుపు(char16_t మధ్య_Unit, కణుపు_2 *less_Than_కణుపు, కణుపు_2 *greater_Or_Equal_కణుపు)
                : Branch_కణుపు(((0x555555u*37u+మధ్య_Unit)*37u+
                              hash_సంకేతం(less_Than_కణుపు))*37u+hash_సంకేతం(greater_Or_Equal_కణుపు)),
                  unit(మధ్య_Unit), lessThan(less_Than_కణుపు), greaterOrEqual(greater_Or_Equal_కణుపు) {}
        మాయా bool క్రియాకారకం==(స్థిరరాశి కణుపు_2 &other) స్థిరరాశి override;
        మాయా int32_t mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య) override;
        మాయా శూన్య లిఖ(StringTrieBuilder &builder) override;
    రక్షిత:
        char16_t unit;
        కణుపు_2 *lessThan;
        కణుపు_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_తల_కణుపు : బహిరంగ విలువ_కణుపు {
    బహిరంగ:
        Branch_తల_కణుపు(int32_t len, కణుపు_2 *sub_కణుపు)
                : విలువ_కణుపు((0x666666u*37u+len)*37u+hash_సంకేతం(sub_కణుపు)),
                  నిడివి(len), తదుపరి(sub_కణుపు) {}
        మాయా bool క్రియాకారకం==(స్థిరరాశి కణుపు_2 &other) స్థిరరాశి override;
        మాయా int32_t mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య) override;
        మాయా శూన్య లిఖ(StringTrieBuilder &builder) override;
    రక్షిత:
        int32_t నిడివి;
        కణుపు_2 *తదుపరి;  // A branch sub-node.
    };

#అయితే_ముగింపు  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    మాయా కణుపు_2 *create_రేఖీయ_సరిపోలిక_కణుపు(int32_t i, int32_t unit_సూచీ, int32_t నిడివి,
                                        కణుపు_2 *తదుపరి_కణుపు) స్థిరరాశి = 0;

    /** @internal */
    మాయా int32_t లిఖ(int32_t unit) = 0;
    /** @internal */
    మాయా int32_t writeElementUnits(int32_t i, int32_t unit_సూచీ, int32_t నిడివి) = 0;
    /** @internal */
    మాయా int32_t write_విలువ_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    మాయా int32_t write_విలువ_And_Type(UBool has_విలువ, int32_t విలువ_2, int32_t కణుపు) = 0;
    /** @internal */
    మాయా int32_t write_Delta_గమ్యం(int32_t jump_లక్ష్యం) = 0;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు  // __STRINGTRIEBUILDER_H__
