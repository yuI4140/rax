import os
import sys
import platform
import glob

argc = len(sys.argv) - 1
argv = sys.argv

LINUX_ADV = """
For files and folders in .git,
there is no fix to remove .git that doesn't get
removed without sudo. If you have a better solution, please
create an issue.\n
"""

if platform.system() == "Linux":
    if argc != 0 and argv[1] == "--nla":
        os.system("python3 ./buildProj/build.py")
        os.system("mv ./buildProj/bool.h ./headers")
        os.system("sudo rm -r ./buildProj/.git")
        os.system("sudo rm -r ./buildProj/")
    else:
        print("Disable LINUX_ADV by setting the flag: " + argv[0] + " --nla")
        print(LINUX_ADV)
        os.system("python3 ./buildProj/build.py")
        os.system("mv ./buildProj/bool.h ./headers")
        os.system("sudo rm -r ./buildProj/.git")
        os.system("sudo rm -r ./buildProj/")
elif platform.system() == "Windows":
    os.system("py .\\buildproj\\build.py")
    files = glob.glob(".\\buildproj\\*[!py]")
    for f in files:
        os.rename(f, os.path.basename(f))
    os.system("rmdir /s /q .\\buildproj")
    os.system("move ./bool.h ./headers")
else:
    print(platform.system() + " is not supported")
