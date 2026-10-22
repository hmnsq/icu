// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 2001-2016, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

#నిర్వచితం_కాకపోతే RBBINODE_H
#నిర్వచించు RBBINODE_H

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/uobject.h"

//
//  class RBBINode
//
//                    Represents a node in the parse tree generated when reading
//                    a rule file.
//

U_NAMESPACE_BEGIN

class    Unicode_సమితి;
class    UVector;

class RBBINode : బహిరంగ UMemory {
    బహిరంగ:
        గణన కణుపు_Type {
            సమితి_Ref,
            uset,
            varRef,
            ఆకు_Char,
            lookAhead,
            ట్యాగ్,
            చివర_Mark,
            op_ప్రారంభం,
            opCat,
            opOr,
            op_నక్షత్రం,
            opPlus,
            opQuestion,
            opBreak,
            op_తిరగేయి,
            opLParen
        };

        గణన OpPrecedence {      
            precZero,
            prec_ప్రారంభం,
            precLParen,
            precOpOr,
            precOpCat
        };
            
        కణుపు_Type      fType;
        RBBINode      *f_మాతృ;
        RBBINode      *f_ఎడమ_బిడ్డ;
        RBBINode      *f_కుడి_బిడ్డ;
        Unicode_సమితి    *f_ఆదానం_సమితి;           // For uset nodes only.
        OpPrecedence  fPrecedence;          // For binary ops only.
        
        UnicodeString f_పాఠ్యం;                // Text corresponding to this node.
                                            //   May be lazily evaluated when (if) needed
                                            //   for some node types.
        పూర్ణాంకం           f_మొదటి_Pos;            // Position in the rule source string of the
                                            //   first text associated with the node.
                                            //   If there's a left child, this will be the same
                                            //   as that child's left pos.
        పూర్ణాంకం           f_చివరి_Pos;             //  Last position in the rule source string
                                            //    of any text associated with this node.
                                            //    If there's a right child, this will be the same
                                            //    as that child's last position.

        UBool         fNullable;            // See Aho.
        int32_t       fVal;                 // For leafChar nodes, the value.
                                            //   Values are the character category,
                                            //   corresponds to columns in the final
                                            //   state transition table.

        UBool         f_Look_Ahead_చివర;        // For endMark nodes, set true if
                                            //   marking the end of a look-ahead rule.

        UBool         f_నియమం_మూలం;            // True if this node is the root of a rule.
        UBool         f_Chain_లోపల;             // True if chaining into this rule is allowed
                                            //     (no '^' present).

        UVector       *f_మొదటి_Pos_సమితి;
        UVector       *f_చివరి_Pos_సమితి;         // TODO: rename fFirstPos & fLastPos to avoid confusion.
        UVector       *fFollowPos;


        RBBINode(కణుపు_Type t, UError_సంకేతం& స్థితి_2);
        RBBINode(స్థిరరాశి RBBINode &other, UError_సంకేతం& స్థితి_2);
        ~RBBINode();
        స్థిర శూన్య  NRDelete_కణుపు(RBBINode *కణుపు);
        
        RBBINode    *clone_వృక్షం(UError_సంకేతం &స్థితి_2, పూర్ణాంకం లోతు=0);
        RBBINode    *flattenVariables(UError_సంకేతం &స్థితి_2, పూర్ణాంకం లోతు=0);
        శూన్య         flattenSets(UError_సంకేతం &స్థితి_2, పూర్ణాంకం లోతు=0);
        శూన్య         కనుగొను_Nodes(UVector *dest, RBBINode::కణుపు_Type రకం, UError_సంకేతం &స్థితి_2);

#నిర్వచితమైతే RBBI_DEBUG
        స్థిర శూన్య print_కణుపు_శీర్షిక();
        స్థిర శూన్య print_కణుపు(స్థిరరాశి RBBINode *n);
        స్థిర శూన్య print_వృక్షం(స్థిరరాశి RBBINode *n, UBool withHeading);
#అయితే_ముగింపు

    అంతరంగ:
        RBBINode &క్రియాకారకం = (స్థిరరాశి RBBINode &other); // No defs.
        bool క్రియాకారకం == (స్థిరరాశి RBBINode &other);     // Private, so these functions won't accidentally be used.

#నిర్వచితమైతే RBBI_DEBUG
    బహిరంగ:
        పూర్ణాంకం           f_Serial_అంకె;           //  Debugging aids.
#అయితే_ముగింపు
};

#నిర్వచితమైతే RBBI_DEBUG
U_CFUNC శూన్య 
RBBI_DEBUG_printUnicodeString(స్థిరరాశి UnicodeString &s, పూర్ణాంకం min_వెడల్పు=0);
#అయితే_ముగింపు

U_NAMESPACE_చివర

#అయితే_ముగింపు

