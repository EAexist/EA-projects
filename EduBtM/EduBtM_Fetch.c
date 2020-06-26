/******************************************************************************/
/*                                                                            */
/*    Copyright (c) 2013-2015, Kyu-Young Whang, KAIST                         */
/*    All rights reserved.                                                    */
/*                                                                            */
/*    Redistribution and use in source and binary forms, with or without      */
/*    modification, are permitted provided that the following conditions      */
/*    are met:                                                                */
/*                                                                            */
/*    1. Redistributions of source code must retain the above copyright       */
/*       notice, this list of conditions and the following disclaimer.        */
/*                                                                            */
/*    2. Redistributions in binary form must reproduce the above copyright    */
/*       notice, this list of conditions and the following disclaimer in      */
/*       the documentation and/or other materials provided with the           */
/*       distribution.                                                        */
/*                                                                            */
/*    3. Neither the name of the copyright holder nor the names of its        */
/*       contributors may be used to endorse or promote products derived      */
/*       from this software without specific prior written permission.        */
/*                                                                            */
/*    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     */
/*    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       */
/*    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS       */
/*    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE          */
/*    COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,    */
/*    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;        */
/*    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER        */
/*    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT      */
/*    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN       */
/*    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE         */
/*    POSSIBILITY OF SUCH DAMAGE.                                             */
/*                                                                            */
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*    ODYSSEUS/EduCOSMOS Educational Purpose Object Storage System            */
/*    (Version 1.0)                                                           */
/*                                                                            */
/*    Developed by Professor Kyu-Young Whang et al.                           */
/*                                                                            */
/*    Advanced Information Technology Research Center (AITrc)                 */
/*    Korea Advanced Institute of Science and Technology (KAIST)              */
/*                                                                            */
/*    e-mail: odysseus.educosmos@gmail.com                                    */
/*                                                                            */
/******************************************************************************/
/*
 * Module: EduBtM_Fetch.c
 *
 * Description :
 *  Find the first object satisfying the given condition.
 *  If there is no such object, then return with 'flag' field of cursor set
 *  to CURSOR_EOS. If there is an object satisfying the condition, then cursor
 *  points to the object position in the B+ tree and the object identifier
 *  is returned via 'cursor' parameter.
 *  The condition is given with a key value and a comparison operator;
 *  the comparison operator is one among SM_BOF, SM_EOF, SM_EQ, SM_LT, SM_LE, SM_GT, SM_GE.
 *
 * Exports:
 *  Four EduBtM_Fetch(PageID*, KeyDesc*, KeyValue*, Four, KeyValue*, Four, BtreeCursor*)
 */


#include <string.h>
#include "EduBtM_common.h"
#include "BfM.h"
#include "EduBtM_Internal.h"


/*@ Internal Function Prototypes */
Four edubtm_Fetch(PageID*, KeyDesc*, KeyValue*, Four, KeyValue*, Four, BtreeCursor*);



/*@================================
 * EduBtM_Fetch()
 *================================*/
/*
 * Function: Four EduBtM_Fetch(PageID*, KeyDesc*, KeyVlaue*, Four, KeyValue*, Four, BtreeCursor*)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  Find the first object satisfying the given condition. See above for detail.
 *
 * Returns:
 *  error code
 *    eBADPARAMETER_BTM
 *    some errors caused by function calls
 *
 * Side effects:
 *  cursor  : The found ObjectID and its position in the Btree Leaf
 *            (it may indicate a ObjectID in an  overflow page).
 */
