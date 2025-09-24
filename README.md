# KITE (K-mer Integration for Transcriptomic Exploration) ![Bash](https://img.shields.io/badge/language-Bash-yellow) ![R](https://img.shields.io/badge/language-R-blue) ![Python](https://img.shields.io/badge/language-Python-orange)

KITE is a bioinformatics pipeline for differential k-mer analysis from RNA-seq data. It provides modules for k-mer counting, filtering, machine learning methods, and a full end-to-end workflow.

[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[Kmtricks Documentation](https://github.com/tseemann/kmtricks) | [XGBoost Python](https://xgboost.readthedocs.io/en/stable/) | [R Project](https://www.r-project.org/)

---

## Table of Contents

* [Features](#features)
* [Installation](#installation)
* [Usage](#usage)

  * [Modules](#modules)
  * [Options](#options)
* [Examples](#examples)
* [Pipeline Overview](#pipeline-overview)
* [Requirements](#requirements)
* [Output](#output)
* [Notes](#notes)

---

## Features

* **kCount** – Generate k-mer occurrence matrices from RNA-seq fastq samples using `kmtricks`.
* **kFilter** – Select differential k-mers based on a threshold.
* **kML** – Train machine learning model and get the most important k-mers.
* **Full Pipeline** – Run kCount → kFilter → kML in a single command.

---

## Installation

Clone the repository:

```bash
git clone <repository_url>
cd kite
```

(Optional) Activate conda environment:

```bash
# source $(conda info --base)/etc/profile.d/conda.sh
# conda activate kite
```

Make scripts executable:

```bash
chmod +x src/*.R src/*.py src/kSelection src/comp_qvalue
```

---

## Usage

```bash
./kite.sh [module] [options]
```

### Modules

| Module     | Description                                                             |
| ---------- | ----------------------------------------------------------------------- |
| `kcount`   | Run k-mer counting. Generates a k-mer occurrence matrix.                |
| `kfilter`  | Run k-mer filtering. Selects differential k-mers based on threshold.    |
| `kml`      | Run machine learning methods and get the most important k-mers.         |
| `pipeline` | Run full pipeline: `kcount` → `kfilter` → `kml`.                        |

### Options

| Option         | Description                                                     |
| -------------- | --------------------------------------------------------------- |
| `-i`           | Input file (sample metadata or k-mer matrix).                   |
| `-p`           | Selection threshold for k-mer filtering (0.4–0.7, default=0.5). |
| `-t`           | Number of threads (default=4).                                  |
| `-d`           | Directory containing fastq samples.                             |
| `-s`           | Separator for k-mer matrix (default: space).                    |
| `-h`, `--help` | Show help message.                                              |

---

## Examples

**Run k-mer counting:**

```bash
./kite.sh kcount -i samples.txt -d fastq/ -t 8
```

**Run k-mer filtering:**

```bash
./kite.sh kfilter -i samples.txt -p 0.6 -t 4
```

**Run machine learning classification:**

```bash
./kite.sh kml -i samples.txt -t 4
```

**Run full pipeline:**

```bash
./kite.sh pipeline -i samples.txt -d fastq/ -p 0.5 -t 8
```

---

## Pipeline Overview

```
┌───────────────┐
│   Input Data  │
│ (samples.txt) │
└───────┬───────┘
        │
        ▼
┌───────────────┐
│    kCount     │
│ kmtricks →    │
│ k-mer matrix  │
└───────┬───────┘
        │
        ▼
┌───────────────┐
│   kFilter     │
│ Select k-mers │
│ threshold p   │
└───────┬───────┘
        │
        ▼
┌───────────────┐
│     kML       │
│ Train model   │
│ XGBoost + XAI │
└───────┬───────┘
        │
        ▼
┌───────────────┐
│    Output     │
│ matrix_kf.tsv │
│ qvalue.tsv    │
│ ML results    │
└───────────────┘
```

---

## Requirements

* **R** 
* **Python 3** (`xgboost`, `pandas`, `numpy`)
* **kmtricks**
* **Bash**
* Executable scripts: `kSelection`, `comp_qvalue`, `xgb.py`, `xai.py`
* `bc` (floating-point comparisons)

---

## Output

All output files are stored in the `output/` directory:

* `matrix.tsv` – k-mer count matrix
* `matrix_kf.tsv` – filtered k-mer matrix
* `qvalue.tsv` – selected k-mers with q-values
* `train_data.csv` – table to train XGB model
* `xgb.pkl` – XGB model
* `important_kmers` – importance of k-mers by SHAP


---

## Notes

* Threshold for `kfilter` must be **0.4–0.7**.
* `kml` will not run if the number of k-mers is too low (<10) or too high (>200,000). Adjust threshold accordingly.
* Ensure your input file and fastq directory are correctly formatted.

---

