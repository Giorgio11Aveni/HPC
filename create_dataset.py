import csv
import random

# Imposta il seme per la riproducibilità
random.seed(42)

# Genera una matrice 1000x4 di numeri float casuali
data = [[random.uniform(1.0, 10.0) for _ in range(4)] for _ in range(1000)]

# Scrivi i dati in un file CSV
with open('numeri_casuali.csv', 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    for row in data:
        writer.writerow(row)
