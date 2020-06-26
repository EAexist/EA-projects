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
 * Module: edubtm_BinarySearch.c
 *
 * Description :
 *  This file has three function about searching. All these functions use the
 *  binary search algorithm. If the entry is found successfully, it returns 
 *  TRUE and the correct position as an index, otherwise, it returns FALSE and
 *  the index whose key value is the largest in the given page but less than
 *  the given key value in the function edubtm_BinarSearchInternal; in the
 *  function edubtm_BinarySearchLeaf() the index whose key value is the smallest
 *  in the given page but larger than the given key value.
 *
 * Exports:
 *  Boolean edubtm_BinarySearchInternal(BtreeInternal*, KeyDesc*, KeyValue*, Two*)
 *  Boolean edubtm_BinarySearchLeaf(BtreeLeaf*, KeyDesc*, KeyValue*, Two*)
 */


#include "EduBtM_common.h"
#include "EduBtM_Internal.h"



/*@================================
 * edubtm_BinarySearchInternal()
 *================================*/
/*
 * Function:  Boolean edubtm_BinarySearchInternal(BtreeInternal*, KeyDesc*,
 *                                             KeyValue*, Two*)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  Search the internal entry of which value equals to or less than the given
 *  key value.
 *
 * Returns:
 *  Result of search: TRUE if the same key is found, otherwise FALSE
 *
 * Side effects:
 *  1) parameter idx : slot No of the slot having the key equal to or
 *                     less than the given key value
 *                     
 */
Boolean edubtm_BinarySearchInternal(
    BtreeInternal 	*ipage,		/* IN Page Pointer to an internal page */
    KeyDesc       	*kdesc,		/* IN key descriptor */
    KeyValue      	*kval,		/* IN key value */
    Two          	*idx)		/* OUT index to be returned */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Two  		low;		/* low index */
    Two  		mid;		/* mid index */
    Two  		high;		/* high index */
    Four 		cmp;		/* result of comparison */
    btm_InternalEntry 	*entry;	/* an internal entry */

    
    /* Error check whether using not supported functionality by EduBtM */
    int i;
    for(i=0; i<kdesc->nparts; i++)
    {
        if(kdesc->kpart[i].type!=SM_INT && kdesc->kpart[i].type!=SM_VARSTRING)
            ERR(eNOTSUPPORTED_EDUBTM);
    }
    /**/
    *idx = -1;
    low = 0;
    high = ipage->hdr.nSlots;
    if( high == 0 ) return FALSE;
    mid = (high+low)/2; 

    do{        
        entry = (btm_InternalEntry *)((char*)(ipage->data) + ipage->slot[-mid]);
//        printf("\nkey1 : %d, CompareKey : %d\n", *(Four *)&(kval->val), *(Four *)&(entry->kval));        
        cmp = btm_KeyCompare(kdesc, kval, (KeyValue *)(&(entry->klen)));

        if(cmp == EQUAL){
            *idx = mid;
            return TRUE;
        }

        else if (cmp == GREATER) low = mid;        
        else if (cmp == LESS) high = mid;
        mid = (high+low)/2;      

    } while(low < mid);

    if (cmp == GREATER) *idx = mid;

    else{
        entry = (btm_InternalEntry *)((char*)(ipage->data) + ipage->slot[-mid]);
        cmp = btm_KeyCompare(kdesc, kval, (KeyValue *)(&(entry->klen)));
        if (cmp == GREATER)
            *idx = mid;
        else if (cmp == LESS)
            *idx = -1;
    }
