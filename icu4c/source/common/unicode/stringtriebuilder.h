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

#यदि_अपरिभाषितम् __STRINGTRIEBUILDER_H__
#परिभाषय __STRINGTRIEBUILDER_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API

#अन्तर्भाव "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Builder API for trie builders
 */

// Forward declaration.
/// \cond
संरचना UHashtable;
प्रकारनिर्देश संरचना UHashtable UHashtable;
/// \endcond

/**
 * Build options for BytesTrieBuilder and CharsTrieBuilder.
 * @stable ICU 4.8
 */
गणना UStringTrieBuildOption {
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
class U_COMMON_API StringTrieBuilder : सार्वजनिक UObject {
सार्वजनिक:
#यदि_अपरिभाषितम् U_HIDE_आन्तरिकम्_API
    /** @internal */
    स्थिर int32_t hash_ग्रन्थिः(ध्रुव शून्य *ग्रन्थिः);
    /** @internal */
    स्थिर UBool equalNodes(ध्रुव शून्य *वामः, ध्रुव शून्य *दक्षिणः);
#यदि_अन्तः  /* U_HIDE_INTERNAL_API */

रक्षित:
    // Do not enclose the protected default constructor with #ifndef U_HIDE_INTERNAL_API
    // or else the compiler will create a public default constructor.
    /** @internal */
    StringTrieBuilder();
    /** @internal */
    आभास ~StringTrieBuilder();

#यदि_अपरिभाषितम् U_HIDE_आन्तरिकम्_API
    /** @internal */
    शून्य createCompactBuilder(int32_t आकार_Guess, UError_संहिता &error_संहिता);
    /** @internal */
    शून्य deleteCompactBuilder();

    /** @internal */
    शून्य build(UStringTrieBuildOption buildOption, int32_t elements_दीर्घता, UError_संहिता &error_संहिता);

    /** @internal */
    int32_t write_ग्रन्थिः(int32_t start, int32_t सीमा, int32_t unit_सूचकांक);
    /** @internal */
    int32_t write_Branch_Sub_ग्रन्थिः(int32_t start, int32_t सीमा, int32_t unit_सूचकांक, int32_t दीर्घता);
#यदि_अन्तः  /* U_HIDE_INTERNAL_API */

    class ग्रन्थिः_2;

#यदि_अपरिभाषितम् U_HIDE_आन्तरिकम्_API
    /** @internal */
    ग्रन्थिः_2* make_ग्रन्थिः(int32_t start,
                   int32_t सीमा,
                   int32_t unit_सूचकांक,
                   UError_संहिता& error_संहिता) U_LIFETIME_BOUND;
    /** @internal */
    ग्रन्थिः_2* make_Branch_Sub_ग्रन्थिः(int32_t start,
                            int32_t सीमा,
                            int32_t unit_सूचकांक,
                            int32_t दीर्घता,
                            UError_संहिता& error_संहिता) U_LIFETIME_BOUND;
#यदि_अन्तः  /* U_HIDE_INTERNAL_API */

    /** @internal */
    आभास int32_t get_Element_String_दीर्घता(int32_t i) ध्रुव = 0;
    /** @internal */
    आभास char16_t getElementUnit(int32_t i, int32_t unit_सूचकांक) ध्रुव = 0;
    /** @internal */
    आभास int32_t get_Element_मानम्(int32_t i) ध्रुव = 0;

    // Finds the first unit index after this one where
    // the first and last element have different units again.
    /** @internal */
    आभास int32_t get_सीमा_सम्बन्धि_रैखिकम्_मेलनम्(int32_t first, int32_t चरमः, int32_t unit_सूचकांक) ध्रुव = 0;

    // Number of different units at unitIndex.
    /** @internal */
    आभास int32_t गणनम्_Element_Units(int32_t start, int32_t सीमा, int32_t unit_सूचकांक) ध्रुव = 0;
    /** @internal */
    आभास int32_t skipElementsBySomeUnits(int32_t i, int32_t unit_सूचकांक, int32_t गणनम्) ध्रुव = 0;
    /** @internal */
    आभास int32_t सूचकांक_सम्बन्धि_Element_With_अग्र_Unit(int32_t i, int32_t unit_सूचकांक, char16_t unit) ध्रुव = 0;

    /** @internal */
    आभास UBool मेलनम्_Nodes_Can_Have_मानानि() ध्रुव = 0;

    /** @internal */
    आभास int32_t get_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता() ध्रुव = 0;
    /** @internal */
    आभास int32_t get_Min_रैखिकम्_मेलनम्() ध्रुव = 0;
    /** @internal */
    आभास int32_t get_Max_रैखिकम्_मेलनम्_दीर्घता() ध्रुव = 0;

#यदि_अपरिभाषितम् U_HIDE_आन्तरिकम्_API
    // max(BytesTrie::kMaxBranchLinearSubNodeLength, UCharsTrie::kMaxBranchLinearSubNodeLength).
    /** @internal */
    स्थिर ध्रुव int32_t k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता=5;

    // Maximum number of nested split-branch levels for a branch on all 2^16 possible char16_t units.
    // log2(2^16/kMaxBranchLinearSubNodeLength) rounded up.
    /** @internal */
    स्थिर ध्रुव int32_t k_Max_विभज_Branch_स्तराः=14;

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
    ग्रन्थिः_2* पञ्जिकाकोशः_ग्रन्थिः(ग्रन्थिः_2* new_ग्रन्थिः, UError_संहिता& error_संहिता) U_LIFETIME_BOUND;
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
    ग्रन्थिः_2* पञ्जिकाकोशः_Final_मानम्(int32_t मानम्_2, UError_संहिता& error_संहिता) U_LIFETIME_BOUND;
#यदि_अन्तः  /* U_HIDE_INTERNAL_API */

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
    class ग्रन्थिः_2 : सार्वजनिक UObject {
    सार्वजनिक:
        ग्रन्थिः_2(int32_t initialHash) : hash(initialHash), विस्थापनम्(0) {}
        अन्तःस्थ int32_t hash_संहिता() ध्रुव { प्रत्यागम hash; }
        // Handles node==nullptr.
        स्थिर अन्तःस्थ int32_t hash_संहिता(ध्रुव ग्रन्थिः_2 *ग्रन्थिः) { प्रत्यागम ग्रन्थिः==nullptr ? 0 : ग्रन्थिः->hash_संहिता(); }
        // Base class operator==() compares the actual class types.
        आभास bool संक्रियकः==(ध्रुव ग्रन्थिः_2 &other) ध्रुव;
        अन्तःस्थ bool संक्रियकः!=(ध्रुव ग्रन्थिः_2 &other) ध्रुव { प्रत्यागम !संक्रियकः==(other); }
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
        आभास int32_t mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या);
        // write() must set the offset to a positive value.
        आभास शून्य लिख(StringTrieBuilder &builder) = 0;
        // See markRightEdgesFirst.
        अन्तःस्थ शून्य write_Unless_Inside_दक्षिणः_Edge(int32_t first_दक्षिणः, int32_t चरमः_दक्षिणः,
                                               StringTrieBuilder &builder) {
            // Note: Edge numbers are negative, lastRight<=firstRight.
            // If offset>0 then this node and its sub-nodes have been written already
            // and we need not write them again.
            // If this node is part of the unwritten right branch edge,
            // then we wait until that is written.
            यदि(विस्थापनम्<0 && (विस्थापनम्<चरमः_दक्षिणः || first_दक्षिणः<विस्थापनम्)) {
                लिख(builder);
            }
        }
        अन्तःस्थ int32_t get_विस्थापनम्() ध्रुव { प्रत्यागम विस्थापनम्; }
    रक्षित:
        int32_t hash;
        int32_t विस्थापनम्;
    };

#यदि_अपरिभाषितम् U_HIDE_आन्तरिकम्_API
    // This class should not be overridden because
    // registerFinalValue() compares a stack-allocated FinalValueNode
    // (stack-allocated so that we don't unnecessarily create lots of duplicate nodes)
    // with the input node, and the
    // !Node::operator==(other) used inside FinalValueNode::operator==(other)
    // will be false if the typeid's are different.
    /** @internal */
    class Final_मानम्_ग्रन्थिः : सार्वजनिक ग्रन्थिः_2 {
    सार्वजनिक:
        Final_मानम्_ग्रन्थिः(int32_t v) : ग्रन्थिः_2(0x111111u*37u+v), मानम्_2(v) {}
        आभास bool संक्रियकः==(ध्रुव ग्रन्थिः_2 &other) ध्रुव override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
    रक्षित:
        int32_t मानम्_2;
    };
#यदि_अन्तः  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class मानम्_ग्रन्थिः : सार्वजनिक ग्रन्थिः_2 {
    सार्वजनिक:
        मानम्_ग्रन्थिः(int32_t initialHash) : ग्रन्थिः_2(initialHash), has_मानम्(false), मानम्_2(0) {}
        आभास bool संक्रियकः==(ध्रुव ग्रन्थिः_2 &other) ध्रुव override;
        शून्य समुच्चयः_मानम्(int32_t v) {
            has_मानम्=true;
            मानम्_2=v;
            hash=hash*37u+v;
        }
    रक्षित:
        UBool has_मानम्;
        int32_t मानम्_2;
    };

