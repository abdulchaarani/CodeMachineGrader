.text
# i pointe vers le debut du tableau
# i = adr(nums)
load_pointeur_i:
lea nums
sta pointeur_i

# j pointe vers la fin du tableau
# j = (adr(nums) + n) - 1
load_pointeur_j:
lea nums
adda n
suba one
sta pointeur_j

swap:
# temp = nums[j]
lda pointeur_i
ldi
st temp
# nums[j] = nums[i]
lda pointeur_j
ldi
lda pointeur_i
sti
# nums[i] = temp
lda pointeur_j
ld temp
sti

next_element:
ld pointeur_i
add one
st pointeur_i
ld pointeur_j
sub one
st pointeur_j

check_end:
ld pointeur_j
sub pointeur_i
brz end

sub minus_one
brnz swap

end:
stop

.data
one: 1
minus_one: -1

pointeur_i: 0
pointeur_j: 0

temp: 0
##############
n: 5 # nbr elements dans nums

#nums
nums: 4   # nums[0]
0  # nums[1]
3   # nums[2]
5   # nums[3]
1   # nums[n - 1]2