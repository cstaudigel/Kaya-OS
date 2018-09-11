#include "../h/types.h"
#include "../h/const.h"

#include "../e/pcb.e"
#include "../e/asl.e"


semd_PTR semdFree_h, /* Head of free list */
         semd_h;     /* Head of ASL */

void debugA(int a)
{
    int i;
    i = 1;
}

HIDDEN semd_PTR searchASL(int *semAdd, semd_PTR s);

HIDDEN semd_PTR allocSemd(int *semAdd)
{
    semd_PTR returnMe;
    if(semdFree_h == NULL)
    {
        returnMe = NULL;
    }
    else
    {
        returnMe = semdFree_h;
        semdFree_h = semdFree_h -> s_next;

        returnMe -> s_next = NULL;
        returnMe -> s_procQ = mkEmptyProcQ();
        returnMe -> s_semAdd = semAdd;

        if (semd_h == NULL)
        {
            semd_h = returnMe;
        }
        else
        {
            if (semAdd == (int*) MAXINT)
            {
                semd_PTR max;
                max -> s_semAdd = semAdd;

                returnMe -> s_next = max -> s_next;
                max -> s_next = returnMe;
            }
            else
            {
                semd_PTR prev;
                prev = searchASL(semAdd, semd_h);

                returnMe -> s_next = prev -> s_next;
                prev -> s_next = returnMe;
            }
        }
    }
    return returnMe;
}

HIDDEN void freeSemd(semd_PTR s) 
{
    s -> s_next = semdFree_h;
    semdFree_h = s;
}

/*
 * searches the ASL for the provided node and returns the 
 * node PREVIOUS to the found (or not) node
 * ex: 10->20->30
 * searching for 30 returns 20
 * searching for 25 returns 20
 */
HIDDEN semd_PTR searchASL(int *semAdd, semd_PTR s)
{
    if( s -> s_next-> s_semAdd < semAdd)
    {
        addokbuf("0");
        return s -> s_next;
    }
    else if(semAdd == NULL)
    {
        addokbuf("set to max");
        semAdd = (int*) MAXINT;
        return searchASL(semAdd, s);
    }
    else
    {
        addokbuf("2");
       return searchASL(semAdd, s->s_next);
    }
}

/*
 * searches semdlist for semAdd
 * 2 cases
 *   Found -> insertProcQ(semAdd->tp, p)
 *   Not Found -> 
 *        new semd_t with empty tp
 *        add new semd_t into ASL (Sorted by s_semAdd)
 *        same process as found
 */
int insertBlocked (int *semAdd, pcb_PTR p)
{
    semd_PTR q;
    q = searchASL(semAdd, semd_h);
    pcb_PTR lol;
    lol = q -> s_next -> s_procQ;
    addokbuf("3");
    if(q -> s_next -> s_semAdd == semAdd)
    {
        addokbuf("4");
        p -> pcb_semAdd = semAdd;
        insertProcQ(lol, p);

    }
    else
    {
        addokbuf("5");
        if(semdFree_h != NULL)
        {
            addokbuf("6");
            semd_PTR temp;
            semd_PTR new;
            temp = q -> s_next;
            new = allocSemd(semAdd);
            addokbuf("g");
            q -> s_next = new;
            addokbuf("8");
            new -> s_next = temp;
            addokbuf("a");
            new -> s_procQ = mkEmptyProcQ();
            addokbuf("b");
            new -> s_semAdd = semAdd;
            addokbuf("c");
            insertProcQ(&new -> s_procQ, p);
            addokbuf("9");
        }
        else
        {
            addokbuf("7");
            return 1;
        }
    }
    return 0;
}

/*
 * search ASL for given semAdd
 * 2 cases:
 *   not found: error case (return null)?
 *   found: removeProcQ on s_procQ returns pcb_PTR
 *     2 cases:
 *          processQueue is not empty -> done
 *          processQueue is Empty ->
 *              deallocate semd node
 */
pcb_PTR removeBlocked (int *semAdd)
{
    semd_PTR q;
    q = searchASL(semAdd, semd_h);
    if((q -> s_next -> s_semAdd != semAdd)||(q -> s_next -> s_semAdd == (int*) MAXINT))
    {
        return NULL;
    }
    else
    {
        q = q -> s_next;
        pcb_PTR p;
        p = removeProcQ((pcb_PTR*)q->s_procQ);
        if(p == NULL)
        {
            freeSemd(q);
            return NULL;
        }
        else
        {
            return p;
        }
    }

}

/*
 * same as removeBlocked except call outProcQ instead of removeProcQ
 */
pcb_PTR outBlocked (pcb_PTR p)
{
    semd_PTR q;
    pcb_PTR returnMe;
    int *semAdd;
    semAdd = p -> pcb_semAdd;

    q = searchASL(semAdd, semd_h);
    if((q -> s_next -> s_semAdd != semAdd)||(q -> s_next -> s_semAdd == (int*)MAXINT))
    {
        returnMe = NULL;
    }
    else
    {
        if(p ==  NULL)
        {
            returnMe = NULL;
        } 
        else
        {
            returnMe = outProcQ((pcb_PTR*)q -> s_next -> s_procQ, p);
        }   
    }
    return returnMe;
}

/*
 * same as remove/out except call headProcQ and do not deallocate
 */
pcb_PTR headBlocked (int *semAdd)
{
    semd_PTR q;
    q = searchASL(semAdd, semd_h);
    pcb_PTR returnMe;
    if((q -> s_next -> s_semAdd != semAdd)||(q -> s_next -> s_semAdd == (int*)MAXINT))
    {
        returnMe = NULL;
    }
    else
    {
        returnMe = headProcQ(q -> s_next -> s_procQ);
    }
    return returnMe;
}

/*
 * create ASL of length MAXPROC
 */
void initASL()
{
    int i;
    HIDDEN semd_t semdTable[MAXPROC + 2];
    for(i=0;i<MAXPROC;i++)
    {
        
        freeSemd(&semdTable[i]);
    }

    allocPcb((int*)0);
    allocPcb((int*)MAXINT);
}
