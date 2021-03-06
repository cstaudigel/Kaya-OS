#ifndef TYPES
#define TYPES

#include "const.h"

/**************************************************************************** 
 *
 * This header file contains utility types definitions.
 * 
 ****************************************************************************/


typedef signed int cpu_t;


typedef unsigned int memaddr;


typedef struct {
	unsigned int d_status;
	unsigned int d_command;
	unsigned int d_data0;
	unsigned int d_data1;
} device_t;

#define t_recv_status		d_status
#define t_recv_command		d_command
#define t_transm_status		d_data0
#define t_transm_command	d_data1

#define DEVINTNUM 5
#define DEVPERINT 8
typedef struct {
	unsigned int rambase;
	unsigned int ramsize;
	unsigned int execbase;
	unsigned int execsize;
	unsigned int bootbase;
	unsigned int bootsize;
	unsigned int todhi;
	unsigned int todlo;
	unsigned int intervaltimer;
	unsigned int timescale;
	unsigned int inst_dev[DEVINTNUM];
	unsigned int interrupt_dev[DEVINTNUM];
	device_t   devreg[DEVINTNUM * DEVPERINT];
} devregarea_t;

#define STATEREGNUM	31
typedef struct state_t {
	unsigned int	s_asid;
	unsigned int	s_cause;
	unsigned int	s_status;
	unsigned int 	s_pc;
	int	 	s_reg[STATEREGNUM]; 

} state_t, *state_PTR;

typedef struct pteEntry_t {
	unsigned int	entryHI;
	unsigned int	entryLO;
} pteEntry_t;

typedef struct pte_t{
	int				header;
	pteEntry_t		pteTable[KUSEGSIZE];
} pte_t;

typedef struct pteOS_t{
	int 			header;
	pteEntry_t		pteTable[KSEGSIZE];
} pteOS_t;

typedef struct segTbl_t{
	pteOS_t			*ksegOS;
	pte_t			*kuseg2;
	pte_t			*kuseg3;
} segTbl_t;

#define	s_at	s_reg[0]
#define	s_v0	s_reg[1]
#define s_v1	s_reg[2]
#define s_a0	s_reg[3]
#define s_a1	s_reg[4]
#define s_a2	s_reg[5]
#define s_a3	s_reg[6]
#define s_t0	s_reg[7]
#define s_t1	s_reg[8]
#define s_t2	s_reg[9]
#define s_t3	s_reg[10]
#define s_t4	s_reg[11]
#define s_t5	s_reg[12]
#define s_t6	s_reg[13]
#define s_t7	s_reg[14]
#define s_s0	s_reg[15]
#define s_s1	s_reg[16]
#define s_s2	s_reg[17]
#define s_s3	s_reg[18]
#define s_s4	s_reg[19]
#define s_s5	s_reg[20]
#define s_s6	s_reg[21]
#define s_s7	s_reg[22]
#define s_t8	s_reg[23]
#define s_t9	s_reg[24]
#define s_gp	s_reg[25]
#define s_sp	s_reg[26]
#define s_fp	s_reg[27]
#define s_ra	s_reg[28]
#define s_HI	s_reg[29]
#define s_LO	s_reg[30]

#define SECTIONS	3
#define OLDNEW		2

/* process block type */
typedef struct pcb_t
{
	struct pcb_t 	*pcb_next, 		/* pointer to the next process in the queue */
				 	*pcb_prev,		/* pointer to the previous process in the queue */
				 	*pcb_child,		/* pointer to the first child process of this process */
				 	*pcb_parent,	/* pointer to this process's parent process */
				 	*pcb_prevSib,	/* pointer to the previous sibling */
					*pcb_nextSib;	/* pointer to the next sibling */

	state_t			pcb_state;		/* processor state */
	int				*pcb_semAdd;	/* pointer to semaphore on which process is blocked */

	cpu_t 			pcb_time; 		/* time the process has spent on the CPU in microseconds */

	state_PTR		pcb_states[SECTIONS][OLDNEW]; /* 2D array for states */

} pcb_t, *pcb_PTR;

/* semaphore descriptor type */
typedef struct semd_t 
{
	struct semd_t 	*s_next; 		/* next element on the ASL */
	int				*s_semAdd;		/* pointer to the semaphore */
	pcb_PTR			s_procQ;		/* tail pointer to a process queue */
} semd_t, *semd_PTR;

typedef struct adl_t
{
	struct adl_t	*d_next;		/* next ADL in list */
	int 			d_wakeTime;		/* time to wake up device */
	int				d_asid;			/* id of process */
} adl_t, *adl_PTR;

typedef struct avsl_t
{
	struct avsl_t 	*v_next;
	struct avsl_t	*v_prev;
	int				*v_sem;
	int				v_asid;
} avsl_t, *avsl_PTR;

typedef struct uProc_t
{
	state_t			uProc_states[SECTIONS][OLDNEW]; /* 2D array of states for SYS5 */
	int				uProc_semAdd;		/* private semaphore */
	pte_t			uProc_pte;		/* page table (kUseg2) */
	int				uProc_backStoreAdd;	/* sector # for seg2 drum area */
} uProc_t;

typedef struct swap_t
{
	int				sw_asid;
	int				sw_pgNum;
	int				sw_segNum;
	pteEntry_t		*sw_pte;
}swap_t;

#endif
