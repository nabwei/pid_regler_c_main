import csv
import math
import sys
from pathlib import Path


def require(condition, message):
    if not condition:
        raise ValueError(message)


def check_step(path):
    with Path(path).open(newline="", encoding="utf-8") as stream:
        reader = csv.DictReader(stream)
        require(reader.fieldnames == ["t", "r", "y", "u", "integrator"], "Unexpected CSV columns")
        rows = [{name: float(value) for name, value in row.items()} for row in reader]
    require(len(rows) == 1201, "Expected 1201 simulation rows")
    require(all(math.isfinite(value) for row in rows for value in row.values()), "Non-finite data")
    require(all(abs(row["t"] - index * 0.01) <= 1e-5 for index, row in enumerate(rows)), "Invalid time grid")
    require(all(0 <= row["u"] <= 5 for row in rows), "Actuator limit exceeded")
    require(all(row["r"] == (0 if row["t"] < 1 else 1) for row in rows), "Incorrect reference step")
    require(all(row["y"] == 0 and row["u"] == 0 for row in rows if row["t"] < 1), "Nonzero pre-step response")
    require(rows[100]["integrator"] >= -1, "Setpoint step drove the integrator negative")
    require(max(row["y"] for row in rows) <= 1.1, "Overshoot exceeds 10 percent")
    require(all(abs(row["y"] - 1) <= 0.02 for row in rows if row["t"] >= 10), "Not within 2 percent from 10 s")
    print(f"PASS synthetic step: y(6s)={rows[600]['y']}, y(10s)={rows[1000]['y']}, y(12s)={rows[-1]['y']}")


if __name__ == "__main__":
    check_step(sys.argv[1] if len(sys.argv) > 1 else "simulation.csv")
