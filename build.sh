
for file in "client.c" "sender.c"
do 
    echo "Building $file...";
    gcc $file -g -o $(basename -s .c $file) -lgmp  -lcrypto 
done
