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

#అన్తర్భావ "utypeinfo.h"  // for 'typeid' to work
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/stringtriebuilder.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "uhash.h"

U_CDECL_BEGIN

స్థిర int32_t U_CALLCONV
hash_String_Trie_కణుపు(స్థిరరాశి UHashTok కీలకం) {
    తిరిగి icu::StringTrieBuilder::hash_కణుపు(కీలకం.సూచిక);
}

స్థిర UBool U_CALLCONV
equalStringTrieNodes(స్థిరరాశి UHashTok key1, స్థిరరాశి UHashTok key2) {
    తిరిగి icu::StringTrieBuilder::equalNodes(key1.సూచిక, key2.సూచిక);
}

U_CDECL_చివర

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(శూన్యసూచిక) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

శూన్య
StringTrieBuilder::createCompactBuilder(int32_t కొలత_Guess, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    nodes=uhash_open_కొలత(hash_String_Trie_కణుపు, equalStringTrieNodes, శూన్యసూచిక,
                         కొలత_Guess, &error_సంకేతం);
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        ఒకవేళ(nodes==శూన్యసూచిక) {
          error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే {
          uhash_సమితి_కీలకం_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

శూన్య
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=శూన్యసూచిక;
}

శూన్య
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elements_నిడివి,
                       UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_కణుపు(0, elements_నిడివి, 0);
    } లేకపోతే /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elements_నిడివి, error_సంకేతం);
        Node *మూలం=make_కణుపు(0, elements_నిడివి, 0, error_సంకేతం);
        ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
            మూలం->mark_కుడి_అంచులు_మొదటి(-1);
            మూలం->లిఖ(*ఇది);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_కణుపు(int32_t ప్రారంభం, int32_t హద్దు, int32_t unit_సూచీ) {
    UBool has_విలువ=అబద్ధం;
    int32_t value=0;
    int32_t type;
    ఒకవేళ(unit_సూచీ==get_Element_String_నిడివి(ప్రారంభం)) {
        // An intermediate or final value.
        value=get_Element_విలువ(ప్రారంభం++);
        ఒకవేళ(ప్రారంభం==హద్దు) {
            తిరిగి write_విలువ_And_Final(value, నిజం);  // final-value node
        }
        has_విలువ=నిజం;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(ప్రారంభం, unit_సూచీ);
    int32_t maxUnit=getElementUnit(హద్దు-1, unit_సూచీ);
    ఒకవేళ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t చివరి_Unit_సూచీ=get_హద్దు_యొక్క_రేఖీయ_సరిపోలిక(ప్రారంభం, హద్దు-1, unit_సూచీ);
        write_కణుపు(ప్రారంభం, హద్దు, చివరి_Unit_సూచీ);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=చివరి_Unit_సూచీ-unit_సూచీ;
        int32_t max_రేఖీయ_సరిపోలిక_నిడివి=get_Max_రేఖీయ_సరిపోలిక_నిడివి();
        అయితే(length>max_రేఖీయ_సరిపోలిక_నిడివి) {
            చివరి_Unit_సూచీ-=max_రేఖీయ_సరిపోలిక_నిడివి;
            length-=max_రేఖీయ_సరిపోలిక_నిడివి;
            writeElementUnits(ప్రారంభం, చివరి_Unit_సూచీ, max_రేఖీయ_సరిపోలిక_నిడివి);
            లిఖ(get_Min_రేఖీయ_సరిపోలిక()+max_రేఖీయ_సరిపోలిక_నిడివి-1);
        }
        writeElementUnits(ప్రారంభం, unit_సూచీ, length);
        type=get_Min_రేఖీయ_సరిపోలిక()+length-1;
    } లేకపోతే {
        // Branch node.
        int32_t length=లెక్క_Element_Units(ప్రారంభం, హద్దు, unit_సూచీ);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_కణుపు(ప్రారంభం, హద్దు, unit_సూచీ, length);
        ఒకవేళ(--length<get_Min_రేఖీయ_సరిపోలిక()) {
            type=length;
        } లేకపోతే {
            లిఖ(length);
            type=0;
        }
    }
    తిరిగి write_విలువ_And_Type(has_విలువ, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_కణుపు(int32_t ప్రారంభం, int32_t హద్దు, int32_t unit_సూచీ, int32_t length) {
    char16_t మధ్య_Units[k_Max_విభజించు_Branch_స్థాయిలు];
    int32_t lessThan[k_Max_విభజించు_Branch_స్థాయిలు];
    int32_t lt_నిడివి=0;
    అయితే(length>get_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(ప్రారంభం, unit_సూచీ, length/2);
        // Encode the less-than branch first.
        మధ్య_Units[lt_నిడివి]=getElementUnit(i, unit_సూచీ);  // middle unit
        lessThan[lt_నిడివి]=write_Branch_Sub_కణుపు(ప్రారంభం, i, unit_సూచీ, length/2);
        ++lt_నిడివి;
        // Continue for the greater-or-equal branch.
        ప్రారంభం=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి];
    UBool isFinal[k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి-1];
    int32_t unit_సంఖ్య=0;
    చేయి {
        int32_t i=starts[unit_సంఖ్య]=ప్రారంభం;
        char16_t unit=getElementUnit(i++, unit_సూచీ);
        i=సూచీ_యొక్క_Element_With_తదుపరి_Unit(i, unit_సూచీ, unit);
        isFinal[unit_సంఖ్య]= ప్రారంభం==i-1 && unit_సూచీ+1==get_Element_String_నిడివి(ప్రారంభం);
        ప్రారంభం=i;
    } అయితే(++unit_సంఖ్య<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_సంఖ్య]=ప్రారంభం;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి-1];
    చేయి {
        --unit_సంఖ్య;
        ఒకవేళ(!isFinal[unit_సంఖ్య]) {
            jumpTargets[unit_సంఖ్య]=write_కణుపు(starts[unit_సంఖ్య], starts[unit_సంఖ్య+1], unit_సూచీ+1);
        }
    } అయితే(unit_సంఖ్య>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_సంఖ్య=length-1;
    write_కణుపు(ప్రారంభం, హద్దు, unit_సూచీ+1);
    int32_t స్థానభ్రంశం=లిఖ(getElementUnit(ప్రారంభం, unit_సూచీ));
    // Write the rest of this node's unit-value pairs.
    అయితే(--unit_సంఖ్య>=0) {
        ప్రారంభం=starts[unit_సంఖ్య];
        int32_t value;
        ఒకవేళ(isFinal[unit_సంఖ్య]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_విలువ(ప్రారంభం);
        } లేకపోతే {
            // Write the delta to the start position of the sub-node.
            value=స్థానభ్రంశం-jumpTargets[unit_సంఖ్య];
        }
        write_విలువ_And_Final(value, isFinal[unit_సంఖ్య]);
        స్థానభ్రంశం=లిఖ(getElementUnit(ప్రారంభం, unit_సూచీ));
    }
    // Write the split-branch nodes.
    అయితే(lt_నిడివి>0) {
        --lt_నిడివి;
        write_Delta_గమ్యం(lessThan[lt_నిడివి]);
        స్థానభ్రంశం=లిఖ(మధ్య_Units[lt_నిడివి]);
    }
    తిరిగి స్థానభ్రంశం;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_కణుపు(int32_t ప్రారంభం,
                            int32_t హద్దు,
                            int32_t unit_సూచీ,
                            UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    UBool has_విలువ=అబద్ధం;
    int32_t value=0;
    ఒకవేళ(unit_సూచీ==get_Element_String_నిడివి(ప్రారంభం)) {
        // An intermediate or final value.
        value=get_Element_విలువ(ప్రారంభం++);
        ఒకవేళ(ప్రారంభం==హద్దు) {
            తిరిగి రిజిస్టరు_Final_విలువ(value, error_సంకేతం);
        }
        has_విలువ=నిజం;
    }
    Node *కణుపు;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(ప్రారంభం, unit_సూచీ);
    int32_t maxUnit=getElementUnit(హద్దు-1, unit_సూచీ);
    ఒకవేళ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t చివరి_Unit_సూచీ=get_హద్దు_యొక్క_రేఖీయ_సరిపోలిక(ప్రారంభం, హద్దు-1, unit_సూచీ);
        Node *తదుపరి_కణుపు=make_కణుపు(ప్రారంభం, హద్దు, చివరి_Unit_సూచీ, error_సంకేతం);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=చివరి_Unit_సూచీ-unit_సూచీ;
        int32_t max_రేఖీయ_సరిపోలిక_నిడివి=get_Max_రేఖీయ_సరిపోలిక_నిడివి();
        అయితే(length>max_రేఖీయ_సరిపోలిక_నిడివి) {
            చివరి_Unit_సూచీ-=max_రేఖీయ_సరిపోలిక_నిడివి;
            length-=max_రేఖీయ_సరిపోలిక_నిడివి;
            కణుపు=create_రేఖీయ_సరిపోలిక_కణుపు(ప్రారంభం, చివరి_Unit_సూచీ, max_రేఖీయ_సరిపోలిక_నిడివి, తదుపరి_కణుపు);
            తదుపరి_కణుపు=రిజిస్టరు_కణుపు(కణుపు, error_సంకేతం);
        }
        కణుపు=create_రేఖీయ_సరిపోలిక_కణుపు(ప్రారంభం, unit_సూచీ, length, తదుపరి_కణుపు);
    } లేకపోతే {
        // Branch node.
        int32_t length=లెక్క_Element_Units(ప్రారంభం, హద్దు, unit_సూచీ);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_కణుపు=make_Branch_Sub_కణుపు(ప్రారంభం, హద్దు, unit_సూచీ, length, error_సంకేతం);
        కణుపు=కొత్త Branch_తల_కణుపు(length, sub_కణుపు);
    }
    ఒకవేళ(has_విలువ && కణుపు!=శూన్యసూచిక) {
        ఒకవేళ(సరిపోలిక_Nodes_Can_Have_విలువలు()) {
            ((విలువ_కణుపు *)కణుపు)->సమితి_విలువ(value);
        } లేకపోతే {
            కణుపు=కొత్త Intermediate_విలువ_కణుపు(value, రిజిస్టరు_కణుపు(కణుపు, error_సంకేతం));
        }
    }
    తిరిగి రిజిస్టరు_కణుపు(కణుపు, error_సంకేతం);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_కణుపు(int32_t ప్రారంభం,
                                     int32_t హద్దు,
                                     int32_t unit_సూచీ,
                                     int32_t length,
                                     UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    char16_t మధ్య_Units[k_Max_విభజించు_Branch_స్థాయిలు];
    Node *lessThan[k_Max_విభజించు_Branch_స్థాయిలు];
    int32_t lt_నిడివి=0;
    అయితే(length>get_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(ప్రారంభం, unit_సూచీ, length/2);
        // Create the less-than branch.
        మధ్య_Units[lt_నిడివి]=getElementUnit(i, unit_సూచీ);  // middle unit
        lessThan[lt_నిడివి]=make_Branch_Sub_కణుపు(ప్రారంభం, i, unit_సూచీ, length/2, error_సంకేతం);
        ++lt_నిడివి;
        // Continue for the greater-or-equal branch.
        ప్రారంభం=i;
        length=length-length/2;
    }
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    List_Branch_కణుపు *list_కణుపు=కొత్త List_Branch_కణుపు();
    ఒకవేళ(list_కణుపు==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_సంఖ్య=0;
    చేయి {
        int32_t i=ప్రారంభం;
        char16_t unit=getElementUnit(i++, unit_సూచీ);
        i=సూచీ_యొక్క_Element_With_తదుపరి_Unit(i, unit_సూచీ, unit);
        ఒకవేళ(ప్రారంభం==i-1 && unit_సూచీ+1==get_Element_String_నిడివి(ప్రారంభం)) {
            list_కణుపు->కూడు(unit, get_Element_విలువ(ప్రారంభం));
        } లేకపోతే {
            list_కణుపు->కూడు(unit, make_కణుపు(ప్రారంభం, i, unit_సూచీ+1, error_సంకేతం));
        }
        ప్రారంభం=i;
    } అయితే(++unit_సంఖ్య<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(ప్రారంభం, unit_సూచీ);
    ఒకవేళ(ప్రారంభం==హద్దు-1 && unit_సూచీ+1==get_Element_String_నిడివి(ప్రారంభం)) {
        list_కణుపు->కూడు(unit, get_Element_విలువ(ప్రారంభం));
    } లేకపోతే {
        list_కణుపు->కూడు(unit, make_కణుపు(ప్రారంభం, హద్దు, unit_సూచీ+1, error_సంకేతం));
    }
    Node *కణుపు=రిజిస్టరు_కణుపు(list_కణుపు, error_సంకేతం);
    // Create the split-branch nodes.
    అయితే(lt_నిడివి>0) {
        --lt_నిడివి;
        కణుపు=రిజిస్టరు_కణుపు(
            కొత్త విభజించు_Branch_కణుపు(మధ్య_Units[lt_నిడివి], lessThan[lt_నిడివి], కణుపు), error_సంకేతం);
    }
    తిరిగి కణుపు;
}

StringTrieBuilder::Node*
StringTrieBuilder::రిజిస్టరు_కణుపు(Node* new_కణుపు, UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తొలగించు new_కణుపు;
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(new_కణుపు==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి UHashElement *old=uhash_కనుగొను(nodes, new_కణుపు);
    ఒకవేళ(old!=శూన్యసూచిక) {
        తొలగించు new_కణుపు;
        తిరిగి static_cast<Node*>(old->కీలకం.సూచిక);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ఒకవేళ U_DEBUG
    int32_t old_విలువ=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#అయితే_ముగింపు
    uhash_puti(nodes, new_కణుపు, 1, &error_సంకేతం);
    U_ASSERT(old_విలువ==0);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తొలగించు new_కణుపు;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి new_కణుపు;
}

StringTrieBuilder::Node*
StringTrieBuilder::రిజిస్టరు_Final_విలువ(int32_t value, UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    Final_విలువ_కణుపు కీలకం(value);
    స్థిరరాశి UHashElement *old=uhash_కనుగొను(nodes, &కీలకం);
    ఒకవేళ(old!=శూన్యసూచిక) {
        తిరిగి static_cast<Node*>(old->కీలకం.సూచిక);
    }
    Node *new_కణుపు=కొత్త Final_విలువ_కణుపు(value);
    ఒకవేళ(new_కణుపు==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ఒకవేళ U_DEBUG
    int32_t old_విలువ=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#అయితే_ముగింపు
    uhash_puti(nodes, new_కణుపు, 1, &error_సంకేతం);
    U_ASSERT(old_విలువ==0);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తొలగించు new_కణుపు;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి new_కణుపు;
}

int32_t
StringTrieBuilder::hash_కణుపు(స్థిరరాశి శూన్య *కణుపు) {
    తిరిగి static_cast<స్థిరరాశి Node*>(కణుపు)->hash_సంకేతం();
}

UBool
StringTrieBuilder::equalNodes(స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    తిరిగి *static_cast<స్థిరరాశి Node*>(ఎడమ) == *static_cast<స్థిరరాశి Node*>(కుడి);
}

తార్కిక
StringTrieBuilder::Node::క్రియాకారకం==(స్థిరరాశి Node &other) స్థిరరాశి {
    తిరిగి ఇది==&other || (typeid(*ఇది)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య) {
    ఒకవేళ(స్థానభ్రంశం==0) {
        స్థానభ్రంశం=అంచు_సంఖ్య;
    }
    తిరిగి అంచు_సంఖ్య;
}

తార్కిక
StringTrieBuilder::Final_విలువ_కణుపు::క్రియాకారకం==(స్థిరరాశి Node &other) స్థిరరాశి {
    ఒకవేళ(ఇది==&other) {
        తిరిగి నిజం;
    }
    ఒకవేళ(!Node::క్రియాకారకం==(other)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి Final_విలువ_కణుపు &o=static_cast<స్థిరరాశి Final_విలువ_కణుపు &>(other);
    తిరిగి value==o.value;
}

శూన్య
StringTrieBuilder::Final_విలువ_కణుపు::లిఖ(StringTrieBuilder &builder) {
    స్థానభ్రంశం=builder.write_విలువ_And_Final(value, నిజం);
}

తార్కిక
StringTrieBuilder::విలువ_కణుపు::క్రియాకారకం==(స్థిరరాశి Node &other) స్థిరరాశి {
    ఒకవేళ(ఇది==&other) {
        తిరిగి నిజం;
    }
    ఒకవేళ(!Node::క్రియాకారకం==(other)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి విలువ_కణుపు &o=static_cast<స్థిరరాశి విలువ_కణుపు &>(other);
    తిరిగి has_విలువ==o.has_విలువ && (!has_విలువ || value==o.value);
}

తార్కిక
StringTrieBuilder::Intermediate_విలువ_కణుపు::క్రియాకారకం==(స్థిరరాశి Node &other) స్థిరరాశి {
    ఒకవేళ(ఇది==&other) {
        తిరిగి నిజం;
    }
    ఒకవేళ(!విలువ_కణుపు::క్రియాకారకం==(other)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి Intermediate_విలువ_కణుపు &o=static_cast<స్థిరరాశి Intermediate_విలువ_కణుపు &>(other);
    తిరిగి next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_విలువ_కణుపు::mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య) {
    ఒకవేళ(స్థానభ్రంశం==0) {
        స్థానభ్రంశం=అంచు_సంఖ్య=next->mark_కుడి_అంచులు_మొదటి(అంచు_సంఖ్య);
    }
    తిరిగి అంచు_సంఖ్య;
}

శూన్య
StringTrieBuilder::Intermediate_విలువ_కణుపు::లిఖ(StringTrieBuilder &builder) {
    next->లిఖ(builder);
    స్థానభ్రంశం=builder.write_విలువ_And_Final(value, అబద్ధం);
}

తార్కిక
StringTrieBuilder::రేఖీయ_సరిపోలిక_కణుపు::క్రియాకారకం==(స్థిరరాశి Node &other) స్థిరరాశి {
    ఒకవేళ(ఇది==&other) {
        తిరిగి నిజం;
    }
    ఒకవేళ(!విలువ_కణుపు::క్రియాకారకం==(other)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి రేఖీయ_సరిపోలిక_కణుపు &o=static_cast<స్థిరరాశి రేఖీయ_సరిపోలిక_కణుపు &>(other);
    తిరిగి length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::రేఖీయ_సరిపోలిక_కణుపు::mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య) {
    ఒకవేళ(స్థానభ్రంశం==0) {
        స్థానభ్రంశం=అంచు_సంఖ్య=next->mark_కుడి_అంచులు_మొదటి(అంచు_సంఖ్య);
    }
    తిరిగి అంచు_సంఖ్య;
}

తార్కిక
StringTrieBuilder::List_Branch_కణుపు::క్రియాకారకం==(స్థిరరాశి Node &other) స్థిరరాశి {
    ఒకవేళ(ఇది==&other) {
        తిరిగి నిజం;
    }
    ఒకవేళ(!Node::క్రియాకారకం==(other)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి List_Branch_కణుపు &o=static_cast<స్థిరరాశి List_Branch_కణుపు &>(other);
    కొరకు(int32_t i=0; i<length; ++i) {
        ఒకవేళ(units[i]!=o.units[i] || విలువలు[i]!=o.విలువలు[i] || equal[i]!=o.equal[i]) {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}

int32_t
StringTrieBuilder::List_Branch_కణుపు::mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య) {
    ఒకవేళ(స్థానభ్రంశం==0) {
        మొదటి_అంచు_సంఖ్య=అంచు_సంఖ్య;
        int32_t అడుగు=0;
        int32_t i=length;
        చేయి {
            Node *అంచు=equal[--i];
            ఒకవేళ(అంచు!=శూన్యసూచిక) {
                అంచు_సంఖ్య=అంచు->mark_కుడి_అంచులు_మొదటి(అంచు_సంఖ్య-అడుగు);
            }
            // For all but the rightmost edge, decrement the edge number.
            అడుగు=1;
        } అయితే(i>0);
        స్థానభ్రంశం=అంచు_సంఖ్య;
    }
    తిరిగి అంచు_సంఖ్య;
}

శూన్య
StringTrieBuilder::List_Branch_కణుపు::లిఖ(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_సంఖ్య=length-1;
    Node *కుడి_అంచు=equal[unit_సంఖ్య];
    int32_t కుడి_అంచు_సంఖ్య= కుడి_అంచు==శూన్యసూచిక ? మొదటి_అంచు_సంఖ్య : కుడి_అంచు->get_స్థానభ్రంశం();
    చేయి {
        --unit_సంఖ్య;
        ఒకవేళ(equal[unit_సంఖ్య]!=శూన్యసూచిక) {
            equal[unit_సంఖ్య]->write_Unless_Inside_కుడి_అంచు(మొదటి_అంచు_సంఖ్య, కుడి_అంచు_సంఖ్య, builder);
        }
    } అయితే(unit_సంఖ్య>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_సంఖ్య=length-1;
    ఒకవేళ(కుడి_అంచు==శూన్యసూచిక) {
        builder.write_విలువ_And_Final(విలువలు[unit_సంఖ్య], నిజం);
    } లేకపోతే {
        కుడి_అంచు->లిఖ(builder);
    }
    స్థానభ్రంశం=builder.లిఖ(units[unit_సంఖ్య]);
    // Write the rest of this node's unit-value pairs.
    అయితే(--unit_సంఖ్య>=0) {
        int32_t value;
        UBool isFinal;
        ఒకవేళ(equal[unit_సంఖ్య]==శూన్యసూచిక) {
            // Write the final value for the one string ending with this unit.
            value=విలువలు[unit_సంఖ్య];
            isFinal=నిజం;
        } లేకపోతే {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_సంఖ్య]->get_స్థానభ్రంశం()>0);
            value=స్థానభ్రంశం-equal[unit_సంఖ్య]->get_స్థానభ్రంశం();
            isFinal=అబద్ధం;
        }
        builder.write_విలువ_And_Final(value, isFinal);
        స్థానభ్రంశం=builder.లిఖ(units[unit_సంఖ్య]);
    }
}

తార్కిక
StringTrieBuilder::విభజించు_Branch_కణుపు::క్రియాకారకం==(స్థిరరాశి Node &other) స్థిరరాశి {
    ఒకవేళ(ఇది==&other) {
        తిరిగి నిజం;
    }
    ఒకవేళ(!Node::క్రియాకారకం==(other)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి విభజించు_Branch_కణుపు &o=static_cast<స్థిరరాశి విభజించు_Branch_కణుపు &>(other);
    తిరిగి unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::విభజించు_Branch_కణుపు::mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య) {
    ఒకవేళ(స్థానభ్రంశం==0) {
        మొదటి_అంచు_సంఖ్య=అంచు_సంఖ్య;
        అంచు_సంఖ్య=greaterOrEqual->mark_కుడి_అంచులు_మొదటి(అంచు_సంఖ్య);
        స్థానభ్రంశం=అంచు_సంఖ్య=lessThan->mark_కుడి_అంచులు_మొదటి(అంచు_సంఖ్య-1);
    }
    తిరిగి అంచు_సంఖ్య;
}

శూన్య
StringTrieBuilder::విభజించు_Branch_కణుపు::లిఖ(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_కుడి_అంచు(మొదటి_అంచు_సంఖ్య, greaterOrEqual->get_స్థానభ్రంశం(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->లిఖ(builder);
    // Write this node.
    U_ASSERT(lessThan->get_స్థానభ్రంశం()>0);
    builder.write_Delta_గమ్యం(lessThan->get_స్థానభ్రంశం());  // less-than
    స్థానభ్రంశం=builder.లిఖ(unit);
}

తార్కిక
StringTrieBuilder::Branch_తల_కణుపు::క్రియాకారకం==(స్థిరరాశి Node &other) స్థిరరాశి {
    ఒకవేళ(ఇది==&other) {
        తిరిగి నిజం;
    }
    ఒకవేళ(!విలువ_కణుపు::క్రియాకారకం==(other)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి Branch_తల_కణుపు &o=static_cast<స్థిరరాశి Branch_తల_కణుపు &>(other);
    తిరిగి length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_తల_కణుపు::mark_కుడి_అంచులు_మొదటి(int32_t అంచు_సంఖ్య) {
    ఒకవేళ(స్థానభ్రంశం==0) {
        స్థానభ్రంశం=అంచు_సంఖ్య=next->mark_కుడి_అంచులు_మొదటి(అంచు_సంఖ్య);
    }
    తిరిగి అంచు_సంఖ్య;
}

శూన్య
StringTrieBuilder::Branch_తల_కణుపు::లిఖ(StringTrieBuilder &builder) {
    next->లిఖ(builder);
    ఒకవేళ(length<=builder.get_Min_రేఖీయ_సరిపోలిక()) {
        స్థానభ్రంశం=builder.write_విలువ_And_Type(has_విలువ, value, length-1);
    } లేకపోతే {
        builder.లిఖ(length-1);
        స్థానభ్రంశం=builder.write_విలువ_And_Type(has_విలువ, value, 0);
    }
}

U_NAMESPACE_చివర
