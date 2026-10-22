// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestrieiterator.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov03
*   created by: Markus W. Scherer
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/bytestrie.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "uvectr32.h"

U_NAMESPACE_BEGIN

బైట్లు_Trie::Iterator::Iterator(స్థిరరాశి శూన్య* trie_బైట్లు U_LIFETIME_BOUND,
                              int32_t max_String_నిడివి,
                              UError_సంకేతం& error_సంకేతం)
        : బైట్లు_2(static_cast<స్థిరరాశి uint8_t *>(trie_బైట్లు)),
          pos_(బైట్లు_2), initialPos_(బైట్లు_2),
          మిగిలిన_సరిపోలిక_నిడివి(-1), initial_మిగిలిన_సరిపోలిక_నిడివి(-1),
          str_(శూన్యసూచిక), max_నిడివి(max_String_నిడివి), విలువ(0), దొంతర(శూన్యసూచిక) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=కొత్త CharString();
    దొంతర=కొత్త UVector32(error_సంకేతం);
    ఒకవేళ(U_SUCCESS(error_సంకేతం) && (str_==శూన్యసూచిక || దొంతర==శూన్యసూచిక)) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}

బైట్లు_Trie::Iterator::Iterator(స్థిరరాశి బైట్లు_Trie &trie, int32_t max_String_నిడివి,
                              UError_సంకేతం &error_సంకేతం)
        : బైట్లు_2(trie.బైట్లు_2), pos_(trie.pos_), initialPos_(trie.pos_),
          మిగిలిన_సరిపోలిక_నిడివి(trie.మిగిలిన_సరిపోలిక_నిడివి),
          initial_మిగిలిన_సరిపోలిక_నిడివి(trie.మిగిలిన_సరిపోలిక_నిడివి),
          str_(శూన్యసూచిక), max_నిడివి(max_String_నిడివి), విలువ(0), దొంతర(శూన్యసూచిక) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    str_=కొత్త CharString();
    దొంతర=కొత్త UVector32(error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ(str_==శూన్యసూచిక || దొంతర==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    int32_t length=మిగిలిన_సరిపోలిక_నిడివి;  // Actual remaining match length minus 1.
    ఒకవేళ(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        ఒకవేళ(max_నిడివి>0 && length>max_నిడివి) {
            length=max_నిడివి;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<స్థిరరాశి అక్షరం *>(pos_), length, error_సంకేతం);
        pos_+=length;
        మిగిలిన_సరిపోలిక_నిడివి-=length;
    }
}

బైట్లు_Trie::Iterator::~Iterator() {
    తొలగించు str_;
    తొలగించు దొంతర;
}

బైట్లు_Trie::Iterator&
బైట్లు_Trie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    మిగిలిన_సరిపోలిక_నిడివి=initial_మిగిలిన_సరిపోలిక_నిడివి;
    int32_t length=మిగిలిన_సరిపోలిక_నిడివి+1;  // Remaining match length.
    ఒకవేళ(max_నిడివి>0 && length>max_నిడివి) {
        length=max_నిడివి;
    }
    str_->truncate(length);
    pos_+=length;
    మిగిలిన_సరిపోలిక_నిడివి-=length;
    దొంతర->సమితి_కొలత(0);
    తిరిగి *ఇది;
}

UBool
బైట్లు_Trie::Iterator::has_తదుపరి() స్థిరరాశి { తిరిగి pos_!=శూన్యసూచిక || !దొంతర->is_ఖాళీలు(); }

UBool
బైట్లు_Trie::Iterator::next(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి uint8_t *pos=pos_;
    ఒకవేళ(pos==శూన్యసూచిక) {
        ఒకవేళ(దొంతర->is_ఖాళీలు()) {
            తిరిగి అబద్ధం;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t దొంతర_కొలత=దొంతర->size();
        int32_t length=దొంతర->elementAti(దొంతర_కొలత-1);
        pos=బైట్లు_2+దొంతర->elementAti(దొంతర_కొలత-2);
        దొంతర->సమితి_కొలత(దొంతర_కొలత-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ఒకవేళ(length>1) {
            pos=branch_తదుపరి(pos, length, error_సంకేతం);
            ఒకవేళ(pos==శూన్యసూచిక) {
                తిరిగి నిజం;  // Reached a final value.
            }
        } లేకపోతే {
            str_->append(static_cast<అక్షరం>(*pos++), error_సంకేతం);
        }
    }
    ఒకవేళ(మిగిలిన_సరిపోలిక_నిడివి>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        తిరిగి truncateAndStop();
    }
    కొరకు(;;) {
        int32_t కణుపు=*pos++;
        ఒకవేళ(కణుపు>=k_Min_విలువ_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(కణుపు & k_విలువ_Is_Final);
            విలువ=read_విలువ(pos, కణుపు>>1);
            ఒకవేళ(isFinal || (max_నిడివి>0 && str_->length()==max_నిడివి)) {
                pos_=శూన్యసూచిక;
            } లేకపోతే {
                pos_=skip_విలువ(pos, కణుపు);
            }
            తిరిగి నిజం;
        }
        ఒకవేళ(max_నిడివి>0 && str_->length()==max_నిడివి) {
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
            // Linear-match node, append length bytes to str_.
            int32_t length=కణుపు-k_Min_రేఖీయ_సరిపోలిక+1;
            ఒకవేళ(max_నిడివి>0 && str_->length()+length>max_నిడివి) {
                str_->append(reinterpret_cast<స్థిరరాశి అక్షరం *>(pos),
                            max_నిడివి-str_->length(), error_సంకేతం);
                తిరిగి truncateAndStop();
            }
            str_->append(reinterpret_cast<స్థిరరాశి అక్షరం *>(pos), length, error_సంకేతం);
            pos+=length;
        }
    }
}

StringPiece
బైట్లు_Trie::Iterator::getString() స్థిరరాశి U_LIFETIME_BOUND {
    తిరిగి str_ == శూన్యసూచిక ? StringPiece() : str_->గమ్యం_String_Piece();
}

UBool
బైట్లు_Trie::Iterator::truncateAndStop() {
    pos_=శూన్యసూచిక;
    విలువ=-1;  // no real value for str
    తిరిగి నిజం;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
స్థిరరాశి uint8_t*
బైట్లు_Trie::Iterator::branch_తదుపరి(స్థిరరాశి uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UError_సంకేతం& error_సంకేతం) {
    అయితే(length>k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        దొంతర->కూడు_Element(static_cast<int32_t>(skipDelta(pos) - బైట్లు_2), error_సంకేతం);
        దొంతర->కూడు_Element(((length-(length>>1))<<16)|str_->length(), error_సంకేతం);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t కణుపు=*pos++;
    UBool isFinal = static_cast<UBool>(కణుపు & k_విలువ_Is_Final);
    int32_t value=read_విలువ(pos, కణుపు>>1);
    pos=skip_విలువ(pos, కణుపు);
    దొంతర->కూడు_Element(static_cast<int32_t>(pos - బైట్లు_2), error_సంకేతం);
    దొంతర->కూడు_Element(((length-1)<<16)|str_->length(), error_సంకేతం);
    str_->append(static_cast<అక్షరం>(trieByte), error_సంకేతం);
    ఒకవేళ(isFinal) {
        pos_=శూన్యసూచిక;
        విలువ=value;
        తిరిగి శూన్యసూచిక;
    } లేకపోతే {
        తిరిగి pos+value;
    }
}

U_NAMESPACE_చివర
