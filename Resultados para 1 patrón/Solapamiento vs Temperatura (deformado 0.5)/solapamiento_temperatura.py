import matplotlib.pyplot as plt

# Leer los datos del archivo
temperatura = []
solapamiento = []

with open('solapamiento_temperatura.txt', 'r') as f:
    for linea in f:
        valores = linea.strip().split()
        temperatura.append(float(valores[0]))
        solapamiento.append(float(valores[1]))

# Crear la figura y los ejes
fig, ax = plt.subplots()

# Trazar los datos
ax.plot(temperatura, solapamiento, linestyle='-', marker='.', color='blue')

# Insertar una línea punteada horizontal en y = 0.75 (solapamiento mínimo)
ax.axhline(y=0.75, linestyle='--', color='black')

# Configurar los ejes
ax.set_xlabel('Temperatura')
ax.set_ylabel('Solapamiento máximo')
ax.set_xscale('log')

# Mostrar la gráfica
plt.show()
#plt.savefig('solapamiento_logtemperatura.png', format='png', dpi=300, bbox_inches='tight')