#यदि_अपरिभाषितम् U_HIDE_आन्तरिकम्_API
    /** 
     * @internal 
     */
    class Intermediate_मानम्_ग्रन्थिः : सार्वजनिक मानम्_ग्रन्थिः {
    सार्वजनिक:
        Intermediate_मानम्_ग्रन्थिः(int32_t v, ग्रन्थिः_2 *अग्र_ग्रन्थिः)
                : मानम्_ग्रन्थिः(0x222222u*37u+hash_संहिता(अग्र_ग्रन्थिः)), अग्र(अग्र_ग्रन्थिः) { समुच्चयः_मानम्(v); }
        आभास bool संक्रियकः==(ध्रुव ग्रन्थिः_2 &other) ध्रुव override;
        आभास int32_t mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या) override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
    रक्षित:
        ग्रन्थिः_2 *अग्र;
    };
#यदि_अन्तः  /* U_HIDE_INTERNAL_API */

    // Do not conditionalize the following with #ifndef U_HIDE_INTERNAL_API,
    // it is needed for layout of other objects.
    /**
     * @internal 
     */
    class रैखिकम्_मेलनम्_ग्रन्थिः : सार्वजनिक मानम्_ग्रन्थिः {
    सार्वजनिक:
        रैखिकम्_मेलनम्_ग्रन्थिः(int32_t len, ग्रन्थिः_2 *अग्र_ग्रन्थिः)
                : मानम्_ग्रन्थिः((0x333333u*37u+len)*37u+hash_संहिता(अग्र_ग्रन्थिः)),
                  दीर्घता(len), अग्र(अग्र_ग्रन्थिः) {}
        आभास bool संक्रियकः==(ध्रुव ग्रन्थिः_2 &other) ध्रुव override;
        आभास int32_t mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या) override;
    रक्षित:
        int32_t दीर्घता;
        ग्रन्थिः_2 *अग्र;
    };

