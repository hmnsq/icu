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
    אם(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } אחרת אם(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } אחרת אם(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } אחרת אם(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } אחרת {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    החזר value;
}

ثابت uint8_t*
BytesTrie::jumpByDelta(ثابت uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    אם(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } אחרת אם(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } אחרת אם(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } אחרת אם(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } אחרת {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    החזר pos+delta;
}

UStringTrieResult
BytesTrie::current() ثابت {
    ثابت uint8_t *pos=pos_;
    אם(pos==nullptr) {
        החזר USTRINGTRIE_NO_MATCH;
    } אחרת {
        int32_t node;
        החזר (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(ثابت uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    אם(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    כלעוד(length>kMaxBranchLinearSubNodeLength) {
        אם(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } אחרת {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    עשה {
        אם(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            אם(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } אחרת {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                אם(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } אחרת אם(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } אחרת אם(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } אחרת אם(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } אחרת {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            החזר result;
        }
        --length;
        pos=skipValue(pos);
    } כלעוד(length>1);
    אם(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        החזר node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } אחרת {
        stop();
        החזר USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ثابت uint8_t *pos, int32_t inByte) {
    עבור(;;) {
        int32_t node=*pos++;
        אם(node<kMinLinearMatch) {
            החזר branchNext(pos, node, inByte);
        } אחרת אם(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            אם(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                החזר (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } אחרת {
                // No match.
                اكسر;
            }
        } אחרת אם(node&kValueIsFinal) {
            // No further matching bytes.
            اكسر;
        } אחרת {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    החזר USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    ثابت uint8_t *pos=pos_;
    אם(pos==nullptr) {
        החזר USTRINGTRIE_NO_MATCH;
    }
    אם(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    אם(length>=0) {
        // Remaining part of a linear-match node.
        אם(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            החזר (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } אחרת {
            stop();
            החזר USTRINGTRIE_NO_MATCH;
        }
    }
    החזר nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(ثابت حرف *s, int32_t sLength) {
    אם(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        החזר current();
    }
    ثابت uint8_t *pos=pos_;
    אם(pos==nullptr) {
        החזר USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    עבור(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        אם(sLength<0) {
            עבור(;;) {
                אם((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    החזר (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                אם(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                אם(inByte!=*pos) {
                    stop();
                    החזר USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } אחרת {
            עבור(;;) {
                אם(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    החזר (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                אם(length<0) {
                    remainingMatchLength_=length;
                    اكسر;
                }
                אם(inByte!=*pos) {
                    stop();
                    החזר USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        עבור(;;) {
            int32_t node=*pos++;
            אם(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                אם(result==USTRINGTRIE_NO_MATCH) {
                    החזר USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                אם(sLength<0) {
                    אם((inByte=*s++)==0) {
                        החזר result;
                    }
                } אחרת {
                    אם(sLength==0) {
                        החזר result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                אם(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    החזר USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } אחרת אם(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                אם(inByte!=*pos) {
                    stop();
                    החזר USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                اكسر;
            } אחרת אם(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                החזר USTRINGTRIE_NO_MATCH;
            } אחרת {
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
    כלעוד(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        אם(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            החזר nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    עשה {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t node=*pos++;
        UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
        int32_t value=readValue(pos, node>>1);
        pos=skipValue(pos, node);
        אם(isFinal) {
            אם(haveUniqueValue) {
                אם(value!=uniqueValue) {
                    החזר nullptr;
                }
            } אחרת {
                uniqueValue=value;
                haveUniqueValue=אמת;
            }
        } אחרת {
            אם(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                החזר nullptr;
            }
            haveUniqueValue=אמת;
        }
    } כלעוד(--length>1);
    החזר pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(ثابت uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    עבור(;;) {
        int32_t node=*pos++;
        אם(node<kMinLinearMatch) {
            אם(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            אם(pos==nullptr) {
                החזר שקר;
            }
            haveUniqueValue=אמת;
        } אחרת אם(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } אחרת {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            אם(haveUniqueValue) {
                אם(value!=uniqueValue) {
                    החזר שקר;
                }
            } אחרת {
                uniqueValue=value;
                haveUniqueValue=אמת;
            }
            אם(isFinal) {
                החזר אמת;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) ثابت {
    ثابت uint8_t *pos=pos_;
    אם(pos==nullptr) {
        החזר 0;
    }
    אם(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        החזר 1;
    }
    int32_t node=*pos++;
    אם(node>=kMinValueLead) {
        אם(node&kValueIsFinal) {
            החזר 0;
        } אחרת {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    אם(node<kMinLinearMatch) {
        אם(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        החזר node;
    } אחרת {
        // First byte of the linear-match node.
        append(out, *pos);
        החזר 1;
    }
}

فراغ
BytesTrie::getNextBranchBytes(ثابت uint8_t *pos, int32_t length, ByteSink &out) {
    כלעוד(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    עשה {
        append(out, *pos++);
        pos=skipValue(pos);
    } כלעוד(--length>1);
    append(out, *pos);
}

فراغ
BytesTrie::append(ByteSink &out, صحيح c) {
    حرف ch = static_cast<حرف>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