Four EduBtM_Fetch(
    PageID   *root,		/* IN The current root of the subtree */
    KeyDesc  *kdesc,		/* IN Btree key descriptor */
    KeyValue *startKval,	/* IN key value of start condition */
    Four     startCompOp,	/* IN comparison operator of start condition */
    KeyValue *stopKval,		/* IN key value of stop condition */
    Four     stopCompOp,	/* IN comparison operator of stop condition */
    BtreeCursor *cursor)	/* OUT Btree Cursor */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    int i;
    Four e;		   /* error number */
    
    if (root == NULL) ERR(eBADPARAMETER_BTM);

    /* Error check whether using not supported functionality by EduBtM */
    for(i=0; i<kdesc->nparts; i++)
    {
        if(kdesc->kpart[i].type!=SM_INT && kdesc->kpart[i].type!=SM_VARSTRING)
            ERR(eNOTSUPPORTED_EDUBTM);
    }

    /**/
    switch(startCompOp){
        case SM_BOF :
            /* Find the first ObjectID of the given Btree */
            if((e = edubtm_FirstObject(root, kdesc, stopKval, stopCompOp, cursor))<0) ERR(e);
            break;

        case SM_EOF :
            /* Find the last ObjectID of the given Btree */
            if((e = edubtm_LastObject(root, kdesc,	stopKval, stopCompOp, cursor))<0) ERR(e);
            break;

        default :
            /* Find the first object satisfying the given condition */
            if((e = btm_Fetch(root, kdesc, startKval, startCompOp, stopKval, stopCompOp, cursor))<0) ERR(e);                
    }    
    /**/
    return(eNOERROR);

} /* EduBtM_Fetch() */



/*@================================
 * edubtm_Fetch()
 *================================*/
/*
 * Function: Four edubtm_Fetch(PageID*, KeyDesc*, KeyVlaue*, Four, KeyValue*, Four, BtreeCursor*)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  Find the first object satisfying the given condition.
 *  This function handles only the following conditions:
 *  SM_EQ, SM_LT, SM_LE, SM_GT, SM_GE.
 *
 * Returns:
 *  Error code *   
 *    eBADCOMPOP_BTM
 *    eBADBTREEPAGE_BTM
 *    some errors caused by function calls
 */
