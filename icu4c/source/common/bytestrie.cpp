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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/bytestream.h"
#അന്തര്ഭാവ "unicode/bytestrie.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_മൂല്യം(സ്ഥിരം uint8_t *pos, int32_t leadByte) {
    int32_t value;
    എങ്കിൽ(leadByte<k_Min_Two_Byte_മൂല്യം_Lead) {
        value=leadByte-k_Min_One_Byte_മൂല്യം_Lead;
    } അല്ലെങ്കിൽ എങ്കിൽ(leadByte<k_Min_Three_Byte_മൂല്യം_Lead) {
        value=((leadByte-k_Min_Two_Byte_മൂല്യം_Lead)<<8)|*pos;
    } അല്ലെങ്കിൽ എങ്കിൽ(leadByte<k_Four_Byte_മൂല്യം_Lead) {
        value=((leadByte-k_Min_Three_Byte_മൂല്യം_Lead)<<16)|(pos[0]<<8)|pos[1];
    } അല്ലെങ്കിൽ എങ്കിൽ(leadByte==k_Four_Byte_മൂല്യം_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } അല്ലെങ്കിൽ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    തിരികെ value;
}

സ്ഥിരം uint8_t*
BytesTrie::jumpByDelta(സ്ഥിരം uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    എങ്കിൽ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } അല്ലെങ്കിൽ എങ്കിൽ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } അല്ലെങ്കിൽ എങ്കിൽ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } അല്ലെങ്കിൽ എങ്കിൽ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } അല്ലെങ്കിൽ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    തിരികെ pos+delta;
}

UStringTrieResult
BytesTrie::current() സ്ഥിരം {
    സ്ഥിരം uint8_t *pos=pos_;
    എങ്കിൽ(pos==nullptr) {
        തിരികെ USTRINGTRIE_NO_MATCH;
    } അല്ലെങ്കിൽ {
        int32_t കണ്ണി;
        തിരികെ (remainingMatchLength_<0 && (കണ്ണി=*pos)>=k_Min_മൂല്യം_Lead) ?
                മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
    }
}

