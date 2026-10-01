"""Расчёты и графики для research по векторизации текста (ModelWorker, Worker №8)."""
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

plt.rcParams.update({
    "font.family": "DejaVu Sans",
    "axes.titlesize": 13,
    "axes.labelsize": 11,
    "figure.facecolor": "white",
})

PALETTE = {
    "cache":  "#2E86AB",
    "gpu":    "#6A994E",
    "cpu":    "#F18F01",
    "http":   "#C1554E",
    "cloud":  "#8D6A9F",
    "gray":   "#6c757d",
}
# Запускать из корня репозитория: python workers/model/docs/vectorization-calc.py
OUT = "workers/model/docs/img"

# ------------------------------------------------------------------
# График 1. Латентность получения эмбеддинга одного короткого текста
# ------------------------------------------------------------------
labels = [
    "Попадание в кэш\n(hashmap, in-memory)",
    "In-process ONNX,\nCPU (MiniLM)",
    "TEI на GPU (A100)\n+ HTTP localhost",
    "HTTP-воркер →\nTEI/Ollama на CPU",
    "Удалённый облачный\nAPI (сеть)",
]
# порядковые оценки, мс (из источников research)
values = [0.001, 5.0, 4.0, 15.0, 150.0]
colors = [PALETTE["cache"], PALETTE["cpu"], PALETTE["gpu"], PALETTE["http"], PALETTE["cloud"]]

fig, ax = plt.subplots(figsize=(9, 4.8))
bars = ax.barh(labels, values, color=colors)
ax.set_xscale("log")
ax.set_xlabel("Латентность, мс (логарифмическая шкала)")
ax.set_title("Латентность получения эмбеддинга одного короткого текста\n(порядковые оценки по данным research)")
for bar, v in zip(bars, values):
    txt = f"{v:g} мс" if v >= 0.01 else "~1 мкс"
    ax.text(v * 1.25, bar.get_y() + bar.get_height() / 2, txt,
            va="center", fontsize=10)
ax.set_xlim(5e-4, 1500)
ax.invert_yaxis()
ax.grid(axis="x", which="both", alpha=0.3)
fig.tight_layout()
fig.savefig(f"{OUT}/embedding-latency.png", dpi=150)
plt.close(fig)

# ------------------------------------------------------------------
# График 2. Память кэша эмбеддингов (384-мерные векторы)
# ------------------------------------------------------------------
entries = np.logspace(3, 7, 50)          # 1e3 .. 1e7 записей
dim = 384
bytes_fp32 = entries * dim * 4           # float32
bytes_int8 = entries * dim * 1           # int8-скалярное квантование
overhead = entries * 96                  # ключ-хеш (32 B) + служебные данные unordered_map (~64 B)

fig, ax = plt.subplots(figsize=(9, 4.8))
ax.plot(entries, (bytes_fp32 + overhead) / 1e9, color=PALETTE["http"], lw=2.2,
        label="float32 (1536 Б/вектор) + служебные данные")
ax.plot(entries, (bytes_int8 + overhead) / 1e9, color=PALETTE["gpu"], lw=2.2,
        label="int8-квантование (384 Б/вектор) + служебные данные")
ax.set_xscale("log")
ax.set_yscale("log")
ax.set_xlabel("Число уникальных текстов в кэше")
ax.set_ylabel("Память, ГБ")
ax.set_title("Оценка памяти hashmap-кэша эмбеддингов\n(384-мерные векторы, например all-MiniLM-L6-v2)")
ax.axhline(1.5, color=PALETTE["gray"], ls="--", lw=1)
ax.text(1.3e3, 1.7, "1 млн векторов float32 ≈ 1,5 ГБ", fontsize=9, color=PALETTE["gray"])
ax.legend(loc="upper left")
ax.grid(which="both", alpha=0.3)
fig.tight_layout()
fig.savefig(f"{OUT}/cache-memory.png", dpi=150)
plt.close(fig)

# ------------------------------------------------------------------
# График 3. Эффективная латентность в зависимости от hit rate кэша
# ------------------------------------------------------------------
hit = np.linspace(0, 1, 200)
T_miss, T_hit = 15.0, 0.001   # мс: промах (HTTP-воркер → сервер на CPU), попадание
T_eff = (1 - hit) * T_miss + hit * T_hit

fig, ax = plt.subplots(figsize=(9, 4.4))
ax.plot(hit * 100, T_eff, color=PALETTE["cache"], lw=2.4)
ax.set_yscale("log")
ax.set_xlabel("Cache hit rate, %")
ax.set_ylabel("Средняя латентность, мс (лог. шкала)")
ax.set_title("Эффективная латентность векторизации в зависимости от hit rate кэша\n"
             "(T_miss ≈ 15 мс, T_hit ≈ 1 мкс)")
for h in [0.5, 0.9, 0.99]:
    t = (1 - h) * T_miss + h * T_hit
    ax.scatter([h * 100], [t], color=PALETTE["http"], zorder=5)
    ax.annotate(f"{t:.2f} мс", (h * 100, t), textcoords="offset points",
                xytext=(8, 6), fontsize=10)
ax.grid(which="both", alpha=0.3)
fig.tight_layout()
fig.savefig(f"{OUT}/cache-hit-latency.png", dpi=150)
plt.close(fig)

# ------------------------------------------------------------------
# Числовая сводка для текста документа
# ------------------------------------------------------------------
print("=== Память кэша (384 dims) ===")
for n in [1_000, 10_000, 100_000, 1_000_000]:
    fp32 = n * dim * 4 / 1e6
    i8 = n * dim / 1e6
    print(f"{n:>9,} текстов: float32 = {fp32:8.1f} МБ | int8 = {i8:7.1f} МБ")

print("\n=== Эффективная латентность ===")
for h in [0.0, 0.5, 0.9, 0.99]:
    t = (1 - h) * T_miss + h * T_hit
    print(f"hit rate {h*100:5.1f}% -> {t:8.3f} мс (ускорение {T_miss/t:8.1f}x)")

print("\n=== Амортизация: 1 млн коротких текстов (~30 токенов) ===")
cpu_seq_s = 200          # MiniLM на 24-ядерном CPU, seq/s
gpu_seq_s = 3000         # MiniLM на RTX 2080, seq/s
for name, sps in [("CPU 24-core", cpu_seq_s), ("GPU RTX 2080", gpu_seq_s)]:
    hours = 1_000_000 / sps / 3600
    print(f"{name}: {hours:.2f} ч на полную векторизацию")
tokens = 1_000_000 * 30
cost = tokens / 1e6 * 0.02   # text-embedding-3-small, $/1M tokens
print(f"Облачный API (text-embedding-3-small, $0.02/1M tok): ~${cost:.2f} за 30 млн токенов")
