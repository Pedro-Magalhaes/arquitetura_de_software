import matplotlib.pyplot as plt
import time
import os

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

    for i,size in enumerate(size_list):
        y = [0] * len(thread_list)
               
        for j,thread_count in enumerate(thread_list):
            acc = 0
            num_retries = 5
            for k in range(num_retries):
                inicio = time.time() # wall time se usar o clock não computa o sys call
                execute(f"./{binary} {thread_count} {size}")
                acc = acc + (time.time() - inicio)
            y[j] = acc/num_retries # pegando a média
        axs[i].bar(thread_list,y)
        axs[i].set_title(f"Problem size: {size}")
    plt.show()

build()
runTest([1,2,4,8,16,32],[128,256,512,1024])
clean()