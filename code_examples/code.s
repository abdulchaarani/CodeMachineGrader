# Solution possible: Compter l’occurrence d’un nombre dans un tableau [MA]
.text
# pointeur = adresse début de num
lea nums
sta pointeur

# Mem[pointeur] == target ?
compare:
lda pointeur
ldi
sub target
brnz 9 # incrementCount, works with literals

# ++compteur
addone:
ld result
add one
st result

# incrementer decompte
increment_count:
ld count
add 23 # one, works with literals
st count
sub n
brz end

# incrementer pointeur
incrementPtr:
lda pointeur
adda one
sta pointeur
br compare

end:
ld result
stop

.data
n: 6 # nbr elements dans nums NE PAS MODIFIER LE NOM DE CETTE VARIABLE

target: 2 # chiffre à compter NE PAS MODIFIER LE NOM DE CETTE VARIABLE
result: 0

one: 1
count: 0

pointeur: 21

nums: 1   # nums[0] NE PAS MODIFIER LE NOM DE CETTE VARIABLE
2   # nums[1]
3   # nums[2]
2   # nums[3]
2   # nums[4]
4   # nums[n - 1]
