	INITIAL_GP = 0x10008000 
INITIAL_SP = 0x7ffffffc
# system call service number
print_int = 1 
print_string = 4
stop_service = 99
.text
main:
addi    $sp, $sp, -32
sw $ra, 20($sp)
sw $fp, 16($sp)
addiu   $fp, $sp, 28
addi $t0, $zero, 10
addi $t1, $zero, 20
add $s0, $zero, $t1
addi $s1, $s0, 0
add $s0, $zero, $t0
addi $s2, $s0, 0
add $s0, $s1, $s2
addi $s0, $zero, 40
addi $s1, $s0, 0
add $s0, $zero, $t1
addi $s2, $s0, 0
add $s0, $s1, $s2
add $t2, $zero, $s0
or $v0, $t2, $zero
lw $ra, 20($sp)
lw $fp, 16($sp)
addi $sp, $sp, 32
jr  $ra

.data

