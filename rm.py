import os,platform, glob
if platform.system()=="Linux": 
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
