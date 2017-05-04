#!/bin/bash
#!/Task 5
read -p "Please Enter a SHA checksum sequence > " checksum
read -p "Pleas Enter a file path into terminal > " filepath
echo "Your input SHA checksum is: $checksum"

if [ -f "$filepath" ]
  then
    I=0
    for text in $(openssl sha1 "$filepath")
      do
      Sum[I]=$text
      let I=I+1
      done
    echo "Your file SHA checksum is: ${Sum[1]}"
  
    if [ "${Sum[1]}"=="$checksum" ]
    then
      echo "[RESULT]: File and Sequence ARE THE SAME."
    else
      echo "[RESULT]: File and Sequence ARE NOT THE SAME."
    fi
  
else
  echo "[ERROR]: Can not find $filepath."
fi