UStringTrieResult
BytesTrie::branchNext(സ്ഥിരം uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    എങ്കിൽ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ആകുമ്പോൾ(length>k_Max_Branch_Linear_Sub_കണ്ണി_Length) {
        എങ്കിൽ(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } അല്ലെങ്കിൽ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    ചെയ്യുക {
        എങ്കിൽ(inByte==*pos++) {
            UStringTrieResult result;
            int32_t കണ്ണി=*pos;
            U_ASSERT(കണ്ണി>=k_Min_മൂല്യം_Lead);
            എങ്കിൽ(കണ്ണി&k_മൂല്യം_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_മൂല്യം;
            } അല്ലെങ്കിൽ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                കണ്ണി>>=1;
                int32_t delta;
                എങ്കിൽ(കണ്ണി<k_Min_Two_Byte_മൂല്യം_Lead) {
                    delta=കണ്ണി-k_Min_One_Byte_മൂല്യം_Lead;
                } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി<k_Min_Three_Byte_മൂല്യം_Lead) {
                    delta=((കണ്ണി-k_Min_Two_Byte_മൂല്യം_Lead)<<8)|*pos++;
                } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി<k_Four_Byte_മൂല്യം_Lead) {
                    delta=((കണ്ണി-k_Min_Three_Byte_മൂല്യം_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി==k_Four_Byte_മൂല്യം_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } അല്ലെങ്കിൽ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                കണ്ണി=*pos;
                result= കണ്ണി>=k_Min_മൂല്യം_Lead ? മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
            }
            pos_=pos;
            തിരികെ result;
        }
        --length;
        pos=skip_മൂല്യം(pos);
    } ആകുമ്പോൾ(length>1);
    എങ്കിൽ(inByte==*pos++) {
        pos_=pos;
        int32_t കണ്ണി=*pos;
        തിരികെ കണ്ണി>=k_Min_മൂല്യം_Lead ? മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
    } അല്ലെങ്കിൽ {
        stop();
        തിരികെ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(സ്ഥിരം uint8_t *pos, int32_t inByte) {
    വേണ്ടി(;;) {
        int32_t കണ്ണി=*pos++;
        എങ്കിൽ(കണ്ണി<kMinLinearMatch) {
            തിരികെ branchNext(pos, കണ്ണി, inByte);
        } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി<k_Min_മൂല്യം_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=കണ്ണി-kMinLinearMatch;  // Actual match length minus 1.
            എങ്കിൽ(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                തിരികെ (length<0 && (കണ്ണി=*pos)>=k_Min_മൂല്യം_Lead) ?
                        മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
            } അല്ലെങ്കിൽ {
                // No match.
                നിർത്തുക;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി&k_മൂല്യം_Is_Final) {
            // No further matching bytes.
            നിർത്തുക;
        } അല്ലെങ്കിൽ {
            // Skip intermediate value.
            pos=skip_മൂല്യം(pos, കണ്ണി);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_മൂല്യം_Lead);
        }
    }
    stop();
    തിരികെ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    സ്ഥിരം uint8_t *pos=pos_;
    എങ്കിൽ(pos==nullptr) {
        തിരികെ USTRINGTRIE_NO_MATCH;
    }
    എങ്കിൽ(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    എങ്കിൽ(length>=0) {
        // Remaining part of a linear-match node.
        എങ്കിൽ(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t കണ്ണി;
            തിരികെ (length<0 && (കണ്ണി=*pos)>=k_Min_മൂല്യം_Lead) ?
                    മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
        } അല്ലെങ്കിൽ {
            stop();
            തിരികെ USTRINGTRIE_NO_MATCH;
        }
    }
    തിരികെ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(സ്ഥിരം അക്ഷരം *s, int32_t sLength) {
    എങ്കിൽ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        തിരികെ current();
    }
    സ്ഥിരം uint8_t *pos=pos_;
    എങ്കിൽ(pos==nullptr) {
        തിരികെ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    വേണ്ടി(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        എങ്കിൽ(sLength<0) {
            വേണ്ടി(;;) {
                എങ്കിൽ((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t കണ്ണി;
                    തിരികെ (length<0 && (കണ്ണി=*pos)>=k_Min_മൂല്യം_Lead) ?
                            മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
                }
                എങ്കിൽ(length<0) {
                    remainingMatchLength_=length;
                    നിർത്തുക;
                }
                എങ്കിൽ(inByte!=*pos) {
                    stop();
                    തിരികെ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } അല്ലെങ്കിൽ {
            വേണ്ടി(;;) {
                എങ്കിൽ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t കണ്ണി;
                    തിരികെ (length<0 && (കണ്ണി=*pos)>=k_Min_മൂല്യം_Lead) ?
                            മൂല്യം_Result(കണ്ണി) : USTRINGTRIE_NO_മൂല്യം;
                }
                inByte=*s++;
                --sLength;
                എങ്കിൽ(length<0) {
                    remainingMatchLength_=length;
                    നിർത്തുക;
                }
                എങ്കിൽ(inByte!=*pos) {
                    stop();
                    തിരികെ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        വേണ്ടി(;;) {
            int32_t കണ്ണി=*pos++;
            എങ്കിൽ(കണ്ണി<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, കണ്ണി, inByte);
                എങ്കിൽ(result==USTRINGTRIE_NO_MATCH) {
                    തിരികെ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                എങ്കിൽ(sLength<0) {
                    എങ്കിൽ((inByte=*s++)==0) {
                        തിരികെ result;
                    }
                } അല്ലെങ്കിൽ {
                    എങ്കിൽ(sLength==0) {
                        തിരികെ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                എങ്കിൽ(result==USTRINGTRIE_FINAL_മൂല്യം) {
                    // No further matching bytes.
                    stop();
                    തിരികെ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി<k_Min_മൂല്യം_Lead) {
                // Match length+1 bytes.
                length=കണ്ണി-kMinLinearMatch;  // Actual match length minus 1.
                എങ്കിൽ(inByte!=*pos) {
                    stop();
                    തിരികെ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                നിർത്തുക;
            } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി&k_മൂല്യം_Is_Final) {
                // No further matching bytes.
                stop();
                തിരികെ USTRINGTRIE_NO_MATCH;
            } അല്ലെങ്കിൽ {
                // Skip intermediate value.
                pos=skip_മൂല്യം(pos, കണ്ണി);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_മൂല്യം_Lead);
            }
        }
    }
}

സ്ഥിരം uint8_t*
BytesTrie::find_Unique_മൂല്യം_From_Branch(സ്ഥിരം uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_മൂല്യം,
                                     int32_t& unique_മൂല്യം) {
    ആകുമ്പോൾ(length>k_Max_Branch_Linear_Sub_കണ്ണി_Length) {
        ++pos;  // ignore the comparison byte
        എങ്കിൽ(nullptr==find_Unique_മൂല്യം_From_Branch(jumpByDelta(pos), length>>1, have_Unique_മൂല്യം, unique_മൂല്യം)) {
            തിരികെ nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ചെയ്യുക {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t കണ്ണി=*pos++;
        UBool isFinal = static_cast<UBool>(കണ്ണി & k_മൂല്യം_Is_Final);
        int32_t value=read_മൂല്യം(pos, കണ്ണി>>1);
        pos=skip_മൂല്യം(pos, കണ്ണി);
        എങ്കിൽ(isFinal) {
            എങ്കിൽ(have_Unique_മൂല്യം) {
                എങ്കിൽ(value!=unique_മൂല്യം) {
                    തിരികെ nullptr;
                }
            } അല്ലെങ്കിൽ {
                unique_മൂല്യം=value;
                have_Unique_മൂല്യം=സത്യം;
            }
        } അല്ലെങ്കിൽ {
            എങ്കിൽ(!find_Unique_മൂല്യം(pos+value, have_Unique_മൂല്യം, unique_മൂല്യം)) {
                തിരികെ nullptr;
            }
            have_Unique_മൂല്യം=സത്യം;
        }
    } ആകുമ്പോൾ(--length>1);
    തിരികെ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_മൂല്യം(സ്ഥിരം uint8_t *pos, UBool have_Unique_മൂല്യം, int32_t &unique_മൂല്യം) {
    വേണ്ടി(;;) {
        int32_t കണ്ണി=*pos++;
        എങ്കിൽ(കണ്ണി<kMinLinearMatch) {
            എങ്കിൽ(കണ്ണി==0) {
                കണ്ണി=*pos++;
            }
            pos=find_Unique_മൂല്യം_From_Branch(pos, കണ്ണി+1, have_Unique_മൂല്യം, unique_മൂല്യം);
            എങ്കിൽ(pos==nullptr) {
                തിരികെ അസത്യം;
            }
            have_Unique_മൂല്യം=സത്യം;
        } അല്ലെങ്കിൽ എങ്കിൽ(കണ്ണി<k_Min_മൂല്യം_Lead) {
            // linear-match node
            pos+=കണ്ണി-kMinLinearMatch+1;  // Ignore the match bytes.
        } അല്ലെങ്കിൽ {
            UBool isFinal = static_cast<UBool>(കണ്ണി & k_മൂല്യം_Is_Final);
            int32_t value=read_മൂല്യം(pos, കണ്ണി>>1);
            എങ്കിൽ(have_Unique_മൂല്യം) {
                എങ്കിൽ(value!=unique_മൂല്യം) {
                    തിരികെ അസത്യം;
                }
            } അല്ലെങ്കിൽ {
                unique_മൂല്യം=value;
                have_Unique_മൂല്യം=സത്യം;
            }
            എങ്കിൽ(isFinal) {
                തിരികെ സത്യം;
            }
            pos=skip_മൂല്യം(pos, കണ്ണി);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) സ്ഥിരം {
    സ്ഥിരം uint8_t *pos=pos_;
    എങ്കിൽ(pos==nullptr) {
        തിരികെ 0;
    }
    എങ്കിൽ(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        തിരികെ 1;
    }
    int32_t കണ്ണി=*pos++;
    എങ്കിൽ(കണ്ണി>=k_Min_മൂല്യം_Lead) {
        എങ്കിൽ(കണ്ണി&k_മൂല്യം_Is_Final) {
            തിരികെ 0;
        } അല്ലെങ്കിൽ {
            pos=skip_മൂല്യം(pos, കണ്ണി);
            കണ്ണി=*pos++;
            U_ASSERT(കണ്ണി<k_Min_മൂല്യം_Lead);
        }
    }
    എങ്കിൽ(കണ്ണി<kMinLinearMatch) {
        എങ്കിൽ(കണ്ണി==0) {
            കണ്ണി=*pos++;
        }
        getNextBranchBytes(pos, ++കണ്ണി, out);
        തിരികെ കണ്ണി;
    } അല്ലെങ്കിൽ {
        // First byte of the linear-match node.
        append(out, *pos);
        തിരികെ 1;
    }
}

ശൂന്യം
BytesTrie::getNextBranchBytes(സ്ഥിരം uint8_t *pos, int32_t length, ByteSink &out) {
    ആകുമ്പോൾ(length>k_Max_Branch_Linear_Sub_കണ്ണി_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ചെയ്യുക {
        append(out, *pos++);
        pos=skip_മൂല്യം(pos);
    } ആകുമ്പോൾ(--length>1);
    append(out, *pos);
}

ശൂന്യം
BytesTrie::append(ByteSink &out, പൂർണ്ണസംഖ്യ c) {
    അക്ഷരം ch = static_cast<അക്ഷരം>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
