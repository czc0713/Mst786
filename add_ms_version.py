#!/usr/bin/python

import re, fnmatch, os, sys

matches = []
match_ptn=("*.c", "*.h", "*.cpp", "*.s", "*.reg", "*.bib")

ver_string_head="MSVC00"
ver_string_lib="LX"
ver_string_build_number="000000"



def get_all_source_file(file_list):
    for file in file_list:
        if os.path.isdir(file) == True:
            if not os.path.abspath(file).lower().endswith("scaler\\pq") and not os.path.abspath(file).lower().endswith("\\obj") and not os.path.abspath(file).lower().endswith("mm\\src") and not os.path.abspath(file).lower().endswith("asiclib") :
#                print(os.path.abspath(file).lower())
                cur_dir=os.path.abspath(".")
                os.chdir(file)
                get_all_source_file(os.listdir("."))
                os.chdir(cur_dir)
        else: 
            for ptn in match_ptn:
                if fnmatch.fnmatch(file, ptn):
                    matches.append(os.path.abspath(file))
                    break                    
                       

def get_all_changelist(files):
    ptn = re.compile(r"\$Change: (\d+) \$")
    chglst = []
    error = 0
    
    for fn in files:
        #f = open(fn, "rb+")
        txt = open(fn, "r").read()
        sch = ptn.search(txt)
        if sch:
            chglst.append(int(sch.group(1)))
            # print fn, ':', chglst[-1]
        else:
            #f.seek(0)
            #f.write("// $Change: 0 $\n\n"+txt)
            print 'ERROR!!!! \'', fn, '\' does not have changelist keyword!!'
            error+=1
    if error != 0:
        sys.exit(-1) 

    return chglst


def get_latest_changelist(files):
    return max(get_all_changelist(files))


if __name__ == '__main__':
    matches = []
    ver_string_chip_customer="C300000"

        
#    if os.path.exists(sys.argv[1]):
#        os.remove(sys.argv[1])
    
    src_list = sys.argv[2:]
#    get_all_source_file(src_list)
#    latest_cl=get_latest_changelist(matches)
    latest_cl=get_latest_changelist(src_list)
    
    version_file=open(sys.argv[1],'w')
    version_file.seek(0,2)
    
    version_file.write("// $Change: %d $\n" % latest_cl)    
    version_file.write('#define MS_CHANGELIST ' + str(list('%08d' % latest_cl)).replace('[','{').replace(']','}'))
    version_file.close()
    print ('@CL:%d' % latest_cl)
    
