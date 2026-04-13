# Solution possible: Encodage différentiel [PolyRisc]
.text
ldi r8, 1
ldi r1, n
ld  r1, (r1)        # r1 = n
sub r1, r1, r8      # r1 = n-1 (nombre de différences)
ldi r2, table       # r2 = pointeur sur table[i]
ldi r3, diff        # r3 = pointeur sur diff[i]

# Votre code ici

boucle:
ld  r4, (r2)        # r4 = table[i]
mv  r5, r2
add r5, r5, r8      # r5 = adresse table[i+1]
ld  r5, (r5)        # r5 = table[i+1]
sub r6, r5, r4      # r6 = table[i+1] - table[i]
st  (r3), r6        # diff[i] = r6
add r2, r2, r8      # pointeur table++
add r3, r3, r8      # pointeur diff++
sub r1, r1, r8      # compteur--
brnz boucle

done:
stop

.data
n:     6
table: 3 7 2 9 4 8 0 0 0 0
diff:  0 0 0 0 0 0 0 0 0

# === RÉSULTATS ATTENDUS ===
# diff[0] =  4   (7-3)
# diff[1] = -5   (2-7)
# diff[2] =  7   (9-2)
# diff[3] = -5   (4-9)
# diff[4] =  4   (8-4)
