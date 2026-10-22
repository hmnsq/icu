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
    གལ་ཏེ(leadByte<kMinTwoByteValueLead) {
        value=leadByte-kMinOneByteValueLead;
    } དེ་མིན གལ་ཏེ(leadByte<kMinThreeByteValueLead) {
        value=((leadByte-kMinTwoByteValueLead)<<8)|*pos;
    } དེ་མིན གལ་ཏེ(leadByte<kFourByteValueLead) {
        value=((leadByte-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
    } དེ་མིན གལ་ཏེ(leadByte==kFourByteValueLead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } དེ་མིན {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ཕྱིར value;
}

const uint8_t*
BytesTrie::jumpByDelta(const uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    གལ་ཏེ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } དེ་མིན གལ་ཏེ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } དེ་མིན གལ་ཏེ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } དེ་མིན གལ་ཏེ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } དེ་མིན {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ཕྱིར pos+delta;
}

UStringTrieResult
BytesTrie::current() const {
    const uint8_t *pos=pos_;
    གལ་ཏེ(pos==nullptr) {
        ཕྱིར USTRINGTRIE_NO_MATCH;
    } དེ་མིན {
        int32_t node;
        ཕྱིར (remainingMatchLength_<0 && (node=*pos)>=kMinValueLead) ?
                valueResult(node) : USTRINGTRIE_NO_VALUE;
    }
}

