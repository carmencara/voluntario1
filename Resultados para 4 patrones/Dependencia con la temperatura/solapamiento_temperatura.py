import matplotlib.pyplot as plt

# Leer los datos del archivo
temperatura = []
solapamiento_aleatorio = []
solapamiento_deformado = []

# Crear la figura y las subfiguras
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 5))

with open('solapamientomax_deformado-aleatorio.txt', 'r') as f:
    for linea in f:
        valores = linea.strip().split()
        temperatura.append(float(valores[0]))
        solapamiento_aleatorio.append(float(valores[1]))
        solapamiento_deformado.append(float(valores[2]))

# SOLAPAMIENTO ALEATORIO --------------------------------------------------------------
ax1.plot(temperatura, solapamiento_aleatorio, linestyle='-', marker='.', color='blue')
ax1.set_xlabel('Temperatura')
ax1.set_xscale('log')
ax1.set_ylabel('Solapamiento máximo')
ax1.set_ylim([-0.12,1.12])
ax1.set_title('Configuración inicial: aleatoria', fontsize='10.5')

# Insertar una línea punteada horizontal en y = 0.75 (solapamiento mínimo)
ax1.axhline(y=0.75, linestyle='--', color='black')

# SOLAPAMIENTO DEFORMADO --------------------------------------------------------------
ax2.plot(temperatura, solapamiento_deformado, linestyle='-', marker='.', color='blue')
ax2.set_xlabel('Temperatura')
ax2.set_xscale('log')
ax2.set_ylim([-0.12,1.12])
ax2.get_yaxis().set_ticklabels([])
ax2.set_title('Configuración inicial: patrón 1 deformado al 50%', fontsize='10.5')

# Insertar una línea punteada horizontal en y = 0.75 (solapamiento mínimo)
ax2.axhline(y=0.75, linestyle='--', color='black')

# Ajustar los espacios entre subfiguras
plt.subplots_adjust(wspace=0.05)

# Mostrar la gráfica
plt.show()
#plt.savefig('solapamiento_4patrones_logtemperatura.png', format='png', dpi=300, bbox_inches='tight')