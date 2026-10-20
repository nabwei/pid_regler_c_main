# Diskreter PID-Regler in C

Ein kleines C11-Beispiel für einen PID-Regler mit Stellgrößenbegrenzung und
einfacher Anti-Windup-Korrektur. Das Programm simuliert eine PT1-ähnliche
Strecke und schreibt den Verlauf in `simulation.csv`.

## Implementierung

Der vollständige Quellcode liegt in
[`pid_min_project/pid_min.c`](pid_min_project/pid_min.c).
`pid_update()` berechnet aus Sollwert und Messwert:

- den proportionalen Anteil des Regelfehlers,
- den Integralanteil mit der Trapezregel,
- den Differentialanteil aus aktuellem und vorherigem Fehler,
- eine auf `u_min` und `u_max` begrenzte Stellgröße.

Die Differenz zwischen begrenzter und unbegrenzter Stellgröße wird zum
Integrator addiert. Diese einfache Anti-Windup-Korrektur verwendet einen
festen Faktor von 1.

`main()` setzt `kp = 2`, `ki = 1`, `kd = 0.1`, `ts = 0.01` Sekunden und
Stellgrößengrenzen von 0 bis 5. Der Sollwert springt bei einer Sekunde von
0 auf 1. Die simulierte Strecke hat eine Zeitkonstante von 0.5 Sekunden:

```text
a = exp(-ts / 0.5)
y_next = a * y + (1 - a) * u
```

## Reglerentwurf und MATLAB/Simulink

Dieses Repository enthält die C-Implementierung und ihre Simulation.
Der Reglerentwurf wurde ursprünglich im Rahmen eines Laborprojekts mit
MATLAB/Simulink bearbeitet.
MATLAB-Skripte, Simulink-Modelle und eine Herleitung der eingestellten
Reglerparameter sind hier nicht enthalten. Ein Abgleich der C-Ergebnisse
mit MATLAB/Simulink ist in diesem Repository nicht dokumentiert.

## Bauen und ausführen

Voraussetzung ist ein C-Compiler mit C11-Unterstützung.

### GCC unter Linux

Im Repository-Hauptverzeichnis:

```sh
cd pid_min_project
gcc -O2 -Wall -Wextra -std=c11 pid_min.c -o pid_min -lm
./pid_min
```

Dieser GCC-Befehl wurde bei der unten beschriebenen Prüfung nicht ausgeführt.

### MSVC unter Windows

In einer **x64 Native Tools Command Prompt for Visual Studio 2022**,
ausgehend vom Repository-Hauptverzeichnis:

```bat
cd pid_min_project
cl /nologo /W4 /std:c11 pid_min.c /Fe:pid_min.exe
pid_min.exe
```

Das Programm erzeugt beziehungsweise überschreibt `simulation.csv` im
aktuellen Arbeitsverzeichnis. Die Spalten sind `t,r,y,u`: Simulationszeit,
Sollwert, Streckenausgang und Stellgröße.

**Zeitzuordnung:** Jede Zeile enthält die Zeit `t` und die dafür berechnete
Stellgröße `u`, aber bereits den anschließend aktualisierten Streckenausgang
`y_next`. Beim Vergleich mit anderen Simulationen ist dieser Versatz von
einem Simulationsschritt zu beachten.

## Bisherige Prüfung

Am 20.10.2026 wurde die unveränderte C-Datei unter Windows mit dem oben
gezeigten MSVC-Befehl gebaut und ausgeführt. Der Compiler meldete dabei keine
Warnungen. Die erzeugte CSV-Datei wurde anschließend mit Python geprüft:

- Spalten `t,r,y,u` und ausschließlich endliche Zahlenwerte;
- Stellgröße durchgehend zwischen 0 und 5;
- Sollwert 0 vor einer Sekunde, anschließend 1;
- Zeitabstände von 0.01 Sekunden mit einer Prüftoleranz von 0.00001 Sekunden;
- Streckenausgang und Stellgröße vor dem Sollwertsprung gleich 0;
- positiver Streckenausgang am Ende der Simulation.

Der Lauf erzeugte 601 Datenzeilen mit einem letzten Zeitstempel von
6 Sekunden. Diese Prüfung ist ein einfacher Funktionstest. Eine
automatisierte Testsuite und CI sind bisher nicht Bestandteil des Repositorys.

## Grenzen

- Demonstration mit einer simulierten Strecke; keine Hardwarevalidierung.
- Kein Nachweis von Stabilität, optimaler Parametrierung oder Robustheit.
- Keine Filterung des Differentialanteils; Sollwertsprünge wirken auch auf ihn.
- Keine vollständige Prüfung ungültiger Parameter oder nicht endlicher Eingaben.
- `ts` muss sinnvoll und positiv gewählt werden. Der Schutz im Nenner des
  Differentialanteils ersetzt keine Parameterprüfung.
- Keine Echtzeit- oder Embedded-Integration.

## Lizenz

[MIT](LICENSE), Copyright (c) 2026 Nabwei Elfakhrany.
