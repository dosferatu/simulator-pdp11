;Index and index deferred test

MOV #16356, R4
MOV #2964, R3
MOV R3, 8(R4)
MOV #523, R5
MOV R5, 10(R4)
MOV 10(R4), R3
MOV 8(R4), R5
MOV #30, R2
MOV R2, @8(R4)
MOV (R4), R3	;R3 = 2964
MOV @10(R4), R3	;R3 = 30