#!/bin/bash
#!/Task 5
read -p "Pleas Enter an encrypted file path into terminal > " filepath

if [ -f "$filepath" ]
  then
    while read LINE           
      do
      
      length=${#LINE}
      target=16
	  if [ $length -lt $target ]
	  then
	    echo "Length=$length:Password=$LINE"
	    openssl enc -aes-128-cbc -d -in ./some.aes-128-cbc -out ./dictionary_atk.txt -nosalt -pass "pass:$LINE"
	    if [ "$?" -eq 0 ]
	    then
	      while read decrypttext
	      do
	      echo "Password:[$LINE] is a key. Result plain text is: $decrypttext" >> ./result.txt
	      done < ./dictionary_atk.txt
	    fi
	  fi
	  
      done < $filepath
  
else
  echo "[ERROR]: Can not find $filepath."
fi