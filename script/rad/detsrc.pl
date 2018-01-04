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

	    my $significance = 3.0;
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

#	### milcsmooth
#	open(OUT, ">setup/func_par.dat");
#	printf(OUT "## name  par\n");
#	printf(OUT "mu_xp        0\n");
#	printf(OUT "sigma_xp     2\n");
#	printf(OUT "norm         1\n");
#	close(OUT);
#
#	{
#	    for(my $irank = 0; $irank < $nrank; $irank ++){
#		my $infile = sprintf("imgline/%s/out_row_hd1d_%3.3d.dat", $midname, $irank);
#		my $infile_mask = sprintf("zeroline/%s/out_mask1d_hd1d.dat", $midname);
#		my $func = "Gauss1dFunc";
#		my $par_file = "setup/func_par.dat";
#		my $nbin_kernel_half = 10;
#		my $outdir = sprintf("lcsmooth/%s/r%3.3d", $midname, $irank);
#		my $outfile_head = "out";
#
#		$cmd = sprintf("/home/morii/work/github/moriiism/mitool/miutil/milcsmooth/milcsmooth " .
#			       "%s  %s  %s  %s  " .
#			       "%d  %s  %s" ,
#			       $infile, $infile_mask, $func, $par_file,
#			       $nbin_kernel_half, $outdir, $outfile_head);
#		printf("cmd = %s\n", $cmd);
#		system($cmd);
#	    }
#	}

	{
	    for(my $irank = 0; $irank < $nrank; $irank ++){
		# my $infile = sprintf("lcsmooth/%s/r%3.3d/out_smooth.dat", $midname, $irank);
		my $infile = sprintf("imgline/%s/out_row_hd1d_%3.3d.dat", $midname, $irank);
		my $infile_mask = sprintf("zeroline/%s/out_mask1d_hd1d.dat", $midname);
		my $significance_src = 5.0;
		my $significance_bg = 2.0;
		my $nclip  = 30;
		my $outdir = sprintf("lcsig/%s/r%3.3d", $midname, $irank);
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
	### merge lc
	###

	{
	    my $hd1d_list = "setup/imgline.list";
	    open(OUT, ">$hd1d_list");
	    for(my $irank = 0; $irank < $nrank; $irank ++){
		my $imgline_file = sprintf("imgline/%s/out_row_hd1d_%3.3d.dat", $midname, $irank);
		printf(OUT "%s\n", $imgline_file);
	    }
	    close(OUT);
	    my $sv_dat = sprintf("out/%s/out_L_S.txt", $midname);
	    my $outdir = sprintf("mergelc/%s", $midname);
	    my $outfile_head = "mergelc";
	    $cmd = sprintf("/home/morii/work/github/moriiism/" .
			   "tme/util/mergelcrank/mergelcrank " .
			   "%s  %s  %d  " .
			   "%s  %s" ,
			   $hd1d_list, $sv_dat, $nrank,
			   $outdir, $outfile_head);
	    printf("cmd = %s\n", $cmd);
	    system($cmd);
	}

	{
	    my $infile = sprintf("mergelc/%s/mergelc.dat", $midname);
	    my $infile_mask = sprintf("zeroline/%s/out_mask1d_hd1d.dat", $midname);
	    my $significance_src = 5.0;
	    my $significance_bg = 2.0;
	    my $nclip  = 30;
	    my $outdir = sprintf("lcsig_merge/%s", $midname);
	    my $outfile_head = "out";

	    $cmd = sprintf("/home/morii/work/github/moriiism/mitool/miutil/migetsighd1d/migetsighd1d " .
			   "%s  %s  %e  %e  " .
			   "%d  %s  %s" ,
			   $infile, $infile_mask, $significance_src, $significance_bg, 
			   $nclip, $outdir, $outfile_head);
	    printf("cmd = %s\n", $cmd);
	    system($cmd);
	}

	###
	### for image
	###
	{
	    my $infile = sprintf("out/%s/out_L_U.fits", $midname);
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

	## merge image
	{
	    my $fits_list = "setup/cubeframe.list";
	    open(OUT, ">$fits_list");
	    for(my $irank = 0; $irank < $nrank; $irank ++){
		my $cubeframe_file = sprintf("cubeframe/%s/out_frame_%3.3d.fits", $midname, $irank);
		printf(OUT "%s\n", $cubeframe_file);
	    }
	    close(OUT);
	    my $sv_dat = sprintf("out/%s/out_L_S.txt", $midname);
	    my $outdir = sprintf("mergeimg/%s", $midname);
	    my $outfile_head = "mergeimg";
	    $cmd = sprintf("/home/morii/work/github/moriiism/" .
			   "tme/util/mergeimgrank/mergeimgrank " .
			   "%s  %s  %d  " .
			   "%s  %s" ,
			   $fits_list, $sv_dat, $nrank,
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

	###
	### for merged image
	###
	{
	    my $infile = sprintf("mergeimg/%s/mergeimg_merge.fits", $midname);
	    my $infile_mask = sprintf("enmask/%s/out_mask2d.fits", $midname);
	    my $func = "Gauss2dFunc";
	    my $par_file = "setup/func_par.dat";
	    my $nbin_kernel_half = 15;
	    my $outdir = sprintf("imgsmooth_merge/%s", $midname);
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

	{
	    my $infile = sprintf("imgsmooth_merge/%s/out_smooth.fits", $midname);
	    my $infile_mask = sprintf("enmask/%s/out_mask2d.fits", $midname);
	    my $significance = 5.0;
	    my $outdir = sprintf("imgsig_merge/%s", $midname);
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


	# merge img & lc
	{
	    for(my $irank = 0; $irank < $nrank; $irank ++){
		my $img_src = sprintf("imgsig/%s/r%3.3d/out_src.txt", $midname, $irank);
		if(! -e $img_src){
		    next;
		}
		my $wcout = `wc $img_src`;
		my @wc_arr = split(' ', $wcout);
		if(0 == $wc_arr[0]){
		    next;
		}

		my $lc_src = sprintf("lcsig/%s/r%3.3d/out_src.txt", $midname, $irank);
		if(! -e $lc_src){
		    next;
		}
		$wcout = `wc $lc_src`;
		@wc_arr = split(' ', $wcout);
		if(0 == $wc_arr[0]){
		    next;
		}
		
		my $outdir = sprintf("imglc/%s/r%3.3d", $midname, $irank);
		$cmd = sprintf("mkdir -p %s", $outdir);
		printf("cmd = %s\n", $cmd);
		system($cmd);

		my $outimg = sprintf("%s/img_src.txt", $outdir);
		$cmd = sprintf("cp %s %s", $img_src, $outimg); 
		printf("cmd = %s\n", $cmd);
		system($cmd);

		my $outlc = sprintf("%s/lc_src.txt", $outdir);
		$cmd = sprintf("cp %s %s", $lc_src, $outlc); 
		printf("cmd = %s\n", $cmd);
		system($cmd);
	    }
	}

	### for merged rank, signed
	{
	    my $img_src = sprintf("imgsig_merge/%s/out_src_signed.txt", $midname);
	    if(! -e $img_src){
		next;
	    }
	    my $wcout = `wc $img_src`;
	    my @wc_arr = split(' ', $wcout);
	    if(0 == $wc_arr[0]){
		next;
	    }

	    my $lc_src = sprintf("lcsig_merge/%s/out_src_signed.txt", $midname);
	    if(! -e $lc_src){
		next;
	    }
	    $wcout = `wc $lc_src`;
	    @wc_arr = split(' ', $wcout);
	    if(0 == $wc_arr[0]){
		next;
	    }
	    
	    my $outdir = sprintf("imglc_merge/%s", $midname);
	    $cmd = sprintf("mkdir -p %s", $outdir);
	    printf("cmd = %s\n", $cmd);
	    system($cmd);

	    my $outimg = sprintf("%s/img_src.txt", $outdir);
	    $cmd = sprintf("cp %s %s", $img_src, $outimg); 
	    printf("cmd = %s\n", $cmd);
	    system($cmd);

	    my $outlc = sprintf("%s/lc_src.txt", $outdir);
	    $cmd = sprintf("cp %s %s", $lc_src, $outlc); 
	    printf("cmd = %s\n", $cmd);
	    system($cmd);
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
