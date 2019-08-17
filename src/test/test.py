#!/usr/bin/env python3
"""
bitnes test suite.
compares bitnes.log to nestest log http://www.qmtpro.com/~nes/misc/nestest.log
"""
import re

__author__ = "Matthias Linford"
__version__ = "0.0.1"
__license__ = "MIT"

ADDRESS = 0;

INSTRUCTION = 1;

def print_Difference(bit_nes_msg, nes_test_msg, nes_line, bit_line):
    print("Difference found!")
    print("--------------Bit-nes Output--------------")
    print(" Line: " + str(bit_line+1) + " " + bit_nes_msg)
    print("--------------NesTest Output--------------")
    print(" Line: " + str(nes_line+1) + " " + nes_test_msg)

def test():
    """ Main entry point of the app """
    bit_nes_file = open("bitnes.log", 'r')
    nes_test_file = open("nestest.log", 'r')

    bit_nes_log = list(bit_nes_file)
    nes_test_log = list(nes_test_file)

    #let's sync the instruction lines
    bit_nes_index = 5
    nes_test_index = 1
    bit_nes_length = len(bit_nes_log)-1
    nes_test_length = len(nes_test_log)-1

    regex_reg_data = ["(A:).*","(X:).*","(Y:).*","(P:).*","(SP:).*","(CYC:).*"]

    #now we test the output
    no_error = True
    while(no_error):
        bit_nes_line = bit_nes_log[bit_nes_index].split()
        nes_test_line = nes_test_log[nes_test_index].split()

        nes_test_data = []
        bit_nes_data = []

        # find nestest's register output because it is not always in the same place
        for expr in regex_reg_data:
            regex = re.compile(expr)
            temp = [m.group(0) for l in nes_test_line for m in [regex.search(l)] if m]
            nes_test_data += temp[0]

            temp = [m.group(0) for l in bit_nes_line for m in [regex.search(l)] if m]
            bit_nes_data += temp[0]

        if bit_nes_line[ADDRESS] != nes_test_line[ADDRESS] or bit_nes_line[INSTRUCTION] != nes_test_line[INSTRUCTION]:
            no_error = False
            print_Difference(bit_nes_log[bit_nes_index], nes_test_log[nes_test_index], nes_test_index, bit_nes_index)

        for i in range(25):
            if nes_test_data[i] != bit_nes_data[i]:
                no_error = False
                print_Difference(bit_nes_log[bit_nes_index], nes_test_log[nes_test_index], nes_test_index, bit_nes_index)
                break

        bit_nes_index += 1
        nes_test_index += 1

        if bit_nes_index == bit_nes_length-1 or nes_test_index == nes_test_length-1:
            print("Passed")
            no_error = False
            break



if __name__ == "__main__":
    test()
