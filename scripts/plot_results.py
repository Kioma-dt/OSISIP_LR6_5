"""Построение рисунков 6.1 и 6.2 по измерениям лабораторной работы."""

import csv
from pathlib import Path
from statistics import mean

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


PROJECT = Path(__file__).resolve().parents[1]


def read_experiments():
    # Один номер run соответствует одному из трёх повторов в плане.
    plan = [(2, 50), (4, 50), (6, 50), (6, 10), (6, 50), (6, 90)]
    runs = {}
    with (PROJECT / "data/results.csv").open(encoding="utf-8-sig", newline="") as file:
        for row in csv.DictReader(file):
            if row["student_id"] != "1" or row["pattern"] != "target":
                continue  # В том числе пропускает повторяющиеся заголовки CSV.
            run = int(row["run"])
            if not 1 <= run <= 18:
                continue
            root = rf"HKEY_CURRENT_USER\Software\OSiSP\Lab06\Student_1\Run_{run}"
            config = int(row["depth"]), int(row["target_pct"])
            if row["root"] == root and config == plan[(run - 1) // 3]:
                # После экспериментов в файл дописаны проверки того же Run_1.
                runs.setdefault(run, row)
    missing = sorted(set(range(1, 19)) - runs.keys())
    if missing:
        raise ValueError(f"В CSV отсутствуют плановые измерения: {missing}")
    return runs


def draw(filename, title, xlabel, ylabel, labels, values, precision, color):
    fig, ax = plt.subplots(figsize=(7.4, 4.4), constrained_layout=True)
    bars = ax.bar(labels, values, color=color, width=0.55)
    ax.set(title=title, xlabel=xlabel, ylabel=ylabel, ylim=(0, max(values) * 1.18 or 1))
    ax.set_axisbelow(True)
    ax.grid(axis="y", alpha=0.22)
    ax.spines[["top", "right"]].set_visible(False)
    numbers = [format(value, precision).replace(".", ",") for value in values]
    ax.bar_label(bars, labels=numbers, padding=4)
    path = PROJECT / "diagrams" / filename
    path.parent.mkdir(exist_ok=True)
    fig.savefig(path, dpi=240)
    plt.close(fig)


def main():
    runs = read_experiments()
    times = [mean(float(runs[i]["elapsed_ms"]) for i in range(start, start + 3))
             for start in (1, 4, 7)]
    probability_times = [mean(float(runs[i]["elapsed_ms"]) for i in range(start, start + 3))
                         for start in (10, 13, 16)]
    draw("search_time_by_depth.png", "Время поиска при вероятности совпадения 50 %",
         "Глубина обхода, уровни", "Среднее время поиска, мс",
         ["2", "4", "6"], times, ".4f", "#35648a")
    draw("matches_by_probability.png", "Время поиска при глубине обхода 6 уровней",
         "Вероятность появления образца в имени, %", "Среднее время поиска, мс",
         ["10", "50", "90"], probability_times, ".4f", "#337566")


if __name__ == "__main__":
    main()
