#!/usr/bin/perl
#################################################################################
#										#
# rom data generator/patcher for AX81						#
# copyright (c) 2010-2011 Joerg Wolfram (joerg@jcwolfram.de)			#
#										#
# This program is free software; you can redistribute it and/or			#
# modify it under the terms of the GNU General Public License			#
# as published by the Free Software Foundation; either version 3		#
# of the License, or (at your option) any later version.			#
#										#
# This program is distributed in the hope that it will be useful,		#
# but WITHOUT ANY WARRANTY; without even the implied warranty of		#
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU		#
# General Public License for more details.					#
#										#
# You should have received a copy of the GNU General Public			#
# License along with this library; if not, write to the				#
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,			#
# Boston, MA 02111-1307, USA.							#
#										#
#################################################################################
use POSIX;
use Fcntl;

$hexfile="boot.s19";
$datfile=">exec_ppcjtag.h";
$head="exec_ppcjtag_0b";


$dstart=0x100;
$dlen=1024;

&gencode;


sub gencode
{
	for($i=0;$i<8192;$i++)
	{
		$romdata[$i]="FF";
	}
	open (BREAD, $hexfile);
	$active=0;
	while (<BREAD>)
	{
		chomp;
		$line=$_;
		if(substr($line,0,6) eq "S11301")
		{
			$active=1;
		}
		if((substr($line,0,2) eq "S1") && ($active == 1))
		{
			$line=substr($line,0,length($line)-2)."FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
			$addr=substr($line,4,4);
			$addr=sprintf("%u",hex($addr));
			$llen=substr($line,2,2);
			$llen=sprintf("%u",hex($llen))-3;
#			print $addr."\t".$llen."\n";
			for($i=0;$i<$llen;$i++)
			{
				$romdata[$addr+$i]=substr($line,8+2*$i,2);
			}
			
		}
	}

	close(BREAD);

	open (DWRITE,$datfile);
	print DWRITE "unsigned char ".$head."[$dlen] = {";


	for($i=$dstart;$i<($dstart+$dlen);$i++)
	{
		if(($i%16)==0)
		{
			printf DWRITE "// %04X \n\t\t",$i;
		}
		printf DWRITE "0x%s",$romdata[$i];
		if($i<(($dlen+$dstart)-1))
		{
			print DWRITE ",";
		}
	}
	print DWRITE "};\n";

	close(DWRITE);
}

