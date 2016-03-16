#!/usr/bin/env python3

import subprocess

expected = {
        ";" : "SEMI",
        "," : "COMMA",
        "=" : "ASSIGNOP",
        "<" : "RELOP-LT",
        "<=" : "RELOP-LE",
        ">" : "RELOP-GT",
        ">=" : "RELOP-GE",
        "==" : "RELOP-EQ",
        "!=" : "RELOP-NE",
        "+" : "PLUS",
        "-" : "MINUS",
        "*" : "STAR",
        "/" : "DIV",
        "&&" : "AND",
        "||" : "OR",
        "." : "DOT",
        "!" : "NOT",
        "(" : "LP",
        ")" : "RP",
        "[" : "LB",
        "]" : "RB",
        "{" : "LC",
        "}" : "RC",
}

def output_result(input_, expected, output):
    passed = False

    if output.startswith(expected):
        print('[Pass]', end=' ')
        passed = True
    else:
        print('[Fail]', end=' ')
        passed = False

    print('input="{}", expected="{}"'.format(input_, expected))
    print('\t', output)

    return passed

def test_single_token():

    for token in expected:

        filename = 'test-single-token-{}.cmm'.format(
                expected[token])
        cmd = "echo '{}' > {}".format(token, filename)
        subprocess.call(cmd, shell=True)
    
        test_cmd = '../../Code/lexer < {}'.format(filename)
        proc = subprocess.Popen(test_cmd, shell=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE)
        out = proc.stdout.read().decode('utf-8').rstrip('\n')

        passed = output_result(token, expected[token], out)


    

if __name__ == '__main__':

    test_single_token()
