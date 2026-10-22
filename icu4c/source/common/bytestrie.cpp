// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestrie.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010sep25
*   created by: Markus W. Scherer
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/bytestream.h"
#అన్తర్భావ "unicode/bytestrie.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"

U_NAMESPACE_BEGIN

బైట్లు_Trie::~బైట్లు_Trie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
బైట్లు_Trie::read_విలువ(స్థిరరాశి uint8_t *pos, int32_t leadByte) {
    int32_t value;
    ఒకవేళ(leadByte<k_Min_Two_Byte_విలువ_Lead) {
        value=leadByte-k_Min_One_Byte_విలువ_Lead;
    } లేకపోతే ఒకవేళ(leadByte<k_Min_Three_Byte_విలువ_Lead) {
        value=((leadByte-k_Min_Two_Byte_విలువ_Lead)<<8)|*pos;
    } లేకపోతే ఒకవేళ(leadByte<k_Four_Byte_విలువ_Lead) {
        value=((leadByte-k_Min_Three_Byte_విలువ_Lead)<<16)|(pos[0]<<8)|pos[1];
    } లేకపోతే ఒకవేళ(leadByte==k_Four_Byte_విలువ_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } లేకపోతే {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    తిరిగి value;
}

స్థిరరాశి uint8_t*
బైట్లు_Trie::jumpByDelta(స్థిరరాశి uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ఒకవేళ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } లేకపోతే ఒకవేళ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } లేకపోతే ఒకవేళ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } లేకపోతే ఒకవేళ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } లేకపోతే {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    తిరిగి pos+delta;
}

UString_Trie_ఫలితం
బైట్లు_Trie::current() స్థిరరాశి {
    స్థిరరాశి uint8_t *pos=pos_;
    ఒకవేళ(pos==శూన్యసూచిక) {
        తిరిగి USTRINGTRIE_NO_సరిపోలిక;
    } లేకపోతే {
        int32_t కణుపు;
        తిరిగి (మిగిలిన_సరిపోలిక_నిడివి<0 && (కణుపు=*pos)>=k_Min_విలువ_Lead) ?
                విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
    }
}

