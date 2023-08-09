import os,sys,platform, glob
argc=len(sys.argv)-1
argv=sys.argv
LINUX_ADV="""
For files and folders in .git 
there is no fix to remove .git that doesn't be 
remove with sudo. If yoou have a better solution please
make a issue.\n"""
if platform.system()=="Linux": 
    if argv[1]=="--nla":
        os.system("python3 ./buildproj/build.py")
        os.system("mv ./buildproj/bool.h ./headers")
        os.system("sudo rm -r ./buildproj/.git")
        os.system("sudo rm -r ./buildproj/") 
else: 
    print("Desable LINUX_ADV setting the flag:"+argv[0]+" --nla") 
    print(LINUX_ADV)
    os.system("python3 ./buildproj/build.py")
    os.system("mv ./buildproj/bool.h ./headers")
    os.system("sudo rm -r ./buildproj/.git")
    os.system("sudo rm -r ./buildproj/") 
elif platform.system()=="Windows":
    os.system("py .\\buildproj\\build.py")
    files = glob.glob(".\\buildproj\\*[!py]")
    for f in files:
        os.rename(f, os.path.basename(f))
    os.system("rmdir /s /q .\\buildproj")
    os.system("move ./bool.h ./headers")
else:
    print(platform.system()+" not supported")
