#
# setup.sh
#
# setup script for tme
#
#

#
# This script must be loaded by "source" command of bash,
# before using tme.
#

##
## mitool 
##

export MITOOL=/home/morii/work/github/moriiism/mitool
export LD_LIBRARY_PATH=/soft/root/6.08.02/lib:/soft/gsl/2.3/lib:${LD_LIBRARY_PATH}
export PATH=/soft/root/6.08.02/bin:${PATH}

alias root="root -l"

#
# HEADAS
#

HEADAS_VER=6.16
export HEADAS=/soft/heasoft/${HEADAS_VER}/x86_64-unknown-linux-gnu-libc2.17
source $HEADAS/headas-init.sh
export PGPLOT_TYPE=/xw

################################################################
################################################################
############ Don't Edit Below. #################################

##
## change the terminal title
##

TME_VER=master
termtitle="tme("$TME_VER") "
PROMPT_COMMAND='echo -ne "\033]0;${termtitle}\007"'
