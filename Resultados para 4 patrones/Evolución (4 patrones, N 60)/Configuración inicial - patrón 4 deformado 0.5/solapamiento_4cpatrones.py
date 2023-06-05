import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.axes_grid1.inset_locator import zoomed_inset_axes, mark_inset

# Cargar los datos del archivo
data = np.loadtxt('solapamiento_cultivos.txt')

# Extraer las columnas
tiempo = data[:, 0]
solapamiento_1 = data[:, 1]
solapamiento_2 = data[:, 2]
solapamiento_3 = data[:, 3]
solapamiento_4 = data[:, 4]

# Crear la gráfica principal
fig, ax = plt.subplots()
ax.plot(tiempo, solapamiento_1, label='Patrón 1')
ax.plot(tiempo, solapamiento_2, label='Patrón 2')
ax.plot(tiempo, solapamiento_3, label='Patrón 3')
ax.plot(tiempo, solapamiento_4, label='Patrón 4')

# Configurar los ejes y la leyenda de la gráfica principal
ax.set_xlabel('Tiempo (PMC)')
ax.set_ylabel('Solapamiento')
ax.legend(loc='upper right')

# Añadir el zoom
axins = zoomed_inset_axes(ax, zoom=2, loc='center right')
axins.plot(tiempo, solapamiento_1, label='Patrón 1')
axins.plot(tiempo, solapamiento_2, label='Patrón 2')
axins.plot(tiempo, solapamiento_3, label='Patrón 3')
axins.plot(tiempo, solapamiento_4, label='Patrón 4')

# Establecer los límites del eje x e y en el zoom
axins.set_xlim(-2, 7.5)
axins.set_ylim(-0.15, 0)

# Marcar el área de zoom en la gráfica principal
mark_inset(ax, axins, loc1=2, loc2=4, fc="none", ec="0.5")

# Mostrar y guardar la figura
#plt.show()
plt.savefig('solapamiento_4patrones.png', format='png', dpi=300, bbox_inches='tight')
