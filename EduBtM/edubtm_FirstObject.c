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
 * Module: edubtm_FirstObject.c
 *
 * Description : 
 *  Find the first ObjectID of the given Btree. 
 *
 * Exports:
 *  Four edubtm_FirstObject(PageID*, KeyDesc*, KeyValue*, Four, BtreeCursor*)
 */


#include <string.h>
#include "EduBtM_common.h"
#include "BfM.h"
#include "EduBtM_Internal.h"



/*@================================
 * edubtm_FirstObject()
 *================================*/
/*
 * Function: Four edubtm_FirstObject(PageID*, KeyDesc*, KeyValue*, Four, BtreeCursor*)
 *
 * Description : 
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  Find the first ObjectID of the given Btree. The 'cursor' will indicate
 *  the first ObjectID in the Btree, and it will be used as successive access
 *  by using the Btree.
 *
 * Returns:
 *  error code
 *    eBADPAGE_BTM
 *    some errors caused by function calls
 *
 * Side effects:
 *  cursor : A position in the Btree which indicates the first ObjectID.
 *             The first object's object identifier is also returned via this.
 */
Four edubtm_FirstObject(
    PageID  		*root,		/* IN The root of Btree */
    KeyDesc 		*kdesc,		/* IN Btree key descriptor */
    KeyValue 		*stopKval,	/* IN key value of stop condition */
    Four     		stopCompOp,	/* IN comparison operator of stop condition */
    BtreeCursor 	*cursor)	/* OUT The first ObjectID in the Btree */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    int			i;
    Four 		e;		/* error */
    Four 		cmp;		/* result of comparison */
    PageID 		curPid;		/* PageID of the current page */
    PageID 		child;		/* PageID of the child page */
    BtreePage 		*apage;		/* a page pointer */
    Two                 lEntryOffset;   /* starting offset of a leaf entry */
    btm_LeafEntry 	*lEntry;	/* a leaf entry */
    Two                 alignedKlen;    /* aligned length of the key length */    

    if (root == NULL) ERR(eBADPAGE_BTM);

    /* Error check whether using not supported functionality by EduBtM */
    for(i=0; i<kdesc->nparts; i++)
    {
        if(kdesc->kpart[i].type!=SM_INT && kdesc->kpart[i].type!=SM_VARSTRING)
            ERR(eNOTSUPPORTED_EDUBTM);
    }

    /**/
    curPid = *root;
    while(TRUE){        
        if((e = BfM_GetTrain((TrainID*)&curPid, (char**)&apage, PAGE_BUF)) < 0) ERR(e); 
        if(apage->any.hdr.type == LEAF) break;
        if(apage->bi.hdr.p0 != NIL) MAKE_PAGEID(child, root->volNo, apage->bi.hdr.p0);
        if((e = BfM_FreeTrain((TrainID*)&curPid, PAGE_BUF))<0) ERR(e);
        curPid = child;
    }
    
    lEntryOffset = apage->bl.slot[0];
    printf("%d",lEntryOffset);
    lEntry = (btm_LeafEntry *)(&(apage->bl.data[lEntryOffset]));

    cursor->flag = CURSOR_ON;
    alignedKlen = ALIGNED_LENGTH(lEntry->klen);
    cursor->oid = *((ObjectID *)(&(lEntry->kval[0]) + alignedKlen));
    cursor->key = *((KeyValue *)(&(lEntry->klen)));
    cursor->leaf = curPid;
    cursor->slotNo = 0;
    cursor->oidArrayElemNo = 1;    
    
    /**/
    return(eNOERROR);
    
} /* edubtm_FirstObject() */
