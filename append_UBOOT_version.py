#!/usr/bin/python

import re, fnmatch, os, sys, array

ver_string_head="MSVC00"
ver_string_lib="IL"
ver_string_build_number="000000"




def check_input():
    if not os.path.exists(sys.argv[1]):
        print 'ERROR!! \"%s\" file not existed!!' % sys.argv[1]
        sys.exit(-1)
     

def main():
    print('Append version stamp to UBOOT.bin')
    latest_cl = ['0','0','0','0','0','0','0','0']
    stamp_index = 0
    stamp_flag_found = 0 
    ver_string_chip_customer="C300000"

        
    check_input()

              
    check_sum_odd=0
    check_sum_even=0
    check_sum_version =0
    count=0
    size_without_version = os.path.getsize(sys.argv[1]) - 32
    target_file =open(sys.argv[1],'rb')
    while (count != size_without_version):   
        byte = target_file.read(1)
        if byte == "":
            break;
        
        if count%2 == 1:
            check_sum_odd += ord(byte)
        else:
            check_sum_even += ord(byte)
            
        count+=1
        
        if 0 == stamp_flag_found:
        
		        if   0==stamp_index and 'M'==byte:
		            stamp_index=1
		            
		        elif 1==stamp_index and 'S'==byte:    
		            stamp_index=2
		            
		        elif 2==stamp_index and 'V'==byte:    
		            stamp_index=3 
		            
		        elif 3==stamp_index and 'C'==byte:    
		            stamp_index=4
		            
		        elif 3<stamp_index and stamp_index<22:            
		            if stamp_index>13:
		                latest_cl[stamp_index-14]=byte
		            
		            stamp_index+=1    
		        elif 22==stamp_index:
		            stamp_index=0
		            stamp_flag_found=1        
		        else:
		            stamp_index=0
                            
            
    target_file.close()
    
    target_file =open(sys.argv[1],"r+b")
    target_file.seek(-32,2)
    target_file.write(ver_string_head+ver_string_lib+ver_string_build_number)    
    target_file.write(bytearray(latest_cl))
    print ('ChangeList:'+str(latest_cl))
    target_file.write(ver_string_chip_customer)
    target_file.write('%c' % (check_sum_even & 0xff))
    print ('check_sum_even: 0x%X' % (check_sum_even & 0xff))
    target_file.write('%c' % (check_sum_odd & 0xff))
    print ('check_sum_odd: 0x%X' % (check_sum_odd & 0xff))
    target_file.close()

    count = 0
    target_file =open(sys.argv[1],"r+b")
    target_file.seek(-32, 2)
    while 1:   
        byte = target_file.read(1)
        if count == 31 or byte == "":
            break;
        check_sum_version += ord(byte)
        count+=1
        
    check_sum_version &= 0xff
    check_sum_version = (0x100 - check_sum_version) & 0xff
    target_file.close()

    target_file =open(sys.argv[1],"r+b")
    target_file.seek(-1,2)
    target_file.write('%c' % check_sum_version)
    print ('check_sum_version: 0x%X' % check_sum_version)
    target_file.close()
    


if __name__ == '__main__':
    main()

    

