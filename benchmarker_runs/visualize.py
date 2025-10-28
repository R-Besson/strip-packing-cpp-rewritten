# pip install plotly pandas scipy

import plotly.graph_objects as go
import pandas as pd
import numpy as np
from scipy.interpolate import griddata

df = pd.read_csv("summary_results.csv")

x = df['N']
y = df['H/W']
z = df['H/OPTH']
xi = np.linspace(x.min(), x.max(), 100)
yi = np.linspace(y.min(), y.max(), 100)
xi, yi = np.meshgrid(xi, yi)

zi = griddata((x, y), z, (xi, yi), method='linear')
fig = go.Figure(data=[go.Surface(
    x=xi,
    y=yi,
    z=zi,
    colorscale='Plasma',
    colorbar=dict(title='α = H/OPT(I) (Result / Optimal)'),
    opacity=0.9
)])

fig.update_layout(
    title='Plotting worst α as a function of rectangle count and Height/Width ratio of the optimal solution',
	font_family="monospace",
	font_size=14,
    scene=dict(
        xaxis_title='N (number of rectangles)',
        yaxis_title='OPT(I)/W (Optimal Height / Width of strip)',
        zaxis_title='α'
    ),
    width=2000,
    height=2000
)

fig.show()