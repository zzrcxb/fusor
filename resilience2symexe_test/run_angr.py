#!/usr/bin/env python

import angr
import claripy
import os
import sys
import time
import argparse
import subprocess

from termcolor import colored


def run_symexe(path, argv_size=2, show_bytes=True, show_model=False):
    sym_argv = claripy.BVS('sym_argv', argv_size * 8)

    try:
        p = angr.Project(path)
    except:
        print (colored('Invalid path: \"' + path + '\"', 'red'))
        return None

    state = p.factory.entry_state(args=[p.filename, sym_argv])
    pg = p.factory.simgr(state)

    # while len(pg.active) > 0:
    #     pg.step()
    pg.run()

    print (colored('[*] Paths found: ' + str(len(pg.deadended)), 'white'))

    results = []
    for dd in pg.deadended:
        res = dd.solver.eval(sym_argv, cast_to=bytes)
        results.append(res)
        print (colored('[+] New Input:  {}'.format(res), 'green')),
        print ('')
        if show_model:
            print (colored(str(dd.se.constraints), 'yellow'))

    errored = False
    for err in pg.errored:
        print (colored('[-] Error: ' + repr(err), 'red'))
        with open('errors.txt', 'a') as f:
            f.write(path + repr(err) + '\n')
        errored = True

    return results, errored


if __name__ == '__main__':
    print ('\n\n')
    parser = argparse.ArgumentParser()
    parser.add_argument("-l", "--length", type=int, help="Stdin size")
    parser.add_argument("file_path", type=str, help="Binary path")
    args = parser.parse_args()

    bin_path = args.file_path

    if args.length is None:
        lenSolution = 8
    else:
        lenSolution = args.length
    results, errored = run_symexe(bin_path, lenSolution, show_model=False)
    print (colored('[*] Analysis completed\n', 'green'))