UString_Trie_ఫలితం
బైట్లు_Trie::branch_తదుపరి(స్థిరరాశి uint8_t *pos, int32_t length, int32_t లోపల_Byte) {
    // Branch according to the current byte.
    ఒకవేళ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    అయితే(length>k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి) {
        ఒకవేళ(లోపల_Byte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } లేకపోతే {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    చేయి {
        ఒకవేళ(లోపల_Byte==*pos++) {
            UString_Trie_ఫలితం ఫలితం;
            int32_t కణుపు=*pos;
            U_ASSERT(కణుపు>=k_Min_విలువ_Lead);
            ఒకవేళ(కణుపు&k_విలువ_Is_Final) {
                // Leave the final value for getValue() to read.
                ఫలితం=USTRINGTRIE_FINAL_విలువ;
            } లేకపోతే {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                కణుపు>>=1;
                int32_t delta;
                ఒకవేళ(కణుపు<k_Min_Two_Byte_విలువ_Lead) {
                    delta=కణుపు-k_Min_One_Byte_విలువ_Lead;
                } లేకపోతే ఒకవేళ(కణుపు<k_Min_Three_Byte_విలువ_Lead) {
                    delta=((కణుపు-k_Min_Two_Byte_విలువ_Lead)<<8)|*pos++;
                } లేకపోతే ఒకవేళ(కణుపు<k_Four_Byte_విలువ_Lead) {
                    delta=((కణుపు-k_Min_Three_Byte_విలువ_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } లేకపోతే ఒకవేళ(కణుపు==k_Four_Byte_విలువ_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } లేకపోతే {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                కణుపు=*pos;
                ఫలితం= కణుపు>=k_Min_విలువ_Lead ? విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
            }
            pos_=pos;
            తిరిగి ఫలితం;
        }
        --length;
        pos=skip_విలువ(pos);
    } అయితే(length>1);
    ఒకవేళ(లోపల_Byte==*pos++) {
        pos_=pos;
        int32_t కణుపు=*pos;
        తిరిగి కణుపు>=k_Min_విలువ_Lead ? విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
    } లేకపోతే {
        stop();
        తిరిగి USTRINGTRIE_NO_సరిపోలిక;
    }
}

UString_Trie_ఫలితం
బైట్లు_Trie::తదుపరి_Impl(స్థిరరాశి uint8_t *pos, int32_t లోపల_Byte) {
    కొరకు(;;) {
        int32_t కణుపు=*pos++;
        ఒకవేళ(కణుపు<k_Min_రేఖీయ_సరిపోలిక) {
            తిరిగి branch_తదుపరి(pos, కణుపు, లోపల_Byte);
        } లేకపోతే ఒకవేళ(కణుపు<k_Min_విలువ_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=కణుపు-k_Min_రేఖీయ_సరిపోలిక;  // Actual match length minus 1.
            ఒకవేళ(లోపల_Byte==*pos++) {
                మిగిలిన_సరిపోలిక_నిడివి=--length;
                pos_=pos;
                తిరిగి (length<0 && (కణుపు=*pos)>=k_Min_విలువ_Lead) ?
                        విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
            } లేకపోతే {
                // No match.
                ఆపు;
            }
        } లేకపోతే ఒకవేళ(కణుపు&k_విలువ_Is_Final) {
            // No further matching bytes.
            ఆపు;
        } లేకపోతే {
            // Skip intermediate value.
            pos=skip_విలువ(pos, కణుపు);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_విలువ_Lead);
        }
    }
    stop();
    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
}

UString_Trie_ఫలితం
బైట్లు_Trie::next(int32_t లోపల_Byte) {
    స్థిరరాశి uint8_t *pos=pos_;
    ఒకవేళ(pos==శూన్యసూచిక) {
        తిరిగి USTRINGTRIE_NO_సరిపోలిక;
    }
    ఒకవేళ(లోపల_Byte<0) {
        లోపల_Byte+=0x100;
    }
    int32_t length=మిగిలిన_సరిపోలిక_నిడివి;  // Actual remaining match length minus 1.
    ఒకవేళ(length>=0) {
        // Remaining part of a linear-match node.
        ఒకవేళ(లోపల_Byte==*pos++) {
            మిగిలిన_సరిపోలిక_నిడివి=--length;
            pos_=pos;
            int32_t కణుపు;
            తిరిగి (length<0 && (కణుపు=*pos)>=k_Min_విలువ_Lead) ?
                    విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
        } లేకపోతే {
            stop();
            తిరిగి USTRINGTRIE_NO_సరిపోలిక;
        }
    }
    తిరిగి తదుపరి_Impl(pos, లోపల_Byte);
}

UString_Trie_ఫలితం
బైట్లు_Trie::next(స్థిరరాశి అక్షరం *s, int32_t s_నిడివి) {
    ఒకవేళ(s_నిడివి<0 ? *s==0 : s_నిడివి==0) {
        // Empty input.
        తిరిగి current();
    }
    స్థిరరాశి uint8_t *pos=pos_;
    ఒకవేళ(pos==శూన్యసూచిక) {
        తిరిగి USTRINGTRIE_NO_సరిపోలిక;
    }
    int32_t length=మిగిలిన_సరిపోలిక_నిడివి;  // Actual remaining match length minus 1.
    కొరకు(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t లోపల_Byte;
        ఒకవేళ(s_నిడివి<0) {
            కొరకు(;;) {
                ఒకవేళ((లోపల_Byte=*s++)==0) {
                    మిగిలిన_సరిపోలిక_నిడివి=length;
                    pos_=pos;
                    int32_t కణుపు;
                    తిరిగి (length<0 && (కణుపు=*pos)>=k_Min_విలువ_Lead) ?
                            విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
                }
                ఒకవేళ(length<0) {
                    మిగిలిన_సరిపోలిక_నిడివి=length;
                    ఆపు;
                }
                ఒకవేళ(లోపల_Byte!=*pos) {
                    stop();
                    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
                }
                ++pos;
                --length;
            }
        } లేకపోతే {
            కొరకు(;;) {
                ఒకవేళ(s_నిడివి==0) {
                    మిగిలిన_సరిపోలిక_నిడివి=length;
                    pos_=pos;
                    int32_t కణుపు;
                    తిరిగి (length<0 && (కణుపు=*pos)>=k_Min_విలువ_Lead) ?
                            విలువ_ఫలితం(కణుపు) : USTRINGTRIE_NO_విలువ;
                }
                లోపల_Byte=*s++;
                --s_నిడివి;
                ఒకవేళ(length<0) {
                    మిగిలిన_సరిపోలిక_నిడివి=length;
                    ఆపు;
                }
                ఒకవేళ(లోపల_Byte!=*pos) {
                    stop();
                    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
                }
                ++pos;
                --length;
            }
        }
        కొరకు(;;) {
            int32_t కణుపు=*pos++;
            ఒకవేళ(కణుపు<k_Min_రేఖీయ_సరిపోలిక) {
                UString_Trie_ఫలితం ఫలితం=branch_తదుపరి(pos, కణుపు, లోపల_Byte);
                ఒకవేళ(ఫలితం==USTRINGTRIE_NO_సరిపోలిక) {
                    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
                }
                // Fetch the next input byte, if there is one.
                ఒకవేళ(s_నిడివి<0) {
                    ఒకవేళ((లోపల_Byte=*s++)==0) {
                        తిరిగి ఫలితం;
                    }
                } లేకపోతే {
                    ఒకవేళ(s_నిడివి==0) {
                        తిరిగి ఫలితం;
                    }
                    లోపల_Byte=*s++;
                    --s_నిడివి;
                }
                ఒకవేళ(ఫలితం==USTRINGTRIE_FINAL_విలువ) {
                    // No further matching bytes.
                    stop();
                    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } లేకపోతే ఒకవేళ(కణుపు<k_Min_విలువ_Lead) {
                // Match length+1 bytes.
                length=కణుపు-k_Min_రేఖీయ_సరిపోలిక;  // Actual match length minus 1.
                ఒకవేళ(లోపల_Byte!=*pos) {
                    stop();
                    తిరిగి USTRINGTRIE_NO_సరిపోలిక;
                }
                ++pos;
                --length;
                ఆపు;
            } లేకపోతే ఒకవేళ(కణుపు&k_విలువ_Is_Final) {
                // No further matching bytes.
                stop();
                తిరిగి USTRINGTRIE_NO_సరిపోలిక;
            } లేకపోతే {
                // Skip intermediate value.
                pos=skip_విలువ(pos, కణుపు);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_విలువ_Lead);
            }
        }
    }
}

స్థిరరాశి uint8_t*
బైట్లు_Trie::కనుగొను_Unique_విలువ_From_Branch(స్థిరరాశి uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_విలువ,
                                     int32_t& unique_విలువ) {
    అయితే(length>k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి) {
        ++pos;  // ignore the comparison byte
        ఒకవేళ(శూన్యసూచిక==కనుగొను_Unique_విలువ_From_Branch(jumpByDelta(pos), length>>1, have_Unique_విలువ, unique_విలువ)) {
            తిరిగి శూన్యసూచిక;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    చేయి {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t కణుపు=*pos++;
        UBool isFinal = static_cast<UBool>(కణుపు & k_విలువ_Is_Final);
        int32_t value=read_విలువ(pos, కణుపు>>1);
        pos=skip_విలువ(pos, కణుపు);
        ఒకవేళ(isFinal) {
            ఒకవేళ(have_Unique_విలువ) {
                ఒకవేళ(value!=unique_విలువ) {
                    తిరిగి శూన్యసూచిక;
                }
            } లేకపోతే {
                unique_విలువ=value;
                have_Unique_విలువ=నిజం;
            }
        } లేకపోతే {
            ఒకవేళ(!కనుగొను_Unique_విలువ(pos+value, have_Unique_విలువ, unique_విలువ)) {
                తిరిగి శూన్యసూచిక;
            }
            have_Unique_విలువ=నిజం;
        }
    } అయితే(--length>1);
    తిరిగి pos+1;  // ignore the last comparison byte
}

UBool
బైట్లు_Trie::కనుగొను_Unique_విలువ(స్థిరరాశి uint8_t *pos, UBool have_Unique_విలువ, int32_t &unique_విలువ) {
    కొరకు(;;) {
        int32_t కణుపు=*pos++;
        ఒకవేళ(కణుపు<k_Min_రేఖీయ_సరిపోలిక) {
            ఒకవేళ(కణుపు==0) {
                కణుపు=*pos++;
            }
            pos=కనుగొను_Unique_విలువ_From_Branch(pos, కణుపు+1, have_Unique_విలువ, unique_విలువ);
            ఒకవేళ(pos==శూన్యసూచిక) {
                తిరిగి అబద్ధం;
            }
            have_Unique_విలువ=నిజం;
        } లేకపోతే ఒకవేళ(కణుపు<k_Min_విలువ_Lead) {
            // linear-match node
            pos+=కణుపు-k_Min_రేఖీయ_సరిపోలిక+1;  // Ignore the match bytes.
        } లేకపోతే {
            UBool isFinal = static_cast<UBool>(కణుపు & k_విలువ_Is_Final);
            int32_t value=read_విలువ(pos, కణుపు>>1);
            ఒకవేళ(have_Unique_విలువ) {
                ఒకవేళ(value!=unique_విలువ) {
                    తిరిగి అబద్ధం;
                }
            } లేకపోతే {
                unique_విలువ=value;
                have_Unique_విలువ=నిజం;
            }
            ఒకవేళ(isFinal) {
                తిరిగి నిజం;
            }
            pos=skip_విలువ(pos, కణుపు);
        }
    }
}

int32_t
బైట్లు_Trie::get_తదుపరి_బైట్లు(ByteSink &out) స్థిరరాశి {
    స్థిరరాశి uint8_t *pos=pos_;
    ఒకవేళ(pos==శూన్యసూచిక) {
        తిరిగి 0;
    }
    ఒకవేళ(మిగిలిన_సరిపోలిక_నిడివి>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        తిరిగి 1;
    }
    int32_t కణుపు=*pos++;
    ఒకవేళ(కణుపు>=k_Min_విలువ_Lead) {
        ఒకవేళ(కణుపు&k_విలువ_Is_Final) {
            తిరిగి 0;
        } లేకపోతే {
            pos=skip_విలువ(pos, కణుపు);
            కణుపు=*pos++;
            U_ASSERT(కణుపు<k_Min_విలువ_Lead);
        }
    }
    ఒకవేళ(కణుపు<k_Min_రేఖీయ_సరిపోలిక) {
        ఒకవేళ(కణుపు==0) {
            కణుపు=*pos++;
        }
        get_తదుపరి_Branch_బైట్లు(pos, ++కణుపు, out);
        తిరిగి కణుపు;
    } లేకపోతే {
        // First byte of the linear-match node.
        append(out, *pos);
        తిరిగి 1;
    }
}

శూన్య
బైట్లు_Trie::get_తదుపరి_Branch_బైట్లు(స్థిరరాశి uint8_t *pos, int32_t length, ByteSink &out) {
    అయితే(length>k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి) {
        ++pos;  // ignore the comparison byte
        get_తదుపరి_Branch_బైట్లు(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    చేయి {
        append(out, *pos++);
        pos=skip_విలువ(pos);
    } అయితే(--length>1);
    append(out, *pos);
}

శూన్య
బైట్లు_Trie::append(ByteSink &out, పూర్ణాంకం c) {
    అక్షరం ch = static_cast<అక్షరం>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_చివర
