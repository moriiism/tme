#!/usr/bin/perl

#
# detsrc.pl
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
$mipllib::sevar{'progname'} = "detsrc.pl";

# main
{
    printf("#--- %s: START ---#\n", $mipllib::sevar{'progname'});

    my $cmd;
    # -- argument
    my $NARG = 1;
    my($filelist);
    printf("# of arg = %d\n", $#ARGV + 1);
    if( $#ARGV == $NARG - 1){
	my $iarg = 0;
	$filelist = $ARGV[$iarg]; $iarg ++;
    }else{
        printf("# of arg must be %d.\n", $NARG);
	&Usage();
    }
    my $prompt_arg = "arg";
    printf("%s: %s: filelist  = %s\n", $mipllib::sevar{'progname'}, $prompt_arg, $filelist);
    # == argument

    open(LIST, "<$filelist");
    while(my $line = <LIST>){
	chomp($line);
        if($line =~ /^\s*$/ or $line =~ /^\s*\#/ or $line =~ /^\s*\!/) {
            next;
        }
	my $dirname = dirname($line);
	my $index_st = index($dirname, "/");
	my $midname = substr($dirname, $index_st + 1);

	### getrank
	{ 
	    my $infile = sprintf("%s/out_L_S.txt", $dirname);
	    my $outdir = sprintf("getrank/%s", $midname);
	    my $outfile_head = "out";

	    my $significance = 2.0;
	    my $nclip = 30;
	    $cmd = sprintf("/home/morii/work/github/moriiism/tme/util/getrank/getrank " .
			   "%s  %e  " .
			   "%d  %s  %s" ,
			   $infile, $significance,
			   $nclip, $outdir, $outfile_head);
	    printf("cmd = %s\n", $cmd);
	    system($cmd);
	}

	## get rank
	$cmd = sprintf("cat getrank/%s/out_rank.dat", $midname);
	my $nrank = `$cmd`;
	printf("nrank = %d\n", $nrank);
	
	if($nrank == 0){
	    next;
	}

	### zeroline mask
	{ 
	    my $infile = sprintf("%s/out_L_V.fits", $dirname);
	    my $row_or_col = "row";
	    my $stddev_threshold = 0.01;
	    my $outdir = sprintf("zeroline/%s", $midname);
	    my $outfile_head = "out";
	    $cmd = sprintf("/home/morii/work/github/moriiism/" .
			   "mitool/miutil/miselimg_zeroline/miselimg_zeroline " .
			   "%s  %s  %e  " .
			   "%s  %s" ,
			   $infile, $row_or_col, $stddev_threshold,
			   $outdir, $outfile_head);
	    printf("cmd = %s\n", $cmd);
	    system($cmd);
	}


	### miselimgline
	{ 
	    my $infile = sprintf("%s/out_L_V.fits", $dirname);
	    my $row_or_col = "row";
	    my $line_st= 0;
	    my $line_ed= $nrank - 1;
	    my $outdir = sprintf("imgline/%s", $midname);
	    my $outfile_head = "out";
	    $cmd = sprintf("/home/morii/work/github/moriiism/" .
			   "mitool/miutil/miselimgline/miselimgline " .
			   "%s  %s  %d  %d  " .
			   "%s  %s" ,
			   $infile, $row_or_col, $line_st, $line_ed,
			   $outdir, $outfile_head);
	    printf("cmd = %s\n", $cmd);
	    system($cmd);
	}

	### milcsmooth
	open(OUT, ">setup/func_par.dat");
	printf(OUT "## name  par\n");
	printf(OUT "mu_xp        0\n");
	printf(OUT "sigma_xp     2\n");
	printf(OUT "norm         1\n");
	close(OUT);

	{
	    for(my $irank = 0; $irank < $nrank; $irank ++){
		my $infile = sprintf("imgline/%s/out_row_hd1d_%3.3d.dat", $midname, $irank);
		my $infile_mask = sprintf("zeroline/%s/out_mask1d_hd1d.dat", $midname);
		my $func = "Gauss1dFunc";
		my $par_file = "setup/func_par.dat";
		my $nbin_kernel_half = 10;
		my $outdir = sprintf("lcsmooth/%s/r%3.3d", $midname, $irank);
		my $outfile_head = "out";

		$cmd = sprintf("/home/morii/work/github/moriiism/mitool/miutil/milcsmooth/milcsmooth " .
			       "%s  %s  %s  %s  " .
			       "%d  %s  %s" ,
			       $infile, $infile_mask, $func, $par_file,
			       $nbin_kernel_half, $outdir, $outfile_head);
		printf("cmd = %s\n", $cmd);
		system($cmd);
	    }
	}

	{
	    for(my $irank = 0; $irank < $nrank; $irank ++){
		my $infile = sprintf("lcsmooth/%s/r%3.3d/out_smooth.dat", $midname, $irank);
		my $infile_mask = sprintf("zeroline/%s/out_mask1d_hd1d.dat", $midname);
		my $significance_src = 5.0;
		my $significance_bg = 2.0;
		my $nclip  = 30;
		my $outdir = sprintf("getsig/%s/r%3.3d", $midname, $irank);
		my $outfile_head = "out";

		$cmd = sprintf("/home/morii/work/github/moriiism/mitool/miutil/migetsighd1d/migetsighd1d " .
			       "%s  %s  %e  %e  " .
			       "%d  %s  %s" ,
			       $infile, $infile_mask, $significance_src, $significance_bg, 
			       $nclip, $outdir, $outfile_head);
		printf("cmd = %s\n", $cmd);
		system($cmd);
	    }
	}

	###
	### for image
	###
	{
	    my $infile = sprintf("godec/%s/out_L_U.fits", $midname);
	    my $frame_st = 0;
	    my $frame_ed = $nrank - 1;
	    my $outdir = sprintf("cubeframe/%s", $midname);
	    my $outfile_head = "out";
	    $cmd = sprintf("/home/morii/work/github/moriiism/mitool/miutil/miselcubeframe/miselcubeframe " .
			   "%s  %d  %d  " .
			   "%s  %s" ,
			   $infile, $frame_st, $frame_ed,
			   $outdir, $outfile_head);
	    printf("cmd = %s\n", $cmd);
	    system($cmd);
	}

	## en mask
	{
	    my $infile = sprintf("cubeframe/%s/out_frame_000.fits", $midname);
	    my $cx = 160.0;
	    my $cy = 160.0;
	    my $rad = 150.0;
	    my $outdir = sprintf("enmask/%s", $midname);
	    my $outfile_head = "out";
	    $cmd = sprintf("/home/morii/work/github/moriiism/mitool/miutil/mimkimg_enmask/mimkimg_enmask " .
			   "%s  %e  %e  %e " .
			   "%s  %s" ,
			   $infile, $cx, $cy, $rad,
			   $outdir, $outfile_head);
	    printf("cmd = %s\n", $cmd);
	    system($cmd);
	}

	open(OUT, ">setup/func_par.dat");
	printf(OUT "## name  par\n");
	printf(OUT "sigma_xp    5\n");
	printf(OUT "sigma_yp    5\n");
	printf(OUT "norm        1\n");
	printf(OUT "rot_angle   0\n");
	printf(OUT "mu_xp       0\n");
	printf(OUT "mu_yp       0\n");
	printf(OUT "shift_z     0\n");
	close(OUT);
	{
	    for(my $irank = 0; $irank < $nrank; $irank ++){
		my $infile = sprintf("cubeframe/%s/out_frame_%3.3d.fits", $midname, $irank);
		my $infile_mask = sprintf("enmask/%s/out_mask2d.fits", $midname);
		my $func = "Gauss2dFunc";
		my $par_file = "setup/func_par.dat";
		my $nbin_kernel_half = 15;
		my $outdir = sprintf("imgsmooth/%s/r%3.3d", $midname, $irank);
		my $outfile_head = "out";
		$cmd = sprintf("/home/morii/work/github/moriiism/mitool/miutil/miimgsmooth/miimgsmooth " .
			       "%s  %s  " .
			       "%s  %s  %d  " .
			       "%s  %s",
			       $infile, $infile_mask,
			       $func, $par_file, $nbin_kernel_half,
			       $outdir, $outfile_head);
		printf("cmd = %s\n", $cmd);
		system($cmd);
	    }
	}

	{
	    for(my $irank = 0; $irank < $nrank; $irank ++){
		my $infile = sprintf("imgsmooth/%s/r%3.3d/out_smooth.fits", $midname, $irank);
		my $infile_mask = sprintf("enmask/%s/out_mask2d.fits", $midname);
		my $significance = 5.0;
		my $outdir = sprintf("imgsig/%s/r%3.3d", $midname, $irank);
		my $outfile_head = "out";
		$cmd = sprintf("/home/morii/work/github/moriiism/mitool/miutil/migetsigimg/migetsigimg " .
			       "%s  %s  " .
			       "%e  " .
			       "%s  %s",
			       $infile, $infile_mask,
			       $significance,
			       $outdir, $outfile_head);
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
