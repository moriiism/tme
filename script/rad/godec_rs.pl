#!/usr/bin/perl

#
# godec_rs.pl
#   loop with rank and sparsity
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
$mipllib::sevar{'progname'} = "godec_rs.pl";

# main
{
    printf("#--- %s: START ---#\n", $mipllib::sevar{'progname'});

    my $cmd;
    # -- argument
    my $NARG = 2;
    my($filelist, $tag);
    printf("# of arg = %d\n", $#ARGV + 1);
    if( $#ARGV == $NARG - 1){
	my $iarg = 0;
	$filelist = $ARGV[$iarg]; $iarg ++;
	$tag      = $ARGV[$iarg]; $iarg ++;
    }else{
        printf("# of arg must be %d.\n", $NARG);
	&Usage();
    }
    my $prompt_arg = "arg";
    printf("%s: %s: filelist  = %s\n", $mipllib::sevar{'progname'}, $prompt_arg, $filelist);
    printf("%s: %s: tag       = %s\n", $mipllib::sevar{'progname'}, $prompt_arg, $tag);
    # == argument

    # my @nrank_arr = (2, 3, 5, 10, 50, 100, 999);
    my @nrank_arr = (999);
    my @ncard_arr = (10000, 100000, 1000000);

    open(LIST, "<$filelist");
    while(my $line = <LIST>){
	chomp($line);
        if($line =~ /^\s*$/ or $line =~ /^\s*\#/ or $line =~ /^\s*\!/) {
            next;
        }
	my $in_file = $line;

	# parse
	my $index_st = index($in_file, $tag);
	my $index_ed = index($in_file, ".fits");
	my $file_name_last = substr($in_file, $index_st, $index_ed - $index_st);

	my $infofile_dir = sprintf("out/%s", $file_name_last);
	if(! -e $infofile_dir){
	    $cmd = sprintf("mkdir -p %s", $infofile_dir);
	    printf("cmd = %s\n", $cmd);
	    system($cmd);
	}
	my $infofile = sprintf("%s/fitsinfo.txt", $infofile_dir);
	$cmd = sprintf("/home/morii/work/github/moriiism/tme/util/fits/fitsinfo " .
		       "%s  %s",
		       $in_file, $infofile);
	printf("cmd = %s\n", $cmd);
	system($cmd);

	open(IN, "<$infofile");
	<IN>; <IN>;
	my $tmp;
	my ($xpix, $ypix, $zpix);
	($tmp, $xpix) = split(' ', <IN>);
	($tmp, $ypix) = split(' ', <IN>);
	($tmp, $zpix) = split(' ', <IN>);
	close(IN);

	my $subcube = sprintf("%s/subcube.dat", $infofile_dir);
	open(OUT, ">$subcube");
	printf(OUT "#  x(pixel)  y(pixel)  freq(frame) 4d\n");
	printf(OUT "1    1   1   1\n");
	printf(OUT "%d  %d  %d   1\n", $xpix, $ypix, $zpix);
	close(OUT);

	for(my $irank = 0; $irank <= $#nrank_arr; $irank ++){
	    my $nrank = $nrank_arr[$irank];
	    if($irank == $#nrank_arr){
		$nrank = $zpix / 2;
	    }
	    if($nrank > $zpix){
		next;
	    }
	    for(my $icard = 0; $icard <= $#ncard_arr; $icard ++){
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
