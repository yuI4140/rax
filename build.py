import os
os.system("mkdir build bin src headers third-party")
win="""
gcc -Wall -Wextra -Werror -o ../bin/main ../src/main.c
"""
linux="""    
gcc -Wall -Wextra -Werror -o ../bin/main ../src/main.c
x86_64-w64-mingw32-gcc -Wall -Wextra -Werror -o ../bin/main ../src/main.c
"""
os.system("touch build/build.sh")
os.system("touch build/build.bat")
os.system("echo {}>build/build.sh".format(linux))
os.system("echo {}>build/build.bat".format(win))