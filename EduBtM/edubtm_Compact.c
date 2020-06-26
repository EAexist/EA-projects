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
 * Module: edubtm_Compact.c
 * 
 * Description:
 *  Two functions edubtm_CompactInternalPage() and edubtm_CompactLeafPage() are
 *  used to compact the internal page and the leaf page, respectively.
 *
 * Exports:
 *  void edubtm_CompactInternalPage(BtreeInternal*, Two)
 *  void edubtm_CompactLeafPage(BtreeLeaf*, Two)
 */


#include <string.h>
#include "EduBtM_common.h"
#include "EduBtM_Internal.h"



/*@================================
 * edubtm_CompactInternalPage()
 *================================*/
/*
 * Function: edubtm_CompactInternalPage(BtreeInternal*, Two)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  Reorganize the internal page to make sure the unused bytes in the page
 *  are located contiguously "in the middle", between the entries and the
 *  slot array. To compress out holes, entries must be moved toward the
 *  beginning of the page.
 *
 * Returns:
 *  None
 *
 * Side effects:
 *  The leaf page is reorganized to compact the space.
 */
void edubtm_CompactInternalPage(
    BtreeInternal       *apage,                 /* INOUT internal page to compact */
    Two                 slotNo)                 /* IN slot to go to the boundary of free space */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    BtreeInternal       tpage;                  /* temporay page used to save the given page */
    Two                 apageDataOffset;        /* where the next object is to be moved */
    Two                 len;                    /* length of the leaf entry */
    Two                 i;                      /* index variable */
    btm_InternalEntry   *entry;                 /* an entry in leaf page */
    /**/
    apageDataOffset = 0;
    int j = 0;

    /* Copy apage data[] to tpage data[] */    
    for(int i = 0; i < PAGESIZE - BI_FIXED; i++){
        *((char *)(tpage.data) + i) = *((char *)(apage->data) + i);
    }

    /*Delete the part if the code works*/
    /* Copy object from tpage to apage in order */
    /*
    if (slotNo == NIL){
        for (i = 0; i < apage->hdr.nSlots; i++){
            entry = tpage.data[apage->slot[-i]];
            len = sizeof(entry);
            strncpy(apage->data + apageDataOffset, entry, len);             
            apageDataOffset += len;                 
        }
    }
    */

    for (i = 0; i < apage->hdr.nSlots; i++){
        if(i == slotNo)
            continue;

        entry = (btm_InternalEntry *)((char*)(tpage.data) + tpage.slot[-i]);
        len = sizeof(ShortPageID) + ALIGNED_LENGTH(sizeof(Two) + entry->klen);     
        ((btm_InternalEntry *)((char*)(apage->data) + apageDataOffset))->spid = entry->spid;
        ((btm_InternalEntry *)((char*)(apage->data) + apageDataOffset))->klen = entry->klen;
        for (j = 0; j < entry->klen; j++){
            *((char *)((btm_InternalEntry *)((char*)(apage->data) + apageDataOffset))->kval + j) = *((char *)(entry->kval) + j);
        }
        apage->slot[-i] = apageDataOffset;              
        apageDataOffset = apageDataOffset + len;                 
    }

    if(slotNo != NIL){
        entry = (btm_InternalEntry *)((char*)(tpage.data) + tpage.slot[-slotNo]);
        len = sizeof(ShortPageID) + ALIGNED_LENGTH(sizeof(Two) + entry->klen);     
        ((btm_InternalEntry *)((char*)(apage->data) + apageDataOffset))->spid = entry->spid;
        ((btm_InternalEntry *)((char*)(apage->data) + apageDataOffset))->klen = entry->klen;
        for (j = 0; j < entry->klen; j++){
            *((char *)((btm_InternalEntry *)((char*)(apage->data) + apageDataOffset))->kval + j) = *((char *)(entry->kval) + j);
        }
        apage->slot[-i] = apageDataOffset;              
        apageDataOffset = apageDataOffset + len; 
    }   

    /* Update page header */
    apage->hdr.free = apageDataOffset;
    apage->hdr.unused = 0;
    
    /**/
} /* edubtm_CompactInternalPage() */



/*@================================
 * edubtm_CompactLeafPage()
 *================================*/
/*
 * Function: void edubtm_CompactLeafPage(BtreeLeaf*, Two)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  Reorganizes the leaf page to make sure the unused bytes in the page
 *  are located contiguously "in the middle", between the entries and the
 *  slot array. To compress out holes, entries must be moved toward the
 *  beginning of the page.
 *	
 * Return Values :
 *  None
 *
 * Side Effects :
 *  The leaf page is reorganized to comact the space.
 */
void edubtm_CompactLeafPage(
    BtreeLeaf 		*apage,			/* INOUT leaf page to compact */
    Two       		slotNo)			/* IN slot to go to the boundary of free space */
{	
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    BtreeLeaf 		tpage;			/* temporay page used to save the given page */
    Two                 apageDataOffset;        /* where the next object is to be moved */
    Two                 len;                    /* length of the leaf entry */
    Two                 i;                      /* index variable */
    btm_LeafEntry 	*entry;			/* an entry in leaf page */
    Two 		alignedKlen;		/* aligned length of the key length */
    /**/
    int j = 0;
    apageDataOffset = 0;

    /* Copy apage data[] to tpage data[] */
    for(i = 0; i < PAGESIZE - BL_FIXED; i++){
        *((char *)(tpage.data) + i) = *((char *)(apage->data) + i);
    }

    for (i = 0; i < apage->hdr.nSlots; i++){
        if(i == slotNo)
            continue;

        entry = (btm_LeafEntry *)((char*)(tpage.data) + tpage.slot[-i]);
        alignedKlen = ALIGNED_LENGTH(entry->klen);
        len = BTM_LEAFENTRY_FIXED + alignedKlen + OBJECTID_SIZE;
        ((btm_LeafEntry *)((char*)(apage->data) + apageDataOffset))->nObjects = entry->nObjects;    
        ((btm_LeafEntry *)((char*)(apage->data) + apageDataOffset))->klen = entry->klen;
        for (j = 0; j < alignedKlen + OBJECTID_SIZE; j++){
            *((char *)((btm_LeafEntry *)((char*)(apage->data) + apageDataOffset))->kval + j) = *((char *)(entry->kval) + j);
        }
        apage->slot[-i] = apageDataOffset;              
        apageDataOffset = apageDataOffset + len;                 
    }

    if(slotNo != NIL){
        entry = (btm_LeafEntry *)((char*)(tpage.data) + tpage.slot[-slotNo]);
        alignedKlen = ALIGNED_LENGTH(entry->klen);
        len = BTM_LEAFENTRY_FIXED + alignedKlen + OBJECTID_SIZE;     
        ((btm_LeafEntry *)((char*)(apage->data) + apageDataOffset))->nObjects = entry->nObjects; 
        ((btm_LeafEntry *)((char*)(apage->data) + apageDataOffset))->klen = entry->klen;
        for (j = 0; j < alignedKlen + OBJECTID_SIZE; j++){
            *((char *)((btm_LeafEntry *)((char*)(apage->data) + apageDataOffset))->kval + j) = *((char *)(entry->kval) + j);
        }
        apage->slot[-i] = apageDataOffset;              
        apageDataOffset = apageDataOffset + len; 
    }   

    /* Update page header */
    apage->hdr.free = apageDataOffset;
    apage->hdr.unused = 0;    
    /**/
} /* edubtm_CompactLeafPage() */
