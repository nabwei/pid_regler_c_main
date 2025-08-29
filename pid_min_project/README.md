# Minimaler PID‑Regler in C (Ein‑Datei‑Version)

Kurzes, vorzeigbares Beispielprojekt für Interviews/GitHub.  
- Eine C‑Datei `pid_min.c` mit einem **diskreten PID** (Positionsform), **Sättigung** und einfachem **Anti‑Windup**.  
- Eine **Konsole‑Simulation** (Step auf Sollwert) schreibt `simulation.csv` zum Plotten.

## Build
```bash
gcc -O2 -Wall -Wextra -std=c11 pid_min.c -o pid_min -lm
```

## Run
```bash
./pid_min
# erzeugt simulation.csv
```

## Datei
- `pid_min.c` – PID‑Struct, `pid_update()`, einfache Schleife für die Simulation

## Hinweise
- Parameter: `Kp, Ki, Kd, Ts, u_min, u_max` sind oben im Code gesetzt.
- Anti‑Windup: Integrator wird proportional zur Sättigungsdifferenz korrigiert.
- Für Embedded: `pid_update()` zyklisch in einem Timer‑Interrupt aufrufen.
