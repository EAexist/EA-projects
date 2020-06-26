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
 * Module: edubtm_Insert.c
 *
 * Description : 
 *  This function edubtm_Insert(...) recursively calls itself until the type
 *  of a root page becomes LEAF.  If the given root page is an internal,
 *  it recursively calls itself using a proper child.  If the result of
 *  the call occur spliting, merging, or redistributing the children, it
 *  may insert, delete, or replace its own internal item, and if the given
 *  root page may be merged, splitted, or redistributed, it affects the
 *  return values.
 *
 * Exports:
 *  Four edubtm_Insert(ObjectID*, PageID*, KeyDesc*, KeyValue*, ObjectID*,
 *                  Boolean*, Boolean*, InternalItem*, Pool*, DeallocListElem*)
 *  Four edubtm_InsertLeaf(ObjectID*, PageID*, BtreeLeaf*, KeyDesc*, KeyValue*,
 *                      ObjectID*, Boolean*, Boolean*, InternalItem*)
 *  Four edubtm_InsertInternal(ObjectID*, BtreeInternal*, InternalItem*,
 *                          Two, Boolean*, InternalItem*)
 */


#include <string.h>
#include "EduBtM_common.h"
#include "BfM.h"
#include "OM_Internal.h"	/* for SlottedPage containing catalog object */
#include "EduBtM_Internal.h"



/*@================================
 * edubtm_Insert()
 *================================*/
/*
 * Function: Four edubtm_Insert(ObjectID*, PageID*, KeyDesc*, KeyValue*,
 *                           ObjectID*, Boolean*, Boolean*, InternalItem*,
 *                           Pool*, DeallocListElem*)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  If the given root is a leaf page, it should get the correct entry in the
 *  leaf. If the entry is already in the leaf, it simply insert it into the
 *  entry and increment the number of ObjectIDs.  If it is not in the leaf it
 *  makes a new entry and insert it into the leaf.
 *  If there is not enough spage in the leaf, the page should be splitted.  The
 *  overflow page may be used or created by this routine. It is created when
 *  the size of the entry is greater than a third of a page.
 * 
 *  'h' is TRUE if the given root page is splitted and the entry item will be
 *  inserted into the parent page.  'f' is TRUE if the given page is not half
 *  full because of creating a new overflow page.
 *
 * Returns:
 *  Error code
 *    eBADBTREEPAGE_BTM
 *    some errors caused by function calls
 */