Four edubtm_Fetch(
    PageID              *root,          /* IN The current root of the subtree */
    KeyDesc             *kdesc,         /* IN Btree key descriptor */
    KeyValue            *startKval,     /* IN key value of start condition */
    Four                startCompOp,    /* IN comparison operator of start condition */
    KeyValue            *stopKval,      /* IN key value of stop condition */
    Four                stopCompOp,     /* IN comparison operator of stop condition */
    BtreeCursor         *cursor)        /* OUT Btree Cursor */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Four                e;              /* error number */
    Four                cmp;            /* result of comparison */
    Two                 idx;            /* index */
    PageID              child;          /* child page when the root is an internla page */
    Two                 alignedKlen;    /* aligned size of the key length */
    BtreePage           *apage;         /* a Page Pointer to the given root */
    BtreeOverflow       *opage;         /* a page pointer if it necessary to access an overflow page */
    Boolean             found;          /* search result */
    PageID              *leafPid;       /* leaf page pointed by the cursor */
    Two                 slotNo;         /* slot pointed by the slot */
    PageID              ovPid;          /* PageID of the overflow page */
    PageNo              ovPageNo;       /* PageNo of the overflow page */
    PageID              prevPid;        /* PageID of the previous page */
    PageID              nextPid;        /* PageID of the next page */
    ObjectID            *oidArray;      /* array of the ObjectIDs */
    Two                 iEntryOffset;   /* starting offset of an internal entry */
    btm_InternalEntry   *iEntry;        /* an internal entry */
    Two                 lEntryOffset;   /* starting offset of a leaf entry */
    btm_LeafEntry       *lEntry;        /* a leaf entry */


    /* Error check whether using not supported functionality by EduBtM */
    int i;
    for(i=0; i<kdesc->nparts; i++)
    {
        if(kdesc->kpart[i].type!=SM_INT && kdesc->kpart[i].type!=SM_VARSTRING)
            ERR(eNOTSUPPORTED_EDUBTM);
    }

    /**/

    /* Fix the page to the buffer */
    if((e = BfM_GetTrain(root, (char**)&apage, PAGE_BUF))<0) ERR(e);
    
    if(apage->any.hdr.type & INTERNAL){
        if ((e = edubtm_BinarySearchInternal(&(apage->bi), kdesc, startKval, &idx)) < 0) ERR(e);
        iEntryOffset = apage->bi.slot[-idx];
        iEntry = (btm_InternalEntry *)&(apage->bi.data[iEntryOffset]);
        MAKE_PAGEID(child, root->volNo, iEntry->spid);

        /* Recursive Call */
        if ((e = edubtm_Fetch(&child, kdesc, startKval, startCompOp, stopKval, stopCompOp, cursor))<0) ERR(e);
    }
    
    else if (apage->any.hdr.type & LEAF){
        found = edubtm_BinarySearchLeaf(&(apage->bl), kdesc, startKval, &idx);

        if(found){
            if (startCompOp == SM_LT){
                if (idx == 0){
                    /* If current is the first entry of the whole index*/
                    if(apage->bl.hdr.prevPage == NIL){
                        cursor->flag = CURSOR_EOS;
                        if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);    
                        return(eNOERROR);                        
                    }

                     /* Else */
                    if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);
                    MAKE_PAGEID(*root, root->volNo, apage->bl.hdr.prevPage);        
                    if((e = BfM_GetTrain((TrainID*)root, (char**)&apage, PAGE_BUF)) < 0) ERR(e);
                    idx = apage->bl.hdr.nSlots;
                }
                idx = idx - 1;
            }
                
            else if (startCompOp == SM_GT){
                if (idx == apage->bl.hdr.nSlots-1){
                    /* If current is the first entry of the whole index*/
                    if(apage->bl.hdr.nextPage == NIL){
                        cursor->flag = CURSOR_EOS;
                        if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);    
                        return(eNOERROR);                        
                    }

                     /* Else */
                    if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);
                    MAKE_PAGEID(*root, root->volNo, apage->bl.hdr.nextPage);        
                    if((e = BfM_GetTrain((TrainID*)root, (char**)&apage, PAGE_BUF)) < 0) ERR(e);
                    idx = -1;
                }
                idx = idx + 1;
            }                 
        }

        else{
            if (startCompOp == SM_EQ){
                cursor->flag = CURSOR_EOS;
                if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);    
                return(eNOERROR);                
            }

            else if ((startCompOp == SM_GT) || (startCompOp == SM_GE)){
                if (idx == apage->bl.hdr.nSlots-1){
                    /* If current is the first entry of the whole index*/
                    if(apage->bl.hdr.nextPage == NIL){
                        cursor->flag = CURSOR_EOS;
                        if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);    
                        return(eNOERROR);                        
                    }

                     /* Else */
                    if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);
                    MAKE_PAGEID(*root, root->volNo, apage->bl.hdr.nextPage);        
                    if((e = BfM_GetTrain((TrainID*)root, (char**)&apage, PAGE_BUF)) < 0) ERR(e);
                    idx = -1;
                }
                idx = idx + 1;
            }
        }
    
        lEntryOffset = apage->bl.slot[-idx];    
        lEntry = (btm_LeafEntry *)&(apage->bl.data[lEntryOffset]);

        cmp = edubtm_KeyCompare(kdesc, lEntry->kval, stopKval);

        if(cmp == EQUAL){
            if ((stopCompOp == SM_LT) || (stopCompOp == SM_GT)){
                cursor->flag = CURSOR_EOS;
                if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);    
                return(eNOERROR);             
            }
        }

        if(cmp == LESS){
            if ((stopCompOp == SM_EQ) || (stopCompOp == SM_GT) || (stopCompOp == SM_GE)){
                cursor->flag = CURSOR_EOS;
                if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);    
                return(eNOERROR);             
            }        
        }

        if(cmp == GREATER){
            if ((stopCompOp == SM_EQ) || (stopCompOp == SM_LT) || (stopCompOp == SM_LE)){
                cursor->flag = CURSOR_EOS;
                if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);    
                return(eNOERROR);             
            }        
        }

        /* Satisfied */
        cursor->flag = CURSOR_ON;
        cursor->leaf = *root;
        cursor->slotNo = idx;
        cursor->oidArrayElemNo = 1;
        alignedKlen = ALIGNED_LENGTH(lEntry->klen);
        cursor->oid = *(ObjectID *)(&(lEntry->kval) + alignedKlen);
        cursor->key = *(KeyValue *)(&(lEntry->klen));

        /* Unfix the leaf page from the buffer */ 
        if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);    
    
    }
    
    /**/

    return(eNOERROR);
    
} /* edubtm_Fetch() */

