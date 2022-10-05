//###############################################################################
//#										#
//# UPROG2 universal programmer							#
//#										#
//# copyright (c) 2012-2022 Joerg Wolfram (joerg@jcwolfram.de)			#
//#										#
//#										#
//# This program is free software; you can redistribute it and/or		#
//# modify it under the terms of the GNU General Public License			#
//# as published by the Free Software Foundation; either version 3		#
//# of the License, or (at your option) any later version.			#
//#										#
//# This program is distributed in the hope that it will be useful,		#
//# but WITHOUT ANY WARRANTY; without even the implied warranty of		#
//# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU		#
//# General Public License for more details.					#
//#										#
//# You should have received a copy of the GNU General Public			#
//# License along with this library// if not, write to the			#
//# Free Software Foundation, Inc., 59 Temple Place - Suite 330,		#
//# Boston, MA 02111-1307, USA.							#
//#										#
//###############################################################################

#include <main.h>
#include "exec/stm32f2xx/exec_stm32f2xx.h"

void print_stm32f2_error(int errc)
{
	printf("\n");
	switch(errc)
	{
		case 0:		set_error("OK",errc);
				break;

		case 0x41:	set_error("(timeout: no ACK)",errc);
				break;

		case 0x42:	set_error("(timeout: prog/erase)",errc);
				break;

		case 0x50:	set_error("(wrong ID)",errc);
				break;

		case 0x60:	set_error("(Write Protection Error)",errc);
				break;

		case 0x61:	set_error("(Erase timeout error)",errc);
				break;



		default:	set_error("(unexpected error)",errc);
	}
	print_error();
}

/*


int stm32f2_erase(void)
{
	int i,e;
	
	i=0;
	// CR=0x00000000 (clear all)
	memory[i++]=0xd1;	memory[i++]=0x40;	memory[i++]=0x02;	memory[i++]=0x20;		memory[i++]=0x14;		//TAR = CR
	memory[i++]=0xdd;	memory[i++]=0x00;	memory[i++]=0x00;	memory[i++]=0x00;		memory[i++]=0x00;		//0x00 -> clear all

	// KEYR=0x45670123
	memory[i++]=0xd1;	memory[i++]=0x40;	memory[i++]=0x02;	memory[i++]=0x20;		memory[i++]=0x08;		//TAR = KEYR
	memory[i++]=0xdd;	memory[i++]=0x45;	memory[i++]=0x67;	memory[i++]=0x01;		memory[i++]=0x23;

	// KEYR=0xCDEF89AB
	memory[i++]=0xd1;	memory[i++]=0x40;	memory[i++]=0x02;	memory[i++]=0x20;		memory[i++]=0x08;		//TAR = KEYR
	memory[i++]=0xdd;	memory[i++]=0xCD;	memory[i++]=0xEF;	memory[i++]=0x89;		memory[i++]=0xAB;

	// CLER SR
	memory[i++]=0xd1;	memory[i++]=0x40;	memory[i++]=0x02;	memory[i++]=0x20;		memory[i++]=0x10;		//TAR = SR
	memory[i++]=0xdd;	memory[i++]=0x00;	memory[i++]=0x02;	memory[i++]=0xFF;		memory[i++]=0xFF;		//

	// CR=0x00000004 (MER1)
	memory[i++]=0xd1;	memory[i++]=0x40;	memory[i++]=0x02;	memory[i++]=0x20;		memory[i++]=0x14;		//TAR = CR
	memory[i++]=0xdd;	memory[i++]=0x00;	memory[i++]=0x00;	memory[i++]=0x00;		memory[i++]=0x04;		//MER1

	// CR=0x00010004 (MER1+STRT)
	memory[i++]=0xd1;	memory[i++]=0x40;	memory[i++]=0x02;	memory[i++]=0x20;		memory[i++]=0x14;		//TAR = CR
	memory[i++]=0xdd;	memory[i++]=0x00;	memory[i++]=0x01;	memory[i++]=0x00;		memory[i++]=0x04;		//MER1+STRT

	// SR
	memory[i++]=0xd1;	memory[i++]=0x40;	memory[i++]=0x02;	memory[i++]=0x20;		memory[i++]=0x10;		//TAR = SR

//	printf( "SEND %d BYTES\n",i);
//	waitkey();
	e=prg_comm(0x9d,i,0,0,ROFFSET,0,0,0,i/5);
	return e;
}
*/


