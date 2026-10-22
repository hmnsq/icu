// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  stringtriebuilder.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010dec24
*   created by: Markus W. Scherer
*/

#ಅನ್ತರ್ಭಾವ "utypeinfo.h"  // for 'typeid' to work
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringtriebuilder.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"

U_CDECL_BEGIN

ಸ್ಥಿರ int32_t U_CALLCONV
hash_String_Trie_ಗಂಟು(ಸ್ಥಿರಾಂಕ UHashTok ಕೀಲಿ) {
    ಹಿಂತಿರುಗಿ icu::StringTrieBuilder::hash_ಗಂಟು(ಕೀಲಿ.ಸೂಚಕ);
}

ಸ್ಥಿರ UBool U_CALLCONV
equalStringTrieNodes(ಸ್ಥಿರಾಂಕ UHashTok key1, ಸ್ಥಿರಾಂಕ UHashTok key2) {
    ಹಿಂತಿರುಗಿ icu::StringTrieBuilder::equalNodes(key1.ಸೂಚಕ, key2.ಸೂಚಕ);
}

U_CDECL_ಕೊನೆ

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(ಶೂನ್ಯಸೂಚಕ) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

ಶೂನ್ಯ
StringTrieBuilder::createCompactBuilder(int32_t ಗಾತ್ರ_Guess, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    nodes=uhash_open_ಗಾತ್ರ(hash_String_Trie_ಗಂಟು, equalStringTrieNodes, ಶೂನ್ಯಸೂಚಕ,
                         ಗಾತ್ರ_Guess, &error_ಸಂಕೇತ);
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        ಆದರೆ(nodes==ಶೂನ್ಯಸೂಚಕ) {
          error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ {
          uhash_ಗಣ_ಕೀಲಿ_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

ಶೂನ್ಯ
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=ಶೂನ್ಯಸೂಚಕ;
}

ಶೂನ್ಯ
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elements_ಉದ್ದಳತೆ,
                       UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_ಗಂಟು(0, elements_ಉದ್ದಳತೆ, 0);
    } ಇಲ್ಲದಿದ್ದರೆ /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elements_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
        Node *ಬೇರು=make_ಗಂಟು(0, elements_ಉದ್ದಳತೆ, 0, error_ಸಂಕೇತ);
        ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
            ಬೇರು->mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(-1);
            ಬೇರು->ಲಿಖ(*ಇದು);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_ಗಂಟು(int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, int32_t unit_ಸೂಚ್ಯಂಕ) {
    UBool has_ಮೌಲ್ಯ=ಸುಳ್ಳು;
    int32_t value=0;
    int32_t type;
    ಆದರೆ(unit_ಸೂಚ್ಯಂಕ==get_Element_String_ಉದ್ದಳತೆ(ಆರಂಭ)) {
        // An intermediate or final value.
        value=get_Element_ಮೌಲ್ಯ(ಆರಂಭ++);
        ಆದರೆ(ಆರಂಭ==ಎಲ್ಲೆ) {
            ಹಿಂತಿರುಗಿ write_ಮೌಲ್ಯ_And_Final(value, ನಿಜ);  // final-value node
        }
        has_ಮೌಲ್ಯ=ನಿಜ;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(ಆರಂಭ, unit_ಸೂಚ್ಯಂಕ);
    int32_t maxUnit=getElementUnit(ಎಲ್ಲೆ-1, unit_ಸೂಚ್ಯಂಕ);
    ಆದರೆ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t ಕೊನೆಯ_Unit_ಸೂಚ್ಯಂಕ=get_ಎಲ್ಲೆ_ದ_ರೇಖೀಯ_ಹೊಂದಿಕೆ(ಆರಂಭ, ಎಲ್ಲೆ-1, unit_ಸೂಚ್ಯಂಕ);
        write_ಗಂಟು(ಆರಂಭ, ಎಲ್ಲೆ, ಕೊನೆಯ_Unit_ಸೂಚ್ಯಂಕ);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=ಕೊನೆಯ_Unit_ಸೂಚ್ಯಂಕ-unit_ಸೂಚ್ಯಂಕ;
        int32_t max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=get_Max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ();
        ಆಗ(length>max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ) {
            ಕೊನೆಯ_Unit_ಸೂಚ್ಯಂಕ-=max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
            length-=max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
            writeElementUnits(ಆರಂಭ, ಕೊನೆಯ_Unit_ಸೂಚ್ಯಂಕ, max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ);
            ಲಿಖ(get_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ()+max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ-1);
        }
        writeElementUnits(ಆರಂಭ, unit_ಸೂಚ್ಯಂಕ, length);
        type=get_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ()+length-1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Branch node.
        int32_t length=ಎಣಿಕೆ_Element_Units(ಆರಂಭ, ಎಲ್ಲೆ, unit_ಸೂಚ್ಯಂಕ);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_ಗಂಟು(ಆರಂಭ, ಎಲ್ಲೆ, unit_ಸೂಚ್ಯಂಕ, length);
        ಆದರೆ(--length<get_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ()) {
            type=length;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಲಿಖ(length);
            type=0;
        }
    }
    ಹಿಂತಿರುಗಿ write_ಮೌಲ್ಯ_And_Type(has_ಮೌಲ್ಯ, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_ಗಂಟು(int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, int32_t unit_ಸೂಚ್ಯಂಕ, int32_t length) {
    char16_t ಮಧ್ಯ_Units[k_Max_ವಿಭಜಿಸು_Branch_ಮಟ್ಟಗಳು];
    int32_t lessThan[k_Max_ವಿಭಜಿಸು_Branch_ಮಟ್ಟಗಳು];
    int32_t lt_ಉದ್ದಳತೆ=0;
    ಆಗ(length>get_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(ಆರಂಭ, unit_ಸೂಚ್ಯಂಕ, length/2);
        // Encode the less-than branch first.
        ಮಧ್ಯ_Units[lt_ಉದ್ದಳತೆ]=getElementUnit(i, unit_ಸೂಚ್ಯಂಕ);  // middle unit
        lessThan[lt_ಉದ್ದಳತೆ]=write_Branch_Sub_ಗಂಟು(ಆರಂಭ, i, unit_ಸೂಚ್ಯಂಕ, length/2);
        ++lt_ಉದ್ದಳತೆ;
        // Continue for the greater-or-equal branch.
        ಆರಂಭ=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ];
    UBool isFinal[k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ-1];
    int32_t unit_ಸಂಖ್ಯೆ=0;
    ಮಾಡು {
        int32_t i=starts[unit_ಸಂಖ್ಯೆ]=ಆರಂಭ;
        char16_t unit=getElementUnit(i++, unit_ಸೂಚ್ಯಂಕ);
        i=ಸೂಚ್ಯಂಕ_ದ_Element_With_ಮುಂದಿನ_Unit(i, unit_ಸೂಚ್ಯಂಕ, unit);
        isFinal[unit_ಸಂಖ್ಯೆ]= ಆರಂಭ==i-1 && unit_ಸೂಚ್ಯಂಕ+1==get_Element_String_ಉದ್ದಳತೆ(ಆರಂಭ);
        ಆರಂಭ=i;
    } ಆಗ(++unit_ಸಂಖ್ಯೆ<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_ಸಂಖ್ಯೆ]=ಆರಂಭ;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ-1];
    ಮಾಡು {
        --unit_ಸಂಖ್ಯೆ;
        ಆದರೆ(!isFinal[unit_ಸಂಖ್ಯೆ]) {
            jumpTargets[unit_ಸಂಖ್ಯೆ]=write_ಗಂಟು(starts[unit_ಸಂಖ್ಯೆ], starts[unit_ಸಂಖ್ಯೆ+1], unit_ಸೂಚ್ಯಂಕ+1);
        }
    } ಆಗ(unit_ಸಂಖ್ಯೆ>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_ಸಂಖ್ಯೆ=length-1;
    write_ಗಂಟು(ಆರಂಭ, ಎಲ್ಲೆ, unit_ಸೂಚ್ಯಂಕ+1);
    int32_t ಸ್ಥಾನಾಂತರ=ಲಿಖ(getElementUnit(ಆರಂಭ, unit_ಸೂಚ್ಯಂಕ));
    // Write the rest of this node's unit-value pairs.
    ಆಗ(--unit_ಸಂಖ್ಯೆ>=0) {
        ಆರಂಭ=starts[unit_ಸಂಖ್ಯೆ];
        int32_t value;
        ಆದರೆ(isFinal[unit_ಸಂಖ್ಯೆ]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_ಮೌಲ್ಯ(ಆರಂಭ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Write the delta to the start position of the sub-node.
            value=ಸ್ಥಾನಾಂತರ-jumpTargets[unit_ಸಂಖ್ಯೆ];
        }
        write_ಮೌಲ್ಯ_And_Final(value, isFinal[unit_ಸಂಖ್ಯೆ]);
        ಸ್ಥಾನಾಂತರ=ಲಿಖ(getElementUnit(ಆರಂಭ, unit_ಸೂಚ್ಯಂಕ));
    }
    // Write the split-branch nodes.
    ಆಗ(lt_ಉದ್ದಳತೆ>0) {
        --lt_ಉದ್ದಳತೆ;
        write_Delta_ಗಮ್ಯ(lessThan[lt_ಉದ್ದಳತೆ]);
        ಸ್ಥಾನಾಂತರ=ಲಿಖ(ಮಧ್ಯ_Units[lt_ಉದ್ದಳತೆ]);
    }
    ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_ಗಂಟು(int32_t ಆರಂಭ,
                            int32_t ಎಲ್ಲೆ,
                            int32_t unit_ಸೂಚ್ಯಂಕ,
                            UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    UBool has_ಮೌಲ್ಯ=ಸುಳ್ಳು;
    int32_t value=0;
    ಆದರೆ(unit_ಸೂಚ್ಯಂಕ==get_Element_String_ಉದ್ದಳತೆ(ಆರಂಭ)) {
        // An intermediate or final value.
        value=get_Element_ಮೌಲ್ಯ(ಆರಂಭ++);
        ಆದರೆ(ಆರಂಭ==ಎಲ್ಲೆ) {
            ಹಿಂತಿರುಗಿ ರಿಜಿಸ್ಟರ್_Final_ಮೌಲ್ಯ(value, error_ಸಂಕೇತ);
        }
        has_ಮೌಲ್ಯ=ನಿಜ;
    }
    Node *ಗಂಟು;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(ಆರಂಭ, unit_ಸೂಚ್ಯಂಕ);
    int32_t maxUnit=getElementUnit(ಎಲ್ಲೆ-1, unit_ಸೂಚ್ಯಂಕ);
    ಆದರೆ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t ಕೊನೆಯ_Unit_ಸೂಚ್ಯಂಕ=get_ಎಲ್ಲೆ_ದ_ರೇಖೀಯ_ಹೊಂದಿಕೆ(ಆರಂಭ, ಎಲ್ಲೆ-1, unit_ಸೂಚ್ಯಂಕ);
        Node *ಮುಂದಿನ_ಗಂಟು=make_ಗಂಟು(ಆರಂಭ, ಎಲ್ಲೆ, ಕೊನೆಯ_Unit_ಸೂಚ್ಯಂಕ, error_ಸಂಕೇತ);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=ಕೊನೆಯ_Unit_ಸೂಚ್ಯಂಕ-unit_ಸೂಚ್ಯಂಕ;
        int32_t max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=get_Max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ();
        ಆಗ(length>max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ) {
            ಕೊನೆಯ_Unit_ಸೂಚ್ಯಂಕ-=max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
            length-=max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
            ಗಂಟು=create_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು(ಆರಂಭ, ಕೊನೆಯ_Unit_ಸೂಚ್ಯಂಕ, max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ, ಮುಂದಿನ_ಗಂಟು);
            ಮುಂದಿನ_ಗಂಟು=ರಿಜಿಸ್ಟರ್_ಗಂಟು(ಗಂಟು, error_ಸಂಕೇತ);
        }
        ಗಂಟು=create_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು(ಆರಂಭ, unit_ಸೂಚ್ಯಂಕ, length, ಮುಂದಿನ_ಗಂಟು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Branch node.
        int32_t length=ಎಣಿಕೆ_Element_Units(ಆರಂಭ, ಎಲ್ಲೆ, unit_ಸೂಚ್ಯಂಕ);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_ಗಂಟು=make_Branch_Sub_ಗಂಟು(ಆರಂಭ, ಎಲ್ಲೆ, unit_ಸೂಚ್ಯಂಕ, length, error_ಸಂಕೇತ);
        ಗಂಟು=ಹೊಸ Branch_ತಲೆ_ಗಂಟು(length, sub_ಗಂಟು);
    }
    ಆದರೆ(has_ಮೌಲ್ಯ && ಗಂಟು!=ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(ಹೊಂದಿಕೆ_Nodes_Can_Have_ಮೌಲ್ಯಗಳು()) {
            ((ಮೌಲ್ಯ_ಗಂಟು *)ಗಂಟು)->ಗಣ_ಮೌಲ್ಯ(value);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಂಟು=ಹೊಸ Intermediate_ಮೌಲ್ಯ_ಗಂಟು(value, ರಿಜಿಸ್ಟರ್_ಗಂಟು(ಗಂಟು, error_ಸಂಕೇತ));
        }
    }
    ಹಿಂತಿರುಗಿ ರಿಜಿಸ್ಟರ್_ಗಂಟು(ಗಂಟು, error_ಸಂಕೇತ);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_ಗಂಟು(int32_t ಆರಂಭ,
                                     int32_t ಎಲ್ಲೆ,
                                     int32_t unit_ಸೂಚ್ಯಂಕ,
                                     int32_t length,
                                     UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    char16_t ಮಧ್ಯ_Units[k_Max_ವಿಭಜಿಸು_Branch_ಮಟ್ಟಗಳು];
    Node *lessThan[k_Max_ವಿಭಜಿಸು_Branch_ಮಟ್ಟಗಳು];
    int32_t lt_ಉದ್ದಳತೆ=0;
    ಆಗ(length>get_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(ಆರಂಭ, unit_ಸೂಚ್ಯಂಕ, length/2);
        // Create the less-than branch.
        ಮಧ್ಯ_Units[lt_ಉದ್ದಳತೆ]=getElementUnit(i, unit_ಸೂಚ್ಯಂಕ);  // middle unit
        lessThan[lt_ಉದ್ದಳತೆ]=make_Branch_Sub_ಗಂಟು(ಆರಂಭ, i, unit_ಸೂಚ್ಯಂಕ, length/2, error_ಸಂಕೇತ);
        ++lt_ಉದ್ದಳತೆ;
        // Continue for the greater-or-equal branch.
        ಆರಂಭ=i;
        length=length-length/2;
    }
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    List_Branch_ಗಂಟು *list_ಗಂಟು=ಹೊಸ List_Branch_ಗಂಟು();
    ಆದರೆ(list_ಗಂಟು==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_ಸಂಖ್ಯೆ=0;
    ಮಾಡು {
        int32_t i=ಆರಂಭ;
        char16_t unit=getElementUnit(i++, unit_ಸೂಚ್ಯಂಕ);
        i=ಸೂಚ್ಯಂಕ_ದ_Element_With_ಮುಂದಿನ_Unit(i, unit_ಸೂಚ್ಯಂಕ, unit);
        ಆದರೆ(ಆರಂಭ==i-1 && unit_ಸೂಚ್ಯಂಕ+1==get_Element_String_ಉದ್ದಳತೆ(ಆರಂಭ)) {
            list_ಗಂಟು->ಕೂಡಿಸು(unit, get_Element_ಮೌಲ್ಯ(ಆರಂಭ));
        } ಇಲ್ಲದಿದ್ದರೆ {
            list_ಗಂಟು->ಕೂಡಿಸು(unit, make_ಗಂಟು(ಆರಂಭ, i, unit_ಸೂಚ್ಯಂಕ+1, error_ಸಂಕೇತ));
        }
        ಆರಂಭ=i;
    } ಆಗ(++unit_ಸಂಖ್ಯೆ<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(ಆರಂಭ, unit_ಸೂಚ್ಯಂಕ);
    ಆದರೆ(ಆರಂಭ==ಎಲ್ಲೆ-1 && unit_ಸೂಚ್ಯಂಕ+1==get_Element_String_ಉದ್ದಳತೆ(ಆರಂಭ)) {
        list_ಗಂಟು->ಕೂಡಿಸು(unit, get_Element_ಮೌಲ್ಯ(ಆರಂಭ));
    } ಇಲ್ಲದಿದ್ದರೆ {
        list_ಗಂಟು->ಕೂಡಿಸು(unit, make_ಗಂಟು(ಆರಂಭ, ಎಲ್ಲೆ, unit_ಸೂಚ್ಯಂಕ+1, error_ಸಂಕೇತ));
    }
    Node *ಗಂಟು=ರಿಜಿಸ್ಟರ್_ಗಂಟು(list_ಗಂಟು, error_ಸಂಕೇತ);
    // Create the split-branch nodes.
    ಆಗ(lt_ಉದ್ದಳತೆ>0) {
        --lt_ಉದ್ದಳತೆ;
        ಗಂಟು=ರಿಜಿಸ್ಟರ್_ಗಂಟು(
            ಹೊಸ ವಿಭಜಿಸು_Branch_ಗಂಟು(ಮಧ್ಯ_Units[lt_ಉದ್ದಳತೆ], lessThan[lt_ಉದ್ದಳತೆ], ಗಂಟು), error_ಸಂಕೇತ);
    }
    ಹಿಂತಿರುಗಿ ಗಂಟು;
}

StringTrieBuilder::Node*
StringTrieBuilder::ರಿಜಿಸ್ಟರ್_ಗಂಟು(Node* new_ಗಂಟು, UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಅಳಿಸು new_ಗಂಟು;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(new_ಗಂಟು==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ UHashElement *old=uhash_ಕಂಡುಹಿಡಿ(nodes, new_ಗಂಟು);
    ಆದರೆ(old!=ಶೂನ್ಯಸೂಚಕ) {
        ಅಳಿಸು new_ಗಂಟು;
        ಹಿಂತಿರುಗಿ static_cast<Node*>(old->ಕೀಲಿ.ಸೂಚಕ);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ಆದರೆ U_DEBUG
    int32_t old_ಮೌಲ್ಯ=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#ಆದರೆ_ಅಂತ್ಯ
    uhash_puti(nodes, new_ಗಂಟು, 1, &error_ಸಂಕೇತ);
    U_ASSERT(old_ಮೌಲ್ಯ==0);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಅಳಿಸು new_ಗಂಟು;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ new_ಗಂಟು;
}

StringTrieBuilder::Node*
StringTrieBuilder::ರಿಜಿಸ್ಟರ್_Final_ಮೌಲ್ಯ(int32_t value, UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    Final_ಮೌಲ್ಯ_ಗಂಟು ಕೀಲಿ(value);
    ಸ್ಥಿರಾಂಕ UHashElement *old=uhash_ಕಂಡುಹಿಡಿ(nodes, &ಕೀಲಿ);
    ಆದರೆ(old!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ static_cast<Node*>(old->ಕೀಲಿ.ಸೂಚಕ);
    }
    Node *new_ಗಂಟು=ಹೊಸ Final_ಮೌಲ್ಯ_ಗಂಟು(value);
    ಆದರೆ(new_ಗಂಟು==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ಆದರೆ U_DEBUG
    int32_t old_ಮೌಲ್ಯ=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#ಆದರೆ_ಅಂತ್ಯ
    uhash_puti(nodes, new_ಗಂಟು, 1, &error_ಸಂಕೇತ);
    U_ASSERT(old_ಮೌಲ್ಯ==0);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಅಳಿಸು new_ಗಂಟು;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ new_ಗಂಟು;
}

int32_t
StringTrieBuilder::hash_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಗಂಟು) {
    ಹಿಂತಿರುಗಿ static_cast<ಸ್ಥಿರಾಂಕ Node*>(ಗಂಟು)->hash_ಸಂಕೇತ();
}

UBool
StringTrieBuilder::equalNodes(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    ಹಿಂತಿರುಗಿ *static_cast<ಸ್ಥಿರಾಂಕ Node*>(ಎಡ) == *static_cast<ಸ್ಥಿರಾಂಕ Node*>(ಬಲ);
}

ತಾರ್ಕಿಕ
StringTrieBuilder::Node::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Node &other) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಇದು==&other || (typeid(*ಇದು)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ) {
    ಆದರೆ(ಸ್ಥಾನಾಂತರ==0) {
        ಸ್ಥಾನಾಂತರ=ಅಂಚು_ಸಂಖ್ಯೆ;
    }
    ಹಿಂತಿರುಗಿ ಅಂಚು_ಸಂಖ್ಯೆ;
}

ತಾರ್ಕಿಕ
StringTrieBuilder::Final_ಮೌಲ್ಯ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Node &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(!Node::ಕ್ರಿಯಾಕಾರಕ==(other)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ Final_ಮೌಲ್ಯ_ಗಂಟು &o=static_cast<ಸ್ಥಿರಾಂಕ Final_ಮೌಲ್ಯ_ಗಂಟು &>(other);
    ಹಿಂತಿರುಗಿ value==o.value;
}

ಶೂನ್ಯ
StringTrieBuilder::Final_ಮೌಲ್ಯ_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    ಸ್ಥಾನಾಂತರ=builder.write_ಮೌಲ್ಯ_And_Final(value, ನಿಜ);
}

ತಾರ್ಕಿಕ
StringTrieBuilder::ಮೌಲ್ಯ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Node &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(!Node::ಕ್ರಿಯಾಕಾರಕ==(other)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ ಮೌಲ್ಯ_ಗಂಟು &o=static_cast<ಸ್ಥಿರಾಂಕ ಮೌಲ್ಯ_ಗಂಟು &>(other);
    ಹಿಂತಿರುಗಿ has_ಮೌಲ್ಯ==o.has_ಮೌಲ್ಯ && (!has_ಮೌಲ್ಯ || value==o.value);
}

ತಾರ್ಕಿಕ
StringTrieBuilder::Intermediate_ಮೌಲ್ಯ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Node &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(!ಮೌಲ್ಯ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(other)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ Intermediate_ಮೌಲ್ಯ_ಗಂಟು &o=static_cast<ಸ್ಥಿರಾಂಕ Intermediate_ಮೌಲ್ಯ_ಗಂಟು &>(other);
    ಹಿಂತಿರುಗಿ next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_ಮೌಲ್ಯ_ಗಂಟು::mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ) {
    ಆದರೆ(ಸ್ಥಾನಾಂತರ==0) {
        ಸ್ಥಾನಾಂತರ=ಅಂಚು_ಸಂಖ್ಯೆ=next->mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(ಅಂಚು_ಸಂಖ್ಯೆ);
    }
    ಹಿಂತಿರುಗಿ ಅಂಚು_ಸಂಖ್ಯೆ;
}

ಶೂನ್ಯ
StringTrieBuilder::Intermediate_ಮೌಲ್ಯ_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    next->ಲಿಖ(builder);
    ಸ್ಥಾನಾಂತರ=builder.write_ಮೌಲ್ಯ_And_Final(value, ಸುಳ್ಳು);
}

ತಾರ್ಕಿಕ
StringTrieBuilder::ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Node &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(!ಮೌಲ್ಯ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(other)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು &o=static_cast<ಸ್ಥಿರಾಂಕ ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು &>(other);
    ಹಿಂತಿರುಗಿ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಗಂಟು::mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ) {
    ಆದರೆ(ಸ್ಥಾನಾಂತರ==0) {
        ಸ್ಥಾನಾಂತರ=ಅಂಚು_ಸಂಖ್ಯೆ=next->mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(ಅಂಚು_ಸಂಖ್ಯೆ);
    }
    ಹಿಂತಿರುಗಿ ಅಂಚು_ಸಂಖ್ಯೆ;
}

ತಾರ್ಕಿಕ
StringTrieBuilder::List_Branch_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Node &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(!Node::ಕ್ರಿಯಾಕಾರಕ==(other)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ List_Branch_ಗಂಟು &o=static_cast<ಸ್ಥಿರಾಂಕ List_Branch_ಗಂಟು &>(other);
    ಗಾಗಿ(int32_t i=0; i<length; ++i) {
        ಆದರೆ(units[i]!=o.units[i] || ಮೌಲ್ಯಗಳು[i]!=o.ಮೌಲ್ಯಗಳು[i] || equal[i]!=o.equal[i]) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

int32_t
StringTrieBuilder::List_Branch_ಗಂಟು::mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ) {
    ಆದರೆ(ಸ್ಥಾನಾಂತರ==0) {
        ಮೊದಲ_ಅಂಚು_ಸಂಖ್ಯೆ=ಅಂಚು_ಸಂಖ್ಯೆ;
        int32_t ಹೆಜ್ಜೆ=0;
        int32_t i=length;
        ಮಾಡು {
            Node *ಅಂಚು=equal[--i];
            ಆದರೆ(ಅಂಚು!=ಶೂನ್ಯಸೂಚಕ) {
                ಅಂಚು_ಸಂಖ್ಯೆ=ಅಂಚು->mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(ಅಂಚು_ಸಂಖ್ಯೆ-ಹೆಜ್ಜೆ);
            }
            // For all but the rightmost edge, decrement the edge number.
            ಹೆಜ್ಜೆ=1;
        } ಆಗ(i>0);
        ಸ್ಥಾನಾಂತರ=ಅಂಚು_ಸಂಖ್ಯೆ;
    }
    ಹಿಂತಿರುಗಿ ಅಂಚು_ಸಂಖ್ಯೆ;
}

ಶೂನ್ಯ
StringTrieBuilder::List_Branch_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_ಸಂಖ್ಯೆ=length-1;
    Node *ಬಲ_ಅಂಚು=equal[unit_ಸಂಖ್ಯೆ];
    int32_t ಬಲ_ಅಂಚು_ಸಂಖ್ಯೆ= ಬಲ_ಅಂಚು==ಶೂನ್ಯಸೂಚಕ ? ಮೊದಲ_ಅಂಚು_ಸಂಖ್ಯೆ : ಬಲ_ಅಂಚು->get_ಸ್ಥಾನಾಂತರ();
    ಮಾಡು {
        --unit_ಸಂಖ್ಯೆ;
        ಆದರೆ(equal[unit_ಸಂಖ್ಯೆ]!=ಶೂನ್ಯಸೂಚಕ) {
            equal[unit_ಸಂಖ್ಯೆ]->write_Unless_Inside_ಬಲ_ಅಂಚು(ಮೊದಲ_ಅಂಚು_ಸಂಖ್ಯೆ, ಬಲ_ಅಂಚು_ಸಂಖ್ಯೆ, builder);
        }
    } ಆಗ(unit_ಸಂಖ್ಯೆ>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_ಸಂಖ್ಯೆ=length-1;
    ಆದರೆ(ಬಲ_ಅಂಚು==ಶೂನ್ಯಸೂಚಕ) {
        builder.write_ಮೌಲ್ಯ_And_Final(ಮೌಲ್ಯಗಳು[unit_ಸಂಖ್ಯೆ], ನಿಜ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬಲ_ಅಂಚು->ಲಿಖ(builder);
    }
    ಸ್ಥಾನಾಂತರ=builder.ಲಿಖ(units[unit_ಸಂಖ್ಯೆ]);
    // Write the rest of this node's unit-value pairs.
    ಆಗ(--unit_ಸಂಖ್ಯೆ>=0) {
        int32_t value;
        UBool isFinal;
        ಆದರೆ(equal[unit_ಸಂಖ್ಯೆ]==ಶೂನ್ಯಸೂಚಕ) {
            // Write the final value for the one string ending with this unit.
            value=ಮೌಲ್ಯಗಳು[unit_ಸಂಖ್ಯೆ];
            isFinal=ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_ಸಂಖ್ಯೆ]->get_ಸ್ಥಾನಾಂತರ()>0);
            value=ಸ್ಥಾನಾಂತರ-equal[unit_ಸಂಖ್ಯೆ]->get_ಸ್ಥಾನಾಂತರ();
            isFinal=ಸುಳ್ಳು;
        }
        builder.write_ಮೌಲ್ಯ_And_Final(value, isFinal);
        ಸ್ಥಾನಾಂತರ=builder.ಲಿಖ(units[unit_ಸಂಖ್ಯೆ]);
    }
}

ತಾರ್ಕಿಕ
StringTrieBuilder::ವಿಭಜಿಸು_Branch_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Node &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(!Node::ಕ್ರಿಯಾಕಾರಕ==(other)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ ವಿಭಜಿಸು_Branch_ಗಂಟು &o=static_cast<ಸ್ಥಿರಾಂಕ ವಿಭಜಿಸು_Branch_ಗಂಟು &>(other);
    ಹಿಂತಿರುಗಿ unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::ವಿಭಜಿಸು_Branch_ಗಂಟು::mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ) {
    ಆದರೆ(ಸ್ಥಾನಾಂತರ==0) {
        ಮೊದಲ_ಅಂಚು_ಸಂಖ್ಯೆ=ಅಂಚು_ಸಂಖ್ಯೆ;
        ಅಂಚು_ಸಂಖ್ಯೆ=greaterOrEqual->mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(ಅಂಚು_ಸಂಖ್ಯೆ);
        ಸ್ಥಾನಾಂತರ=ಅಂಚು_ಸಂಖ್ಯೆ=lessThan->mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(ಅಂಚು_ಸಂಖ್ಯೆ-1);
    }
    ಹಿಂತಿರುಗಿ ಅಂಚು_ಸಂಖ್ಯೆ;
}

ಶೂನ್ಯ
StringTrieBuilder::ವಿಭಜಿಸು_Branch_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_ಬಲ_ಅಂಚು(ಮೊದಲ_ಅಂಚು_ಸಂಖ್ಯೆ, greaterOrEqual->get_ಸ್ಥಾನಾಂತರ(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->ಲಿಖ(builder);
    // Write this node.
    U_ASSERT(lessThan->get_ಸ್ಥಾನಾಂತರ()>0);
    builder.write_Delta_ಗಮ್ಯ(lessThan->get_ಸ್ಥಾನಾಂತರ());  // less-than
    ಸ್ಥಾನಾಂತರ=builder.ಲಿಖ(unit);
}

ತಾರ್ಕಿಕ
StringTrieBuilder::Branch_ತಲೆ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Node &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ(!ಮೌಲ್ಯ_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(other)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ Branch_ತಲೆ_ಗಂಟು &o=static_cast<ಸ್ಥಿರಾಂಕ Branch_ತಲೆ_ಗಂಟು &>(other);
    ಹಿಂತಿರುಗಿ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_ತಲೆ_ಗಂಟು::mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(int32_t ಅಂಚು_ಸಂಖ್ಯೆ) {
    ಆದರೆ(ಸ್ಥಾನಾಂತರ==0) {
        ಸ್ಥಾನಾಂತರ=ಅಂಚು_ಸಂಖ್ಯೆ=next->mark_ಬಲ_ಅಂಚುಗಳು_ಮೊದಲ(ಅಂಚು_ಸಂಖ್ಯೆ);
    }
    ಹಿಂತಿರುಗಿ ಅಂಚು_ಸಂಖ್ಯೆ;
}

ಶೂನ್ಯ
StringTrieBuilder::Branch_ತಲೆ_ಗಂಟು::ಲಿಖ(StringTrieBuilder &builder) {
    next->ಲಿಖ(builder);
    ಆದರೆ(length<=builder.get_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ()) {
        ಸ್ಥಾನಾಂತರ=builder.write_ಮೌಲ್ಯ_And_Type(has_ಮೌಲ್ಯ, value, length-1);
    } ಇಲ್ಲದಿದ್ದರೆ {
        builder.ಲಿಖ(length-1);
        ಸ್ಥಾನಾಂತರ=builder.write_ಮೌಲ್ಯ_And_Type(has_ಮೌಲ್ಯ, value, 0);
    }
}

U_NAMESPACE_ಕೊನೆ
