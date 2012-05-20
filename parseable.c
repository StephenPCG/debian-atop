/*
** ATOP - System & Process Monitor
**
** The program 'atop' offers the possibility to view the activity of
** the system on system-level as well as process-level.
**
** ==========================================================================
** Author:      Gerlof Langeveld - AT Computing, Nijmegen, Holland
** E-mail:      gerlof@ATComputing.nl
** Date:        February 2007
** --------------------------------------------------------------------------
** Copyright (C) 2007 Gerlof Langeveld
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2, or (at your option) any
** later version.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
** --------------------------------------------------------------------------
**
** $Id: parseable.c,v 1.7 2008/03/06 09:08:29 gerlof Exp $
** $Log: parseable.c,v $
** Revision 1.7  2008/03/06 09:08:29  gerlof
** Bug-solution regarding parseable output of PPID.
**
** Revision 1.6  2008/03/06 08:38:03  gerlof
** Register/show ppid of a process.
**
** Revision 1.5  2008/01/18 08:03:40  gerlof
** Show information about the number of threads in state 'running',
** 'interruptible sleeping' and 'non-interruptible sleeping'.
**
** Revision 1.4  2007/12/11 13:33:12  gerlof
** Cosmetic change.
**
** Revision 1.3  2007/08/16 12:00:11  gerlof
** Add support for atopsar reporting.
** Concerns networking-counters that have been changed.
**
** Revision 1.2  2007/03/20 13:01:12  gerlof
** Introduction of variable supportflags.
**
** Revision 1.1  2007/02/19 11:55:43  gerlof
** Initial revision
**
*/
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

#include "atop.h"
#include "photosyst.h"
#include "photoproc.h"
#include "parseable.h"

void 	print_CPU();
void 	print_cpu();
void 	print_CPL();
void 	print_MEM();
void 	print_SWP();
void 	print_PAG();
void 	print_DSK();
void 	print_NET();

void 	print_PRG();
void 	print_PRC();
void 	print_PRM();
void 	print_PRD();
void 	print_PRN();

/*
** table with possible labels and the corresponding
** print-function for parseable output.
*/
struct labeldef {
	char	*label;
	int	valid;
	void	(*prifunc)(char *, struct sstat *, struct pstat *, int);
};

static struct labeldef	labeldef[] = {
	{ "CPU",	0,	print_CPU },
	{ "cpu",	0,	print_cpu },
	{ "CPL",	0,	print_CPL },
	{ "MEM",	0,	print_MEM },
	{ "SWP",	0,	print_SWP },
	{ "PAG",	0,	print_PAG },
	{ "DSK",	0,	print_DSK },
	{ "NET",	0,	print_NET },

	{ "PRG",	0,	print_PRG },
	{ "PRC",	0,	print_PRC },
	{ "PRM",	0,	print_PRM },
	{ "PRD",	0,	print_PRD },
	{ "PRN",	0,	print_PRN },
};

static int	numlabels = sizeof labeldef/sizeof(struct labeldef);

/*
** analyse the parse-definition string that has been
** passed as argument with the flag -P
*/
int
parsedef(char *pd)
{
	register int	i;
	char		*p, *ep = pd + strlen(pd);

	/*
	** check if string passed bahind -P is not another flag
	*/
	if (*pd == '-')
	{
		fprintf(stderr, "flag -P should be followed by label list\n");
		return 0;
	}

	/*
	** check list of comma-separated labels 
	*/
	while (pd < ep)
	{
		/*
		** exchange comma by null-byte
		*/
		if ( (p = strchr(pd, ',')) )
			*p = 0;	
		else
			p  = ep-1;

		/*
		** check if the next label exists
		*/
		for (i=0; i < numlabels; i++)
		{
			if ( strcmp(labeldef[i].label, pd) == 0)
			{
				labeldef[i].valid = 1;
				break;
			}
		}

		/*
		** non-existing label has been used
		*/
		if (i == numlabels)
		{
			/*
			** check if special label 'ALL' has been used
			*/
			if ( strcmp("ALL", pd) == 0)
			{
				for (i=0; i < numlabels; i++)
					labeldef[i].valid = 1;
				break;
			}
			else
			{
				fprintf(stderr, "label %s not found\n", pd);
				return 0;
			}
		}

		pd = p+1;
	}

	setbuf(stdout, (char *)0);

	return 1;
}

