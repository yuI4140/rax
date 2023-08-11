import os,platform
if platform.system()=="Linux": 
    os.system("python3 ./buildproj/build.py")
    os.system("mv ./buildproj/bool.h ./headers")
    os.system("sudo rm -r ./buildproj/.git")
    os.system("sudo rm -r ./buildproj/")
elif platform.system()=="Windows":
    os.system("del ./buildproj/.git")
    os.system("del ./buildproj/")
else:
    print(platform.system()+" not supported")
