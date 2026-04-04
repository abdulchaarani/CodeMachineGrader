.text
load_variables:
ldi r2, one
ld r1, (r2) # 1
ld r11, (r2) # k
ld r21, (r2) # 2^k

ldi r2, dec
ld r30, (r2)
ld r31, (r2) # dec

ldi r10, bin # bin ptr

find_n:
add r5, r5, r1
shr r30, r30
brnz find_n
mv r6, r5

# i = n - 1
add r20, r10, r5
sub r20, r20, r1 # i

# dec & 1 == 1 ?
check:
and r7, r31, r1
brz next_power

is_bit_high:
st (r20), r1 // bin[i] = 1
 
next_power:
shr r31, r31
sub r20, r20, r1 # i--
sub r6, r6, r1 # n--
brgez check

end:
stop
.data
one: 1
################## NE RIEN PLACER EN DESSOUS DE  LIGNE ##################
# ⚠️ Ne pas modifier le NOM de ces variables. Vous pouvez modifier leurs valeurs.
dec: 42
bin: 0 # attendu 1 0 1 0 1 0 
# Vous pouvez etendre bin, mais ne creez pas d'autdec variables das en dessous