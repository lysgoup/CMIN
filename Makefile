cmin: cmin.c
	gcc -o cmin cmin.c

debug: cmin.c
	gcc -o cmin cmin.c -DDEBUG

libxml2:
	./cmin -i ./OperatingSystem/libxml2/testcases/crash.xml -m "SEGV on unknown address" -o reduced "./OperatingSystem/libxml2/xmllint --recover --postvalid -"
	
balance:
	./cmin -i ./OperatingSystem/balance/testcases/fail -m msg -o reduced ./OperatingSystem/balance/balance

libpng:
	./cmin -i ./OperatingSystem/libpng/crash.png -m "MemorySanitizer: use-of-uninitialized-value" -o reduced ./OperatingSystem/libpng/libpng/test_pngfix
