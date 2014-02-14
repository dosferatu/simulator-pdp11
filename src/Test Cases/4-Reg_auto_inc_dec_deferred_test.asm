;Test auto inc/dec deferred

MOV #1, R0
MOV #8348, R1
MOV #16820, (R1)+
MOV #3820, (R1)+
MOV #4260, (R1)
MOV #8348, R1
MOV R0, @(R1)+
MOV R0, @(R1)+
MOV R0, @(R1)+
MOV @-(R1), R2
MOV @-(R1), R3
MOV @-(R1), R4

;R2, R3, and R4 = 1