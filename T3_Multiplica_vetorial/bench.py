import matplotlib.pyplot as plt
import time
import os
import sys
import re
from io import StringIO

filename = "matrixMulti.c"
binary = "mm_fma_exe"

def build():
    build_cmd = f"gcc -W -mfma -pthread -o {binary} {filename}"
    execute(build_cmd)

def clean():
    cmd = f"rm {binary}"
    execute(cmd)

def execute(cmd):
    os.system(cmd)

def runTest(thread_list, size_list):
    thread_list = [str(x) for x in thread_list] # passando pra string pro plot
    fig, axs = plt.subplots(len(size_list),sharex=True)
    fig.suptitle('Time(s) x num_threads ')
    old_stdout = sys.stdout
    sys.stdout = StringIO()
    redirected_output = sys.stdout
    for i,size in enumerate(size_list):
        y = [0] * len(thread_list)
               
        for j,thread_count in enumerate(thread_list):
            acc = 0
            num_retries = 5
            for k in range(num_retries):
                 # redirecionando o output para uma string
                
                inicio = time.time() # wall time se usar o clock não computa o sys call
                execute(f"./{binary} {thread_count} {size}")
                acc = acc + (time.time() - inicio)
                
                # only_multi = redirected_output.getvalue()
                # 
                # print(only_multi)
                # print(re.search(r'.*(\w+)',only_multi))
                
            y[j] = acc/num_retries # pegando a média
        axs[i].bar(thread_list,y)
        axs[i].set_title(f"Problem size: {size}")
    sys.stdout = old_stdout # voltando output ao normal
    plt.show()

build()
# runTest([1,2,4,8,16,32],[128,256,512,1024])
runTest([1,2,4,8,16,32],[128,256])
clean()