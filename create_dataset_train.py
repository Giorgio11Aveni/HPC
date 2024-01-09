import numpy as np

# Carica i dati esistenti dal tuo file CSV
data = np.genfromtxt('X_train.csv', delimiter=',')

# Specifica la quantità di rumore da aggiungere
noise_factor = 0.01

# Specifica il numero di righe da aggiungere
numero_righe_da_aggiungere = 999891  # Cambia questo valore a tuo piacimento

# Crea nuove righe con valori simili ma non uguali
new_rows = []
for _ in range(numero_righe_da_aggiungere):
    noise = np.random.uniform(-noise_factor, noise_factor, size=data.shape[1])
    new_row = data[np.random.randint(data.shape[0])] + noise
    new_rows.append(new_row)

# Aggiungi le nuove righe al set di dati esistente
extended_data = np.vstack([data, np.array(new_rows)])

# Salva il nuovo set di dati in un nuovo file CSV
np.savetxt('Nuovo.csv', extended_data, delimiter=',', fmt='%.18e')