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
    အကယ်၍(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } မဟုတ်ပါက အကယ်၍(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } မဟုတ်ပါက အကယ်၍(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } မဟုတ်ပါက အကယ်၍(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } မဟုတ်ပါက {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ပြန်ပေး value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    အကယ်၍(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } မဟုတ်ပါက အကယ်၍(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } မဟုတ်ပါက အကယ်၍(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } မဟုတ်ပါက အကယ်၍(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } မဟုတ်ပါက {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ပြန်ပေး pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    အကယ်၍(pos==nullptr) {
        ပြန်ပေး USTRINGTRIE_NO_MATCH;
    } မဟုတ်ပါက {
        int32_t node;
        ပြန်ပေး (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    အကယ်၍(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    နေစဉ်(length>kMaxBranchLinearSubNodeLength) {
        အကယ်၍(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } မဟုတ်ပါက {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        အကယ်၍(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            အကယ်၍(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } မဟုတ်ပါက {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                အကယ်၍(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } မဟုတ်ပါက အကယ်၍(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } မဟုတ်ပါက အကယ်၍(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } မဟုတ်ပါက အကယ်၍(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } မဟုတ်ပါက {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ပြန်ပေး result;
        }
        --length;
        pos=skipValue(pos);
    } နေစဉ်(length>1);
    အကယ်၍(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ပြန်ပေး node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } မဟုတ်ပါက {
        stop();
        ပြန်ပေး USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    အတွက်(;;) {
        int32_t node=*pos++;
        အကယ်၍(node<kMinLinearMatch) {
            ပြန်ပေး branchNext(pos, node, inByte);
        } မဟုတ်ပါက အကယ်၍(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            အကယ်၍(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ပြန်ပေး (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } မဟုတ်ပါက {
                // No match.
                bha_nga;
            }
        } မဟုတ်ပါက အကယ်၍(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } မဟုတ်ပါက {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ပြန်ပေး USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    အကယ်၍(pos==nullptr) {
        ပြန်ပေး USTRINGTRIE_NO_MATCH;
    }
    အကယ်၍(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    အကယ်၍(length>=0) {
        // Remaining part of a linear-match node.
        အကယ်၍(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ပြန်ပေး (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } မဟုတ်ပါက {
            stop();
            ပြန်ပေး USTRINGTRIE_NO_MATCH;
        }
    }
    ပြန်ပေး nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    အကယ်၍(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ပြန်ပေး current();
    }
    const uint8_t *pos=pos_;
    အကယ်၍(pos==nullptr) {
        ပြန်ပေး USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    အတွက်(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        အကယ်၍(sLength<0) {
            အတွက်(;;) {
                အကယ်၍((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ပြန်ပေး (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                အကယ်၍(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                အကယ်၍(inByte!=*pos) {
                    stop();
                    ပြန်ပေး USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } မဟုတ်ပါက {
            အတွက်(;;) {
                အကယ်၍(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ပြန်ပေး (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                အကယ်၍(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                အကယ်၍(inByte!=*pos) {
                    stop();
                    ပြန်ပေး USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        အတွက်(;;) {
            int32_t node=*pos++;
            အကယ်၍(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                အကယ်၍(result==USTRINGTRIE_NO_MATCH) {
                    ပြန်ပေး USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                အကယ်၍(sLength<0) {
                    အကယ်၍((inByte=*s++)==0) {
                        ပြန်ပေး result;
                    }
                } မဟုတ်ပါက {
                    အကယ်၍(sLength==0) {
                        ပြန်ပေး result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                အကယ်၍(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ပြန်ပေး USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } မဟုတ်ပါက အကယ်၍(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                အကယ်၍(inByte!=*pos) {
                    stop();
                    ပြန်ပေး USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } မဟုတ်ပါက အကယ်၍(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ပြန်ပေး USTRINGTRIE_NO_MATCH;
            } မဟုတ်ပါက {
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
    နေစဉ်(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        အကယ်၍(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ပြန်ပေး nullptr;
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
        အကယ်၍(isFinal) {
            အကယ်၍(haveUniqueValue) {
                အကယ်၍(value!=uniqueValue) {
                    ပြန်ပေး nullptr;
                }
            } မဟုတ်ပါက {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } မဟုတ်ပါက {
            အကယ်၍(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ပြန်ပေး nullptr;
            }
            haveUniqueValue=true;
        }
    } နေစဉ်(--length>1);
    ပြန်ပေး pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    အတွက်(;;) {
        int32_t node=*pos++;
        အကယ်၍(node<kMinLinearMatch) {
            အကယ်၍(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            အကယ်၍(pos==nullptr) {
                ပြန်ပေး false;
            }
            haveUniqueValue=true;
        } မဟုတ်ပါက အကယ်၍(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } မဟုတ်ပါက {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            အကယ်၍(haveUniqueValue) {
                အကယ်၍(value!=uniqueValue) {
                    ပြန်ပေး false;
                }
            } မဟုတ်ပါက {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            အကယ်၍(isFinal) {
                ပြန်ပေး true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    အကယ်၍(pos==nullptr) {
        ပြန်ပေး 0;
    }
    အကယ်၍(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ပြန်ပေး 1;
    }
    int32_t node=*pos++;
    အကယ်၍(node>=kMinValueLead) {
        အကယ်၍(node&kValueIsFinal) {
            ပြန်ပေး 0;
        } မဟုတ်ပါက {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    အကယ်၍(node<kMinLinearMatch) {
        အကယ်၍(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ပြန်ပေး node;
    } မဟုတ်ပါက {
        // First byte of the linear-match node.
        append(out, *pos);
        ပြန်ပေး 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    နေစဉ်(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } နေစဉ်(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, ကိန်းပြည့် c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
