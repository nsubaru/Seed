include ksamd64.inc

EXTERNDEF   ExecInstBuf: qword
EXTERNDEF   ResInstBuf: qword

NESTED_ENTRY LoadRegistersStub, _TEXT
    END_PROLOGUE
	MOV RAX, ExecInstBuf
	MOV RBX, ExecInstBuf + 8
	MOV RCX, ExecInstBuf + 16
	MOV RDX, ExecInstBuf + 24
	MOV RSI, ExecInstBuf + 32
	MOV RDI, ExecInstBuf + 40
	MOV RSP, ExecInstBuf + 48
	MOV RBP, ExecInstBuf + 56

	MOV R8,  ExecInstBuf + 64
	MOV R9,  ExecInstBuf + 72
	MOV R10, ExecInstBuf + 80
	MOV R11, ExecInstBuf + 88
	MOV R12, ExecInstBuf + 96
	MOV R13, ExecInstBuf + 104
	MOV R14, ExecInstBuf + 112
	MOV R15, ExecInstBuf + 120
	RET
NESTED_END LoadRegistersStub, _TEXT

NESTED_ENTRY StoreResRegistersStub, _TEXT
    END_PROLOGUE
	MOV ResInstBuf, RAX
	MOV ResInstBuf + 8, RBX
	MOV ResInstBuf + 16, RCX
	MOV ResInstBuf + 24, RDX
	MOV ResInstBuf + 32, RSI
	MOV ResInstBuf + 40, RDI
	MOV ResInstBuf + 48, RSP
	MOV ResInstBuf + 56, RBP

	MOV ResInstBuf + 64, R8
	MOV ResInstBuf + 72, R9
	MOV ResInstBuf + 80, R10
	MOV ResInstBuf + 88, R11
	MOV ResInstBuf + 96, R12
	MOV ResInstBuf + 104, R13
	MOV ResInstBuf + 112, R14
	MOV ResInstBuf + 120, R15
	RET
NESTED_END StoreResRegistersStub, _TEXT

NESTED_ENTRY StoreRegistersStub, _TEXT
    END_PROLOGUE
	MOV ExecInstBuf, RAX
	MOV ExecInstBuf + 8, RBX
	MOV ExecInstBuf + 16, RCX
	MOV ExecInstBuf + 24, RDX
	MOV ExecInstBuf + 32, RSI
	MOV ExecInstBuf + 40, RDI
	MOV ExecInstBuf + 48, RSP
	MOV ExecInstBuf + 56, RBP

	MOV ExecInstBuf + 64, R8
	MOV ExecInstBuf + 72, R9
	MOV ExecInstBuf + 80, R10
	MOV ExecInstBuf + 88, R11
	MOV ExecInstBuf + 96, R12
	MOV ExecInstBuf + 104, R13
	MOV ExecInstBuf + 112, R14
	MOV ExecInstBuf + 120, R15
	RET
NESTED_END StoreRegistersStub, _TEXT

END