int prog_stm32f2(void)
{
	int errc,blocks,i,j;
	unsigned long addr,len,maddr;
	int main_erase=0;
	int main_prog=0;
	int main_verify=0;
	int main_readout=0;
	int dev_start=0;
	int run_ram=0;
	int option_erase=0;
	int option_prog=0;
	int option_verify=0;
	int option_readout=0;
	int debug_ram=0;
	int debug_flash=0;
	int unsecure=0;
	int ignore_id=0;

	errc=0;

	if((strstr(cmd,"help")) && ((strstr(cmd,"help") - cmd) == 1))
	{
		printf("-- em -- main flash erase\n");
		printf("-- pm -- main flash program\n");
		printf("-- vm -- main flash verify\n");
		printf("-- rm -- main flash readout\n");

		printf("-- eo -- option bytes erase\n");
		printf("-- po -- option bytes program\n");
		printf("-- vo -- option bytes verify\n");
		printf("-- ro -- option bytes readout\n");

		printf("-- ii -- ignore ID\n");

		printf("-- rr -- run code in RAM\n");
		printf("-- dr -- debug code in RAM\n");
		printf("-- df -- debug code in FLASH\n");
		printf("-- st -- start device\n");
 		printf("-- d2 -- switch to device 2\n");

		return 0;
	}

	if(find_cmd("d2"))
	{
		errc=prg_comm(0x2ee,0,0,0,0,0,0,0,0);	//dev 2
		printf("## switch to device 2\n");
	}

	if(find_cmd("ii"))
	{
		ignore_id=1;
		printf("## ignore ID\n");
	}


	if(find_cmd("rr"))
	{
		if(file_found < 2)
		{
			run_ram = 0;
			printf("## Action: run code in RAM !! DISABLED BECAUSE OF NO FILE !!\n");
		}
		else
		{
			run_ram=1;
			printf("## Action: run code in RAM using %s\n",sfile);
			goto stm32f2_ORUN;
		}
	}
	else if(find_cmd("dr"))
	{
		if(file_found < 2)
		{
			debug_ram = 0;
			printf("## Action: debug code in RAM !! DISABLED BECAUSE OF NO FILE !!\n");
		}
		else
		{
			debug_ram = 1;
			printf("## Action: debug code in RAM using %s\n",sfile);
			goto stm32f2_ORUN;
		}
	}
	else if(find_cmd("df"))
	{
		debug_flash = 1;
		printf("## Action: debug code in FLASH\n");
		goto stm32f2_ORUN;
	}
	else
	{
		if(find_cmd("un"))
		{
			unsecure=1;
			printf("## Action: unsecure device\n");
		}

		if(find_cmd("em"))
		{
			main_erase=1;
			printf("## Action: main flash erase\n");
		}


		if(find_cmd("eo"))
		{
			option_erase=1;
			printf("## Action: option bytes erase\n");
		}

		main_prog=check_cmd_prog("pm","code flash");
		main_verify=check_cmd_verify("vm","code flash");
		main_readout=check_cmd_read("rm","code flash",&main_prog,&main_verify);

		option_prog=check_cmd_prog("po","option bytes");
		option_verify=check_cmd_verify("vo","option bytes");
		option_readout=check_cmd_read("ro","option bytes",&option_prog,&option_verify);

		if(find_cmd("st"))
		{
			dev_start=1;
			printf("## Action: start device\n");
		}
	}
	printf("\n");

stm32f2_ORUN:

	//open file if read 
	if((main_readout == 1) || (option_readout == 1))
	{
		errc=writeblock_open();
	}

	if(errc > 0) goto STM32_EXIT;

	if(dev_start == 0)
	{
		errc=prg_comm(0xbe,0,16,0,0,0,0,0,0);					//init

		if(errc > 0) 
		{
			printf("ACK STATUS: %d\n",memory[0]);
			goto STM32_EXIT;
		}

		printf("JID: %02X%02X%02X%02X\n",memory[3],memory[2],memory[1],memory[0]);


		errc=prg_comm(0xbf,0,256,0,ROFFSET,0x20,0x04,0xe0,1);

		printf("ID : %03X\n",((memory[ROFFSET+0x01] << 8) + memory[ROFFSET+0x00]) & 0xfff);
		if((ignore_id == 0) && ((((memory[ROFFSET+0x01] << 8) + memory[ROFFSET+0x00]) & 0xfff) > 0xf00)) errc=0x50;

//		printf("DHCSR: %02X%02X%02X%02X\n",memory[7],memory[6],memory[5],memory[4]);
//		printf("DEMCR: %02X%02X%02X%02X\n",memory[11],memory[10],memory[9],memory[8]);
//		printf("DHCSR: %02X%02X%02X%02X\n",memory[15],memory[14],memory[13],memory[12]);


		if((option_erase == 1) && (errc == 0))
		{

			printf("ERASE OPTIONBYTES\n");
			errc=prg_comm(0x4e,0,4,0,0,param[9],0,0,0);	//erase direct
//			show_data(0,4);
			printf("RE-INIT\n");
			errc=prg_comm(0xbe,0,16,0,0,0,0,0,0);		//re-init
		}

		if((main_erase == 1) && (errc == 0))
		{
			printf("ERASE FLASH (MODE=%ld)\n",param[8]);
			errc=prg_comm(0x4e,0,4,0,0,param[8],0,0,param[11]);	//erase direct
//			show_data(0,4);
			if(memory[0] & 0x10) 
			{
				errc= 0x60;
			}
			else
			{
				printf("RE-INIT\n");
//				prg_comm(0x0f,0,0,0,0,0,0,0,0);			//exit
				errc=prg_comm(0xbe,0,16,0,0,0,0,0,0);		//re-init
			}
		}

		//transfer loader to ram
		if((run_ram == 0) && (errc == 0) && ((main_prog == 1) || (option_prog == 1) || (option_erase == 1) || (unsecure == 1)))
		{
			printf("TRANSFER LOADER...");
			for(j=0;j<512;j++)
			{
				memory[j]=exec_stm32f2xx[j];
			}

			addr=param[4];				//RAM start

			errc=prg_comm(0xb2,0x200,0,0,0,		//write 1 K bootloader
				(addr >> 8) & 0xff,
				(addr >> 16) & 0xff,
				(addr >> 24) & 0xff,
				2);

//		
			errc=prg_comm(0x128,8,12,0,0,	
				addr & 0xff,
				(addr >> 8) & 0xff,
				(addr >> 16) & 0xff,
				(addr >> 24) & 0xff);

			errc=prg_comm(0x12b,0,64,0,0,0,0,0,0);	

			printf("STARTED\n");


/*
			errc=prg_comm(0xb3,8,12,0,0,		//go
				addr & 0xff,			
				(addr >> 8) & 0xff,
				(addr >> 16) & 0xff,
				(addr >> 24) & 0xff);
*/		}
	}
	
	if((run_ram == 0) && (errc == 0) && (dev_start == 0))
	{
		
		if((main_prog == 1) && (errc == 0))
		{
			addr=param[0];
			maddr=0;
			blocks=param[1]/max_blocksize;
			len=read_block(param[0],param[1],0);			//read flash
			if(len==0) len=read_block(0,param[1],0);		//read flash from addr=0

			progress("FLASH PROG ",blocks,0);
//			printf("ADDR = %08lx  LEN= %d Blocks\n",addr,blocks);

			errc=prg_comm(0x59,0,0,0,0,0x55,0x00,0x00,0x00);			//unlock main

			for(i=0;i<blocks;i++)
			{
				if(must_prog(maddr,max_blocksize) && (errc==0))
				{
					//transfer data
					errc=prg_comm(0xb2,max_blocksize,0,maddr,0,
						0x04,0x00,0x20,max_blocksize >> 8);

					//execute prog
					errc=prg_comm(0x59,0,0,0,0,
						0x52,
						(addr >> 8) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 24) & 0xff);

				}
				addr+=max_blocksize;
				maddr+=max_blocksize;
				progress("FLASH PROG ",blocks,i+1);
			}
			printf("\n");
		}

		if(((main_readout == 1) || (main_verify == 1)) && (errc == 0))
		{
			maddr=0;
			addr=param[0];
			blocks=param[1]/max_blocksize;
//			addr=0x20000000;
//			printf("ADDR = %08lx  LEN= %d Blocks\n",addr,blocks);
			progress("FLASH READ ",blocks,0);
			for(i=0;i<blocks;i++)
			{
				if(errc==0)
				{
					errc=prg_comm(0xbf,0,2048,0,ROFFSET+maddr,
						(addr >> 8) & 0xff,
						(addr >> 16) & 0xff,
						(addr >> 24) & 0xff,
						max_blocksize >> 8);
					addr+=max_blocksize;
					maddr+=max_blocksize;
					progress("FLASH READ ",blocks,i+1);
				}
			}
			printf("\n");
		}


		if((main_readout == 1) && (errc == 0))
		{
			writeblock_data(0,param[1],param[0]);
		}

		//verify main
		if((main_verify == 1) && (errc == 0))
		{
			len=read_block(param[0],param[1],0);			//read flash
			if(len==0) len=read_block(0,param[1],0);		//read flash from addr=0
			printf("VERIFY DATA (%ld KBytes)\n",param[1]/1024);
			addr = param[0];
			maddr=0;
			len = param[1];
			for(j=0;j<len;j++)
			{
				if(memory[maddr+j] != memory[maddr+j+ROFFSET])
				{
					printf("ERR -> ADDR= %08lX  FILE= %02X  READ= %02X\n",
						addr+j,memory[maddr+j],memory[maddr+j+ROFFSET]);
					errc=1;
				}
			}
		}


		if((option_prog == 1) && (errc == 0))
		{
			printf("PROGRAM OPTIONBYTES: ");
			read_block(param[2],param[3],0);	//read option bytes
			addr=param[2];
			maddr=0;

			printf("ADDR = %08lX\n",addr);
			
			show_data(0,16);
			
			//transfer data
			errc=prg_comm(0xb2,256,0,maddr,0,0x04,0x00,0x20,1);

			//execute prog
			errc=prg_comm(0x59,0,0,0,0,
					0x72,
					(addr >> 8) & 0xff,
					(addr >> 16) & 0xff,
					(addr >> 24) & 0xff);

			for(i=0;i<16;i++) printf(" %02X",memory[i]);

			printf("\n");
		}


		if(((option_readout == 1) || (option_verify == 1)) && (errc == 0))
		{
			addr=param[2];

			printf("READ OPTIONBYTES   : ");

			errc=prg_comm(0xbf,0,256,0,ROFFSET,
				(addr >> 8) & 0xff,
				(addr >> 16) & 0xff,
				(addr >> 24) & 0xff,
				1);

			for(i=0;i<16;i++) printf(" %02X",memory[ROFFSET+i]);

			printf("\n");
		}

		if((option_readout == 1) && (errc == 0))
		{
			writeblock_data(0,param[3],param[2]);
		}

		//verify option bytes
		if((option_verify == 1) && (errc == 0))
		{
			read_block(param[2],param[3],0);	//read option bytes
			printf("VERIFY OPTION BYTES\n");
			addr = param[2];
			len = param[3];
			maddr=0;
			for(j=0;j<len;j++)
			{
				if(memory[maddr+j] != memory[maddr+j+ROFFSET])
				{
					printf("ERR -> ADDR= %08lX  FILE= %02X  READ= %02X\n",
						addr+j,memory[maddr+j],memory[maddr+j+ROFFSET]);
					errc=1;
				}
			}
		}


		//open file if was read 
		if((main_readout == 1) || (option_readout == 1))
		{
			writeblock_close();
		}
	}



	if((run_ram == 1) && (errc == 0))
	{
		len = read_block(param[4],param[5],0);
		printf("BYTES= %04lX\n",len);
		if(len < 8)
		{	
			len = read_block(0,param[5],0);	//read from addr 0
			printf("LOW BYTES= %04lX\n",len);
		}

		printf("TRANSFER & START CODE\n");
		addr=param[4];
		maddr=0;
		blocks=(param[5]+2047) >> 11;

		progress("TRANSFER ",blocks,0);

		for(i=0;i<blocks;i++)
		{
			errc=prg_comm(0xb2,max_blocksize,0,maddr,0,		//write 1.K
				(addr >> 8) & 0xff,
				(addr >> 16) & 0xff,
				0x20,max_blocksize >> 8);
		
			addr+=max_blocksize;
			maddr+=max_blocksize;
			progress("TRANSFER ",blocks,i+1);
		}


		addr=param[4];

		printf("\nSTART CODE AT 0x%02x%02x%02x%02x\n",memory[7],memory[6],memory[5],memory[4]);
		
		errc=prg_comm(0x128,8,12,0,0,0,0,0,0);	//set pc + sp	

		errc=prg_comm(0x12b,0,100,0,0,0,0,0,0);	

		if(errc == 0)
		{
			waitkey();
		}
		
	}

	if((debug_ram==1) && (errc==0)) debug_armcortex(0);
	if((debug_flash==1) && (errc==0)) debug_armcortex(1);

	if(dev_start == 1)
	{
		i=prg_comm(0x0e,0,0,0,0,0,0,0,0);			//init
		waitkey();
		i=prg_comm(0x0f,0,0,0,0,0,0,0,0);					//exit
	}

STM32_EXIT:

	i=prg_comm(0x91,0,0,0,0,0,0,0,0);					//SWIM exit

	prg_comm(0x2ef,0,0,0,0,0,0,0,0);	//dev 1

	print_stm32f2_error(errc);

	return errc;
}

