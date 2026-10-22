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

#include "unicode/utypes.h"
#include "unicode/bytestream.h"
#include "unicode/bytestrie.h"
#include "unicode/uobject.h"
#include "cmemory.h"
#include "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::readValue(const uint8_t *pos, int32_t leadByte) {
    int32_t value;
    නම්(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } නැතහොත් නම්(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } නැතහොත් නම්(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } නැතහොත් නම්(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } නැතහොත් {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ආපසු value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    නම්(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } නැතහොත් නම්(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } නැතහොත් නම්(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } නැතහොත් නම්(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } නැතහොත් {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ආපසු pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    නම්(pos==nullptr) {
        ආපසු USTRINGTRIE_NO_MATCH;
    } නැතහොත් {
        int32_t node;
        ආපසු (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    නම්(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    අතරතුර(length>kMaxBranchLinearSubNodeLength) {
        නම්(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } නැතහොත් {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        නම්(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            නම්(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } නැතහොත් {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                නම්(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } නැතහොත් නම්(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } නැතහොත් නම්(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } නැතහොත් නම්(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } නැතහොත් {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ආපසු result;
        }
        --length;
        pos=skipValue(pos);
    } අතරතුර(length>1);
    නම්(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ආපසු node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } නැතහොත් {
        stop();
        ආපසු USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    සඳහා(;;) {
        int32_t node=*pos++;
        නම්(node<kMinLinearMatch) {
            ආපසු branchNext(pos, node, inByte);
        } නැතහොත් නම්(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            නම්(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ආපසු (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } නැතහොත් {
                // No match.
                bha_nga;
            }
        } නැතහොත් නම්(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } නැතහොත් {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ආපසු USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    නම්(pos==nullptr) {
        ආපසු USTRINGTRIE_NO_MATCH;
    }
    නම්(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    නම්(length>=0) {
        // Remaining part of a linear-match node.
        නම්(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ආපසු (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } නැතහොත් {
            stop();
            ආපසු USTRINGTRIE_NO_MATCH;
        }
    }
    ආපසු nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    නම්(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ආපසු current();
    }
    const uint8_t *pos=pos_;
    නම්(pos==nullptr) {
        ආපසු USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    සඳහා(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        නම්(sLength<0) {
            සඳහා(;;) {
                නම්((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ආපසු (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                නම්(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                නම්(inByte!=*pos) {
                    stop();
                    ආපසු USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } නැතහොත් {
            සඳහා(;;) {
                නම්(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ආපසු (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                නම්(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                නම්(inByte!=*pos) {
                    stop();
                    ආපසු USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        සඳහා(;;) {
            int32_t node=*pos++;
            නම්(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                නම්(result==USTRINGTRIE_NO_MATCH) {
                    ආපසු USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                නම්(sLength<0) {
                    නම්((inByte=*s++)==0) {
                        ආපසු result;
                    }
                } නැතහොත් {
                    නම්(sLength==0) {
                        ආපසු result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                නම්(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ආපසු USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } නැතහොත් නම්(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                නම්(inByte!=*pos) {
                    stop();
                    ආපසු USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } නැතහොත් නම්(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ආපසු USTRINGTRIE_NO_MATCH;
            } නැතහොත් {
                // Skip intermediate value.
                pos=skipValue(pos, node);
                // The next node must not also be a value node.
                U_ASSERT(*pos<kMinValueLead);
            }
        }
    }
}

const uint8_t*
BytesTrie::findUniqueValueFromBranch(const uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool haveUniqueValue,
                                     int32_t& uniqueValue) {
    අතරතුර(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        නම්(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ආපසු nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t node=*pos++;
        UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
        int32_t value=readValue(pos, node>>1);
        pos=skipValue(pos, node);
        නම්(isFinal) {
            නම්(haveUniqueValue) {
                නම්(value!=uniqueValue) {
                    ආපසු nullptr;
                }
            } නැතහොත් {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } නැතහොත් {
            නම්(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ආපසු nullptr;
            }
            haveUniqueValue=true;
        }
    } අතරතුර(--length>1);
    ආපසු pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    සඳහා(;;) {
        int32_t node=*pos++;
        නම්(node<kMinLinearMatch) {
            නම්(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            නම්(pos==nullptr) {
                ආපසු false;
            }
            haveUniqueValue=true;
        } නැතහොත් නම්(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } නැතහොත් {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            නම්(haveUniqueValue) {
                නම්(value!=uniqueValue) {
                    ආපසු false;
                }
            } නැතහොත් {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            නම්(isFinal) {
                ආපසු true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    නම්(pos==nullptr) {
        ආපසු 0;
    }
    නම්(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ආපසු 1;
    }
    int32_t node=*pos++;
    නම්(node>=kMinValueLead) {
        නම්(node&kValueIsFinal) {
            ආපසු 0;
        } නැතහොත් {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    නම්(node<kMinLinearMatch) {
        නම්(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ආපසු node;
    } නැතහොත් {
        // First byte of the linear-match node.
        append(out, *pos);
        ආපසු 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    අතරතුර(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } අතරතුර(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, පූර්ණ_සංඛ්‍යා c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
