/*
 * Rule: FLP02-C
 * Source: wiki
 * Status: FAIL - Should trigger FLP02-C violation
 */

array[0] = 10.100000 and total is 10.100000
array[1] = 10.100000 and total is 20.200001
array[2] = 10.100000 and total is 30.300001
array[3] = 10.100000 and total is 40.400002
array[4] = 10.100000 and total is 50.500000
array[5] = 10.100000 and total is 60.599998
array[6] = 10.100000 and total is 70.699997
array[7] = 10.100000 and total is 80.799995
array[8] = 10.100000 and total is 90.899994
array[9] = 10.100000 and total is 100.999992
mean is 10.099999
array[0] is not the mean