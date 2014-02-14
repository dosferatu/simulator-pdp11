;Single operand test
MOV #20, R0
CLR R0	;R0 = 0, Z gets set, all other flags cleared
MOV #-2, R0	;R0 = -2
INC R0	;R0 = -1, N gets set, all
INC R0	;R0 = 0, Z gets set
MOV #32767, R0
INC R0	;V flag should be set
MOV #2, R0
DEC R0	;R0 = 1
DEC R0	;R0 = 0, Z flag set
DEC R0	;R0 = -1, N flag set
MOV #-32768, R0
DEC R0	;C should be set? (says C in manual, but seems like it should be V flag)
MOV #531, R0	;R0 = 531
NEG R0	;R0 = -531 N set
NEG R0	;R0 = 531, N cleared
MOV #-32768, R0	;R0 = -32768
NEG R0	;R0 = -32768, V flag should be set
TST R0	;C and V cleared, N set
MOV #0, R0
TST R0	;N cleared, Z set
MOV #-354, R0
COM R0	;R0 = 353, N cleared, V cleared, C set
COM R0	;R0 = -354, N set, V cleared, C set
ADC R0	;R0 = -353,
;Multiple precision operations
MOV #6023, R0
MOV #32766, R1
MOV #2, R2
ADD R0, R1	;C is set
ADC R2	;R2 = 3
MOV #32766, R1
ADD R0, R1	;C is set
SBC R2	;R2 = 2
MOV #1, R1
ROR R1	;R1 = 0, C is set, Z is set, N is cleared, V is set
ROR R1	;R1 = -32768, C is cleared, N is set, V is set
MOV #1, R1
ROR R1	;C is set
ROL R2	;R2 = 5, C cleared
MOV #3, R0
SWAB R0	;R0 = 768, Z set
MOV #960, R0
SWAB R0	;R0 = -16381, all flags cleared
SWAB R0 ;R0 = 960, N set
MOV #-6, R0
ASR R0	;R0 = -3, N set, C cleared, V set
ASR R0	;R0 = -1, C set, N set, V cleared
ASR R0	;R0 = 0, Z flag set, C flag set, N cleared, V set
MOV #-6, R0
ASL R0	;R0 = -12, N is set, C is set, V cleared
MOV #-32768, R0
ASL R0	;R0 = 0, Z is set, N is cleared, C set, V set
