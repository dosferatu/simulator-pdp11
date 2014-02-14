;Test autoincrement and autodecrement

MOV #6038, R3
MOV #5, (R3)+	;R3 should go from 6038 to 6040
MOV #65535, (R3)+	;R3 goes from 6040 to 6042
MOV -(R3), R4	;R4 should get 65535 and R3 goes from 6042 to 6040
MOV -(R3), R5	;R5 gets 5, R3 goes from 6040 to 6038

;Final: R3=6038, R4=65535, and R5=5