/*
    printf("DBG : key = ");
    for (int i = 0; i < kval->len; i++){
        printf("%c", *(char *)(kval->val + i));
    }
    printf("\n");
    if (cmp == GREATER){
        printf(", cmp = ");
            for (int i = 0; i < ((KeyValue *)(&(entry->klen)))->len; i++){
                printf("%c", *(char *)(((KeyValue *)(&(entry->klen)))->val + i));
            }
        printf("\n");

        entry = (btm_InternalEntry *)((char*)(ipage->data) + ipage->slot[-(mid+1)]);
        printf(", cmp = ");
            for (int i = 0; i < ((KeyValue *)(&(entry->klen)))->len; i++){
                printf("%c", *(char *)(((KeyValue *)(&(entry->klen)))->val + i));
            }
        printf("\n");

    }
    
    if (cmp == LESS)
        printf(" is the smllest\n");*/
    //printf("low : %d, mid : %d, high : %d, idx : %d, cmp : %d\n",low,mid,high,*idx,cmp);

    return FALSE;
    /**/    
} /* edubtm_BinarySearchInternal() */



/*@================================
 * edubtm_BinarySearchLeaf()
 *================================*/
/*
 * Function: Boolean edubtm_BinarySearchLeaf(BtreeLeaf*, KeyDesc*,
 *                                        KeyValue*, Two*)
 *
 * Description:
 * (Following description is for original ODYSSEUS/COSMOS BtM.
 *  For ODYSSEUS/EduCOSMOS EduBtM, refer to the EduBtM project manual.)
 *
 *  Search the leaf item of which value equals to or less than the given
 *  key value.
 *
 * Returns:
 *  Result of search: TRUE if the same key is found, FALSE otherwise
 *
 * Side effects:
 *  1) parameter idx: slot No of the slot having the key equal to or
 *                    less than the given key value
 */
Boolean edubtm_BinarySearchLeaf(
    BtreeLeaf 		*lpage,		/* IN Page Pointer to a leaf page */
    KeyDesc   		*kdesc,		/* IN key descriptor */
    KeyValue  		*kval,		/* IN key value */
    Two       		*idx)		/* OUT index to be returned */
{
	/* These local variables are used in the solution code. However, you don��t have to use all these variables in your code, and you may also declare and use additional local variables if needed. */
    Two  		low;		/* low index */
    Two  		mid;		/* mid index */
    Two  		high;		/* high index */
    Four 		cmp;		/* result of comparison */
    btm_LeafEntry 	*entry;		/* a leaf entry */

    /* Error check whether using not supported functionality by EduBtM */
    int i;
    for(i=0; i<kdesc->nparts; i++)
    {
        if(kdesc->kpart[i].type!=SM_INT && kdesc->kpart[i].type!=SM_VARSTRING)
            ERR(eNOTSUPPORTED_EDUBTM);
    }
    /**/
    /*  Integrity Constraints

    mid = (high+low)/2 (=low when high=low+1)
    low <= mid < high
    key(low) < key < key(high)
    if( key(mid) < key ) low = mid,
    if( key(mid) > key ) high = mid 
    key(mid) <= key < key(high)
    if high = low + 1 = mid + 1, break & return high
    */

    *idx = -1;
    low = 0;
    high = lpage->hdr.nSlots;
    if( high == 0 ) return FALSE;
    mid = (high+low)/2; 

    do{        
        entry = (btm_LeafEntry *)(&(lpage->data[lpage->slot[-mid]]));
//        printf("\nkey1 : %d, CompareKey : %d\n", *(Four *)&(kval->val), *(Four *)&(entry->kval));        
        cmp = btm_KeyCompare(kdesc,kval,(KeyValue *)(&(entry->klen)));

        if(cmp == EQUAL){
            *idx = mid;
            return TRUE;
        }

        else if (cmp == GREATER) low = mid;        
        else if (cmp == LESS) high = mid;
        mid = (high+low)/2;      

    }while(low < mid);

    if (cmp == GREATER) *idx = mid;

    else{
        entry = (btm_LeafEntry *)((char*)(lpage->data) + lpage->slot[-mid]);
        cmp = btm_KeyCompare(kdesc, kval, (KeyValue *)(&(entry->klen)));
        if (cmp == GREATER)
            *idx = mid;
        else if (cmp == LESS)
            *idx = -1;
    }

//    printf("low : %d, mid : %d, high : %d, idx : %d, cmp : %d\n",low,mid,high,*idx,cmp);

    return FALSE;
    /**/    
} /* edubtm_BinarySearchLeaf() */