/*
** produce parseable output for an interval
*/
char
parseout(time_t curtime, int numsecs,
	 struct sstat *ss, struct pstat *ps,
	 int nlist, int npresent, int nzombie,
	 int nexit, char flag)
{
	register int	i;
	char		datestr[32], timestr[32], header[256];

	/*
	** search all labels which are selected before
	*/
	for (i=0; i < numlabels; i++)
	{
		if (labeldef[i].valid)
		{
			/*
			** prepare generic columns
			*/
			convdate(curtime, datestr);
			convtime(curtime, timestr);

			snprintf(header, sizeof header, "%s %s %ld %s %s %d",
				labeldef[i].label,
				utsname.nodename,
				curtime,
				datestr, timestr, numsecs);

			/*
			** call a selected print-function
			*/
			(labeldef[i].prifunc)(header, ss, ps, nlist);
		}
	}

	/*
	** print separator
	*/
	printf("SEP\n");

	return '\0';
}

/*
** print functions for system-level statistics
*/
void
print_CPU(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	printf("%s %u %lld %lld %lld %lld %lld %lld %lld %lld %lld\n",
			hp,
			hertz,
	        	ss->cpu.nrcpu,
	        	ss->cpu.all.stime,
        		ss->cpu.all.utime,
        		ss->cpu.all.ntime,
        		ss->cpu.all.itime,
        		ss->cpu.all.wtime,
        		ss->cpu.all.Itime,
        		ss->cpu.all.Stime,
        		ss->cpu.all.steal);
}

void
print_cpu(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	register int i;

	for (i=0; i < ss->cpu.nrcpu; i++)
	{
		printf("%s %u %d %lld %lld %lld %lld %lld %lld %lld %lld\n",
			hp, hertz, i,
	        	ss->cpu.cpu[i].stime,
        		ss->cpu.cpu[i].utime,
        		ss->cpu.cpu[i].ntime,
        		ss->cpu.cpu[i].itime,
        		ss->cpu.cpu[i].wtime,
        		ss->cpu.cpu[i].Itime,
        		ss->cpu.cpu[i].Stime,
        		ss->cpu.cpu[i].steal);
	}
}

void
print_CPL(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	printf("%s %lld %.2f %.2f %.2f %lld %lld\n",
			hp,
	        	ss->cpu.nrcpu,
	        	ss->cpu.lavg1,
	        	ss->cpu.lavg5,
	        	ss->cpu.lavg15,
	        	ss->cpu.csw,
	        	ss->cpu.devint);
}

void
print_MEM(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	printf(	"%s %u %lld %lld %lld %lld %lld\n",
			hp,
			pagesize,
			ss->mem.physmem,
			ss->mem.freemem,
			ss->mem.cachemem,
			ss->mem.buffermem,
			ss->mem.slabmem);
}

void
print_SWP(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	printf(	"%s %u %lld %lld %lld %lld %lld\n",
			hp,
			pagesize,
			ss->mem.totswap,
			ss->mem.freeswap,
			(long long)0,
			ss->mem.committed,
			ss->mem.commitlim);
}

void
print_PAG(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	printf(	"%s %u %lld %lld %lld %lld %lld\n",
			hp,
			pagesize,
			ss->mem.pgscans,
			ss->mem.allocstall,
			(long long)0,
			ss->mem.swins,
			ss->mem.swouts);
}

void
print_DSK(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	register int	i;

        for (i=0; ss->xdsk.xdsk[i].name[0]; i++)
	{
		printf(	"%s %s %lld %lld %lld %lld %lld\n",
			hp,
			ss->xdsk.xdsk[i].name,
			ss->xdsk.xdsk[i].io_ms,
			ss->xdsk.xdsk[i].nread,
			ss->xdsk.xdsk[i].nrsect,
			ss->xdsk.xdsk[i].nwrite,
			ss->xdsk.xdsk[i].nwsect);
	}
}