Four edubtm_Insert(
    ObjectID                    *catObjForFile,         /* IN catalog object of B+-tree file */
    PageID                      *root,                  /* IN the root of a Btree */
    KeyDesc                     *kdesc,                 /* IN Btree key descriptor */
    KeyValue                    *kval,                  /* IN key value */
    ObjectID                    *oid,                   /* IN ObjectID which will be inserted */
    Boolean                     *f,                     /* OUT whether it is merged by creating a new overflow page */
    Boolean                     *h,                     /* OUT whether it is splitted */
    InternalItem                *item,                  /* OUT Internal Item which will be inserted */
                                                        /*     into its parent when 'h' is TRUE */
    Pool                        *dlPool,                /* INOUT pool of dealloc list */
    DeallocListElem             *dlHead)                /* INOUT head of the dealloc list */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Four                        e;                      /* error number */
    Boolean                     lh;                     /* local 'h' */
    Boolean                     lf;                     /* local 'f' */
    Two                         idx;                    /* index for the given key value */
    PageID                      newPid;                 /* a new PageID */
    KeyValue                    tKey;                   /* a temporary key */
    InternalItem                litem;                  /* a local internal item */
    BtreePage                   *apage;                 /* a pointer to the root page */
    btm_InternalEntry           *iEntry;                /* an internal entry */
    Two                         iEntryOffset;           /* starting offset of an internal entry */
    SlottedPage                 *catPage;               /* buffer page containing the catalog object */
    sm_CatOverlayForBtree       *catEntry;              /* pointer to Btree file catalog information */
    PhysicalFileID              pFid;                   /* B+-tree file's FileID */

    /* Error check whether using not supported functionality by EduBtM */
    int i;
    for(i=0; i<kdesc->nparts; i++)
    {
        if(kdesc->kpart[i].type!=SM_INT && kdesc->kpart[i].type!=SM_VARSTRING)
            ERR(eNOTSUPPORTED_EDUBTM);
    }
    /**/    
    /*@ Initially the flags are FALSE */
    *h = *f = FALSE;

    if ((e = BfM_GetTrain((TrainID*)catObjForFile, (char**)&catPage, PAGE_BUF)) < 0) ERR(e);
    GET_PTR_TO_CATENTRY_FOR_BTREE(catObjForFile, catPage, catEntry);
    MAKE_PHYSICALFILEID(pFid, catEntry->fid.volNo, catEntry->firstPage);

    /* Fix the root page to the buffer */    
    if ((e = BfM_GetTrain((TrainID*)root, (char**)&apage, PAGE_BUF)) < 0) ERR(e);

    //printf("DBG : type = %d ", (apage->any.hdr.type)/*|(apage->bl.hdr.type)*/);
    if (apage->any.hdr.type & LEAF){
        /* Insert <key, oid> pair into the page, return the split information */
        if ((e = edubtm_InsertLeaf(catObjForFile, root, (BtreeLeaf *)&(apage->bl), kdesc, kval, oid, f, h, item)) < 0) ERR(e);
        if (*h) printf("Leaf Split\n");
    }

    else if (apage->any.hdr.type & INTERNAL){

        /* Search the internal entry with the pid of proper child page */
        edubtm_BinarySearchInternal(&(apage->bi), kdesc, kval, &idx);

        if (idx == -1){
            MAKE_PAGEID(newPid, pFid.volNo, (PageNo)(apage->bi.hdr.p0));
        } 
            
        else{
            iEntryOffset = apage->bi.slot[-idx];        
            iEntry = (btm_InternalEntry *)((char*)(apage->bi.data) + iEntryOffset);
            MAKE_PAGEID(newPid, pFid.volNo, (PageNo)(iEntry->spid));
        }
        
        /* Recursive Call of edubtm_Insert */
        printf("DBG : newPid = %d\n", newPid);    
        if ((e = edubtm_Insert(catObjForFile, &newPid, kdesc, kval, oid, &lf, &lh, &litem, dlPool, dlHead)) < 0) ERR(e);

        if (lh){
            //printf("DBG : split case ");

            /* Search the internal entry next to which the index entry for new page will be inserted */
            if (edubtm_BinarySearchInternal((BtreeInternal *)&(apage->bi), kdesc, (KeyValue *)&(litem.klen), &idx) == TRUE) ERR(e);
            printf("DBG : kval = ");
            for (int i = 0; i < litem.klen; i++){
                printf("%c", *(char *)(litem.kval + i));
            }
            printf("\n");
            printf("DBG : idx = %d\n", idx);

            /* Insert the index entry for new page, return the split information */
            if ((e = edubtm_InsertInternal(catObjForFile, (BtreeInternal *)&(apage->bi), &litem, idx, h, item)) < 0) ERR(e);
            if (*h) printf("Internal Split\n");
            
        
        }
    }

    /* Set the DIRTY bit */
    if((e = BfM_SetDirty((TrainID*)root, PAGE_BUF))<0) ERRB1(e, root, PAGE_BUF);
    
    /* Unfix the root page from the buffer */ 
    if((e = BfM_FreeTrain((TrainID*)root, PAGE_BUF))<0) ERR(e);

    /* Unfix the root page from the buffer */ 
    if((e = BfM_FreeTrain((TrainID*)catObjForFile, PAGE_BUF))<0) ERR(e);
    
    /**/    
    return(eNOERROR);    
}   /* edubtm_Insert() */



/*@================================
 * edubtm_InsertLeaf()
 *================================*/
