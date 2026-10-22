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
    𐡄𐡍(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } وإلا 𐡄𐡍(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } وإلا 𐡄𐡍(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } وإلا 𐡄𐡍(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } وإلا {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    أرجع value;
}

ثابت uint8_t*
BytesTrie::jumpByDelta(ثابت uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    𐡄𐡍(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } وإلا 𐡄𐡍(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } وإلا 𐡄𐡍(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } وإلا 𐡄𐡍(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } وإلا {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    أرجع pos+delta;
}

UStringTrieResult
BytesTrie::current() ثابت {
    ثابت uint8_t *pos=pos_;
    𐡄𐡍(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    } وإلا {
        int32_t node;
        أرجع (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(ثابت uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    𐡄𐡍(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    طالما(length>kMaxBranchLinearSubNodeLength) {
        𐡄𐡍(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } وإلا {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    𐡏𐡁𐡃 {
        𐡄𐡍(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            𐡄𐡍(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } وإلا {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                𐡄𐡍(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } وإلا 𐡄𐡍(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } وإلا 𐡄𐡍(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } وإلا 𐡄𐡍(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } وإلا {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            أرجع result;
        }
        --length;
        pos=skipValue(pos);
    } طالما(length>1);
    𐡄𐡍(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        أرجع node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } وإلا {
        stop();
        أرجع USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ثابت uint8_t *pos, int32_t inByte) {
    لكل(;;) {
        int32_t node=*pos++;
        𐡄𐡍(node<kMinLinearMatch) {
            أرجع branchNext(pos, node, inByte);
        } وإلا 𐡄𐡍(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            𐡄𐡍(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                أرجع (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } وإلا {
                // No match.
                اكسر;
            }
        } وإلا 𐡄𐡍(node&kValueIsFinal) {
            // No further matching bytes.
            اكسر;
        } وإلا {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    أرجع USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    ثابت uint8_t *pos=pos_;
    𐡄𐡍(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    }
    𐡄𐡍(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    𐡄𐡍(length>=0) {
        // Remaining part of a linear-match node.
        𐡄𐡍(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            أرجع (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } وإلا {
            stop();
            أرجع USTRINGTRIE_NO_MATCH;
        }
    }
    أرجع nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(ثابت حرف *s, int32_t sLength) {
    𐡄𐡍(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        أرجع current();
    }
    ثابت uint8_t *pos=pos_;
    𐡄𐡍(pos==nullptr) {
        أرجع USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    لكل(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        𐡄𐡍(sLength<0) {
            لكل(;;) {
                𐡄𐡍((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    أرجع (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                𐡄𐡍(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                𐡄𐡍(inByte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } وإلا {
            لكل(;;) {
                𐡄𐡍(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    أرجع (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                𐡄𐡍(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                𐡄𐡍(inByte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        لكل(;;) {
            int32_t node=*pos++;
            𐡄𐡍(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                𐡄𐡍(result==USTRINGTRIE_NO_MATCH) {
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                𐡄𐡍(sLength<0) {
                    𐡄𐡍((inByte=*s++)==0) {
                        أرجع result;
                    }
                } وإلا {
                    𐡄𐡍(sLength==0) {
                        أرجع result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                𐡄𐡍(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } وإلا 𐡄𐡍(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                𐡄𐡍(inByte!=*pos) {
                    stop();
                    أرجع USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                اكسر;
            } وإلا 𐡄𐡍(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                أرجع USTRINGTRIE_NO_MATCH;
            } وإلا {
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
    طالما(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        𐡄𐡍(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            أرجع nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    𐡏𐡁𐡃 {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t node=*pos++;
        UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
        int32_t value=readValue(pos, node>>1);
        pos=skipValue(pos, node);
        𐡄𐡍(isFinal) {
            𐡄𐡍(haveUniqueValue) {
                𐡄𐡍(value!=uniqueValue) {
                    أرجع nullptr;
                }
            } وإلا {
                uniqueValue=value;
                haveUniqueValue=𐡒𐡔𐡈;
            }
        } وإلا {
            𐡄𐡍(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                أرجع nullptr;
            }
            haveUniqueValue=𐡒𐡔𐡈;
        }
    } طالما(--length>1);
    أرجع pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(ثابت uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    لكل(;;) {
        int32_t node=*pos++;
        𐡄𐡍(node<kMinLinearMatch) {
            𐡄𐡍(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            𐡄𐡍(pos==nullptr) {
                أرجع خطأ;
            }
            haveUniqueValue=𐡒𐡔𐡈;
        } وإلا 𐡄𐡍(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } وإلا {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            𐡄𐡍(haveUniqueValue) {
                𐡄𐡍(value!=uniqueValue) {
                    أرجع خطأ;
                }
            } وإلا {
                uniqueValue=value;
                haveUniqueValue=𐡒𐡔𐡈;
            }
            𐡄𐡍(isFinal) {
                أرجع 𐡒𐡔𐡈;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) ثابت {
    ثابت uint8_t *pos=pos_;
    𐡄𐡍(pos==nullptr) {
        أرجع 0;
    }
    𐡄𐡍(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        أرجع 1;
    }
    int32_t node=*pos++;
    𐡄𐡍(node>=kMinValueLead) {
        𐡄𐡍(node&kValueIsFinal) {
            أرجع 0;
        } وإلا {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    𐡄𐡍(node<kMinLinearMatch) {
        𐡄𐡍(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        أرجع node;
    } وإلا {
        // First byte of the linear-match node.
        append(out, *pos);
        أرجع 1;
    }
}

فراغ
BytesTrie::getNextBranchBytes(ثابت uint8_t *pos, int32_t length, ByteSink &out) {
    طالما(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    𐡏𐡁𐡃 {
        append(out, *pos++);
        pos=skipValue(pos);
    } طالما(--length>1);
    append(out, *pos);
}

فراغ
BytesTrie::append(ByteSink &out, صحيح c) {
    حرف ch = static_cast<حرف>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
