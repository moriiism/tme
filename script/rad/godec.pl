#!/usr/bin/perl

#
# godec.pl
#
# 2017.11.10  M.Morii
#

use strict;
use warnings;
use POSIX;
no warnings qw(once);

$ENV{'LANG'} = "C";

my $mipllib = sprintf("%s/script/mipllib/mipllib.pl",
		      $ENV{'MITOOL'});
require($mipllib);
$mipllib::sevar{'progname'} = "godec.pl";

# main
{
    printf("#--- %s: START ---#\n", $mipllib::sevar{'progname'});

    my $cmd;
    # -- argument
    my $NARG = 5;
    my($filelist, $subcube, $nrank, $ncard, $outdir);
    printf("# of arg = %d\n", $#ARGV + 1);
    if( $#ARGV == $NARG - 1){
	my $iarg = 0;
	$filelist = $ARGV[$iarg]; $iarg ++;
	$subcube  = $ARGV[$iarg]; $iarg ++;
	$nrank    = $ARGV[$iarg]; $iarg ++;
	$ncard    = $ARGV[$iarg]; $iarg ++;
	$outdir   = $ARGV[$iarg]; $iarg ++;
    }else{
        printf("# of arg must be %d.\n", $NARG);
	&Usage();
    }
    my $prompt_arg = "arg";
    printf("%s: %s: filelist  = %s\n", $mipllib::sevar{'progname'}, $prompt_arg, $filelist);
    printf("%s: %s: subcube   = %s\n", $mipllib::sevar{'progname'}, $prompt_arg, $subcube);
    printf("%s: %s: nrank     = %d\n", $mipllib::sevar{'progname'}, $prompt_arg, $nrank);
    printf("%s: %s: ncard     = %d\n", $mipllib::sevar{'progname'}, $prompt_arg, $ncard);
    printf("%s: %s: outdir    = %s\n", $mipllib::sevar{'progname'}, $prompt_arg, $outdir);
    # == argument

    open(LIST, "<$filelist");
    while(my $line = <LIST>){
	chomp($line);
        if($line =~ /^\s*$/ or $line =~ /^\s*\#/ or $line =~ /^\s*\!/) {
            next;
        }
	my $in_file = $line;

	# parse
	my $index_st = index($in_file, "sample");
	my $index_ed = index($in_file, ".fits");
	my $file_name_last = substr($in_file, $index_st, $index_ed - $index_st);

	my $qpar = 2;
	my $epsilon = 5.0e-2;
	my $niter = 30;
	my $seed = 0;
	my $outdir_this = sprintf("%s/%s/r%3.3d/n%.1e", 
				  $outdir, $file_name_last, $nrank, $ncard);
	my $outfile_head = "out";
	$cmd = sprintf("/home/morii/work/github/moriiism/tme/tmegodec/tmegodec_4d " .
		       "%s  %s  %d  %d  %d  " .
		       "%e  %d  %d  %s  %s" ,
		       $in_file, $subcube, $nrank, $ncard, $qpar,
		       $epsilon, $niter, $seed, $outdir_this, $outfile_head);
	printf("cmd = %s\n", $cmd);
	system($cmd);
    }
    close(LIST);

    printf("#=== %s: END ===#\n", $mipllib::sevar{'progname'});
} # main


sub Usage
{
    printf("usage: %s " .
	   "filelist subcube  nrank  ncard  outdir\n",
	   $mipllib::sevar{'progname'});
    exit;
}
