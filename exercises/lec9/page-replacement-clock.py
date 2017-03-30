#! /usr/bin/env python

import sys
from optparse import OptionParser
import random
import math


def hfunc(index):
    if index == -1:
        return 'MISS'
    else:
        return 'HIT '


def vfunc(victim):
    if victim == -1:
        return '-'
    else:
        return victim


def find_ind(memory, k):
    for i in range(len(memory)):
        if memory[i][0] == k:
            return i
    return -1

#
# main program
#
parser = OptionParser()
parser.add_option('-a', '--addresses', default='-1',   help='a set of comma-separated pages to access; -1 means randomly generate',
                  action='store', type='string', dest='addresses')
parser.add_option('-p', '--policy', default='CLOCK-e',    help='replacement policy: FIFO, LRU, OPT, CLOCK',
                  action='store', type='string', dest='policy')
parser.add_option('-b', '--clockbits', default=1,      help='for CLOCK policy, how many clock bits to use',
                  action='store', type='int', dest='clockbits')
parser.add_option('-f', '--pageframesize', default='3',    help='size of the physical page frame, in pages',
                  action='store', type='string', dest='pageframesize')
parser.add_option('-s', '--seed', default='0',         help='random number seed',
                  action='store', type='string', dest='seed')
parser.add_option('-N', '--notrace', default=False,    help='do not print out a detailed trace',
                  action='store_true', dest='notrace')
parser.add_option('-c', '--compute', default=False,    help='compute answers for me',
                  action='store_true', dest='solve')

(options, args) = parser.parse_args()

print 'ARG addresses', options.addresses
print 'ARG policy', options.policy
print 'ARG clockbits', options.clockbits
print 'ARG pageframesize', options.pageframesize
print 'ARG seed', options.seed
print 'ARG notrace', options.notrace
print ''

addresses = str(options.addresses)
pageframesize = int(options.pageframesize)
seed = int(options.seed)
policy = str(options.policy)

notrace = options.notrace
clockbits = int(options.clockbits)

random.seed(seed)

addrList = []
addrList = addresses.split(',')

if options.solve is False:
    print 'Assuming a replacement policy of %s, and a physical page frame of size %d pages,' % (policy, pageframesize)
    print 'figure out whether each of the following page references hit or miss'

    for n in addrList:
        if n.lower() == n:
            accessStr = 'Read'
        else:
            accessStr = 'Write'
        print '%s:\t %s  Hit/Miss?  State of Memory?' % (accessStr, n)
    print ''

else:
    if notrace is False:
        print 'Solving...\n'

    # init memory structure
    count = 0
    memory = []         # elements of memory should be of shape [key, edit_bit, access_bit]
    mem_set = set()
    hits = 0
    miss = 0
    clock_pt = 0

    if policy == 'CLOCK-e':
        leftStr = 'Left'
        riteStr = 'Right '
    else:
        print 'Policy %s is not yet implemented' % policy
        exit(1)

    cdebug = True

    # need to generate addresses
    addrIndex = 0
    for nStr in addrList:
        # first, lookup
        n = nStr.lower()
        edit = int(n != nStr)
        accessStr = 'Write' if edit else 'Read'
        idx = find_ind(memory, n)
        if idx != -1:
            hits = hits + 1
        else:
            miss = miss + 1

        victim = -1
        if idx != -1:    # hits
            memory[idx][2] = 1      # access
            memory[idx][1] = edit   # edit
        else:           # miss
            if len(memory) < pageframesize:     # still have space
                memory.append([n, edit, 1])
            else:                               # we should find something to replace
                while victim == -1:
                    access_bit = memory[clock_pt][2]
                    edit_bit = memory[clock_pt][1]
                    if access_bit == 0 and edit_bit == 0:   # replace this
                        victim = memory[clock_pt][0]
                        memory[clock_pt] = [n, edit, 1]
                    elif access_bit == 1 and edit_bit == 0:
                        memory[clock_pt][2] = 0
                    elif access_bit == 0 and edit_bit == 1:
                        memory[clock_pt][1] = 0
                    else:
                        memory[clock_pt][2] = 0
                    clock_pt = (clock_pt + 1) % pageframesize
        if notrace is False:
            print '%s:\t %s  %s %s -> %s <- %s Replaced:%s [Hits:%d Misses:%d]' % (accessStr, n, hfunc(idx), leftStr, memory, riteStr, vfunc(victim), hits, miss)
        addrIndex = addrIndex + 1

    print ''
    print 'FINALSTATS hits %d   misses %d   hitrate %.2f' % (hits, miss, (100.0 * float(hits)) / (float(hits) + float(miss)))
    print ''
