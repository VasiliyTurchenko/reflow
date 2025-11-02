#!/bin/bash

###########################################################################
#
# The script produces some files requred for linting (PC-Lint)
#
###########################################################################

set +x
set -e

START_DIR=$(pwd)
TARGET_DIR="${START_DIR}/../compiler"

if [ -z "$1" ]
  then
    echo "The path to gcc's folder is not provided!"
    echo "Usage: $0 /usr/opt/gcc-arm"
    exit 255
fi

GCC_BINDIR="$1/bin"

GCC_MASK="*-gcc"
GPP_MASK="*-g++"

# get the executable name of the gcc
GCC=$(cd ${GCC_BINDIR} && ls ${GCC_MASK})

# get the executable name of the g++
GPP=$(cd ${GCC_BINDIR} && ls ${GPP_MASK})

#get path
echo "gcc found: " ${GCC}
echo "g++ found: " ${GPP}

GCC_BUILTINS="${GCC}-defs.h"
GPP_BUILTINS="${GPP}-defs.h"

GCC_=${GCC_BINDIR}"/"${GCC}
GPP_=${GCC_BINDIR}"/"${GPP}

cd ${TARGET_DIR}

# Make backups
if test -f "${GCC_BUILTINS}"
then
    echo "${GCC_BUILTINS} exists, making a backup: ${GCC_BUILTINS} -> ${GCC_BUILTINS}.bak"
    mv ${GCC_BUILTINS} "${GCC_BUILTINS}.bak"
fi

if test -f "${GPP_BUILTINS}"
then
    echo "${GPP_BUILTINS} exists, making a backup: ${GPP_BUILTINS} -> ${GPP_BUILTINS}.bak"
    mv ${GPP_BUILTINS} "${GPP_BUILTINS}.bak"
fi

echo | ${GCC_} -dM -E - > ${GCC_BUILTINS}
echo | ${GPP_} -dM -E -x c++ - > ${GPP_BUILTINS}

##### get internal include dirs 
TF1=${RANDOM}
TF2=${RANDOM}

${GCC_} -xc -E -v - < /dev/null > ${TF1} 2>&1
${GPP_} -xc++ -E -v - < /dev/null > ${TF2} 2>&1

GCC_BUILTIN_INC="${GCC}-include"
GPP_BUILTIN_INC="${GPP}-include"

FL=$(awk '/#include <...> search starts here:/{ print NR; exit }' ${TF1})
LL=$(awk '/End of search list./{ print NR; exit }' ${TF1})
AWK_CMD="FNR>${FL}"" && ""FNR<${LL}"
awk "FNR>${FL}"" && ""FNR<${LL}" ${TF1} > ${GCC_BUILTIN_INC}

FL=$(awk '/#include <...> search starts here:/{ print NR; exit }' ${TF2})
LL=$(awk '/End of search list./{ print NR; exit }' ${TF2})
AWK_CMD="FNR>${FL}"" && ""FNR<${LL}"
awk "FNR>${FL}"" && ""FNR<${LL}" ${TF2} > ${GPP_BUILTIN_INC}

rm ${TF1}
rm ${TF2}

set +x
set +e
