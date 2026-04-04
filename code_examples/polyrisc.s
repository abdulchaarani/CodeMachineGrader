.text
ldi r8, 1
ldi r1, valeur
ld  r1, (r1)        # r1 = valeur décimale
ldi r2, masque
ld  r2, (r2)        # r2 = 0xF
ldi r3, hex         # r3 = adresse base de hex[]
ldi r4, nb_nibbles
ld  r4, (r4)        # r4 = compteur (4 nibbles)

# Votre code ici

boucle:
and r5, r1, r2      # r5 = nibble = r1 & 0xF
st  (r3), r5        # hex[i] = nibble
shr r1, r1          # décaler de 4 bits (4x shr)
shr r1, r1
shr r1, r1
shr r1, r1
add r3, r3, r8      # pointeur++
sub r4, r4, r8      # compteur--
brnz boucle

done:
stop

.data
valeur:     6699    # 0x1A2B
masque:     15      # 0xF
nb_nibbles: 4
hex:        0 0 0 0

# === RÉSULTATS ATTENDUS ===
# hex[0] = 11  (B, nibble de poids faible)
# hex[1] =  2  (2)
# hex[2] = 10  (A)
# hex[3] =  1  (1)
