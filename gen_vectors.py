NUM_VECTORS = 256
HAS_ERROR_CODE = [0x8, 0xa, 0xb, 0xc, 0xd, 0xe, 0x11, 0x30]


HEADER = '''bits 32
section .text
extern trampoline

'''


def main():
    vectors = ''
    for i in range(NUM_VECTORS):
        vectors += 'vector{trapno}:\n'.format(trapno=i)
        if i not in HAS_ERROR_CODE:
            vectors += '    push 0\n'  # push dummy error code
        vectors += '''    push {trapno}
    pushad
    push esp
    call trampoline
    add esp, 4
    popad
    add esp, 8
    iret
'''.format(trapno=i)

    vector_table = '''section .data
global vector_table
align 4
vector_table:
'''
    for i in range(NUM_VECTORS):
        vector_table += '\tdd vector{trapno}\n'.format(trapno=i)

    with open("vectors.asm", "w") as f:
        f.write(HEADER + vectors + vector_table)


if __name__ == '__main__':
    main()
