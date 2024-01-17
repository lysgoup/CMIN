cmin: cmin.c
	gcc -o cmin cmin.c

debug: cmin.c
	gcc -o cmin cmin.c -DDEBUG

unsigned: test_cmin.c
	gcc -o test_cmin test_cmin.c

jsmn:
	./test_cmin -i ./OperatingSystem/jsmn/testcases/crash.json -m "AddressSanitizer: heap-buffer-overflow" -o reduced ./OperatingSystem/jsmn/jsondump

libxml2:
	./test_cmin -i ./OperatingSystem/libxml2/testcases/crash.xml -m "SEGV on unknown address" -o reduced "./OperatingSystem/libxml2/xmllint --recover --postvalid -"
	
balance:
	./test_cmin -i ./OperatingSystem/balance/testcases/fail -m msg -o reduced ./OperatingSystem/balance/balance

libpng:
	./test_cmin -i ./OperatingSystem/libpng/crash.png -m "MemorySanitizer: use-of-uninitialized-value" -o reduced ./OperatingSystem/libpng/libpng/test_pngfix
