# pip install plotly pandas scipy

import plotly.graph_objects as go
import pandas as pd
import numpy as np
from scipy.interpolate import griddata

df = pd.read_csv("summary_results.csv")

# ---------------------------------------------------------------- surface

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
	title='Plotting average α as a function of rectangle count and Height/Width ratio of the optimal solution',
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

d = df[(df['H/W'] > 0) & (df['H/OPTH'] > 1) & (df['N'] > 0)].copy()
d['excess'] = d['H/OPTH'] - 1.0

conv = go.Figure()
ratios = sorted(d['H/W'].unique())
palette = ['#2a0a4a', '#5c1a7a', '#8e2b90', '#bd3f8b', '#e15c74',
		   '#f6825a', '#fdae43', '#f0f921']
rows = []

for i, r in enumerate(ratios):
	s = d[d['H/W'] == r].sort_values('N')
	if len(s) < 3:
		continue
	col = palette[i % len(palette)]
	conv.add_trace(go.Scatter(
		x=s['N'], y=s['excess'],
		mode='markers+lines',
		name=f'OPT/W = {r:g}',
		marker=dict(size=7, color=col),
		line=dict(width=1.5, color=col),
		hovertemplate='N=%{x}<br>α−1=%{y:.2e}<extra></extra>'
	))
	b, a = np.polyfit(np.log(s['N']), np.log(s['excess']), 1)
	rows.append((r, b, len(s)))

# reference slopes, anchored to the data cloud
ns = np.geomspace(d['N'].min(), d['N'].max(), 50)
anchor = d.loc[d['N'].idxmin(), 'excess']
n0 = d['N'].min()
for k, dash in [(-1.0, 'dash'), (-0.5, 'dot')]:
	conv.add_trace(go.Scatter(
		x=ns, y=anchor * (ns / n0) ** k,
		mode='lines', name=f'reference N^{k:g}',
		line=dict(color='rgba(0,0,0,0.45)', width=2, dash=dash)
	))

conv.update_layout(
	title='Convergence of α toward 1 as rectangles get smaller',
	font_family="monospace",
	font_size=14,
	xaxis=dict(title='N (number of rectangles)', type='log',
			   gridcolor='rgba(0,0,0,0.08)'),
	yaxis=dict(title='α − 1', type='log', gridcolor='rgba(0,0,0,0.08)',
			   exponentformat='power'),
	plot_bgcolor='white',
	width=1400, height=900
)
conv.show()

print("decay exponent of (α−1) in N, fitted per aspect ratio")
print(f"  {'OPT/W':>8}  {'slope':>8}  {'pts':>4}")
for r, b, k in rows:
	print(f"  {r:>8g}  {b:>8.3f}  {k:>4}")
if rows:
	sl = np.array([b for _, b, _ in rows])
	print(f"  {'mean':>8}  {sl.mean():>8.3f}   sd {sl.std():.3f}")

d['hrel'] = d['HMAX'] / (d['H/W'] * d['W']) if 'W' in d.columns else np.nan
g = d.dropna(subset=['hrel'])
g = g[(g.hrel > 0) & (g.excess > 0)]
if len(g) > 3:
	b, a = np.polyfit(np.log(g.hrel), np.log(g.excess), 1)
	rng = np.random.default_rng(0)
	lx, ly = np.log(g.hrel.to_numpy()), np.log(g.excess.to_numpy())
	idx = rng.integers(0, len(lx), size=(2000, len(lx)))
	boot = np.array([np.polyfit(lx[j], ly[j], 1)[0] for j in idx])
	lo, hi = np.percentile(boot, [2.5, 97.5])

	col = go.Figure()
	col.add_trace(go.Scatter(
		x=g.hrel, y=g.excess, mode='markers',
		marker=dict(size=7, color=np.log(g['N']),
					colorscale='Plasma',
					colorbar=dict(title='log N')),
		name='all runs'
	))
	xs = np.geomspace(g.hrel.min(), g.hrel.max(), 50)
	col.add_trace(go.Scatter(
		x=xs, y=np.exp(a) * xs ** b, mode='lines',
		name=f'fit: slope = {b:.3f}  [{lo:.3f}, {hi:.3f}]',
		line=dict(color='black', width=3, dash='dash')
	))
	col.update_layout(
		title='α − 1 against h_max / OPT',
		font_family="monospace", font_size=14,
		xaxis=dict(title='h_max / OPT(I)', type='log',
					gridcolor='rgba(0,0,0,0.08)'),
		yaxis=dict(title='α − 1', type='log',
					gridcolor='rgba(0,0,0,0.08)'),
		plot_bgcolor='white', width=1400, height=900
	)
	col.show()
	print(f"\ncollapse slope: {b:.3f}  95% CI [{lo:.3f}, {hi:.3f}]")
	print("slope near 1 matches the additive-h_max form")