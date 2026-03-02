import numpy as np
import glob
import pandas as pd

def read_matrix(filename):
    """Читает квадратную матрицу из текстового файла."""
    with open(filename) as f:
        data = [float(x) for line in f for x in line.split()]
    n = int(len(data)**0.5)
    return np.array(data).reshape(n, n)

# Получаем файлы и сортируем по N (числовое значение)
result_files = glob.glob("C_*.txt")
result_files.sort(key=lambda f: int(f.split('_')[1].split('.')[0]))

if not result_files:
    print("Файлы C_*.txt не найдены. Загрузите их")
else:
    rows = []
    for c_file in result_files:
        N = int(c_file.split('_')[1].split('.')[0])
        print(f"\nОбработка N = {N} ...")

        # Читаем матрицы
        A = read_matrix(f"A_{N}.txt")
        B = read_matrix(f"B_{N}.txt")
        C_prog = read_matrix(c_file)

        # Эталонное произведение в двойной точности -> float32
        C_ref = np.dot(A.astype(np.float64), B.astype(np.float64)).astype(np.float32)

        # Анализ погрешности
        abs_diff = np.abs(C_ref - C_prog)
        max_abs = np.max(abs_diff)
        mean_abs = np.mean(abs_diff)
        rel_diff = abs_diff / (np.abs(C_ref) + 1e-10)
        max_rel = np.max(rel_diff)
        idx = np.unravel_index(np.argmax(abs_diff), abs_diff.shape)

        # Вывод подробностей
        print(f" Макс. абсолютная ошибка: {max_abs:.2e}")
        print(f" Средняя абсолютная ошибка: {mean_abs:.2e}")
        print(f" Макс. относительная ошибка: {max_rel:.2e}")
        print(f" Позиция макс. ошибки: {idx}")
        print(f"     C_prog[{idx}] = {C_prog[idx]:.10f}")
        print(f"     C_ref[{idx}]  = {C_ref[idx]:.10f}")

        # Проверка на допуск
        rtol, atol = 1e-4, 1.0
        passed = np.allclose(C_ref, C_prog, rtol=rtol, atol=atol)
        print(f"  {'Верификация ПРОЙДЕНА' if passed else 'Верификация НЕ ПРОЙДЕНА'} (rtol={rtol}, atol={atol})")

        # Сохраняем для сводной таблицы
        rows.append({
            "N": N,
            "max_abs_error": max_abs,
            "mean_abs_error": mean_abs,
            "max_rel_error": max_rel,
            "position": str(idx),
            "passed": "Yes" if passed else "No"
        })

    # Сводная таблица
    df = pd.DataFrame(rows)
    print("\n========== СВОДНАЯ ТАБЛИЦА ==========")
    print(df.to_string(index=False))

    # Сохраняем в CSV
    df.to_csv("verification_results.csv", index=False)
    print("\nРезультаты сохранены в verification_results.csv")