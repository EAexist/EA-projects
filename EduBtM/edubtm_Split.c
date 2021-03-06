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
 * Module: edubtm_Split.c
 *
 * Description : 
 *  This file has three functions about 'split'.
 *  'edubtm_SplitInternal(...) and edubtm_SplitLeaf(...) insert the given item
 *  after spliting, and return 'ritem' which should be inserted into the
 *  parent page.
 *
 * Exports:
 *  Four edubtm_SplitInternal(ObjectID*, BtreeInternal*, Two, InternalItem*, InternalItem*)
 *  Four edubtm_SplitLeaf(ObjectID*, PageID*, BtreeLeaf*, Two, LeafItem*, InternalItem*)
 */


#include <string.h>
#include "EduBtM_common.h"
#include "BfM.h"
#include "EduBtM_Internal.h"



/*@================================
 * edubtm_SplitInternal()
 *================================*/
/*
 * Function: Four edubtm_SplitInternal(ObjectID*, BtreeInternal*,Two, InternalItem*, InternalItem*)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  At first, the function edubtm_SplitInternal(...) allocates a new internal page
 *  and initialize it.  Secondly, all items in the given page and the given
 *  'item' are divided by halves and stored to the two pages.  By spliting,
 *  the new internal item should be inserted into their parent and the item will
 *  be returned by 'ritem'.
 *
 *  A temporary page is used because it is difficult to use the given page
 *  directly and the temporary page will be copied to the given page later.
 *
 * Returns:
 *  error code
 *    some errors caused by function calls
 *
 * Note:
 *  The caller should call BfM_SetDirty() for 'fpage'.
 */
Four edubtm_SplitInternal(
    ObjectID                    *catObjForFile,         /* IN catalog object of B+ tree file */
    BtreeInternal               *fpage,                 /* INOUT the page which will be splitted */
    Two                         high,                   /* IN slot No. for the given 'item' */
    InternalItem                *item,                  /* IN the item which will be inserted */
    InternalItem                *ritem)                 /* OUT the item which will be returned by spliting */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Four                        e;                      /* error number */
    Two                         i;                      /* slot No. in the given page, fpage */
    Two                         j;                      /* slot No. in the splitted pages */
    Two                         k;                      /* slot No. in the new page */
    Two                         maxLoop;                /* # of max loops; # of slots in fpage + 1 */
    Four                        sum;                    /* the size of a filled area */
    Boolean                     flag=FALSE;             /* TRUE if 'item' become a member of fpage */
    PageID                      newPid;                 /* for a New Allocated Page */
    BtreeInternal               *npage;                 /* a page pointer for the new allocated page */
    Two                         fEntryOffset;           /* starting offset of an entry in fpage */
    Two                         nEntryOffset;           /* starting offset of an entry in npage */
    Two                         entryLen;               /* length of an entry */
    btm_InternalEntry           *fEntry;                /* internal entry in the given page, fpage */
    btm_InternalEntry           *nEntry;                /* internal entry in the new page, npage*/
    Boolean                     isTmp;

    /**/

    /* Allocate a new page to be used as a B+ index page */
    if((e = btm_AllocPage(catObjForFile, &(fpage->hdr.pid), &newPid))<0) ERR(e);
    /* Initiate the page as a leaf page */
    if((e = btm_InitInternal(&newPid, FALSE, FALSE))<0) ERR(e);
    /* Fix the page that has been newly allocated on the disk to the buffer */    
    if((e = BfM_GetNewTrain(&newPid, (char**)&npage, PAGE_BUF))<0) ERR(e);

    /* Sort index entries + 1 new entry. Insert the entries to the fpage until page is full 50% */
    /* Copy fpage data[] to tpage data[] until tpage is full 50% */






    

    /**/
    return(eNOERROR);
    
} /* edubtm_SplitInternal() */



/*@================================
 * edubtm_SplitLeaf()
 *================================*/
