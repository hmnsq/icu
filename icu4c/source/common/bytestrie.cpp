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

#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestream.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/uobject.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::readValue(ثابت uint8_t *pos, int32_t leadByte) {
    int32_t value;
    ܐܢ(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } ܐܚܪܢܐ ܐܢ(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } ܐܚܪܢܐ ܐܢ(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } ܐܚܪܢܐ ܐܢ(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ܐܚܪܢܐ {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ܦܢܐ value;
}

ثابت uint8_t*
BytesTrie::jumpByDelta(ثابت uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ܐܢ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ܐܚܪܢܐ ܐܢ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ܐܚܪܢܐ ܐܢ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ܐܚܪܢܐ ܐܢ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ܐܚܪܢܐ {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ܦܢܐ pos+delta;
}

UStringTrieResult
BytesTrie::current() ثابت {
    ثابت uint8_t *pos=pos_;
    ܐܢ(pos==nullptr) {
        ܦܢܐ USTRINGTRIE_NO_MATCH;
    } ܐܚܪܢܐ {
        int32_t node;
        ܦܢܐ (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(ثابت uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ܐܢ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ܟܡܐ(length>kMaxBranchLinearSubNodeLength) {
        ܐܢ(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ܐܚܪܢܐ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    ܥܒܕ {
        ܐܢ(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            ܐܢ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } ܐܚܪܢܐ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                ܐܢ(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } ܐܚܪܢܐ ܐܢ(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } ܐܚܪܢܐ ܐܢ(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ܐܚܪܢܐ ܐܢ(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ܐܚܪܢܐ {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ܦܢܐ result;
        }
        --length;
        pos=skipValue(pos);
    } ܟܡܐ(length>1);
    ܐܢ(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ܦܢܐ node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } ܐܚܪܢܐ {
        stop();
        ܦܢܐ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ثابت uint8_t *pos, int32_t inByte) {
    لكل(;;) {
        int32_t node=*pos++;
        ܐܢ(node<kMinLinearMatch) {
            ܦܢܐ branchNext(pos, node, inByte);
        } ܐܚܪܢܐ ܐܢ(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            ܐܢ(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ܦܢܐ (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } ܐܚܪܢܐ {
                // No match.
                اكسر;
            }
        } ܐܚܪܢܐ ܐܢ(node&kValueIsFinal) {
            // No further matching bytes.
            اكسر;
        } ܐܚܪܢܐ {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ܦܢܐ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    ثابت uint8_t *pos=pos_;
    ܐܢ(pos==nullptr) {
        ܦܢܐ USTRINGTRIE_NO_MATCH;
    }
    ܐܢ(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ܐܢ(length>=0) {
        // Remaining part of a linear-match node.
        ܐܢ(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ܦܢܐ (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } ܐܚܪܢܐ {
            stop();
            ܦܢܐ USTRINGTRIE_NO_MATCH;
        }
    }
    ܦܢܐ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(ثابت حرف *s, int32_t sLength) {
    ܐܢ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ܦܢܐ current();
    }
    ثابت uint8_t *pos=pos_;
    ܐܢ(pos==nullptr) {
        ܦܢܐ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ܐܢ(sLength<0) {
            لكل(;;) {
                ܐܢ((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ܦܢܐ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                ܐܢ(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                ܐܢ(inByte!=*pos) {
                    stop();
                    ܦܢܐ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ܐܚܪܢܐ {
            لكل(;;) {
                ܐܢ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ܦܢܐ (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                ܐܢ(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                ܐܢ(inByte!=*pos) {
                    stop();
                    ܦܢܐ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        لكل(;;) {
            int32_t node=*pos++;
            ܐܢ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                ܐܢ(result==USTRINGTRIE_NO_MATCH) {
                    ܦܢܐ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ܐܢ(sLength<0) {
                    ܐܢ((inByte=*s++)==0) {
                        ܦܢܐ result;
                    }
                } ܐܚܪܢܐ {
                    ܐܢ(sLength==0) {
                        ܦܢܐ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ܐܢ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ܦܢܐ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ܐܚܪܢܐ ܐܢ(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                ܐܢ(inByte!=*pos) {
                    stop();
                    ܦܢܐ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                اكسر;
            } ܐܚܪܢܐ ܐܢ(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ܦܢܐ USTRINGTRIE_NO_MATCH;
            } ܐܚܪܢܐ {
                // Skip intermediate value.
                pos=skipValue(pos, node);
                // The next node must not also be a value node.
                U_ASSERT(*pos<kMinValueLead);
            }
        }
    }
}

ثابت uint8_t*
BytesTrie::findUniqueValueFromBranch(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool haveUniqueValue,
                                     int32_t& uniqueValue) {
    ܟܡܐ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        ܐܢ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ܦܢܐ nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ܥܒܕ {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t node=*pos++;
        UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
        int32_t value=readValue(pos, node>>1);
        pos=skipValue(pos, node);
        ܐܢ(isFinal) {
            ܐܢ(haveUniqueValue) {
                ܐܢ(value!=uniqueValue) {
                    ܦܢܐ nullptr;
                }
            } ܐܚܪܢܐ {
                uniqueValue=value;
                haveUniqueValue=ܫܪܪܐ;
            }
        } ܐܚܪܢܐ {
            ܐܢ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ܦܢܐ nullptr;
            }
            haveUniqueValue=ܫܪܪܐ;
        }
    } ܟܡܐ(--length>1);
    ܦܢܐ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(ثابت uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    لكل(;;) {
        int32_t node=*pos++;
        ܐܢ(node<kMinLinearMatch) {
            ܐܢ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            ܐܢ(pos==nullptr) {
                ܦܢܐ ܕܓܠܐ;
            }
            haveUniqueValue=ܫܪܪܐ;
        } ܐܚܪܢܐ ܐܢ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } ܐܚܪܢܐ {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            ܐܢ(haveUniqueValue) {
                ܐܢ(value!=uniqueValue) {
                    ܦܢܐ ܕܓܠܐ;
                }
            } ܐܚܪܢܐ {
                uniqueValue=value;
                haveUniqueValue=ܫܪܪܐ;
            }
            ܐܢ(isFinal) {
                ܦܢܐ ܫܪܪܐ;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) ثابت {
    ثابت uint8_t *pos=pos_;
    ܐܢ(pos==nullptr) {
        ܦܢܐ 0;
    }
    ܐܢ(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ܦܢܐ 1;
    }
    int32_t node=*pos++;
    ܐܢ(node>=kMinValueLead) {
        ܐܢ(node&kValueIsFinal) {
            ܦܢܐ 0;
        } ܐܚܪܢܐ {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    ܐܢ(node<kMinLinearMatch) {
        ܐܢ(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ܦܢܐ node;
    } ܐܚܪܢܐ {
        // First byte of the linear-match node.
        append(out, *pos);
        ܦܢܐ 1;
    }
}

فراغ
BytesTrie::getNextBranchBytes(ثابت uint8_t *pos, int32_t length, ByteSink &out) {
    ܟܡܐ(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ܥܒܕ {
        append(out, *pos++);
        pos=skipValue(pos);
    } ܟܡܐ(--length>1);
    append(out, *pos);
}

فراغ
BytesTrie::append(ByteSink &out, صحيح c) {
    حرف ch = static_cast<حرف>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