UStringTrieResult
BytesTrie::branchNext(const uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    གལ་ཏེ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    སྐབས(length>kMaxBranchLinearSubNodeLength) {
        གལ་ཏེ(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } དེ་མིན {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        གལ་ཏེ(inByte==*pos++) {
            UStringTrieResult result;
            int32_t node=*pos;
            U_ASSERT(node>=kMinValueLead);
            གལ་ཏེ(node&kValueIsFinal) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_VALUE;
            } དེ་མིན {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                node>>=1;
                int32_t delta;
                གལ་ཏེ(node<kMinTwoByteValueLead) {
                    delta=node-kMinOneByteValueLead;
                } དེ་མིན གལ་ཏེ(node<kMinThreeByteValueLead) {
                    delta=((node-kMinTwoByteValueLead)<<8)|*pos++;
                } དེ་མིན གལ་ཏེ(node<kFourByteValueLead) {
                    delta=((node-kMinThreeByteValueLead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } དེ་མིན གལ་ཏེ(node==kFourByteValueLead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } དེ་མིན {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                node=*pos;
                result= node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
            }
            pos_=pos;
            ཕྱིར result;
        }
        --length;
        pos=skipValue(pos);
    } སྐབས(length>1);
    གལ་ཏེ(inByte==*pos++) {
        pos_=pos;
        int32_t node=*pos;
        ཕྱིར node>=kMinValueLead ? valueResult(node) : USTRINGTRIE_NO_VALUE;
    } དེ་མིན {
        stop();
        ཕྱིར USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(const uint8_t *pos, int32_t inByte) {
    ཆེད་དུ(;;) {
        int32_t node=*pos++;
        གལ་ཏེ(node<kMinLinearMatch) {
            ཕྱིར branchNext(pos, node, inByte);
        } དེ་མིན གལ་ཏེ(node<kMinValueLead) {
            // Match the first of length+1 bytes.
            int32_t length=node-kMinLinearMatch;  // Actual match length minus 1.
            གལ་ཏེ(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ཕྱིར (length<0 && (node=*pos)>=kMinValueLead) ?
                        valueResult(node) : USTRINGTRIE_NO_VALUE;
            } དེ་མིན {
                // No match.
                bha_nga;
            }
        } དེ་མིན གལ་ཏེ(node&kValueIsFinal) {
            // No further matching bytes.
            bha_nga;
        } དེ་མིན {
            // Skip intermediate value.
            pos=skipValue(pos, node);
            // The next node must not also be a value node.
            U_ASSERT(*pos<kMinValueLead);
        }
    }
    stop();
    ཕྱིར USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    const uint8_t *pos=pos_;
    གལ་ཏེ(pos==nullptr) {
        ཕྱིར USTRINGTRIE_NO_MATCH;
    }
    གལ་ཏེ(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    གལ་ཏེ(length>=0) {
        // Remaining part of a linear-match node.
        གལ་ཏེ(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t node;
            ཕྱིར (length<0 && (node=*pos)>=kMinValueLead) ?
                    valueResult(node) : USTRINGTRIE_NO_VALUE;
        } དེ་མིན {
            stop();
            ཕྱིར USTRINGTRIE_NO_MATCH;
        }
    }
    ཕྱིར nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(const var_na *s, int32_t sLength) {
    གལ་ཏེ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ཕྱིར current();
    }
    const uint8_t *pos=pos_;
    གལ་ཏེ(pos==nullptr) {
        ཕྱིར USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ཆེད་དུ(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        གལ་ཏེ(sLength<0) {
            ཆེད་དུ(;;) {
                གལ་ཏེ((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ཕྱིར (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                གལ་ཏེ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                གལ་ཏེ(inByte!=*pos) {
                    stop();
                    ཕྱིར USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } དེ་མིན {
            ཆེད་དུ(;;) {
                གལ་ཏེ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t node;
                    ཕྱིར (length<0 && (node=*pos)>=kMinValueLead) ?
                            valueResult(node) : USTRINGTRIE_NO_VALUE;
                }
                inByte=*s++;
                --sLength;
                གལ་ཏེ(length<0) {
                    remainingMatchLength_=length;
                    bha_nga;
                }
                གལ་ཏེ(inByte!=*pos) {
                    stop();
                    ཕྱིར USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        ཆེད་དུ(;;) {
            int32_t node=*pos++;
            གལ་ཏེ(node<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, node, inByte);
                གལ་ཏེ(result==USTRINGTRIE_NO_MATCH) {
                    ཕྱིར USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                གལ་ཏེ(sLength<0) {
                    གལ་ཏེ((inByte=*s++)==0) {
                        ཕྱིར result;
                    }
                } དེ་མིན {
                    གལ་ཏེ(sLength==0) {
                        ཕྱིར result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                གལ་ཏེ(result==USTRINGTRIE_FINAL_VALUE) {
                    // No further matching bytes.
                    stop();
                    ཕྱིར USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } དེ་མིན གལ་ཏེ(node<kMinValueLead) {
                // Match length+1 bytes.
                length=node-kMinLinearMatch;  // Actual match length minus 1.
                གལ་ཏེ(inByte!=*pos) {
                    stop();
                    ཕྱིར USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                bha_nga;
            } དེ་མིན གལ་ཏེ(node&kValueIsFinal) {
                // No further matching bytes.
                stop();
                ཕྱིར USTRINGTRIE_NO_MATCH;
            } དེ་མིན {
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
    སྐབས(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        གལ་ཏེ(nullptr==findUniqueValueFromBranch(jumpByDelta(pos), length>>1, haveUniqueValue, uniqueValue)) {
            ཕྱིར nullptr;
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
        གལ་ཏེ(isFinal) {
            གལ་ཏེ(haveUniqueValue) {
                གལ་ཏེ(value!=uniqueValue) {
                    ཕྱིར nullptr;
                }
            } དེ་མིན {
                uniqueValue=value;
                haveUniqueValue=true;
            }
        } དེ་མིན {
            གལ་ཏེ(!findUniqueValue(pos+value, haveUniqueValue, uniqueValue)) {
                ཕྱིར nullptr;
            }
            haveUniqueValue=true;
        }
    } སྐབས(--length>1);
    ཕྱིར pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::findUniqueValue(const uint8_t *pos, UBool haveUniqueValue, int32_t &uniqueValue) {
    ཆེད་དུ(;;) {
        int32_t node=*pos++;
        གལ་ཏེ(node<kMinLinearMatch) {
            གལ་ཏེ(node==0) {
                node=*pos++;
            }
            pos=findUniqueValueFromBranch(pos, node+1, haveUniqueValue, uniqueValue);
            གལ་ཏེ(pos==nullptr) {
                ཕྱིར false;
            }
            haveUniqueValue=true;
        } དེ་མིན གལ་ཏེ(node<kMinValueLead) {
            // linear-match node
            pos+=node-kMinLinearMatch+1;  // Ignore the match bytes.
        } དེ་མིན {
            UBool isFinal = static_cast<UBool>(node & kValueIsFinal);
            int32_t value=readValue(pos, node>>1);
            གལ་ཏེ(haveUniqueValue) {
                གལ་ཏེ(value!=uniqueValue) {
                    ཕྱིར false;
                }
            } དེ་མིན {
                uniqueValue=value;
                haveUniqueValue=true;
            }
            གལ་ཏེ(isFinal) {
                ཕྱིར true;
            }
            pos=skipValue(pos, node);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) const {
    const uint8_t *pos=pos_;
    གལ་ཏེ(pos==nullptr) {
        ཕྱིར 0;
    }
    གལ་ཏེ(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ཕྱིར 1;
    }
    int32_t node=*pos++;
    གལ་ཏེ(node>=kMinValueLead) {
        གལ་ཏེ(node&kValueIsFinal) {
            ཕྱིར 0;
        } དེ་མིན {
            pos=skipValue(pos, node);
            node=*pos++;
            U_ASSERT(node<kMinValueLead);
        }
    }
    གལ་ཏེ(node<kMinLinearMatch) {
        གལ་ཏེ(node==0) {
            node=*pos++;
        }
        getNextBranchBytes(pos, ++node, out);
        ཕྱིར node;
    } དེ་མིན {
        // First byte of the linear-match node.
        append(out, *pos);
        ཕྱིར 1;
    }
}

void
BytesTrie::getNextBranchBytes(const uint8_t *pos, int32_t length, ByteSink &out) {
    སྐབས(length>kMaxBranchLinearSubNodeLength) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        append(out, *pos++);
        pos=skipValue(pos);
    } སྐབས(--length>1);
    append(out, *pos);
}

void
BytesTrie::append(ByteSink &out, ཧྲིལ c) {
    var_na ch = static_cast<var_na>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