void
print_NET(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	register int 	i;

	printf(	"%s %s %lld %lld %lld %lld %lld %lld %lld %lld\n",
			hp,
			"upper",
        		ss->net.tcp.InSegs,
   		     	ss->net.tcp.OutSegs,
       		 	ss->net.udpv4.InDatagrams +
				ss->net.udpv6.Udp6InDatagrams,
       		 	ss->net.udpv4.OutDatagrams +
				ss->net.udpv6.Udp6OutDatagrams,
       		 	ss->net.ipv4.InReceives  +
				ss->net.ipv6.Ip6InReceives,
       		 	ss->net.ipv4.OutRequests +
				ss->net.ipv6.Ip6OutRequests,
       		 	ss->net.ipv4.InDelivers +
       		 		ss->net.ipv6.Ip6InDelivers,
       		 	ss->net.ipv4.ForwDatagrams +
       		 		ss->net.ipv6.Ip6OutForwDatagrams);

	for (i=0; ss->intf.intf[i].name[0]; i++)
	{
		printf(	"%s %s %lld %lld %lld %lld %ld %d\n",
			hp,
			ss->intf.intf[i].name,
			ss->intf.intf[i].rpack,
			ss->intf.intf[i].rbyte,
			ss->intf.intf[i].spack,
			ss->intf.intf[i].sbyte,
			ss->intf.intf[i].speed,
			ss->intf.intf[i].duplex);
	}
}

/*
** print functions for process-level statistics
*/
void
print_PRG(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	register int i;

	for (i=0; i < nact; i++, ps++)
	{
		printf("%s %d (%s) %c %d %d %d %d %d %ld (%s) %d %d %d %d\n",
				hp,
				ps->gen.pid,
				ps->gen.name,
				ps->gen.state,
				ps->gen.ruid,
				ps->gen.rgid,
				ps->gen.pid,	/* was TGID formerly */
				ps->gen.nthr,
				ps->gen.excode,
				ps->gen.btime,
				ps->gen.cmdline,
				ps->gen.ppid,
				ps->gen.nthrrun,
				ps->gen.nthrslpi,
				ps->gen.nthrslpu);
	}
}

void
print_PRC(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	register int i;

	for (i=0; i < nact; i++, ps++)
	{
		printf("%s %d (%s) %c %u %lld %lld %d %d %d %d %d %d\n",
				hp,
				ps->gen.pid,
				ps->gen.name,
				ps->gen.state,
				hertz,
				ps->cpu.utime,
				ps->cpu.stime,
				ps->cpu.nice,
				ps->cpu.prio,
				ps->cpu.rtprio,
				ps->cpu.policy,
				ps->cpu.curcpu,
				ps->cpu.sleepavg);
	}
}

void
print_PRM(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	register int i;

	for (i=0; i < nact; i++, ps++)
	{
		printf("%s %d (%s) %c %u %lld %lld %lld %lld %lld %lld %lld\n",
				hp,
				ps->gen.pid,
				ps->gen.name,
				ps->gen.state,
				pagesize,
				ps->mem.vmem,
				ps->mem.rmem,
				ps->mem.shtext,
				ps->mem.vgrow,
				ps->mem.rgrow,
				ps->mem.minflt,
				ps->mem.majflt);
	}
}

void
print_PRD(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	register int i;

	for (i=0; i < nact; i++, ps++)
	{
		printf("%s %d (%s) %c %c %c %lld %lld %lld %lld %lld\n",
				hp,
				ps->gen.pid,
				ps->gen.name,
				ps->gen.state,
				supportflags & PATCHSTAT ? 'y' : 'n',
				supportflags & IOSTAT ? 'y' : 'n',
				ps->dsk.rio, ps->dsk.rsz,
				ps->dsk.wio, ps->dsk.wsz, ps->dsk.cwsz);
	}
}

void
print_PRN(char *hp, struct sstat *ss, struct pstat *ps, int nact)
{
	register int i;

	for (i=0; i < nact; i++, ps++)
	{
		printf("%s %d (%s) %c %c %lld %lld %lld %lld %lld %lld "
		       "%lld %lld %lld %lld\n",
				hp,
				ps->gen.pid,
				ps->gen.name,
				ps->gen.state,
				supportflags & PATCHSTAT ? 'y' : 'n',
				ps->net.tcpsnd,
				ps->net.tcpssz,
				ps->net.tcprcv,
				ps->net.tcprsz,
				ps->net.udpsnd,
				ps->net.udpssz,
				ps->net.udprcv,
				ps->net.udprsz,
				ps->net.rawsnd,
				ps->net.rawrcv);
	}
}
