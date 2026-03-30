import numpy as np
import glob
import pandas as pd

def read_matrix(filename):
    with open(filename) as f:
        data = [float(x) for line in f for x in line.split()]
    n = int(len(data)**0.5)
    return np.array(data).reshape(n, n)

# ВЕРИФИКАЦИЯ
result_files = glob.glob("C_*.txt")
result_files.sort(key=lambda f: int(f.split('_')[1].split('.')[0]))

rows = []

if result_files:
    for c_file in result_files:
        N = int(c_file.split('_')[1].split('.')[0])
        print(f"\nПроверка N = {N}")

        A = read_matrix(f"A_{N}.txt")
        B = read_matrix(f"B_{N}.txt")
        C_prog = read_matrix(c_file)

        C_ref = np.dot(A.astype(np.float64), B.astype(np.float64)).astype(np.float32)

        abs_diff = np.abs(C_ref - C_prog)
        max_abs = np.max(abs_diff)
        mean_abs = np.mean(abs_diff)

        print(f"Max abs error: {max_abs:.2e}")
        print(f"Mean abs error: {mean_abs:.2e}")

        passed = np.allclose(C_ref, C_prog, rtol=1e-4, atol=1.0)
        print("OK" if passed else "FAIL")

        rows.append({
            "N": N,
            "max_abs": max_abs,
            "mean_abs": mean_abs,
            "passed": passed
        })

# АНАЛИЗ УСКОРЕНИЯ
print("\n=== Анализ ускорения ===")

timings = pd.read_csv("timings_omp.txt", sep="\t")

# Разделяем по потокам
pivot = timings.pivot(index="N", columns="Threads", values="Time(s)")

print("\nТаблица времени:")
print(pivot)

# Speedup относительно 1 потока
if 1 in pivot.columns:
    speedup = pivot[1].values.reshape(-1,1) / pivot
    speedup_df = pd.DataFrame(speedup, index=pivot.index, columns=pivot.columns)

    print("\nУскорение (Speedup):")
    print(speedup_df)

    speedup_df.to_csv("speedup.csv")

pivot.to_csv("times.csv")