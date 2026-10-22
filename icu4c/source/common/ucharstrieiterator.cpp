// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstrieiterator.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov15
*   created by: Markus W. Scherer
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ucharstrie.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_నిడివి,
                               UError_సంకేతం &error_సంకేతం)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          మిగిలిన_సరిపోలిక_నిడివి(-1), initial_మిగిలిన_సరిపోలిక_నిడివి(-1),
          skip_విలువ_2(అబద్ధం),
          max_నిడివి(max_String_నిడివి), విలువ(0), దొంతర(శూన్యసూచిక) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    దొంతర=కొత్త UVector32(error_సంకేతం);
    ఒకవేళ(దొంతర==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(స్థిరరాశి UCharsTrie &trie, int32_t max_String_నిడివి,
                               UError_సంకేతం &error_సంకేతం)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          మిగిలిన_సరిపోలిక_నిడివి(trie.మిగిలిన_సరిపోలిక_నిడివి),
          initial_మిగిలిన_సరిపోలిక_నిడివి(trie.మిగిలిన_సరిపోలిక_నిడివి),
          skip_విలువ_2(అబద్ధం),
          max_నిడివి(max_String_నిడివి), విలువ(0), దొంతర(శూన్యసూచిక) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    దొంతర=కొత్త UVector32(error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ(దొంతర==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    int32_t length=మిగిలిన_సరిపోలిక_నిడివి;  // Actual remaining match length minus 1.
    ఒకవేళ(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        ఒకవేళ(max_నిడివి>0 && length>max_నిడివి) {
            length=max_నిడివి;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        మిగిలిన_సరిపోలిక_నిడివి-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    తొలగించు దొంతర;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    మిగిలిన_సరిపోలిక_నిడివి=initial_మిగిలిన_సరిపోలిక_నిడివి;
    skip_విలువ_2=అబద్ధం;
    int32_t length=మిగిలిన_సరిపోలిక_నిడివి+1;  // Remaining match length.
    ఒకవేళ(max_నిడివి>0 && length>max_నిడివి) {
        length=max_నిడివి;
    }
    str_.truncate(length);
    pos_+=length;
    మిగిలిన_సరిపోలిక_నిడివి-=length;
    దొంతర->సమితి_కొలత(0);
    తిరిగి *ఇది;
}

UBool
UCharsTrie::Iterator::has_తదుపరి() స్థిరరాశి { తిరిగి pos_!=శూన్యసూచిక || !దొంతర->is_ఖాళీలు(); }

UBool
UCharsTrie::Iterator::next(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి char16_t *pos=pos_;
    ఒకవేళ(pos==శూన్యసూచిక) {
        ఒకవేళ(దొంతర->is_ఖాళీలు()) {
            తిరిగి అబద్ధం;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t దొంతర_కొలత=దొంతర->size();
        int32_t length=దొంతర->elementAti(దొంతర_కొలత-1);
        pos=uchars_+దొంతర->elementAti(దొంతర_కొలత-2);
        దొంతర->సమితి_కొలత(దొంతర_కొలత-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ఒకవేళ(length>1) {
            pos=branch_తదుపరి(pos, length, error_సంకేతం);
            ఒకవేళ(pos==శూన్యసూచిక) {
                తిరిగి నిజం;  // Reached a final value.
            }
        } లేకపోతే {
            str_.append(*pos++);
        }
    }
    ఒకవేళ(మిగిలిన_సరిపోలిక_నిడివి>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        తిరిగి truncateAndStop();
    }
    కొరకు(;;) {
        int32_t కణుపు=*pos++;
        ఒకవేళ(కణుపు>=k_Min_విలువ_Lead) {
            ఒకవేళ(skip_విలువ_2) {
                pos=skip_కణుపు_విలువ(pos, కణుపు);
                కణుపు&=k_కణుపు_Type_ముసుగు;
                skip_విలువ_2=అబద్ధం;
            } లేకపోతే {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(కణుపు >> 15);
                ఒకవేళ(isFinal) {
                    విలువ=read_విలువ(pos, కణుపు&0x7fff);
                } లేకపోతే {
                    విలువ=read_కణుపు_విలువ(pos, కణుపు);
                }
                ఒకవేళ(isFinal || (max_నిడివి>0 && str_.length()==max_నిడివి)) {
                    pos_=శూన్యసూచిక;
                } లేకపోతే {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_విలువ_2=నిజం;
                }
                తిరిగి నిజం;
            }
        }
        ఒకవేళ(max_నిడివి>0 && str_.length()==max_నిడివి) {
            తిరిగి truncateAndStop();
        }
        ఒకవేళ(కణుపు<k_Min_రేఖీయ_సరిపోలిక) {
            ఒకవేళ(కణుపు==0) {
                కణుపు=*pos++;
            }
            pos=branch_తదుపరి(pos, కణుపు+1, error_సంకేతం);
            ఒకవేళ(pos==శూన్యసూచిక) {
                తిరిగి నిజం;  // Reached a final value.
            }
        } లేకపోతే {
            // Linear-match node, append length units to str_.
            int32_t length=కణుపు-k_Min_రేఖీయ_సరిపోలిక+1;
            ఒకవేళ(max_నిడివి>0 && str_.length()+length>max_నిడివి) {
                str_.append(pos, max_నిడివి-str_.length());
                తిరిగి truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
స్థిరరాశి char16_t*
UCharsTrie::Iterator::branch_తదుపరి(స్థిరరాశి char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UError_సంకేతం& error_సంకేతం) {
    అయితే(length>k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        దొంతర->కూడు_Element(static_cast<int32_t>(skipDelta(pos) - uchars_), error_సంకేతం);
        దొంతర->కూడు_Element(((length-(length>>1))<<16)|str_.length(), error_సంకేతం);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t కణుపు=*pos++;
    UBool isFinal = static_cast<UBool>(కణుపు >> 15);
    int32_t value=read_విలువ(pos, కణుపు&=0x7fff);
    pos=skip_విలువ(pos, కణుపు);
    దొంతర->కూడు_Element(static_cast<int32_t>(pos - uchars_), error_సంకేతం);
    దొంతర->కూడు_Element(((length-1)<<16)|str_.length(), error_సంకేతం);
    str_.append(trieUnit);
    ఒకవేళ(isFinal) {
        pos_=శూన్యసూచిక;
        విలువ=value;
        తిరిగి శూన్యసూచిక;
    } లేకపోతే {
        తిరిగి pos+value;
    }
}

U_NAMESPACE_చివర