/*
 * Function: Four edubtm_SplitLeaf(ObjectID*, PageID*, BtreeLeaf*, Two, LeafItem*, InternalItem*)
 *
 * Description: 
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  The function edubtm_SplitLeaf(...) is similar to edubtm_SplitInternal(...) except
 *  that the entry of a leaf differs from the entry of an internal and the first
 *  key value of a new page is used to make an internal item of their parent.
 *  Internal pages do not maintain the linked list, but leaves do it, so links
 *  are properly updated.
 *
 * Returns:
 *  Error code
 *  eDUPLICATEDOBJECTID_BTM
 *    some errors caused by function calls
 *
 * Note:
 *  The caller should call BfM_SetDirty() for 'fpage'.
 */
Four edubtm_SplitLeaf(
    ObjectID                    *catObjForFile, /* IN catalog object of B+ tree file */
    PageID                      *root,          /* IN PageID for the given page, 'fpage' */
    BtreeLeaf                   *fpage,         /* INOUT the page which will be splitted */
    Two                         high,           /* IN slotNo for the given 'item' */
    LeafItem                    *item,          /* IN the item which will be inserted */
    InternalItem                *ritem)         /* OUT the item which will be returned by spliting */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Four                        e;              /* error number */
    Two                         i;              /* slot No. in the given page, fpage */
    Two                         j;              /* slot No. in the splitted pages */
    Two                         k;              /* slot No. in the new page */
    Two                         maxLoop;        /* # of max loops; # of slots in fpage + 1 */
    Four                        sum;            /* the size of a filled area */
    PageID                      newPid;         /* for a New Allocated Page */
    PageID                      nextPid;        /* for maintaining doubly linked list */
    BtreeLeaf                   tpage;          /* a temporary page for the given page */
    BtreeLeaf                   *npage;         /* a page pointer for the new page */
    BtreeLeaf                   *mpage;         /* for doubly linked list */
    btm_LeafEntry               *itemEntry;     /* entry for the given 'item' */
    btm_LeafEntry               *fEntry;        /* an entry in the given page, 'fpage' */
    btm_LeafEntry               *nEntry;        /* an entry in the new page, 'npage' */
    ObjectID                    *iOidArray;     /* ObjectID array of 'itemEntry' */
    ObjectID                    *fOidArray;     /* ObjectID array of 'fEntry' */
    Two                         fEntryOffset;   /* starting offset of 'fEntry' */
    Two                         nEntryOffset;   /* starting offset of 'nEntry' */
    Two                         oidArrayNo;     /* element No in an ObjectID array */
    Two                         alignedKlen;    /* aligned length of the key length */
    Two                         itemEntryLen;   /* length of entry for item */
    Two                         entryLen;       /* entry length */
    Boolean                     flag;
    Boolean                     isTmp;
    /**/

    printf("DBG : Welcome to Splitleaf(root : %d, high : %d, item : \n", *root, high);
    for (int i = 0; i < item->klen; i++){
        printf("%c", *(char *)(item->kval + i));
    }
    printf("\n");

    /* Allocate a new page to be used as a B+ index page */
    if((e = btm_AllocPage(catObjForFile, root, &newPid))<0) ERR(e);
    
    /* Initiate the page as a leaf page */
    if((e = btm_InitLeaf(&newPid, FALSE, FALSE))<0) ERR(e);

    /* Fix the page that has been newly allocated on the disk to the buffer */    
    if((e = BfM_GetNewTrain(&newPid, (char**)&npage, PAGE_BUF))<0) ERR(e);

    /* Fix the root page to the buffer */    
    if ((e = BfM_GetTrain((TrainID*)root, (char**)&fpage, PAGE_BUF)) < 0) ERR(e);

    /* Sort index entries + 1 new entry. Insert the entries to the fpage until page is full 50% */
    /* Copy fpage data[] to tpage data[] until tpage is full 50% */
    high++;
    i = 0;
    j = 0;
    k = 0;
    fEntryOffset = 0;
    nEntryOffset = 0;
    maxLoop = fpage->hdr.nSlots+1;
    flag = FALSE;
    sum = 0;
    /* Copy apage data[] to tpage data[] */
    for(int l = 0; l < fpage->hdr.free; l++){
        tpage.data[l] = fpage->data[l];
    }

    fpage->hdr.nSlots++;
    for(j = maxLoop-1; j > high; j--){                
        fpage->slot[-j] = fpage->slot[-(j-1)];        
    }
    fpage->slot[-high] = -1; 

    printf("DBG : loop A\n");
    for(j = 0; j < maxLoop; j++){

        if (sum > BL_HALF) break;

        if (j == high)
            itemEntry = (btm_LeafEntry *)item;
        else
            itemEntry = (btm_LeafEntry *)&(tpage.data[fpage->slot[-i]]);    

        alignedKlen = ALIGNED_LENGTH(itemEntry->klen);
        entryLen = BTM_LEAFENTRY_FIXED + alignedKlen + OBJECTID_SIZE;

        fEntry = (btm_LeafEntry *)&(fpage->data[fEntryOffset]);            
        fEntry->nObjects = itemEntry->nObjects;
        fEntry->klen = itemEntry->klen;
        for (int l = 0; l < alignedKlen + OBJECTID_SIZE; l++)
            *((char *)(fEntry->kval + l ) = *((char *)(itemEntry->kval + l ));             

        fpage->slot[-i] = fEntryOffset;
        fEntryOffset = fEntryOffset + entryLen;
        sum = sum + entryLen + sizeof(Two);
        i++;
    }
    printf("DBG : high = %d, j = %d, i = %d, nO = %d, klen = %d, kval = \n", high, j, i, fEntry->nObjects, fEntry->klen);
    for (int i = 0; i < fEntry->klen; i++){
        printf("%c", *(char *)(fEntry->kval + i));
    }

    printf("DBG : loop B\n");
    for(j; j < maxLoop; j++){

        npage->slot[k] = fpage->slot[j];

        if (j == high)
            itemEntry = (btm_LeafEntry *)item;
        else
            itemEntry = (btm_LeafEntry *)&(tpage.data[npage->slot[-k]]);    
        
        nEntry = (btm_LeafEntry *)&(npage->data[fEntryOffset]);
        alignedKlen = ALIGNED_LENGTH(itemEntry->klen);
        nEntry->nObjects = itemEntry->nObjects;
        *((KeyValue *)&(nEntry->klen)) = *((KeyValue *)&(itemEntry->klen));        
        for (int l = 0; l < OBJECTID_SIZE; l++)
            *((char *)(nEntry->kval) + alignedKlen + l ) = *((char *)(itemEntry->kval) + alignedKlen + l );             
    
        entryLen = BTM_LEAFENTRY_FIXED + alignedKlen + OBJECTID_SIZE;
        npage->slot[-k] = nEntryOffset;
        nEntryOffset = nEntryOffset + entryLen;
        k++;
    }

    /* Update the headers of fpage, npage */
    fpage->hdr.nSlots = i;
    fpage->hdr.free = fEntryOffset;
    fpage->hdr.unused = 0;

    npage->hdr.nSlots = k;
    npage->hdr.free = nEntryOffset;
    npage->hdr.unused = 0;

    /* Insert the npage to the doubly-linked list of leaf pages */
    npage->hdr.nextPage = fpage->hdr.nextPage;
    npage->hdr.prevPage = root->pageNo;
    fpage->hdr.nextPage = newPid.pageNo;
        
    /* Create internal index entry. (1) Spid = pid of npage (2) discrimator key = npage's 1st index entry key */
    nEntryOffset = npage->slot[0];
    itemEntry = (btm_LeafEntry *)&(npage->data[nEntryOffset]);
    ritem->spid = newPid.pageNo;
    ritem->klen = itemEntry->klen;
    for (i = 0; i < itemEntry->klen; i++){
        *((char *)(ritem->kval) + i) = *((char *)(itemEntry->kval) + i);
    }    

    /* Set the DIRTY bit */
    if((e = BfM_SetDirty((TrainID*)root, PAGE_BUF))<0) ERRB1(e, root, PAGE_BUF);
    if((e = BfM_SetDirty((TrainID*)&newPid, PAGE_BUF))<0) ERRB1(e, &newPid, PAGE_BUF);    
    /* Unfix the pages from the buffer */ 
    if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);
    if((e = BfM_FreeTrain((TrainID*)&newPid, PAGE_BUF))<0) ERR(e);

    /**/
    return(eNOERROR);
    
} /* edubtm_SplitLeaf() */