/*
 * Function: Four edubtm_InsertLeaf(ObjectID*, PageID*, BtreeLeaf*, KeyDesc*,
 *                               KeyValue*, ObjectID*, Boolean*, Boolean*,
 *                               InternalItem*)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  Insert into the given leaf page an ObjectID with the given key.
 *
 * Returns:
 *  Error code
 *    eDUPLICATEDKEY_BTM
 *    eDUPLICATEDOBJECTID_BTM
 *    some errors causd by function calls
 *
 * Side effects:
 *  1) f : TRUE if the leaf page is underflowed by creating an overflow page
 *  2) h : TRUE if the leaf page is splitted by inserting the given ObjectID
 *  3) item : item to be inserted into the parent
 */
Four edubtm_InsertLeaf(
    ObjectID                    *catObjForFile, /* IN catalog object of B+-tree file */
    PageID                      *pid,           /* IN PageID of Leaf Page */
    BtreeLeaf                   *page,          /* INOUT pointer to buffer page of Leaf page */
    KeyDesc                     *kdesc,         /* IN Btree key descriptor */
    KeyValue                    *kval,          /* IN key value */
    ObjectID                    *oid,           /* IN ObjectID which will be inserted */
    Boolean                     *f,             /* OUT whether it is merged by creating */
                                                /*     a new overflow page */
    Boolean                     *h,             /* OUT whether it is splitted */
    InternalItem                *item)          /* OUT Internal Item which will be inserted */
                                                /*     into its parent when 'h' is TRUE */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Four                        e;              /* error number */
    Two                         i;
    Two                         idx;            /* index for the given key value */
    LeafItem                    leaf;           /* a Leaf Item */
    Boolean                     found;          /* search result */
    btm_LeafEntry               *entry;         /* an entry in a leaf page */
    Two                         entryOffset;    /* start position of an entry */
    Two                         alignedKlen;    /* aligned length of the key length */
    PageID                      ovPid;          /* PageID of an overflow page */
    Two                         entryLen;       /* length of an entry */
    ObjectID                    *oidArray;      /* an array of ObjectIDs */
    Two                         oidArrayElemNo; /* an index for the ObjectID array */

    /* Error check whether using not supported functionality by EduBtM */
    for(i=0; i<kdesc->nparts; i++){
        if(kdesc->kpart[i].type!=SM_INT && kdesc->kpart[i].type!=SM_VARSTRING)
            ERR(eNOTSUPPORTED_EDUBTM);
    }
    /*@ Initially the flags are FALSE */
    *h = *f = FALSE;
    
    /**/
    oidArrayElemNo = 1;
    alignedKlen = ALIGNED_LENGTH(kval->len);
    entryLen = BTM_LEAFENTRY_FIXED + alignedKlen + OBJECTID_SIZE; 

    /* Search the index entry next to which the new index entry will be inserted, return ERR if the key already exists */
    //printf("%d", *(Four *)(&(kval->val)));
    if (edubtm_BinarySearchLeaf(page, kdesc, kval, &idx) == TRUE) ERR(eDUPLICATEDKEY_BTM);
 
    if(entryLen <= BL_FREE(page)){

        /* Compact the page if needed */
        if(entryLen > BL_CFREE(page)){
            edubtm_CompactLeafPage(page, NIL);
            //if ((e = edubtm_CompactLeafPage(page, NIL)) < 0) ERR(e);
        }

        /* Update data field and slot field */
        entryOffset = page->hdr.free;
        entry = (btm_LeafEntry *)((char*)(page->data) + entryOffset);
        entry->nObjects = oidArrayElemNo;
        entry->klen = kval->len;
        for (i = 0; i < kval->len; i++){
            *((char *)(entry->kval) + i) = *((char *)(kval->val) + i);
        }

        for (i = 0; i < OBJECTID_SIZE; i++){
            *((char *)(entry->kval) + alignedKlen + i) = *((char *)oid + i);
        } 

        idx++;// idx = index of the insertion point
        
        for(i = page->hdr.nSlots; i > idx; i--){
            page->slot[-i] = page->slot[-(i-1)];
            // printf("%d ", i);
        }
        page->slot[-idx] = entryOffset;

        /* Update page header */
        page->hdr.nSlots++;
        page->hdr.free = page->hdr.free + entryLen;  
    }

    else if(entryLen > BL_FREE(page)){        
        leaf.oid = *oid;        
        leaf.nObjects = oidArrayElemNo;
        leaf.klen = kval->len;
        for (i = 0; i < kval->len; i++){
            *((char *)(leaf.kval) + i) = *((char *)(kval->val) + i);
        }
        //printf("DBG : item : %d, %d\n", leaf.klen, (&leaf.kval));
        if ((e = edubtm_SplitLeaf(catObjForFile, pid, page, idx, &leaf, item)) < 0) ERR(e);
        *h = TRUE;
    }    

    /**/
    return(eNOERROR);
    
} /* edubtm_InsertLeaf() */