#यदि_अपरिभाषितम् U_HIDE_आन्तरिकम्_API
    /**
     * @internal 
     */
    class Branch_ग्रन्थिः : सार्वजनिक ग्रन्थिः_2 {
    सार्वजनिक:
        Branch_ग्रन्थिः(int32_t initialHash) : ग्रन्थिः_2(initialHash) {}
    रक्षित:
        int32_t first_Edge_सङ्ख्या;
    };

    /**
     * @internal 
     */
    class List_Branch_ग्रन्थिः : सार्वजनिक Branch_ग्रन्थिः {
    सार्वजनिक:
        List_Branch_ग्रन्थिः() : Branch_ग्रन्थिः(0x444444), दीर्घता(0) {}
        आभास bool संक्रियकः==(ध्रुव ग्रन्थिः_2 &other) ध्रुव override;
        आभास int32_t mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या) override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
        // Adds a unit with a final value.
        शून्य सङ्कलय(int32_t c, int32_t मानम्_2) {
            units[दीर्घता] = static_cast<char16_t>(c);
            equal[दीर्घता]=nullptr;
            मानानि[दीर्घता]=मानम्_2;
            ++दीर्घता;
            hash=(hash*37u+c)*37u+मानम्_2;
        }
        // Adds a unit which leads to another match node.
        शून्य सङ्कलय(int32_t c, ग्रन्थिः_2 *ग्रन्थिः) {
            units[दीर्घता] = static_cast<char16_t>(c);
            equal[दीर्घता]=ग्रन्थिः;
            मानानि[दीर्घता]=0;
            ++दीर्घता;
            hash=(hash*37u+c)*37u+hash_संहिता(ग्रन्थिः);
        }
    रक्षित:
        ग्रन्थिः_2 *equal[k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता];  // nullptr means "has final value".
        int32_t दीर्घता;
        int32_t मानानि[k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता];
        char16_t units[k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता];
    };

    /**
     * @internal 
     */
    class विभज_Branch_ग्रन्थिः : सार्वजनिक Branch_ग्रन्थिः {
    सार्वजनिक:
        विभज_Branch_ग्रन्थिः(char16_t मध्यः_Unit, ग्रन्थिः_2 *less_Than_ग्रन्थिः, ग्रन्थिः_2 *greater_Or_Equal_ग्रन्थिः)
                : Branch_ग्रन्थिः(((0x555555u*37u+मध्यः_Unit)*37u+
                              hash_संहिता(less_Than_ग्रन्थिः))*37u+hash_संहिता(greater_Or_Equal_ग्रन्थिः)),
                  unit(मध्यः_Unit), lessThan(less_Than_ग्रन्थिः), greaterOrEqual(greater_Or_Equal_ग्रन्थिः) {}
        आभास bool संक्रियकः==(ध्रुव ग्रन्थिः_2 &other) ध्रुव override;
        आभास int32_t mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या) override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
    रक्षित:
        char16_t unit;
        ग्रन्थिः_2 *lessThan;
        ग्रन्थिः_2 *greaterOrEqual;
    };

    // Branch head node, for writing the actual node lead unit.
    /** @internal */
    class Branch_शीर्ष_ग्रन्थिः : सार्वजनिक मानम्_ग्रन्थिः {
    सार्वजनिक:
        Branch_शीर्ष_ग्रन्थिः(int32_t len, ग्रन्थिः_2 *sub_ग्रन्थिः)
                : मानम्_ग्रन्थिः((0x666666u*37u+len)*37u+hash_संहिता(sub_ग्रन्थिः)),
                  दीर्घता(len), अग्र(sub_ग्रन्थिः) {}
        आभास bool संक्रियकः==(ध्रुव ग्रन्थिः_2 &other) ध्रुव override;
        आभास int32_t mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या) override;
        आभास शून्य लिख(StringTrieBuilder &builder) override;
    रक्षित:
        int32_t दीर्घता;
        ग्रन्थिः_2 *अग्र;  // A branch sub-node.
    };

#यदि_अन्तः  /* U_HIDE_INTERNAL_API */
    /// \endcond

    /** @internal */
    आभास ग्रन्थिः_2 *create_रैखिकम्_मेलनम्_ग्रन्थिः(int32_t i, int32_t unit_सूचकांक, int32_t दीर्घता,
                                        ग्रन्थिः_2 *अग्र_ग्रन्थिः) ध्रुव = 0;

    /** @internal */
    आभास int32_t लिख(int32_t unit) = 0;
    /** @internal */
    आभास int32_t writeElementUnits(int32_t i, int32_t unit_सूचकांक, int32_t दीर्घता) = 0;
    /** @internal */
    आभास int32_t write_मानम्_And_Final(int32_t i, UBool isFinal) = 0;
    /** @internal */
    आभास int32_t write_मानम्_And_Type(UBool has_मानम्, int32_t मानम्_2, int32_t ग्रन्थिः) = 0;
    /** @internal */
    आभास int32_t write_Delta_गन्तव्यम्(int32_t jumpTarget) = 0;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः /* U_SHOW_CPLUSPLUS_API */

#यदि_अन्तः  // __STRINGTRIEBUILDER_H__
