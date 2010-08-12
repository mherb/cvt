#! /bin/bash

if [ $# -ne 2 ]
then
    name=$(basename $0);
    echo "usage: $name lib header";
    exit 1;
fi

if [ ! -f $1 ]
then
    echo "No such file!";
    exit 1;
fi

echo $OSTYPE;
case $OSTYPE in
    "linux-gnu") 
		symextractor="objdump -t $1";
		rm -f $2;
		echo -e "#include \"CVTTest.h\"\n\nextern \"C\" { " >> $2;
		$symextractor | awk '/.*_test/ {print "\tbool "$6"( void );"}' >> $2;
		echo -e "}\n" >> $2;

		echo -e "\nCVTTest _tests[] = {" >> $2;
		$symextractor | awk '/.*_test/ {print "\t{ "$6", \""$6"\" }," }' >> $2;
		echo -e "\t{ NULL, NULL}\n};" >> $2;;
    "darwin10.0")
		symextractor="nm -g $1";
		rm -f $2;
		echo -e "#include \"CVTTest.h\"\n\nextern \"C\" { " >> $2;
		$symextractor | awk '/.*_test/ {print "\tbool "$3"( void );"}' >> $2;
		echo -e "}\n" >> $2;

		echo -e "\nCVTTest _tests[] = {" >> $2;
		$symextractor | awk '/.*_test/ {print "\t{ "$3", \""$3"\" }," }' >> $2;
		echo -e "\t{ NULL, NULL}\n};" >> $2;;
    *) echo "Unknown OS";
       exit 1;;
esac