/*@================================
 * edubtm_InsertInternal()
 *================================*/
/*
 * Function: Four edubtm_InsertInternal(ObjectID*, BtreeInternal*, InternalItem*, Two, Boolean*, InternalItem*)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  This routine insert the given internal item into the given page. If there
 *  is not enough space in the page, it should split the page and the new
 *  internal item should be returned for inserting into the parent.
 *
 * Returns:
 *  Error code
 *    some errors caused by function calls
 *
 * Side effects:
 *  h:	TRUE if the page is splitted
 *  ritem: an internal item which will be inserted into parent
 *          if spliting occurs.
 */
Four edubtm_InsertInternal(
    ObjectID            *catObjForFile, /* IN catalog object of B+-tree file */
    BtreeInternal       *page,          /* INOUT Page Pointer */
    InternalItem        *item,          /* IN Iternal item which is inserted */
    Two                 high,           /* IN index in the given page */
    Boolean             *h,             /* OUT whether the given page is splitted */
    InternalItem        *ritem)         /* OUT if the given page is splitted, the internal item may be returned by 'ritem'. */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Four                e;              /* error number */
    Two                 i;              /* index */
    Two                 entryOffset;    /* starting offset of an internal entry */
    Two                 entryLen;       /* length of the new entry */
    btm_InternalEntry   *entry;         /* an internal entry of an internal page */

    /*@ Initially the flag are FALSE */
    *h = FALSE;

    /**/
    entryLen = sizeof(ShortPageID) + ALIGNED_LENGTH(sizeof(Two) + entry->klen);    

    if(entryLen <= BI_FREE(page)){

        /* Compact the page if needed */
        if(entryLen > BI_CFREE(page)){
            edubtm_CompactInternalPage(page, NIL);
            //if ((e = edubtm_CompactInternalPage(page, NIL)) < 0) ERR(e);
        }

        /* Update data field and slot field */        
        entryOffset = page->hdr.free;
        entry = (btm_InternalEntry *)((char*)(page->data) + entryOffset);
        entry->spid = item->spid;
        entry->klen = item->klen;
        for (i = 0; i < item->klen; i++){
            *((char *)(entry->kval) + i) = *((char *)(item->kval) + i);
        }        

        //printf("%d : %d : %d\n", entry->spid, entry->klen, entry->kval);

        high++; // high = index of the insertion point 

        for(i = page->hdr.nSlots; i > high; i--){
            page->slot[-i] = page->slot[-(i-1)];
        }
        page->slot[-high] = entryOffset;

        /* Update page header */
        page->hdr.nSlots++;
        page->hdr.free = page->hdr.free + entryLen;   
    }

    else if(entryLen > BI_FREE(page)){
        if ((e = edubtm_SplitInternal(catObjForFile, page, high, item, ritem)) < 0) ERR(e);
        *h = TRUE;
    }
    
    /**/
    return(eNOERROR);
    
} /* edubtm_InsertInternal() */

