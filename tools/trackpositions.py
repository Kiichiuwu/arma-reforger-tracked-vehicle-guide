"""Offline generator for VehicleTrackedSimulation.TrackPositions (Leopard 2A6 example values; edit C for your tank).
License: MIT (see LICENSE in the repository root).
Points are (z, y) in hull model space (Enfusion: z forward, y up), on the convex hull of the TrackPart
circles using GROUND-FACE radii (chain points sit on the circle of each TrackPart's sim Radius).
Winding matches the engine's generator (0x140fc0520): index 0 = top, increasing index moves FORWARD on
the top run, so the link's local +Y points inward. Usage: python trackpositions.py [N] [pitch]"""
import math, sys
N = int(sys.argv[1]) if len(sys.argv) > 1 else 73
PITCH = float(sys.argv[2]) if len(sys.argv) > 2 else 0.2101
WHEELS = [2.4778, 1.6616, 0.8894, 0.1604, -0.7216, -1.5959, -2.4778]   # v_wheel_l0N, Blender y = Enfusion z
C = [(3.1278, 0.8403, 0.335),            # idler    (z, y, sim Radius)
     (-3.2968, 0.8241, 0.340)]           # sprocket (z, y, sim Radius)
C += [(z, 0.4186, 0.418) for z in WHEELS]                                # road wheels (sim Radius 0.418)
C += [(z, 1.0779, 0.152) for z in (2.1306, 1.3405, -0.4, -2.0577)]       # return rollers (Roller Radius 0.152)
# dense convex hull of circle samples, then resample by arc length
pts = []
for (z, y, r) in C:
    for k in range(3600):
        a = 2 * math.pi * k / 3600; pts.append((z + r * math.cos(a), y + r * math.sin(a)))
pts = sorted(set(pts))
def cross(o, a, b): return (a[0]-o[0])*(b[1]-o[1]) - (a[1]-o[1])*(b[0]-o[0])
lo, up = [], []
for p in pts:
    while len(lo) >= 2 and cross(lo[-2], lo[-1], p) <= 0: lo.pop()
    lo.append(p)
for p in reversed(pts):
    while len(up) >= 2 and cross(up[-2], up[-1], p) <= 0: up.pop()
    up.append(p)
hull = lo[:-1] + up[:-1]            # counter-clockwise in (z, y): bottom run goes +z ... we need the reverse
hull = hull[::-1]                   # clockwise: top run moves +z (forward)
# rotate so we start at the top point closest to z = 0
i0 = min(range(len(hull)), key=lambda i: (abs(hull[i][0]) if hull[i][1] > 1.0 else 1e9))
hull = hull[i0:] + hull[:i0]
seg = [math.dist(hull[i], hull[(i+1) % len(hull)]) for i in range(len(hull))]
L = sum(seg); step = L / N
out = []; acc = 0.0; j = 0; t = 0.0
for k in range(N):
    target = k * step
    while acc + seg[j] < target: acc += seg[j]; j += 1
    f = (target - acc) / seg[j]; a = hull[j]; b = hull[(j+1) % len(hull)]
    out.append((a[0] + f*(b[0]-a[0]), a[1] + f*(b[1]-a[1])))
chords = [math.dist(out[i], out[(i+1) % N]) for i in range(N)]
print(f"# hull length {L:.4f} m, N {N}, arc step {step:.4f}, chord min {min(chords):.4f} max {max(chords):.4f}, N*pitch {N*PITCH:.4f}")
print(f"# second point should be forward of the first: p0={out[0]} p1={out[1]}")
print("TrackPositions {")
print(" " + " ".join(f"{z:.4f} {y:.4f}" for z, y in out))
print("}")
