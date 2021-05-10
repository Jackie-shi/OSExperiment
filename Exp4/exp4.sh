#!/bin/bash
LOOPS=10
MAX_CHANGE=2
LOOP_COUNT=0
CHANGE_TIMES=0
echo -n "Input file name: "
read file_name
if [ ! -f ${file_name} ];
  then
    echo "Can't find the file [${file_name}]"
  else
    FILE_SIZE=$(ls -l ${file_name} | awk '{print $5}')
    ls -l ${file_name}
    while ((${LOOP_COUNT} < ${LOOPS}))
    do
      echo "test file's status ... "
      new_size=$(ls -l ${file_name} | awk '{print $5}')
      if [ ${FILE_SIZE} != ${new_size} ];
      then
          echo "file [${file_name}] size changed!"
          ${FILE_SIZE} = ${new_size}
          let "CHANGE_TIMES++"
      fi
      if [ ${CHANGE_TIMES} == ${MAX_CHANGE} ];
      then
          echo "Change number exceed two, test end!"
          exit 0
      fi
      let "LOOP_COUNT++"
      sleep 5s
    done
  echo "test number exceed ten!"
fi
