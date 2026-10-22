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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __STRINGTRIEBUILDER_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __STRINGTRIEBUILDER_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
ਬਣਤਰ UHashtable;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
ਗਿਣਤੀ UStringTrieBuildOption {
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
class U_COMMON_API StringTrieBuilder : ਜਨਤਕ UObject {
ਜਨਤਕ:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_HIDE_ਅੰਦਰੂਨੀ_API
    /** @internal */
    ਸਥਿਰ int32_t hash_ਗੰਢ(ਅਚਲ ਖਾਲੀ *ਗੰਢ);
    /** @internal */
    ਸਥਿਰ UBool equalNodes(ਅਚਲ ਖਾਲੀ *ਖੱਬਾ, ਅਚਲ ਖਾਲੀ *ਸੱਜਾ);
#ਜੇ_ਅੰਤ  /* U_HIDE_INTERNAL_API */

ਸੁਰੱਖਿਅਤ:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    ਆਭਾਸੀ ~StringTrieBuilder();

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_HIDE_ਅੰਦਰੂਨੀ_API
    /** @internal */
    ਖਾਲੀ createCompactBuilder(int32_t ਮਾਪ_Guess, UError_ਕੋਡ &error_ਕੋਡ);
    /** @internal */
    ਖਾਲੀ deleteCompactBuilder();

    /** @internal */
    ਖਾਲੀ build(UStringTrieBuildOption buildOption, int32_t elements_ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ);

    /** @internal */
    int32_t write_ਗੰਢ(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, int32_t unit_ਸੂਚਕਾਂਕ);
    /** @internal */
    int32_t write_Branch_Sub_ਗੰਢ(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, int32_t unit_ਸੂਚਕਾਂਕ, int32_t ਲੰਮਾਈ);
#ਜੇ_ਅੰਤ  /* U_HIDE_INTERNAL_API */

    class ਗੰਢ_2;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_HIDE_ਅੰਦਰੂਨੀ_API
    /** @internal */
    ਗੰਢ_2* make_ਗੰਢ(int32_t ਸ਼ੁਰੂ,
                   int32_t ਹੱਦਬੰਦੀ,
                   int32_t unit_ਸੂਚਕਾਂਕ,
                   UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;
    /** @internal */
    ਗੰਢ_2* make_Branch_Sub_ਗੰਢ(int32_t ਸ਼ੁਰੂ,
                            int32_t ਹੱਦਬੰਦੀ,
                            int32_t unit_ਸੂਚਕਾਂਕ,
                            int32_t ਲੰਮਾਈ,
                            UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;
#ਜੇ_ਅੰਤ  /* U_HIDE_INTERNAL_API */

    /** @internal */
    ਆਭਾਸੀ int32_t get_Element_String_ਲੰਮਾਈ(int32_t i) ਅਚਲ = 0;
    /** @internal */
    ਆਭਾਸੀ char16_t getElementUnit(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ) ਅਚਲ = 0;
    /** @internal */
    ਆਭਾਸੀ int32_t get_Element_ਮੁੱਲ(int32_t i) ਅਚਲ = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    ਆਭਾਸੀ int32_t get_ਹੱਦਬੰਦੀ_ਦਾ_ਰੇਖਿਕ_ਮੇਲ(int32_t ਪਹਿਲਾ, int32_t ਆਖਰੀ, int32_t unit_ਸੂਚਕਾਂਕ) ਅਚਲ = 0;

    // Number of different units at unitIndex.
    /** @internal */
    ਆਭਾਸੀ int32_t ਗਿਣਤੀ_Element_Units(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, int32_t unit_ਸੂਚਕਾਂਕ) ਅਚਲ = 0;
    /** @internal */
    ਆਭਾਸੀ int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, int32_t ਗਿਣਤੀ_2) ਅਚਲ = 0;
    /** @internal */
    ਆਭਾਸੀ int32_t ਸੂਚਕਾਂਕ_ਦਾ_Element_With_ਅਗਲਾ_Unit(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, char16_t unit) ਅਚਲ = 0;

    /** @internal */
    ਆਭਾਸੀ UBool ਮੇਲ_Nodes_Can_Have_ਮੁੱਲਾਂ() ਅਚਲ = 0;

    /** @internal */
    ਆਭਾਸੀ int32_t get_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ() ਅਚਲ = 0;
    /** @internal */
    ਆਭਾਸੀ int32_t get_Min_ਰੇਖਿਕ_ਮੇਲ() ਅਚਲ = 0;
    /** @internal */
    ਆਭਾਸੀ int32_t get_Max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ() ਅਚਲ = 0;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_HIDE_ਅੰਦਰੂਨੀ_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    ਸਥਿਰ ਅਚਲ int32_t k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    ਸਥਿਰ ਅਚਲ int32_t k_Max_ਵੰਡੋ_Branch_ਪੱਧਰ=14;

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
    ਗੰਢ_2* ਰਜਿਸਟਰ_ਗੰਢ(ਗੰਢ_2* new_ਗੰਢ, UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;
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
    ਗੰਢ_2* ਰਜਿਸਟਰ_Final_ਮੁੱਲ(int32_t ਮੁੱਲ_2, UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND;
#ਜੇ_ਅੰਤ  /* U_HIDE_INTERNAL_API */

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
    class ਗੰਢ_2 : ਜਨਤਕ UObject {
    ਜਨਤਕ:
        ਗੰਢ_2(int32_t initialHash) : hash(initialHash), ਵਿੱਥ(0) {}
        ਸਤਰਅੰਦਰ int32_t hash_ਕੋਡ() ਅਚਲ { ਵਾਪਸ hash; }
        // Handles node==nullptr.
        ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t hash_ਕੋਡ(ਅਚਲ ਗੰਢ_2 *ਗੰਢ) { ਵਾਪਸ ਗੰਢ==nullptr ? 0 : ਗੰਢ->hash_ਕੋਡ(); }
        // Base class operator==() compares the actual class types.
        ਆਭਾਸੀ bool ਸੰਚਾਲਕ==(ਅਚਲ ਗੰਢ_2 &other) ਅਚਲ;
        ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ ਗੰਢ_2 &other) ਅਚਲ { ਵਾਪਸ !ਸੰਚਾਲਕ==(other); }
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
        ਆਭਾਸੀ int32_t mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ);
        // write() must set the offset to a positive value.
        ਆਭਾਸੀ ਖਾਲੀ ਲਿਖ(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        ਸਤਰਅੰਦਰ ਖਾਲੀ write_Unless_Inside_ਸੱਜਾ_ਕਿਨਾਰਾ(int32_t ਪਹਿਲਾ_ਸੱਜਾ, int32_t ਆਖਰੀ_ਸੱਜਾ,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            ਜੇ(ਵਿੱਥ<0 && (ਵਿੱਥ<ਆਖਰੀ_ਸੱਜਾ || ਪਹਿਲਾ_ਸੱਜਾ<ਵਿੱਥ)) {
                ਲਿਖ(builder);
            }
        }
        ਸਤਰਅੰਦਰ int32_t get_ਵਿੱਥ() ਅਚਲ { ਵਾਪਸ ਵਿੱਥ; }
    ਸੁਰੱਖਿਅਤ:
        int32_t hash;
        int32_t ਵਿੱਥ;
    };

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_HIDE_ਅੰਦਰੂਨੀ_API
    // This class should not be overridden because
    // registerFinalValue() compares a stack-allocated FinalValueNode
    // (stack-allocated so that we don't unnecessarily create lots of duplicate nodes)
    // with the input node, and the
    // !Node::operator==(other) used inside FinalValueNode::operator==(other)
    // will be false if the typeid's are different.
    /** @internal */
    class Final_ਮੁੱਲ_ਗੰਢ : ਜਨਤਕ ਗੰਢ_2 {
    ਜਨਤਕ:
        Final_ਮੁੱਲ_ਗੰਢ(int32_t v) : ਗੰਢ_2(0x111111u*37u+v), ਮੁੱਲ_2(v) {}
        ਆਭਾਸੀ bool ਸੰਚਾਲਕ==(ਅਚਲ ਗੰਢ_2 &other) ਅਚਲ override;
        ਆਭਾਸੀ ਖਾਲੀ ਲਿਖ(StringTrieBuilder &builder) override;
    ਸੁਰੱਖਿਅਤ:
        int32_t ਮੁੱਲ_2;
    };
#ਜੇ_ਅੰਤ  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class ਮੁੱਲ_ਗੰਢ : ਜਨਤਕ ਗੰਢ_2 {
    ਜਨਤਕ:
        ਮੁੱਲ_ਗੰਢ(int32_t initialHash) : ਗੰਢ_2(initialHash), has_ਮੁੱਲ(false), ਮੁੱਲ_2(0) {}
        ਆਭਾਸੀ bool ਸੰਚਾਲਕ==(ਅਚਲ ਗੰਢ_2 &other) ਅਚਲ override;
        ਖਾਲੀ ਸੈੱਟ_ਮੁੱਲ(int32_t v) {
            has_ਮੁੱਲ=true;
            ਮੁੱਲ_2=v;
            hash=hash*37u+v;
        }
    ਸੁਰੱਖਿਅਤ:
        UBool has_ਮੁੱਲ;
        int32_t ਮੁੱਲ_2;
    };

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_HIDE_ਅੰਦਰੂਨੀ_API
    /** 
     * @internal 
     */
    class Intermediate_ਮੁੱਲ_ਗੰਢ : ਜਨਤਕ ਮੁੱਲ_ਗੰਢ {
    ਜਨਤਕ:
        Intermediate_ਮੁੱਲ_ਗੰਢ(int32_t v, ਗੰਢ_2 *ਅਗਲਾ_ਗੰਢ)
                : ਮੁੱਲ_ਗੰਢ(0x222222u*37u+hash_ਕੋਡ(ਅਗਲਾ_ਗੰਢ)), ਅਗਲਾ(ਅਗਲਾ_ਗੰਢ) { ਸੈੱਟ_ਮੁੱਲ(v); }
        ਆਭਾਸੀ bool ਸੰਚਾਲਕ==(ਅਚਲ ਗੰਢ_2 &other) ਅਚਲ override;
        ਆਭਾਸੀ int32_t mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ) override;
        ਆਭਾਸੀ ਖਾਲੀ ਲਿਖ(StringTrieBuilder &builder) override;
    ਸੁਰੱਖਿਅਤ:
        ਗੰਢ_2 *ਅਗਲਾ;
    };
#ਜੇ_ਅੰਤ  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ : ਜਨਤਕ ਮੁੱਲ_ਗੰਢ {
    ਜਨਤਕ:
        ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ(int32_t len, ਗੰਢ_2 *ਅਗਲਾ_ਗੰਢ)
                : ਮੁੱਲ_ਗੰਢ((0x333333u*37u+len)*37u+hash_ਕੋਡ(ਅਗਲਾ_ਗੰਢ)),
                  ਲੰਮਾਈ(len), ਅਗਲਾ(ਅਗਲਾ_ਗੰਢ) {}
        ਆਭਾਸੀ bool ਸੰਚਾਲਕ==(ਅਚਲ ਗੰਢ_2 &other) ਅਚਲ override;
        ਆਭਾਸੀ int32_t mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ) override;
    ਸੁਰੱਖਿਅਤ:
        int32_t ਲੰਮਾਈ;
        ਗੰਢ_2 *ਅਗਲਾ;
    };

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_HIDE_ਅੰਦਰੂਨੀ_API
    /**
     * @internal 
     */
    class Branch_ਗੰਢ : ਜਨਤਕ ਗੰਢ_2 {
    ਜਨਤਕ:
        Branch_ਗੰਢ(int32_t initialHash) : ਗੰਢ_2(initialHash) {}
    ਸੁਰੱਖਿਅਤ:
        int32_t ਪਹਿਲਾ_ਕਿਨਾਰਾ_ਸੰਖਿਆ;
    };

    /**
     * @internal 
     */
    class List_Branch_ਗੰਢ : ਜਨਤਕ Branch_ਗੰਢ {
    ਜਨਤਕ:
        List_Branch_ਗੰਢ() : Branch_ਗੰਢ(0x444444), ਲੰਮਾਈ(0) {}
        ਆਭਾਸੀ bool ਸੰਚਾਲਕ==(ਅਚਲ ਗੰਢ_2 &other) ਅਚਲ override;
        ਆਭਾਸੀ int32_t mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ) override;
        ਆਭਾਸੀ ਖਾਲੀ ਲਿਖ(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        ਖਾਲੀ ਜੋੜੋ(int32_t c, int32_t ਮੁੱਲ_2) {
            units[ਲੰਮਾਈ] = static_cast<char16_t>(c);
            equal[ਲੰਮਾਈ]=nullptr;
            ਮੁੱਲਾਂ[ਲੰਮਾਈ]=ਮੁੱਲ_2;
            ++ਲੰਮਾਈ;
            hash=(hash*37u+c)*37u+ਮੁੱਲ_2;
        }
        // Adds a unit which leads to another match node.
        ਖਾਲੀ ਜੋੜੋ(int32_t c, ਗੰਢ_2 *ਗੰਢ) {
            units[ਲੰਮਾਈ] = static_cast<char16_t>(c);
            equal[ਲੰਮਾਈ]=ਗੰਢ;
            ਮੁੱਲਾਂ[ਲੰਮਾਈ]=0;
            ++ਲੰਮਾਈ;
            hash=(hash*37u+c)*37u+hash_ਕੋਡ(ਗੰਢ);
        }
    ਸੁਰੱਖਿਅਤ:
        ਗੰਢ_2 *equal[k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ];  // nullptr means "has final value".
        int32_t ਲੰਮਾਈ;
        int32_t ਮੁੱਲਾਂ[k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ];
        char16_t units[k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ];
    };

    /**
     * @internal 
     */
    class ਵੰਡੋ_Branch_ਗੰਢ : ਜਨਤਕ Branch_ਗੰਢ {
    ਜਨਤਕ:
        ਵੰਡੋ_Branch_ਗੰਢ(char16_t ਵਿਚਕਾਰ_Unit, ਗੰਢ_2 *less_Than_ਗੰਢ, ਗੰਢ_2 *greater_Or_Equal_ਗੰਢ)
                : Branch_ਗੰਢ(((0x555555u*37u+ਵਿਚਕਾਰ_Unit)*37u+
                              hash_ਕੋਡ(less_Than_ਗੰਢ))*37u+hash_ਕੋਡ(greater_Or_Equal_ਗੰਢ)),
                  unit(ਵਿਚਕਾਰ_Unit), lessThan(less_Than_ਗੰਢ), greaterOrEqual(greater_Or_Equal_ਗੰਢ) {}
        ਆਭਾਸੀ bool ਸੰਚਾਲਕ==(ਅਚਲ ਗੰਢ_2 &other) ਅਚਲ override;
        ਆਭਾਸੀ int32_t mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ) override;
        ਆਭਾਸੀ ਖਾਲੀ ਲਿਖ(StringTrieBuilder &builder) override;
    ਸੁਰੱਖਿਅਤ:
        char16_t unit;
        ਗੰਢ_2 *lessThan;
        ਗੰਢ_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_ਸਿਰ_ਗੰਢ : ਜਨਤਕ ਮੁੱਲ_ਗੰਢ {
    ਜਨਤਕ:
        Branch_ਸਿਰ_ਗੰਢ(int32_t len, ਗੰਢ_2 *sub_ਗੰਢ)
                : ਮੁੱਲ_ਗੰਢ((0x666666u*37u+len)*37u+hash_ਕੋਡ(sub_ਗੰਢ)),
                  ਲੰਮਾਈ(len), ਅਗਲਾ(sub_ਗੰਢ) {}
        ਆਭਾਸੀ bool ਸੰਚਾਲਕ==(ਅਚਲ ਗੰਢ_2 &other) ਅਚਲ override;
        ਆਭਾਸੀ int32_t mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ) override;
        ਆਭਾਸੀ ਖਾਲੀ ਲਿਖ(StringTrieBuilder &builder) override;
    ਸੁਰੱਖਿਅਤ:
        int32_t ਲੰਮਾਈ;
        ਗੰਢ_2 *ਅਗਲਾ;  // A branch sub-node.
    };

#ਜੇ_ਅੰਤ  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    ਆਭਾਸੀ ਗੰਢ_2 *create_ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, int32_t ਲੰਮਾਈ,
                                        ਗੰਢ_2 *ਅਗਲਾ_ਗੰਢ) ਅਚਲ = 0;

    /** @internal */
    ਆਭਾਸੀ int32_t ਲਿਖ(int32_t unit) = 0;
    /** @internal */
    ਆਭਾਸੀ int32_t writeElementUnits(int32_t i, int32_t unit_ਸੂਚਕਾਂਕ, int32_t ਲੰਮਾਈ) = 0;
    /** @internal */
    ਆਭਾਸੀ int32_t write_ਮੁੱਲ_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    ਆਭਾਸੀ int32_t write_ਮੁੱਲ_And_Type(UBool has_ਮੁੱਲ, int32_t ਮੁੱਲ_2, int32_t ਗੰਢ) = 0;
    /** @internal */
    ਆਭਾਸੀ int32_t write_Delta_ਮੰਜ਼ਿਲ(int32_t jump_ਨਿਸ਼ਾਨਾ) = 0;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* U_SHOW_CPLUSPLUS_API */

#ਜੇ_ਅੰਤ  // __STRINGTRIEBUILDER_H__
