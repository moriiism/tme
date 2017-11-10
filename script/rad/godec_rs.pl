#!/usr/bin/perl

#
# template for a simple script
#
# 2014.10.22  M.Morii
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
    my $NARG = 2;
    my($filelist, $subcube);
    printf("# of arg = %d\n", $#ARGV + 1);
    if( $#ARGV == $NARG - 1){
	my $iarg = 0;
	$filelist = $ARGV[$iarg]; $iarg ++;
	$subcube  = $ARGV[$iarg]; $iarg ++;
    }else{
        printf("# of arg must be %d.\n", $NARG);
	&Usage();
    }
    my $prompt_arg = "arg";
    printf("%s: %s: filelist  = %s\n", $mipllib::sevar{'progname'}, $prompt_arg, $filelist);
    printf("%s: %s: subcube   = %s\n", $mipllib::sevar{'progname'}, $prompt_arg, $subcube);
    # == argument

    my @nrank_arr = (2, 3, 5, 10, 50, 100);
    my @ncard_arr = (10000, 100000, 1000000);

    open(LIST, "<$filelist");
    while(my $line = <LIST>){
	chomp($line);
        if($line =~ /^\s*$/ or $line =~ /^\s*\#/ or $line =~ /^\s*\!/) {
            next;
        }
	my $in_file = $line;

	# parse
	my $index_st = index($in_file, "test");
	my $index_ed = index($in_file, ".fits");
	my $file_name_last = substr($in_file, $index_st, $index_ed - $index_st);

	for(my $irank = 0; $irank <= $#nrank_arr; $irank ++){
	    for(my $icard = 0; $icard <= $#ncard_arr; $icard ++){
		my $nrank = $nrank_arr[$irank];
		my $ncard = $ncard_arr[$icard];
		my $qpar = 2;
		my $epsilon = 5.0e-2;
		my $niter = 30;
		my $seed = 0;
		my $outdir = sprintf("out/%s/r%3.3d/n%.1e", $file_name_last,
				     $nrank, $ncard);
		my $outfile_head = "out";

		$cmd = sprintf("/home/morii/work/github/moriiism/tme/tmegodec/tmegodec_4d " .
			       "%s  %s  %d  %d  %d  " .
			       "%e  %d  %d  %s  %s" ,
			       $in_file, $subcube, $nrank, $ncard, $qpar,
			       $epsilon, $niter, $seed, $outdir, $outfile_head);
		printf("cmd = %s\n", $cmd);
		system($cmd);
	    }
	}
    }
    close(LIST);

    printf("#=== %s: END ===#\n", $mipllib::sevar{'progname'});
} # main


sub Usage
{
    printf("usage: %s " .
	   "value\n", $mipllib::sevar{'progname'});
    exit;
}
