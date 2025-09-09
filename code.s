.text
# Mem[val + offset] 
tally:
lda pointeur
ldi
add offset
st temp
lda temp
ldi
add one
sti

incrementcount:
ld count
add one
st count
sub n
brz end

# incrementer pointeur
incrementptr:
lda pointeur
adda one
sta pointeur
br tally

end:
stop

# NE RIEN MODIFIER EN DESSOUS DE CETTE LIGNE
.data
#nums
a: 1   # nums[0]
b: 2   # nums[1]
c: 2   # nums[2]
d: 3   # nums[3]
e: 1   # nums[n - 1]

n: 5 # nbr elements dans nums
# NE RIEN MODIFIER AU DESSUS DE CETTE LIGNE

one: 1
count: 0

# adresse debut du tableau
pointeur: 18 #  Peut être codé en dur

# offset vers une portion inutilisée de la memoire principale 
offset: 100
temp: 0
