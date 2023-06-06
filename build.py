import os

os.system("mkdir build bin src headers third-party")
with open("src/main.c", "w") as file:
    file.write("#include <stdio.h>\n\nint main() {\n")
    file.write("\tprintf(\"Hello, World!\\n\");\n")
    file.write("\treturn 0;\n}")

win = """
gcc -Wall -Wextra -Werror -o ../bin/main ../src/main.c
"""

linux = """    
gcc -Wall -Wextra -Werror -o ../bin/main ../src/main.c
x86_64-w64-mingw32-gcc -Wall -Wextra -Werror -o ../bin/main ../src/main.c
"""

os.system("touch build/build.sh")
os.system("touch build/build.bat")

with open("build/build.sh", "w") as sh_file:
    sh_file.write(linux)

with open("build/build.bat", "w") as bat_file:
    bat_file.write(win)