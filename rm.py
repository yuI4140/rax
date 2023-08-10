import os,sys
argv=sys.argv
argc=len(argv)-1
if argv[1]=="-h":
    print(argv[0]+" linux // this activete the linux option")
    print(argv[0]+"// this activete the Windows option")
if argv[1]=="linux": 
    os.system("python3 ./buildproj/build.py")
    os.system("mv ./buildproj/bool.h ./headers")
    os.system("sudo rm -r ./buildproj/.git")
    os.system("sudo rm -r ./buildproj/)
else:
    os.system("del ./buildproj/.git")
    os.system("del ./buildproj/)
